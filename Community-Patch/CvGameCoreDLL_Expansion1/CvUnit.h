/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

// unit.h

#ifndef CIV5_UNIT_H
#define CIV5_UNIT_H

#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "FAutoVariable.h"
#include "FAutoVector.h"
#include "FObjectHandle.h"
#include "CvInfos.h"
#include "CvPromotionClasses.h"
#include "CvAStarNode.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvArea;
class CvAStarNode;
class CvArtInfoUnit;
class CvUnitEntry;
class CvUnitReligion;
class CvPathNode;

typedef MissionData MissionQueueNode;

typedef FFastSmallFixedList< MissionQueueNode, 12, true, c_eCiv5GameplayDLL > MissionQueue;

typedef FObjectHandle<CvUnit> UnitHandle;
typedef FStaticVector<CvPlot*, 20, true, c_eCiv5GameplayDLL, 0> UnitMovementQueue;

struct CvUnitCaptureDefinition
{
	PlayerTypes eOriginalOwner;		// Who first created the unit
	PlayerTypes eOldPlayer;			// The previous owner of the unit, no necessarily the original owner
	UnitTypes	eOldType;			// Previous type of the unit, the type can change when capturing
	PlayerTypes eCapturingPlayer;
	UnitTypes	eCaptureUnitType;
	int iX;
	int iY;
	bool bEmbarked;
	bool bAsIs;
	ReligionTypes eReligion;
	int iReligiousStrength;
	int iSpreadsLeft;
	CvUnitCaptureDefinition()
		: eOriginalOwner(NO_PLAYER)
		, eOldPlayer(NO_PLAYER)
		, eOldType(NO_UNIT)
		, eCapturingPlayer(NO_PLAYER)
		, eCaptureUnitType(NO_UNIT)
		, iX(-1)
		, iY(-1)
		, bEmbarked(false)
		, bAsIs(false)
		, eReligion(NO_RELIGION)
		, iReligiousStrength(0)
		, iSpreadsLeft(0) { }

	inline bool IsValid() const
	{
		return eCapturingPlayer != NO_PLAYER && eCaptureUnitType != NO_UNIT;
	}
};

class CvUnit
{

	friend class CvUnitMission;
	friend class CvUnitCombat;

public:

	CvUnit();
	~CvUnit();

	enum
	{
	    MOVEFLAG_ATTACK						  = 0x001,
	    MOVEFLAG_DECLARE_WAR				  = 0x002,
	    MOVEFLAG_DESTINATION				  = 0x004,
	    MOVEFLAG_NOT_ATTACKING_THIS_TURN	  = 0x008,
	    MOVEFLAG_IGNORE_STACKING			  = 0x010,
	    MOVEFLAG_PRETEND_EMBARKED			  = 0x020, // so we can check movement as if the unit was embarked
	    MOVEFLAG_PRETEND_UNEMBARKED			  = 0x040, // to check movement as if the unit was unembarked
	    MOVEFLAG_PRETEND_CORRECT_EMBARK_STATE = 0x080, // check to see if the unit can move into the tile in an embarked or unembarked state
		MOVEFLAG_STAY_ON_LAND                 = 0x100, // don't embark, even if you can
	};

