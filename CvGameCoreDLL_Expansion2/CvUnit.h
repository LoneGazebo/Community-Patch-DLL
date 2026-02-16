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
#include "CvInfos.h"
#include "CvPromotionClasses.h"
#include "CvAStarNode.h"
#include "CvSerialize.h"

#define DEFAULT_UNIT_MAP_LAYER 0
#define TRADE_UNIT_MAP_LAYER 1

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvUnitEntry;
class CvUnitReligion;

class CvTacticalMove;

typedef std::vector<int> UnitIdContainer; //use a vector as most of the time this will be empty
typedef std::vector<std::pair<TerrainTypes, int>> TerrainTypeCounter;
typedef std::vector<std::pair<FeatureTypes, int>> FeatureTypeCounter;
typedef std::vector<std::pair<UnitClassTypes, int>> UnitClassCounter;
typedef FFastSmallFixedList< MissionData, 12, true, c_eCiv5GameplayDLL > MissionQueue;

struct CvUnitCaptureDefinition
{
	PlayerTypes eOriginalOwner;		// Who first created the unit
	PlayerTypes eOldPlayer;			// The previous owner of the unit, no necessarily the original owner
	UnitTypes	eOldType;			// Previous type of the unit, the type can change when capturing
	PlayerTypes eCapturingPlayer;
	UnitTypes	eCaptureUnitType;
	int iUnitID;
	int iX;
	int iY;
	bool bEmbarked;
	bool bAsIs;
	bool bConscript;
	int iScenarioData;
	ReligionTypes eReligion;
	int iReligiousStrength;
	int iSpreadsUsed;

	CvUnitCaptureDefinition()
		: eOriginalOwner(NO_PLAYER)
		, eOldPlayer(NO_PLAYER)
		, eOldType(NO_UNIT)
		, eCapturingPlayer(NO_PLAYER)
		, eCaptureUnitType(NO_UNIT)
		, iUnitID(-1)
		, iX(-1)
		, iY(-1)
		, bEmbarked(false)
		, bAsIs(false)
		, bConscript(false)
		, iScenarioData(0)
		, eReligion(NO_RELIGION)
		, iReligiousStrength(0)
		, iSpreadsUsed(0) { }

	inline bool IsValid() const
	{
		return eCapturingPlayer != NO_PLAYER && eCaptureUnitType != NO_UNIT;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvUnitReligion
//!  \brief		Information about the religious affiliation of a single unit
//
//!  Key Attributes:
//!  - One instance for each unit
//!  - Accessed by any class that needs to check religious information for this unit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitReligion
{
public:
	CvUnitReligion(void);
	void Init();

	template<typename UnitReligion, typename Visitor>
	static void Serialize(UnitReligion& unitReligion, Visitor& visitor);

	// Accessors
	ReligionTypes GetReligion() const
	{
		return m_eReligion;
	};
	void SetReligion(ReligionTypes eReligion)
	{
		m_eReligion = eReligion;
	};
	int GetReligiousStrength() const
	{
		return m_iStrength;
	};
	int GetMaxSpreads(const CvUnit* pUnit) const;
	int GetSpreadsLeft(const CvUnit* pUnit) const
	{
		return GetMaxSpreads(pUnit) - m_iSpreadsUsed;
	};
	int GetSpreadsUsed() const
	{
		return m_iSpreadsUsed;
	};
	void IncrementSpreadsUsed()
	{
		m_iSpreadsUsed++;
	};
	void SetSpreadsUsed(int iValue)
	{
		m_iSpreadsUsed = iValue;
	};
	void SetReligiousStrength(int iValue)
	{
		m_iStrength = iValue;
	};
	void SetFullStrength(PlayerTypes eOwner, const CvUnitEntry& kUnitInfo, ReligionTypes eReligion);
	bool IsFullStrength() const;

private:
	ReligionTypes m_eReligion;
	unsigned short m_iStrength;
	unsigned short m_iSpreadsUsed;
	unsigned short m_iMaxStrength;

	friend FDataStream& operator>>(FDataStream&, CvUnitReligion&);
	friend FDataStream& operator<<(FDataStream&, const CvUnitReligion&);
};

FDataStream& operator>>(FDataStream&, CvUnitReligion&);
FDataStream& operator<<(FDataStream&, const CvUnitReligion&);

//we calculate a unit's strength modifier very often, so we cache the most recent results
struct SStrengthModifierInput
{
	SStrengthModifierInput(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bIgnoreUnitAdjacencyBoni, const CvPlot* pFromPlot, bool bQuickAndDirty, int iOurDamage);
	const bool operator==(const SStrengthModifierInput& rhs) const;

	int m_iOtherUnitID;
	int m_iBattlePlot;
	int m_iFromPlot;
	bool m_bIgnoreUnitAdjacencyBoni;
	bool m_bQuickAndDirty;
	int m_iOurDamage;
	int m_iTheirDamage;
};

enum AreaEffectType
{
	AE_GREAT_GENERAL,
	AE_SAPPER,
	AE_SIEGETOWER
};

enum SquadsEndMovementType
{
	ALERT_ON_ARRIVAL = 0,
	WAKE_ON_EACH_UNIT_ARRIVED = 1,
	WAKE_ON_ALL_ARRIVED = 2
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
		//these values can be called via the dll external interface, don't modify them
	    MOVEFLAG_ATTACK							= 0x0001, // flag for CvUnit missions to allow attacks (melee combat or ranged capturing civilian). pathfinder handles this automatically
	    MOVEFLAG_UNUSED1						= 0x0002, // 
	    MOVEFLAG_DESTINATION					= 0x0004, // we want to end the turn in the given plot. only relevant for canMoveInto(), pathfinder handles it automatically
	    MOVEFLAG_UNUSED2						= 0x0008, // 
	    MOVEFLAG_IGNORE_STACKING_SELF			= 0x0010, // stacking rules (with owned units) don't apply (on turn end plots)
	    MOVEFLAG_UNUSED3						= 0x0020, //
	    MOVEFLAG_UNUSED4						= 0x0040, // 
		//these values are used internally only
		MOVEFLAG_SAFE_EMBARK_ONLY				= 0x0080, //allow embarkation only if danger is zero on turn end plots
		MOVEFLAG_IGNORE_DANGER					= 0x0100, //do not apply a penalty for dangerous plots
		MOVEFLAG_NO_EMBARK						= 0x0200, //do not ever embark (but move along if already embarked)
		MOVEFLAG_NO_ENEMY_TERRITORY				= 0x0400, //don't enter enemy territory, even if we could (but can still pass through enemy *zones*!) 
		MOVEFLAG_MAXIMIZE_EXPLORE				= 0x0800, //try to reveal as many plots as possible
		MOVEFLAG_NO_DEFENSIVE_SUPPORT			= 0x1000, //without this set in a melee attack, the defender can receive support from adjacent ranged units (unless disabled globally)
		MOVEFLAG_NO_OCEAN						= 0x2000, //don't use deep water even if we could
		MOVEFLAG_DONT_STACK_WITH_NEUTRAL		= 0x4000, //for civilian with escort
		MOVEFLAG_APPROX_TARGET_RING1			= 0x8000, //don't need to reach the target exactly, a ring1 tile is good enough
		MOVEFLAG_APPROX_TARGET_RING2			= 0x10000, //don't need to reach the target exactly, a ring2 tile is good enough
		MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN	= 0x20000, //no embarkation on approximate target tile
		MOVEFLAG_IGNORE_ZOC						= 0x40000, //ignore zones of control
		MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE		= 0x80000, //pretend we can enter everybody's territory
		MOVEFLAG_SELECTIVE_ZOC					= 0x100000, //ignore ZOC from enemy units on given plots
		MOVEFLAG_PRETEND_ALL_REVEALED			= 0x200000, //pretend all plots are revealed, ie territory is known. leaks information, only for AI to recognize dead ends
		MOVEFLAG_AI_ABORT_IN_DANGER				= 0x400000, //abort movement if about to end turn on a dangerous plot (should always check if move was executed afterwards)
															//IMPORTANT: MOVEFLAG_AI_ABORT_IN_DANGER does not prevent path *planning* for combat units! too hard to define a threshold
		MOVEFLAG_NO_STOPNODES					= 0x800000, //if we already know we can reach the target plot, don't bother with stop nodes
		MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED	= 0x1000000, //abort if additional enemies become visible, irrespective of danger level
		MOVEFLAG_IGNORE_ENEMIES					= 0x2000000, //similar to IGNORE_STACKING but pretend we can pass through enemies
		MOVEFLAG_TURN_END_IS_NEXT_TURN			= 0x4000000, //consider when a unit may take action again, ie if the target plot has zero moves left, add one to the turn count
		MOVEFLAG_VISIBLE_ONLY					= 0x8000000, //workers typically should not go exploring
		MOVEFLAG_PRETEND_CANALS					= 0x10000000, //pretend ships can move one tile inland to see if a canal would make sense
	    MOVEFLAG_IGNORE_STACKING_NEUTRAL		= 0x20000000, // stacking rules (with neutral units) don't apply (on turn end plots)
		MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT		= 0x40000000, //if the target plot is occupied go to the closest available plot instead
		MOVEFLAG_KEEP_LINK                      = 0x80000000, //if flag is present, will not break unit link upon issuance of a new move mission

		//seems we are running out of bits, be careful when adding new flags ... maybe we can finally recycle the unused ones above?

		//some flags are relevant during path planning, some only during execution
		PATHFINDER_FLAG_MASK					= ~(MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED|MOVEFLAG_TURN_END_IS_NEXT_TURN|MOVEFLAG_NO_DEFENSIVE_SUPPORT),
	};

	enum MoveResult
	{
		//values >= 0 are valid
		MOVE_RESULT_CANCEL		= 0xFFFFFFFE,		//cannot continue mission
		MOVE_RESULT_NEXT_TURN	= 0xFFFFFFFD,		//continue next turn
		MOVE_RESULT_ATTACK		= 0xFFFFFFFC,		//move resulted in an attack
		MOVE_RESULT_NO_TARGET	= 0xFFFFFFFB,		//attack not required
	};

	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, CvUnit* pPassUnit = NULL);
	void initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical = true, int iMapLayer = DEFAULT_UNIT_MAP_LAYER, int iNumGoodyHutsPopped = 0, ContractTypes eContract = NO_CONTRACT, bool bHistoric = true, bool bSkipNaming = false, CvUnit* pPassUnit = NULL);

	void uninit();

	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);
	void setupGraphical();

	void uninitInfos();  // used to uninit arrays that may be reset due to mod changes

	static bool IsRetainablePromotion(PromotionTypes ePromotion);
	static int CalcExperienceTimes100ForConvert(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iExperienceTimes100);
	void grantExperienceFromLostPromotions(int iNumLost);

	void convert(CvUnit* pUnit, bool bIsUpgrade);
	void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER);

	void doTurn();
	bool isActionRecommended(int iAction);

	void DoLocationPromotions(bool bSpawn, CvPlot* pOldPlot = NULL, CvPlot* pNewPlot = NULL);

	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true) const;
	void doCommand(CommandTypes eCommand, int iData1, int iData2);

	bool IsDoingPartialMove() const;

	ActivityTypes GetActivityType() const;
	void SetActivityType(ActivityTypes eNewValue);

	AutomateTypes GetAutomateType() const;
	bool IsAutomated() const;
	void SetAutomateType(AutomateTypes eNewValue);
	bool CanAutomate(AutomateTypes eAutomate, bool bTestVisibility = false) const;
	void Automate(AutomateTypes eAutomate);

	bool ReadyToSelect() const;
	bool ReadyToMove() const;
	bool ReadyToSwap() const;
	bool ReadyToAuto() const;
	bool IsBusy() const;
	bool SentryAlert(bool bAllowAttacks) const;

	bool CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility = false);

	RouteTypes GetBestBuildRouteForRoadTo(CvPlot* pPlot, BuildTypes* peBestBuild = NULL) const;
	void PlayActionSound();

	TeamTypes GetDeclareWarMove(const CvPlot& pPlot) const;
	TeamTypes GetDeclareWarRangeStrike(const CvPlot& pPlot) const;

	bool canEnterTerritory(TeamTypes eTeam, bool bEndTurn = true) const;
	bool canEnterTerrain(const CvPlot& enterPlot, int iMoveFlags = 0) const;
	bool canMoveInto(const CvPlot& pPlot, int iMoveFlags = 0) const;
	bool canMoveOrAttackInto(const CvPlot& pPlot, int iMoveFlags = 0) const;

	bool CanStayInOcean() const;

	bool IsAngerFreeUnit() const;

	int getMeleeCombatDamageCity(int iStrength, const CvCity* pCity, int& iSelfDamageInflicted, int iGarrisonMaxHP, int& iGarrisonDamage, bool bIncludeRand) const;
	int getMeleeCombatDamage(int iStrength, int iOpponentStrength, int& iSelfDamageInflicted, bool bIncludeRand, const CvUnit* pkOtherUnit, int iExtraDefenderDamage = 0) const;
	void move(CvPlot& targetPlot, bool bShow, bool bNoMovementCost = false);
	bool jumpToNearestValidPlot();
	bool jumpToNearestValidPlotWithinRange(int iRange, CvPlot* pStartPlot=NULL);

	bool canScrap(bool bTestVisible = false) const;
	void scrap(bool bDelay = true);
	int GetScrapGold() const;

	bool canGift(bool bTestVisible = false, bool bTestTransport = true) const;
	void gift(bool bTestTransport = true);

	bool CanDistanceGift(PlayerTypes eToPlayer) const;

	// Cargo/transport methods (units inside other units)
	bool canLoadUnit(const CvUnit& pUnit, const CvPlot& targetPlot) const;
	void loadUnit(CvUnit& pUnit);
	bool canLoad(const CvPlot& targetPlot) const;
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
#if defined(MOD_CARGO_SHIPS)
	SpecialUnitTypes specialUnitCargoLoad() const;