	DestructionNotification<UnitHandle>& getDestructionNotification();

	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, bool bNoMove, bool bSetupGraphical=true);
	void uninit();

	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);
	void setupGraphical();

	void initPromotions();
	void uninitInfos();  // used to uninit arrays that may be reset due to mod changes

	void convert(CvUnit* pUnit);
	void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER);

	void doTurn();

	bool isActionRecommended(int iAction);

	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true) const;
	void doCommand(CommandTypes eCommand, int iData1, int iData2);

	bool IsDoingPartialMove() const;

	ActivityTypes GetActivityType() const;
	void SetActivityType(ActivityTypes eNewValue);

	AutomateTypes GetAutomateType() const;
	bool IsAutomated() const;
	void SetAutomateType(AutomateTypes eNewValue);
	bool CanAutomate(AutomateTypes eAutomate, bool bTestVisible = false) const;
	void Automate(AutomateTypes eAutomate);

	bool ReadyToSelect() const;
	bool ReadyToMove() const;
	bool ReadyToAuto() const;
	bool IsBusy() const;
	bool SentryAlert() const;

	bool ShowMoves() const;
	bool CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility = false);

	bool IsDeclareWar() const;

	RouteTypes GetBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild = NULL) const;
	void PlayActionSound();

	bool UnitAttack(int iX, int iY, int iFlags, int iSteps=0);
	bool UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove = false);
	int  UnitPathTo(int iX, int iY, int iFlags, int iPrevETA = -1, bool bBuildingRoute = false); // slewis'd the iPrevETA
	bool UnitRoadTo(int iX, int iY, int iFlags);
	bool UnitBuild(BuildTypes eBuild);
	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false, bool bIsCity = false, bool bIsDeclareWarMove = false) const;
	bool canEnterTerrain(const CvPlot& pPlot, byte bMoveFlags = 0) const;
	TeamTypes GetDeclareWarMove(const CvPlot& pPlot) const;
	PlayerTypes GetBullyMinorMove(const CvPlot* pPlot) const;
	TeamTypes GetDeclareWarRangeStrike(const CvPlot& pPlot) const;
	bool canMoveInto(const CvPlot& pPlot, byte bMoveFlags = 0) const;
	bool canMoveOrAttackInto(const CvPlot& pPlot, byte bMoveFlags = 0) const;
	bool canMoveThrough(const CvPlot& pPlot, byte bMoveFlags = 0) const;

	bool IsAngerFreeUnit() const;

	int getCombatDamage(int iStrength, int iOpponentStrength, int iCurrentDamage, bool bIncludeRand, bool bAttackerIsCity, bool bDefenderIsCity) const;
	void fightInterceptor(const CvPlot& pPlot);
	void move(CvPlot& pPlot, bool bShow);
	bool jumpToNearestValidPlot();
	bool jumpToNearestValidPlotWithinRange(int iRange);

	bool canScrap(bool bTestVisible = false) const;
	void scrap();
	int GetScrapGold() const;

	bool canGift(bool bTestVisible = false, bool bTestTransport = true) const;
	void gift(bool bTestTransport = true);

	bool CanDistanceGift(PlayerTypes eToPlayer) const;

	// Cargo/transport methods (units inside other units)
	bool canLoadUnit(const CvUnit& pUnit, const CvPlot& pPlot) const;
	void loadUnit(CvUnit& pUnit);
	bool canLoad(const CvPlot& pPlot) const;
	void load();
	bool shouldLoadOnMove(const CvPlot* pPlot) const;
	bool canUnload() const;
	void unload();
	bool canUnloadAll() const;
	void unloadAll();
	const CvUnit* getTransportUnit() const;
	CvUnit* getTransportUnit();
	bool isCargo() const;
	void setTransportUnit(CvUnit* pTransportUnit);

	SpecialUnitTypes specialCargo() const;
	DomainTypes domainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;
	//

	bool canHold(const CvPlot* pPlot) const; // skip turn
	bool canSleep(const CvPlot* pPlot) const;
	bool canFortify(const CvPlot* pPlot) const;
	bool canAirPatrol(const CvPlot* pPlot) const;

	bool IsRangeAttackIgnoreLOS() const;
	int GetRangeAttackIgnoreLOSCount() const;
	void ChangeRangeAttackIgnoreLOSCount(int iChange);

	bool canSetUpForRangedAttack(const CvPlot* pPlot) const;
	bool isSetUpForRangedAttack() const;
	void setSetUpForRangedAttack(bool bValue);

	bool IsCityAttackOnly() const;
	void ChangeCityAttackOnlyCount(int iChange);

	bool IsCaptureDefeatedEnemy() const;
	void ChangeCaptureDefeatedEnemyCount(int iChange);
	int GetCaptureChance(CvUnit *pEnemy);

	bool canEmbark(const CvPlot* pPlot) const;
	bool canDisembark(const CvPlot* pPlot) const;
	bool canEmbarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false) const;
	bool canDisembarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false) const;
	bool canDisembarkOnto(const CvPlot& pTargetPlot) const;
	bool CanEverEmbark() const;  // can this unit ever change into an embarked unit
	void embark(CvPlot* pPlot);
	void disembark(CvPlot* pPlot);
	inline bool isEmbarked() const
	{
		return m_bEmbarked;
	}

	void setEmbarked(bool bValue);

	bool IsHasEmbarkAbility() const;
	int GetEmbarkAbilityCount() const;
	void ChangeEmbarkAbilityCount(int iChange);

	bool canHeal(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot) const;
	int healTurns(const CvPlot* pPlot) const;
	void doHeal();
	void DoAttrition();

	bool canAirlift(const CvPlot* pPlot) const;
	bool canAirliftAt(const CvPlot* pPlot, int iX, int iY) const;
	bool airlift(int iX, int iY);

	bool isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const;
	bool canNuke(const CvPlot* pPlot) const;
	bool canNukeAt(const CvPlot* pPlot, int iX, int iY) const;

	bool canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const;
	bool canParadropAt(const CvPlot* pPlot, int iX, int iY) const;
	bool paradrop(int iX, int iY);

	bool canRebase(const CvPlot* pPlot) const;
	bool canRebaseAt(const CvPlot* pPlot, int iX, int iY) const;
	bool rebase(int iX, int iY);

	bool canPillage(const CvPlot* pPlot) const;
	bool pillage();

	bool canFound(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool found();

	bool canJoin(const CvPlot* pPlot, SpecialistTypes eSpecialist) const;
	bool join(SpecialistTypes eSpecialist);

	bool canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding) const;
	bool construct(BuildingTypes eBuilding);

	bool CanFoundReligion(const CvPlot* pPlot) const;
	bool DoFoundReligion();

	bool CanEnhanceReligion(const CvPlot* pPlot) const;
	bool DoEnhanceReligion();

	bool CanSpreadReligion(const CvPlot* pPlot) const;
	bool DoSpreadReligion();

	bool CanRemoveHeresy(const CvPlot* pPlot) const;
	bool DoRemoveHeresy();

	int GetNumFollowersAfterSpread() const;
	ReligionTypes GetMajorityReligionAfterSpread() const;
	CvCity *GetSpreadReligionTargetCity() const;
	int GetConversionStrength() const;

	bool canDiscover(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool discover();

	bool IsCanRushBuilding(CvCity* pCity, bool bTestVisible) const;
	bool DoRushBuilding();

	int getMaxHurryProduction(CvCity* pCity) const;
	int getHurryProduction(const CvPlot* pPlot) const;
	bool canHurry(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool hurry();

	int getTradeGold(const CvPlot* pPlot) const;
	bool canTrade(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool trade();

	bool canRepairFleet(const CvPlot *pPlot, bool bTestVisible = false) const;
	bool repairFleet();

	bool CanBuildSpaceship(const CvPlot* pPlot, bool bTestVisible) const;
	bool DoBuildSpaceship();

	bool CanCultureBomb(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool DoCultureBomb();
	void PerformCultureBomb(int iRadius);

	bool canGoldenAge(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool goldenAge();
	int GetGoldenAgeTurns() const;

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false, bool bTestGold = true) const;
	bool build(BuildTypes eBuild);

	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	void promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);

	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;

	bool isReadyForUpgrade() const;
	bool CanUpgradeRightNow(bool bOnlyTestVisible) const;
	UnitTypes GetUpgradeUnitType() const;
	int upgradePrice(UnitTypes eUnit) const;
	CvUnit* DoUpgrade();

	HandicapTypes getHandicapType() const;
	CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;
	const char* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	bool IsGreatPerson() const;
	UnitTypes getCaptureUnitType(CivilizationTypes eCivilization) const;
	UnitCombatTypes getUnitCombatType() const;
	DomainTypes getDomainType() const;

	int flavorValue(FlavorTypes eFlavor) const;

	bool isBarbarian() const;
	bool isHuman() const;

	void DoTestBarbarianThreatToMinorsWithThisUnitsDeath(PlayerTypes eKillingPlayer);
	bool IsBarbarianUnitThreateningMinor(PlayerTypes eMinor);

	int visibilityRange() const;

	int baseMoves(DomainTypes eIntoDomain = NO_DOMAIN) const;
	int maxMoves() const;
	int movesLeft() const;
	bool canMove() const;
	bool hasMoved() const;

	int GetRange() const;
	int GetNukeDamageLevel() const;

	bool canBuildRoute() const;
	BuildTypes getBuildType() const;
	int workRate(bool bMax, BuildTypes eBuild = NO_BUILD) const;

	bool isNoBadGoodies() const;
	bool isRivalTerritory() const;
	int getRivalTerritoryCount() const;
	void changeRivalTerritoryCount(int iChange);
	bool isFound() const;
	bool IsFoundAbroad() const;
	bool IsWork() const;
	bool isGoldenAge() const;
	bool canCoexistWithEnemyUnit(TeamTypes eTeam) const;

	bool isMustSetUpToRangedAttack() const;
	int getMustSetUpToRangedAttackCount() const;
	void changeMustSetUpToRangedAttackCount(int iChange);

	bool isRangedSupportFire() const;
	int getRangedSupportFireCount() const;
	void changeRangedSupportFireCount(int iChange);

	bool isFighting() const;
	bool isAttacking() const;
	bool isDefending() const;
	bool isInCombat() const;

	int GetMaxHitPoints() const;
	int GetCurrHitPoints() const;
	bool IsHurt() const;
	bool IsDead() const;

	int GetStrategicResourceCombatPenalty() const;

	void SetBaseCombatStrength(int iCombat);
	int GetBaseCombatStrength(bool bIgnoreEmbarked = false) const;
	int GetBaseCombatStrengthConsideringDamage() const;

	int GetGenericMaxStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacency) const;
	int GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender) const;
	int GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, bool bFromRangedAttack = false) const;
	int GetEmbarkedUnitDefense() const;

	bool canSiege(TeamTypes eTeam) const;

	int GetBaseRangedCombatStrength() const;
	int GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, bool bForRangedAttack) const;

	int GetAirCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage = 0) const;
	int GetRangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage = 0) const;

	bool canAirAttack() const;
	bool canAirDefend(const CvPlot* pPlot = NULL) const;

	int GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand = true) const;

	CvUnit* GetBestInterceptor(const CvPlot& pPlot, CvUnit* pkDefender = NULL, bool bLandInterceptorsOnly=false, bool bVisibleInterceptorsOnly=false) const;
	int GetInterceptorCount(const CvPlot& pPlot, CvUnit* pkDefender = NULL, bool bLandInterceptorsOnly=false, bool bVisibleInterceptorsOnly=false) const;
	int GetInterceptionDamage(const CvUnit* pAttacker, bool bIncludeRand = true) const;

	int GetCombatLimit() const;
	int GetRangedCombatLimit() const;

	bool isWaiting() const;
	bool isFortifyable(bool bCanWaitForNextTurn = false) const;
	bool IsEverFortifyable() const;
	int fortifyModifier() const;

	int experienceNeeded() const;
	int attackXPValue() const;
	int defenseXPValue() const;
	int maxXPValue() const;

	int firstStrikes() const;
	int chanceFirstStrikes() const;
	int maxFirstStrikes() const;
	bool isRanged() const;

	bool immuneToFirstStrikes() const;
	bool ignoreBuildingDefense() const;

	bool ignoreTerrainCost() const;
	int getIgnoreTerrainCostCount() const;
	void changeIgnoreTerrainCostCount(int iValue);

	bool IsRoughTerrainEndsTurn() const;
	int GetRoughTerrainEndsTurnCount() const;
	void ChangeRoughTerrainEndsTurnCount(int iValue);

	bool IsHoveringUnit() const;
	int GetHoveringUnitCount() const;
	void ChangeHoveringUnitCount(int iValue);

	bool flatMovementCost() const;
	int getFlatMovementCostCount() const;
	void changeFlatMovementCostCount(int iValue);

	bool canMoveImpassable() const;
	int getCanMoveImpassableCount() const;
	void changeCanMoveImpassableCount(int iValue);

	bool canMoveAllTerrain() const;
	void changeCanMoveAllTerrainCount(int iValue);
	int getCanMoveAllTerrainCount() const;

	bool canMoveAfterAttacking() const;
	void changeCanMoveAfterAttackingCount(int iValue);
	int getCanMoveAfterAttackingCount() const;

	bool hasFreePillageMove() const;
	void changeFreePillageMoveCount(int iValue);
	int getFreePillageMoveCount() const;

	bool hasHealOnPillage() const;
	void changeHealOnPillageCount(int iValue);
	int getHealOnPillageCount() const;

	bool isHiddenNationality() const;
	void changeHiddenNationalityCount(int iValue);
	int getHiddenNationalityCount() const;

	bool isNoRevealMap() const;
	void changeNoRevealMapCount(int iValue);
	int getNoRevealMapCount() const;

	bool isOnlyDefensive() const;
	int getOnlyDefensiveCount() const;
	void changeOnlyDefensiveCount(int iValue);

	bool noDefensiveBonus() const;
	int getNoDefensiveBonusCount() const;
	void changeNoDefensiveBonusCount(int iValue);

	bool isNoCapture() const;
	int getNoCaptureCount() const;
	void changeNoCaptureCount(int iValue);

	bool isNeverInvisible() const;
	bool isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo = true) const;

	bool isNukeImmune() const;
	void changeNukeImmuneCount(int iValue);
	int getNukeImmuneCount() const;

	int maxInterceptionProbability() const;
	int currInterceptionProbability() const;
	int evasionProbability() const;
	int withdrawalProbability() const;

	int GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL) const;
	bool IsEnemyCityAdjacent() const;
	int GetNumSpecificEnemyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL, const CvUnit* pUnitCompare = NULL) const;
	bool IsFriendlyUnitAdjacent(bool bCombatUnit) const;

	int GetAdjacentModifier() const;
	void ChangeAdjacentModifier(int iValue);
	int GetRangedAttackModifier() const;
	void ChangeRangedAttackModifier(int iValue);
	int GetInterceptionCombatModifier() const;
	void ChangeInterceptionCombatModifier(int iValue);
	int GetInterceptionDefenseDamageModifier() const;
	void ChangeInterceptionDefenseDamageModifier(int iValue);
	int GetAirSweepCombatModifier() const;
	void ChangeAirSweepCombatModifier(int iValue);
	int getAttackModifier() const;
	void changeAttackModifier(int iValue);
	int getDefenseModifier() const;
	void changeDefenseModifier(int iValue);

	int cityAttackModifier() const;
	int cityDefenseModifier() const;
	int rangedDefenseModifier() const;
	int hillsAttackModifier() const;
	int hillsDefenseModifier() const;
	int openAttackModifier() const;
	int openRangedAttackModifier() const;
	int roughAttackModifier() const;
	int roughRangedAttackModifier() const;
	int attackFortifiedModifier() const;
	int attackWoundedModifier() const;
	int openDefenseModifier() const;
	int roughDefenseModifier() const;
	int terrainAttackModifier(TerrainTypes eTerrain) const;
	int terrainDefenseModifier(TerrainTypes eTerrain) const;
	int featureAttackModifier(FeatureTypes eFeature) const;
	int featureDefenseModifier(FeatureTypes eFeature) const;
	int unitClassAttackModifier(UnitClassTypes eUnitClass) const;
	int unitClassDefenseModifier(UnitClassTypes eUnitClass) const;
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const;
	int domainModifier(DomainTypes eDomain) const;

	bool IsHasNoValidMove() const;

	inline int GetID() const
	{
		return m_iID;
	}
	int getIndex() const;
	IDInfo GetIDInfo() const;
	void SetID(int iID);

	int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);

	inline int getX() const
	{
		return m_iX.get();
	}

	inline int getY() const
	{
		return m_iY.get();
	}

	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false, bool bNoMove = false);
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot& plot) const;
	CvPlot* plot() const;
	int getArea() const;
	CvArea* area() const;
	bool onMap() const;

	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);

	int GetCycleOrder() const;
	void SetCycleOrder(int iNewValue);

	int GetDeployFromOperationTurn() const
	{
		return m_iDeployFromOperationTurn;
	};
	void SetDeployFromOperationTurn(int iTurn)
	{
		m_iDeployFromOperationTurn = iTurn;
	};

	bool IsRecon() const;
	int GetReconCount() const;
	void ChangeReconCount(int iChange);

	CvPlot* getReconPlot() const;
	void setReconPlot(CvPlot* pNewValue);

	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);

	int getDamage() const;
	void setDamage(int iNewValue, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, CvString* pAppendText = NULL);
	void changeDamage(int iChange, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, CvString* pAppendText = NULL);

	int getMoves() const;
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();

	bool IsImmobile() const;
	void SetImmobile(bool bValue);

	bool IsInFriendlyTerritory() const;
	bool IsUnderEnemyRangedAttack() const;

	int getExperience() const;
	void setExperience(int iNewValue, int iMax = -1);
	void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	int getLevel() const;
	void setLevel(int iNewValue);
	void changeLevel(int iChange);

	int getCargo() const;
	void changeCargo(int iChange);

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue, bool bAirCombat);
	bool isAirCombat() const;

	int getCombatTimer() const;
	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

	int getCombatFirstStrikes() const;
	void setCombatFirstStrikes(int iNewValue);
	void changeCombatFirstStrikes(int iChange);

	bool IsGarrisoned(void) const;
	CvCity* GetGarrisonedCity();
	int getFortifyTurns() const;
	void setFortifyTurns(int iNewValue);
	void changeFortifyTurns(int iChange);
	bool IsFortifiedThisTurn() const;
	void SetFortifiedThisTurn(bool bValue);

	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphib() const;
	void changeAmphibCount(int iChange);

	int getRiverCrossingNoPenaltyCount() const;
	bool isRiverCrossingNoPenalty() const;
	void changeRiverCrossingNoPenaltyCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const;
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const;
	void changeAlwaysHealCount(int iChange);

	int getHealOutsideFriendlyCount() const;
	bool isHealOutsideFriendly() const;
	void changeHealOutsideFriendlyCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const;
	void changeHillsDoubleMoveCount(int iChange);

	int getImmuneToFirstStrikesCount() const;
	void changeImmuneToFirstStrikesCount(int iChange);

	int getExtraVisibilityRange() const;
	void changeExtraVisibilityRange(int iChange);

	int getExtraMoves() const;
	void changeExtraMoves(int iChange);

	int getExtraMoveDiscount() const;
	void changeExtraMoveDiscount(int iChange);

	int getExtraNavalMoves() const;
	void changeExtraNavalMoves(int iChange);

	int getHPHealedIfDefeatEnemy() const;
	void changeHPHealedIfDefeatEnemy(int iValue);
	bool IsHealIfDefeatExcludeBarbarians() const;
	int GetHealIfDefeatExcludeBarbariansCount() const;
	void ChangeHealIfDefeatExcludeBarbariansCount(int iValue);

	int getExtraRange() const;
	void changeExtraRange(int iChange);

	int getExtraIntercept() const;
	void changeExtraIntercept(int iChange);

	int getExtraEvasion() const;
	void changeExtraEvasion(int iChange);

	int getExtraFirstStrikes() const;
	void changeExtraFirstStrikes(int iChange);

	int getExtraChanceFirstStrikes() const;
	void changeExtraChanceFirstStrikes(int iChange);

	int getExtraWithdrawal() const;
	void changeExtraWithdrawal(int iChange);

	int getExtraEnemyHeal() const;
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const;
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const;
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const;
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const;
	void changeAdjacentTileHeal(int iChange);

	int getEnemyDamageChance() const;
	void changeEnemyDamageChance(int iChange);

	int getNeutralDamageChance() const;
	void changeNeutralDamageChance(int iChange);

	int getEnemyDamage() const;
	void changeEnemyDamage(int iChange);

	int getNeutralDamage() const;
	void changeNeutralDamage(int iChange);

	int getNearbyEnemyCombatMod() const;
	void changeNearbyEnemyCombatMod(int iChange);

	int getNearbyEnemyCombatRange() const;
	void changeNearbyEnemyCombatRange(int iChange);

	int getExtraCombatPercent() const;
	void changeExtraCombatPercent(int iChange);

	int getExtraCityAttackPercent() const;
	void changeExtraCityAttackPercent(int iChange);

	int getExtraCityDefensePercent() const;
	void changeExtraCityDefensePercent(int iChange);

	int getExtraRangedDefenseModifier() const;
	void changeExtraRangedDefenseModifier(int iChange);

	int getExtraHillsAttackPercent() const;
	void changeExtraHillsAttackPercent(int iChange);

	int getExtraHillsDefensePercent() const;
	void changeExtraHillsDefensePercent(int iChange);

	int getExtraOpenAttackPercent() const;
	void changeExtraOpenAttackPercent(int iChange);

	int getExtraOpenRangedAttackMod() const;
	void changeExtraOpenRangedAttackMod(int iChange);

	int getExtraRoughAttackPercent() const;
	void changeExtraRoughAttackPercent(int iChange);

	int getExtraRoughRangedAttackMod() const;
	void changeExtraRoughRangedAttackMod(int iChange);

	int getExtraAttackFortifiedMod() const;
	void changeExtraAttackFortifiedMod(int iChange);

	int getExtraAttackWoundedMod() const;
	void changeExtraAttackWoundedMod(int iChange);

	int GetFlankAttackModifier() const;
	void ChangeFlankAttackModifier(int iChange);

	int getExtraOpenDefensePercent() const;
	void changeExtraOpenDefensePercent(int iChange);

	int getExtraRoughDefensePercent() const;
	void changeExtraRoughDefensePercent(int iChange);

	void changeExtraAttacks(int iChange);

	// Citadel
	bool IsNearEnemyCitadel(int& iCitadelDamage);

	// Great General Stuff
	bool IsNearGreatGeneral() const;
	bool IsStackedGreatGeneral() const;
	int GetGreatGeneralStackMovement() const;
	int GetReverseGreatGeneralModifier() const;
	int GetNearbyImprovementModifier() const;

	bool IsGreatGeneral() const;
	int GetGreatGeneralCount() const;
	void ChangeGreatGeneralCount(int iChange);
	bool IsGreatAdmiral() const;
	int GetGreatAdmiralCount() const;
	void ChangeGreatAdmiralCount(int iChange);

	int getGreatGeneralModifier() const;
	void changeGreatGeneralModifier(int iChange);

	bool IsGreatGeneralReceivesMovement() const;
	void ChangeGreatGeneralReceivesMovementCount(int iChange);
	int GetGreatGeneralCombatModifier() const;
	void ChangeGreatGeneralCombatModifier(int iChange);

	bool IsIgnoreGreatGeneralBenefit() const;
	void ChangeIgnoreGreatGeneralBenefitCount(int iChange);
	// END Great General Stuff

	int getFriendlyLandsModifier() const;
	void changeFriendlyLandsModifier(int iChange);

	int getFriendlyLandsAttackModifier() const;
	void changeFriendlyLandsAttackModifier(int iChange);

	int getOutsideFriendlyLandsModifier() const;
	void changeOutsideFriendlyLandsModifier(int iChange);

	int getPillageChange() const;
	void changePillageChange(int iChange);

	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);

	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);

	int getKamikazePercent() const;
	void changeKamikazePercent(int iChange);

	DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();

	bool isSuicide() const;

	int getDropRange() const;
	void changeDropRange(int iChange);

	bool isOutOfAttacks() const;
	void setMadeAttack(bool bNewValue);

	int GetNumInterceptions() const;
	void ChangeNumInterceptions(int iChange);

	bool isOutOfInterceptions() const;
	int getMadeInterceptionCount() const;
	void setMadeInterception(bool bNewValue);

	bool TurnProcessed() const;
	void SetTurnProcessed(bool bValue);

	bool isPromotionReady() const;
	void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath() const;
	bool isDelayedDeathExported() const;
	void startDelayedDeath();
	bool doDelayedDeath();

	bool isCombatFocus() const;

	bool isInfoBarDirty() const;
	void setInfoBarDirty(bool bNewValue);

	bool IsNotConverting() const;
	void SetNotConverting(bool bNewValue);

	inline PlayerTypes getOwner() const
	{
		return m_eOwner.get();
	}
	PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const;
	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot& pPlot) const;
	TeamTypes getTeam() const;

	PlayerTypes GetOriginalOwner() const;
	void SetOriginalOwner(PlayerTypes ePlayer);

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);
	bool IsCapturedAsIs() const;
	void SetCapturedAsIs(bool bSetValue);

	const UnitTypes getUnitType() const;
	CvUnitEntry& getUnitInfo() const;
	UnitClassTypes getUnitClassType() const;

	const UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);

	const InvisibleTypes getInvisibleType() const;
	void setInvisibleType(InvisibleTypes InvisibleType);

	const InvisibleTypes getSeeInvisibleType() const;
	void setSeeInvisibleType(InvisibleTypes InvisibleType);

	const CvUnit* getCombatUnit() const;
	CvUnit* getCombatUnit();
	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false);

	const CvCity* getCombatCity() const;
	CvCity* getCombatCity();
	void setCombatCity(CvCity* pCity);

	void clearCombat();

	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);

	const CvString getName() const;
	const char* getNameKey() const;
	const CvString getNameNoDesc() const;
	void setName(const CvString strNewValue);

	// Arbitrary Script Data
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	int getScenarioData() const;
	void setScenarioData(int iNewValue);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	bool isTerrainDoubleMove(TerrainTypes eIndex) const;
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	bool isFeatureDoubleMove(FeatureTypes eIndex) const;
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);

	int getImpassableCount() const;

	int getTerrainImpassableCount(TerrainTypes eIndex) const;
	void changeTerrainImpassableCount(TerrainTypes eIndex, int iChange);

	int getFeatureImpassableCount(FeatureTypes eIndex) const;
	void changeFeatureImpassableCount(FeatureTypes eIndex, int iChange);

	bool isTerrainImpassable(TerrainTypes eIndex) const
	{
		return m_terrainImpassableCount[eIndex] > 0;
	}

	bool isFeatureImpassable(FeatureTypes eIndex) const
	{
		return m_featureImpassableCount[eIndex] > 0;
	}

	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);

	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);

	int getUnitClassModifier(UnitClassTypes eIndex) const;
	void changeUnitClassModifier(UnitClassTypes eIndex, int iChange);

	bool canAcquirePromotion(PromotionTypes ePromotion) const;
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(PromotionTypes ePromotion) const;
	bool isHasPromotion(PromotionTypes eIndex) const;
	void setHasPromotion(PromotionTypes eIndex, bool bNewValue);

	int getSubUnitCount() const;
	int getSubUnitsAlive() const;
	int getSubUnitsAlive(int iDamage) const;

	bool isEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;
	bool isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL) const;

	bool canRangeStrike() const;
	bool canEverRangeStrikeAt(int iX, int iY) const;
	bool canRangeStrikeAt(int iX, int iY, bool bNeedWar = true, bool bNoncombatAllowed = true) const;

	bool IsAirSweepCapable() const;
	int GetAirSweepCapableCount() const;
	void ChangeAirSweepCapableCount(int iChange);

	bool canAirSweep() const;
	bool canAirSweepAt(int iX, int iY) const;
	bool airSweep(int iX, int iY);

	bool potentialWarAction(const CvPlot* pPlot) const;
	bool willRevealByMove(const CvPlot& pPlot) const;

	bool isAlwaysHostile(const CvPlot& pPlot) const;
	void changeAlwaysHostileCount(int iValue);
	int getAlwaysHostileCount() const;

	int getArmyID() const;
	void setArmyID(int iNewArmyID) ;

	bool isUnderTacticalControl() const;
	void setTacticalMove(TacticalAIMoveTypes eMove);
	TacticalAIMoveTypes getTacticalMove() const;
	bool canRecruitFromTacticalAI() const;
	void SetTacticalAIPlot(CvPlot* pPlot);
	CvPlot* GetTacticalAIPlot() const;

	void LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction);

	int GetPower() const;

	bool AreUnitsOfSameType(const CvUnit& pUnit2, const bool bPretendEmbarked = false) const;
	bool CanSwapWithUnitHere(CvPlot& pPlot) const;

	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;

	void AI_promote();
	UnitAITypes AI_getUnitAIType() const;
	void AI_setUnitAIType(UnitAITypes eNewValue);
	int AI_promotionValue(PromotionTypes ePromotion);

	GreatPeopleDirectiveTypes GetGreatPeopleDirective() const;
	void SetGreatPeopleDirective(GreatPeopleDirectiveTypes eDirective);

	bool IsSelected() const;
	bool IsFirstTimeSelected() const;
	void IncrementFirstTimeSelected();

	void SetPosition(CvPlot* pkPlot);

	const FAutoArchive& getSyncArchive() const;
	FAutoArchive& getSyncArchive();

	// Mission routines
	void PushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	void PopMission();
	void AutoMission();
	void UpdateMission();
	CvPlot* LastMissionPlot();
	bool CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false);
	int GetMissionTimer() const;
	void SetMissionTimer(int iNewValue);
	void ChangeMissionTimer(int iChange);
	void ClearMissionQueue(int iUnitCycleTimer = 1);
	int GetLengthMissionQueue() const;
	const MissionData* GetHeadMissionData();
	const MissionData* GetMissionData(int iIndex);
	CvPlot* GetMissionAIPlot();
	MissionAITypes GetMissionAIType();
	void SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit);
	CvUnit* GetMissionAIUnit();

	// Combat eligibility routines
	inline bool IsCombatUnit() const
	{
		return (m_iBaseCombat > 0);
	}

	bool IsCanAttackWithMove() const;
	bool IsCanAttackRanged() const;
	bool IsCanAttack() const;
	bool IsCanAttackWithMoveNow() const;
	bool IsCanDefend(const CvPlot* pPlot = NULL) const;
	bool IsEnemyInMovementRange(bool bOnlyFortified = false, bool bOnlyCities = false);

	// Path-finding routines
	bool GeneratePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL) const;
	void ResetPath();
	CvPlot* GetPathFirstPlot() const;
	CvPlot* GetPathLastPlot() const;
	const CvPathNodeArray& GetPathNodeArray() const;
	CvPlot* GetPathEndTurnPlot() const;

	bool isBusyMoving() const;
	void setBusyMoving(bool bState);

	bool IsIgnoringDangerWakeup() const;
	void SetIgnoreDangerWakeup(bool bState);

	bool IsNotCivilianIfEmbarked() const;
	void ChangeNotCivilianIfEmbarkedCount(int iValue);
	int GetNotCivilianIfEmbarkedCount() const;

	bool IsEmbarkAllWater() const;
	void ChangeEmbarkAllWaterCount(int iValue);
	int GetEmbarkAllWaterCount() const;

	void ChangeEmbarkExtraVisibility(int iValue);
	int GetEmbarkExtraVisibility() const;

	void ChangeEmbarkDefensiveModifier(int iValue);
	int GetEmbarkDefensiveModifier() const;

	void ChangeCapitalDefenseModifier(int iValue);
	int GetCapitalDefenseModifier() const;
	void ChangeCapitalDefenseFalloff(int iValue);
	int GetCapitalDefenseFalloff() const;

	void ChangeCityAttackPlunderModifier(int iValue);
	int GetCityAttackPlunderModifier() const;

	void ChangeReligiousStrengthLossRivalTerritory(int iValue);
	int GetReligiousStrengthLossRivalTerritory() const;

	bool IsHasBeenPromotedFromGoody() const;
	void SetBeenPromotedFromGoody(bool bBeenPromoted);

	bool IsHigherTechThan(UnitTypes otherUnit) const;
	bool IsLargerCivThan(const CvUnit* pOtherUnit) const;

	// Ported in from old CvUnitAI class
	int SearchRange(int iRange) const;
	bool PlotValid(CvPlot* pPlot) const;

	CvUnitReligion* GetReligionData() const
	{
		return m_pReligion;
	};

	static void dispatchingNetMessage(bool dispatching);
	static bool dispatchingNetMessage();

	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

protected:
	const MissionQueueNode* HeadMissionQueueNode() const;
	MissionQueueNode* HeadMissionQueueNode();

	bool	getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer = NO_PLAYER);
	static CvUnit* createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef);

	void	ClearPathCache();
	bool	UpdatePathCache(CvPlot* pDestPlot, int iFlags);

	void QueueMoveForVisualization(CvPlot* pkPlot);
	void PublishQueuedVisualizationMoves();

	typedef enum Flags
	{
	    UNITFLAG_EVALUATING_MISSION = 0x1,
	    UNITFLAG_ALREADY_GOT_GOODY_UPGRADE = 0x2
	};

	FAutoArchiveClassContainer<CvUnit> m_syncArchive;

	FAutoVariable<PlayerTypes, CvUnit> m_eOwner;
	FAutoVariable<PlayerTypes, CvUnit> m_eOriginalOwner;
	FAutoVariable<UnitTypes, CvUnit> m_eUnitType;
	FAutoVariable<int, CvUnit> m_iX;
	FAutoVariable<int, CvUnit> m_iY;
	FAutoVariable<int, CvUnit> m_iID;

	FAutoVariable<int, CvUnit> m_iHotKeyNumber;
	FAutoVariable<int, CvUnit> m_iDeployFromOperationTurn;
	int m_iLastMoveTurn;
	short m_iCycleOrder;
	FAutoVariable<int, CvUnit> m_iReconX;
	FAutoVariable<int, CvUnit> m_iReconY;
	FAutoVariable<int, CvUnit> m_iReconCount;
	FAutoVariable<int, CvUnit> m_iGameTurnCreated;
	FAutoVariable<int, CvUnit> m_iDamage;
	FAutoVariable<int, CvUnit> m_iMoves;
	FAutoVariable<bool, CvUnit> m_bImmobile;
	FAutoVariable<int, CvUnit> m_iExperience;
	FAutoVariable<int, CvUnit> m_iLevel;
	FAutoVariable<int, CvUnit> m_iCargo;
	FAutoVariable<int, CvUnit> m_iCargoCapacity;
	FAutoVariable<int, CvUnit> m_iAttackPlotX;
	FAutoVariable<int, CvUnit> m_iAttackPlotY;
	FAutoVariable<int, CvUnit> m_iCombatTimer;
	FAutoVariable<int, CvUnit> m_iCombatFirstStrikes;
	FAutoVariable<int, CvUnit> m_iCombatDamage;
	FAutoVariable<int, CvUnit> m_iFortifyTurns;
	FAutoVariable<bool, CvUnit> m_bFortifiedThisTurn;
	FAutoVariable<int, CvUnit> m_iBlitzCount;
	FAutoVariable<int, CvUnit> m_iAmphibCount;
	FAutoVariable<int, CvUnit> m_iRiverCrossingNoPenaltyCount;
	FAutoVariable<int, CvUnit> m_iEnemyRouteCount;
	FAutoVariable<int, CvUnit> m_iRivalTerritoryCount;
	FAutoVariable<int, CvUnit> m_iMustSetUpToRangedAttackCount;
	FAutoVariable<int, CvUnit> m_iRangeAttackIgnoreLOSCount;
	int m_iCityAttackOnlyCount;
	int m_iCaptureDefeatedEnemyCount;
	FAutoVariable<int, CvUnit> m_iRangedSupportFireCount;
	FAutoVariable<int, CvUnit> m_iAlwaysHealCount;
	FAutoVariable<int, CvUnit> m_iHealOutsideFriendlyCount;
	FAutoVariable<int, CvUnit> m_iHillsDoubleMoveCount;
	FAutoVariable<int, CvUnit> m_iImmuneToFirstStrikesCount;
	FAutoVariable<int, CvUnit> m_iExtraVisibilityRange;
	FAutoVariable<int, CvUnit> m_iExtraMoves;
	FAutoVariable<int, CvUnit> m_iExtraMoveDiscount;
	FAutoVariable<int, CvUnit> m_iExtraRange;
	FAutoVariable<int, CvUnit> m_iExtraIntercept;
	FAutoVariable<int, CvUnit> m_iExtraEvasion;
	FAutoVariable<int, CvUnit> m_iExtraFirstStrikes;
	FAutoVariable<int, CvUnit> m_iExtraChanceFirstStrikes;
	FAutoVariable<int, CvUnit> m_iExtraWithdrawal;
	FAutoVariable<int, CvUnit> m_iExtraEnemyHeal;
	FAutoVariable<int, CvUnit> m_iExtraNeutralHeal;
	FAutoVariable<int, CvUnit> m_iExtraFriendlyHeal;
	int m_iEnemyDamageChance;
	int m_iNeutralDamageChance;
	int m_iEnemyDamage;
	int m_iNeutralDamage;
	int m_iNearbyEnemyCombatMod;
	int m_iNearbyEnemyCombatRange;
	FAutoVariable<int, CvUnit> m_iSameTileHeal;
	FAutoVariable<int, CvUnit> m_iAdjacentTileHeal;
	FAutoVariable<int, CvUnit> m_iAdjacentModifier;
	FAutoVariable<int, CvUnit> m_iRangedAttackModifier;
	FAutoVariable<int, CvUnit> m_iInterceptionCombatModifier;
	FAutoVariable<int, CvUnit> m_iInterceptionDefenseDamageModifier;
	FAutoVariable<int, CvUnit> m_iAirSweepCombatModifier;
	FAutoVariable<int, CvUnit> m_iAttackModifier;
	FAutoVariable<int, CvUnit> m_iDefenseModifier;
	FAutoVariable<int, CvUnit> m_iExtraCombatPercent;
	FAutoVariable<int, CvUnit> m_iExtraCityAttackPercent;
	FAutoVariable<int, CvUnit> m_iExtraCityDefensePercent;
	FAutoVariable<int, CvUnit> m_iExtraRangedDefenseModifier;
	FAutoVariable<int, CvUnit> m_iExtraHillsAttackPercent;
	FAutoVariable<int, CvUnit> m_iExtraHillsDefensePercent;
	FAutoVariable<int, CvUnit> m_iExtraOpenAttackPercent;
	FAutoVariable<int, CvUnit> m_iExtraOpenRangedAttackMod;
	FAutoVariable<int, CvUnit> m_iExtraRoughAttackPercent;
	FAutoVariable<int, CvUnit> m_iExtraRoughRangedAttackMod;
	FAutoVariable<int, CvUnit> m_iExtraAttackFortifiedMod;
	FAutoVariable<int, CvUnit> m_iExtraAttackWoundedMod;
	int m_iFlankAttackModifier;
	FAutoVariable<int, CvUnit> m_iExtraOpenDefensePercent;
	FAutoVariable<int, CvUnit> m_iExtraRoughDefensePercent;
	FAutoVariable<int, CvUnit> m_iPillageChange;
	FAutoVariable<int, CvUnit> m_iUpgradeDiscount;
	FAutoVariable<int, CvUnit> m_iExperiencePercent;
	FAutoVariable<int, CvUnit> m_iDropRange;
	FAutoVariable<int, CvUnit> m_iAirSweepCapableCount;
	FAutoVariable<int, CvUnit> m_iExtraNavalMoves;
	FAutoVariable<int, CvUnit> m_iKamikazePercent;
	FAutoVariable<int, CvUnit> m_iBaseCombat;
	FAutoVariable<DirectionTypes, CvUnit> m_eFacingDirection;
	FAutoVariable<int, CvUnit> m_iArmyId;

	FAutoVariable<int, CvUnit> m_iIgnoreTerrainCostCount;
	FAutoVariable<int, CvUnit> m_iRoughTerrainEndsTurnCount;
	FAutoVariable<int, CvUnit> m_iEmbarkAbilityCount;
	FAutoVariable<int, CvUnit> m_iHoveringUnitCount;
	FAutoVariable<int, CvUnit> m_iFlatMovementCostCount;
	FAutoVariable<int, CvUnit> m_iCanMoveImpassableCount;
	FAutoVariable<int, CvUnit> m_iOnlyDefensiveCount;
	FAutoVariable<int, CvUnit> m_iNoDefensiveBonusCount;
	FAutoVariable<int, CvUnit> m_iNoCaptureCount;
	FAutoVariable<int, CvUnit> m_iNukeImmuneCount;
	FAutoVariable<int, CvUnit> m_iHiddenNationalityCount;
	FAutoVariable<int, CvUnit> m_iAlwaysHostileCount;
	FAutoVariable<int, CvUnit> m_iNoRevealMapCount;
	FAutoVariable<int, CvUnit> m_iCanMoveAllTerrainCount;
	FAutoVariable<int, CvUnit> m_iCanMoveAfterAttackingCount;
	FAutoVariable<int, CvUnit> m_iFreePillageMoveCount;
	int m_iHealOnPillageCount;
	FAutoVariable<int, CvUnit> m_iHPHealedIfDefeatEnemy;
	FAutoVariable<int, CvUnit> m_iTacticalAIPlotX;
	FAutoVariable<int, CvUnit> m_iTacticalAIPlotY;
	FAutoVariable<int, CvUnit> m_iGarrisonCityID;   // unused
	FAutoVariable<int, CvUnit> m_iFlags;
	FAutoVariable<int, CvUnit> m_iNumAttacks;
	FAutoVariable<int, CvUnit> m_iAttacksMade;
	FAutoVariable<int, CvUnit> m_iGreatGeneralCount;
	int m_iGreatAdmiralCount;
	FAutoVariable<int, CvUnit> m_iGreatGeneralModifier;
	int m_iGreatGeneralReceivesMovementCount;
	int m_iGreatGeneralCombatModifier;
	int m_iIgnoreGreatGeneralBenefit;
	FAutoVariable<int, CvUnit> m_iFriendlyLandsModifier;
	FAutoVariable<int, CvUnit> m_iFriendlyLandsAttackModifier;
	FAutoVariable<int, CvUnit> m_iOutsideFriendlyLandsModifier;
	FAutoVariable<int, CvUnit> m_iHealIfDefeatExcludeBarbariansCount;
	FAutoVariable<int, CvUnit> m_iNumInterceptions;
	FAutoVariable<int, CvUnit> m_iMadeInterceptionCount;
	int m_iEverSelectedCount;

	FAutoVariable<bool, CvUnit> m_bPromotionReady;
	FAutoVariable<bool, CvUnit> m_bDeathDelay;
	FAutoVariable<bool, CvUnit> m_bCombatFocus;
	FAutoVariable<bool, CvUnit> m_bInfoBarDirty;
	FAutoVariable<bool, CvUnit> m_bNotConverting;
	FAutoVariable<bool, CvUnit> m_bAirCombat;
	FAutoVariable<bool, CvUnit> m_bSetUpForRangedAttack;
	FAutoVariable<bool, CvUnit> m_bEmbarked;
	FAutoVariable<bool, CvUnit> m_bAITurnProcessed;

	FAutoVariable<TacticalAIMoveTypes, CvUnit> m_eTacticalMove;
	FAutoVariable<PlayerTypes, CvUnit> m_eCapturingPlayer;
	bool m_bCapturedAsIs;
	FAutoVariable<UnitTypes, CvUnit> m_eLeaderUnitType;
	FAutoVariable<InvisibleTypes, CvUnit> m_eInvisibleType;
	FAutoVariable<InvisibleTypes, CvUnit> m_eSeeInvisibleType;
	FAutoVariable<GreatPeopleDirectiveTypes, CvUnit> m_eGreatPeopleDirectiveType;
	CvUnitEntry* m_pUnitInfo;

	bool m_bWaitingForMove;			///< If true, the unit is busy visualizing its move.

	IDInfo m_combatUnit;
	IDInfo m_combatCity;
	IDInfo m_transportUnit;

	std::vector<int> m_extraDomainModifiers;

	FAutoVariable<CvString, CvUnit> m_strNameIAmNotSupposedToBeUsedAnyMoreBecauseThisShouldNotBeCheckedAndWeNeedToPreserveSaveGameCompatibility;
	FAutoVariable<CvString, CvUnit> m_strScriptData;
	int m_iScenarioData;

	CvUnitPromotions  m_Promotions;
	CvUnitReligion* m_pReligion;

	FAutoVariable<std::vector<int>, CvUnit> m_terrainDoubleMoveCount;
	FAutoVariable<std::vector<int>, CvUnit> m_featureDoubleMoveCount;
	FAutoVariable<std::vector<int>, CvUnit> m_terrainImpassableCount;
	FAutoVariable<std::vector<int>, CvUnit> m_featureImpassableCount;
	FAutoVariable<std::vector<int>, CvUnit> m_extraTerrainAttackPercent;
	FAutoVariable<std::vector<int>, CvUnit> m_extraTerrainDefensePercent;
	FAutoVariable<std::vector<int>, CvUnit> m_extraFeatureAttackPercent;
	FAutoVariable<std::vector<int>, CvUnit> m_extraFeatureDefensePercent;
	FAutoVariable<std::vector<int>, CvUnit> m_extraUnitCombatModifier;
	FAutoVariable<std::vector<int>, CvUnit> m_unitClassModifier;
	int m_iMissionTimer;
	FAutoVariable<int, CvUnit> m_iMissionAIX;
	FAutoVariable<int, CvUnit> m_iMissionAIY;
	FAutoVariable<MissionAITypes, CvUnit> m_eMissionAIType;
	IDInfo m_missionAIUnit;
	FAutoVariable<ActivityTypes, CvUnit> m_eActivityType;
	FAutoVariable<AutomateTypes, CvUnit> m_eAutomateType;
	FAutoVariable<UnitAITypes, CvUnit> m_eUnitAIType;
	DestructionNotification<UnitHandle> m_destructionNotification;

	UnitHandle  m_thisHandle;

	UnitMovementQueue m_unitMoveLocs;

	bool m_bIgnoreDangerWakeup; // slewis - make this an autovariable when saved games are broken
	int m_iEmbarkedAllWaterCount;
	int m_iEmbarkExtraVisibility;
	int m_iEmbarkDefensiveModifier;
	int m_iCapitalDefenseModifier;
	int m_iCapitalDefenseFalloff;
	int m_iCityAttackPlunderModifier;
	int m_iReligiousStrengthLossRivalTerritory;

	CvString m_strName;

	mutable CvPathNodeArray m_kLastPath;
	mutable uint m_uiLastPathCacheDest;

	bool canAdvance(const CvPlot& pPlot, int iThreshold) const;

	CvUnit* airStrikeTarget(CvPlot& pPlot, bool bNoncombatAllowed) const;

	bool CanWithdrawFromMelee(CvUnit& pAttacker);
	bool DoWithdrawFromMelee(CvUnit& pAttacker);

private:

	mutable MissionQueue m_missionQueue;
};

FDataStream& operator<<(FDataStream&, const CvUnit&);
FDataStream& operator>>(FDataStream&, CvUnit&);

namespace FSerialization
{
void SyncUnits();
void ClearUnitDeltas();
}

#endif