#endif
	SpecialUnitTypes specialCargo() const;
	DomainTypes domainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool CanHaveCargo(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;

	bool canHold(const CvPlot* pPlot) const; // skip turn
	bool canSleep(const CvPlot* pPlot) const;
	bool canFortify(const CvPlot* pPlot) const;
	bool canAirPatrol(const CvPlot* pPlot) const;

	bool IsRangeAttackIgnoreLOS() const;
	int GetRangeAttackIgnoreLOSCount() const;
	void ChangeRangeAttackIgnoreLOSCount(int iChange);
    
	int GetSeeThrough() const;
    void ChangeSeeThrough(int iChange);

	bool canSetUpForRangedAttack(const CvPlot* pPlot) const; //no longer used
	bool isSetUpForRangedAttack() const; //no longer used
	void setSetUpForRangedAttack(bool bValue); //no longer used

	bool IsCityAttackSupport() const;
	void ChangeCityAttackOnlyCount(int iChange);

	bool IsCaptureDefeatedEnemy() const;
	void ChangeCaptureDefeatedEnemyCount(int iChange);
	int GetCaptureChance(CvUnit *pEnemy);

	void ChangeCannotBeCapturedCount(int iChange);
	bool GetCannotBeCaptured();
	int getForcedDamageValue() const;
	void ChangeForcedDamageValue(int iChange);

	int getChangeDamageValue() const;
	void ChangeChangeDamageValue(int iChange);
	
	int GetDamageTakenMod() const;
	void ChangeDamageTakenMod(int iChange);

	int GetInfluenceFromCombatXPTimes100() const;
	void ChangeInfluenceFromCombatXPTimes100(int iChange);

	int getPromotionDuration(PromotionTypes eIndex) const;
	void SetPromotionDuration(PromotionTypes eIndex, int iValue);
	void ChangePromotionDuration(PromotionTypes eIndex, int iChange);

	int getTurnPromotionGained(PromotionTypes eIndex) const;
	void SetTurnPromotionGained(PromotionTypes eIndex, int iValue);

	int getNegatorPromotion();
	void SetNegatorPromotion(int iValue);

	bool canEmbarkAtPlot(const CvPlot* pPlot) const;
	bool canDisembarkAtPlot(const CvPlot* pPlot) const;
	bool canEmbarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false, int iMoveFlags = 0) const;
	bool canDisembarkOnto(const CvPlot& pOriginPlot, const CvPlot& pTargetPlot, bool bOverrideEmbarkedCheck = false, int iMoveFlags = 0) const;
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

	bool canHeal(const CvPlot* pPlot, bool bCheckMovement = true) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot) const;
	int healTurns(const CvPlot* pPlot) const;
	void doHeal();
	void DoAttrition();
	int GetDanger(const CvPlot* pAtPlot=NULL) const;
	int GetDanger(const CvPlot* pAtPlot, const UnitIdContainer& unitsToIgnore, int iExtraDamage) const;

	const CvPlot* getAirliftFromPlot(const CvPlot* pPlot) const;
	const CvPlot* getAirliftToPlot(const CvPlot* pPlot, bool bIncludeCities) const;

	bool canAirlift(const CvPlot* pPlot) const;
	bool canAirliftAt(const CvPlot* pPlot, int iX, int iY) const;
	bool airlift(int iX, int iY);

	bool isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const;
	bool canNuke() const;
	bool canNukeAt(const CvPlot* pPlot, int iX, int iY) const;

	bool canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const;
	bool canParadropAt(const CvPlot* pPlot, int iX, int iY) const;
	bool paradrop(int iX, int iY, bool& bAnimationShown);

	bool canMakeTradeRoute(const CvPlot* pPlot) const;
	bool canMakeTradeRouteAt(const CvPlot* pPlot, int iX, int iY, TradeConnectionType eConnectionType) const;
	bool makeTradeRoute(int iX, int iY, TradeConnectionType eConnectionType);

	bool canChangeTradeUnitHomeCity(const CvPlot* pPlot) const;
	bool canChangeTradeUnitHomeCityAt(const CvPlot* pPlot, int iX, int iY) const;
	bool changeTradeUnitHomeCity(int iX, int iY);

	bool canChangeAdmiralPort(const CvPlot* pPlot) const;
	bool canChangeAdmiralPortAt(const CvPlot* pPlot, int iX, int iY) const;
	bool changeAdmiralPort(int iX, int iY);

	bool canPlunderTradeRoute(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	bool plunderTradeRoute();

	bool canCreateGreatWork(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	bool createGreatWork();

	bool canGetFreeLuxury() const;
	bool createFreeLuxury();
	int CreateFreeLuxuryCheckCopy();
	int CreateFreeLuxuryCheck();

	int getNumExoticGoods() const;
	void setNumExoticGoods(int iValue);
	void changeNumExoticGoods(int iChange);
	float calculateExoticGoodsDistanceFactor(const CvPlot* pPlot) const;
	bool canSellExoticGoods(const CvPlot* pPlot, bool bOnlyTestVisibility = false) const;
	int getExoticGoodsGoldAmount();
	int getExoticGoodsXPAmount();
	bool sellExoticGoods();

	int getRebaseRange() const;
	bool canRebase(bool bIgnoreMovementPoints = false) const;
	bool canRebaseAt(int iXDest, int iYDest, bool bIgnoreMovementPoints = false) const;
	bool rebase(int iX, int iY, bool bForced = false);

	bool canPillage(const CvPlot* pPlot) const;
	int getPillageHealAmount(const CvPlot* pPlot, bool bPotential = false) const;
	bool shouldPillage(const CvPlot* pPlot, bool bConservative = false, bool bIgnoreMovement = false) const;
	bool pillage();

	bool canFoundCity(const CvPlot* pPlot, bool bIgnoreDistanceToExistingCities = false, bool bIgnoreHappiness = false, bool bForAliveCheck = false) const;
	bool foundCity();

	bool canJoinCity(const CvPlot* pPlot, SpecialistTypes eSpecialist) const;
	bool joinCity(SpecialistTypes eSpecialist);

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

	int GetNumFollowersAfterInquisitor() const;
	ReligionTypes GetMajorityReligionAfterInquisitor() const;
	int GetNumFollowersAfterSpread() const;
	ReligionTypes GetMajorityReligionAfterSpread() const;
	CvCity *GetSpreadReligionTargetCity() const;
	int GetConversionStrength(const CvCity* pCity) const;

	bool greatperson();
	int GetScaleAmount(int iAmountToScale) const;

	bool canDiscover(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool discover();

	bool canHurry(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getHurryProduction(const CvPlot* pPlot) const;
	bool hurry();

	bool canTrade(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getTradeInfluence(const CvPlot* pPlot) const;
	bool trade();

	bool canBuyCityState(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool buyCityState();

	bool canRepairFleet(const CvPlot *pPlot, bool bTestVisible = false) const;
	bool repairFleet();

	bool CanBuildSpaceship(const CvPlot* pPlot, bool bVisible) const;
	bool DoBuildSpaceship();

	bool CanCultureBomb(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool isCultureBomb() const;
	bool DoCultureBomb();
	void PerformCultureBomb(int iRadius);
	int getNumberOfCultureBombs() const;
	void setNumberOfCultureBombs(const int iBombs);

	bool canGoldenAge(const CvPlot* pPlot, bool bTestVisible = false) const;
	int getGoldenAgeTurns() const;
	bool goldenAge();

	bool canGivePolicies(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool givePolicies();

	bool canBlastTourism(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool blastTourism();

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false, bool bTestGold = true, bool bTestEra = false) const;
	bool build(BuildTypes eBuild);

	int getBuilderStrength() const;
	void setBuilderStrength(const int newPower);

	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	void promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);

	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;

	bool isReadyForUpgrade() const;
	bool CanUpgradeRightNow(bool bOnlyTestVisible) const;
	bool CanUpgradeTo(UnitTypes eUpgradeUnitType, bool bOnlyTestVisible) const;
	bool CanUpgradeInTerritory(bool bOnlyTestVisible) const;
	UnitTypes GetUpgradeUnitType() const;
	int upgradePrice(UnitTypes eUnit) const;
	CvUnit* DoUpgrade(bool bFree = false);
	CvUnit* DoUpgradeTo(UnitTypes eUpgradeUnitType, bool bFree = false);

	HandicapTypes getHandicapType() const;
	const CvCivilizationInfo& getCivilizationInfo() const;
	CivilizationTypes getCivilizationType() const;
	const char* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	bool IsGreatPerson() const;
	UnitTypes getCaptureUnitType(PlayerTypes eCapturingPlayer) const;
	UnitCombatTypes getUnitCombatType() const;
	void setUnitCombatType(UnitCombatTypes eCombat);
	UnitCombatTypes getUnitPromotionType() const;
	DomainTypes getDomainType() const;
	//check if plot type matches the (primary) domain type
	bool isNativeDomain(const CvPlot* pPlot) const;
	//check territory, terrain, stacking, etc
	bool canEndTurnAtPlot(const CvPlot* pPlot) const;

	int flavorValue(FlavorTypes eFlavor) const;

	bool isBarbarian() const;
	bool isHuman(IsHumanReason eIsHumanReason = OTHER_ISHUMAN_REASON) const;

	int visibilityRange() const;
	int reconRange() const;
	bool canChangeVisibility() const;

	int baseMoves(bool bPretendEmbarked) const;
	int maxMoves() const;
	int movesLeft() const;
	bool canMove() const;
	bool hasMoved() const;

	// VP - Linked & Group Movement 
#if defined(MOD_LINKED_MOVEMENT)
	bool IsLinked() const;
	void SetIsLinked(bool bValue);
	bool IsLinkedLeader() const;
	void SetIsLinkedLeader(bool bValue);
	int GetLinkedLeaderID() const;
	void SetLinkedLeaderID(int iLinkedLeaderID);
	bool IsGrouped() const;
	void SetIsGrouped(bool bValue);
	void SetLinkedUnits(const UnitIdContainer& LinkedUnits);
	UnitIdContainer GetLinkedUnits();
	int GetLinkedMaxMoves() const;
	void SetLinkedMaxMoves(int iValue);
	bool CanLinkUnits();
	void LinkUnits();
	void UnlinkUnits();
	void MoveLinkedLeader(CvPlot* pDestPlot);
	void DoGroupMovement(CvPlot* pDestPlot);
#endif

	// VP - Squads control groups modmod
	int  GetSquadNumber() const;
	void AssignToSquad(int iNewSquadNumber);
	void RemoveFromSquad();
	CvPlot* GetSquadCenterOfMass();
	void DoSquadPlotAssignmentsByDomain(std::vector<CvUnit*> eligibleUnits, CvPlot* pDestPlot, std::map<CvUnit*, CvPlot*>& unitToPlotMap);
	std::map<CvUnit*, CvPlot*> DoSquadPlotAssignments(CvPlot* pDestPlot, bool escort, bool computeOnly);
	void DoSquadMovement(CvPlot* pDestPlot, bool escort);
	void GetSquadMovementPreview(std::vector<CvPlot*>& pPlotList, CvPlot* pDestPlot);
	bool IsUnitInActiveMoveMission();
	bool IsSquadMoving();
	bool SquadHasLink();
	void TryEndSquadMovement();
	void SetSquadDestination(CvPlot* pDestPlot = NULL);
	bool HasSquadDestination();
	CvPlot* GetSquadDestination();
	SquadsEndMovementType GetSquadEndMovementType() const;
	void SetSquadEndMovementType(SquadsEndMovementType endMovementType);

	int GetRange() const;
	int GetNukeDamageLevel() const;

	bool canBuildRoute() const;
	BuildTypes getBuildType() const;
	bool IsWorking() const;
	int workRate(bool bMax, BuildTypes eBuild = NO_BUILD) const;

	bool isNoBadGoodies() const;
	bool isRivalTerritory() const;
	int getRivalTerritoryCount() const;
	void changeRivalTerritoryCount(int iChange);
	bool isFound() const;
	bool IsFoundAbroad() const;
	bool IsFoundMid() const;
	bool IsFoundLate() const;
	bool CanFoundColony() const;
	bool IsWork() const;
	bool isGoldenAge() const;
	bool isGivesPolicies() const;
	bool isBlastTourism() const;
	bool canCoexistWithEnemyUnit(TeamTypes eTeam) const;

	bool isSlowInEnemyLand() const; //used to be setup for ranged attack
	int getIsSlowInEnemyLandCount() const;
	void changeIsSlowInEnemyLandCount(int iChange);

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
	int GetUnhappinessCombatPenalty() const;

	void SetBaseCombatStrength(int iCombat);
	void ChangeBaseCombatStrength(int iValue);
	int GetBaseCombatStrength() const;
	int GetBestAttackStrength() const; //ranged or melee, whichever is greater
	int GetDamageCombatModifier(bool bForDefenseAgainstRanged = false, int iAssumedDamage = 0) const;

	int GetCombatModifierFromCapitalDistance(const CvPlot* pBattlePlot) const;

	int GetGenericMeleeStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bAttacking,
									bool bIgnoreUnitAdjacencyBoni, const CvPlot* pFromPlot = NULL, bool bQuickAndDirty = false) const;
	int GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false, int iAssumeExtraDamage = 0) const;
	int GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, const CvPlot* pFromPlot, 
									bool bFromRangedAttack = false, bool bQuickAndDirty = false, int iAssumeExtraDamage = 0) const;

	int GetEmbarkedUnitDefense() const;

	int GetBaseRangedCombatStrength() const;
	void SetBaseRangedCombatStrength(int iStrength);

	int GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking, 
									const CvPlot* pMyPlot = NULL, const CvPlot* pOtherPlot = NULL, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false, int iAssumeExtraDamage = 0) const;
	int GetAirCombatDamage(const CvUnit* pDefender, const CvCity* pCity, int iGarrisonMaxHP, int& iGarrisonDamage, bool bIncludeRand,
									int iAssumeExtraDefenderDamage = 0,
									const CvPlot* pTargetPlot = NULL, const CvPlot* pFromPlot = NULL, bool bQuickAndDirty = false) const;
	int GetRangeCombatDamage(const CvUnit* pDefender, const CvCity* pCity, int iGarrisonMaxHP, int& iGarrisonDamage, bool bIncludeRand,
									int iAssumeExtraDefenderDamage = 0,
									const CvPlot* pTargetPlot = NULL, const CvPlot* pFromPlot = NULL, 
									bool bIgnoreUnitAdjacencyBoni = false, bool bQuickAndDirty = false) const;
	int GetRangeCombatSplashDamage(const CvPlot* pTargetPlot) const;

	int EstimatePlagueDamage(const CvUnit* pEnemy) const;

	bool canSiege(TeamTypes eTeam) const;
	bool canAirDefend(const CvPlot* pPlot = NULL) const;

	int GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand = true, const CvPlot* pTargetPlot = NULL) const;
	int GetInterceptionDamage(const CvUnit* pInterceptedAttacker, bool bIncludeRand = true, const CvPlot* pTargetPlot = NULL) const;

	int GetResistancePower(const CvUnit* pOtherUnit) const;

	bool isWaiting() const;
	bool IsEverFortifyable() const;
	int fortifyModifier() const;

	int experienceNeeded() const;
	int maxXPValue() const;

	bool ignoreBuildingDefense() const;

	bool ignoreTerrainCost() const;
	int getIgnoreTerrainCostCount() const;
	void changeIgnoreTerrainCostCount(int iValue);

	bool ignoreTerrainDamage() const;
	int getIgnoreTerrainDamageCount() const;
	void changeIgnoreTerrainDamageCount(int iValue);

	bool ignoreFeatureDamage() const;
	int getIgnoreFeatureDamageCount() const;
	void changeIgnoreFeatureDamageCount(int iValue);

	bool extraTerrainDamage() const;
	int getExtraTerrainDamageCount() const;
	void changeExtraTerrainDamageCount(int iValue);

	bool extraFeatureDamage() const;
	int getExtraFeatureDamageCount() const;
	void changeExtraFeatureDamageCount(int iValue);

	bool IsCannotHeal(bool bConsiderResourceShortage) const;
	int getCannotHealCount() const;
	void changeCannotHealCount(int iValue);

	bool IsPillageFortificationsOnKill() const;
	int getPillageFortificationsOnKillCount() const;
	void changePillageFortificationsOnKillCount(int iValue);

	int GetNearbyImprovementCombatBonus() const;
	void SetNearbyImprovementCombatBonus(int iCombatBonus);
	int GetNearbyImprovementBonusRange() const;
	void SetNearbyImprovementBonusRange(int iBonusRange);
	ImprovementTypes GetCombatBonusImprovement() const;
	void SetCombatBonusImprovement(ImprovementTypes eImprovement);

	int getNearbyUnitClassBonus() const;
	void SetNearbyUnitClassBonus(int iCombatBonus);
	int getNearbyUnitClassBonusRange() const;
	void SetNearbyUnitClassBonusRange(int iBonusRange);
	UnitClassTypes getCombatBonusFromNearbyUnitClass() const;
	void SetCombatBonusFromNearbyUnitClass(UnitClassTypes eUnitClass);
	void ChangeNearbyPromotion(int iValue);
	int GetNearbyPromotion() const;
	bool isNearbyPromotion() const;
	int GetNearbyUnitPromotionsRange() const;
	void ChangeNearbyUnitPromotionRange(int iBonusRange);
	void ChangeNearbyCityCombatMod(int iValue);
	int getNearbyCityCombatMod() const;
	void ChangeNearbyFriendlyCityCombatMod(int iValue);
	int getNearbyFriendlyCityCombatMod() const;
	void ChangeNearbyEnemyCityCombatMod(int iValue);
	int getNearbyEnemyCityCombatMod() const;
	int getPillageBonusStrengthPercent() const;
	void ChangePillageBonusStrengthPercent(int iBonus);
	int getStackedGreatGeneralExperience() const;
	void ChangeStackedGreatGeneralExperience(int iExperience);
	int getWonderProductionModifier() const;
	void ChangeWonderProductionModifier(int iValue);
	int getMilitaryProductionModifier() const;
	void ChangeMilitaryProductionModifier(int iValue);
	int GetTileDamageIfNotMoved() const;
	void ChangeTileDamageIfNotMoved(int iValue);
	int GetFortifiedModifier() const;
	void ChangeFortifiedModifier(int iValue);
	int getNearbyEnemyDamage() const;
	void ChangeNearbyEnemyDamage(int iValue);
	int GetAdjacentCityDefenseMod() const;
	int GetGGGAXPPercent() const;
	void ChangeAdjacentCityDefenseMod(int iValue);
	void ChangeGGGAXPPercent(int iValue);
	int getGiveCombatMod() const;
	void ChangeGiveCombatMod(int iValue);
	int getGiveHPIfEnemyKilled() const;
	void ChangeGiveHPIfEnemyKilled(int iValue);
	int getGiveExperiencePercent() const;
	void ChangeGiveExperiencePercent(int iValue);
	int getGiveOutsideFriendlyLandsModifier() const;
	void ChangeGiveOutsideFriendlyLandsModifier(int iValue);
	const DomainTypes getGiveDomain() const;
	void ChangeGiveDomain(DomainTypes eDomain);
	int getGiveExtraAttacks() const;
	void ChangeGiveExtraAttacks(int iValue);
	int getGiveDefenseMod() const;
	void ChangeGiveDefenseMod(int iValue);
	int getNearbyHealEnemyTerritory() const;
	void ChangeNearbyHealEnemyTerritory(int iValue);
	int getNearbyHealNeutralTerritory() const;
	void ChangeNearbyHealNeutralTerritory(int iValue);
	int getNearbyHealFriendlyTerritory() const;
	void ChangeNearbyHealFriendlyTerritory(int iValue);
	void ChangeIsGiveInvisibility(int iValue);
	int GetIsGiveInvisibility() const;
	bool isGiveInvisibility() const;
	bool isGiveOnlyOnStartingTurn() const;
	void SetIsGiveOnlyOnStartingTurn(bool bNewValue);
	void ChangeIsConvertUnit(int iValue);
	int getIsConvertUnit() const;
	bool isConvertUnit() const;
	const DomainTypes getConvertDomain() const;
	void ChangeConvertDomain(DomainTypes eDomain);
	const UnitTypes getConvertDomainUnitType() const;
	void ChangeConvertDomainUnit(UnitTypes eUnit);
	void ChangeIsConvertEnemyUnitToBarbarian(int iValue);
	int getIsConvertEnemyUnitToBarbarian() const;
	bool isConvertEnemyUnitToBarbarian() const;
	void ChangeIsConvertOnFullHP(int iValue);
	bool isConvertOnFullHP() const;
	void ChangeIsConvertOnDamage(int iValue);
	bool isConvertOnDamage() const;
	int getDamageThreshold() const;
	void ChangeDamageThreshold(int iValue);
	const UnitTypes getConvertDamageOrFullHPUnit() const;
	void ChangeConvertDamageOrFullHPUnit(UnitTypes eUnit);
	bool canIntercept() const;
	int GetAirInterceptRange() const;
	bool canCrossMountains() const;
	int getCanCrossMountainsCount() const;
	void changeCanCrossMountainsCount(int iValue);

	bool canCrossOceans() const;
	int getCanCrossOceansCount() const;
	void changeCanCrossOceansCount(int iValue);

	bool canCrossIce() const;
	int getCanCrossIceCount() const;
	void changeCanCrossIceCount(int iValue);

	void ChangeNumTilesRevealedThisTurn(int iValue);
	void SetNumTilesRevealedThisTurn(int iValue);
	int GetNumTilesRevealedThisTurn();

	bool HasSpottedEnemy() const;
	void SetSpottedEnemy(bool bValue);

	bool HasSpottedRuin() const;
	void SetSpottedRuin(bool bValue);

	bool IsGainsXPFromScouting() const;
	int GetGainsXPFromScouting() const;
	void ChangeGainsXPFromScouting(int iValue);

	bool IsGainsXPFromSpotting() const;
	int GetGainsXPFromSpotting() const;
	void ChangeGainsXPFromSpotting(int iValue);

	int GetXPFromPillaging() const;
	void ChangeXPFromPillaging(int iValue);

	int GetExtraXPOnKill() const;
	void ChangeExtraXPOnKill(int iValue);

	bool IsGainsYieldFromScouting() const;

	int GetCaptureDefeatedEnemyChance() const;
	void ChangeCaptureDefeatedEnemyChance(int iValue);
	
	int GetBarbarianCombatBonus() const;
	void ChangeBarbarianCombatBonus(int iValue);

	int GetAdjacentEnemySapMovement() const;
	void ChangeAdjacentEnemySapMovement(int iValue);

	bool isGGFromBarbarians() const;
	int getGGFromBarbariansCount() const;
	void changeGGFromBarbariansCount(int iValue);

	bool IsRoughTerrainEndsTurn() const;
	int GetRoughTerrainEndsTurnCount() const;
	void ChangeRoughTerrainEndsTurnCount(int iValue);

	bool IsCapturedUnitsConscripted() const;
	int GetCapturedUnitsConscriptedCount() const;
	void ChangeCapturedUnitsConscriptedCount(int iValue);

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

	bool isNoAttackInOcean() const;
	int getNoAttackInOceanCount() const;
	void changeNoAttackInOceanCount(int iValue);

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

	int interceptionProbability() const;
	int evasionProbability() const;
	int withdrawalProbability() const;

	int GetNumFriendlyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL) const;
	int GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude = NULL) const;
	bool IsEnemyCityAdjacent(const CvCity* pSpecifyCity = NULL) const;
	int GetNumOwningPlayerUnitsAdjacent(const CvUnit* pUnitToExclude = NULL, const CvUnit* pExampleUnitType = NULL, bool bCombatOnly = true) const;
	bool IsFriendlyUnitAdjacent(bool bCombatUnit) const;
	bool IsCoveringFriendlyCivilian() const;

	int GetAdjacentModifier() const;
	int GetNoAdjacentUnitModifier() const;
	void ChangeAdjacentModifier(int iValue);
	void ChangeNoAdjacentUnitModifier(int iValue);
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
	int getDefenseModifier(bool bQuick = false) const;
	void changeDefenseModifier(int iValue);

	int getGroundAttackDamage() const;
	void changeGroundAttackDamage(int iValue);

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
	int attackFullyHealedModifier() const;
	int attackAbove50HealthModifier() const;
	int attackBelow50HealthModifier() const;
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

	int GetYieldModifier(YieldTypes eYield) const;
	void SetYieldModifier(YieldTypes eYield, int iValue);

	int GetYieldChange(YieldTypes eYield) const;
	void SetYieldChange(YieldTypes eYield, int iValue);

	int GetYieldFromCombatExperienceTimes100(YieldTypes eYield) const;
	void SetYieldFromCombatExperienceTimes100(YieldTypes eYield, int iValue);

	int GetGarrisonYieldChange(YieldTypes eYield) const;
	void SetGarrisonYieldChange(YieldTypes eYield, int iValue);

	int GetFortificationYieldChange(YieldTypes eYield) const;
	void SetFortificationYieldChange(YieldTypes eYield, int iValue);

	inline int GetID() const
	{
		return m_iID;
	}
	IDInfo GetIDInfo() const;
	void SetID(int iID);

	int getHotKeyNumber() const;
	void setHotKeyNumber(int iNewValue);

	inline int getX() const
	{
		return m_iX;
	}

	inline int getY() const
	{
		return m_iY;
	}

	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false, bool bNoMove = false);
	bool at(int iX, int iY) const;
	bool atPlot(const CvPlot& plot) const;
	CvPlot* plot() const;
	bool onMap() const;

	void setOriginCity(int iNewValue);
	CvCity* getOriginCity() const;

	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);

	int GetCycleOrder() const;
	void SetCycleOrder(int iNewValue);

	bool IsRecentlyDeployedFromOperation() const;
	void SetDeployFromOperationTurn(int iTurn)
	{
		m_iDeployFromOperationTurn = iTurn;
	};
	int GetDeployFromOperationTurn()
	{
		return m_iDeployFromOperationTurn;
	};

	bool IsRecon() const;
	int GetReconCount() const;
	void ChangeReconCount(int iChange);

	CvPlot* getReconPlot() const;
	void setReconPlot(CvPlot* pNewValue);

	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);

	int getTurnSliceCreated() const;
	void setTurnSliceCreated(int iNewValue);

	int getDamage() const;
	int setDamage(int iNewValue, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, const CvString* pAppendText = NULL, bool bDontShow = false);
	int changeDamage(int iChange, PlayerTypes ePlayer = NO_PLAYER, float fAdditionalTextDelay = 0.0f, const CvString* pAppendText = NULL);

	int addDamageReceivedThisTurn(int iDamage, CvUnit* pAttacker=NULL);
	void flipDamageReceivedPerTurn();
	bool isProjectedToDieNextTurn() const;

	int getMoves() const;
	void changeMoves(int iChange);
	void restoreFullMoves();
	void updateConditionalPromotions();
	void finishMoves();

	bool IsImmobile() const;
	void SetImmobile(bool bValue);

	bool IsInForeignOwnedTerritory() const;

	int getExperienceTimes100() const;
	void setExperienceTimes100(int iNewValueTimes100, int iMax = -1, bool bDontShow = false);
	void changeExperienceTimes100(int iChangeTimes100, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false, bool bFromHuman = false);

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

	int GetMapLayer() const;
	bool CanGarrison() const;
	bool IsGarrisoned(void) const;
	void SetGarrisonedCity(int iCityID);
	CvCity* GetGarrisonedCity() const;
	void triggerFortifyAnimation(bool bState);
	bool IsFortified() const;
	void SetFortified(bool bValue);
	bool getHasWithdrawnThisTurn() const;
	void setHasWithdrawnThisTurn(bool bNewValue);

	void DoExtraPlotDamage(CvPlot* pWhere, int iValue, const char* chTextKey);
	int DoAdjacentPlotDamage(CvPlot* pWhere, int iValue, const char* chTextKey = NULL);
	void DoAdjacentHeal(CvPlot* pWhere, int iValue, const char* chTextKey = NULL);

	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphibious() const;
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
	
	int getRiverDoubleMoveCount() const;
	bool isRiverDoubleMove() const;
	void changeRiverDoubleMoveCount(int iChange);

	int getEmbarkFlatCostCount() const;
	bool isEmbarkFlatCost() const;
	void changeEmbarkFlatCostCount(int iChange);

	int getDisembarkFlatCostCount() const;
	bool isDisembarkFlatCost() const;
	void changeDisembarkFlatCostCount(int iChange);

	int getAOEDamageOnKill() const;
	void changeAOEDamageOnKill(int iChange);

	int getAOEDamageOnPillage() const;
	void changeAOEDamageOnPillage(int iChange);

	int getAOEHealOnPillage() const;
	void changeAOEHealOnPillage(int iChange);

	int GetCombatModPerCSAlliance() const;
	void ChangeCombatModPerCSAlliance(int iChange);

	int GetStrengthThisTurnFromPreviousSamePromotionAttacks() const;
	void SetStrengthThisTurnFromPreviousSamePromotionAttacks(int iNewValue);

	int getPartialHealOnPillage() const;
	void changePartialHealOnPillage(int iChange);

	int getAoEDamageOnMove() const;
	void changeAoEDamageOnMove(int iChange);

	int getSplashDamage() const;
	void changeSplashDamage(int iChange);

	int getMultiAttackBonus() const;
	void changeMultiAttackBonus(int iChange);

	int getLandAirDefenseValue() const;
	void changeLandAirDefenseValue(int iChange);

	int getExtraVisibilityRange() const;
	void changeExtraVisibilityRange(int iChange);

	int getExtraReconRange() const;
	void changeExtraReconRange(int iChange);

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

	int GetGoldenAgeValueFromKills() const;
	void ChangeGoldenAgeValueFromKills(int iValue);

	int getExtraRange() const;
	void changeExtraRange(int iChange);

	int getInterceptChance() const;
	void changeInterceptChance(int iChange);

	int getExtraEvasion() const;
	void changeExtraEvasion(int iChange);

	int getExtraWithdrawal() const;
	void changeExtraWithdrawal(int iChange);

	// Plague Stuff
	bool HasPlague(int iPlagueID = -1, int iMinimumPriority = -1) const;
	void RemovePlague(int iPlagueID = -1, int iHigherPriority = -1);

	void ProcessAttackForPromotionSameAttackBonus();

	void setContractUnit(ContractTypes eContract);
	bool isContractUnit() const;
	ContractTypes getContract() const;

	bool IsNoMaintenance() const;
	void SetNoMaintenance(bool bValue);

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

	int getExtraAttackFullyHealedMod() const;
	void changeExtraAttackFullyHealedMod(int iChange);

	int getExtraAttackAboveHealthMod() const;
	void changeExtraAttackAboveHealthMod(int iChange);

	int getExtraAttackBelowHealthMod() const;
	void changeExtraAttackBelowHealthMod(int iChange);

	bool IsRangedFlankAttack() const;
	void ChangeRangedFlankAttackCount(int iChange);

	int GetFlankPower() const;
	void ChangeFlankPower(int iChange);

	int GetFlankAttackModifier() const;
	void ChangeFlankAttackModifier(int iChange);

	int getExtraOpenDefensePercent() const;
	void changeExtraOpenDefensePercent(int iChange);

	int getExtraOpenFromPercent() const;
	void changeExtraOpenFromPercent(int iChange);

	int getExtraRoughDefensePercent() const;
	void changeExtraRoughDefensePercent(int iChange);

	int getExtraRoughFromPercent() const;
	void changeExtraRoughFromPercent(int iChange);

	int getNumAttacks() const;
	int getNumAttacksMadeThisTurn() const;
	void changeExtraAttacks(int iChange);

	int GetNearbyCityBonusCombatMod(const CvPlot * pAtPlot = NULL) const;

	int GetGoldenAgeGeneralExpPercent() const;
	int GetGiveExperiencePercentToUnit() const;
	int GetGiveCombatModToUnit(const CvPlot * pAtPlot = NULL) const;
	int GetGiveDefenseModToUnit() const;
	int GetHealEnemyTerritoryFromNearbyUnit() const;
	int GetHealNeutralTerritoryFromNearbyUnit() const;
	int GetHealFriendlyTerritoryFromNearbyUnit() const;
	int GetGiveOutsideFriendlyLandsModifierToUnit() const;
	int GetGiveExtraAttacksToUnit() const;
	int GetGiveHPIfEnemyKilledToUnit() const;
	bool IsHiddenByNearbyUnit(const CvPlot * pAtPlot = NULL) const;

	// Great General Stuff
	bool IsNearCityAttackSupport(const CvPlot* pAtPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;
	bool IsNearGreatGeneral(const CvPlot* pAtPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;

	bool IsStackedGreatGeneral(const CvPlot* pLoopPlot = NULL, const CvUnit* pIgnoreThisGeneral = NULL) const;
	int GetGreatGeneralStackMovement(const CvPlot* pLoopPlot = NULL) const;
	int GetReverseGreatGeneralModifier(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifier(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifierFromTraits(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifierFromPromotions(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyImprovementModifier(ImprovementTypes eBonusImprovement, int iImprovementRange, int iImprovementModifier, const CvPlot* pAtPlot = NULL) const;

	bool IsCombatSupportUnit() const;

	bool IsGreatGeneral() const;
	int GetGreatGeneralCount() const;
	void ChangeGreatGeneralCount(int iChange);
	bool IsGreatAdmiral() const;
	int GetGreatAdmiralCount() const;
	void ChangeGreatAdmiralCount(int iChange);

	int GetAuraRangeChange() const;
	void ChangeAuraRangeChange(int iChange);
	int GetAuraEffectChange() const;
	void ChangeAuraEffectChange(int iChange);

	int GetNumRepairCharges() const;
	void ChangeNumRepairCharges(int iChange);

	int GetMilitaryCapChange() const;
	void ChangeMilitaryCapChange(int iChange);

	int getGreatGeneralModifier() const;
	void changeGreatGeneralModifier(int iChange);

	bool IsGreatGeneralReceivesMovement() const;
	void ChangeGreatGeneralReceivesMovementCount(int iChange);
	int GetGreatGeneralCombatModifier() const;
	void ChangeGreatGeneralCombatModifier(int iChange);

	bool IsIgnoreGreatGeneralBenefit() const;
	void ChangeIgnoreGreatGeneralBenefitCount(int iChange);
	// END Great General Stuff

	bool isNoSupply() const;
	void changeNoSupply(int iChange);

	int getMaxHitPointsBase() const;
	void setMaxHitPointsBase(int iMaxHitPoints);
	void changeMaxHitPointsBase(int iChange);
	
	int getMaxHitPointsChange() const;
	int getMaxHitPointsModifier() const;
	void changeMaxHitPointsModifier(int iChange);
	void changeMaxHitPointsChange(int iChange);
	int GetVsUnhappyMod() const;
	void ChangeVsUnhappyMod(int iChange);

	bool IsIgnoreZOC() const;
	void ChangeIgnoreZOCCount(int iChange);

	bool IsSapper() const;
	void ChangeSapperCount(int iChange);
	int GetNearbyUnitClassModifierFromUnitClass(const CvPlot* pAtPlot = NULL) const;
	int GetNearbyUnitClassModifier(UnitClassTypes eUnitClass, int iUnitClassRange, int iUnitClassModifier, const CvPlot* pAtPlot = NULL) const;
	void DoNearbyUnitPromotion(const CvPlot* pPlot = NULL);
	void DoImprovementExperience(const CvPlot* pPlot = NULL);
	bool IsStrongerDamaged() const;
	void ChangeIsStrongerDamaged(int iChange);
	bool IsDiplomaticMissionAccomplishment() const;
	void ChangeDiplomaticMissionAccomplishment(int iChange);
	bool IsFreeAttackMoves() const;
	void ChangeFreeAttackMoves(int iChange);
	bool IsFightWellDamaged() const;
	void ChangeIsFightWellDamaged(int iChange);

	int GetGoodyHutYieldBonus() const;
	void ChangeGoodyHutYieldBonus(int iChange);

	int GetReligiousPressureModifier() const;
	void ChangeReligiousPressureModifier(int iChange);

	void DoStackedGreatGeneralExperience(const CvPlot* pPlot = NULL);
	void DoConvertOnDamageThreshold(const CvPlot* pPlot = NULL);
	void DoConvertEnemyUnitToBarbarian(const CvPlot* pPlot = NULL);
	void DoConvertReligiousUnitsToMilitary(const CvPlot* pPlot = NULL);
	void DoFinishBuildIfSafe();

	bool IsCanHeavyCharge() const;
	void ChangeCanHeavyChargeCount(int iChange);

	int GetMoraleBreakChance() const;
	void ChangeMoraleBreakChance(int iChange);

	int GetDamageAoEFortified() const;
	void ChangeDamageAoEFortified(int iChange);

	int GetWorkRateMod() const;
	void ChangeWorkRateMod(int iChange);

	int GetDamageReductionCityAssault() const;
	void ChangeDamageReductionCityAssault(int iChange);

	int getFriendlyLandsModifier() const;
	void changeFriendlyLandsModifier(int iChange);

	int getFriendlyLandsAttackModifier() const;
	void changeFriendlyLandsAttackModifier(int iChange);

	int getOutsideFriendlyLandsModifier() const;
	void changeOutsideFriendlyLandsModifier(int iChange);

	int getBorderCombatStrengthModifier() const;
	void changeBorderCombatStrengthModifier(int iChange);

	int getCombatStrengthModifierPerMarriage() const;
	void changeCombatStrengthModifierPerMarriage(int iChange);

	int getCombatStrengthModifierPerMarriageCap() const;
	void changeCombatStrengthModifierPerMarriageCap(int iChange);

	int getCSMarriageStrength() const;

	int getPillageChange() const;
	void changePillageChange(int iChange);

	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);

	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);

	DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();

	int GetBlockadeRange() const;

	bool isSuicide() const;
	bool isTrade() const;

	int getDropRange() const;
	void changeDropRange(int iChange);

	bool isOutOfAttacks(bool bIgnoreMoves = false) const;
	void setMadeAttack(bool bNewValue);

	int GetNumInterceptions() const;
	void ChangeNumInterceptions(int iChange);

	int GetExtraAirInterceptRange() const; // JJ: New
	void ChangeExtraAirInterceptRange(int iChange);

	bool canInterceptNow() const;
	int getMadeInterceptionCount() const;
	void increaseInterceptionCount();
	void resetInterceptionCount();

	bool TurnProcessed() const;
	void SetTurnProcessed(bool bValue);

	bool isPromotionReady() const;
	void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath(bool bCheckOnMap = true) const;
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
		return m_eOwner;
	}
	PlayerTypes getVisualOwner(TeamTypes eFromPerspectiveOfTeam = NO_TEAM) const;
	PlayerTypes getCombatOwner(TeamTypes eFromPerspectiveOfTeam, const CvPlot& assumedUnitPlot) const;
	TeamTypes getTeam() const;

	PlayerTypes GetOriginalOwner() const;
	void SetOriginalOwner(PlayerTypes ePlayer);

	PlayerTypes GetGiftedByPlayer() const;
	void SetGiftedByPlayer(PlayerTypes ePlayer);

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);
	bool IsCapturedAsIs() const;
	void SetCapturedAsIs(bool bSetValue);
	bool IsCapturedAsConscript() const;
	void SetCapturedAsConscript(bool bSetValue);

	const UnitTypes getUnitType() const;
	CvUnitEntry& getUnitInfo() const;
	UnitClassTypes getUnitClassType() const;
	bool isUnitAI(UnitAITypes eType) const;

	const UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);

	const InvisibleTypes getInvisibleType() const;
	void setInvisibleType(InvisibleTypes InvisibleType);

	const InvisibleTypes getSeeInvisibleType() const;
	void setSeeInvisibleType(InvisibleTypes InvisibleType);

	const CvUnit* getCombatUnit() const;
	CvUnit* getCombatUnit();
	void setCombatUnit(CvUnit* pCombatUnit, bool bAttacking = false);

	const CvCity* getCombatCity() const;
	CvCity* getCombatCity();
	void setCombatCity(CvCity* pCombatCity);

	void clearCombat();

	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);

	int getExtraDomainAttack(DomainTypes eIndex) const;
	void changeExtraDomainAttack(DomainTypes eIndex, int iChange);

	int getExtraDomainDefense(DomainTypes eIndex) const;
	void changeExtraDomainDefense(DomainTypes eIndex, int iChange);

	const CvString getName() const;
	const char* getNameKey() const;
	const CvString getUnitName() const;
	void setUnitName(const CvString& strNewValue);
	const CvString getNameNoDesc() const;
	void setName(const CvString strNewValue);
	const CvString getGreatName() const;
	void setGreatName(const CvString& strName);
	GreatWorkType GetGreatWork() const;
	void SetGreatWork(GreatWorkType eGreatWork);
	bool HasGreatWork() const;
	bool HasUnusedGreatWork() const;
	int GetTourismBlastStrength() const;
	void SetTourismBlastStrength(int iValue);

	int GetTourismBlastLength() const;
	void SetTourismBlastLength(int iValue);

	int GetScienceBlastStrength() const;
	void SetScienceBlastStrength(int iValue);

	int GetHurryStrength() const;
	void SetHurryStrength(int iValue);

	int GetGoldBlastStrength() const;
	void SetGoldBlastStrength(int iValue);

	int GetCultureBlastStrength() const;
	void SetCultureBlastStrength(int iValue);

	int GetGAPBlastStrength() const;
	void SetGAPBlastStrength(int iValue);

	bool IsPromotionEverObtained(PromotionTypes eIndex) const;
	void SetPromotionEverObtained(PromotionTypes eIndex, bool bValue);

	// Arbitrary Script Data
	std::string getScriptData() const;
	void setScriptData(const std::string& strNewValue);
	int getScenarioData() const;
	void setScenarioData(int iNewValue);

	int getIgnoreTerrainCostInCount(TerrainTypes eIndex) const;
	inline bool isIgnoreTerrainCostIn(TerrainTypes eIndex) const { return getIgnoreTerrainCostInCount(eIndex) > 0; }
	void changeIgnoreTerrainCostInCount(TerrainTypes eIndex, int iChange);

	int getIgnoreTerrainCostFromCount(TerrainTypes eIndex) const;
	inline bool isIgnoreTerrainCostFrom(TerrainTypes eIndex) const { return getIgnoreTerrainCostFromCount(eIndex) > 0; }
	void changeIgnoreTerrainCostFromCount(TerrainTypes eIndex, int iChange);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainDoubleMove(TerrainTypes eIndex) const { return getTerrainDoubleMoveCount(eIndex) > 0; }
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getIgnoreFeatureCostInCount(FeatureTypes eIndex) const;
	inline bool isIgnoreFeatureCostIn(FeatureTypes eIndex) const { return getIgnoreFeatureCostInCount(eIndex) > 0; }
	void changeIgnoreFeatureCostInCount(FeatureTypes eIndex, int iChange);

	int getIgnoreFeatureCostFromCount(FeatureTypes eIndex) const;
	inline bool isIgnoreFeatureCostFrom(FeatureTypes eIndex) const { return getIgnoreFeatureCostFromCount(eIndex) > 0; }
	void changeIgnoreFeatureCostFromCount(FeatureTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureDoubleMove(FeatureTypes eIndex) const { return getFeatureDoubleMoveCount(eIndex) > 0; }
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);

	int getTerrainHalfMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainHalfMove(TerrainTypes eIndex) const { return getTerrainHalfMoveCount(eIndex) > 0; }
	void changeTerrainHalfMoveCount(TerrainTypes eIndex, int iChange);

	int getTerrainExtraMoveCount(TerrainTypes eIndex) const;
	inline bool isTerrainExtraMove(TerrainTypes eIndex) const { return getTerrainExtraMoveCount(eIndex) > 0; }
	void changeTerrainExtraMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureHalfMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureHalfMove(FeatureTypes eIndex) const { return getFeatureHalfMoveCount(eIndex) > 0; }
	void changeFeatureHalfMoveCount(FeatureTypes eIndex, int iChange);

	int getFeatureExtraMoveCount(FeatureTypes eIndex) const;
	inline bool isFeatureExtraMove(FeatureTypes eIndex) const { return getFeatureExtraMoveCount(eIndex) > 0; }
	void changeFeatureExtraMoveCount(FeatureTypes eIndex, int iChange);

	int getTerrainDoubleHeal(TerrainTypes eIndex) const;
	bool isTerrainDoubleHeal(TerrainTypes eIndex) const;
	void changeTerrainDoubleHeal(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleHeal(FeatureTypes eIndex) const;
	bool isFeatureDoubleHeal(FeatureTypes eIndex) const;
	void changeFeatureDoubleHeal(FeatureTypes eIndex, int iChange);

	void ChangeNumTimesAttackedThisTurn(PlayerTypes ePlayer, int iValue);
	int GetNumTimesAttackedThisTurn(PlayerTypes ePlayer) const;

	int getCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex, int iChange);

	int getCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex, int iChange);

	int getCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex) const;
	void changeCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex, int iChange);

	int getTerrainImpassableCount(TerrainTypes eIndex) const;
	bool isTerrainImpassable(TerrainTypes eIndex) const;
	void changeTerrainImpassableCount(TerrainTypes eIndex, int iChange);

	int getFeatureImpassableCount(FeatureTypes eIndex) const;
	bool isFeatureImpassable(FeatureTypes eIndex) const;
	void changeFeatureImpassableCount(FeatureTypes eIndex, int iChange);

	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int GetTerrainModifierDefense(TerrainTypes eIndex) const;
	void ChangeTerrainModifierDefense(TerrainTypes eIndex, int iChange);
	int GetTerrainModifierAttack(TerrainTypes eIndex) const;
	void ChangeTerrainModifierAttack(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);

	int getUnitClassAttackMod(UnitClassTypes eIndex) const;
	void changeUnitClassAttackMod(UnitClassTypes eIndex, int iChange);

	int getUnitClassDefenseMod(UnitClassTypes eIndex) const;
	void changeUnitClassDefenseMod(UnitClassTypes eIndex, int iChange);

	int getYieldFromKills(YieldTypes eIndex) const;
	void changeYieldFromKills(YieldTypes eIndex, int iChange);
	int getYieldFromBarbarianKills(YieldTypes eIndex) const;
	void changeYieldFromBarbarianKills(YieldTypes eIndex, int iChange);

	int getYieldFromScouting(YieldTypes eIndex) const;
	void changeYieldFromScouting(YieldTypes eIndex, int iChange);
	int getYieldFromAncientRuins(YieldTypes eIndex) const;
	void changeYieldFromAncientRuins(YieldTypes eIndex, int iChange);
	int getYieldFromTRPlunder(YieldTypes eIndex) const;
	void changeYieldFromTRPlunder(YieldTypes eIndex, int iChange);
	bool isCultureFromExperienceDisbandUpgrade() const;
	bool isFreeUpgrade() const;
	bool isUnitEraUpgrade() const;
	bool isReligiousUnit() const;
	bool isWLKTKDOnBirth() const;
	bool isGoldenAgeOnBirth() const;
	bool isCultureBoost() const;
	bool isExtraAttackHealthOnKill() const;
	bool isHighSeaRaiderUnit() const;

	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);
	int getExtraUnitCombatModifierAttack(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifierAttack(UnitCombatTypes eIndex, int iChange);
	int getExtraUnitCombatModifierDefense(UnitCombatTypes eIndex) const;
	void changeExtraUnitCombatModifierDefense(UnitCombatTypes eIndex, int iChange);

	int getUnitClassModifier(UnitClassTypes eIndex) const;
	void changeUnitClassModifier(UnitClassTypes eIndex, int iChange);

	std::pair<int, int> getYieldFromPillage(YieldTypes eYield) const;
	void changeYieldFromPillage(YieldTypes eYield, std::pair<int, int> change);

	bool canAcquirePromotion(PromotionTypes ePromotion) const;
	bool canAcquirePromotionAny() const;
	std::set<PromotionTypes> GetConditionalPromotions() const;
	bool IsPromotionBlocked(PromotionTypes eIndex) const;
	std::vector<PlagueInfo> GetPlaguesToInflict() const;
	void ModifyPlaguesToInflict(PlagueInfo sPlagueInfo, bool bAdd);
	bool arePromotionConditionsFulfilled(PromotionTypes eIndex) const;
	void SetPromotionBlocked(PromotionTypes eIndex, bool bNewValue);
	std::set<PromotionTypes> GetPromotionsWithSameAttackBonus() const;
	void SetPromotionWithSameAttackBonus(PromotionTypes eIndex, bool bNewValue);
	bool isPromotionValid(PromotionTypes ePromotion) const;
	bool isHasPromotion(PromotionTypes eIndex) const;
	void setHasPromotion(PromotionTypes eIndex, bool bNewValue);
	bool isPromotionActive(PromotionTypes eIndex) const;
	void setPromotionActive(PromotionTypes eIndex, bool bNewValue);

	int getSubUnitCount() const;
	int getSubUnitsAlive() const;
	int getSubUnitsAlive(int iDamage) const;

	//some units are always hostile, independent of war/peace, but only outside of cities ...
	bool isEnemy(TeamTypes eFromPerspectiveOfTeam, const CvPlot* pAssumedUnitPlot = NULL) const;

	bool canRangeStrike() const;
	bool canEverRangeStrikeAt(int iX, int iY, const CvPlot* pSourcePlot, bool bIgnoreVisibility) const;
	bool canEverRangeStrikeAt(int iX, int iY) const;
	bool canRangeStrikeAt(int iX, int iY, bool bNeedWar = true, bool bNoncombatAllowed = true) const;

#if defined(MOD_GLOBAL_STACKING_RULES)
	int getNumberStackingUnits() const;
	bool IsStackingUnit() const;
	bool IsCargoCombatUnit() const;
#endif

	bool IsAirSweepCapable() const;
	int GetAirSweepCapableCount() const;
	void ChangeAirSweepCapableCount(int iChange);

	bool canAirSweep() const;
	bool canAirSweepAt(int iX, int iY) const;
	bool airSweep(int iX, int iY);

	bool attemptGroundAttacks(const CvPlot& pPlot);

	bool isAlwaysHostile(const CvPlot& pPlot) const;
	void changeAlwaysHostileCount(int iValue);
	int getAlwaysHostileCount() const;

	int GetDamageAcceptableForConditionalPromotion() const;

	int getArmyID() const;
	void setArmyID(int iNewArmyID);
	CvString getTacticalZoneInfo() const;
	bool shouldHeal(bool bBeforeAttacks) const;

	void setTacticalMove(AITacticalMove eMove);
	AITacticalMove getTacticalMove(int* pTurnSet=NULL) const;
	bool canUseForAIOperation() const;
	bool canUseForTacticalAI() const;
	bool canUseNow() const;

	void SetTacticalAIPlot(CvPlot* pPlot);
	CvPlot* GetTacticalAIPlot() const;

	void LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction);

	int GetPower() const;

	bool CanSwapWithUnitHere(const CvPlot& swapPlot) const;
	CvUnit* GetPotentialUnitToSwapWith(const CvPlot& swapPlot) const;

	bool CanPushOutUnitHere(const CvPlot& pushPlot) const;
	CvUnit* GetPotentialUnitToPushOut(const CvPlot& pushPlot, CvPlot** ppToPlot=NULL) const;
	bool PushBlockingUnitOutOfPlot(const CvPlot& atPlot);

	bool CanStackUnitAtPlot(const CvPlot* pPlot) const;
	int CountStackingUnitsAtPlot(const CvPlot* pPlot) const;

	template<typename Unit, typename Visitor>
	static void Serialize(Unit& unit, Visitor& visitor);
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

	const CvSyncArchive<CvUnit>& getSyncArchive() const;
	CvSyncArchive<CvUnit>& getSyncArchive();

	// Mission routines
	void PushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = false, MissionAITypes eMissionAI = NO_MISSIONAI, CvPlot* pMissionAIPlot = NULL, CvUnit* pMissionAIUnit = NULL);
	void PopMission();
	void AutoMission();
	void UpdateMission();
	CvPlot* LastMissionPlot() const;
	bool CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot = NULL, bool bTestVisible = false);
	int GetMissionTimer() const;
	void SetMissionTimer(int iNewValue);
	void ChangeMissionTimer(int iChange);
	void ClearMissionQueue(bool bKeepPathCache = false, int iUnitCycleTimer = 1);
	int GetLengthMissionQueue() const;
	const MissionData* GetHeadMissionData() const;
	const MissionData* GetMissionData(int iIndex) const;
	CvPlot* GetMissionAIPlot() const;
	MissionAITypes GetMissionAIType() const;
	void SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit);
	CvUnit* GetMissionAIUnit();

	CvUnit* rangeStrikeTarget(const CvPlot& targetPlot, bool bNoncombatAllowed) const;

	bool IsCanAttackWithMove() const;
	bool IsCanAttackRanged() const;
	bool IsCanAttack() const;
	bool IsCanAttackWithMoveNow() const;
	bool IsCanDefend() const;
	bool IsCivilianUnit() const;
	bool IsCombatUnit() const;

	ReachablePlots GetAllPlotsInReachThisTurn(bool bCheckTerritory=true, bool bCheckZOC=true, bool bAllowEmbark=true, int iMinMovesLeft=0) const;
	vector<int> GetPlotsWithEnemyInMovementRange(bool bOnlyFortified = false, bool bOnlyCities = false, int iMaxPathLength=INT_MAX);

	// Path-finding routines
	bool GeneratePath(const CvPlot* pToPlot, int iFlags = 0, int iMaxTurns = INT_MAX, int* piPathTurns = NULL);

	// you must call GeneratePath with caching before using these methods!
	CvPlot* GetPathFirstPlot() const;
	CvPlot* GetPathLastPlot() const;
	CvPlot* GetPathEndFirstTurnPlot() const;
	int GetMovementPointsAtCachedTarget() const;
	CvPlot* GetLastValidDestinationPlotInCachedPath() const;
	const CvPathNodeArray& GetLastPath() const;
	bool CachedPathIsSafeForCivilian() const;

	bool IsEmbarkAllWater() const;
	void ChangeEmbarkAllWaterCount(int iValue);
	int GetEmbarkAllWaterCount() const;

	bool IsEmbarkDeepWater() const;
	void ChangeEmbarkDeepWaterCount(int iValue);
	int GetEmbarkDeepWaterCount() const;

	void ChangeEmbarkExtraVisibility(int iValue);
	int GetEmbarkExtraVisibility() const;

	void ChangeEmbarkDefensiveModifier(int iValue);
	int GetEmbarkDefensiveModifier() const;

	void ChangeCapitalDefenseModifier(int iValue);
	int GetCapitalDefenseModifier() const;
	void ChangeCapitalDefenseFalloff(int iValue);
	int GetCapitalDefenseFalloff() const;
	void ChangeCapitalDefenseLimit(int iValue);
	int GetCapitalDefenseLimit() const;

	void ChangeCityAttackPlunderModifier(int iValue);
	int GetCityAttackPlunderModifier() const;

	void ChangeReligiousStrengthLossRivalTerritory(int iValue);
	int GetReligiousStrengthLossRivalTerritory() const;

	void ChangeTradeMissionInfluenceModifier(int iValue);
	int GetTradeMissionInfluenceModifier() const;

	void ChangeTradeMissionGoldModifier(int iValue);
	int GetTradeMissionGoldModifier() const;

	void ChangeCombatModPerLevel(int iValue);
	int GetCombatModPerLevel() const;

    void ChangeDiploMissionInfluence(int iValue);
	int GetDiploMissionInfluence() const;

	bool IsHasBeenPromotedFromGoody() const;
	void SetBeenPromotedFromGoody(bool bBeenPromoted);

	bool IsHigherPopThan(const CvUnit* pOtherUnit) const;
	bool IsHigherTechThan(UnitTypes otherUnit) const;
	bool IsLargerCivThan(const CvUnit* pOtherUnit) const;

	int GetNumGoodyHutsPopped() const;
	void SetNumGoodyHutsPopped(int iValue);

	const CvUnitReligion* GetReligionData() const { return &m_Religion; }
	CvUnitReligion* GetReligionDataMutable() { return &m_Religion; }

	static void dispatchingNetMessage(bool dispatching);
	static bool dispatchingNetMessage();

	std::string debugDump(const FAutoVariableBase&) const;
	std::string stackTraceRemark(const FAutoVariableBase&) const;

	const char* GetMissionInfo();
	void DumpDangerInNeighborhood();

	void setHomelandMove(AIHomelandMove eMove);
	AIHomelandMove getHomelandMove(int* pTurnSet=NULL) const;
	bool hasCurrentTacticalMove() const;

	bool IsCivilization(CivilizationTypes iCivilizationType) const;
	bool HasPromotion(PromotionTypes iPromotionType) const;
	bool IsUnit(UnitTypes iUnitType) const;
	bool IsUnitClass(UnitClassTypes iUnitClassType) const;
	bool IsOnFeature(FeatureTypes iFeatureType) const;
	bool IsAdjacentToFeature(FeatureTypes iFeatureType) const;
	bool IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const;
	bool IsWithinDistanceOfUnit(UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitClass(UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitCombatType(UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfUnitPromotion(PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsWithinDistanceOfCity(int iDistance, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnit(UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitClass(UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitCombatType(UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const;
	bool IsAdjacentToUnitPromotion(PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const;
	bool IsOnImprovement(ImprovementTypes iImprovementType) const;
	bool IsAdjacentToImprovement(ImprovementTypes iImprovementType) const;
	bool IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const;
	bool IsOnPlotType(PlotTypes iPlotType) const;
	bool IsAdjacentToPlotType(PlotTypes iPlotType) const;
	bool IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const;
	bool IsOnResource(ResourceTypes iResourceType) const;
	bool IsAdjacentToResource(ResourceTypes iResourceType) const;
	bool IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const;
	bool IsOnTerrain(TerrainTypes iTerrainType) const;
	bool IsAdjacentToTerrain(TerrainTypes iTerrainType) const;
	bool IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const;

	int TurnsToReachTarget(const CvPlot* pTarget,int iFlags, int iTargetTurns);
	int TurnsToReachTarget(const CvPlot* pTarget, bool bIgnoreUnits = false, bool bIgnoreStacking = false, int iTargetTurns = MAX_INT);
	bool CanSafelyReachInXTurns(const CvPlot* pTarget, int iTurns);
	void ClearPathCache();

	bool	getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer = NO_PLAYER);
	static CvUnit* createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef, bool ForcedCapture = false);

	void DoGreatPersonSpawnBonus(CvCity* pSpawnCity);

protected:
	const MissionData* HeadMissionData() const;
	MissionData* HeadMissionData();

	void setMoves(int iNewValue);

	bool HaveCachedPathTo(const CvPlot* pToPlot, int iFlags) const;
	bool IsCachedPathValid() const;
	bool VerifyCachedPath(const CvPlot* pDestPlot, int iFlags, int iMaxTurns);
	//return -1 if impossible, turns to target otherwise (zero is valid!)
	int ComputePath(const CvPlot* pToPlot, int iFlags, int iMaxTurns, bool bCacheResult);

	bool HasQueuedVisualizationMoves() const;
	void QueueMoveForVisualization(CvPlot* pkPlot);
	void PublishQueuedVisualizationMoves();

	bool EmergencyRebase();
	bool CheckDOWNeededForMove(int iX, int iY, bool bPopup = true);
	MoveResult UnitAttackWithMove(int iX, int iY, int iFlags);
	int UnitPathTo(int iX, int iY, int iFlags);
	bool UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove = false);
	bool UnitRoadTo(int iX, int iY, int iFlags);
	bool UnitBuild(BuildTypes eBuild);

	SYNC_ARCHIVE_MEMBER(CvUnit)

	PlayerTypes m_eOwner;
	PlayerTypes m_eOriginalOwner;
	PlayerTypes m_eGiftedByPlayer;
	UnitTypes m_eUnitType;
	int m_iX;
	int m_iY;
	int m_iID;

	int m_iDamage;
	int m_iMoves;
	bool m_bIsLinked;
	bool m_bIsLinkedLeader;
	bool m_bIsGrouped;
	int m_iLinkedMaxMoves;
	UnitIdContainer m_LinkedUnitIDs;
	int m_iLinkedLeaderID;
	int m_iArmyId;
	int m_iBaseCombat;
	int m_iBaseRangedCombat;

	int m_iSquadNumber;
	int m_iSquadDestinationX;
	int m_iSquadDestinationY;
	int m_SquadEndMovementType;

	int m_iHotKeyNumber;
	int m_iDeployFromOperationTurn;
	int m_iLastMoveTurn;
	int m_iCycleOrder; // not serialized
	int m_iReconX;
	int m_iReconY;
	int m_iReconCount;
	int m_iGameTurnCreated;
	int m_iTurnSliceCreated; // not serialized
	bool m_bImmobile;
	int m_iExperienceTimes100;
	int m_iLevel;
	int m_iCargo;
	int m_iCargoCapacity;
	int m_iAttackPlotX;
	int m_iAttackPlotY;
	int m_iCombatTimer;
	bool m_bMovedThisTurn;
	bool m_bHasWithdrawnThisTurn;
	bool m_bFortified;
	int m_iBlitzCount;
	int m_iAmphibCount;
	int m_iRiverCrossingNoPenaltyCount;
	int m_iEnemyRouteCount;
	int m_iRivalTerritoryCount;
	int m_iIsSlowInEnemyLandCount;
	int m_iRangeAttackIgnoreLOSCount;
    int m_iSeeThrough;
	int m_iCityAttackOnlyCount;
	int m_iCaptureDefeatedEnemyCount;
	int m_iOriginCity;
	int m_iCannotBeCapturedCount;
	int m_iForcedDamage;
	int m_iChangeDamage;
	int m_iDamageTakenMod;
	int m_iInfluenceFromCombatXPTimes100;
	std::map<PromotionTypes, int> m_PromotionDuration;
	std::map<PromotionTypes, int> m_TurnPromotionGained;
	int m_iRangedSupportFireCount;
	int m_iAlwaysHealCount;
	int m_iHealOutsideFriendlyCount;
	int m_iRiverDoubleMoveCount;
	int m_iEmbarkFlatCostCount;
	int m_iDisembarkFlatCostCount;
	int m_iAOEDamageOnKill;
	int m_iAOEDamageOnPillage;
	int m_iAOEHealOnPillage;
	int m_iCombatModPerCSAlliance;
	std::set<PromotionTypes> m_sePromotionsWithSameAttackBonus;
	int m_iStrengthThisTurnFromPreviousSamePromotionAttacks;
	int m_iAoEDamageOnMove;
	std::set<PromotionTypes> m_seBlockedPromotions;
	std::set<PromotionTypes> m_seConditionalPromotions;
	std::vector<PlagueInfo> m_vsPlaguesToInflict;
	int m_iPartialHealOnPillage;
	int m_iSplashDamage;
	int m_iMultiAttackBonus;
	int m_iLandAirDefenseValue;
	int m_iExtraVisibilityRange;
	int m_iExtraReconRange;
	int m_iExtraMoves;
	int m_iExtraMoveDiscount;
	int m_iExtraRange;
	int m_iInterceptChance;
	int m_iExtraEvasion;
	int m_iExtraWithdrawal;
	ContractTypes m_eUnitContract;
	int m_iNegatorPromotion;
	bool m_bIsNoMaintenance;
	int m_iExtraEnemyHeal;
	int m_iExtraNeutralHeal;
	int m_iExtraFriendlyHeal;
	int m_iEnemyDamageChance;
	int m_iNeutralDamageChance;
	int m_iEnemyDamage;
	int m_iNeutralDamage;
	int m_iNearbyEnemyCombatMod;
	int m_iNearbyEnemyCombatRange;
	int m_iSameTileHeal;
	int m_iAdjacentTileHeal;
	int m_iAdjacentModifier;
	int m_iNoAdjacentUnitModifier;
	int m_iRangedAttackModifier;
	int m_iInterceptionCombatModifier;
	int m_iInterceptionDefenseDamageModifier;
	int m_iAirSweepCombatModifier;
	int m_iAttackModifier;
	int m_iDefenseModifier;
	int m_iGroundAttackDamage;
	int m_iExtraCombatPercent;
	int m_iExtraCityAttackPercent;
	int m_iExtraCityDefensePercent;
	int m_iExtraRangedDefenseModifier;
	int m_iExtraHillsAttackPercent;
	int m_iExtraHillsDefensePercent;
	int m_iExtraOpenAttackPercent;
	int m_iExtraOpenRangedAttackMod;
	int m_iExtraRoughAttackPercent;
	int m_iExtraRoughRangedAttackMod;
	int m_iExtraAttackFortifiedMod;
	int m_iExtraAttackWoundedMod;
	int m_iExtraFullyHealedMod;
	int m_iExtraAttackAboveHealthMod;
	int m_iExtraAttackBelowHealthMod;
	int m_iRangedFlankAttack;
	int m_iFlankPower;
	int m_iFlankAttackModifier;
	int m_iExtraOpenDefensePercent;
	int m_iExtraRoughDefensePercent;
	int m_iExtraOpenFromPercent;
	int m_iExtraRoughFromPercent;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iDropRange;
	int m_iAirSweepCapableCount;
	int m_iExtraNavalMoves;
	DirectionTypes m_eFacingDirection;
	int m_iIgnoreTerrainCostCount;
	int m_iIgnoreTerrainDamageCount;
	int m_iIgnoreFeatureDamageCount;
	int m_iExtraTerrainDamageCount;
	int m_iExtraFeatureDamageCount;
	int m_iCannotHealCount;
	int m_iPillageFortificationsOnKillCount;
	int m_iNearbyImprovementCombatBonus;
	int m_iNearbyImprovementBonusRange;
	ImprovementTypes m_eCombatBonusImprovement;
	int m_iNearbyUnitClassBonus;
	int m_iNearbyUnitClassBonusRange;
	UnitClassTypes m_iCombatBonusFromNearbyUnitClass;
	int m_iNearbyPromotion;
	int m_iNearbyUnitPromotionRange;
	int m_iNearbyCityCombatMod;
	int m_iNearbyFriendlyCityCombatMod;
	int m_iNearbyEnemyCityCombatMod;
	int m_iPillageBonusStrengthPercent;
	int m_iStackedGreatGeneralExperience;
	int m_iWonderProductionModifier;
	int m_iUnitProductionModifier;
	int m_iTileDamageIfNotMoved;
	int m_iFortifiedModifier;
	int m_iNearbyEnemyDamage;
	int m_iAdjacentCityDefenseMod;
	int m_iGGGAXPPercent;
	int m_iGiveCombatMod;
	int m_iGiveHPIfEnemyKilled;
	int m_iGiveExperiencePercent;
	int m_iGiveOutsideFriendlyLandsModifier;
	int m_eGiveDomain;
	int m_iGiveExtraAttacks;
	int m_iGiveDefenseMod;
	int m_iGiveInvisibility;
	bool m_bGiveOnlyOnStartingTurn;
	int m_iConvertUnit;
	int m_eConvertDomain;
	UnitTypes m_eConvertDomainUnit;
	int m_iConvertEnemyUnitToBarbarian;
	bool m_bConvertOnFullHP;
	bool m_bConvertOnDamage;
	int m_iDamageThreshold;
	UnitTypes m_eConvertDamageOrFullHPUnit;
	int m_iNumberOfCultureBombs;
	int m_iNearbyHealEnemyTerritory;
	int m_iNearbyHealNeutralTerritory;
	int m_iNearbyHealFriendlyTerritory;
	int m_iCanCrossMountainsCount;
	int m_iCanCrossOceansCount;
	int m_iCanCrossIceCount;
	int m_iNumTilesRevealedThisTurn;
	bool m_bSpottedEnemy;
	bool m_bSpottedRuin;
	int m_iGainsXPFromScouting;
	int m_iXPFromPillaging;
	int m_iExtraXPOnKill;
	int m_iGainsXPFromSpotting;
	int m_iCaptureDefeatedEnemyChance;
	int m_iBarbCombatBonus;
	int m_iAdjacentEnemySapMovement;
	int m_iGGFromBarbariansCount;
	int m_iAuraRangeChange;
	int m_iAuraEffectChange;
	int m_iNumRepairCharges;
	int m_iMilitaryCapChange;
	int m_iRoughTerrainEndsTurnCount;
	int m_iCapturedUnitsConscriptedCount;
	int m_iEmbarkAbilityCount;
	int m_iHoveringUnitCount;
	int m_iFlatMovementCostCount;
	int m_iCanMoveImpassableCount;
	int m_iOnlyDefensiveCount;
	int m_iNoAttackInOceanCount;
	int m_iNoDefensiveBonusCount;
	int m_iNoCaptureCount;
	int m_iNukeImmuneCount;
	int m_iHiddenNationalityCount;
	int m_iAlwaysHostileCount;
	int m_iNoRevealMapCount;
	int m_iCanMoveAllTerrainCount;
	int m_iCanMoveAfterAttackingCount;
	int m_iFreePillageMoveCount;
	int m_iHealOnPillageCount;
	int m_iHPHealedIfDefeatEnemy;
	int m_iGoldenAgeValueFromKills;
	int m_iTacticalAIPlotX;
	int m_iTacticalAIPlotY;
	int m_iGarrisonCityID;
	int m_iNumAttacks;
	int m_iAttacksMade;
	int m_iGreatGeneralCount;
	int m_iGreatAdmiralCount;
	int m_iGreatGeneralModifier;
	int m_iGreatGeneralReceivesMovementCount;
	int m_iGreatGeneralCombatModifier;
	int m_iIgnoreGreatGeneralBenefit;
	int m_iIgnoreZOC;
	int m_iNoSupply;
	int m_iMaxHitPointsBase;
	int m_iMaxHitPointsChange;
	int m_iMaxHitPointsModifier;
	int m_iVsUnhappyMod;
	int m_iFriendlyLandsModifier;
	int m_iFriendlyLandsAttackModifier;
	int m_iOutsideFriendlyLandsModifier;
	int m_iBorderCombatModifier;
	int m_iCombatStrengthModifierPerMarriage;
	int m_iCombatStrengthModifierPerMarriageCap;
	int m_iHealIfDefeatExcludeBarbariansCount;
	int m_iNumInterceptions;
	int m_iExtraAirInterceptRange;
	int m_iMadeInterceptionCount;
	int m_iEverSelectedCount;
	int m_iSapperCount;
	int m_iCanHeavyCharge;
	int m_iStrongerDamaged;
	int m_iDiplomaticMissionAccomplishment;
	int m_iFightWellDamaged;
	int m_iFreeAttackMoves;
	int m_iCanMoraleBreak;
	int m_iDamageAoEFortified;
	int m_iWorkRateMod;
	int m_iDamageReductionCityAssault;
	int m_iGoodyHutYieldBonus;
	int m_iReligiousPressureModifier;
	int m_iNumExoticGoods;
	bool m_bPromotionReady;
	bool m_bDeathDelay;
	bool m_bCombatFocus;
	bool m_bInfoBarDirty;
	bool m_bNotConverting;
	bool m_bAirCombat;
	bool m_bEmbarked;
	bool m_bPromotedFromGoody;
	bool m_bAITurnProcessed;
	int m_iDamageTakenThisTurn;
	int m_iDamageTakenLastTurn;

	PlayerTypes m_eCapturingPlayer;
	bool m_bCapturedAsIs;
	bool m_bCapturedAsConscript;
	UnitTypes m_eLeaderUnitType;
	InvisibleTypes m_eInvisibleType;
	InvisibleTypes m_eSeeInvisibleType;
	GreatPeopleDirectiveTypes m_eGreatPeopleDirectiveType;
	CvUnitEntry* m_pUnitInfo;

	bool m_bWaitingForMove; ///< If true, the unit is busy visualizing its move.

	IDInfo m_combatUnit;
	IDInfo m_combatCity;
	IDInfo m_transportUnit;

	std::vector<int> m_extraDomainModifiers;
	std::vector<int> m_extraDomainAttacks;
	std::vector<int> m_extraDomainDefenses;
	std::vector<int> m_YieldModifier;
	std::vector<int> m_YieldChange;
	std::vector<int> m_aiYieldFromCombatExperienceTimes100;
	std::vector<int> m_iGarrisonYieldChange;
	std::vector<int> m_iFortificationYieldChange;

	CvString m_strScriptData;
	int m_iScenarioData;

	CvUnitPromotions m_Promotions;
	CvUnitReligion m_Religion;

	int m_iBuilderStrength;

	TerrainTypeCounter m_ignoreTerrainCostInCount;
	TerrainTypeCounter m_ignoreTerrainCostFromCount;
	FeatureTypeCounter m_ignoreFeatureCostInCount;
	FeatureTypeCounter m_ignoreFeatureCostFromCount;

	TerrainTypeCounter m_terrainDoubleMoveCount;
	FeatureTypeCounter m_featureDoubleMoveCount;

	TerrainTypeCounter m_terrainHalfMoveCount;
	FeatureTypeCounter m_featureHalfMoveCount;

	TerrainTypeCounter m_terrainExtraMoveCount;
	FeatureTypeCounter m_featureExtraMoveCount;

	TerrainTypeCounter m_terrainDoubleHeal;
	FeatureTypeCounter m_featureDoubleHeal;
	TerrainTypeCounter m_terrainImpassableCount;
	FeatureTypeCounter m_featureImpassableCount;
	TerrainTypeCounter m_extraTerrainAttackPercent;
	TerrainTypeCounter m_extraTerrainDefensePercent;
	TerrainTypeCounter m_vTerrainModifierAttack;
	TerrainTypeCounter m_vTerrainModifierDefense;
	FeatureTypeCounter m_extraFeatureAttackPercent;
	FeatureTypeCounter m_extraFeatureDefensePercent;

	UnitClassCounter m_extraUnitClassAttackMod;
	UnitClassCounter m_extraUnitClassDefenseMod;
	std::vector<int> m_aiNumTimesAttackedThisTurn;
	std::vector<int> m_yieldFromScouting;
	std::vector<int> m_piYieldFromAncientRuins;
	std::vector<int> m_piYieldFromTRPlunder;
	std::vector<int> m_yieldFromKills;
	std::vector<int> m_yieldFromBarbarianKills;
	std::vector<int> m_extraUnitCombatModifier;
	std::vector<int> m_extraUnitCombatModifierAttack;
	std::vector<int> m_extraUnitCombatModifierDefense;
	std::vector<int> m_unitClassModifier;
	std::vector<int> m_iCombatModPerAdjacentUnitCombatModifier;
	std::vector<int> m_iCombatModPerAdjacentUnitCombatAttackMod;
	std::vector<int> m_iCombatModPerAdjacentUnitCombatDefenseMod;
	std::map<int, std::pair<int, int>> m_yieldFromPillage;
	int m_iMissionTimer;
	int m_iMissionAIX;
	int m_iMissionAIY;
	MissionAITypes m_eMissionAIType;
	IDInfo m_missionAIUnit;
	ActivityTypes m_eActivityType;
	AutomateTypes m_eAutomateType;
	UnitAITypes m_eUnitAIType; //current AI type, might be different from default
	UnitCombatTypes m_eCombatType;

	//not serialized
	std::vector<CvPlot*> m_unitMoveLocs;

	int m_iEmbarkedAllWaterCount;
	int m_iEmbarkedDeepWaterCount;
	int m_iEmbarkExtraVisibility;
	int m_iEmbarkDefensiveModifier;
	int m_iCapitalDefenseModifier;
	int m_iCapitalDefenseFalloff;
	int m_iCapitalDefenseLimit;
	int m_iCityAttackPlunderModifier;
	int m_iReligiousStrengthLossRivalTerritory;
	int m_iTradeMissionInfluenceModifier;
	int m_iTradeMissionGoldModifier;
	int m_iCombatModPerLevel;
	int m_iDiploMissionInfluence;
	int m_iMapLayer;		// Which layer does the unit reside on for pathing/stacking/etc.
	int m_iNumGoodyHutsPopped;
	int m_iTourismBlastStrength;
	int m_iTourismBlastLength;
	int m_iHurryStrength;
	int m_iGoldBlastStrength;
	int m_iScienceBlastStrength;
	int m_iCultureBlastStrength;
	int m_iGAPBlastStrength;
	std::vector<bool> m_abPromotionEverObtained;

	CvString m_strUnitName;
	CvString m_strName;
	CvString m_strGreatName;
	GreatWorkType m_eGreatWork;

	//this is always stored with the zero-counting convention
	//ie every plot we can reach this turn has turn count 0, even if there are no moves left
	mutable CvPathNodeArray m_kLastPath;
	mutable uint m_uiLastPathCacheOrigin;
	mutable uint m_uiLastPathCacheDestination;
	mutable uint m_uiLastPathFlags;
	mutable uint m_uiLastPathTurnSlice;

	bool canAdvance(const CvPlot& pPlot, int iThreshold) const;

	void DoPlagueTransfer(CvUnit& defender, bool bAttacking);
#if defined(MOD_CARGO_SHIPS)
	void DoCargoPromotions(CvUnit& cargounit);
	void RemoveCargoPromotions(CvUnit& cargounit);
#endif

	int  GetNumFallBackPlotsAvailable(const CvUnit& pAttacker) const;
	int  GetWithdrawChance(const CvUnit& pAttacker) const;
	bool CheckWithdrawal(const CvUnit& pAttacker) const;
	bool DoFallBack(const CvUnit& pAttacker, bool bWithdraw = false, bool bCaptured = false);

private:

	mutable MissionQueue m_missionQueue;

	// for debugging
	CvString m_strMissionInfoString;
	AITacticalMove m_eTacticalMove;
	int m_iTacticalMoveSetTurn;
	AIHomelandMove m_eHomelandMove;
	int m_iHomelandMoveSetTurn;

	friend class CvLuaUnit;
};

FDataStream& operator<<(FDataStream&, const CvUnit&);
FDataStream& operator>>(FDataStream&, CvUnit&);

namespace FSerialization
{
void SyncUnits();
void ClearUnitDeltas();
}

SYNC_ARCHIVE_BEGIN(CvUnit)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eOwner)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eOriginalOwner)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eGiftedByPlayer)
SYNC_ARCHIVE_VAR(UnitTypes, m_eUnitType)
SYNC_ARCHIVE_VAR(int, m_iX)
SYNC_ARCHIVE_VAR(int, m_iY)
SYNC_ARCHIVE_VAR(int, m_iID)
SYNC_ARCHIVE_VAR(int, m_iDamage)
SYNC_ARCHIVE_VAR(int, m_iMoves)
SYNC_ARCHIVE_VAR(bool, m_bIsLinked)
SYNC_ARCHIVE_VAR(bool, m_bIsLinkedLeader)
SYNC_ARCHIVE_VAR(bool, m_bIsGrouped)
SYNC_ARCHIVE_VAR(int, m_iLinkedMaxMoves)
SYNC_ARCHIVE_VAR(UnitIdContainer, m_LinkedUnitIDs)
SYNC_ARCHIVE_VAR(int, m_iLinkedLeaderID)
SYNC_ARCHIVE_VAR(int, m_iArmyId)
SYNC_ARCHIVE_VAR(int, m_iBaseCombat)
SYNC_ARCHIVE_VAR(int, m_iBaseRangedCombat)
SYNC_ARCHIVE_VAR(int, m_iHotKeyNumber)
SYNC_ARCHIVE_VAR(int, m_iDeployFromOperationTurn)
SYNC_ARCHIVE_VAR(int, m_iLastMoveTurn)
SYNC_ARCHIVE_VAR(int, m_iReconX)
SYNC_ARCHIVE_VAR(int, m_iReconY)
SYNC_ARCHIVE_VAR(int, m_iReconCount)
SYNC_ARCHIVE_VAR(int, m_iGameTurnCreated)
SYNC_ARCHIVE_VAR(bool, m_bImmobile)
SYNC_ARCHIVE_VAR(int, m_iExperienceTimes100)
SYNC_ARCHIVE_VAR(int, m_iLevel)
SYNC_ARCHIVE_VAR(int, m_iCargo)
SYNC_ARCHIVE_VAR(int, m_iCargoCapacity)
SYNC_ARCHIVE_VAR(int, m_iAttackPlotX)
SYNC_ARCHIVE_VAR(int, m_iAttackPlotY)
SYNC_ARCHIVE_VAR(int, m_iCombatTimer)
SYNC_ARCHIVE_VAR(bool, m_bMovedThisTurn)
SYNC_ARCHIVE_VAR(bool, m_bHasWithdrawnThisTurn)
SYNC_ARCHIVE_VAR(bool, m_bFortified)
SYNC_ARCHIVE_VAR(int, m_iBlitzCount)
SYNC_ARCHIVE_VAR(int, m_iAmphibCount)
SYNC_ARCHIVE_VAR(int, m_iRiverCrossingNoPenaltyCount)
SYNC_ARCHIVE_VAR(int, m_iEnemyRouteCount)
SYNC_ARCHIVE_VAR(int, m_iRivalTerritoryCount)
SYNC_ARCHIVE_VAR(int, m_iIsSlowInEnemyLandCount)
SYNC_ARCHIVE_VAR(int, m_iRangeAttackIgnoreLOSCount)
SYNC_ARCHIVE_VAR(int, m_iSeeThrough)
SYNC_ARCHIVE_VAR(int, m_iCityAttackOnlyCount)
SYNC_ARCHIVE_VAR(int, m_iCaptureDefeatedEnemyCount)
SYNC_ARCHIVE_VAR(int, m_iOriginCity)
SYNC_ARCHIVE_VAR(int, m_iCannotBeCapturedCount)
SYNC_ARCHIVE_VAR(int, m_iForcedDamage)
SYNC_ARCHIVE_VAR(int, m_iChangeDamage)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenMod)
SYNC_ARCHIVE_VAR(int, m_iInfluenceFromCombatXPTimes100)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<PromotionTypes, int>), m_PromotionDuration)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<PromotionTypes, int>), m_TurnPromotionGained)
SYNC_ARCHIVE_VAR(int, m_iRangedSupportFireCount)
SYNC_ARCHIVE_VAR(int, m_iAlwaysHealCount)
SYNC_ARCHIVE_VAR(int, m_iHealOutsideFriendlyCount)
SYNC_ARCHIVE_VAR(int, m_iRiverDoubleMoveCount)
SYNC_ARCHIVE_VAR(int, m_iEmbarkFlatCostCount)
SYNC_ARCHIVE_VAR(int, m_iDisembarkFlatCostCount)
SYNC_ARCHIVE_VAR(int, m_iAOEDamageOnKill)
SYNC_ARCHIVE_VAR(int, m_iAOEDamageOnPillage)
SYNC_ARCHIVE_VAR(int, m_iAOEHealOnPillage)
SYNC_ARCHIVE_VAR(int, m_iCombatModPerCSAlliance)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::set<PromotionTypes>), m_sePromotionsWithSameAttackBonus)
SYNC_ARCHIVE_VAR(int, m_iStrengthThisTurnFromPreviousSamePromotionAttacks)
SYNC_ARCHIVE_VAR(int, m_iAoEDamageOnMove)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::set<PromotionTypes>), m_seBlockedPromotions)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::set<PromotionTypes>), m_seConditionalPromotions)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::vector<PlagueInfo>), m_vsPlaguesToInflict)
SYNC_ARCHIVE_VAR(int, m_iPartialHealOnPillage)
SYNC_ARCHIVE_VAR(int, m_iSplashDamage)
SYNC_ARCHIVE_VAR(int, m_iMultiAttackBonus)
SYNC_ARCHIVE_VAR(int, m_iLandAirDefenseValue)
SYNC_ARCHIVE_VAR(int, m_iExtraVisibilityRange)
SYNC_ARCHIVE_VAR(int, m_iExtraReconRange)
SYNC_ARCHIVE_VAR(int, m_iExtraMoves)
SYNC_ARCHIVE_VAR(int, m_iExtraMoveDiscount)
SYNC_ARCHIVE_VAR(int, m_iExtraRange)
SYNC_ARCHIVE_VAR(int, m_iInterceptChance)
SYNC_ARCHIVE_VAR(int, m_iExtraEvasion)
SYNC_ARCHIVE_VAR(int, m_iExtraWithdrawal)
SYNC_ARCHIVE_VAR(ContractTypes, m_eUnitContract)
SYNC_ARCHIVE_VAR(int, m_iNegatorPromotion)
SYNC_ARCHIVE_VAR(bool, m_bIsNoMaintenance)
SYNC_ARCHIVE_VAR(int, m_iExtraEnemyHeal)
SYNC_ARCHIVE_VAR(int, m_iExtraNeutralHeal)
SYNC_ARCHIVE_VAR(int, m_iExtraFriendlyHeal)
SYNC_ARCHIVE_VAR(int, m_iEnemyDamageChance)
SYNC_ARCHIVE_VAR(int, m_iNeutralDamageChance)
SYNC_ARCHIVE_VAR(int, m_iEnemyDamage)
SYNC_ARCHIVE_VAR(int, m_iNeutralDamage)
SYNC_ARCHIVE_VAR(int, m_iNearbyEnemyCombatMod)
SYNC_ARCHIVE_VAR(int, m_iNearbyEnemyCombatRange)
SYNC_ARCHIVE_VAR(int, m_iSameTileHeal)
SYNC_ARCHIVE_VAR(int, m_iAdjacentTileHeal)
SYNC_ARCHIVE_VAR(int, m_iAdjacentModifier)
SYNC_ARCHIVE_VAR(int, m_iNoAdjacentUnitModifier)
SYNC_ARCHIVE_VAR(int, m_iRangedAttackModifier)
SYNC_ARCHIVE_VAR(int, m_iInterceptionCombatModifier)
SYNC_ARCHIVE_VAR(int, m_iInterceptionDefenseDamageModifier)
SYNC_ARCHIVE_VAR(int, m_iAirSweepCombatModifier)
SYNC_ARCHIVE_VAR(int, m_iAttackModifier)
SYNC_ARCHIVE_VAR(int, m_iDefenseModifier)
SYNC_ARCHIVE_VAR(int, m_iGroundAttackDamage)
SYNC_ARCHIVE_VAR(int, m_iExtraCombatPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraCityAttackPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraCityDefensePercent)
SYNC_ARCHIVE_VAR(int, m_iExtraRangedDefenseModifier)
SYNC_ARCHIVE_VAR(int, m_iExtraHillsAttackPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraHillsDefensePercent)
SYNC_ARCHIVE_VAR(int, m_iExtraOpenAttackPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraOpenRangedAttackMod)
SYNC_ARCHIVE_VAR(int, m_iExtraRoughAttackPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraRoughRangedAttackMod)
SYNC_ARCHIVE_VAR(int, m_iExtraAttackFortifiedMod)
SYNC_ARCHIVE_VAR(int, m_iExtraAttackWoundedMod)
SYNC_ARCHIVE_VAR(int, m_iExtraFullyHealedMod)
SYNC_ARCHIVE_VAR(int, m_iExtraAttackAboveHealthMod)
SYNC_ARCHIVE_VAR(int, m_iExtraAttackBelowHealthMod)
SYNC_ARCHIVE_VAR(int, m_iRangedFlankAttack)
SYNC_ARCHIVE_VAR(int, m_iFlankPower)
SYNC_ARCHIVE_VAR(int, m_iFlankAttackModifier)
SYNC_ARCHIVE_VAR(int, m_iExtraOpenDefensePercent)
SYNC_ARCHIVE_VAR(int, m_iExtraRoughDefensePercent)
SYNC_ARCHIVE_VAR(int, m_iExtraOpenFromPercent)
SYNC_ARCHIVE_VAR(int, m_iExtraRoughFromPercent)
SYNC_ARCHIVE_VAR(int, m_iPillageChange)
SYNC_ARCHIVE_VAR(int, m_iUpgradeDiscount)
SYNC_ARCHIVE_VAR(int, m_iExperiencePercent)
SYNC_ARCHIVE_VAR(int, m_iDropRange)
SYNC_ARCHIVE_VAR(int, m_iAirSweepCapableCount)
SYNC_ARCHIVE_VAR(int, m_iExtraNavalMoves)
SYNC_ARCHIVE_VAR(DirectionTypes, m_eFacingDirection)
SYNC_ARCHIVE_VAR(int, m_iIgnoreTerrainCostCount)
SYNC_ARCHIVE_VAR(int, m_iIgnoreTerrainDamageCount)
SYNC_ARCHIVE_VAR(int, m_iIgnoreFeatureDamageCount)
SYNC_ARCHIVE_VAR(int, m_iExtraTerrainDamageCount)
SYNC_ARCHIVE_VAR(int, m_iExtraFeatureDamageCount)
SYNC_ARCHIVE_VAR(int, m_iCannotHealCount)
SYNC_ARCHIVE_VAR(int, m_iPillageFortificationsOnKillCount)
SYNC_ARCHIVE_VAR(int, m_iNearbyImprovementCombatBonus)
SYNC_ARCHIVE_VAR(int, m_iNearbyImprovementBonusRange)
SYNC_ARCHIVE_VAR(ImprovementTypes, m_eCombatBonusImprovement)
SYNC_ARCHIVE_VAR(int, m_iNearbyUnitClassBonus)
SYNC_ARCHIVE_VAR(int, m_iNearbyUnitClassBonusRange)
SYNC_ARCHIVE_VAR(UnitClassTypes, m_iCombatBonusFromNearbyUnitClass)
SYNC_ARCHIVE_VAR(int, m_iNearbyPromotion)
SYNC_ARCHIVE_VAR(int, m_iNearbyUnitPromotionRange)
SYNC_ARCHIVE_VAR(int, m_iNearbyCityCombatMod)
SYNC_ARCHIVE_VAR(int, m_iNearbyFriendlyCityCombatMod)
SYNC_ARCHIVE_VAR(int, m_iNearbyEnemyCityCombatMod)
SYNC_ARCHIVE_VAR(int, m_iPillageBonusStrengthPercent)
SYNC_ARCHIVE_VAR(int, m_iStackedGreatGeneralExperience)
SYNC_ARCHIVE_VAR(int, m_iWonderProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iUnitProductionModifier)
SYNC_ARCHIVE_VAR(int, m_iTileDamageIfNotMoved)
SYNC_ARCHIVE_VAR(int, m_iFortifiedModifier)
SYNC_ARCHIVE_VAR(int, m_iNearbyEnemyDamage)
SYNC_ARCHIVE_VAR(int, m_iAdjacentCityDefenseMod)
SYNC_ARCHIVE_VAR(int, m_iGGGAXPPercent)
SYNC_ARCHIVE_VAR(int, m_iGiveCombatMod)
SYNC_ARCHIVE_VAR(int, m_iGiveHPIfEnemyKilled)
SYNC_ARCHIVE_VAR(int, m_iGiveExperiencePercent)
SYNC_ARCHIVE_VAR(int, m_iGiveOutsideFriendlyLandsModifier)
SYNC_ARCHIVE_VAR(int, m_eGiveDomain)
SYNC_ARCHIVE_VAR(int, m_iGiveExtraAttacks)
SYNC_ARCHIVE_VAR(int, m_iGiveDefenseMod)
SYNC_ARCHIVE_VAR(int, m_iGiveInvisibility)
SYNC_ARCHIVE_VAR(bool, m_bGiveOnlyOnStartingTurn)
SYNC_ARCHIVE_VAR(int, m_iConvertUnit)
SYNC_ARCHIVE_VAR(int, m_eConvertDomain)
SYNC_ARCHIVE_VAR(UnitTypes, m_eConvertDomainUnit)
SYNC_ARCHIVE_VAR(int, m_iConvertEnemyUnitToBarbarian)
SYNC_ARCHIVE_VAR(bool, m_bConvertOnFullHP)
SYNC_ARCHIVE_VAR(bool, m_bConvertOnDamage)
SYNC_ARCHIVE_VAR(int, m_iDamageThreshold)
SYNC_ARCHIVE_VAR(UnitTypes, m_eConvertDamageOrFullHPUnit)
SYNC_ARCHIVE_VAR(int, m_iNumberOfCultureBombs)
SYNC_ARCHIVE_VAR(int, m_iNearbyHealEnemyTerritory)
SYNC_ARCHIVE_VAR(int, m_iNearbyHealNeutralTerritory)
SYNC_ARCHIVE_VAR(int, m_iNearbyHealFriendlyTerritory)
SYNC_ARCHIVE_VAR(int, m_iCanCrossMountainsCount)
SYNC_ARCHIVE_VAR(int, m_iCanCrossOceansCount)
SYNC_ARCHIVE_VAR(int, m_iCanCrossIceCount)
SYNC_ARCHIVE_VAR(int, m_iNumTilesRevealedThisTurn)
SYNC_ARCHIVE_VAR(bool, m_bSpottedEnemy)
SYNC_ARCHIVE_VAR(int, m_iGainsXPFromScouting)
SYNC_ARCHIVE_VAR(int, m_iXPFromPillaging)
SYNC_ARCHIVE_VAR(int, m_iExtraXPOnKill)
SYNC_ARCHIVE_VAR(int, m_iGainsXPFromSpotting)
SYNC_ARCHIVE_VAR(int, m_iCaptureDefeatedEnemyChance)
SYNC_ARCHIVE_VAR(int, m_iBarbCombatBonus)
SYNC_ARCHIVE_VAR(int, m_iAdjacentEnemySapMovement)
SYNC_ARCHIVE_VAR(int, m_iGGFromBarbariansCount)
SYNC_ARCHIVE_VAR(int, m_iAuraRangeChange)
SYNC_ARCHIVE_VAR(int, m_iAuraEffectChange)
SYNC_ARCHIVE_VAR(int, m_iNumRepairCharges)
SYNC_ARCHIVE_VAR(int, m_iMilitaryCapChange)
SYNC_ARCHIVE_VAR(int, m_iRoughTerrainEndsTurnCount)
SYNC_ARCHIVE_VAR(int, m_iCapturedUnitsConscriptedCount)
SYNC_ARCHIVE_VAR(int, m_iEmbarkAbilityCount)
SYNC_ARCHIVE_VAR(int, m_iHoveringUnitCount)
SYNC_ARCHIVE_VAR(int, m_iFlatMovementCostCount)
SYNC_ARCHIVE_VAR(int, m_iCanMoveImpassableCount)
SYNC_ARCHIVE_VAR(int, m_iOnlyDefensiveCount)
SYNC_ARCHIVE_VAR(int, m_iNoAttackInOceanCount)
SYNC_ARCHIVE_VAR(int, m_iNoDefensiveBonusCount)
SYNC_ARCHIVE_VAR(int, m_iNoCaptureCount)
SYNC_ARCHIVE_VAR(int, m_iNukeImmuneCount)
SYNC_ARCHIVE_VAR(int, m_iHiddenNationalityCount)
SYNC_ARCHIVE_VAR(int, m_iAlwaysHostileCount)
SYNC_ARCHIVE_VAR(int, m_iNoRevealMapCount)
SYNC_ARCHIVE_VAR(int, m_iCanMoveAllTerrainCount)
SYNC_ARCHIVE_VAR(int, m_iCanMoveAfterAttackingCount)
SYNC_ARCHIVE_VAR(int, m_iFreePillageMoveCount)
SYNC_ARCHIVE_VAR(int, m_iHealOnPillageCount)
SYNC_ARCHIVE_VAR(int, m_iHPHealedIfDefeatEnemy)
SYNC_ARCHIVE_VAR(int, m_iGoldenAgeValueFromKills)
SYNC_ARCHIVE_VAR(int, m_iTacticalAIPlotX)
SYNC_ARCHIVE_VAR(int, m_iTacticalAIPlotY)
SYNC_ARCHIVE_VAR(int, m_iGarrisonCityID)
SYNC_ARCHIVE_VAR(int, m_iNumAttacks)
SYNC_ARCHIVE_VAR(int, m_iAttacksMade)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralCount)
SYNC_ARCHIVE_VAR(int, m_iGreatAdmiralCount)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralModifier)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralReceivesMovementCount)
SYNC_ARCHIVE_VAR(int, m_iGreatGeneralCombatModifier)
SYNC_ARCHIVE_VAR(int, m_iIgnoreGreatGeneralBenefit)
SYNC_ARCHIVE_VAR(int, m_iIgnoreZOC)
SYNC_ARCHIVE_VAR(int, m_iNoSupply)
SYNC_ARCHIVE_VAR(int, m_iMaxHitPointsChange)
SYNC_ARCHIVE_VAR(int, m_iMaxHitPointsModifier)
SYNC_ARCHIVE_VAR(int, m_iVsUnhappyMod)
SYNC_ARCHIVE_VAR(int, m_iFriendlyLandsModifier)
SYNC_ARCHIVE_VAR(int, m_iBorderCombatModifier)
SYNC_ARCHIVE_VAR(int, m_iCombatStrengthModifierPerMarriage)
SYNC_ARCHIVE_VAR(int, m_iCombatStrengthModifierPerMarriageCap)
SYNC_ARCHIVE_VAR(int, m_iFriendlyLandsAttackModifier)
SYNC_ARCHIVE_VAR(int, m_iOutsideFriendlyLandsModifier)
SYNC_ARCHIVE_VAR(int, m_iHealIfDefeatExcludeBarbariansCount)
SYNC_ARCHIVE_VAR(int, m_iNumInterceptions)
SYNC_ARCHIVE_VAR(int, m_iExtraAirInterceptRange)
SYNC_ARCHIVE_VAR(int, m_iMadeInterceptionCount)
SYNC_ARCHIVE_VAR(int, m_iEverSelectedCount)
SYNC_ARCHIVE_VAR(int, m_iSapperCount)
SYNC_ARCHIVE_VAR(int, m_iCanHeavyCharge)
SYNC_ARCHIVE_VAR(int, m_iStrongerDamaged)
SYNC_ARCHIVE_VAR(int, m_iDiplomaticMissionAccomplishment)
SYNC_ARCHIVE_VAR(int, m_iFightWellDamaged)
SYNC_ARCHIVE_VAR(int, m_iFreeAttackMoves)
SYNC_ARCHIVE_VAR(int, m_iCanMoraleBreak)
SYNC_ARCHIVE_VAR(int, m_iDamageAoEFortified)
SYNC_ARCHIVE_VAR(int, m_iWorkRateMod)
SYNC_ARCHIVE_VAR(int, m_iDamageReductionCityAssault)
SYNC_ARCHIVE_VAR(int, m_iGoodyHutYieldBonus)
SYNC_ARCHIVE_VAR(int, m_iReligiousPressureModifier)
SYNC_ARCHIVE_VAR(int, m_iNumExoticGoods)
SYNC_ARCHIVE_VAR(bool, m_bPromotionReady)
SYNC_ARCHIVE_VAR(bool, m_bDeathDelay)
SYNC_ARCHIVE_VAR(bool, m_bCombatFocus)
SYNC_ARCHIVE_VAR(bool, m_bInfoBarDirty)
SYNC_ARCHIVE_VAR(bool, m_bNotConverting)
SYNC_ARCHIVE_VAR(bool, m_bAirCombat)
SYNC_ARCHIVE_VAR(bool, m_bEmbarked)
SYNC_ARCHIVE_VAR(bool, m_bPromotedFromGoody)
SYNC_ARCHIVE_VAR(bool, m_bAITurnProcessed)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenThisTurn)
SYNC_ARCHIVE_VAR(int, m_iDamageTakenLastTurn)
SYNC_ARCHIVE_VAR(PlayerTypes, m_eCapturingPlayer)
SYNC_ARCHIVE_VAR(bool, m_bCapturedAsIs)
SYNC_ARCHIVE_VAR(bool, m_bCapturedAsConscript)
SYNC_ARCHIVE_VAR(UnitTypes, m_eLeaderUnitType)
SYNC_ARCHIVE_VAR(InvisibleTypes, m_eInvisibleType)
SYNC_ARCHIVE_VAR(InvisibleTypes, m_eSeeInvisibleType)
SYNC_ARCHIVE_VAR(GreatPeopleDirectiveTypes, m_eGreatPeopleDirectiveType)
SYNC_ARCHIVE_VAR(CvString, m_strScriptData)
SYNC_ARCHIVE_VAR(int, m_iScenarioData)
SYNC_ARCHIVE_VAR(int, m_iBuilderStrength)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_ignoreTerrainCostInCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_ignoreTerrainCostFromCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_ignoreFeatureCostInCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_ignoreFeatureCostFromCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_terrainDoubleMoveCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_featureDoubleMoveCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_terrainHalfMoveCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_featureHalfMoveCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_terrainExtraMoveCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_featureExtraMoveCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_terrainDoubleHeal)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_featureDoubleHeal)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_terrainImpassableCount)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_featureImpassableCount)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_extraTerrainAttackPercent)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_extraTerrainDefensePercent)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_vTerrainModifierAttack)
SYNC_ARCHIVE_VAR(TerrainTypeCounter, m_vTerrainModifierDefense)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_extraFeatureAttackPercent)
SYNC_ARCHIVE_VAR(FeatureTypeCounter, m_extraFeatureDefensePercent)
SYNC_ARCHIVE_VAR(UnitClassCounter, m_extraUnitClassAttackMod)
SYNC_ARCHIVE_VAR(UnitClassCounter, m_extraUnitClassDefenseMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_aiNumTimesAttackedThisTurn)
SYNC_ARCHIVE_VAR(std::vector<int>, m_yieldFromScouting)
SYNC_ARCHIVE_VAR(std::vector<int>, m_piYieldFromAncientRuins)
SYNC_ARCHIVE_VAR(std::vector<int>, m_piYieldFromTRPlunder)
SYNC_ARCHIVE_VAR(std::vector<int>, m_yieldFromKills)
SYNC_ARCHIVE_VAR(std::vector<int>, m_yieldFromBarbarianKills)
SYNC_ARCHIVE_VAR(std::vector<int>, m_extraUnitCombatModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_extraUnitCombatModifierAttack)
SYNC_ARCHIVE_VAR(std::vector<int>, m_extraUnitCombatModifierDefense)
SYNC_ARCHIVE_VAR(std::vector<int>, m_unitClassModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatModifier)
SYNC_ARCHIVE_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatAttackMod)
SYNC_ARCHIVE_VAR(std::vector<int>, m_iCombatModPerAdjacentUnitCombatDefenseMod)
SYNC_ARCHIVE_VAR(SYNC_ARCHIVE_VAR_TYPE(std::map<int, std::pair<int, int>>), m_yieldFromPillage)
SYNC_ARCHIVE_VAR(int, m_iMissionTimer)
SYNC_ARCHIVE_VAR(int, m_iMissionAIX)
SYNC_ARCHIVE_VAR(int, m_iMissionAIY)
SYNC_ARCHIVE_VAR(MissionAITypes, m_eMissionAIType)
SYNC_ARCHIVE_VAR(ActivityTypes, m_eActivityType)
SYNC_ARCHIVE_VAR(AutomateTypes, m_eAutomateType)
SYNC_ARCHIVE_VAR(UnitAITypes, m_eUnitAIType)
SYNC_ARCHIVE_VAR(UnitCombatTypes, m_eCombatType)
SYNC_ARCHIVE_VAR(int, m_iEmbarkedAllWaterCount)
SYNC_ARCHIVE_VAR(int, m_iEmbarkedDeepWaterCount)
SYNC_ARCHIVE_VAR(int, m_iEmbarkExtraVisibility)
SYNC_ARCHIVE_VAR(int, m_iEmbarkDefensiveModifier)
SYNC_ARCHIVE_VAR(int, m_iCapitalDefenseModifier)
SYNC_ARCHIVE_VAR(int, m_iCapitalDefenseFalloff)
SYNC_ARCHIVE_VAR(int, m_iCapitalDefenseLimit)
SYNC_ARCHIVE_VAR(int, m_iCityAttackPlunderModifier)
SYNC_ARCHIVE_VAR(int, m_iReligiousStrengthLossRivalTerritory)
SYNC_ARCHIVE_VAR(int, m_iTradeMissionInfluenceModifier)
SYNC_ARCHIVE_VAR(int, m_iTradeMissionGoldModifier)
SYNC_ARCHIVE_VAR(int, m_iCombatModPerLevel)
SYNC_ARCHIVE_VAR(int, m_iDiploMissionInfluence)
SYNC_ARCHIVE_VAR(int, m_iMapLayer)
SYNC_ARCHIVE_VAR(int, m_iNumGoodyHutsPopped)
SYNC_ARCHIVE_VAR(int, m_iTourismBlastStrength)
SYNC_ARCHIVE_VAR(int, m_iTourismBlastLength)
SYNC_ARCHIVE_VAR(int, m_iHurryStrength)
SYNC_ARCHIVE_VAR(int, m_iGoldBlastStrength)
SYNC_ARCHIVE_VAR(int, m_iScienceBlastStrength)
SYNC_ARCHIVE_VAR(int, m_iCultureBlastStrength)
SYNC_ARCHIVE_VAR(int, m_iGAPBlastStrength)
SYNC_ARCHIVE_VAR(std::vector<bool>, m_abPromotionEverObtained)
SYNC_ARCHIVE_VAR(AITacticalMove, m_eTacticalMove)
SYNC_ARCHIVE_VAR(int, m_iTacticalMoveSetTurn)
SYNC_ARCHIVE_VAR(AIHomelandMove, m_eHomelandMove)
SYNC_ARCHIVE_VAR(int, m_iHomelandMoveSetTurn)
SYNC_ARCHIVE_END()

#endif
