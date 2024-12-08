/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"

#include "CvAStar.h"
#include "CvAchievementUnlocker.h"
#include "CvCity.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvEnumSerialization.h"
#include "CvGameCoreUtils.h"
#include "CvGlobals.h"
#include "CvImprovementClasses.h"
#include "CvInfos.h"
#include "CvInternalGameCoreUtils.h"
#include "CvMap.h"
#include "CvMilitaryAI.h"
#include "CvMinorCivAI.h"
#include "CvNotifications.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeam.h"
#include "CvTypes.h"
#include "CvUnitCombat.h"
#include "CvUnitMission.h"
#include "FMemoryStream.h"
#include "ICvDLLUserInterface.h"
#include <math.h>

#include "CvDllPlot.h"
#include "CvDllUnit.h"
#include "CvDllCity.h"
#include "CvGameQueries.h"
#include "CvBarbarians.h"

#if !defined(FINAL_RELEASE)
#include <sstream>
// If defined, various operations related to the movement of units will be logged.
//#define LOG_UNIT_MOVES
#endif

#if defined(LOG_UNIT_MOVES)
#define LOG_UNIT_MOVES_MESSAGE(x)		gDLL->netMessageDebugLog(x)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	{ std::ostringstream str; str << x; gDLL->netMessageDebugLog(str.str()); }
#else
#define LOG_UNIT_MOVES_MESSAGE(x)		((void)0)
#define LOG_UNIT_MOVES_MESSAGE_OSTR(x)	((void)0)
#endif

// Come back to this
#include "LintFree.h"

// for statistics
int saiTaskWhenKilled[100] = {0};

// for diagnosing movement problems, it's useful so set this flag to true on a breakpoint and examine the AI units in game
bool g_bFreezeUnits = false;

namespace FSerialization
{

//is it wise to store pointers here?
//anyway vector seems better than set because of the defined ordering
//a set of IDInfos would be best but we don't know the ID when creating the unit
std::vector<CvUnit*> unitsToCheck;

void SyncUnits()
{
	if(GC.getGame().isNetworkMultiPlayer())
	{
		PlayerTypes authoritativePlayer = GC.getGame().getActivePlayer();

		std::vector<CvUnit*>::const_iterator i;
		for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
		{
			CvUnit* unit = *i;

			if(unit)
			{
				const CvPlayer& player = GET_PLAYER(unit->getOwner());
				if(unit->getOwner() == authoritativePlayer || (gDLL->IsHost() && !player.isHuman() && player.isAlive()))
				{
					CvSyncArchive<CvUnit>& archive = unit->getSyncArchive();
					archive.collectDeltas();
					if(archive.hasDeltas())
					{
						FMemoryStream memoryStream;
						std::vector<std::pair<std::string, std::string> > callStacks;
						archive.saveDelta(memoryStream, callStacks);
						gDLL->sendUnitSyncCheck(unit->getOwner(), unit->GetID(), memoryStream, callStacks);
					}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
// clears ALL deltas for ALL units
void ClearUnitDeltas()
{
	std::vector<CvUnit*>::iterator i;
	for(i = unitsToCheck.begin(); i != unitsToCheck.end(); ++i)
	{
		CvUnit* unit = *i;

		if(unit)
		{
			FAutoArchive& archive = unit->getSyncArchive();
			archive.clearDelta();
		}
	}
}
}

bool s_dispatchingNetMessage = false;

OBJECT_VALIDATE_DEFINITION(CvUnit)

//	--------------------------------------------------------------------------------
// Public Functions...
CvUnit::CvUnit() :
	m_syncArchive()
	, m_iID()
	, m_iHotKeyNumber()
	, m_iX()
	, m_iY()
	, m_iLastMoveTurn()
	, m_iCycleOrder()
	, m_iDeployFromOperationTurn()
	, m_iReconX()
	, m_iReconY()
	, m_iReconCount()
	, m_iGameTurnCreated()
	, m_iDamage()
	, m_iMoves()
#if defined(MOD_LINKED_MOVEMENT)
	, m_bIsLinked()
	, m_bIsLinkedLeader()
	, m_bIsGrouped()
	, m_iLinkedMaxMoves()
	, m_LinkedUnitIDs()
	, m_iLinkedLeaderID()
#endif
#if defined(MOD_SQUADS)
	, m_iSquadNumber()
	, m_iSquadDestinationX()
	, m_iSquadDestinationY()
	, m_SquadEndMovementType()
#endif
	, m_bImmobile()
	, m_iExperienceTimes100()
	, m_iLevel()
	, m_iCargo()
	, m_iCargoCapacity()
	, m_iAttackPlotX()
	, m_iAttackPlotY()
	, m_iCombatTimer()
	, m_bMovedThisTurn()
	, m_bFortified()
	, m_iBlitzCount()
	, m_iAmphibCount()
	, m_iRiverCrossingNoPenaltyCount()
	, m_iEnemyRouteCount()
	, m_iRivalTerritoryCount()
	, m_iIsSlowInEnemyLandCount()
	, m_iRangeAttackIgnoreLOSCount()
	, m_iCityAttackOnlyCount()
	, m_iCaptureDefeatedEnemyCount()
	, m_iRangedSupportFireCount()
	, m_iAlwaysHealCount()
	, m_iHealOutsideFriendlyCount()
	, m_iHillsDoubleMoveCount()
	, m_iRiverDoubleMoveCount()
#if defined(MOD_BALANCE_CORE)
	, m_iMountainsDoubleMoveCount()
	, m_iEmbarkFlatCostCount()
	, m_iDisembarkFlatCostCount()
	, m_iAOEDamageOnKill()
	, m_iAOEDamageOnPillage()
	, m_iAoEDamageOnMove()
	, m_iPartialHealOnPillage()
	, m_iSplashDamage()
	, m_iMultiAttackBonus()
	, m_iLandAirDefenseValue()
#endif
	, m_iExtraVisibilityRange()
	, m_iExtraMoves()
	, m_iExtraMoveDiscount()
	, m_iExtraRange()
	, m_iInterceptChance()
#if defined(MOD_BALANCE_CORE)
	, m_iExtraAirInterceptRange()  // JJ: This is new
#endif
	, m_iExtraEvasion()
	, m_iExtraWithdrawal()
#if defined(MOD_BALANCE_CORE_JFD)
	, m_eUnitContract()
	, m_iNegatorPromotion()
#endif
	, m_bIsNoMaintenance()
	, m_iExtraEnemyHeal()
	, m_iExtraNeutralHeal()
	, m_iExtraFriendlyHeal()
	, m_iSameTileHeal()
	, m_iAdjacentTileHeal()
	, m_iEnemyDamageChance()
	, m_iNeutralDamageChance()
	, m_iEnemyDamage()
	, m_iNeutralDamage()
	, m_iNearbyEnemyCombatMod()
	, m_iNearbyEnemyCombatRange()
	, m_iSapperCount()
	, m_iCanHeavyCharge()
	, m_iNumExoticGoods()
	, m_iAdjacentModifier()
	, m_iNoAdjacentUnitModifier()
	, m_iRangedAttackModifier()
	, m_iInterceptionCombatModifier()
	, m_iInterceptionDefenseDamageModifier()
	, m_iAirSweepCombatModifier()
	, m_iAttackModifier()
	, m_iDefenseModifier()
	, m_iGroundAttackDamage()
	, m_iExtraCombatPercent()
	, m_iExtraCityAttackPercent()
	, m_iExtraCityDefensePercent()
	, m_iExtraRangedDefenseModifier()
	, m_iExtraHillsAttackPercent()
	, m_iExtraHillsDefensePercent()
	, m_iExtraOpenAttackPercent()
	, m_iExtraOpenRangedAttackMod()
	, m_iExtraRoughAttackPercent()
	, m_iExtraRoughRangedAttackMod()
	, m_iExtraAttackFortifiedMod()
	, m_iExtraAttackWoundedMod()
	, m_iExtraFullyHealedMod()
	, m_iExtraAttackAboveHealthMod()
	, m_iExtraAttackBelowHealthMod()
	, m_iRangedFlankAttack()
	, m_iFlankPower()
	, m_iFlankAttackModifier()
	, m_iExtraOpenDefensePercent()
	, m_iExtraRoughDefensePercent()
	, m_iExtraOpenFromPercent()
	, m_iExtraRoughFromPercent()
	, m_iPillageChange()
	, m_iUpgradeDiscount()
	, m_iExperiencePercent()
	, m_iDropRange()
	, m_iAirSweepCapableCount()
	, m_iExtraNavalMoves()
	, m_iBaseCombat()
	, m_eFacingDirection()
	, m_iArmyId()
	, m_iIgnoreTerrainCostCount()
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	, m_iGGFromBarbariansCount()
#endif
	, m_iRoughTerrainEndsTurnCount()
	, m_iCapturedUnitsConscriptedCount()
	, m_iEmbarkAbilityCount()
	, m_iHoveringUnitCount()
	, m_iFlatMovementCostCount()
	, m_iCanMoveImpassableCount()
	, m_iOnlyDefensiveCount()
	, m_iNoAttackInOceanCount()
	, m_iNoDefensiveBonusCount()
	, m_iNoCaptureCount()
	, m_iNukeImmuneCount()
	, m_iHiddenNationalityCount()
	, m_iAlwaysHostileCount()
	, m_iNoRevealMapCount()
	, m_iCanMoveAllTerrainCount()
	, m_iCanMoveAfterAttackingCount()
	, m_iFreePillageMoveCount()
	, m_iHealOnPillageCount()
	, m_iHPHealedIfDefeatEnemy()
	, m_iGoldenAgeValueFromKills()
	, m_iHealIfDefeatExcludeBarbariansCount()
	, m_iTacticalAIPlotX()
	, m_iTacticalAIPlotY()
	, m_iGarrisonCityID()
	, m_iNumAttacks()
	, m_iAttacksMade()
	, m_iGreatGeneralCount()
	, m_iGreatAdmiralCount()
	, m_iAuraRangeChange()
	, m_iAuraEffectChange()
	, m_iNumRepairCharges()
	, m_iMilitaryCapChange()
	, m_iGreatGeneralModifier()
	, m_iGreatGeneralReceivesMovementCount()
	, m_iGreatGeneralCombatModifier()
	, m_iIgnoreGreatGeneralBenefit()
	, m_iIgnoreZOC()
	, m_iNoSupply()
	, m_iMaxHitPointsBase(GD_INT_GET(MAX_HIT_POINTS))
	, m_iMaxHitPointsChange()
	, m_iMaxHitPointsModifier()
	, m_iFriendlyLandsModifier()
	, m_iFriendlyLandsAttackModifier()
	, m_iOutsideFriendlyLandsModifier()
	, m_iNumInterceptions()
	, m_iMadeInterceptionCount()
	, m_iEverSelectedCount()
	, m_iEmbarkedAllWaterCount()
	, m_iEmbarkExtraVisibility()
	, m_iEmbarkDefensiveModifier()
	, m_iCapitalDefenseModifier()
	, m_iCapitalDefenseFalloff()
	, m_iCityAttackPlunderModifier()
	, m_iReligiousStrengthLossRivalTerritory()
	, m_iTradeMissionInfluenceModifier()
	, m_iTradeMissionGoldModifier()
	, m_iDiploMissionInfluence()
	, m_strName("")
	, m_eGreatWork(NO_GREAT_WORK)
	, m_iTourismBlastStrength()
	, m_iTourismBlastLength()
	, m_bPromotionReady()
	, m_bDeathDelay()
	, m_bCombatFocus()
	, m_bInfoBarDirty()
	, m_bNotConverting()
	, m_bAirCombat()
	, m_bEmbarked()
	, m_bPromotedFromGoody()
	, m_bAITurnProcessed()
	, m_eOwner()
	, m_eOriginalOwner()
	, m_eGiftedByPlayer()
	, m_eCapturingPlayer()
	, m_bCapturedAsIs()
	, m_bCapturedAsConscript()
	, m_eUnitType()
	, m_eLeaderUnitType()
	, m_eInvisibleType()
	, m_eSeeInvisibleType()
	, m_eGreatPeopleDirectiveType()
	, m_combatUnit()
	, m_transportUnit()
	, m_extraDomainModifiers()
	, m_extraDomainAttacks()
	, m_extraDomainDefenses()
	, m_YieldModifier()
	, m_YieldChange()
	, m_iGarrisonYieldChange()
	, m_iFortificationYieldChange()
	, m_strScriptData()
	, m_iScenarioData()
	, m_ignoreTerrainCostInCount()
	, m_ignoreTerrainCostFromCount()
	, m_ignoreFeatureCostInCount()
	, m_ignoreFeatureCostFromCount()
	, m_terrainDoubleMoveCount()
	, m_featureDoubleMoveCount()
	, m_terrainImpassableCount()
	, m_featureImpassableCount()
	, m_extraTerrainAttackPercent()
	, m_extraTerrainDefensePercent()
	, m_extraFeatureAttackPercent()
	, m_extraFeatureDefensePercent()
	, m_extraUnitClassAttackMod()
	, m_extraUnitClassDefenseMod()
	, m_extraUnitCombatModifier()
	, m_unitClassModifier()
#if defined(MOD_BALANCE_CORE)
	, m_iCombatModPerAdjacentUnitCombatModifier()
	, m_iCombatModPerAdjacentUnitCombatAttackMod()
	, m_iCombatModPerAdjacentUnitCombatDefenseMod()
#endif
	, m_yieldFromPillage()
	, m_iMissionTimer()
	, m_iMissionAIX()
	, m_iMissionAIY()
	, m_eMissionAIType()
	, m_missionAIUnit()
	, m_eActivityType()
	, m_eAutomateType()
	, m_eUnitAIType()
	, m_bWaitingForMove(false)
	, m_iMapLayer()
	, m_iNumGoodyHutsPopped()
	, m_eCombatType()
#if defined(MOD_BALANCE_CORE)
	, m_iOriginCity()
	, m_iCannotBeCapturedCount()
	, m_iForcedDamage()
	, m_iChangeDamage()
	, m_PromotionDuration()
	, m_TurnPromotionGained()
	, m_iNumTilesRevealedThisTurn()
	, m_bSpottedEnemy()
	, m_iGainsXPFromScouting()
	, m_iGainsXPFromPillaging()
	, m_iGainsXPFromSpotting()
	, m_iCaptureDefeatedEnemyChance()
	, m_iBarbCombatBonus()
	, m_iAdjacentEnemySapMovement()
	, m_iCanMoraleBreak()
	, m_iDamageAoEFortified()
	, m_iWorkRateMod()
	, m_iDamageReductionCityAssault()
	, m_iStrongerDamaged()
	, m_iFightWellDamaged()
	, m_iGoodyHutYieldBonus()
	, m_iReligiousPressureModifier()
#endif
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	, m_iExtraReconRange()
#endif
	, m_iBaseRangedCombat()
	, m_iIgnoreTerrainDamageCount()
	, m_iIgnoreFeatureDamageCount()
	, m_iExtraTerrainDamageCount()
	, m_iExtraFeatureDamageCount()
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	, m_iNearbyImprovementCombatBonus()
	, m_iNearbyImprovementBonusRange()
	, m_eCombatBonusImprovement()
#endif
#if defined(MOD_BALANCE_CORE)
	, m_iCombatBonusFromNearbyUnitClass()
	, m_iNearbyUnitClassBonusRange()
	, m_iNearbyUnitClassBonus()
	, m_iNearbyPromotion()
	, m_iNearbyUnitPromotionRange()
	, m_iNearbyCityCombatMod()
	, m_iNearbyFriendlyCityCombatMod()
	, m_iNearbyEnemyCityCombatMod()
	, m_iPillageBonusStrengthPercent()
	, m_iStackedGreatGeneralExperience()
	, m_iWonderProductionModifier()
	, m_iUnitProductionModifier()
	, m_iNearbyEnemyDamage()
	, m_iAdjacentCityDefenseMod()
	, m_iGGGAXPPercent()
	, m_iGiveCombatMod()
	, m_iGiveHPIfEnemyKilled()
	, m_iGiveExperiencePercent()
	, m_iGiveOutsideFriendlyLandsModifier()
	, m_eGiveDomain()
	, m_iGiveExtraAttacks()
	, m_iGiveDefenseMod()
	, m_iGiveInvisibility()
	, m_bGiveOnlyOnStartingTurn()
	, m_iConvertUnit()
	, m_eConvertDomain()
	, m_eConvertDomainUnit()
	, m_iConvertEnemyUnitToBarbarian()
	, m_bConvertOnFullHP()
	, m_bConvertOnDamage()
	, m_iDamageThreshold()
	, m_eConvertDamageOrFullHPUnit()
	, m_iNumberOfCultureBombs()
	, m_iNearbyHealEnemyTerritory()
	, m_iNearbyHealNeutralTerritory()
	, m_iNearbyHealFriendlyTerritory()
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	, m_iCanCrossMountainsCount()
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	, m_iCanCrossOceansCount()
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	, m_iCanCrossIceCount()
#endif
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	, m_iEmbarkedDeepWaterCount()
#endif
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	, m_strUnitName("")
#endif
#if defined(MOD_CORE_PER_TURN_DAMAGE)
	, m_iDamageTakenThisTurn()
	, m_iDamageTakenLastTurn()
#endif
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	, m_terrainHalfMoveCount()
	, m_featureHalfMoveCount()
	, m_terrainExtraMoveCount()
	, m_featureExtraMoveCount()
#endif
#if defined(MOD_BALANCE_CORE)
	, m_iHurryStrength()
	, m_iGoldBlastStrength()
	, m_iScienceBlastStrength()
	, m_iCultureBlastStrength()
	, m_iGAPBlastStrength()
	, m_abPromotionEverObtained()
	, m_terrainDoubleHeal()
	, m_featureDoubleHeal()
#endif
	, m_yieldFromKills()
	, m_yieldFromBarbarianKills()
#if defined(MOD_BALANCE_CORE)
	, m_aiNumTimesAttackedThisTurn()
	, m_yieldFromScouting()
#endif
#if defined(MOD_CIV6_WORKER)
	, m_iBuilderStrength()
#endif
	, m_eTacticalMove()
	, m_iTacticalMoveSetTurn()
	, m_eHomelandMove()
	, m_iHomelandMoveSetTurn()
{
	OBJECT_ALLOCATED
	FSerialization::unitsToCheck.push_back(this);
	reset(0, NO_UNIT, NO_PLAYER, true);

	if (GC.getGame().isNetworkMultiPlayer())
	{
		m_syncArchive.initSyncVars(*FNEW(CvSyncArchive<CvUnit>::SyncVars(*this), c_eCiv5GameplayDLL, 0));
	}
}


//	--------------------------------------------------------------------------------
CvUnit::~CvUnit()
{
	//really shouldn't happen that it's not present, but there was a crash here
	std::vector<CvUnit*>::iterator it = std::find( FSerialization::unitsToCheck.begin(), FSerialization::unitsToCheck.end(), this);
	if (it!=FSerialization::unitsToCheck.end())
		FSerialization::unitsToCheck.erase(it);

	if(gDLL && !gDLL->GetDone() && GC.IsGraphicsInitialized())  // don't need to remove entity when the app is shutting down, or crash can occur
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitDestroyed(pDllUnit.get());
	}

	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
		GC.getMap().plotManager().RemoveUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);

	uninit();

	OBJECT_DESTROYED
}


//	--------------------------------------------------------------------------------
void CvUnit::init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical, int iMapLayer /*= DEFAULT_UNIT_MAP_LAYER*/, int iNumGoodyHutsPopped, ContractTypes eContract, bool bHistoric, CvUnit* pPassUnit)
{
	initWithNameOffset(iID, eUnit, -1, eUnitAI, eOwner, iX, iY, eReason, bNoMove, bSetupGraphical, iMapLayer, iNumGoodyHutsPopped, eContract, bHistoric, false, pPassUnit);
}
//	--------------------------------------------------------------------------------
void CvUnit::initWithNameOffset(int iID, UnitTypes eUnit, int iNameOffset, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, UnitCreationReason eReason, bool bNoMove, bool bSetupGraphical, int iMapLayer, int iNumGoodyHutsPopped, ContractTypes eContract, bool bHistoric, bool bSkipNaming, CvUnit* pPassUnit)
{
	VALIDATE_OBJECT
	CvString strBuffer;
	int iUnitName = 0;
	int iI = 0;

	CvAssert(NO_UNIT != eUnit);

	m_Promotions.Init();
	m_Religion.Init();

	//--------------------------------
	// Init saved data
	reset(iID, eUnit, eOwner);

	//set to some default
	m_eFacingDirection = DIRECTION_SOUTHEAST;

	// If this is a hovering unit, we must add that promotion before setting XY, or else it'll get the embark promotion (which we don't want)
	PromotionTypes ePromotion;
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes)iI;

		// If upgrading, transfer memory of the promotions the old unit has ever obtained (this is in initWithNamedOffset() instead of convert() because promotions can get granted here, and the game needs to know if the promotion is ever obtained before promotions are granted)
		if (eReason == REASON_UPGRADE && pPassUnit != NULL && pPassUnit->IsPromotionEverObtained(ePromotion))
		{
			SetPromotionEverObtained(ePromotion, true);
	}
		if(getUnitInfo().GetFreePromotions(iI))
		{
			if(GC.getPromotionInfo(ePromotion)->IsHoveringUnit())
				setHasPromotion(ePromotion, true);

			if(GC.getPromotionInfo(ePromotion)->IsConvertUnit())
				setHasPromotion(ePromotion, true);
		}
	}

	// Set the layer of the map the unit resides
	m_iMapLayer = iMapLayer;
	
	// Set num goody huts popped
	m_iNumGoodyHutsPopped = iNumGoodyHutsPopped;

	//--------------------------------
	// Init containers

	//--------------------------------
	// Init pre-setup() data
	setXY(iX, iY, false, false, false, false, bNoMove);

	if(plot() && plot()->getOwningCity() && plot()->getOwner() == getOwner())
		setOriginCity(plot()->getOwningCity()->GetID());

	//--------------------------------
	// Init non-saved data

	if (MOD_BALANCE_CORE_MILITARY_LOGGING)
	{
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString info = CvString::format( "%03d;%s;id;0x%08X;owner;%02d;army;0x%08X;%s;arg1;%d;arg2;%d;flags;0x%08X;at;%d;%d\n", 
				GC.getGame().getElapsedGameTurns(),this->getNameKey(),this->GetID(),this->getOwner(),this->getArmyID(),"NEW_UNIT",-1,-1,0,iX,iY); 
			FILogFile* pLog=LOGFILEMGR.GetLog( "unit-missions.csv", FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );
			pLog->Msg( info.c_str() );
		}
	}

	//--------------------------------
	// Init other game data
	plot()->updateCenterUnit();

	SetGreatWork(NO_GREAT_WORK);
	iUnitName = GC.getGame().getUnitCreatedCount(getUnitType());
	int iNumNames = getUnitInfo().GetNumUnitNames();
	if (!bSkipNaming && iNumNames > 0)
	{
		std::vector<int> vfPossibleUnits;
		if(iNameOffset == -1)
		{
			iNameOffset = GC.getGame().randRangeExclusive(0, iNumNames, CvSeeder(plot()->GetPseudoRandomSeed()));
		}
		CvString strName = NULL;
		//Look for units from previous and current eras.
		for(iI = 0; iI < iNumNames; iI++)
		{
			EraTypes eUnitEra = getUnitInfo().GetGreatPersonEra(iI);
			if((eUnitEra != NO_ERA) && eUnitEra <= GET_PLAYER(getOwner()).GetCurrentEra())
			{
				strName = getUnitInfo().GetUnitNames(iI);
				if(!GC.getGame().isGreatPersonBorn(strName))
				{
					vfPossibleUnits.push_back(iI);
				}
			}
		}
		if(vfPossibleUnits.size() > 0)
		{
			uint uRoll = GC.getGame().urandLimitExclusive(vfPossibleUnits.size(), CvSeeder(iID));
			strName = getUnitInfo().GetUnitNames(vfPossibleUnits[uRoll]);
			setName(strName);
			SetGreatWork(getUnitInfo().GetGreatWorks(vfPossibleUnits[uRoll]));
			GC.getGame().addGreatPersonBornName(strName);

			if (MOD_GLOBAL_NO_LOST_GREATWORKS)
			{
				// setName strips undesirable characters, but we stored those into the list of GPs born, so we need to keep the original name
				setGreatName(strName);
			}
		}
		//None? Let's look to the next era.
		else
		{
			for(iI = 0; iI < iNumNames; iI++)
			{
				EraTypes eUnitEra = getUnitInfo().GetGreatPersonEra(iI);
				if((eUnitEra != NO_ERA) && ((eUnitEra - 1 ) == GET_PLAYER(getOwner()).GetCurrentEra()))
				{
					strName = getUnitInfo().GetUnitNames(iI);
					if(!GC.getGame().isGreatPersonBorn(strName))
					{
						vfPossibleUnits.push_back(iI);
					}
				}
			}
			if(vfPossibleUnits.size() > 0)
			{
				uint uRoll = GC.getGame().urandLimitExclusive(vfPossibleUnits.size(), CvSeeder(iID));
				strName = getUnitInfo().GetUnitNames(vfPossibleUnits[uRoll]);
				setName(strName);
				SetGreatWork(getUnitInfo().GetGreatWorks(vfPossibleUnits[uRoll]));
				GC.getGame().addGreatPersonBornName(strName);

				if (MOD_GLOBAL_NO_LOST_GREATWORKS)
				{
					// setName strips undesirable characters, but we stored those into the list of GPs born, so we need to keep the original name
					setGreatName(strName);
				}
			}
		}
		//If still no valid GPs, do the old random method.
		if (vfPossibleUnits.size() <= 0)
		{
			for (iI = 0; iI < iNumNames; iI++)
			{
				int iIndex = (iNameOffset + iI) % iNumNames;
				CvString strName = getUnitInfo().GetUnitNames(iIndex);

				if (MOD_EVENTS_UNIT_DATA)
				{
					if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveName, getOwner(), GetID(), iIndex) == GAMEEVENTRETURN_FALSE)
					{
						continue;
					}
				}

				if (!GC.getGame().isGreatPersonBorn(strName))
				{
					if (MOD_EVENTS_UNIT_DATA)
					{
						if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveGreatWork, getOwner(), GetID(), getUnitInfo().GetGreatWorks(iIndex)) == GAMEEVENTRETURN_FALSE)
						{
							continue;
						}
					}

					setName(strName);
					SetGreatWork(getUnitInfo().GetGreatWorks(iIndex));
					GC.getGame().addGreatPersonBornName(strName);

					if (MOD_GLOBAL_NO_LOST_GREATWORKS)
					{
						// setName strips undesirable characters, but we stored those into the list of GPs born, so we need to keep the original name
						setGreatName(strName);
					}

					break;
				}
			}
		}
	}
	setGameTurnCreated(GC.getGame().getGameTurn());

	GC.getGame().incrementUnitCreatedCount(getUnitType());

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	GC.getGame().incrementUnitClassCreatedCount((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(getUnitInfo().GetUnitClassType())), 1);

	// Builder Limit
	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		kPlayer.ChangeNumBuilders(1);
	}

	//get builder strength
	if (getUnitInfo().GetBuilderStrength() > 0)
	{
		// use speed modifier to increase the work count, but not for archaeologists since they are killed anyways after dig site build.
		int ibuildercharges = getUnitInfo().GetBuilderStrength();
		UnitClassTypes eArchaeologistClass = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_ARCHAEOLOGIST", true);
		if (getUnitClassType() != eArchaeologistClass)
		{
			const int iworkerspeedmodifier = kPlayer.getWorkerSpeedModifier() + kPlayer.GetPlayerTraits()->GetWorkerSpeedModifier();
			if (0 != iworkerspeedmodifier)
			{
				float fTemp = (float)ibuildercharges;
				fTemp *= (100 + iworkerspeedmodifier);
				fTemp /= 100;
				ibuildercharges = (int)ceil(fTemp); // Round up
			}
		}
		setBuilderStrength(ibuildercharges);
	}

	if (getUnitInfo().GetNumberOfCultureBombs() > 0)
	{
		setNumberOfCultureBombs(getUnitInfo().GetNumberOfCultureBombs());
	}

	if(MOD_BALANCE_CORE_SETTLER_RESET_FOOD && getUnitInfo().IsFound())
	{
		CvCity* pCity = plot()->getPlotCity();
		if(pCity != NULL)
		{
			pCity->setFood(0);
		}
	}
	if (plot() != NULL)
	{
		CvCity* pCity = plot()->getPlotCity();
		if (pCity != NULL)
		{
			if (GET_PLAYER(getOwner()).GetFlatDefenseFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				pCity->updateStrengthValue();
			}

			if (GET_PLAYER(getOwner()).GetNeedsModifierFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				GET_PLAYER(getOwner()).CalculateNetHappiness();
			}
		}
	}

	// Units can add Unhappiness
	if(GC.getUnitInfo(getUnitType())->GetUnhappiness() != 0)
	{
		kPlayer.ChangeUnhappinessFromUnits(GC.getUnitInfo(getUnitType())->GetUnhappiness());
	}

	kPlayer.changeExtraUnitCost(getUnitInfo().GetExtraMaintenanceCost());

	// Add Resource Quantity to Used
	if(MOD_BALANCE_CORE_JFD)
	{
		if(eContract != NO_CONTRACT)
		{
			setContractUnit(eContract);
		}
		if(!isContractUnit())
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
				{
					kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, GC.getUnitInfo(getUnitType())->GetResourceQuantityRequirement(iResourceLoop));
				}
			}
		}
	}
	else
	{
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
			{
				kPlayer.changeNumResourceUsed((ResourceTypes) iResourceLoop, GC.getUnitInfo(getUnitType())->GetResourceQuantityRequirement(iResourceLoop));
			}
		}
	}
	if(getUnitInfo().GetNukeDamageLevel() > 0)
	{
		kPlayer.changeNumNukeUnits(1);
	}

#if defined(MOD_BALANCE_CORE)
	SetNoMaintenance(getUnitInfo().IsNoMaintenance());

	if (getUnitInfo().IsMilitarySupport() && (isNoSupply() || isContractUnit()))
	{
		GET_PLAYER(getOwner()).changeNumUnitsSupplyFree(1);
	}
#endif

	if(getUnitInfo().IsMilitarySupport())
	{
		kPlayer.changeNumMilitaryUnits(1, getUnitInfo().GetDomainType());
	}

	// Free Promotions from Unit XML
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if(getUnitInfo().GetFreePromotions(iI))
		{
			ePromotion = (PromotionTypes) iI;

			PromotionTypes ePromotionRoughTerrain = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_ROUGH_TERRAIN_ENDS_TURN");
			if(ePromotion == ePromotionRoughTerrain && kPlayer.GetPlayerTraits()->IsConquestOfTheWorld())
				continue;

			if(!GC.getPromotionInfo(ePromotion)->IsHoveringUnit() && !GC.getPromotionInfo(ePromotion)->IsConvertUnit())	// Hovering and Convert domain units handled above
				setHasPromotion(ePromotion, true);
		}
	}

	const UnitCombatTypes unitCombatType = (UnitCombatTypes)getUnitCombatType();
	if(unitCombatType != NO_UNITCOMBAT)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitCombat(promotionID, unitCombatType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	if(isUnitEraUpgrade())
	{
		EraTypes eEra;
		int iNumEraInfos = GC.getNumEraInfos();
		for(int iEraLoop = 0; iEraLoop < iNumEraInfos; iEraLoop++)
		{
			eEra = (EraTypes) iEraLoop;
			if((m_pUnitInfo->GetEraCombatStrength(eEra) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eEra))
			{
				SetBaseCombatStrength(m_pUnitInfo->GetEraCombatStrength(eEra));
			}
			for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
			{
				UnitCombatTypes eUnitCombatClass;
				for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
				{
					eUnitCombatClass = (UnitCombatTypes) iI;
					if((m_pUnitInfo->GetUnitNewEraCombatType(eUnitCombatClass, eEra) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eEra))
					{
						setUnitCombatType(eUnitCombatClass);
					}
				}
				PromotionTypes ePromotion;
				for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
				{
					ePromotion = (PromotionTypes) iI;
					if((m_pUnitInfo->GetUnitNewEraPromotions(ePromotion, eEra) > 0) && (GET_TEAM(kPlayer.getTeam()).GetCurrentEra() >= eEra))
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
	}
	const UnitClassTypes unitClassType = getUnitClassType();
	if(unitClassType != NO_UNITCLASS)
	{
		// Any free Promotions to apply?
		for(int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			const PromotionTypes promotionID = (PromotionTypes)iJ;
			if(kPlayer.GetPlayerTraits()->HasFreePromotionUnitClass(promotionID, unitClassType))
			{
				setHasPromotion(promotionID, true);
			}
		}
	}

#endif

	// Free Promotions from Policies, Techs, etc.
	for(iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		ePromotion = (PromotionTypes) iI;

		if(kPlayer.IsFreePromotion(ePromotion))
		{
			// Valid Promotion for this Unit?
			if(::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

			else if(::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType()))
			{
				setHasPromotion(ePromotion, true);
			}

		}
	}

	// Give embark promotion for free?
	if(kPlayer.CanEmbark())
	{
		PromotionTypes ePromotionEmbarkation = kPlayer.GetEmbarkationPromotion();

		bool bGivePromotion = false;

		// Civilians get it for free
		if(getDomainType() == DOMAIN_LAND)
		{
			if(!IsCombatUnit())
				bGivePromotion = true;
		}

		// Can the unit get this? (handles water units and such)
		if(!bGivePromotion && ::IsPromotionValidForUnitCombatType(ePromotionEmbarkation, getUnitType()))
			bGivePromotion = true;

		// Some case that gives us the promotion?
		if(bGivePromotion)
			setHasPromotion(ePromotionEmbarkation, true);
	}

	// These get done in SetXY, but if the unit doesn't have the promotion, the variable doesn't get stored.
	kPlayer.UpdateAreaEffectUnit(this);
	if (isGiveInvisibility())
	{
		int iLoop = 0;
		int iRange = GetNearbyUnitPromotionsRange();
		for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
		{
			if (plotDistance(getX(), getY(), pLoopUnit->getX(), pLoopUnit->getY()) > iRange)
				continue;
			if (pLoopUnit->IsHiddenByNearbyUnit(pLoopUnit->plot()))
			{
				pLoopUnit->plot()->updateVisibility();
			}
		}
	}

	// Flip Deep Water Embarkation to Defensive Deep Water Embarkation if the player has the required trait
	if (IsEmbarkDeepWater()) {
		setHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_DEEPWATER_EMBARKATION), false);
		setHasPromotion(kPlayer.GetDeepWaterEmbarkationPromotion(), true);
	}

	// Any exotic goods that can be sold? (Portuguese unique unit mission)
	if (getUnitInfo().GetNumExoticGoods() > 0)
	{
		changeNumExoticGoods(getUnitInfo().GetNumExoticGoods());
	}

	if (canAcquirePromotionAny())
	{
		if (kPlayer.isMajorCiv())
		{
			// free XP from handicap?
			int iXP = kPlayer.getHandicapInfo().getFreeXP();
			iXP += kPlayer.isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIFreeXP();
			if (iXP > 0)
			{
				changeExperienceTimes100(iXP * 100);
			}

			// bonus xp in combat from handicap?
			int iXPPercent = kPlayer.getHandicapInfo().getFreeXPPercent();
			iXPPercent += kPlayer.isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIFreeXPPercent();
			if (iXPPercent > 0)
			{
				changeExperiencePercent(iXPPercent);
			}
		}
		else if (kPlayer.isMinorCiv())
		{
			// free XP from handicap?
			int iXP = GC.getGame().getHandicapInfo().getCityStateFreeXP();
			if (iXP > 0)
			{
				changeExperienceTimes100(iXP * 100);
			}

			// bonus xp in combat from handicap?
			int iXPPercent = GC.getGame().getHandicapInfo().getCityStateFreeXPPercent();
			if (iXPPercent > 0)
			{
				changeExperiencePercent(iXPPercent);
			}
		}
	}


	// Is this Unit immobile?
	if(getUnitInfo().IsImmobile())
	{
		SetImmobile(true);
	}

	setMoves(maxMoves());

	// Religious unit? If so takes religion from city
	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
		CvCity *pCity = plot()->getEffectiveOwningCity();
		if (pCity)
		{
			ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
			m_Religion.SetFullStrength(pCity->getOwner(),getUnitInfo(),eReligion);
		}
	}
	else if (MOD_GLOBAL_RELIGIOUS_SETTLERS && MOD_BALANCE_CORE_SETTLER_ADVANCED && (getUnitInfo().IsFound() || getUnitInfo().IsFoundAbroad() || getUnitInfo().IsFoundMid() || getUnitInfo().IsFoundLate() || getUnitInfo().GetNumColonyFound() > 0))
	{
		ReligionTypes eReligion = RELIGION_PANTHEON;

		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			CvCityReligions *pCityReligions = pPlotCity->GetCityReligions();

			int totalFollowers = pPlotCity->getPopulation();
			int randFollower = GC.getGame().randRangeInclusive(1, totalFollowers, CvSeeder(GET_PLAYER(pPlotCity->getOwner()).GetPseudoRandomSeed()));

			for (int i = RELIGION_PANTHEON; i < GC.getNumReligionInfos(); i++)
			{
				int theseFollowers = pCityReligions->GetNumFollowers((ReligionTypes) i);

				if (randFollower <= theseFollowers)
				{
					eReligion = (ReligionTypes) i;
					break;
				}

				randFollower = randFollower - theseFollowers;
				if (randFollower < 0) break;
			}
		}

		m_Religion.SetReligion(eReligion);
	}

	else if (MOD_GLOBAL_RELIGIOUS_SETTLERS && (getUnitInfo().IsFound() || getUnitInfo().IsFoundAbroad()))
	{
		ReligionTypes eReligion = RELIGION_PANTHEON;

		CvCity *pPlotCity = plot()->getPlotCity();
		if (pPlotCity)
		{
			CvCityReligions *pCityReligions = pPlotCity->GetCityReligions();

			int totalFollowers = pPlotCity->getPopulation();
			int randFollower = GC.getGame().urandRangeInclusive(1, totalFollowers, CvSeeder(GET_PLAYER(pPlotCity->getOwner()).GetPseudoRandomSeed()));

			for (int i = RELIGION_PANTHEON; i < GC.getNumReligionInfos(); i++)
			{
				int theseFollowers = pCityReligions->GetNumFollowers((ReligionTypes) i);

				if (randFollower <= theseFollowers)
				{
					eReligion = (ReligionTypes) i;
					break;
				}

				randFollower = randFollower - theseFollowers;
				if (randFollower < 0) break;
			}
		}

		m_Religion.SetReligion(eReligion);
	}

	// Instant Yields/Bonuses on Expend
	if (getUnitInfo().GetOneShotTourism() > 0)
	{
		SetTourismBlastStrength(getBlastTourism());
	}
	if (getUnitInfo().GetTourismBonusTurns() > 0)
	{
		SetTourismBlastLength(getBlastTourismTurns());
	}
	if (getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		SetScienceBlastStrength(getDiscoverAmount());
	}
	if (getUnitInfo().GetBaseHurry() > 0 || getUnitInfo().GetBaseProductionTurnsToCount() > 0)
	{
		SetHurryStrength(getHurryProduction(plot()));
	}
	if (getUnitInfo().GetBaseGold() > 0 || getUnitInfo().GetBaseGoldTurnsToCount() > 0)
	{
		SetGoldBlastStrength(getTradeGold());
	}
	if (getUnitInfo().GetBaseCultureTurnsToCount() > 0)
	{
		SetCultureBlastStrength(getGivePoliciesCulture());
	}
	if (getUnitInfo().GetBaseTurnsForGAPToCount() > 0)
	{
		SetGAPBlastStrength(getGAPBlast());
	}

	int iTourism = kPlayer.GetPlayerPolicies()->GetTourismFromUnitCreation((UnitClassTypes)(getUnitInfo().GetUnitClassType()));
	if (iTourism > 0)
	{
		kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourism);
	}

	// Recon unit? If so, he sees what's around him
	if(IsRecon())
	{
		setReconPlot(plot());
	}

	if(getDomainType() == DOMAIN_LAND)
	{
		if(IsCombatUnit())
		{
			if((GC.getGame().getBestLandUnit() == NO_UNIT) || (GetBaseCombatStrength() > GC.getGame().getBestLandUnitCombat()))
			{
				GC.getGame().setBestLandUnit(getUnitType());
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass(getUnitClassType());
	if (eGreatPerson != NO_GREATPERSON && bHistoric)
	{
		CvCity* pCity = plot()->getEffectiveOwningCity();
		if (pCity != NULL && pCity->getOwner() == GetID())
		{
			GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_GP_BORN, false, eGreatPerson, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		}
		else if (GET_PLAYER(getOwner()).getCapitalCity() != NULL)
		{
			GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_GP_BORN, false, eGreatPerson, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, GET_PLAYER(getOwner()).getCapitalCity());
		}
		GET_PLAYER(getOwner()).doInstantGWAM(eGreatPerson, getGreatName());
	}
#endif
	// Update UI
	if(getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->setDirty(GameData_DIRTY_BIT, true);
		kPlayer.GetUnitCycler().AddUnit(GetID());
	}

	// Message for World Unit being born
	if(isWorldUnitClass((UnitClassTypes)(getUnitInfo().GetUnitClassType())))
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive() && GC.getGame().getActivePlayer())
			{
				if(GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX(), getY(), true, true*/);
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_UNKNOWN_CREATED_UNIT", getNameKey());
					DLLUI->AddMessage(0, ((PlayerTypes)iI), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_WONDER_UNIT_BUILD", MESSAGE_TYPE_MAJOR_EVENT, getUnitInfo().GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT")*/);
				}
			}
		}

		strBuffer = GetLocalizedText("TXT_KEY_MISC_SOMEONE_CREATED_UNIT", kPlayer.getNameKey(), getNameKey());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), strBuffer, getX(), getY());
	}

	// Update City Strength
	CvPlot* pUnitPlot = plot();
	if(pUnitPlot && pUnitPlot->isCity())
	{
		pUnitPlot->getPlotCity()->SetGarrison( pUnitPlot->getBestGarrison( getOwner() ) );
	}

#if defined(MOD_BALANCE_CORE)
	DoLocationPromotions(true);
#endif
	m_iArmyId = -1;

	m_eUnitAIType = eUnitAI;
#if defined(MOD_BALANCE_CORE)
	if(IsGreatPerson() && bHistoric)
	{
		int iTourism = kPlayer.GetHistoricEventTourism(HISTORIC_EVENT_GREAT_PERSON);
		kPlayer.ChangeNumHistoricEvents(HISTORIC_EVENT_GREAT_PERSON, 1);
		// Culture boost based on previous turns
		if(iTourism > 0)
		{
			kPlayer.GetCulture()->AddTourismAllKnownCivsWithModifiers(iTourism);
			if(kPlayer.GetID() == GC.getGame().getActivePlayer())
			{
				char text[256] = {0};
				sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourism);
				SHOW_PLOT_POPUP(plot(), kPlayer.GetID(), text);

				CvNotifications* pNotification = kPlayer.GetNotifications();
				if(pNotification)
				{
					CvString strMessage;
					CvString strSummary;
					strMessage = GetLocalizedText("TXT_KEY_TOURISM_EVENT_GP", iTourism);
					strSummary = GetLocalizedText("TXT_KEY_TOURISM_EVENT_SUMMARY");
					pNotification->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, strMessage, strSummary, getX(), getY(), kPlayer.GetID());
				}
			}
		}
		CvPlayer* pPlayer = &GET_PLAYER(getOwner());
		CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
		if (pPlayer)
		{
			int iGATurnsfromGPBirth = pPlayer->GetPlayerTraits()->GetGoldenAgeFromGreatPersonBirth(GetGreatPersonFromUnitClass(getUnitClassType())); // Get number of GA turns as defined in table Trait_GoldenAgeFromGreatPersonBirth for this GP type
			if (iGATurnsfromGPBirth > 0) // If someone defined a positive value (no such thing as negative GA turns!)
			{
				pPlayer->changeGoldenAgeTurns(pPlayer->getGoldenAgeLength(iGATurnsfromGPBirth), true);
			}
		}
	}
#endif
#if defined(MOD_BALANCE_CORE)
	if (IsGreatGeneral() && kPlayer.GetPlayerTraits()->GetInspirationalLeader() != 0)
	{
		int iLoop = 0;
		CvUnit* pLoopUnit = NULL;
		for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kPlayer.nextUnit(&iLoop))
		{
			if (!pLoopUnit)
			{
				continue;
			}
			if (pLoopUnit->IsCombatUnit() && pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				pLoopUnit->changeDamage(-pLoopUnit->getDamage());
				pLoopUnit->changeExperienceTimes100(kPlayer.GetPlayerTraits()->GetInspirationalLeader() * 100);
			}
		}
	}
#endif
	// Update Unit Production Maintenance
	kPlayer.UpdateUnitProductionMaintenanceMod();

	// Minor Civ quest test
	if (IsGreatPerson() && kPlayer.isMajorCiv())
	{
		for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			// Does this Minor want us to spawn a Unit?
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
			if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				GET_PLAYER(eMinor).GetMinorCivAI()->DoTestActiveQuestsForPlayer(getOwner(), /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_GREAT_PERSON);
		}
	}

	//safety first
	if(GC.getMap().plot(iX,iY)->needsEmbarkation(this))
	{
		if (CanEverEmbark())
			setEmbarked(true);
		else
			CUSTOMLOG("warning: putting un-embarkable unit on water plot!\n");
	}
	
	if(bSetupGraphical)
		setupGraphical();
		
#if defined(MOD_EVENTS_UNIT_CREATED)
	if (MOD_EVENTS_UNIT_CREATED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCreated, getOwner(), GetID(), getUnitType(), getX(), getY());
	}
#endif
}


//	--------------------------------------------------------------------------------
void CvUnit::uninit()
{
	VALIDATE_OBJECT
	uninitInfos();

	m_missionQueue.clear();
	m_Promotions.Uninit();
}


//	--------------------------------------------------------------------------------
// FUNCTION: reset()
// Initializes data members that are serialized.
void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	VALIDATE_OBJECT
	int iI = 0;

	FAutoArchive& archive = getSyncArchive();
	archive.clearDelta();
	archive.reset();

	m_iID = iID;
	m_iHotKeyNumber = -1;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_iLastMoveTurn = 0;
	m_iCycleOrder = -1;
	m_iDeployFromOperationTurn = -100;
	m_iReconX = INVALID_PLOT_COORD;
	m_iReconY = INVALID_PLOT_COORD;
	m_iReconCount = 0;
	m_iGameTurnCreated = 0;
	m_iDamage = 0;
	m_iMoves = 0;
#if defined(MOD_LINKED_MOVEMENT)
	m_bIsLinked = false;
	m_bIsLinkedLeader = false;
	m_bIsGrouped = false;
	m_iLinkedMaxMoves = 0;
	m_LinkedUnitIDs.clear();
	m_iLinkedLeaderID = -1;
#endif
#if defined(MOD_SQUADS)
	m_iSquadNumber = -1;
	m_iSquadDestinationX = -1;
	m_iSquadDestinationY = -1;
	m_SquadEndMovementType = ALERT_ON_ARRIVAL;
#endif
	m_bImmobile = false;
	m_iExperienceTimes100 = 0;
	m_iLevel = 1;
	m_iCargo = 0;
	m_iAttackPlotX = INVALID_PLOT_COORD;
	m_iAttackPlotY = INVALID_PLOT_COORD;
	m_iCombatTimer = 0;
	m_bMovedThisTurn = false;
	m_bHasWithdrawnThisTurn = false;
	m_bFortified = false;
	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCrossingNoPenaltyCount = 0;
	m_iEnemyRouteCount = 0;
	m_iRivalTerritoryCount = 0;
	m_iIsSlowInEnemyLandCount = 0;
	m_iRangeAttackIgnoreLOSCount = 0;
	m_iCityAttackOnlyCount = 0;
	m_iCaptureDefeatedEnemyCount = 0;
#if defined(MOD_BALANCE_CORE)
	m_iOriginCity = -1;
	m_iCannotBeCapturedCount = 0;
	m_iForcedDamage = 0;
	m_iChangeDamage = 0;
#endif
	m_iRangedSupportFireCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHealOutsideFriendlyCount = 0;
	m_iHillsDoubleMoveCount = 0;
	m_iRiverDoubleMoveCount = 0;
#if defined(MOD_BALANCE_CORE)
	m_iMountainsDoubleMoveCount = 0;
	m_iEmbarkFlatCostCount = 0;
	m_iDisembarkFlatCostCount = 0;
	m_iAOEDamageOnKill = 0;
	m_iAOEDamageOnPillage = 0;
	m_iAoEDamageOnMove = 0;
	m_iPartialHealOnPillage = 0;
	m_iSplashDamage = 0;
	m_iMultiAttackBonus = 0;
	m_iLandAirDefenseValue = 0;
#endif
	m_iExtraVisibilityRange = 0;
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	m_iExtraReconRange = 0;
#endif
	m_iExtraMoves = 0;
	m_iExtraMoveDiscount = 0;
	m_iExtraRange = 0;
	m_iInterceptChance = 0;
#if defined(MOD_BALANCE_CORE)
	m_iExtraAirInterceptRange = 0; // JJ: This is new
#endif
	m_iExtraEvasion = 0;
	m_iExtraWithdrawal = 0;
#if defined(MOD_BALANCE_CORE_JFD)
	m_eUnitContract = NO_CONTRACT;
	m_iNegatorPromotion = -1;
#endif
	m_bIsNoMaintenance = false;
	m_iExtraEnemyHeal = 0;
	m_iExtraNeutralHeal = 0;
	m_iExtraFriendlyHeal = 0;
	m_iSameTileHeal = 0;
	m_iAdjacentTileHeal = 0;
	m_iEnemyDamageChance = 0;
	m_iNeutralDamageChance = 0;
	m_iEnemyDamage = 0;
	m_iNeutralDamage = 0;
	m_iNearbyEnemyCombatMod = 0;
	m_iNearbyEnemyCombatRange = 0;
	m_iExtraCombatPercent = 0;
	m_iAdjacentModifier = 0;
	m_iNoAdjacentUnitModifier = 0;
	m_iRangedAttackModifier = 0;
	m_iInterceptionCombatModifier = 0;
	m_iInterceptionDefenseDamageModifier = 0;
	m_iAirSweepCombatModifier = 0;
	m_iAttackModifier = 0;
	m_iDefenseModifier = 0;
	m_iGroundAttackDamage = 0;
	m_iExtraCityAttackPercent = 0;
	m_iExtraCityDefensePercent = 0;
	m_iExtraRangedDefenseModifier = 0;
	m_iExtraHillsAttackPercent = 0;
	m_iExtraHillsDefensePercent = 0;
	m_iExtraOpenAttackPercent = 0;
	m_iExtraOpenRangedAttackMod= 0;
	m_iExtraRoughAttackPercent = 0;
	m_iExtraRoughRangedAttackMod= 0;
	m_iExtraAttackFortifiedMod= 0;
	m_iExtraAttackWoundedMod= 0;
	m_iExtraFullyHealedMod = 0;
	m_iExtraAttackAboveHealthMod = 0;
	m_iExtraAttackBelowHealthMod = 0;
	m_iRangedFlankAttack = 0;
	m_iFlankPower = 1;
	m_iFlankAttackModifier = 0;
	m_iExtraOpenDefensePercent = 0;
	m_iExtraRoughDefensePercent = 0;
	m_iExtraOpenFromPercent = 0;
	m_iExtraRoughFromPercent = 0;
	m_iPillageChange = 0;
	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_iDropRange = 0;
	m_iAirSweepCapableCount = 0;
	m_iExtraNavalMoves = 0;
	m_eFacingDirection = DIRECTION_SOUTHEAST;
	m_iIgnoreTerrainCostCount = 0;
	m_iIgnoreTerrainDamageCount = 0;
	m_iIgnoreFeatureDamageCount = 0;
	m_iExtraTerrainDamageCount = 0;
	m_iExtraFeatureDamageCount = 0;
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	m_iNearbyImprovementCombatBonus = 0;
	m_iNearbyImprovementBonusRange = 0;
	m_eCombatBonusImprovement = NO_IMPROVEMENT;
#endif
#if defined(MOD_BALANCE_CORE)
	m_iNearbyUnitClassBonus = 0;
	m_iNearbyUnitClassBonusRange = 0;
	m_iCombatBonusFromNearbyUnitClass = NO_UNITCLASS;
	m_iNearbyPromotion = false;
	m_iNearbyUnitPromotionRange = 0;
	m_iNearbyCityCombatMod = 0;
	m_iNearbyFriendlyCityCombatMod = 0;
	m_iNearbyEnemyCityCombatMod = 0;
	m_iPillageBonusStrengthPercent = 0;
	m_iStackedGreatGeneralExperience = 0;
	m_iWonderProductionModifier = 0;
	m_iUnitProductionModifier = 0;
	m_iNearbyEnemyDamage = 0;
	m_iAdjacentCityDefenseMod = 0;
	m_iGGGAXPPercent = 0;
	m_eGiveDomain = NO_DOMAIN;
	m_iGiveCombatMod = 0;
	m_iGiveHPIfEnemyKilled = 0;
	m_iGiveExperiencePercent = 0;
	m_iGiveOutsideFriendlyLandsModifier = 0;
	m_iGiveExtraAttacks = 0;
	m_iGiveDefenseMod = 0;
	m_iGiveInvisibility = false;
	m_bGiveOnlyOnStartingTurn = false;
	m_iConvertUnit = false;
	m_eConvertDomainUnit = NO_UNIT;
	m_eConvertDomain = NO_DOMAIN;
	m_iConvertEnemyUnitToBarbarian = 0;
	m_bConvertOnFullHP = false;
	m_bConvertOnDamage = false;
	m_iDamageThreshold = 0;
	m_eConvertDamageOrFullHPUnit = NO_UNIT;
	m_iNumberOfCultureBombs = 0;
	m_iNearbyHealEnemyTerritory = 0;
	m_iNearbyHealNeutralTerritory = 0;
	m_iNearbyHealFriendlyTerritory = 0;
#endif
#if defined(MOD_CIV6_WORKER)
	m_iBuilderStrength = 0;
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	m_iCanCrossMountainsCount = 0;
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	m_iCanCrossOceansCount = 0;
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	m_iCanCrossIceCount = 0;
#endif
#if defined(MOD_BALANCE_CORE)
	m_iNumTilesRevealedThisTurn = 0;
	m_bSpottedEnemy = false;
	m_iGainsXPFromScouting = 0;
	m_iGainsXPFromSpotting = 0;
	m_iGainsXPFromPillaging = 0;
	m_iCaptureDefeatedEnemyChance = 0;
	m_iBarbCombatBonus = 0;
	m_iAdjacentEnemySapMovement = 0;
	m_iCanMoraleBreak = 0;
	m_iDamageAoEFortified = 0;
	m_iWorkRateMod = 0;
	m_iDamageReductionCityAssault = 0;
	m_iStrongerDamaged = 0;
	m_iFightWellDamaged = 0;
	m_iGoodyHutYieldBonus = 0;
	m_iReligiousPressureModifier = 0;
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	m_iGGFromBarbariansCount = 0;
#endif
	m_iRoughTerrainEndsTurnCount = 0;
	m_iCapturedUnitsConscriptedCount = 0;
	m_iEmbarkAbilityCount = 0;
	m_iHoveringUnitCount = 0;
	m_iFlatMovementCostCount = 0;
	m_iCanMoveImpassableCount = 0;
	m_iOnlyDefensiveCount = 0;
	m_iNoAttackInOceanCount = 0;
	m_iNoDefensiveBonusCount = 0;
	m_iNoCaptureCount = 0;
	m_iNukeImmuneCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHiddenNationalityCount = 0;
	m_iAlwaysHostileCount = 0;
	m_iNoRevealMapCount = 0;
	m_iCanMoveAllTerrainCount = 0;
	m_iCanMoveAfterAttackingCount = 0;
	m_iFreePillageMoveCount = 0;
	m_iHPHealedIfDefeatEnemy = 0;
	m_iGoldenAgeValueFromKills = 0;
	m_iSapperCount = 0;
	m_iCanHeavyCharge = 0;
	m_iNumExoticGoods = 0;
	m_iTacticalAIPlotX = INVALID_PLOT_COORD;
	m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	m_iGarrisonCityID = -1;   // unused
	m_iGreatGeneralCount = 0;
	m_iGreatAdmiralCount = 0;
	m_iAuraRangeChange = 0;
	m_iAuraEffectChange = 0;
	m_iNumRepairCharges = 0;
	m_iMilitaryCapChange = 0;
	m_iGreatGeneralModifier = 0;
	m_iGreatGeneralReceivesMovementCount = 0;
	m_iGreatGeneralCombatModifier = 0;
	m_iIgnoreGreatGeneralBenefit = 0;
	m_iIgnoreZOC = 0;
	m_iNoSupply = 0;
	m_iHealIfDefeatExcludeBarbariansCount = 0;
	m_iNumInterceptions = 1;
	m_iMadeInterceptionCount = 0;
	m_iEverSelectedCount = 0;

	m_iEmbarkedAllWaterCount = 0;
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	m_iEmbarkedDeepWaterCount = 0;
#endif
#if defined(MOD_CORE_PER_TURN_DAMAGE)
	m_iDamageTakenThisTurn = 0;
	m_iDamageTakenLastTurn = 0;
#endif
	m_iEmbarkExtraVisibility = 0;
	m_iEmbarkDefensiveModifier = 0;
	m_iCapitalDefenseModifier = 0;
	m_iCapitalDefenseFalloff = 0;
	m_iCityAttackPlunderModifier = 0;
	m_iReligiousStrengthLossRivalTerritory = 0;
	m_iTradeMissionInfluenceModifier = 0;
	m_iTradeMissionGoldModifier = 0;
	m_iDiploMissionInfluence = 0;

	m_bPromotionReady = false;
	m_bDeathDelay = false;
	m_bCombatFocus = false;
	m_bInfoBarDirty = false;
	m_bNotConverting = false;
	m_bAirCombat = false;
	m_bEmbarked = false;
	m_bPromotedFromGoody = false;
	m_bAITurnProcessed = false;
	m_bWaitingForMove = false;
	m_eOwner = eOwner;
	m_eOriginalOwner = eOwner;
	m_eGiftedByPlayer = NO_PLAYER;
	m_eCapturingPlayer = NO_PLAYER;
	m_bCapturedAsIs = false;
	m_bCapturedAsConscript = false;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? GC.getUnitInfo(m_eUnitType) : NULL;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetCombat() : 0;
	m_iBaseRangedCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetRangedCombat() : 0;
	m_eCombatType = (NO_UNIT != m_eUnitType) ? (UnitCombatTypes)m_pUnitInfo->GetUnitCombatType() : NO_UNITCOMBAT;
	m_iNumAttacks = (m_iBaseCombat > 0 || m_iBaseRangedCombat > 0) ? 1 : 0;
	m_iAttacksMade = 0;
	m_iMaxHitPointsBase = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->GetMaxHitPoints() : GD_INT_GET(MAX_HIT_POINTS);
	m_iMaxHitPointsChange = 0;
	m_iMaxHitPointsModifier = 0;
	m_eLeaderUnitType = NO_UNIT;
	m_eInvisibleType = NO_INVISIBLE;
	m_eSeeInvisibleType = NO_INVISIBLE;
	m_eGreatPeopleDirectiveType = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	m_iCargoCapacity = 0;

	m_combatUnit.reset();
	m_transportUnit.reset();
	m_extraDomainModifiers.clear();
	m_extraDomainAttacks.clear();
	m_extraDomainDefenses.clear();

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_extraDomainModifiers.push_back(0);
		m_extraDomainAttacks.push_back(0);
		m_extraDomainDefenses.push_back(0);
	}

	m_YieldModifier.clear();
	m_YieldChange.clear();
	m_iGarrisonYieldChange.clear();
	m_iFortificationYieldChange.clear();
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_YieldModifier.push_back(0);
		m_YieldChange.push_back(0);
		m_iGarrisonYieldChange.push_back(0);
		m_iFortificationYieldChange.push_back(0);
	}

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	m_strUnitName = "";
#endif
	m_strName = "";
#if defined(MOD_GLOBAL_NO_LOST_GREATWORKS)
	m_strGreatName = "";
#endif
	m_eGreatWork = NO_GREAT_WORK;
	m_iTourismBlastStrength = 0;
	m_iTourismBlastLength = 0;
	m_strScriptData ="";
	m_iScenarioData = 0;

	m_unitMoveLocs.clear();

	uninitInfos();

	// Migrated in from CvSelectionGroup
	m_iMissionTimer = 0;
	m_eActivityType = ACTIVITY_AWAKE;
	m_eAutomateType = NO_AUTOMATE;

	ClearPathCache();

#if defined(MOD_BALANCE_CORE)
	m_iHurryStrength = 0;
	m_iGoldBlastStrength = 0;
	m_iScienceBlastStrength = 0;
	m_iCultureBlastStrength = 0;
	m_iGAPBlastStrength = 0;
	m_abPromotionEverObtained = std::vector<bool>(GC.getNumPromotionInfos(),false);
#endif

	m_iMapLayer = DEFAULT_UNIT_MAP_LAYER;
	m_iNumGoodyHutsPopped = 0;

	m_eTacticalMove = AI_TACTICAL_MOVE_NONE;
	m_iTacticalMoveSetTurn = 0;
	m_eHomelandMove = AI_HOMELAND_MOVE_NONE;
	m_iHomelandMoveSetTurn = 0;
	m_strMissionInfoString = "";

	if(!bConstructorCall)
	{
		m_Promotions.Reset();

		m_ignoreTerrainCostInCount.clear();
		m_ignoreTerrainCostFromCount.clear();
		m_terrainDoubleMoveCount.clear();
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		m_terrainHalfMoveCount.clear();
		m_terrainExtraMoveCount.clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_terrainDoubleHeal.clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_PromotionDuration.clear();
		m_TurnPromotionGained.clear();
#endif
		m_terrainImpassableCount.clear();
		m_extraTerrainAttackPercent.clear();
		m_extraTerrainDefensePercent.clear();

		m_ignoreFeatureCostInCount.clear();
		m_ignoreFeatureCostFromCount.clear();
		m_featureDoubleMoveCount.clear();
#if defined(MOD_PROMOTIONS_HALF_MOVE)
		m_featureHalfMoveCount.clear();
		m_featureExtraMoveCount.clear();
#endif
#if defined(MOD_BALANCE_CORE)
		m_featureDoubleHeal.clear();
#endif
		m_featureImpassableCount.clear();
		m_extraFeatureDefensePercent.clear();
		m_extraFeatureAttackPercent.clear();

		m_extraUnitClassAttackMod.clear();
		m_extraUnitClassDefenseMod.clear();

		m_yieldFromKills.clear();
		m_yieldFromBarbarianKills.clear();
		m_aiNumTimesAttackedThisTurn.clear();
		m_aiNumTimesAttackedThisTurn.resize(REALLY_MAX_PLAYERS);
		m_yieldFromScouting.clear();
		
		m_yieldFromKills.resize(NUM_YIELD_TYPES);
		m_yieldFromBarbarianKills.resize(NUM_YIELD_TYPES);
		m_yieldFromScouting.resize(NUM_YIELD_TYPES);

		for(int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			m_yieldFromKills[i] = 0;
			m_yieldFromBarbarianKills[i] = 0;
			m_yieldFromScouting[i] = 0;
		}

		for (int iI = 0; iI < REALLY_MAX_PLAYERS; iI++)
		{
			m_aiNumTimesAttackedThisTurn[iI] =  0;
		}

		CvAssertMsg((0 < GC.getNumUnitCombatClassInfos()), "GC.getNumUnitCombatClassInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_extraUnitCombatModifier.clear();
		m_extraUnitCombatModifier.resize(GC.getNumUnitCombatClassInfos());
#if defined(MOD_BALANCE_CORE)
		m_iCombatModPerAdjacentUnitCombatModifier.clear();
		m_iCombatModPerAdjacentUnitCombatModifier.resize(GC.getNumUnitCombatClassInfos());
		m_iCombatModPerAdjacentUnitCombatAttackMod.clear();
		m_iCombatModPerAdjacentUnitCombatAttackMod.resize(GC.getNumUnitCombatClassInfos());
		m_iCombatModPerAdjacentUnitCombatDefenseMod.clear();
		m_iCombatModPerAdjacentUnitCombatDefenseMod.resize(GC.getNumUnitCombatClassInfos());
#endif
		for(int i = 0; i < GC.getNumUnitCombatClassInfos(); i++)
		{
			m_extraUnitCombatModifier[i] = 0;
#if defined(MOD_BALANCE_CORE)
			m_iCombatModPerAdjacentUnitCombatModifier[i] = 0;
			m_iCombatModPerAdjacentUnitCombatAttackMod[i] = 0;
			m_iCombatModPerAdjacentUnitCombatDefenseMod[i] = 0;
#endif
		}

		m_unitClassModifier.clear();
		m_unitClassModifier.resize(GC.getNumUnitClassInfos());
		for(int i = 0; i < GC.getNumUnitClassInfos(); i++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)i);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			m_unitClassModifier[i] = 0;
		}

		m_yieldFromPillage.clear();

		// Migrated in from CvSelectionGroup
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
		m_eMissionAIType = NO_MISSIONAI;
		m_missionAIUnit.reset();

		m_eUnitAIType = NO_UNITAI;
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvUnit::setupGraphical()
{
	VALIDATE_OBJECT
	if(!GC.IsGraphicsInitialized())
	{
		return;
	}

	ICvEngineUtility1* pDLL = GC.getDLLIFace();
	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));

	if(isEmbarked())
	{
		pDLL->GameplayUnitEmbark(pDllUnit.get(), true);
	}
	else
	{
		pDLL->GameplayUnitCreated(pDllUnit.get());

		pDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ getMoves() <= 0);

		pDLL->GameplayUnitFortify(pDllUnit.get(), IsFortified());

		int iNewValue = getDamage();
		if(iNewValue > 0)
		{
			pDLL->GameplayUnitSetDamage(pDllUnit.get(), iNewValue, 0);
		}

		if(IsWork())
		{
			BuildTypes currentBuild = getBuildType();
			if(currentBuild != NO_BUILD)
			{
				pDLL->GameplayUnitWork(pDllUnit.get(), currentBuild);
			}
		}

		if(IsGarrisoned())
		{
			pDLL->GameplayUnitVisibility(pDllUnit.get(), false);
			pDLL->GameplayUnitGarrison(pDllUnit.get(), true);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::uninitInfos()
{
	VALIDATE_OBJECT
	m_ignoreTerrainCostInCount.clear();
	m_ignoreTerrainCostFromCount.clear();
	m_ignoreFeatureCostInCount.clear();
	m_ignoreFeatureCostFromCount.clear();
	m_terrainDoubleMoveCount.clear(); // BUG FIX
	m_featureDoubleMoveCount.clear();
	m_terrainHalfMoveCount.clear();
	m_featureHalfMoveCount.clear();
	m_terrainExtraMoveCount.clear();
	m_featureExtraMoveCount.clear();
#if defined(MOD_BALANCE_CORE)
	m_PromotionDuration.clear();
	m_TurnPromotionGained.clear();
#endif
#if defined(MOD_BALANCE_CORE)
	m_terrainDoubleHeal.clear();
	m_featureDoubleHeal.clear();
#endif
	m_terrainImpassableCount.clear();
	m_featureImpassableCount.clear();
	m_extraTerrainAttackPercent.clear();
	m_extraTerrainDefensePercent.clear();
	m_extraFeatureAttackPercent.clear();
	m_extraFeatureDefensePercent.clear();

	m_extraUnitClassAttackMod.clear();
	m_extraUnitClassDefenseMod.clear();
#if defined(MOD_BALANCE_CORE)
	m_iCombatModPerAdjacentUnitCombatModifier.clear();
	m_iCombatModPerAdjacentUnitCombatAttackMod.clear();
	m_iCombatModPerAdjacentUnitCombatDefenseMod.clear();
#endif
	m_yieldFromKills.clear();
	m_yieldFromBarbarianKills.clear();
#if defined(MOD_BALANCE_CORE)
	m_yieldFromScouting.clear();
#endif
	m_extraUnitCombatModifier.clear();
	m_unitClassModifier.clear();
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsRetainablePromotion(PromotionTypes ePromotion)
{
	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	CvAssert(pkPromotionInfo);
	if (pkPromotionInfo)
	{
		if (pkPromotionInfo->IsEmbarkedDeepWater())
		{
			// Deep water promotions are handled as special cases of the general embark promotion
			return false;
		}
		else if (static_cast<int>(ePromotion) == GD_INT_GET(PROMOTION_OCEAN_IMPASSABLE))
		{
			// Handled by unit initialization
			return false;
		}
		else if (static_cast<int>(ePromotion) == GD_INT_GET(PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY))
		{
			// Handled by unit initialization
			return false;
		}
		else if (pkPromotionInfo->IsConvertOnDamage())
		{
			// Not certain this actually matters
			return false;
		}
		else if (pkPromotionInfo->IsConvertOnFullHP())
		{
			// Not certain this actually matters
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::CalcExperienceTimes100ForConvert(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iExperienceTimes100)
{
	if (eFromPlayer == eToPlayer)
	{
		return iExperienceTimes100;
	}
	else
	{
		int iOldModifier = std::max(1, 100 + GET_PLAYER(eFromPlayer).getLevelExperienceModifier());
		int iOurModifier = std::max(1, 100 + GET_PLAYER(eToPlayer).getLevelExperienceModifier());
		return std::max(0, (iExperienceTimes100 * iOurModifier) / iOldModifier);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::grantExperienceFromLostPromotions(int iNumLost)
{
	// 20 xp per lost 'good' promotion (plus level).
	if (iNumLost > 0)
	{
		changeExperienceTimes100(20 * iNumLost * std::max(1, getLevel()) * 100);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::convert(CvUnit* pUnit, bool bIsUpgrade)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = NULL;
	CvUnit* pTransportUnit = NULL;
	CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;

	pPlot = plot();
	int iLostPromotions = 0;

	// Transfer Promotions over
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo)
		{
			bool bGivePromotion = false;
			bool bFree = false;

			if (!IsRetainablePromotion(ePromotion))
			{
				continue;
			}

			// Old unit has the promotion
			if (pUnit->isHasPromotion(ePromotion) && !pkPromotionInfo->IsLostWithUpgrade())
			{
				bGivePromotion = true;
			}

			// New unit gets promotion for free (as per XML)
			else if (getUnitInfo().GetFreePromotions(ePromotion) && (!bIsUpgrade || !pkPromotionInfo->IsNotWithUpgrade()))
			{
				bFree = true;
				bGivePromotion = true;
			}

			// if we get this due to a policy or wonder
			else if (GET_PLAYER(getOwner()).IsFreePromotion(ePromotion) && (
						::IsPromotionValidForUnitCombatType(ePromotion, getUnitType()) || ::IsPromotionValidForCivilianUnitType(ePromotion, getUnitType())))
			{
				bFree = true;
				bGivePromotion = true;
			}
			if (pUnit->getUnitCombatType() != getUnitCombatType())
			{
				bool bMelee = false;
				bool bRanged = false;
				if ((pkPromotionInfo->GetRangedAttackModifier() > 0) || (pkPromotionInfo->GetRangeChange() > 0) || (pkPromotionInfo->IsRangeAttackIgnoreLOS()) || (pkPromotionInfo->GetOpenRangedAttackMod() > 0) || (pkPromotionInfo->GetRoughRangedAttackMod() > 0) || (pkPromotionInfo->GetExtraAttacks() > 0))
				{
					bRanged = true;
				}
				// Note: might be more Promotions than this for melee but these are the essentials of Drill and Shock. Things like Attack wounded, etc. can apply to ranged units on conversion from melee.
				if ((pkPromotionInfo->GetRoughAttackPercent() > 0) || (pkPromotionInfo->GetRoughDefensePercent() > 0) || (pkPromotionInfo->GetOpenAttackPercent() > 0) || (pkPromotionInfo->GetOpenDefensePercent()))
				{
					bMelee = true;
				}
				if ((pkPromotionInfo->GetCityAttackPlunderModifier() > 0) || (pkPromotionInfo->GetHPHealedIfDefeatEnemy() > 0))
				{
					bMelee = true;
				}
				//If we're losing standard promotions because of a combatclass change, let's replace with some experience.
				if (!IsCanAttackRanged() && pUnit->HasPromotion(ePromotion) && pUnit->IsCanAttackRanged() && bRanged && !bFree && !::IsPromotionValidForUnitCombatType(ePromotion, pUnit->getUnitType()))
				{
					iLostPromotions++;
					bGivePromotion = false;
				}
				//Naval Misfire Promotion Catch (sorry for hardcode)
				else if (!IsCanAttackRanged() && pUnit->HasPromotion(ePromotion) && pUnit->IsCanAttackRanged() && (pkPromotionInfo->GetDomainAttackPercent(DOMAIN_SEA) < 0) && (getDomainType() == DOMAIN_LAND))
				{
					iLostPromotions++;
					bGivePromotion = false;
				}
				// Siege Tower / Battering Ram Upgrade? Now becomes a UNITCOMBAT_SIEGE and a ranged unit
				else if (IsCanAttackRanged() && !pUnit->IsCanAttackRanged() && pUnit->HasPromotion(ePromotion) && bMelee && !bFree)
				{
					iLostPromotions++;
					bGivePromotion = false;
				}
			}

			setHasPromotion(ePromotion, bGivePromotion);
			if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && bGivePromotion && pkPromotionInfo->IsAllowsEmbarkation() && IsHoveringUnit())
			{
				setHasPromotion(GET_PLAYER(getOwner()).GetDeepWaterEmbarkationPromotion(), true);
			}
		}
	}
	setGameTurnCreated(pUnit->getGameTurnCreated());
	setLastMoveTurn(pUnit->getLastMoveTurn());
	// Don't kill the unit if upgrading from a unit with more base hit points!!!
	setDamage(min(pUnit->getDamage(), GetMaxHitPoints() - 1), NO_PLAYER, 0.0F, NULL, true);
	setMoves(pUnit->getMoves());
	setEmbarked(pUnit->isEmbarked());
	setFacingDirection(pUnit->getFacingDirection(false));
	SetBeenPromotedFromGoody(pUnit->IsHasBeenPromotedFromGoody());
	if (pUnit->hasMoved())
	{
		finishMoves();
	}

	// Instant Yields/Bonuses on Expend
	{
		int iTheirTourism = pUnit->GetTourismBlastStrength();
		if (iTheirTourism > GetTourismBlastStrength())
			SetTourismBlastStrength(iTheirTourism);

		int iTheirTourismTurns = pUnit->GetTourismBlastLength();
		if (iTheirTourismTurns > GetTourismBlastLength())
			SetTourismBlastLength(iTheirTourismTurns);

		int iTheirProduction = pUnit->GetHurryStrength();
		if (iTheirProduction > GetHurryStrength())
			SetHurryStrength(iTheirProduction);

		int iTheirGold = pUnit->GetGoldBlastStrength();
		if (iTheirGold > GetGoldBlastStrength())
			SetGoldBlastStrength(iTheirGold);

		int iTheirScience = pUnit->GetScienceBlastStrength();
		if (iTheirScience > GetScienceBlastStrength())
			SetScienceBlastStrength(iTheirScience);

		int iTheirCulture = pUnit->GetCultureBlastStrength();
		if (iTheirCulture > GetCultureBlastStrength())
			SetCultureBlastStrength(iTheirCulture);

		int iTheirGAP = pUnit->GetGAPBlastStrength();
		if (iTheirGAP > GetGAPBlastStrength())
			SetGAPBlastStrength(iTheirGAP);
	}

	if (pUnit->getOriginCity() == NULL)
	{
		if (plot() && plot()->getOwningCity() && plot()->getOwner() == getOwner())
		{
			setOriginCity(plot()->getOwningCity()->GetID());
		}
	}
	else
	{
		setOriginCity(pUnit->getOriginCity()->GetID());
	}
	if (pUnit->getUnitInfo().GetNumExoticGoods() > 0)
	{
		setNumExoticGoods(pUnit->getNumExoticGoods());
	}
	if (pUnit->getUnitCombatType() != getUnitCombatType() && (iLostPromotions > 0))
	{
		setLevel(1);
	}
	else
	{
		setLevel(pUnit->getLevel());
	}
	setExperienceTimes100(CalcExperienceTimes100ForConvert(pUnit->getOwner(), getOwner(), pUnit->getExperienceTimes100()), -1, true);
	grantExperienceFromLostPromotions(iLostPromotions);

	setName(pUnit->getNameNoDesc());
	setLeaderUnitType(pUnit->getLeaderUnitType());
	SetNumGoodyHutsPopped(pUnit->GetNumGoodyHutsPopped());

	pTransportUnit = pUnit->getTransportUnit();

	if (pTransportUnit != NULL)
	{
		pUnit->setTransportUnit(NULL);
		setTransportUnit(pTransportUnit);
	}

	pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == pUnit)
		{
			pLoopUnit->setTransportUnit(this);
		}
	}

	// Transfer original owner
	PlayerTypes eOriginalOwner = pUnit->GetOriginalOwner();
	if (eOriginalOwner != NO_PLAYER)
	{
		SetOriginalOwner(eOriginalOwner);
	}
	// Transfer gifted by player
	PlayerTypes eGiftedByPlayer = bIsUpgrade ? pUnit->GetGiftedByPlayer() : NO_PLAYER;
	if (eGiftedByPlayer != NO_PLAYER)
	{
		SetGiftedByPlayer(eGiftedByPlayer);
	}

	if (MOD_EVENTS_UNIT_CONVERTS)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitConverted, pUnit->getOwner(),getOwner(), pUnit->GetID(), GetID(), bIsUpgrade);
	}

	pUnit->kill(true, NO_PLAYER);
}

//	----------------------------------------------------------------------------
//	Kill a unit.
//	Parameters:
//		bDelay			- If true, the unit will be partially cleaned up, but its final removal will happen at the end of the frame.
//		ePlayer			- Optional player ID who is doing the killing.
void CvUnit::kill(bool bDelay, PlayerTypes ePlayer /*= NO_PLAYER*/)
{
	//nothing to do
	if (bDelay && isDelayedDeath())
		return;

	VALIDATE_OBJECT
	CvUnit* pTransportUnit = NULL;
	CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;
	CvString strBuffer;
	PlayerTypes eUnitOwner = getOwner();

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(eUnitOwner);
		args->Push(GetID());  // unit being killed

		bool bResult = false;
		if(LuaSupport::CallTestAny(pkScriptSystem, "CanSaveUnit", args.get(), bResult))
		{
			// Check the result.
			if(bResult)
			{
				return;   // Unit was saved
			}
		}
	}

	bool bCheckForMurder = ePlayer != NO_PLAYER && !GET_PLAYER(eUnitOwner).isBarbarian() && GET_PLAYER(eUnitOwner).getNumCities() <= 0 && (GET_PLAYER(eUnitOwner).getNumUnits() <= 1 || canFoundCity(NULL, true, true, true));

	if (MOD_UNIT_KILL_STATS && ePlayer != NO_PLAYER && !bDelay)
	{
		AITacticalMove move = getTacticalMove();
		saiTaskWhenKilled[(int)move + 1]++;

		if (GET_PLAYER(m_eOwner).isMajorCiv() && plot())
			GC.getMap().IncrementUnitKillCount(m_eOwner, plot()->GetPlotIndex());
	}

	CvInterfacePtr<ICvUnit1> pDllThisUnit = GC.WrapUnitPointer(this);

	if(IsSelected() && !bDelay)
	{
		DLLUI->VisuallyDeselectUnit(pDllThisUnit.get());
	}

	GET_PLAYER(eUnitOwner).removeFromArmy(m_iArmyId, GetID());
	ClearMissionQueue();

	pPlot = plot();
	CvAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

	if(pPlot)
	{
		IDInfo* pkOldUnits = NULL;
		uint uiOldUnitCount = pPlot->getNumUnits();
		const IDInfo* pUnitNode = pPlot->headUnitNode();

		if(pUnitNode)
		{
			pkOldUnits = (IDInfo*)_malloca(pPlot->getNumUnits() * sizeof(IDInfo));	// Allocate an array on the stack, it shouldn't be too large
			IDInfo* pkEntry = pkOldUnits;
			while(pUnitNode != NULL)
			{
				*pkEntry++ = *pUnitNode;
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
			}
		}

		for(uint i = 0; i < uiOldUnitCount; i++)
		{
			pLoopUnit = ::GetPlayerUnit(pkOldUnits[i]);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)	// Is this the transport for the search unit?
				{
					if(pLoopUnit->isSuicide() && pLoopUnit->isInCombat())
					{
						// Let suicide units (missiles & other single use items) kill themselves.
						pLoopUnit->setTransportUnit(NULL);
					}
					else
					{
						pLoopUnit->setCapturingPlayer(getCapturingPlayer());	// KWG: Creating a new captured cargo, but how does its transport (this) then get attached to the new cargo?
						pLoopUnit->kill(false, ePlayer);
					}
				}
			}
		}

		if(pkOldUnits)
			_freea(pkOldUnits);
	}
	// If a player killed this Unit...
	if (ePlayer != NO_PLAYER)
	{
#if defined(MOD_BALANCE_CORE)
		if (IsCombatUnit())
		{
			CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_DEATH);
		}
#endif
		if (!GET_PLAYER(ePlayer).isBarbarian() && ePlayer != eUnitOwner)
		{
			int iUnitValue = (getUnitInfo().GetPower() * 100);

			// Compare the strength of this unit to the strongest unit we can build in that domain, for an apples to apples comparison
			// Best unit that can be built now is given a value of 100
			if (!isBarbarian())
			{
				if (getDomainType() == DOMAIN_AIR)
				{
					int iTypicalAirPower = GET_PLAYER(eUnitOwner).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_AIR);
					if (iTypicalAirPower > 0)
					{
						iUnitValue /= iTypicalAirPower;
					}
					else
					{
						iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
					}
				}
				else if (getDomainType() == DOMAIN_SEA)
				{
					int iTypicalNavalPower = GET_PLAYER(eUnitOwner).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_SEA);
					if (iTypicalNavalPower > 0)
					{
						iUnitValue /= iTypicalNavalPower;
					}
					else
					{
						iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
					}
				}
				else
				{
					int iTypicalLandPower = GET_PLAYER(eUnitOwner).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);
					if (iTypicalLandPower > 0)
					{
						iUnitValue /= iTypicalLandPower;
					}
					else
					{
						iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
					}
				}
			}

			int iCivValue = 0;
			if (IsCivilianUnit() && (GetOriginalOwner() == eUnitOwner || isBarbarian()))
			{
				if (!IsCombatSupportUnit())
				{
					// AI cares less about lost workers / etc in lategame
					int iEraFactor = !isBarbarian() ? max(8 - (int)GET_PLAYER(eUnitOwner).GetCurrentEra(), 1) : (int)GC.getGame().getCurrentEra();

					if (IsGreatPerson())
					{
						iCivValue = 500 * iEraFactor;
					}
					else if (isFound() || IsFoundAbroad())
					{
						iCivValue = 300 * iEraFactor;
					}
					else
					{
						iCivValue = 100 * iEraFactor;
					}

					if (GC.getGame().getGameTurn() <= 100)
					{
						iCivValue *= 2;
					}

					iCivValue *= GC.getGame().getGameSpeedInfo().getTrainPercent();
					iCivValue /= 100;

					// Diplo penalty for killing civilians (doesn't apply if stationed in a city, since civilians aren't being targeted in particular)
					if (pPlot && !pPlot->isCity() && GET_PLAYER(eUnitOwner).isMajorCiv() && GET_PLAYER(ePlayer).isMajorCiv())
						GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeCivilianKillerValue(ePlayer, iCivValue);

					if (GET_PLAYER(eUnitOwner).isMajorCiv())
					{
						if (isFound() || IsFoundAbroad())
							GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeWarProgressScore(ePlayer, /*-10*/ GD_INT_GET(WAR_PROGRESS_LOST_SETTLER));
						else
							GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeWarProgressScore(ePlayer, /*-5*/ GD_INT_GET(WAR_PROGRESS_LOST_WORKER));
					}
					if (GET_PLAYER(ePlayer).isMajorCiv())
					{
						if (isFound() || IsFoundAbroad())
							GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeWarProgressScore(eUnitOwner, /*20*/ GD_INT_GET(WAR_PROGRESS_CAPTURED_SETTLER));
						else
							GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeWarProgressScore(eUnitOwner, /*10*/ GD_INT_GET(WAR_PROGRESS_CAPTURED_WORKER));
					}
				}
				else
				{
					if (GET_PLAYER(ePlayer).isMajorCiv())
						GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeWarProgressScore(eUnitOwner, /*20*/ GD_INT_GET(WAR_PROGRESS_KILLED_UNIT));

					if (GET_PLAYER(getOwner()).isMajorCiv())
						GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeWarProgressScore(ePlayer, /*-10*/ GD_INT_GET(WAR_PROGRESS_LOST_UNIT));
				}
			}

			if (pPlot && (iUnitValue > 0 || iCivValue > 0))
			{
				iCivValue /= 5;

				// Unit belongs to us - did our Master fail to protect one of our units?
				if (GET_PLAYER(eUnitOwner).isMajorCiv() && GET_PLAYER(getOwner()).IsVassalOfSomeone())
				{
					// Unit was killed inside my territory (or my teammate's territory)
					if (GET_PLAYER(pPlot->getOwner()).getTeam() == GET_PLAYER(eUnitOwner).getTeam())
					{
						// Loop through all masters and penalize them
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if (GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsVassal(eLoopPlayer))
							{
								if (iCivValue > 0)
									GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(eLoopPlayer, -iCivValue);
								else
									GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(eLoopPlayer, -iUnitValue);
							}
						}
					}
					// Unit was killed in our master's territory
					else if (GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsPlayerValid(pPlot->getOwner()) && GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsVassal(pPlot->getOwner()))
					{
						// Penalize the master whose territory we're in
						if (iCivValue > 0)
							GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(pPlot->getOwner(), -iCivValue);
						else
							GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(pPlot->getOwner(), -iUnitValue);
					}
					// Unit killed in neutral territory near one of the vassal's cities (currently disabled)
					else if (pPlot->getOwner() == NO_PLAYER && GET_PLAYER(eUnitOwner).GetCityDistanceInPlots(pPlot) <= /*0*/ GD_INT_GET(VASSALAGE_FAILED_PROTECT_CITY_DISTANCE))
					{
						// Loop through all masters and penalize them
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if (GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_PLAYER(eUnitOwner).GetDiplomacyAI()->IsVassal(eLoopPlayer))
							{
								if (iCivValue > 0)
									GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(eLoopPlayer, -iCivValue);
								else
									GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeVassalProtectValue(eLoopPlayer, -iUnitValue);
							}
						}
					}
				}
				// Did the killer protect some of his vassals? They should be grateful if a combat unit or non-Barbarian Settler was killed.
				if (GET_PLAYER(ePlayer).isMajorCiv() && GET_PLAYER(ePlayer).GetNumVassals() > 0)
				{
					if (iCivValue == 0 || (!isBarbarian() && (isFound() || IsFoundAbroad())))
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsMaster(eLoopPlayer))
							{
								// If the unit killed was a Barbarian combat unit, calculate unit value (comparing against the vassal's typical unit power)
								if (isBarbarian() && iCivValue == 0)
								{
									if (getDomainType() == DOMAIN_AIR)
									{
										int iTypicalAirPower = GET_PLAYER(eLoopPlayer).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_AIR);
										if (iTypicalAirPower > 0)
										{
											iUnitValue /= iTypicalAirPower;
										}
										else
										{
											iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
										}
									}
									else if (getDomainType() == DOMAIN_SEA)
									{
										int iTypicalNavalPower = GET_PLAYER(eLoopPlayer).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_SEA);
										if (iTypicalNavalPower > 0)
										{
											iUnitValue /= iTypicalNavalPower;
										}
										else
										{
											iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
										}
									}
									else
									{
										int iTypicalLandPower = GET_PLAYER(eLoopPlayer).GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);
										if (iTypicalLandPower > 0)
										{
											iUnitValue /= iTypicalLandPower;
										}
										else
										{
											iUnitValue = /*100*/ GD_INT_GET(DEFAULT_WAR_VALUE_FOR_UNIT);
										}
									}
								}

								// Unit killed in/adjacent to the vassal's territory or near one of the vassal's cities
								if (pPlot->getOwner() == eLoopPlayer || pPlot->isAdjacentPlayer(eLoopPlayer) || GET_PLAYER(eLoopPlayer).GetCityDistanceInPlots(pPlot) <= /*6*/ GD_INT_GET(VASSALAGE_PROTECTED_CITY_DISTANCE))
								{
									if (iCivValue > 0)
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeVassalProtectValue(ePlayer, iCivValue);
									else
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeVassalProtectValue(ePlayer, iUnitValue);
								}
								// Combat unit killed in a more distant plot visible to the vassal (and not in another player's lands - excluding the master's team and the vassal's team)
								else if (pPlot->isVisible(GET_PLAYER(eLoopPlayer).getTeam()) && iCivValue == 0)
								{
									if (!pPlot->isOwned() || GET_PLAYER(pPlot->getOwner()).getTeam() == GET_PLAYER(ePlayer).getTeam() || GET_PLAYER(pPlot->getOwner()).getTeam() == GET_PLAYER(eLoopPlayer).getTeam())
									{
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeVassalProtectValue(ePlayer, iUnitValue);
									}
								}
							}
						}
					}
				}
			}

			if (iCivValue == 0)
			{
				GET_PLAYER(ePlayer).ApplyWarDamage(eUnitOwner, iUnitValue);

				if (GET_PLAYER(ePlayer).isMajorCiv())
					GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeWarProgressScore(eUnitOwner, /*20*/ GD_INT_GET(WAR_PROGRESS_KILLED_UNIT));

				if (GET_PLAYER(eUnitOwner).isMajorCiv())
					GET_PLAYER(eUnitOwner).GetDiplomacyAI()->ChangeWarProgressScore(ePlayer, /*-10*/ GD_INT_GET(WAR_PROGRESS_LOST_UNIT));
			}
		}

		if (getLeaderUnitType() != NO_UNIT)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					strBuffer = GetLocalizedText("TXT_KEY_MISC_GENERAL_KILLED", getNameKey());
					DLLUI->AddUnitMessage(0, GetIDInfo(), ((PlayerTypes)iI), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
				}
			}
		}
	}

	if (MOD_EVENTS_UNIT_PREKILL)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPrekill, eUnitOwner, GetID(), getUnitType(), getX(), getY(), bDelay, ePlayer);
	}
	else
	{
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(((int)eUnitOwner));
			args->Push(GetID());
			args->Push(getUnitType());
			args->Push(getX());
			args->Push(getY());
			args->Push(bDelay);
			args->Push(ePlayer);

			bool bResult = false;
			LuaSupport::CallHook(pkScriptSystem, "UnitPrekill", args.get(), bResult);
		}
	}

	// Check for Difficulty Bonus
	if (ePlayer != NO_PLAYER && !IsCivilianUnit())
	{
		if (GET_PLAYER(eUnitOwner).isMajorCiv())
			GET_PLAYER(ePlayer).DoDifficultyBonus(DIFFICULTY_BONUS_KILLED_MAJOR_UNIT);
		else if (GET_PLAYER(eUnitOwner).isMinorCiv())
			GET_PLAYER(ePlayer).DoDifficultyBonus(DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT);
		else if (GET_PLAYER(eUnitOwner).isBarbarian())
			GET_PLAYER(ePlayer).DoDifficultyBonus(DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT);
	}

	if (bDelay)
	{
		if (ePlayer == NO_PLAYER && isCultureFromExperienceDisbandUpgrade())
		{
			int iExperience = getExperienceTimes100() / 100;
			if (iExperience > 0)
			{
				GET_PLAYER(eUnitOwner).changeJONSCulture(iExperience);
				if (eUnitOwner == GC.getGame().getActivePlayer())
				{
					char text[256] = { 0 };
					sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iExperience);
					SHOW_PLOT_POPUP(plot(),eUnitOwner, text);
				}
			}
		}
		startDelayedDeath();
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// EVERYTHING AFTER THIS LINE OCCURS UPON THE ACTUAL DELETION OF THE UNIT AND NOT WITH A DELAYED DEATH
	///////////////////////////////////////////////////////////////////////////////////////////////

	if (MOD_GLOBAL_NO_LOST_GREATWORKS && HasUnusedGreatWork())
		GC.getGame().removeGreatPersonBornName(getGreatName());

	if (IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		if(DLLUI->GetLengthSelectionList() == 1)
		{
			if(!(DLLUI->isFocused()) && !(DLLUI->isCitySelection()) && !(DLLUI->isDiploOrPopupWaiting()))
			{
				GC.getGame().updateSelectionList();
			}

			if(IsSelected())
			{
				DLLUI->setCycleSelectionCounter(1);
			}
			else
			{
				DLLUI->setDirty(SelectionCamera_DIRTY_BIT, true);
			}
		}
	}

	DLLUI->RemoveFromSelectionList(pDllThisUnit.get());
	GET_PLAYER(getOwner()).GetUnitCycler().RemoveUnit(GetID());

	// Killing a unit while in combat is not something we really expect to happen.
	// It is *mostly* safe for it to happen, but combat systems must be able to gracefully handle the disapperance of a unit.
	CvAssertMsg_Debug(!isInCombat(), "isCombat did not return false as expected");

	clearCombat();	// Disconnect from any combat

	pTransportUnit = getTransportUnit();

	if (pTransportUnit != NULL)
		setTransportUnit(NULL);

	if (MOD_LINKED_MOVEMENT)
	{
		// remove linked status
		if (IsLinkedLeader())
		{
			SetIsLinkedLeader(false);
		}
		else if (IsLinked())
		{
			CvUnit* pLinkedLeader = GET_PLAYER(m_eOwner).getUnit(GetLinkedLeaderID());
			pLinkedLeader->SetIsLinkedLeader(false);
		}
	}

	setReconPlot(NULL);

	CvAssertMsg(getAttackPlot() == NULL, "The current unit instance's attack plot is expected to be NULL");
	CvAssertMsg(getCombatUnit() == NULL, "The current unit instance's combat unit is expected to be NULL");

	GET_TEAM(getTeam()).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);
	GET_PLAYER(eUnitOwner).changeUnitClassCount((UnitClassTypes)getUnitInfo().GetUnitClassType(), -1);

	// Builder Limit
	if(getUnitInfo().GetWorkRate() > 0 && getUnitInfo().GetDomainType() == DOMAIN_LAND)
	{
		GET_PLAYER(eUnitOwner).ChangeNumBuilders(-1);
	}

	// Some Units count against Happiness
	if(getUnitInfo().GetUnhappiness() != 0)
	{
		GET_PLAYER(eUnitOwner).ChangeUnhappinessFromUnits(-getUnitInfo().GetUnhappiness());
	}

	GET_PLAYER(eUnitOwner).changeExtraUnitCost(-(getUnitInfo().GetExtraMaintenanceCost()));

	if(getUnitInfo().GetNukeDamageLevel() > 0)
	{
		GET_PLAYER(eUnitOwner).changeNumNukeUnits(-1);
	}

	if(getUnitInfo().IsMilitarySupport())
	{
		GET_PLAYER(eUnitOwner).changeNumMilitaryUnits(-1, getUnitInfo().GetDomainType());
	}

	if (getUnitInfo().IsMilitarySupport() && (isNoSupply() || isContractUnit()))
	{
		GET_PLAYER(eUnitOwner).changeNumUnitsSupplyFree(-1);
	}

	// A unit dying reduces the Great General meter
	if (getExperienceTimes100() > 0 && ePlayer != NO_PLAYER)
	{
		int iGreatGeneralMeterLossTimes100 = getExperienceTimes100() * /*50*/ GD_INT_GET(UNIT_DEATH_XP_GREAT_GENERAL_LOSS);
		iGreatGeneralMeterLossTimes100 /= 100;

		if(getDomainType() == DOMAIN_SEA)
		{
			GET_PLAYER(eUnitOwner).changeNavalCombatExperienceTimes100(-iGreatGeneralMeterLossTimes100);
		}
		else
		{
			GET_PLAYER(eUnitOwner).changeCombatExperienceTimes100(-iGreatGeneralMeterLossTimes100);
		}
	}

	CvUnitCaptureDefinition kCaptureDef;
	getCaptureDefinition(&kCaptureDef);
	
	if (MOD_EVENTS_UNIT_CAPTURE && (kCaptureDef.eCapturingPlayer != NO_PLAYER && kCaptureDef.eCaptureUnitType != NO_UNIT)) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, kCaptureDef.eCapturingPlayer, kCaptureDef.eCaptureUnitType, eUnitOwner, GetID(), false, 1);
	}

	setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true);
	if(pPlot)
		pPlot->removeUnit(this, false);

	if (pPlot)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity != NULL)
		{
			if (GET_PLAYER(eUnitOwner).GetFlatDefenseFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				pCity->updateStrengthValue();
			}
			
			if (GET_PLAYER(eUnitOwner).GetNeedsModifierFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				GET_PLAYER(eUnitOwner).CalculateNetHappiness();
			}
		}

		//check if this removes a blockade immediately (would be lifted anyhow once the enemy turn starts but nice for humans)
		if (IsCombatUnit())
		{
			vector<CvCity*> affectedCities;
			for (int i = 0; i < RING_PLOTS[GetBlockadeRange()]; i++)
			{
				CvPlot* pNeighbor = iterateRingPlots(pPlot, i);
				if (pNeighbor && pNeighbor->getLandmass()==pPlot->getLandmass() && pNeighbor->isBlockaded(pNeighbor->getOwner()))
					affectedCities.push_back( pPlot->getEffectiveOwningCity() );
			}

			for (vector<CvCity*>::iterator it = affectedCities.begin(); it != affectedCities.end(); ++it)
			{
				//we assume blockades are lifted ... so find better plot assignments
				if ((*it) && (*it)->GetCityCitizens()->DoVerifyWorkingPlots())
					(*it)->GetCityCitizens()->OptimizeWorkedPlots(false);
			}
		}

		// Clear cached danger in the vicinity for instant update
		if (ePlayer!=NO_PLAYER)
			GET_PLAYER(ePlayer).ResetDangerCache(*pPlot,3);
	}

	// Remove Resource Quantity from Used
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		if(getUnitInfo().GetResourceQuantityRequirement(iResourceLoop) > 0)
		{
			GET_PLAYER(eUnitOwner).changeNumResourceUsed((ResourceTypes) iResourceLoop, -getUnitInfo().GetResourceQuantityRequirement(iResourceLoop));
		}
	}
	// Let's force a visibility update to all nearby units (in the domain)--within range--if this general is captured or killed regardless if there is another general around to give the bonus
	if (isGiveInvisibility())
	{
		int iLoop = 0;
		int iRange = GetNearbyUnitPromotionsRange();
		CvPlayerAI& kPlayer = GET_PLAYER(eUnitOwner);
		TeamTypes activeTeam = GC.getGame().getActiveTeam();
		for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
		{
			if (!pLoopUnit->IsCombatUnit())
				continue;

			if (getGiveDomain() != NO_DOMAIN && (getGiveDomain() != pLoopUnit->getDomainType()))
				continue;

			if (plotDistance(getX(), getY(), pLoopUnit->getX(), pLoopUnit->getY()) > iRange)
				continue;

			if (pLoopUnit->getInvisibleType() == NO_INVISIBLE && pLoopUnit->getTeam() != activeTeam)
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), true /*bVisible*/, true);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// WARNING: This next statement will delete 'this'
	// ANYTHING BELOW HERE MUST NOT REFERENCE THE UNIT!
	GET_PLAYER(eUnitOwner).deleteUnit(GetID());

	// Update Unit Production Maintenance
	GET_PLAYER(kCaptureDef.eOldPlayer).UpdateUnitProductionMaintenanceMod();

	// Create the captured unit that will replace this unit (if the capture definition is valid)
	CvUnit::createCaptureUnit(kCaptureDef);

	if (GC.getGame().getActivePlayer() == kCaptureDef.eOldPlayer)
	{
		CvMap& theMap = GC.getMap();
		theMap.updateDeferredFog();
	}

	/// If checking for murder, do that now
	if (bCheckForMurder)
		GET_PLAYER(ePlayer).CheckForMurder(eUnitOwner);
}

//	---------------------------------------------------------------------------
//	Get a definition that can be used to create a captured version of the unit.
bool CvUnit::getCaptureDefinition(CvUnitCaptureDefinition* pkCaptureDef, PlayerTypes eCapturingPlayer /* = NO_PLAYER */)
{
	CvUnitCaptureDefinition kCaptureDef;
	kCaptureDef.eOldPlayer = getOwner();
	kCaptureDef.eOriginalOwner = GetOriginalOwner();
	kCaptureDef.eOldType = getUnitType();
	kCaptureDef.eCapturingPlayer = (eCapturingPlayer != NO_PLAYER) ? eCapturingPlayer : getCapturingPlayer();
	kCaptureDef.bEmbarked = m_bEmbarked;
	kCaptureDef.eCaptureUnitType = NO_UNIT;
	kCaptureDef.iUnitID = GetID();
	kCaptureDef.iScenarioData = m_iScenarioData;

	if (GetReligionData())
	{
		kCaptureDef.eReligion = GetReligionData()->GetReligion();
		kCaptureDef.iReligiousStrength = GetReligionData()->GetReligiousStrength();
		kCaptureDef.iSpreadsUsed = GetReligionData()->GetSpreadsUsed();
	}


	// Captured as conscripted unit?
	if (IsCapturedAsConscript())
	{
		kCaptureDef.bConscript = true;
	}

	// Captured as is?
	if(IsCapturedAsIs())
	{
		kCaptureDef.bAsIs = true;
		kCaptureDef.eCaptureUnitType = getUnitType();
	}
	// Barbs captured this unit, or a player capturing this unit from the barbs
	else if(isBarbarian() || (kCaptureDef.eCapturingPlayer != NO_PLAYER && GET_PLAYER(kCaptureDef.eCapturingPlayer).isBarbarian()))
	{
		// Must be able to capture this unit normally... don't want the barbs picking up Workboats, Generals, etc.
		if(kCaptureDef.eCapturingPlayer != NO_PLAYER && getCaptureUnitType(kCaptureDef.eCapturingPlayer) != NO_UNIT)
			// Unit type is the same as what it was
			kCaptureDef.eCaptureUnitType = getUnitType();
	}

	// Barbs not involved
	else
	{
		if(kCaptureDef.eCapturingPlayer != NO_PLAYER)
			kCaptureDef.eCaptureUnitType = getCaptureUnitType(kCaptureDef.eCapturingPlayer);
	}

	CvPlot* pkPlot = plot();
	if(pkPlot)
	{
		kCaptureDef.iX = pkPlot->getX();
		kCaptureDef.iY = pkPlot->getY();
	}
	else
	{
		kCaptureDef.iX = INVALID_PLOT_COORD;
		kCaptureDef.iY = INVALID_PLOT_COORD;
	}

	if(pkCaptureDef)
		*pkCaptureDef = kCaptureDef;

	return kCaptureDef.eCaptureUnitType != NO_UNIT && kCaptureDef.eCapturingPlayer != NO_PLAYER;
}

//	---------------------------------------------------------------------------
//	Create a new unit using a capture definition.
//	Returns the unit if create or NULL is the definition is not valid, or something else
//	goes awry.
//
//	Please note this method is static because it is often called AFTER the original unit
//	has been deleted.

/* static */ CvUnit* CvUnit::createCaptureUnit(const CvUnitCaptureDefinition& kCaptureDef, bool ForcedCapture)
{
	if (kCaptureDef.eCapturingPlayer == NO_PLAYER || kCaptureDef.eCaptureUnitType == NO_UNIT)
		return NULL;

	CvPlot* pkPlot = GC.getMap().plot(kCaptureDef.iX , kCaptureDef.iY);
	if (!pkPlot)
		return NULL;

	CvPlayerAI& kCapturingPlayer = GET_PLAYER(kCaptureDef.eCapturingPlayer);
	CvUnit* pkCapturedUnit = kCapturingPlayer.initUnit(kCaptureDef.eCaptureUnitType, kCaptureDef.iX, kCaptureDef.iY);

	if (!pkCapturedUnit)
		return NULL;

	pkCapturedUnit->GetReligionDataMutable()->SetReligion(kCaptureDef.eReligion);
	pkCapturedUnit->GetReligionDataMutable()->SetReligiousStrength(kCaptureDef.iReligiousStrength);
	pkCapturedUnit->GetReligionDataMutable()->SetSpreadsUsed(kCaptureDef.iSpreadsUsed);

	pkCapturedUnit->SetOriginalOwner(kCaptureDef.eOriginalOwner);

	if (MOD_BALANCE_CORE_BARBARIAN_THEFT && pkCapturedUnit->IsCivilianUnit() && pkCapturedUnit->GetOriginalOwner() != kCapturingPlayer.GetID())
	{
		PromotionTypes ePromotionForced = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_PRISONER_WAR");
		if (ePromotionForced != NO_PROMOTION && !pkCapturedUnit->HasPromotion(ePromotionForced))
		{
			pkCapturedUnit->setHasPromotion(ePromotionForced, true);
		}
	}

	if (pkCapturedUnit->IsCombatUnit() && kCaptureDef.bConscript)
	{
		PromotionTypes ePromotionForced = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_CONSCRIPT");
		if (ePromotionForced != NO_PROMOTION)
		{
			kCapturingPlayer.changeNumUnitsSupplyFree(1);
			pkCapturedUnit->changeNoSupply(1);
			pkCapturedUnit->setHasPromotion(ePromotionForced, true);
		}
	}

	pkCapturedUnit->setScenarioData(kCaptureDef.iScenarioData);


	if(GC.getLogging() && GC.getAILogging())
	{
		CvString szMsg;
		szMsg.Format("Captured: %s, Enemy was: %s", GC.getUnitInfo(kCaptureDef.eOldType)->GetDescription(), kCapturingPlayer.getCivilizationShortDescription());
		GET_PLAYER(kCaptureDef.eOldPlayer).GetTacticalAI()->LogTacticalMessage(szMsg);
	}

	if(kCaptureDef.bEmbarked)
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pkCapturedUnit));
		gDLL->GameplayUnitEmbark(pDllUnit.get(), true);
		pkCapturedUnit->setEmbarked(true);
		if (!pkCapturedUnit->jumpToNearestValidPlot())
		{
			pkCapturedUnit->kill(true);
			pkCapturedUnit = NULL;
		}
	}

	bool bDisbanded = false;
	if (pkCapturedUnit != NULL)
	{
		pkCapturedUnit->finishMoves();

		// Minor civs can't capture settlers, ever!
		if(!bDisbanded && GET_PLAYER(pkCapturedUnit->getOwner()).isMinorCiv() && (pkCapturedUnit->isFound() || pkCapturedUnit->IsFoundAbroad()))
		{
			bDisbanded = true;
			pkCapturedUnit->kill(false);
			pkCapturedUnit = NULL;
		}
		if (pkCapturedUnit != NULL && pkCapturedUnit->IsCombatUnit())
		{
			if(!pkCapturedUnit->jumpToNearestValidPlot())
			{
				pkCapturedUnit->kill(true);
				pkCapturedUnit = NULL;
			}
		}
	}

	// Captured civilian who could be returned?
	if(!kCaptureDef.bAsIs)
	{
		bool bShowingHumanPopup = true;
		bool bShowingActivePlayerPopup = true;

		// Only active player gets the choice
		if(GC.getGame().getActivePlayer() != kCaptureDef.eCapturingPlayer)
		{
			bShowingActivePlayerPopup = false;
		}

		// Original owner is dead!
		if(!GET_PLAYER(kCaptureDef.eOriginalOwner).isAlive())
			bShowingHumanPopup = false;

		// If original owner
		else if(kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer)
			bShowingHumanPopup = false;

		// Players at war
		else if (GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isAtWar(kCapturingPlayer.getTeam()))
			bShowingHumanPopup = false;

		else if (GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isAtWar(kCapturingPlayer.getTeam()))
			bShowingHumanPopup = false;

		// Players haven't met
		else if(!GET_TEAM(GET_PLAYER(kCaptureDef.eOriginalOwner).getTeam()).isHasMet(kCapturingPlayer.getTeam()))
			bShowingHumanPopup = false;

#if defined(MOD_BALANCE_CORE)
		// Not human?
		else if(!GET_PLAYER(GC.getGame().getActivePlayer()).isHuman())
			bShowingHumanPopup = false;

		if (ForcedCapture)
			bShowingHumanPopup = false;
#endif

		// Show the popup
		if(bShowingHumanPopup && bShowingActivePlayerPopup && pkCapturedUnit)
		{
			CvPopupInfo kPopupInfo(BUTTONPOPUP_RETURN_CIVILIAN, kCaptureDef.eCapturingPlayer, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
			DLLUI->AddPopup(kPopupInfo);
			// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
			if (kCapturingPlayer.isLocalPlayer())
				CancelActivePlayerEndTurn();
		}

		// Take it automatically!
		else if(!bShowingHumanPopup && !bDisbanded && pkCapturedUnit != NULL && !pkCapturedUnit->isBarbarian())	// Don't process if the AI decided to disband the unit, or the barbs captured something
		{
			// If the unit originally belonged to us, we've already done what we needed to do
			if(kCaptureDef.eCapturingPlayer != kCaptureDef.eOriginalOwner)
			{
#if defined(MOD_BALANCE_CORE)
				if (kCaptureDef.eOriginalOwner != NO_PLAYER && GET_PLAYER(kCaptureDef.eOriginalOwner).isAlive() && !GET_PLAYER(kCaptureDef.eCapturingPlayer).isHuman() && !GET_PLAYER(kCaptureDef.eCapturingPlayer).IsAtWarWith(kCaptureDef.eOriginalOwner))
				{
					CivOpinionTypes eMajorOpinion = CIV_OPINION_NEUTRAL;
					CivApproachTypes eMinorOpinion = CIV_APPROACH_NEUTRAL;
					if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMajorCiv())
					{
						eMajorOpinion = kCapturingPlayer.GetDiplomacyAI()->GetCivOpinion(kCaptureDef.eOriginalOwner);
					}
					else
					{
						eMinorOpinion = kCapturingPlayer.GetDiplomacyAI()->GetCivApproach(kCaptureDef.eOriginalOwner);
					}

					if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMajorCiv() && eMajorOpinion >= CIV_OPINION_FAVORABLE)
					{	
						kCapturingPlayer.DoCivilianReturnLogic(true, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
					}
					else if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMinorCiv() && eMinorOpinion == CIV_APPROACH_FRIENDLY)
					{
						kCapturingPlayer.DoCivilianReturnLogic(true, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
					}
					else
					{
						kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
					}
				}
				else
				{
#endif

				kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
#if defined(MOD_BALANCE_CORE)
				}
			}
#endif
		}
		// if Venice
		else if (kCapturingPlayer.GetPlayerTraits()->IsNoAnnexing())
		{
#if defined(MOD_BALANCE_CORE)
			if(kCaptureDef.eOriginalOwner != NO_PLAYER && !GET_PLAYER(kCaptureDef.eCapturingPlayer).isHuman())
			{
				CivOpinionTypes eMajorOpinion = CIV_OPINION_NEUTRAL;
				CivApproachTypes eMinorOpinion = CIV_APPROACH_NEUTRAL;
				if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMajorCiv())
				{
					eMajorOpinion = kCapturingPlayer.GetDiplomacyAI()->GetCivOpinion(kCaptureDef.eOriginalOwner);
				}
				else
				{
					eMinorOpinion = kCapturingPlayer.GetDiplomacyAI()->GetCivApproach(kCaptureDef.eOriginalOwner);
				}

				if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMajorCiv() && eMajorOpinion >= CIV_OPINION_FAVORABLE)
				{	
					kCapturingPlayer.DoCivilianReturnLogic(true, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
				}
				else if(GET_PLAYER(kCaptureDef.eOriginalOwner).isMinorCiv() && eMinorOpinion == CIV_APPROACH_FRIENDLY)
				{
					kCapturingPlayer.DoCivilianReturnLogic(true, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
				}
				else
				{
					kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
				}
			}
			else
			{
#endif

			kCapturingPlayer.DoCivilianReturnLogic(false, kCaptureDef.eOriginalOwner, pkCapturedUnit->GetID());
#if defined(MOD_BALANCE_CORE)
			}
#endif
		}
	}
	else
	{
		// restore combat units at some percentage of their original health
		if (pkCapturedUnit != NULL)
		{
			int iCapturedHealth = (pkCapturedUnit->GetMaxHitPoints() * /*50 in CP, 75 in VP*/ GD_INT_GET(COMBAT_CAPTURE_HEALTH)) / 100;
			pkCapturedUnit->setDamage(iCapturedHealth);

			// (5-82): Captured Units can still move/pillage (but not attack)
			if (MOD_BALANCE_VP)
			{
				// Unused code, but might be used in next Congress
				// allows captured units to gain XP off its origin city (of its capturer which is either the nearest city or the Capital City!)
				/*
				CvCity* pOriginCityCaptured = pkCapturedUnit->getOriginCity();
				if (pOriginCityCaptured == NULL)
					pOriginCityCaptured = GET_PLAYER(kCaptureDef.eCapturingPlayer).getCapitalCity();
				pOriginCityCaptured->addProductionExperience(pkCapturedUnit, false, true);
				*/
				pkCapturedUnit->restoreFullMoves();
				while (pkCapturedUnit->getNumAttacksMadeThisTurn()<pkCapturedUnit->getNumAttacks())
					pkCapturedUnit->setMadeAttack(true);
				pkCapturedUnit->SetTurnProcessed(false);

				//let tactical AI handle the unit
				//DO NOT PUSH MISSIONS DIRECTLY WHILE ANOTHER UNIT IS EXECUTING ITS MISSION
				CvPlayer& kOwner = GET_PLAYER(pkCapturedUnit->getOwner());
				if (!kOwner.isHuman())
					kOwner.GetTacticalAI()->AddCurrentTurnUnit(pkCapturedUnit);
			}
		}
	}

	if(kCaptureDef.eCapturingPlayer == GC.getGame().getActivePlayer())
	{
		CvString strBuffer;
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(kCaptureDef.eCaptureUnitType);
		if (pkUnitInfo)
		{
			if (kCaptureDef.eOriginalOwner == kCaptureDef.eCapturingPlayer){
				//player recaptured a friendly unit
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_RECAPTURED_UNIT", pkUnitInfo->GetTextKey());
			}
			else{
				strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", pkUnitInfo->GetTextKey());
			}
			DLLUI->AddUnitMessage(0, IDInfo(kCaptureDef.eCapturingPlayer, pkCapturedUnit->GetID()), kCaptureDef.eCapturingPlayer, true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eCaptureUnitType)->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY()*/);
			if (MOD_WH_MILITARY_LOG)
				MILITARYLOG(kCaptureDef.eCapturingPlayer, strBuffer.c_str(), pkPlot, kCaptureDef.eOldPlayer);
		}
	}

	return pkCapturedUnit;
}

//	----------------------------------------------------------------------------
void CvUnit::doTurn()
{
	VALIDATE_OBJECT
	CvAssertMsg(!IsDead(), "isDead did not return false as expected");

	// Wake unit if skipped last turn
	ActivityTypes eActivityType = GetActivityType();
	bool bHoldCheck = (eActivityType == ACTIVITY_HOLD); //this is after a skip mission
	bool bHealCheck = (eActivityType == ACTIVITY_HEAL) && (m_iDamageTakenLastTurn>0 || !IsHurt()); //done healing or under attack?
	bool bSentryCheck = (eActivityType == ACTIVITY_SENTRY) && SentryAlert(true); //on alert
	bool bFortifyCheck = (eActivityType == ACTIVITY_SLEEP) && isProjectedToDieNextTurn(); //fortified but about to die
	bool bInterceptCheck = (eActivityType == ACTIVITY_INTERCEPT) && !isHuman(); //AI interceptors reconsider each turn

	if (bHoldCheck || bHealCheck || bSentryCheck || bFortifyCheck || bInterceptCheck)	
	{
		SetActivityType(ACTIVITY_AWAKE);
	}
#if defined(MOD_BALANCE_CORE)
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ChangeNumTimesAttackedThisTurn((PlayerTypes)iPlayerLoop, (-1 * GetNumTimesAttackedThisTurn((PlayerTypes)iPlayerLoop)));
	}

	//Behind enemy lines?
	if (IsGainsXPFromSpotting() && !isEmbarked())
	{
		if (plot() != NULL && plot()->getTeam() != getTeam() && plot()->getTeam() != NO_TEAM && plot()->getTeam() != BARBARIAN_TEAM)
		{
			if (GET_TEAM(plot()->getTeam()).isMajorCiv() && GET_TEAM(plot()->getTeam()).isAtWar(getTeam()))
			{
				int iExperience = /*2*/ GD_INT_GET(BALANCE_SCOUT_XP_BASE) * 2;
				if (iExperience > 0)
				{
					//Up to max barb value - rest has to come through combat!
					changeExperienceTimes100(iExperience * 100);
				}
			}
		}
	}

	if (!IsIgnoreZOC())
	{
		int iTotalMovePenalty = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				for (int iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
				{
					CvUnit* loopUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
					if (loopUnit == NULL)
						continue;
					if (!loopUnit->IsCombatUnit())
						continue;
					if (loopUnit->getDomainType() != getDomainType())
						continue;
					if (!loopUnit->isEnemy(getTeam(), plot()))
						continue;

					int iMovePenalty = loopUnit->GetAdjacentEnemySapMovement();
					if (iMovePenalty <= 0)
						continue;

					iTotalMovePenalty += iMovePenalty;
				}
			}
		}
		if (iTotalMovePenalty > 0)
		{
			iTotalMovePenalty = min(getMoves() - 1, iTotalMovePenalty);
			changeMoves(-iTotalMovePenalty);

			CvString strLogString;
			strLogString.Format("%s movement slowed by adjacent unit by %d moves!", getName().GetCString(), iTotalMovePenalty);
			GET_PLAYER(getOwner()).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}
#endif

	testPromotionReady();

	// Only increase our Fortification level if we've actually been told to Fortify
	if(IsFortified() && GetDamageAoEFortified() > 0)
		DoAdjacentPlotDamage(plot(), GetDamageAoEFortified(), "TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_AOE_STRIKE_FORTIFY");

	// Recon unit? If so, he sees what's around him
	if(IsRecon())
	{
		setReconPlot(plot());
	}

	// If we're not busy doing anything with the turn cycle, make the Unit's Flag bright again
	if(GetActivityType() == ACTIVITY_AWAKE)
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ false);
	}

	// Remove any expired promotions
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo && pkPromotionInfo->PromotionDuration() > 0 && isHasPromotion(ePromotion))
		{
			int iTurnsElapsed = GC.getGame().getGameTurn() - getTurnPromotionGained(ePromotion);
			if (iTurnsElapsed > getPromotionDuration(ePromotion))
			{
				setHasPromotion(ePromotion, false);
				restoreFullMoves();
				continue;
			}
		}
	}

	// prevent linked units in movement from asking orders
	if (MOD_LINKED_MOVEMENT && IsLinked() && !IsLinkedLeader())
	{
		SetTurnProcessed(true);
	}

	doDelayedDeath();
#if defined(MOD_BALANCE_CORE)
	DoImprovementExperience(plot());
	DoStackedGreatGeneralExperience(plot());
	DoConvertOnDamageThreshold(plot());
	DoNearbyUnitPromotion();
	DoConvertEnemyUnitToBarbarian(plot());
	DoConvertReligiousUnitsToMilitary();
	DoFinishBuildIfSafe();
#endif
}
//	--------------------------------------------------------------------------------
bool CvUnit::isActionRecommended(int iAction)
{
	VALIDATE_OBJECT

	BuildTypes eBuild;

	if(getOwner() != GC.getGame().getActivePlayer())
	{
		return false;
	}

	CvPlot* pPlot = NULL;

	if(pPlot == NULL)
	{
		if(gDLL->shiftKey())
		{
			pPlot = LastMissionPlot();
		}
	}

	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_HEAL())
	{
		if(IsHurt())
		{
			if(!hasMoved())
			{
				if((pPlot->getTeam() == getTeam()) || (healTurns(pPlot) < 4))
				{
					return true;
				}
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_FOUND())
	{
		if(canFoundCity(pPlot))
		{
			if(pPlot->isBestAdjacentFoundValue(getOwner()))
			{
				return true;
			}
		}
	}

	if(GC.getActionInfo(iAction)->getMissionType() == CvTypes::getMISSION_BUILD())
	{
		eBuild = ((BuildTypes)(GC.getActionInfo(iAction)->getMissionData()));
		CvAssert(eBuild != NO_BUILD);
		CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

		vector<BuilderDirective> directives = GET_PLAYER(getOwner()).GetBuilderTaskingAI()->GetDirectives();

		if (directives.empty())
			return false;

		for (vector<BuilderDirective>::iterator it = directives.begin(); it != directives.end(); ++it)
		{
			BuilderDirective eDirective = *it;
			CvPlot* pDirectivePlot = GC.getMap().plot(eDirective.m_sX, eDirective.m_sY);

			if (pPlot != pDirectivePlot)
				continue;

			bool bCanBuild = GET_PLAYER(getOwner()).GetBuilderTaskingAI()->CanUnitPerformDirective(this, eDirective);

			if (!bCanBuild)
				continue;

			// If this is not the best improvement we can build on this tile, return false
			return eDirective.m_eBuild == eBuild;
		}
	}

	if(GC.getActionInfo(iAction)->getCommandType() == COMMAND_PROMOTION)
	{
		return true;
	}

	return false;
}

#if defined(MOD_BALANCE_CORE)
void CvUnit::DoLocationPromotions(bool bSpawn, CvPlot* pOldPlot, CvPlot* pNewPlot)
{
	// Adjacent terrain/feature that provides free promotions?
	int iI = 0;
	if(pNewPlot == NULL)
	{
		pNewPlot = plot();
	}
	if(bSpawn && pNewPlot != NULL)
	{
		CvPlot* pAdjacentPlot = NULL;
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
				if(eFeature != NO_FEATURE)
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{				
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}

				// Starting terrain that provides free promotions?
				TerrainTypes eAdjacentTerrain = pAdjacentPlot->getTerrainType();
				if(eAdjacentTerrain != NO_TERRAIN && (eAdjacentTerrain <= TERRAIN_SNOW))
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eAdjacentTerrain)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				// Starting terrain that provides free promotions?
				if(pAdjacentPlot->isHills())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				if(pAdjacentPlot->isMountain())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				if(pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
				if(!pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake() && pAdjacentPlot->isWater())
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getAdjacentSpawnLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION)
					{
						CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
						{
							bool bNoPromotion = false;
							// Check for negating promotions
							if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
							{
								bNoPromotion = true;
							}
							if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
							{
								bNoPromotion = true;
							}
							if(!bNoPromotion)
							{
								PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
								// Unit has negation promotion
								if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
								{
									bNoPromotion = true;
								}
							}
							if(!bNoPromotion)
							{
								setHasPromotion(ePromotion, true);
							}
						}
					}
				}
			}
		}
		// Feature that provides free promotions?
		FeatureTypes eFeature = pNewPlot->getFeatureType();
		if(eFeature != NO_FEATURE)
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		// Terrain that provides free promotions?
		TerrainTypes eTerrain = pNewPlot->getTerrainType();
		if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isHills())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isMountain())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isShallowWater() && !pNewPlot->isLake())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(!pNewPlot->isShallowWater() && !pNewPlot->isLake() && pNewPlot->isWater())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getSpawnLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
	}
	if(pNewPlot != NULL)
	{
		// Feature that provides free promotions?
		FeatureTypes eFeature = pNewPlot->getFeatureType();
		if(eFeature != NO_FEATURE)
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		//Improvement that provides free promotion? Only if player owns them.
		ImprovementTypes eNeededImprovement = pNewPlot->getImprovementType();
		if(eNeededImprovement != NO_IMPROVEMENT)
		{
			//Only check for it to be owned by the player if that's valid!
			if (!GC.getImprovementInfo(eNeededImprovement)->IsOwnerOnly() || pNewPlot->getOwner() == getOwner())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getImprovementInfo(eNeededImprovement)->GetUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
				if (pNewPlot->IsRestoreMoves())
				{
					// Not using maxMoves() here since it shouldn't be affected by linked status and plot move changes (added below)
					setMoves(baseMoves(isEmbarked()) * GD_INT_GET(MOVE_DENOMINATOR));
				}
				if (pNewPlot->GetPlotMovesChange() > 0)
				{
					setMoves(movesLeft() + (pNewPlot->GetPlotMovesChange() * GD_INT_GET(MOVE_DENOMINATOR)));
				}
			}
		}
		// Terrain that provides free promotions?
		TerrainTypes eTerrain = pNewPlot->getTerrainType();
		if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isHills())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isMountain())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(pNewPlot->isShallowWater() && !pNewPlot->isLake())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		if(!pNewPlot->isShallowWater() && !pNewPlot->isLake() && pNewPlot->isWater())
		{
			PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getLocationUnitFreePromotion();
			if(ePromotion != NO_PROMOTION)
			{
				CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
				if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
				{
					bool bNoPromotion = false;
					// Check for negating promotions
					if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
					{
						bNoPromotion = true;
					}
					if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
					{
						bNoPromotion = true;
					}
					if(!bNoPromotion)
					{
						PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
						// Unit has negation promotion
						if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
						{
							bNoPromotion = true;
						}
					}
					if(!bNoPromotion)
					{
						setHasPromotion(ePromotion, true);
					}
				}
			}
		}
		CvPlot* pAdjacentPlot = NULL;
		for(iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot == NULL)
				continue;

			// Feature that provides free promotions?
			FeatureTypes eFeature = pAdjacentPlot->getFeatureType();
			if(eFeature != NO_FEATURE)
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eFeature)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			// Adjacent terrain that provides free promotions?
			TerrainTypes eTerrain = pAdjacentPlot->getTerrainType();
			if(eTerrain != NO_TERRAIN && (eTerrain <= TERRAIN_SNOW))
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eTerrain)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			// Adjacent terrain that provides free promotions?
			if(pAdjacentPlot->isHills())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			// Adjacent terrain that provides free promotions?
			if(pAdjacentPlot->isMountain())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
			if(!pAdjacentPlot->isShallowWater() && !pAdjacentPlot->isLake() && pAdjacentPlot->isWater())
			{
				PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getAdjacentUnitFreePromotion();
				if(ePromotion != NO_PROMOTION)
				{
					CvPromotionEntry* pkOriginalPromotionInfo = GC.getPromotionInfo(ePromotion);
					if(pkOriginalPromotionInfo && m_pUnitInfo->GetUnitCombatType() != NO_UNITCOMBAT && (::IsPromotionValidForUnitCombatType(ePromotion, getUnitType())))
					{
						bool bNoPromotion = false;
						// Check for negating promotions
						if(pkOriginalPromotionInfo->IsBarbarianOnly() && !isBarbarian())
						{
							bNoPromotion = true;
						}
						if(pkOriginalPromotionInfo->IsCityStateOnly() && !GET_PLAYER(getOwner()).isMinorCiv())
						{
							bNoPromotion = true;
						}
						if(!bNoPromotion)
						{
							PromotionTypes eNegatorPromotion = (PromotionTypes)getNegatorPromotion();
							// Unit has negation promotion
							if(eNegatorPromotion != NO_PROMOTION && ePromotion == eNegatorPromotion)
							{
								bNoPromotion = true;
							}
						}
						if(!bNoPromotion)
						{
							setHasPromotion(ePromotion, true);
						}
					}
				}
			}
		}
		if(pOldPlot != NULL)
		{
			// No longer on terrain that provides free promotions?
			if(pOldPlot->getTerrainType() != pNewPlot->getTerrainType())
			{
				const TerrainTypes eNeededTerrain = pOldPlot->getTerrainType();
				if(eNeededTerrain != NO_TERRAIN)
				{
					if(pOldPlot->isHills() && eNeededTerrain == TERRAIN_HILL)
					{
						PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_HILL)->getLocationUnitFreePromotion();
						if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
						{
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
							{
								setHasPromotion(ePromotion, false);
							}
						}
					}
					else if(pOldPlot->isMountain() && eNeededTerrain == TERRAIN_MOUNTAIN)
					{
						PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_MOUNTAIN)->getLocationUnitFreePromotion();
						if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
						{
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
							{
								setHasPromotion(ePromotion, false);
							}
						}
					}
					else if((pOldPlot->isWater() && pOldPlot->isShallowWater() && !pOldPlot->isLake()) && eNeededTerrain == TERRAIN_COAST)
					{
						PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_COAST)->getLocationUnitFreePromotion();
						if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
						{
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
							{
								setHasPromotion(ePromotion, false);
							}
						}
					}
					else if((pOldPlot->isWater() && !pOldPlot->isShallowWater() && !pOldPlot->isLake()) && eNeededTerrain == TERRAIN_OCEAN)
					{
						PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(TERRAIN_OCEAN)->getLocationUnitFreePromotion();
						if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
						{
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
							{
								setHasPromotion(ePromotion, false);
							}
						}
					}
					else if(eNeededTerrain <= TERRAIN_SNOW)
					{
						PromotionTypes ePromotion = (PromotionTypes)GC.getTerrainInfo(eNeededTerrain)->getLocationUnitFreePromotion();
						if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
						{
							CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
							if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
							{
								setHasPromotion(ePromotion, false);
							}
						}
					}
				}
			}
			if(pOldPlot->getFeatureType() != pNewPlot->getFeatureType())
			{
				FeatureTypes eNeededFeature = pOldPlot->getFeatureType();
				if(eNeededFeature != NO_FEATURE)
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getFeatureInfo(eNeededFeature)->getLocationUnitFreePromotion();
					if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
					{
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
						{
							setHasPromotion(ePromotion, false);
						}
					}
				}
			}
			if(pOldPlot->getImprovementType() != pNewPlot->getImprovementType())
			{
				ImprovementTypes eNeededImprovement = pOldPlot->getImprovementType();
				if(eNeededImprovement != NO_IMPROVEMENT)
				{
					PromotionTypes ePromotion = (PromotionTypes)GC.getImprovementInfo(eNeededImprovement)->GetUnitFreePromotion();
					if(ePromotion != NO_PROMOTION && isHasPromotion(ePromotion))
					{
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo && pkPromotionInfo->IsLostOnMove())
						{
							setHasPromotion(ePromotion, false);
						}
					}
				}
			}
		}
	}
}
#endif
//	--------------------------------------------------------------------------------
bool CvUnit::isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const
{
	VALIDATE_OBJECT
	if(pDefender == NULL)
	{
		return true;
	}

	TeamTypes eAttackerTeam = NO_TEAM;
	if(NULL != pAttacker)
	{
		eAttackerTeam = pAttacker->getTeam();
	}

	if(canCoexistWithEnemyUnit(eAttackerTeam))
	{
		return false;
	}

	if(!IsCanDefend())
	{
		return false;
	}

	if(IsCanDefend() && !(pDefender->IsCanDefend()))
	{
		return true;
	}

	// Don't let embarked units be best
	if(isEmbarked() && !pDefender->isEmbarked())
	{
		return false;
	}

	int iOurDefense = GetMaxDefenseStrength(plot(), pAttacker, pAttacker ? pAttacker->plot() : NULL, false, true);
	if(::isWorldUnitClass(getUnitClassType()))
	{
		iOurDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(pDefender->interceptionProbability() > 0)
		{
			iOurDefense *= (100 + pDefender->interceptionProbability());
			iOurDefense /= 100;
		}
	}

	iOurDefense /= (getCargo() + 1);

	int iTheirDefense = pDefender->GetMaxDefenseStrength(plot(), pAttacker, pAttacker ? pAttacker->plot() : NULL, false, true);
	if(::isWorldUnitClass(pDefender->getUnitClassType()))
	{
		iTheirDefense /= 2;
	}

	//special check for naval garrison in city
	if (pDefender->getDomainType()==DOMAIN_SEA && !pDefender->plot()->isWater())
	{
		iTheirDefense /= 2;
	}

	if(NULL == pAttacker)
	{
		if(interceptionProbability() > 0)
		{
			iTheirDefense *= (100 + interceptionProbability());
			iTheirDefense /= 100;
		}
	}

	iTheirDefense /= (pDefender->getCargo() + 1);

	if(iOurDefense == iTheirDefense)
	{
		if(NO_UNIT == getLeaderUnitType() && NO_UNIT != pDefender->getLeaderUnitType())
		{
			++iOurDefense;
		}
		else if(NO_UNIT != getLeaderUnitType() && NO_UNIT == pDefender->getLeaderUnitType())
		{
			++iTheirDefense;
		}
		else if(isBeforeUnitCycle(this, pDefender))
		{
			++iOurDefense;
		}
	}

	return (iOurDefense > iTheirDefense);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bTestBusy) const
{
	VALIDATE_OBJECT

	if(bTestBusy && IsBusy())
	{
		return false;
	}

	if (GetAutomateType() == AUTOMATE_TRADE)
		return false;	// No commands for this type.
		
#if defined(MOD_EVENTS_COMMAND)
	if (MOD_EVENTS_COMMAND) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanDoCommand, getOwner(), GetID(), eCommand, iData1, iData2, getX(), getY(), bTestVisible) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	switch(eCommand)
	{
	case COMMAND_PROMOTION:
		if(canPromote((PromotionTypes)iData1, iData2))
		{
			return true;
		}
		break;

	case COMMAND_UPGRADE:
		if(CanUpgradeRightNow(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_AUTOMATE:
		if(CanAutomate((AutomateTypes)iData1, bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_WAKE:
		return false; //don't really need this since fortification is automatic now
		break;

	case COMMAND_CANCEL:
	case COMMAND_CANCEL_ALL:
		if(!IsAutomated() && (GetLengthMissionQueue() > 0))
		{
			return true;
		}
		break;

	case COMMAND_STOP_AUTOMATION:
		if(IsAutomated())
		{
			return true;
		}
		break;

	case COMMAND_DELETE:
		if(canScrap(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_GIFT:
		if(canGift(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_HOTKEY:
		return true;
		break;

	default:
		CvAssert(false);
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	VALIDATE_OBJECT
	bool bCycle = false;
	if(canDoCommand(eCommand, iData1, iData2))
	{
		switch(eCommand)
		{
		case COMMAND_PROMOTION:
			promote((PromotionTypes)iData1, iData2);
			break;

		case COMMAND_UPGRADE:
			DoUpgrade();
			bCycle = true;
			break;

		case COMMAND_AUTOMATE:
			Automate((AutomateTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_WAKE:
			SetActivityType(ACTIVITY_AWAKE);
			if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
			{
				// Force the UI to update so they see any new commands
				DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			}
			break;

		case COMMAND_CANCEL:
			PopMission();
			break;

		case COMMAND_CANCEL_ALL:
			ClearMissionQueue();
			break;

		case COMMAND_STOP_AUTOMATION:
			SetAutomateType(NO_AUTOMATE);
			break;

		case COMMAND_DELETE:
			scrap();
			bCycle = true;
			break;

		case COMMAND_GIFT:
			gift();
			bCycle = true;
			break;

		case COMMAND_HOTKEY:
			setHotKeyNumber(iData1);
			break;

		default:
			CvAssert(false);
			break;
		}
	}

	if(bCycle)
	{
		if(IsSelected())
		{
			DLLUI->setCycleSelectionCounter(1);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEnterTerritory(TeamTypes eTeam, bool bEndTurn) const
{
	VALIDATE_OBJECT

	if(eTeam == NO_TEAM)
		return true;
	else if (isBarbarian() && GC.getGame().getGameTurn() < GC.getGame().GetBarbarianReleaseTurn())
		// Barbarians cannot enter owned plots in the beginning
		return false;

	TeamTypes eMyTeam = GET_PLAYER(getOwner()).getTeam();
	CvTeam& kMyTeam = GET_TEAM(eMyTeam);
	CvTeam& kTheirTeam = GET_TEAM(eTeam);

	if(kTheirTeam.IsAllowsOpenBordersToTeam(eMyTeam))
		return true;

	if(isEnemy(eTeam))
		return true;

	if(isRivalTerritory() || isTrade())
		return true;

	//if this option is active, we need open borders only if we want to end the turn in foreign territory
	if (MOD_CORE_RELAXED_BORDER_CHECK && !bEndTurn)
		return true;

	// Minors can't intrude into one another's territory
	if(kTheirTeam.isMinorCiv() && kMyTeam.isMajorCiv())
	{
		// Humans can always enter a minor's territory and bear the consequences
		if (isHuman())
			return true;

		// Allow AI players to pass through minors' territory
		if (!bEndTurn)
			return true;

		// If we haven't yet met the Minor we can move in
		if(!kMyTeam.isHasMet(eTeam))
			return true;

		// Is this an excluded unit that doesn't cause anger?
		if (IsAngerFreeUnit())
			return true;

		// If we are friends etc we may go there
		CvMinorCivAI* pMinorAI = GET_PLAYER(kTheirTeam.getLeaderID()).GetMinorCivAI();
		if (pMinorAI->IsPlayerHasOpenBorders(getOwner()))
			return true;
	}

	//city states may enter their ally's territory - may help for defense
	if (kMyTeam.isMinorCiv() && kTheirTeam.isMajorCiv() && GET_PLAYER(getOwner()).GetMinorCivAI()->IsAllies(kTheirTeam.getLeaderID()))
		return true;

	//default
	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEnterTerrain(const CvPlot& enterPlot, int iMoveFlags) const
{
	VALIDATE_OBJECT
	DomainTypes eDomain = getDomainType();

#if defined(MOD_CORE_UNREVEALED_IMPASSABLE)
	if (!isHuman() && !enterPlot.isRevealed(getTeam()) && (iMoveFlags & CvUnit::MOVEFLAG_PRETEND_ALL_REVEALED) == 0 && AI_getUnitAIType() != UNITAI_EXPLORE)
		return false;
#endif

	// Part 1 : Domain specific exclusions -----------------------------------------------

	// Air can go anywhere - provided it's a city or a carrier
	if (eDomain==DOMAIN_AIR)
		return (enterPlot.isCity() && enterPlot.getPlotCity()->getOwner()==getOwner()) || canLoad(enterPlot);

	// Immobile can go nowhere ... except where they are or cities
	if (eDomain==DOMAIN_IMMOBILE || m_bImmobile)
		if (!at(enterPlot.getX(),enterPlot.getY()) && !enterPlot.isCity())
			return false;

	// Sea units - we can exclude non-water plots right away
	// also allow forts and cities if adjacent to real water
	if (eDomain==DOMAIN_SEA)
	{
		//only trade units can pass through non-friendly improvements
		if (!enterPlot.isWater() && !enterPlot.isCoastalCityOrPassableImprovement(getOwner(), false, !isTrade()))
			return false;
	}

	// Land units and hover units may go anywhere in principle (with embarkation)
	if (!CanEverEmbark() || (iMoveFlags & MOVEFLAG_NO_EMBARK))
	{
		if (enterPlot.needsEmbarkation(this)) //this takes a while, check it last
			return false;
	}

	// Part 2 : player traits and unit traits ---------------------------------------------------

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	// If the plot is impassable, we need to check for positive promotions / traits and their exceptions
	if(enterPlot.isImpassable(getTeam()))
	{
		// Some special units also have it easy (if we have a trade path here, don't check the units individually)
		if(canMoveImpassable() || canMoveAllTerrain() || isTrade())
			return true;

		// if there's a route, anyone can use it (this includes city plots)
		if(enterPlot.isRoute() && !enterPlot.IsRoutePillaged())
			return true;

		// Check high seas
		if (enterPlot.isDeepWater() && enterPlot.getFeatureType()==NO_FEATURE)
		{
			if(eDomain == DOMAIN_SEA)
			{
				return ((iMoveFlags&CvUnit::MOVEFLAG_DESTINATION)==0) || canCrossOceans() || kPlayer.CanCrossOcean();
			}
			else if(eDomain == DOMAIN_LAND)
			{
				return IsEmbarkAllWater() || IsEmbarkDeepWater() || kPlayer.CanCrossOcean();
			}
		}

		// Check ice with specialty: is passable if owned
		if (enterPlot.isIce()) 
		{
			bool bCanCross = (canCrossIce() || kPlayer.CanCrossIce() || (eDomain==DOMAIN_SEA && enterPlot.isOwned() && ((iMoveFlags&CvUnit::MOVEFLAG_DESTINATION)==0)));
			return bCanCross;
		}

		// Check mountain.
		if (enterPlot.isMountain())
		{
			bool bCanCross = canCrossMountains() || kPlayer.CanCrossMountain();
			return bCanCross;
		}

		// general promotions ---------------------------------------------------
		if(enterPlot.getFeatureType() != NO_FEATURE && m_Promotions.HasAllowFeaturePassable() && m_Promotions.GetAllowFeaturePassable(enterPlot.getFeatureType(), getTeam()))
			return true;

		else if(enterPlot.getTerrainType() != NO_TERRAIN && m_Promotions.HasAllowTerrainPassable())
			return m_Promotions.GetAllowTerrainPassable(enterPlot.getTerrainType(), getTeam());

		//ok, seems we ran out of jokers. no pasaran!
		return false;
	}
	else //passable. need to check for negative promotions / traits and their exceptions
	{
		//special handling for ocean
		if (enterPlot.isDeepWater() && enterPlot.getFeatureType()==NO_FEATURE)
		{
			//true naval units can enter ocean plots if they don't stay there. embarked units need the tech in any case
			if ( (iMoveFlags&CvUnit::MOVEFLAG_DESTINATION) || enterPlot.needsEmbarkation(this))
			{
				//this promotion overrides the exception ...
				PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GD_INT_GET(PROMOTION_OCEAN_IMPASSABLE);
				bool bOceanImpassable = isHasPromotion(ePromotionOceanImpassable);
				if(bOceanImpassable)
					return false;

				if (canCrossOceans())
					return true;

				// tech-locked promotion
				if (m_Promotions.HasAllowTerrainPassable())
					return m_Promotions.GetAllowTerrainPassable(enterPlot.getTerrainType(), getTeam());

				// tech limited embarkation
				if (eDomain == DOMAIN_LAND)
					return IsEmbarkDeepWater() || IsEmbarkAllWater() || kPlayer.CanCrossOcean();
			}
		}
		else if(enterPlot.getFeatureType() != NO_FEATURE && isFeatureImpassable(enterPlot.getFeatureType()))
		{
			return false;
		}
		else if(enterPlot.getTerrainType() != NO_TERRAIN && isTerrainImpassable(enterPlot.getTerrainType()))
		{
			return false;
		}
		else if (kPlayer.isMinorCiv() && enterPlot.getRevealedImprovementType(getTeam()) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
		{
			//vp special: minors cannot enter/clear barbarian camps
			return false;
		}

		//ok, seems there are no objections. let's go!
		return true;
	}
}

//	--------------------------------------------------------------------------------
/// Returns the ID of the team to declare war against
TeamTypes CvUnit::GetDeclareWarMove(const CvPlot& plot) const
{
	VALIDATE_OBJECT
		CvAssert(isHuman());

	if (getDomainType() != DOMAIN_AIR)
	{
		// trying to enter foreign territory without open borders?
		TeamTypes eRevealedTeam = plot.getRevealedTeam(getTeam(), false);
		if (eRevealedTeam != NO_TEAM)
		{
			if (!canEnterTerritory(eRevealedTeam))
			{
				if (GET_TEAM(getTeam()).canDeclareWar(plot.getTeam(), getOwner()))
				{
					return eRevealedTeam;
				}
			}
		}

		if (plot.isActiveVisible())
		{
			TeamTypes eTeamAttack = NO_TEAM;
			if (plot.isCity())
			{
				eTeamAttack = plot.getTeam();
			}
			else
			{
				//can't attack if not in native domain or plot not accessible
				if (isNativeDomain(&plot) && canMoveInto(plot, MOVEFLAG_IGNORE_ENEMIES))
				{
					// do not declare war if plot can be entered peacefully
					if (!canMoveInto(plot, MOVEFLAG_DESTINATION))
					{
						if (IsCanAttackWithMove())
						{
							// check which units are on the tile
							// there can only be one land unit, one naval unit and one embarked unit on each tile
							// civilian units are ignored
							if (plot.getNumUnits() > 0)
							{
								TeamTypes eLandUnitTeam = NO_TEAM;
								TeamTypes eNavalUnitTeam = NO_TEAM;
								TeamTypes eEmbarkedUnitTeam = NO_TEAM;

								for (int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
								{
									CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);
									if (loopUnit != NULL)
									{
										if (loopUnit->IsCombatUnit())
										{
											if (loopUnit->isEmbarked())
											{
												eEmbarkedUnitTeam = loopUnit->getTeam();
											}
											else if (loopUnit->getDomainType() == DOMAIN_LAND)
											{
												eLandUnitTeam = loopUnit->getTeam();
											}
											else
											{
												eNavalUnitTeam = loopUnit->getTeam();
											}
										}
									}
								}

								// attack priority: first land unit, than naval unit, than embarked unit
								if (eLandUnitTeam != NO_TEAM) {
									eTeamAttack = eLandUnitTeam;
								}
								else
								{
									if (eNavalUnitTeam != NO_TEAM) {
										eTeamAttack = eNavalUnitTeam;
									}
									else
									{
										eTeamAttack = eEmbarkedUnitTeam;
									}
								}
							}
						}
					}
				}
			}
			if (eTeamAttack != NO_TEAM) {
				if (GET_TEAM(getTeam()).canDeclareWar(eTeamAttack, getOwner()))
				{
					return eTeamAttack;
				}
			}
		}
	}

	return NO_TEAM;
}

//	--------------------------------------------------------------------------------
/// Returns the ID of the team to declare war against
TeamTypes CvUnit::GetDeclareWarRangeStrike(const CvPlot& plot) const
{
	VALIDATE_OBJECT
		CvAssert(isHuman());

	if (plot.isActiveVisible())
	{
		if (canRangeStrikeAt(plot.getX(), plot.getY(), false))
		{
			TeamTypes eTeamAttack = NO_TEAM;
			if (plot.isCity())
			{
				eTeamAttack = plot.getTeam();
			}
			else
			{
				// check which units are on the tile
				// there can only be one land unit, one naval unit and one embarked unit on each tile
				// civilian units are ignored
				if (plot.getNumUnits() > 0)
				{
					TeamTypes eLandUnitTeam = NO_TEAM;
					TeamTypes eNavalUnitTeam = NO_TEAM;
					TeamTypes eEmbarkedUnitTeam = NO_TEAM;

					for (int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
					{
						CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);
						if (loopUnit != NULL)
						{
							if (loopUnit->IsCombatUnit())
							{
								if (loopUnit->isEmbarked())
								{
									eEmbarkedUnitTeam = loopUnit->getTeam();
								}
								else if (loopUnit->getDomainType() == DOMAIN_LAND)
								{
									eLandUnitTeam = loopUnit->getTeam();
								}
								else
								{
									eNavalUnitTeam = loopUnit->getTeam();
								}
							}
						}
					}

					// attack priority: first land unit, than naval unit, than embarked unit
					if (eLandUnitTeam != NO_TEAM) {
						eTeamAttack = eLandUnitTeam;
					}
					else
					{
						if (eNavalUnitTeam != NO_TEAM) {
							eTeamAttack = eNavalUnitTeam;
						}
						else
						{
							eTeamAttack = eEmbarkedUnitTeam;
						}
					}
				}
			}
			if (eTeamAttack != NO_TEAM) {
				if (GET_TEAM(getTeam()).canDeclareWar(eTeamAttack, getOwner()))
				{
					return eTeamAttack;
				}
			}
		}
	}

	return NO_TEAM;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveInto(const CvPlot& plot, int iMoveFlags) const
{
	// do not check this, the current plot may well be invalid!
	/*
	// nothing to do
	if(atPlot(plot))
	{
		return true;
	}
	*/

	// Added in Civ 5: Destination plots can't allow stacked Units of the same type
	if((iMoveFlags & CvUnit::MOVEFLAG_DESTINATION) && !IsCivilianUnit())
	{
		// Don't let another player's unit inside someone's city
		if((iMoveFlags & CvUnit::MOVEFLAG_ATTACK)==0)
		{

			if(plot.isCity() && plot.getPlotCity()->getOwner() != getOwner())
				return false;
		}

		// Check to see if any units are present at this full-turn move plot
		if(!(iMoveFlags & CvUnit::MOVEFLAG_IGNORE_STACKING_SELF) && !(iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
		{
			//this does not account for unit visibility!
			if (!CanStackUnitAtPlot(&plot))
			{
				return false;
			}
		}
	}

	if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
	{
		if(isOutOfAttacks())
		{
			// If there are only enemy civilians in the plot, then we don't need remaining attacks to capture them
			// eg a worker in a barbarian camp where we just killed the defender
			if (plot.getBestDefender(NO_PLAYER) != NULL)
			{
				return false;
			}
		}

		// Does unit only attack cities?
		if(IsCityAttackSupport() && !plot.isEnemyCity(*this) && plot.getBestDefender(NO_PLAYER))
		{
			return false;
		}
	}

	if (plot.isEnemyCity(*this))
	{
		if (isNoCapture())
		{
			return false;
		}
		// Can't enter an enemy city until it's "defeated"
		if (plot.getPlotCity()->getDamage() < plot.getPlotCity()->GetMaxHitPoints() && !(iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
		{
			return false;
		}
		if (iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if (getDomainType() == DOMAIN_AIR)
				return false;
			if (isHasPromotion((PromotionTypes)GD_INT_GET(PROMOTION_ONLY_DEFENSIVE)))
				return false;	// Can't advance into an enemy city
		}
	}
	else
	{
		//combat units cannot even pass through other teams' cities
		if (!IsCivilianUnit() && plot.isCity() && plot.getTeam() != getTeam())
		{
			return false;
		}
	}

	//air units use move missions to do range attacks, wtf
	if(getDomainType() == DOMAIN_AIR)
	{
		if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if(!canRangeStrikeAt(plot.getX(), plot.getY()))
			{
				return false;
			}
		}
		else
		{
			//just a failsafe, aircraft do range attacks when moving and move by rebasing ...
			if (!canRebaseAt(plot.getX(),plot.getY()))
			{
				return false;
			}
		}
	}
	else
	{
		if(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			if(!IsCanAttack())  // trying to give an attack order to a unit that can't fight. That doesn't work!
			{
				return false;
			}

			//cannot attack into non-native domain. but allow ships to capture cities
			if(!isNativeDomain(&plot) && !plot.isCity())
			{
				return false;
			}

			if(!isHuman() || (plot.isVisible(getTeam())))
			{
				// This stuff to the next if statement is to get units to advance into a tile with an enemy if that enemy is dying...
				bool bCanAdvanceOnDeadUnit = false;

				const IDInfo* pUnitNode = plot.headUnitNode();
				const CvUnit* pLoopUnit = NULL;

				bool bPlotContainsCombat = false;
				while(pUnitNode != NULL)
				{
					pLoopUnit = ::GetPlayerUnit(*pUnitNode);
					CvAssertMsg(pLoopUnit, "pUnitNode data should lead to a unit");

					pUnitNode = plot.nextUnitNode(pUnitNode);

					// NULL check is always a good thing to do.
					if(pLoopUnit)
					{
						if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pLoopUnit->getOwner()).getTeam()))
						{
							if(!pLoopUnit->IsDead() && pLoopUnit->isInCombat())
							{
								if(pLoopUnit->getCombatUnit() != this)
									bPlotContainsCombat = true;
							}

							if(pLoopUnit->IsDead() || !pLoopUnit->IsCombatUnit())
							{
								bCanAdvanceOnDeadUnit = true;
							}
							else
							{
								bCanAdvanceOnDeadUnit = false;
								break;
							}
						}
					}
				}

				if(bPlotContainsCombat)
				{
					return false;		// Can't enter a plot that contains combat that doesn't involve us.
				}

				if(!bCanAdvanceOnDeadUnit)
				{
					if(plot.isVisibleEnemyUnit(this) != (iMoveFlags & CvUnit::MOVEFLAG_ATTACK))
					{
						// Prevent an attack from failing if a city is empty but still an "enemy" capable of being attacked (this wouldn't happen before in Civ 4)
						if(!(iMoveFlags & CvUnit::MOVEFLAG_ATTACK) || !plot.isEnemyCity(*this))
						{
							return false;
						}
					}
				}
			}
		}
		else //if !(iMoveFlags & CvUnit::MOVEFLAG_ATTACK)
		{
			bool bEmbarkedAndAdjacent = false;
			bool bEnemyUnitPresent = false;

			// Without this code, Embarked Units can move on top of enemies because they have no visibility
			if(isEmbarked())
			{
				if(plotDistance(getX(), getY(), plot.getX(), plot.getY()) == 1)
				{
					bEmbarkedAndAdjacent = true;
				}
			}

			bool bPlotContainsCombat = false;
			if(plot.getNumUnits())
			{
				for(int iUnitLoop = 0; iUnitLoop < plot.getNumUnits(); iUnitLoop++)
				{
					CvUnit* loopUnit = plot.getUnitByIndex(iUnitLoop);

					if(loopUnit && !loopUnit->isDelayedDeath() && GET_TEAM(getTeam()).isAtWar(loopUnit->getTeam()))
					{
						bEnemyUnitPresent = true;
						if(!loopUnit->IsDead() && loopUnit->isInCombat())
						{
							if(loopUnit->getCombatUnit() != this)
								bPlotContainsCombat = true;
						}
						break;
					}
				}
			}

			if(bPlotContainsCombat)
			{
				return false;		// Can't enter a plot that contains combat that doesn't involve us.
			}

			if(!isHuman() || plot.isVisible(getTeam()) || bEmbarkedAndAdjacent)
			{
				if (!(iMoveFlags & CvUnit::MOVEFLAG_IGNORE_ENEMIES))
				{
					if (plot.isEnemyCity(*this))
					{
						return false;
					}

					//check for combat units only! enemy civilians are captured en passant, there is no downside ...
					// don't capture enemy civilians if embarked
					if (plot.isEnemyUnit(getOwner(),true,true) || ((bEmbarkedAndAdjacent || plot.needsEmbarkation(this)) && bEnemyUnitPresent))
					{
						return false;
					}
				}
			}
		}

		TeamTypes ePlotTeam = ((isHuman()) ? plot.getRevealedTeam(getTeam()) : plot.getTeam());

		bool bCanEnterTerritory = ((iMoveFlags&CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE)>0) || canEnterTerritory(ePlotTeam, (iMoveFlags&CvUnit::MOVEFLAG_DESTINATION)>0);
		if (!bCanEnterTerritory)
		{
			return false;
		}
	}

#if defined(MOD_EVENTS_CAN_MOVE_INTO)
	if (MOD_EVENTS_CAN_MOVE_INTO && m_pUnitInfo->IsSendCanMoveIntoEvent()) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanMoveInto, getOwner(), GetID(), plot.getX(), plot.getY(), ((iMoveFlags & CvUnit::MOVEFLAG_ATTACK) != 0), false) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	// Make sure we can enter the terrain.  Somewhat expensive call, so we do this last.
	if(!canEnterTerrain(plot, iMoveFlags))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canMoveOrAttackInto(const CvPlot& plot, int iMoveFlags) const
{
	VALIDATE_OBJECT
	return (canMoveInto(plot, iMoveFlags & ~CvUnit::MOVEFLAG_ATTACK) || canMoveInto(plot, iMoveFlags | CvUnit::MOVEFLAG_ATTACK));
}

//	--------------------------------------------------------------------------------
/// can this unit move into enemy territory without open borders
bool CvUnit::IsAngerFreeUnit() const
{
	// If it's a "rival territory" Unit (e.g. Caravel) don't worry about it
	if (isRivalTerritory())
		return true;

	// Don't care about noncombat units
	if (!IsCombatUnit())
		return true;

	// We don't care about other Minors or the Barbs
	return !GET_PLAYER(getOwner()).isMajorCiv();
}

//	---------------------------------------------------------------------------
int CvUnit::getCombatDamage(int iStrength, int iOpponentStrength, bool bIncludeRand, bool bAttackerIsCity, bool bDefenderIsCity) const
{
	int iModifier = 0;

	// Modify damage for when a city "attacks" a unit
	if (bAttackerIsCity)
	{
		//since this is melee combat, it's actually the city defending against an attack
		iModifier += /*0*/ GD_INT_GET(CITY_ATTACKING_DAMAGE_MOD) - 100;

		//sometimes we take even less damage from cities
		iModifier -= GetDamageReductionCityAssault();
	}
	// Modify damage for when unit is attacking a city
	else if (bDefenderIsCity)
	{
		iModifier += /*0*/ GD_INT_GET(ATTACKING_CITY_MELEE_DAMAGE_MOD) - 100;
	}
	
	CvSeeder randomSeed;
	if (bIncludeRand)
	{
		randomSeed
			.mixAssign(plot()->GetPseudoRandomSeed())
			.mixAssign(GetID())
			.mixAssign(iStrength)
			.mixAssign(iOpponentStrength);
	}

	return CvUnitCombat::DoDamageMath(
		iStrength,
		iOpponentStrength,
		/*2400*/ GD_INT_GET(ATTACK_SAME_STRENGTH_MIN_DAMAGE), //ignore the min part, it's misleading
		/*1200*/ GD_INT_GET(ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE),
		bIncludeRand,
		randomSeed,
		iModifier ) / 100;
}

//	--------------------------------------------------------------------------------
void CvUnit::move(CvPlot& targetPlot, bool bShow)
{
	VALIDATE_OBJECT
	CvAssert(canMoveOrAttackInto(targetPlot) || isOutOfAttacks());

	CvPlot* pOldPlot = plot();
	CvAssertMsg(pOldPlot, "pOldPlot needs to have a value");

	//will never be more than we have left!
	int iMoveCost = targetPlot.movementCost(this, plot(), getMoves());

	// we need to get our dis/embarking on
	bool bChangeEmbarkedState = CanEverEmbark() && (targetPlot.needsEmbarkation(this) != pOldPlot->needsEmbarkation(this));
	if (bChangeEmbarkedState)
	{
		if(isEmbarked() && !targetPlot.needsEmbarkation(this)) // moving from water to the land
		{
			// If we have some queued moves, execute them now, so that the disembark is done at the proper location visually
			PublishQueuedVisualizationMoves();

			disembark(pOldPlot);
		}
		else if(!isEmbarked() && targetPlot.needsEmbarkation(this))  // moving from land to the water
		{
			// If we have some queued moves, execute them now, so that the embark is done at the proper location visually
			PublishQueuedVisualizationMoves();

			embark(pOldPlot);
		}
	}

	if (MOD_LINKED_MOVEMENT && IsLinkedLeader()) // moving the whole stack, one plot at a time
	{
		UnitIdContainer LinkedUnitIDs = GetLinkedUnits();
		bool bCanDoLinkedMove = true;
		vector<CvUnit*> LinkedUnits;
		for (int iI = 0; iI < (int)LinkedUnitIDs.size(); iI++)
		{
			CvUnit* pLinkedUnit = GET_PLAYER(m_eOwner).getUnit(LinkedUnitIDs[iI]);
			if (!pLinkedUnit->canMoveInto(targetPlot, CvUnit::MOVEFLAG_DESTINATION)) {
				bCanDoLinkedMove = false;
				break;
			}
			else {
				LinkedUnits.push_back(pLinkedUnit);
			}
		}
		if (bCanDoLinkedMove)
		{
			setXY(targetPlot.getX(), targetPlot.getY(), true, true, bShow && targetPlot.isVisibleToWatchingHuman(), bShow);

			for (int iI = 0; iI < (int)LinkedUnits.size(); iI++)
			{
				CvUnit* pLinkedUnit = LinkedUnits[iI];
				pLinkedUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), targetPlot.getX(), targetPlot.getY(), CvUnit::MOVEFLAG_DESTINATION);
			}
		}
	}
	//important, first do the move, then subtract the cost
	//that way setXY can tell whether it's the initial move this turn
	else
	{
		setXY(targetPlot.getX(), targetPlot.getY(), true, true, bShow && targetPlot.isVisibleToWatchingHuman(), bShow);
	}

	if (!targetPlot.IsRestoreMoves())
		changeMoves(-iMoveCost);
}

bool CvUnit::EmergencyRebase()
{
	if (getDomainType() != DOMAIN_AIR)
		return false;

	if (canRebaseAt(getX(), getY(), true))
		return true;

	int iLoopCity = 0;
	for (CvCity* pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoopCity))
	{
		if (canRebaseAt(pLoopCity->getX(), pLoopCity->getY(), true) && HomelandAIHelpers::ScoreAirBase(pLoopCity->plot(), getOwner(), false, GetRange()) > 0)
		{
			rebase(pLoopCity->getX(), pLoopCity->getY(), true);
			return true;
		}
	}

	for (CvCity* pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoopCity))
	{
		if (canRebaseAt(pLoopCity->getX(), pLoopCity->getY(), true) && HomelandAIHelpers::ScoreAirBase(pLoopCity->plot(), getOwner(), true, GetRange()) > 0)
		{
			rebase(pLoopCity->getX(), pLoopCity->getY(), true);
			return true;
		}
	}

	int iLoop = 0;
	for (CvUnit* pLoopUnit = GET_PLAYER(getOwner()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwner()).nextUnit(&iLoop))
	{
		if (pLoopUnit->AI_getUnitAIType()!=UNITAI_CARRIER_SEA)
			continue;

		if (canRebaseAt(pLoopUnit->getX(), pLoopUnit->getY(), true) && HomelandAIHelpers::ScoreAirBase(pLoopUnit->plot(), getOwner(), true, GetRange()) > 0)
		{
			rebase(pLoopUnit->getX(), pLoopUnit->getY(), true);
			return true;
		}
	}

	if (GC.getLogging() && GC.getAILogging())
		GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage("failed to find a valid plot for air unit");

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::jumpToNearestValidPlot()
{
	VALIDATE_OBJECT
	CvAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	CvAssertMsg(!isFighting(), "isFighting did not return false as expected");

	if (getDomainType() == DOMAIN_AIR)
		return EmergencyRebase();

	//check for no-op
	if (plot() && canMoveInto(*plot(), CvUnit::MOVEFLAG_DESTINATION))
		return true;

	//ignore all sorts of restrictions
	SPathFinderUserData data(this, CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF | CvUnit::MOVEFLAG_IGNORE_ENEMIES | CvUnit::MOVEFLAG_IGNORE_ZOC, 12);

	CvPlot* pBestPlot = NULL;
	vector<SPlotWithScore> candidates;
	ReachablePlots reachablePlots = GC.GetPathFinder().GetPlotsInReach(plot(), data);
	for (ReachablePlots::iterator it = reachablePlots.begin(); it != reachablePlots.end(); ++it)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);

		//plot must be empty even of civilians
		if (pLoopPlot->getNumUnits() == 0 && canMoveInto(*pLoopPlot,CvUnit::MOVEFLAG_DESTINATION))
		{
			int iValue = it->iNormalizedDistanceRaw + GET_PLAYER(getOwner()).GetCityDistanceInPlots(pLoopPlot);

			//avoid putting ships on lakes etc (only possible in degenerate cases anyway)
			if (getDomainType() == DOMAIN_SEA)
				if (pLoopPlot->landmass()->isLake() || pLoopPlot->landmass()->getCitiesPerPlayer(getOwner()) == 0 || !isNativeDomain(pLoopPlot))
					iValue += 20000;

			//avoid embarkation but not all all cost
			if (pLoopPlot->needsEmbarkation(this) && !isEmbarked())
				iValue += 3000;

			//we allowed passage through enemy territory but we don't want to end up there
			if (!GET_PLAYER(getOwner()).IsAtWarWith(pLoopPlot->getOwner()))
				candidates.push_back(SPlotWithScore(pLoopPlot,iValue));
		}
	}

	//we want lowest scores first
	std::stable_sort(candidates.begin(), candidates.end());

	for (size_t i = 0; i < candidates.size(); i++)
	{
		CvPlot* pTestPlot = candidates[i].pPlot;

		// check to make sure this is not a dead end
		// alternatively we could verify against all plots reachable from owner's capital?
		SPathFinderUserData data2(this, CvUnit::MOVEFLAG_IGNORE_STACKING_SELF, 4);
		ReachablePlots plots2 = GC.GetPathFinder().GetPlotsInReach(pTestPlot->getX(), pTestPlot->getY(), data2);

		//if we have lots of room here, use the plot immediately
		if (plots2.size() > 23)
		{
			pBestPlot = pTestPlot;
			break;
		}
	}

	if (!pBestPlot && !candidates.empty())
	{
		//try again if there are only bad places
		pBestPlot = candidates.front().pPlot;
	}

	//last chance
	if (!pBestPlot)
	{
		CvCity* pClosestCity = (getDomainType()==DOMAIN_SEA) ? 
			OperationalAIHelpers::GetClosestFriendlyCoastalCity(getOwner(),plot(), 1) : 
			GET_PLAYER(getOwner()).GetClosestCityByPlots(plot());
		if (pClosestCity)
			return jumpToNearestValidPlotWithinRange(6, pClosestCity->plot());
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		if(pBestPlot != NULL)
		{
			strLogString.Format("Jump to nearest valid plot by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
								pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		else
		{
			strLogString.Format("Can't find a valid plot for %s at, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
	}

	if(pBestPlot != NULL)
	{
		ClearMissionQueue(); //do this before changing the position in case we have queued moves
		SetActivityType(ACTIVITY_AWAKE);
		if (pBestPlot->needsEmbarkation(this))
			embark(plot()); //at the current plot so that the vision update works correctly
		else 
			disembark(plot());

		setXY(pBestPlot->getX(), pBestPlot->getY(), false, false);
		return true;
	}
	else
	{
		return false;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::jumpToNearestValidPlotWithinRange(int iRange, CvPlot* pStartPlot)
{
	CvPlot* pBestPlot = NULL;
	if (!pStartPlot)
		pStartPlot = plot();

	if (!pStartPlot)
		return false;

	if (getDomainType() == DOMAIN_AIR)
		return EmergencyRebase();

	//nothing to do?
	if (canMoveInto(*pStartPlot, CvUnit::MOVEFLAG_DESTINATION))
	{
		if (at(pStartPlot->getX(),pStartPlot->getY()))
			return true;

		pBestPlot = pStartPlot;
	}
	else
	{
		int iBestValue = INT_MAX;
		iRange = min(max(1, iRange), 5);

		for (int i = 1; i < RING_PLOTS[iRange]; i++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pStartPlot, i);

			//needs to be visible so we don't run into problems with stacking
			if (!pLoopPlot || !pLoopPlot->isVisible(getTeam()))
				continue;

			if (!pLoopPlot->isEnemyUnit(getOwner(), true, false) && !pLoopPlot->isNeutralUnit(getOwner(), true, false))
			{
				//need to check for invisible units as well ...
				if (canMoveInto(*pLoopPlot, CvUnit::MOVEFLAG_DESTINATION))
				{
					int iValue = (plotDistance(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY()) * 2);

					//avoid putting ships on lakes etc
					if (getDomainType() == DOMAIN_SEA && pLoopPlot->area()->getCitiesPerPlayer(getOwner()) == 0)
						iValue += 54;

					//avoid embarkation
					if (getDomainType() == DOMAIN_LAND && pLoopPlot->needsEmbarkation(this))
						iValue += 23;

					//try to stay within the same area
					if (pLoopPlot->getArea() != plot()->getArea())
						iValue += 11;

					if (iValue < iBestValue || (iValue == iBestValue && GC.getGame().randRangeExclusive(0, 3, CvSeeder(pLoopPlot->GetPseudoRandomSeed())) < 2))
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}
	}

	if(pBestPlot != NULL)
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Jump to nearest valid plot within range by %s , X: %d, Y: %d, From X: %d, From Y: %d", getName().GetCString(),
								pBestPlot->getX(), pBestPlot->getY(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		ClearMissionQueue(); //do this before changing the position in case we have queued moves
		SetActivityType(ACTIVITY_AWAKE);
		if (pBestPlot->needsEmbarkation(this))
			embark(plot()); //at the current plot so that the vision update works correctly
		else 
			disembark(plot());

		setXY(pBestPlot->getX(), pBestPlot->getY(), false, false);
		return true;
	}
	else
	{
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Can't find a valid plot within range for %s, X: %d, Y: %d", getName().GetCString(), getX(), getY());
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		CUSTOMLOG("jumpToNearestValidPlotWithinRange(%i) failed for unit %s at plot (%i, %i)", iRange, getName().GetCString(), getX(), getY());
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanAutomate(AutomateTypes eAutomate, bool bTestVisibility) const
{
	VALIDATE_OBJECT
	if(eAutomate == NO_AUTOMATE)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(eAutomate == 2)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_MISSIONARY)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(GetID());
			if(pUnit)
			{
				CvCity* pTarget = GET_PLAYER(m_eOwner).GetReligionAI()->ChooseMissionaryTargetCity(pUnit,vector<pair<int,int>>());
				if(pTarget == NULL)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (/*0*/ GD_INT_GET(UNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED) == 1)
			{
				return false;
			}
		}
	}
	if(eAutomate == 3)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_ARCHAEOLOGIST)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(GetID());
			if(pUnit)
			{
				if(!GET_PLAYER(m_eOwner).GetHomelandAI()->FindTestArchaeologistPlotPrimer(pUnit))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (/*0*/ GD_INT_GET(UNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED) == 1)
			{
				return false;
			}
		}
	}
#endif
	if(eAutomate == 4)
	{
		if ((getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		if(AI_getUnitAIType() != UNITAI_MESSENGER)
		{
			return false;
		}

		if (!bTestVisibility)
		{
			CvUnit* pUnit = GET_PLAYER(m_eOwner).getUnit(GetID());
			if(pUnit)
			{
				CvPlot* pTarget = GET_PLAYER(m_eOwner).ChooseMessengerTargetPlot(pUnit);
				if(pTarget == NULL)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (!bTestVisibility)
		{
			if (/*0*/ GD_INT_GET(UNIT_AUTO_EXTRA_AUTOMATIONS_DISABLED) == 1)
			{
				return false;
			}
		}
	}
	switch(eAutomate)
	{
	case AUTOMATE_BUILD:
		if((AI_getUnitAIType() != UNITAI_WORKER) && (AI_getUnitAIType() != UNITAI_WORKER_SEA))
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if (/*0*/ GD_INT_GET(UNIT_WORKER_AUTOMATION_DISABLED) == 1)
			{
				return false;
			}
		}

		break;

	case AUTOMATE_EXPLORE:
		if((GetBaseCombatStrength() == 0) || (getDomainType() == DOMAIN_AIR) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if(GET_PLAYER(m_eOwner).GetEconomicAI()->GetExplorationPlots( getDomainType() ).empty())
			{
				return false;
			}
		}

		if (/*0*/ GD_INT_GET(UNIT_AUTO_EXPLORE_FULL_DISABLED) == 1)
		{
			return false;
		}

		if(!bTestVisibility)
		{
			if (/*0*/ GD_INT_GET(UNIT_AUTO_EXPLORE_DISABLED) == 1)
			{
				return false;
			}
		}
		break;
	default:
		CvAssert(false);
		break;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::Automate(AutomateTypes eAutomate)
{
	VALIDATE_OBJECT
	if(!CanAutomate(eAutomate))
	{
		return;
	}

	SetAutomateType(eAutomate);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canScrap(bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(plot()->isUnitFighting())
		return false;

	if (isDelayedDeath())
		return false;

	if(!canMove())
		return false;

	if(!bTestVisible)
	{
		//prevent an exploit where players disband units to deny kill yields to their enemies
		if (MOD_BALANCE_VP && getDomainType() != DOMAIN_AIR && !GET_PLAYER(m_eOwner).GetPossibleAttackers(*plot(), getTeam()).empty() && !plot()->isCity())
			return false;

		if (/*0*/ GD_INT_GET(UNIT_DELETE_DISABLED) == 1)
		{
			return false;
		}
	}

	if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanDisbandUnit, getOwner(), GetID()) == GAMEEVENTRETURN_FALSE)
		return false;

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::scrap(bool bDelay)
{
	VALIDATE_OBJECT
	if(!canScrap())
	{
		return;
	}

	if(plot()->getOwner() == getOwner())
	{
		int iGold = GetScrapGold();
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
	}

	kill(bDelay);
}

//	--------------------------------------------------------------------------------
/// How much gold do we get?
int CvUnit::GetScrapGold() const
{
	int iNumGold = getUnitInfo().GetProductionCost();

	if(iNumGold <= 0)
		return 0;

	iNumGold *= /*25*/ GD_INT_GET(DISBAND_UNIT_REFUND_PERCENT);
	iNumGold /= 100;

	// Modify amount based on current health
	iNumGold *= 100 * (GetMaxHitPoints() - getDamage()) / GetMaxHitPoints();
	iNumGold /= 100;


	// slewis - moved this out of the plot check because the game speed should effect all scrap gold calculations, not just the ones that are in the owner's plot
	// Modify for game speed
	iNumGold *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
	iNumGold /= 100;

#if defined(MOD_CIV6_WORKER)
	//if we are a builder (something with builderstrength), our value decrease with our build strength
	if (MOD_CIV6_WORKER && getBuilderStrength() > 0)
	{
		iNumGold *= getUnitInfo().GetBuilderStrength() - getBuilderStrength();
		iNumGold /= getUnitInfo().GetBuilderStrength();
	}
#endif

	// Check to see if we are transporting other units and add in their scrap value as well.
	CvPlot* pPlot = plot();
	if(pPlot)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode)
		{
			const CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);

			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->getTransportUnit() == this)	// Is this the transport for the search unit?
				{
					iNumGold += pLoopUnit->GetScrapGold();
				}
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	return iNumGold;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canGift(bool bTestVisible, bool bTestTransport) const
{
	VALIDATE_OBJECT
	const CvPlot* pPlot = plot();
	const CvUnit* pTransport = getTransportUnit();

	if(isDelayedDeath())
		return false;

	if(!(pPlot->isOwned()))
	{
		return false;
	}

	if(pPlot->getOwner() == getOwner())
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	if(pPlot->isVisibleEnemyUnit(pPlot->getOwner()))
	{
		return false;
	}

	if(!isNativeDomain(pPlot) && NULL == pTransport)
	{
		return false;
	}

	if (getDamage() > 0)
		return false;

	if (GetDanger() > 0)
		return false;

	// Minors
	if (GET_PLAYER(pPlot->getOwner()).isMinorCiv())
	{
		// No settlers
		if(isFound() || IsFoundAbroad())
			return false;

		// No scouts
		if (getUnitInfo().GetDefaultUnitAIType() == UNITAI_EXPLORE)
			return false;

		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);

		// No non-combat units
		if (!IsCombatUnit())
		{
			// Unless okay by trait
			if (kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}
	}
	else if (MOD_NO_MAJORCIV_GIFTING)
	{
		return false;
	}

	// No for religious units
	if (getUnitInfo().IsSpreadReligion() || getUnitInfo().IsRemoveHeresy())
	{
		return false;
	}

	if(bTestTransport)
	{
		if(pTransport && pTransport->getTeam() != pPlot->getTeam())
		{
			return false;
		}
	}

	if(!bTestVisible)
	{
		if(GET_TEAM(pPlot->getTeam()).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(pPlot->getTeam()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}

		if(GET_PLAYER(pPlot->getOwner()).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(pPlot->getOwner()).getUnitClassMaking(getUnitClassType())))
		{
			return false;
		}
	}

	if (atWar(pPlot->getTeam(), getTeam()))
		return false;

	if (MOD_EVENTS_MINORS_INTERACTION)
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanGiftUnit, getOwner(), pPlot->getOwner(), GetID()) == GAMEEVENTRETURN_FALSE)
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::gift(bool bTestTransport)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = NULL;
	CvUnit* pGiftUnit = NULL;
	CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;
	CvString strBuffer;
	PlayerTypes eCurrentOwner = getOwner();

	if (!canGift(false, bTestTransport))
		return;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->kill(false);
			pUnitNode = pPlot->headUnitNode();
		}
		else
		{
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}

	CvAssertMsg(plot()->getOwner() != NO_PLAYER, "plot()->getOwner() is not expected to be equal with NO_PLAYER");
	pGiftUnit = GET_PLAYER(plot()->getOwner()).initUnit(getUnitType(), getX(), getY(), AI_getUnitAIType(), REASON_GIFT, false, false);

	CvAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	if (pGiftUnit != NULL)
	{
		pGiftUnit->convert(this, false);
		pGiftUnit->setupGraphical();

		pGiftUnit->GetReligionDataMutable()->SetReligion(GetReligionData()->GetReligion());
		pGiftUnit->GetReligionDataMutable()->SetReligiousStrength(GetReligionData()->GetReligiousStrength());
		pGiftUnit->GetReligionDataMutable()->SetSpreadsUsed(GetReligionData()->GetSpreadsUsed());

		if (pGiftUnit->getOwner() == GC.getGame().getActivePlayer())
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eCurrentOwner).getNameKey(), pGiftUnit->getNameKey());
			DLLUI->AddUnitMessage(0, pGiftUnit->GetIDInfo(), pGiftUnit->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer);
		}

		// Set gifted by player
		pGiftUnit->SetGiftedByPlayer(eCurrentOwner);

		CvPlayer* pMinorCiv = &GET_PLAYER(pGiftUnit->getOwner());

		// Gift to Minor Civ
		CvPlayer& kPlayer = GET_PLAYER(eCurrentOwner);
		if (!kPlayer.isMinorCiv() && pMinorCiv->isMinorCiv())
		{
			pMinorCiv->GetMinorCivAI()->DoUnitGiftFromMajor(eCurrentOwner, pGiftUnit, /*bDistanceGift*/ false);
		}
	}
}

//	--------------------------------------------------------------------------------
// Long-distance gift to a city state
bool CvUnit::CanDistanceGift(PlayerTypes eToPlayer) const
{
	VALIDATE_OBJECT

	if (eToPlayer == NO_PLAYER)
		return false;

	// Minors
	if(GET_PLAYER(eToPlayer).isMinorCiv())
	{
		// No settlers
		if(isFound() || IsFoundAbroad())
			return false;

		if (getDamage() > 0)
			return false;

		// No scouts
		if (getUnitInfo().GetDefaultUnitAIType() == UNITAI_EXPLORE)
			return false;

		// No scouts
		UnitClassTypes eScoutClass = (UnitClassTypes) GC.getInfoTypeForString("UNITCLASS_SCOUT", true);
		if (eScoutClass != NO_UNITCLASS && eScoutClass == getUnitClassType())
			return false;

		CvPlayer& kPlayer = GET_PLAYER(m_eOwner);

		// No non-combat units
		if (!IsCanAttack())
		{
			// Unless okay by trait
			if(kPlayer.GetPlayerTraits()->GetGreatPersonGiftInfluence() == 0 || !IsGreatPerson())
			{
				return false;
			}
		}

		// Is there a distance gift from us waiting to be delivered?
		CvMinorCivAI* pMinorCivAI = GET_PLAYER(eToPlayer).GetMinorCivAI();
		if (pMinorCivAI && pMinorCivAI->getIncomingUnitGift(getOwner()).getArrivalCountdown() != -1)
		{
			return false;
		}
	}

	// No Majors
	else
		return false;

	TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();

	// Maxed out unit class for team
	if(GET_TEAM(eToTeam).isUnitClassMaxedOut(getUnitClassType(), GET_TEAM(eToTeam).getUnitClassMaking(getUnitClassType())))
		return false;

	// Maxed out unit class for Player
	if(GET_PLAYER(eToPlayer).isUnitClassMaxedOut(getUnitClassType(), GET_PLAYER(eToPlayer).getUnitClassMaking(getUnitClassType())))
		return false;

	if(atWar(eToTeam, getTeam()))
		return false;
	
	if (MOD_EVENTS_MINORS_INTERACTION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanGiftUnit, getOwner(), eToPlayer, GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
	
	return true;			
}

//	--------------------------------------------------------------------------------
#if defined(MOD_CORE_PER_TURN_DAMAGE)
int CvUnit::addDamageReceivedThisTurn(int iDamage, CvUnit* pAttacker)
{
	m_iDamageTakenThisTurn+=iDamage;

	//remember the attacker for AI danger calculation - in case he came out of nowhere, now moves and becomes invisible again
	if (pAttacker && !isHuman())
		GET_PLAYER(getOwner()).AddKnownAttacker(pAttacker);

	return m_iDamageTakenThisTurn;
}

void CvUnit::flipDamageReceivedPerTurn()
{
	m_iDamageTakenLastTurn = m_iDamageTakenThisTurn;
	m_iDamageTakenThisTurn = 0;
}

bool CvUnit::isProjectedToDieNextTurn() const
{
	return m_iDamageTakenLastTurn>GetCurrHitPoints();
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::canLoadUnit(const CvUnit& unit, const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT
	if(&unit == this)
	{
		return false;
	}

	if(unit.getTeam() != getTeam())
	{
		return false;
	}

	if(getCargo() > 0)
	{
		return false;
	}

	if(unit.isCargo())
	{
		return false;
	}

	if(!(unit.cargoSpaceAvailable(getSpecialUnitType(), getDomainType())))
	{
		return false;
	}

	if(!(unit.atPlot(targetPlot)))
	{
		return false;
	}

	if(!isHiddenNationality() && unit.isHiddenNationality())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::loadUnit(CvUnit& unit)
{
	VALIDATE_OBJECT
	CvPlot* p = plot();
	if(!p || !canLoadUnit(unit, *p))
	{
		return;
	}

	setTransportUnit(&unit);
}

//	--------------------------------------------------------------------------------
bool CvUnit::shouldLoadOnMove(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isCargo())
	{
		return false;
	}

	if (pPlot == NULL)
		return false;

	switch(getDomainType())
	{
	case DOMAIN_LAND:
		if(pPlot->isWater())
		{
			return true;
		}
		break;
	case DOMAIN_AIR:
		if(!pPlot->isFriendlyCity(*this))
		{
			return true;
		}
		break;
	case DOMAIN_SEA:
		return false;
		break;
	default:
		break;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canLoad(const CvPlot& targetPlot) const
{
	VALIDATE_OBJECT

	if(NO_SPECIALUNIT != getSpecialUnitType())
	{
		CvSpecialUnitInfo* pkSpecialUnitInfo = GC.getSpecialUnitInfo(getSpecialUnitType());
		if(pkSpecialUnitInfo)
		{
			if(pkSpecialUnitInfo->isCityLoad())
			{
				if(targetPlot.isCity())
				{
					return true;
				}

#if defined(MOD_EVENTS_REBASE)
				if (MOD_EVENTS_REBASE)
				{
					if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanLoadAt, getOwner(), GetID(), targetPlot.getX(), targetPlot.getY()) == GAMEEVENTRETURN_TRUE)
					{
						return true;
					}
				}
				else
				{
#endif
					ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
					if (pkScriptSystem)
					{
						CvLuaArgsHandle args;
						args->Push(getOwner());
						args->Push(GetID());
						args->Push(targetPlot.getX());
						args->Push(targetPlot.getY());

						// Attempt to execute the game events.
						// Will return false if there are no registered listeners.
						bool bResult = false;
						if (LuaSupport::CallTestAny(pkScriptSystem, "CanLoadAt", args.get(), bResult))
						{
							// Check the result.
							if (bResult) {
								return true;
							}
						}
					}
#if defined(MOD_EVENTS_REBASE)
				}
#endif
			}
		}
	}

	//this is a bug right here - the result should not depend on the current state of the unit!
	if (isEmbarked())
		return false;

	const IDInfo* pUnitNode = targetPlot.headUnitNode();
	while(pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = targetPlot.nextUnitNode(pUnitNode);

		if(pLoopUnit && canLoadUnit(*pLoopUnit, targetPlot))
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
void CvUnit::load()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = NULL;
	CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;
	int iPass = 0;

	pPlot = plot();

	if(!pPlot || !canLoad(*pPlot))
	{
		return;
	}


	for(iPass = 0; iPass < 2; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && canLoadUnit(*pLoopUnit, *pPlot))
			{
				if((iPass == 0) ? (pLoopUnit->getOwner() == getOwner()) : (pLoopUnit->getTeam() == getTeam()))
				{
					setTransportUnit(pLoopUnit);
					break;
				}
			}
		}

		if(isCargo())
		{
			break;
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::canUnload() const
{
	VALIDATE_OBJECT

	if(getTransportUnit() == NULL)
	{
		return false;
	}

	if(!isNativeDomain(plot()))
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
void CvUnit::unload()
{
	VALIDATE_OBJECT
	if(!canUnload())
	{
		return;
	}

	setTransportUnit(NULL);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canUnloadAll() const
{
	VALIDATE_OBJECT
	return getCargo() != 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::unloadAll()
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = NULL;
	CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;

	if(!canUnloadAll())
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->canUnload())
			{
				pLoopUnit->setTransportUnit(NULL);
			}
			else
			{
				CvAssert(isHuman());
				pLoopUnit->SetActivityType(ACTIVITY_AWAKE);
			}
		}
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::canHold(const CvPlot* pPlot) const // skip turn
{
	VALIDATE_OBJECT
	if(isHuman() && !IsFortified())  // we aren't fortified
	{
		if (!canEndTurnAtPlot(pPlot))
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canSleep(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	// Can't sleep if we're a Unit that can normally fortify
	if(canFortify(pPlot))
	{
		return false;
	}

	if(isHuman() && !IsFortified())  // we aren't fortified
	{
		if (!canEndTurnAtPlot(pPlot))
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canFortify(const CvPlot* pPlot) const
{
	if (!canEndTurnAtPlot(pPlot))
	{
		return false;
	}

	return IsEverFortifyable() && isNativeDomain(pPlot);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirPatrol(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	if(!canAirDefend(pPlot))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsRangeAttackIgnoreLOS() const
{
	VALIDATE_OBJECT
	return GetRangeAttackIgnoreLOSCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetRangeAttackIgnoreLOSCount() const
{
	VALIDATE_OBJECT
	return m_iRangeAttackIgnoreLOSCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRangeAttackIgnoreLOSCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iRangeAttackIgnoreLOSCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCityAttackSupport() const
{
	VALIDATE_OBJECT
	//In VP city attack support units are civilians with a sapper bonus
	//sapper is a separate promotion that can theoretically apply to combat units as well
	return (getUnitInfo().IsCityAttackSupport() || m_iCityAttackOnlyCount > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCityAttackOnlyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCityAttackOnlyCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCaptureDefeatedEnemy() const
{
	VALIDATE_OBJECT
	return m_iCaptureDefeatedEnemyCount > 0 || m_iCaptureDefeatedEnemyChance > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCaptureDefeatedEnemyCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCaptureDefeatedEnemyCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCaptureChance(CvUnit *pEnemy)
{
	// This unit can't capture enemies
	if (!IsCaptureDefeatedEnemy())
		return 0;

	// Can't capture units belonging to different domain
	if (getDomainType() != pEnemy->getDomainType())
		return 0;
	
	// This enemy cannot be captured
	if(pEnemy->GetCannotBeCaptured())
		return 0;

	// Can't capture units in cities
	if (pEnemy->plot()->isCity())
		return 0;

	// This unit has a fixed capture chance? Use it!
	int iFixedCaptureChance = GetCaptureDefeatedEnemyChance();
	if (iFixedCaptureChance > 0)
		return iFixedCaptureChance;

	if (MOD_BALANCE_VP)
	{
		// Count adjacent enterable plots
		CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pEnemy->plot());
		int iEnterablePlotCount = 0;
		for (int iCount = 0; iCount < NUM_DIRECTION_TYPES; iCount++)
		{
			CvPlot* pLoopPlot = aPlotsToCheck[iCount];
			if (pLoopPlot && pEnemy->canEnterTerrain(*pLoopPlot, MOVEFLAG_NO_EMBARK | MOVEFLAG_DESTINATION))
				iEnterablePlotCount++;
		}

		// Count adjacent enemies (doesn't have to be YOUR units!)
		int iAdjEnemyCount = pEnemy->plot()->GetNumEnemyUnitsAdjacent(pEnemy->getTeam(), pEnemy->getDomainType());

		// Enemy is captured if and only if more than half of the surrounding enterable plots are occupied by its enemies
		if (iAdjEnemyCount * 2 >= iEnterablePlotCount)
			return 100;
	}
	else
	{
		// Look at ratio of intrinsic combat strengths
		CvUnitEntry *pkEnemyInfo = GC.getUnitInfo(pEnemy->getUnitType());
		int iTheirCombat = pkEnemyInfo->GetCombat();
		if (iTheirCombat > 0)
		{
			int iMyCombat = m_pUnitInfo->GetCombat();
			int iComputedChance = /*10*/ GD_INT_GET(COMBAT_CAPTURE_MIN_CHANCE) + iMyCombat * /*40*/ GD_INT_GET(COMBAT_CAPTURE_RATIO_MULTIPLIER) / iTheirCombat;
			return min(/*80*/ GD_INT_GET(COMBAT_CAPTURE_MAX_CHANCE), iComputedChance);
		}
	}

	return 0;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvUnit::ChangeCannotBeCapturedCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCannotBeCapturedCount += iChange;
	}
}
//	--------------------------------------------------------------------------------
bool CvUnit::GetCannotBeCaptured()
{
	VALIDATE_OBJECT
	return m_iCannotBeCapturedCount > 0;
}
//	--------------------------------------------------------------------------------
void CvUnit::ChangeForcedDamageValue(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iForcedDamage += iChange;
	}
}
//	--------------------------------------------------------------------------------
int CvUnit::getForcedDamageValue()
{
	VALIDATE_OBJECT
	return m_iForcedDamage;
}
//	--------------------------------------------------------------------------------
void CvUnit::ChangeChangeDamageValue(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iChangeDamage += iChange;
	}
}
//	--------------------------------------------------------------------------------
int CvUnit::getChangeDamageValue()
{
	VALIDATE_OBJECT
	return m_iChangeDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetPromotionDuration(PromotionTypes eIndex, int iValue)
{
	std::map<PromotionTypes, int>& m_map = m_PromotionDuration;
	if (iValue > 0)
		m_map[eIndex] = iValue;
	else
		m_map.erase(eIndex);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangePromotionDuration(PromotionTypes eIndex, int iChange)
{
	std::map<PromotionTypes, int>& m_map = m_PromotionDuration;
	if (m_map.find(eIndex) != m_map.end())
	{
		m_map[eIndex] += iChange;
		if (m_map[eIndex] == 0)
			m_map.erase(eIndex);
	}
	else
		m_map[eIndex] = iChange;
}
//	--------------------------------------------------------------------------------
int CvUnit::getPromotionDuration(PromotionTypes eIndex) const
{
	const std::map<PromotionTypes, int>& m_map = m_PromotionDuration;
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetTurnPromotionGained(PromotionTypes eIndex, int iValue)
{
	std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained;
	if (iValue>0)
		m_map[eIndex] = iValue;
	else
		m_map.erase(eIndex);
}
//	--------------------------------------------------------------------------------
int CvUnit::getTurnPromotionGained(PromotionTypes eIndex) const
{
	const std::map<PromotionTypes, int>& m_map = m_TurnPromotionGained;
	std::map<PromotionTypes, int>::const_iterator it = m_map.find(eIndex);
	if (it != m_map.end())
		return it->second;
	else
		return 0;
}

void CvUnit::SetNegatorPromotion(int iValue)
{
	if(iValue != m_iNegatorPromotion)
	{
		m_iNegatorPromotion = iValue;
	}
}

int CvUnit::getNegatorPromotion()
{
	return m_iNegatorPromotion;
}

#endif

//	--------------------------------------------------------------------------------
bool CvUnit::canSetUpForRangedAttack(const CvPlot* /*pPlot*/) const
{
	return false; //no longer used
}

//	--------------------------------------------------------------------------------
bool CvUnit::isSetUpForRangedAttack() const
{
	return true; //no longer used
}

//	--------------------------------------------------------------------------------
void CvUnit::setSetUpForRangedAttack(bool /*bValue*/)
{
	//no longer used
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEmbarkAtPlot(const CvPlot*) const
{
	return false; //no longer used
}

//	--------------------------------------------------------------------------------
bool CvUnit::canDisembarkAtPlot(const CvPlot*) const
{
	return false; //no longer used
}

//	--------------------------------------------------------------------------------
bool CvUnit::canEmbarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck /* = false */, int iMoveFlags) const
{
	VALIDATE_OBJECT

	if(!CanEverEmbark())
	{
		return false;
	}
		
	if(isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(!targetPlot.needsEmbarkation(this))
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	if(originPlot.needsEmbarkation(this))
	{
		return false;
	}

	return canMoveInto(targetPlot, iMoveFlags);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canDisembarkOnto(const CvPlot& originPlot, const CvPlot& targetPlot, bool bOverrideEmbarkedCheck /* = false */, int iMoveFlags) const
{
	VALIDATE_OBJECT

	if(!CanEverEmbark())
	{
		return false;
	}
		
	if(!isEmbarked() && !bOverrideEmbarkedCheck)
	{
		return false;
	}

	if(targetPlot.needsEmbarkation(this))
	{
		return false;
	}

	if(!targetPlot.isRevealed(getTeam()))
	{
		return false;
	}

	if(!originPlot.needsEmbarkation(this))
	{
		return false;
	}

	return canMoveInto(targetPlot, iMoveFlags);
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanEverEmbark() const
{
	VALIDATE_OBJECT
	return (getDomainType() == DOMAIN_LAND && IsHasEmbarkAbility() && !isCargo() && !canMoveAllTerrain() && !isConvertUnit());
}

//	--------------------------------------------------------------------------------
void CvUnit::embark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);

	setEmbarked(true);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true), this);

	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), true);

	if (MOD_API_ACHIEVEMENTS && isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_EMBARK);
}

//	--------------------------------------------------------------------------------
void CvUnit::disembark(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);

	setEmbarked(false);

	if (canChangeVisibility())
		pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true), this);

	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitEmbark(pDllUnit.get(), false);
}

//	--------------------------------------------------------------------------------
void CvUnit::setEmbarked(bool bValue)
{
	VALIDATE_OBJECT
	if(m_bEmbarked != bValue)
	{
		m_bEmbarked = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
bool CvUnit::IsHasEmbarkAbility() const
{
#if defined(MOD_GLOBAL_CANNOT_EMBARK)
	if (MOD_GLOBAL_CANNOT_EMBARK && getUnitInfo().CannotEmbark()) {
		return false;
	}
#endif

	if (IsEmbarkDeepWater())
		return true;

	return GetEmbarkAbilityCount() > 0;
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
int CvUnit::GetEmbarkAbilityCount() const
{
	return m_iEmbarkAbilityCount;
}

//	--------------------------------------------------------------------------------
/// Does this Unit have the ability to embark?
void CvUnit::ChangeEmbarkAbilityCount(int iChange)
{
	if(iChange != 0)
	{
		m_iEmbarkAbilityCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsEmbarkAllWater() const
{
	VALIDATE_OBJECT
	return (GetEmbarkAllWaterCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkAllWaterCount(int iValue)
{
	m_iEmbarkedAllWaterCount += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkAllWaterCount() const
{
	return m_iEmbarkedAllWaterCount;
}

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
//	--------------------------------------------------------------------------------
bool CvUnit::IsEmbarkDeepWater() const
{
	VALIDATE_OBJECT
	return MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && (GetEmbarkDeepWaterCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkDeepWaterCount(int iValue)
{
	m_iEmbarkedDeepWaterCount += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkDeepWaterCount() const
{
	return m_iEmbarkedDeepWaterCount;
}
#endif

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkExtraVisibility(int iValue)
{
	m_iEmbarkExtraVisibility += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkExtraVisibility() const
{
	return m_iEmbarkExtraVisibility;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeEmbarkDefensiveModifier(int iValue)
{
	m_iEmbarkDefensiveModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkDefensiveModifier() const
{
	return m_iEmbarkDefensiveModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCapitalDefenseModifier(int iValue)
{
	m_iCapitalDefenseModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCapitalDefenseModifier() const
{
	return m_iCapitalDefenseModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCapitalDefenseFalloff(int iValue)
{
	m_iCapitalDefenseFalloff += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCapitalDefenseFalloff() const
{
	return m_iCapitalDefenseFalloff;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCityAttackPlunderModifier(int iValue)
{
	m_iCityAttackPlunderModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCityAttackPlunderModifier() const
{
	return m_iCityAttackPlunderModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeReligiousStrengthLossRivalTerritory(int iValue)
{
	m_iReligiousStrengthLossRivalTerritory += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetReligiousStrengthLossRivalTerritory() const
{
	return m_iReligiousStrengthLossRivalTerritory;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeTradeMissionInfluenceModifier(int iValue)
{
	m_iTradeMissionInfluenceModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTradeMissionInfluenceModifier() const
{
	return m_iTradeMissionInfluenceModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeTradeMissionGoldModifier(int iValue)
{
	m_iTradeMissionGoldModifier += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTradeMissionGoldModifier() const
{
	return m_iTradeMissionGoldModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeDiploMissionInfluence(int iValue)
{
	m_iDiploMissionInfluence += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetDiploMissionInfluence() const
{
	return m_iDiploMissionInfluence;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumGoodyHutsPopped() const
{
	return m_iNumGoodyHutsPopped;
}
//	--------------------------------------------------------------------------------
void CvUnit::SetNumGoodyHutsPopped(int iValue)
{
	m_iNumGoodyHutsPopped = iValue;
}
//	--------------------------------------------------------------------------------
bool CvUnit::TurnProcessed() const
{
	VALIDATE_OBJECT
	return m_bAITurnProcessed;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetTurnProcessed(bool bValue)
{
	VALIDATE_OBJECT
	//failsafe, setting any value on a zombie unit leads to a crash
	if(TurnProcessed() != bValue && !isDelayedDeath())
	{
		m_bAITurnProcessed = bValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::setTacticalMove(AITacticalMove eMove)
{
#ifdef VPDEBUG
	//sanity check (ignore units in armies ... especially escorts)
	if (m_eTacticalMove != AI_TACTICAL_MOVE_NONE && eMove != AI_TACTICAL_MOVE_NONE && m_eTacticalMove != eMove && getArmyID()==-1)
	{
		CvString msg = CvString::format("Warning, overwriting tactical move %s with %s for unit %d\n", tacticalMoveNames[m_eTacticalMove], tacticalMoveNames[eMove], m_iID);
		OutputDebugString(msg.c_str());
	}
#endif

	//clear homeland move, can't have both ...
	m_eHomelandMove = AI_HOMELAND_MOVE_NONE;
	m_iTacticalMoveSetTurn = GC.getGame().getGameTurn();
	m_eTacticalMove = eMove;
}

//	--------------------------------------------------------------------------------
AITacticalMove CvUnit::getTacticalMove(int* pTurnSet) const
{
	VALIDATE_OBJECT
	if (pTurnSet)
		*pTurnSet = m_iTacticalMoveSetTurn;

	return m_eTacticalMove;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canUseForAIOperation() const
{
	//already in an army?
	if (getArmyID() != -1)
		return false;

	//don't forget ordinary civilians! they are always fair game
	if (IsCivilianUnit() && !canUseForTacticalAI())
		return true;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	//do not poach important garrisons
	if (IsGarrisoned() && GetGarrisonedCity()->NeedsGarrison())
		return false;

	//check if it's a city zone! don't care about no-mans land.
	CvTacticalDominanceZone* pZone = kPlayer.GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(plot());
	if (pZone && pZone->GetZoneCity()!=NULL && pZone->GetOverallDominanceFlag() != TACTICAL_DOMINANCE_FRIENDLY)
		return false;

	//do not call GetPlotsWithEnemyInMovementRange, it's too expensive ...
	//instead look at our immediate neighbor plots only
	for (int i = 0; i < RING3_PLOTS; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(plot(),i);
		if (pTestPlot && pTestPlot->getBestDefender(NO_PLAYER, getOwner(), this, true) != NULL)
			return false;
	}

	//don't pull units out of important citadels
	return !(TacticalAIHelpers::IsPlayerCitadel(plot(), getOwner()) && TacticalAIHelpers::IsCloseToContestedBorder(&kPlayer, plot()));
}

//	--------------------------------------------------------------------------------
bool CvUnit::canUseForTacticalAI() const
{
	if (isDelayedDeath())
		return false;

	if (!canMove() || TurnProcessed())
		return false;

	//handled by homeland AI for stupid reasons
	if (AI_getUnitAIType() == UNITAI_EXPLORE || AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
		return false;

	//these are only used for operations
	if (AI_getUnitAIType() == UNITAI_CARRIER_SEA || AI_getUnitAIType() == UNITAI_ICBM)
		return false;

	//do not poach important garrisons
	if (IsGarrisoned() && GetGarrisonedCity()->NeedsGarrison())
		return false;

	//we want all barbarians ...
	if (isBarbarian())
		return true;

	//aircraft are not considered combat units!
	if (IsCombatUnit() || IsCanAttackRanged())
		return true;

	if (IsCombatSupportUnit())
	{
		if (IsGreatPerson())
			return GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
		else
			return true;
	}

	return false;
}

bool CvUnit::canUseNow() const
{
	return canMove() && !isDelayedDeath() && !TurnProcessed();
}

//	--------------------------------------------------------------------------------
/// Set a destination plot for multi-turn tactical moves
void CvUnit::SetTacticalAIPlot(CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if(pPlot != NULL)
	{
		m_iTacticalAIPlotX = pPlot->getX();
		m_iTacticalAIPlotY = pPlot->getY();
	}
	else
	{
		m_iTacticalAIPlotX = INVALID_PLOT_COORD;
		m_iTacticalAIPlotY = INVALID_PLOT_COORD;
	}
}

//	--------------------------------------------------------------------------------
/// Get the destination plot for multi-turn tactical moves
CvPlot* CvUnit::GetTacticalAIPlot() const
{
	VALIDATE_OBJECT
	CvPlot* pPlot = NULL;

	pPlot = GC.getMap().plot(m_iTacticalAIPlotX, m_iTacticalAIPlotY);

	return pPlot;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
//	--------------------------------------------------------------------------------
bool CvUnit::hasCurrentTacticalMove() const
{ 
	return ( m_eTacticalMove!=AI_TACTICAL_MOVE_NONE && m_eTacticalMove!=AI_TACTICAL_UNASSIGNED && m_iTacticalMoveSetTurn==GC.getGame().getGameTurn() );
}

void CvUnit::setHomelandMove(AIHomelandMove eMove)
{
	VALIDATE_OBJECT

#ifdef VPDEBUG
	if (hasCurrentTacticalMove())
	{
			CvString msg = CvString::format("Warning: Unit %d with current tactical move %s used for homeland move %s\n",
			GetID(), tacticalMoveNames[m_eTacticalMove], homelandMoveNames[eMove]);
			GET_PLAYER(m_eOwner).GetHomelandAI()->LogHomelandMessage(msg);
		}

	//sanity check
	if (m_eHomelandMove != AI_HOMELAND_MOVE_NONE && eMove != AI_HOMELAND_MOVE_NONE && m_eHomelandMove != eMove)
	{
		CvString msg = CvString::format("Warning, overwriting homeland move %s with %s\n", homelandMoveNames[m_eHomelandMove], homelandMoveNames[eMove] );
		OutputDebugString(msg.c_str());
	}
#endif

	//clear tactical move, can't have both ...
	m_eTacticalMove = AI_TACTICAL_MOVE_NONE;
	m_iHomelandMoveSetTurn = GC.getGame().getGameTurn();
	m_eHomelandMove = eMove;
}

//	--------------------------------------------------------------------------------
AIHomelandMove CvUnit::getHomelandMove(int* pTurnSet) const
{
	VALIDATE_OBJECT
	if (pTurnSet)
		*pTurnSet = m_iHomelandMoveSetTurn;

	return m_eHomelandMove;
}
#endif

//	--------------------------------------------------------------------------------
/// Logs information about when a worker begins and finishes construction of an improvement
void CvUnit::LogWorkerEvent(BuildTypes eBuildType, bool bStartingConstruction)
{
	VALIDATE_OBJECT
	// if not logging, don't log!
	if(!GC.getLogging() || !GC.GetBuilderAILogging())
	{
		return;
	}

	CvString strLog;
	CvString strTemp;

	strTemp.Format("%d,", GC.getGame().getGameTurn()); // turn
	strLog += strTemp;

	CvString strPlayerName;
	strPlayerName = GET_PLAYER(getOwner()).getCivilizationShortDescription();
	strLog += strPlayerName;
	strLog += ",";

	strTemp.Format("%d,", GetID()); // unit id
	strLog += strTemp;

	CvString strLoc; // location
	strLoc.Format("%d,%d,", getX(), getY());
	strLog += strLoc;

	strLog += GC.getBuildInfo(eBuildType)->GetType(); // what is being constructed
	strLog += ",";

	// beginning or completing construction
	if(bStartingConstruction)
	{
		strLog += "begin,";
	}
	else
	{
		strLog += "complete,";
	}

	// drawing improvement information
	CvString strResource = ",";
	CvString strCanSee = ",";

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuildType)->getImprovement();
	if(eImprovement != NO_IMPROVEMENT)
	{
		ResourceTypes eResource = plot()->getResourceType(getTeam());
		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo)
		{
			if (eResource != NO_RESOURCE)
			{
				strResource = pkResourceInfo->GetType();
				strResource += ",";
			}
			else if (plot()->getResourceType(NO_TEAM) != NO_RESOURCE)
			{
				eResource = plot()->getResourceType(NO_TEAM);
				strResource = pkResourceInfo->GetType();
				strResource += ",";

				strCanSee = "Can't see!,";
			}
		}
	}

	strLog += strResource;
	strLog += strCanSee;

	// gold cost
	CvString strCost = ",";
	if(bStartingConstruction)
	{
		strCost.Format("%d,", GET_PLAYER(getOwner()).getBuildCost(plot(), eBuildType));
	}
	strLog += strCost;

	// gold amount
	CvString strGoldAmount = ",";
	if(bStartingConstruction)
	{
		strGoldAmount.Format("%d,", GET_PLAYER(getOwner()).GetTreasury()->GetGold());
	}
	strLog += strGoldAmount;

	// Open the log file
	CvString strFileName;
	if(GC.getPlayerAndCityAILogSplit())
	{
		strFileName = "BuilderEventLog_" + strPlayerName + ".csv";
	}
	else
	{
		strFileName = "BuilderEventLog.csv";
	}


	// Open the log file
	FILogFile* pLog = NULL;
	pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
	pLog->Msg(strLog);
}

//	--------------------------------------------------------------------------------
/// Current power of unit (raw unit type power adjusted for health)
int CvUnit::GetPower() const
{
	VALIDATE_OBJECT
	int iPower = getUnitInfo().GetPower();

	if (getUnitInfo().GetRangedCombat() > getUnitInfo().GetCombat()) 
	{
		iPower = iPower * GetBaseRangedCombatStrength() / getUnitInfo().GetRangedCombat();
	}
	else if (getUnitInfo().GetCombat() > 0) 
	{
		iPower = iPower * GetBaseCombatStrength() / getUnitInfo().GetCombat();
	}
	
	//Take promotions into account: unit with 4 promotions worth ~50% more
	int iPowerMod = getLevel() * 125;
	iPower = (iPower * (1000 + iPowerMod)) / 1000;

	//Reduce power for damaged units
	int iDamageMod = m_iDamage * /*33*/ GD_INT_GET(WOUNDED_DAMAGE_MULTIPLIER) / 100;
	iPower -= (iPower * iDamageMod / GetMaxHitPoints());

	return iPower;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canHeal(const CvPlot* pPlot, bool bCheckMovement) const
{
	VALIDATE_OBJECT

	if (!IsHurt())
		return false;

	// No healing after movement, except for exceptions
	if (bCheckMovement && hasMoved() && !isAlwaysHeal())
		return false;

	// No barb healing
	if (isBarbarian())
		return false;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	if (MOD_BALANCE_CORE_MILITARY_RESOURCES && kPlayer.isMajorCiv())
	{
		// No healing if lacking required resources
		if (!kPlayer.HasResourceForNewUnit(getUnitType(), false, false, NO_UNIT, /*bContinue*/ true))
		{
			return false;
		}
	}

	if (MOD_UNITS_HOVERING_LAND_ONLY_HEAL)
	{
		// Hovering units can only heal over land
		if (IsHoveringUnit() && pPlot->isWater())
		{
			return false;
		}
	}

	if (MOD_NO_HEALING_ON_MOUNTAINS)
	{
		// No healing on mountains outside of cities
		if (pPlot->isMountain() && !pPlot->isCity())
		{
			return false;
		}
	}

	// Embarked Units can't heal
	if (pPlot->needsEmbarkation(this) && !isCargo())
		return false;

	// Boats can only heal in friendly territory (without promotion)
	if (getDomainType() == DOMAIN_SEA)
	{
		if (!pPlot->IsFriendlyTerritory(getOwner()) && !isHealOutsideFriendly())
		{
			return false;
		}
	}

	if (healRate(pPlot) <= 0)
		return false;

	// Can't heal in a blockaded city
	CvCity* pCity = pPlot->getPlotCity();
	return !(pCity && pCity->IsBlockadedWaterAndLand());
}


//	--------------------------------------------------------------------------------
bool CvUnit::canSentry(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(isHuman() && !IsFortified())  // we aren't fortified
	{
		if (!canEndTurnAtPlot(pPlot))
		{
			return false;
		}
	}

	if(!IsCanDefend() && !IsCanAttack())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::healRate(const CvPlot* pPlot) const
{
	if (isDelayedDeath())
		return 0;
	if (!pPlot)
		pPlot = plot();

	const IDInfo* pUnitNode = NULL;
	CvCity* pCity = pPlot->getPlotCity();

	CvCity* pClosestCity = NULL;
	const CvUnit* pLoopUnit = NULL;
	CvPlot* pLoopPlot = NULL;

	int iExtraHeal = 0;
	int iExtraFriendlyHeal = getExtraFriendlyHeal();
	int iExtraNeutralHeal = getExtraNeutralHeal();
	int iExtraEnemyHeal = getExtraEnemyHeal();

	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
	{
		iExtraFriendlyHeal += GetHealFriendlyTerritoryFromNearbyUnit();
		iExtraNeutralHeal += GetHealNeutralTerritoryFromNearbyUnit();
		iExtraEnemyHeal += GetHealEnemyTerritoryFromNearbyUnit();
	}

#if defined(MOD_BALANCE_CORE_BELIEFS)
	int iReligionMod = 0;
	if(MOD_BALANCE_CORE_BELIEFS)
	{
		if(GET_PLAYER(getOwner()).getCapitalCity() != NULL && (plot()->getOwner() == getOwner()))
		{
			ReligionTypes eMajority = GET_PLAYER(getOwner()).GetReligions()->GetStateReligion();
			BeliefTypes eSecondaryPantheon = NO_BELIEF;

			if(eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
				if(pReligion)
				{
					CvCity* pHolyCity = pReligion->GetHolyCity();
					iReligionMod = pReligion->m_Beliefs.GetFriendlyHealChange(getOwner(), pHolyCity);
					eSecondaryPantheon = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange();
					}
					iExtraFriendlyHeal += iReligionMod;
				}
			}

			// Mod for civs keeping their pantheon belief forever
			if (MOD_RELIGION_PERMANENT_PANTHEON)
			{
				if (GC.getGame().GetGameReligions()->HasCreatedPantheon(getOwner()))
				{
					const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, getOwner());
					BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(getOwner());
					if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
					{
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
						if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, getOwner()))) // check that the our religion does not have our belief, to prevent double counting
						{
							iExtraFriendlyHeal += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetFriendlyHealChange();
						}
					}
				}
			}
		}
	}
	else
	{
#endif
	// Heal from religion
	int iReligionMod = 0;
	if(!pCity)
	{
		pClosestCity = pPlot->GetAdjacentFriendlyCity(getTeam());
	}
	else
	{
		pClosestCity = pCity;
	}
	if(pClosestCity && pClosestCity->getOwner() == getOwner())
	{
		ReligionTypes eMajority = pClosestCity->GetCityReligions()->GetReligiousMajority();
		BeliefTypes eSecondaryPantheon = NO_BELIEF;
		if(eMajority != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
			if(pReligion)
			{
				iReligionMod = pReligion->m_Beliefs.GetFriendlyHealChange(getOwner());
				eSecondaryPantheon = pClosestCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iReligionMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetFriendlyHealChange();
				}
				iExtraFriendlyHeal += iReligionMod;
			}
		}

		// Mod for civs keeping their pantheon belief forever
		if (MOD_RELIGION_PERMANENT_PANTHEON)
		{
			if (GC.getGame().GetGameReligions()->HasCreatedPantheon(getOwner()))
			{
				const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, getOwner());
				BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(getOwner());
				if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
					if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, eMajority, getOwner()))) // check that the our religion does not have our belief, to prevent double counting
					{
						iExtraFriendlyHeal += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetFriendlyHealChange();
					}
				}
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	}
#endif
	// Heal from units
	int iBestHealFromUnits = 0;
	pUnitNode = pPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
		{
			int iHeal = pLoopUnit->getSameTileHeal();

			if(iHeal > iBestHealFromUnits)
			{
				iBestHealFromUnits = iHeal;
			}
		}
	}
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit && pLoopUnit->getTeam() == getTeam() && pLoopUnit->getDomainType() == getDomainType())
				{
					int iHeal = pLoopUnit->getAdjacentTileHeal();

					if(iHeal > iBestHealFromUnits)
					{
						iBestHealFromUnits = iHeal;
					}
				}
			}
		}
	}
	iExtraHeal += iBestHealFromUnits;

	// Heal from territory ownership (friendly, enemy, etc.)
	int iBaseHeal = 0;
	if (pPlot->isCity())
	{
		if (!MOD_BALANCE_VP || !pCity->IsResistance() || pCity->IsRazing())
		{
			iBaseHeal = /*25 in CP, 20 in VP*/ GD_INT_GET(CITY_HEAL_RATE);
			iExtraHeal += (pPlot->getTeam()==getTeam()) ? iExtraFriendlyHeal : iExtraNeutralHeal;
			if (pCity)
			{
				iExtraHeal += pCity->getHealRate();
			}
		}
		else
		{
			iBaseHeal = /*5*/ GD_INT_GET(ENEMY_HEAL_RATE);
			iExtraHeal += (pPlot->getTeam()==getTeam()) ? iExtraFriendlyHeal : iExtraNeutralHeal;
		}
	}
	else
	{
		if (pPlot->IsFriendlyTerritory(getOwner()))
		{
			CvCity* pOwningCity = pPlot->getOwningCity();
			if (!MOD_BALANCE_VP || !pOwningCity)
			{
				iBaseHeal = /*20 in CP, 15 in VP*/ GD_INT_GET(FRIENDLY_HEAL_RATE);
			}
			else if (pOwningCity->IsRazing())
			{
				iBaseHeal = /*20*/ GD_INT_GET(CITY_HEAL_RATE);
			}
			else if (pOwningCity->IsResistance())
			{
				iBaseHeal = /*5*/ GD_INT_GET(ENEMY_HEAL_RATE);
			}
			else
			{
				iBaseHeal = /*20 in CP, 15 in VP*/ GD_INT_GET(FRIENDLY_HEAL_RATE);
			}
			iExtraHeal += iExtraFriendlyHeal;
		}
		else
		{
			if (isEnemy(pPlot->getTeam(), pPlot))
			{
				iBaseHeal = /*10 in CP, 5 in VP*/ GD_INT_GET(ENEMY_HEAL_RATE);
				iExtraHeal += iExtraEnemyHeal;
			}
			else
			{
				iBaseHeal = /*10*/ GD_INT_GET(NEUTRAL_HEAL_RATE);
				iExtraHeal += iExtraNeutralHeal;
			}
		}
	}

	if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
		for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
		{
			ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->getMonopolyHealBonus() > 0)
			{
				iExtraHeal += pInfo->getMonopolyHealBonus();
			}
		}
	}

	// Base healing rate mod
	int iBaseHealMod = GET_PLAYER(getOwner()).getUnitBaseHealModifier();
	if(iBaseHealMod != 0)
	{
		iBaseHeal = ((100 + iBaseHealMod) / 100) * iBaseHeal;
	}
	CvAssertMsg(iBaseHeal >= 0, "Base healing rate not expected to be negative!");

	int iTotalHeal = iBaseHeal + iExtraHeal;
#if defined(MOD_BALANCE_CORE)
	TerrainTypes eTerrain = plot()->getTerrainType();
	if(eTerrain != NO_TERRAIN)
	{
		if(isTerrainDoubleHeal(eTerrain))
		{
			iTotalHeal *= 2;
		}
	}
	FeatureTypes eFeature = plot()->getFeatureType();
	if(eFeature != NO_FEATURE)
	{
		if(isFeatureDoubleHeal(eFeature))
		{
			iTotalHeal *= 2;
		}
	}
#endif
	return iTotalHeal;
}


//	--------------------------------------------------------------------------------
int CvUnit::healTurns(const CvPlot* pPlot) const
{
	if(!IsHurt())
	{
		return 0;
	}

	int iHeal = healRate(pPlot);

	if(iHeal > 0)
	{
		int iTurns = (getDamage() / iHeal);

		if((getDamage() % iHeal) != 0)
		{
			iTurns++;
		}

		return iTurns;
	}
	else
	{
		return INT_MAX;
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::doHeal()
{
	VALIDATE_OBJECT
	if (isBarbarian())
	{
		if (IsCombatUnit())
		{
			if (IsHurt() && !hasMoved() && getDomainType()==DOMAIN_LAND && isNativeDomain(plot()) && (plot()->isBarbarian() || !plot()->isOwned()))
			{
				//barbarians consider all territory as neutral
				int iHealRate = getExtraNeutralHeal() + /*0 in CP, 10 in VP*/ GD_INT_GET(BALANCE_BARBARIAN_HEAL_RATE);
				changeDamage(-iHealRate);
			}
		}
	}
	else if (canHeal(plot())) //normal player units
	{
		int iHealRate = healRate(plot());
		if (iHealRate==0)
			return;

		changeDamage( -iHealRate );

#if defined(MOD_BALANCE_CORE_BELIEFS)
		if(GET_PLAYER(getOwner()).getCapitalCity() != NULL && (plot()->getOwner() == getOwner()) && (plot()->getTurnDamage(false, false, true, true) == 0))
		{
			int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
			if (iEra < 1)
			{
				iEra = 1;
			}
			ReligionTypes eMajority = GET_PLAYER(getOwner()).GetReligions()->GetStateReligion();
			if (eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
				if (pReligion)
				{
					CvCity* pHolyCity = pReligion->GetHolyCity();
					if (pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH, getOwner(), pHolyCity) > 0)
					{
						GET_PLAYER(getOwner()).ChangeFaith(pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH, getOwner(), pHolyCity) * iEra);
						if(getOwner() == GC.getGame().getActivePlayer())
						{
							char text[256] = {0};
							sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", pReligion->m_Beliefs.GetYieldPerHeal(YIELD_FAITH, getOwner(), pHolyCity)* iEra);
							SHOW_PLOT_POPUP(plot(),getOwner(), text);
						}
					}
					BeliefTypes eSecondaryPantheon = GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						if(GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldPerHeal(YIELD_FAITH) > 0)
						{
							GET_PLAYER(getOwner()).ChangeFaith(GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldPerHeal(YIELD_FAITH) * iEra);
							if(getOwner() == GC.getGame().getActivePlayer())
							{
								char text[256] = {0};

								sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetYieldPerHeal(YIELD_FAITH) * iEra);
								SHOW_PLOT_POPUP(plot(),getOwner(), text);
							}
						}
					}
				}
			}

			// Mod for civs keeping their pantheon belief forever
			if (MOD_RELIGION_PERMANENT_PANTHEON)
			{
				if (GC.getGame().GetGameReligions()->HasCreatedPantheon(getOwner()))
				{
					const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, getOwner());
					BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(getOwner());
					if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != GET_PLAYER(getOwner()).getCapitalCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief())
					{
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, getOwner());
						if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, pReligion->m_eReligion, getOwner()))) // check that the our religion does not have our belief, to prevent double counting
						{
							GET_PLAYER(getOwner()).ChangeFaith(GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetYieldPerHeal(YIELD_FAITH) * iEra);
							if (getOwner() == GC.getGame().getActivePlayer())
							{
								char text[256] = { 0 };

								sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetYieldPerHeal(YIELD_FAITH) * iEra);
								SHOW_PLOT_POPUP(plot(), getOwner(), text);
							}
						}
					}
				}
			}
		}
#endif
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::DoAttrition()
{
	if (isTrade())
		return;

	CvPlot* pPlot = plot();
	CvString strAppendText;

	if (!pPlot->IsFriendlyTerritory(getOwner()))
	{
		if (MOD_ATTRITION && !isBarbarian() && !IsCivilianUnit() && isEnemy(pPlot->getTeam(), pPlot) && (GC.getGame().getGameTurn() - getLastMoveTurn() < 2) && !isHasPromotion((PromotionTypes)GC.getInfoTypeForString("PROMOTION_ATTRITION_IMMUNITY", true)))
		{
			strAppendText = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
			changeDamage(5, NO_PLAYER, 0.0, &strAppendText);
		}

		if (isEnemy(pPlot->getTeam(), pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() > 0)
		{
			if (GC.getGame().randRangeExclusive(0, 100, CvSeeder::fromRaw(0x1396b499).mix(pPlot->GetPseudoRandomSeed())) < getEnemyDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getEnemyDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
		else if (isEnemy(pPlot->getTeam(), pPlot) && getEnemyDamageChance() > 0 && getEnemyDamage() < 0)
		{
			if (GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0xbd4dbf3a).mix(pPlot->GetPseudoRandomSeed())) <= getEnemyDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_ENEMY_UNITS_DEFECT");
				changeDamage(getEnemyDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
		else if (getNeutralDamageChance() > 0 && getNeutralDamage() > 0)
		{
			if (GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x2b952129).mix(pPlot->GetPseudoRandomSeed())) <= getNeutralDamageChance())
			{
				strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(getNeutralDamage(), NO_PLAYER, 0.0, &strAppendText);
			}
		}
	}

	int iDamage = plot()->getTurnDamage(ignoreTerrainDamage(), ignoreFeatureDamage(), extraTerrainDamage(), extraFeatureDamage());

	// No damage for cargo or naval units
	if (!isCargo() && getDomainType() != DOMAIN_SEA)
	{
		if (iDamage != 0)
		{
			if (iDamage > 0)
			{
				// CUSTOMLOG("Applying terrain/feature damage (of %i) for player/unit %i/%i at (%i, %i)", iDamage, getOwner(), GetID(), plot()->getX(), plot()->getY());
				CvString strAppendText =  GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION");
				changeDamage(iDamage, NO_PLAYER, 0.0, &strAppendText);
			}
			else
			{
				// CUSTOMLOG("Applying terrain/feature healing (of %i) for player/unit %i/%i at (%i, %i)", iDamage, getOwner(), GetID(), plot()->getX(), plot()->getY());
				changeDamage(iDamage, NO_PLAYER);
			}
		}
	}

	if (getDamage() >= GetMaxHitPoints())
	{
		CvString strBuffer;
		CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
		if (pNotification)
		{
			strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
			Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
			pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
		}
	}

	TeamTypes eOwnerTeam = pPlot->getTeam();

	if (eOwnerTeam != NO_TEAM)
	{
		CvTeam &kTeam = GET_TEAM(eOwnerTeam);
		if (!kTeam.isMinorCiv() && eOwnerTeam != getTeam() && !kTeam.IsAllowsOpenBordersToTeam(getTeam()))
		{
			int iReligiousStrengthLoss = GetReligiousStrengthLossRivalTerritory();
			if (iReligiousStrengthLoss > 0)
			{
				int iStrength = GetReligionData()->GetReligiousStrength();
				int iStrengthLoss = (getUnitInfo().GetReligiousStrength() * iReligiousStrengthLoss) / 100;

				if ((iStrength - iStrengthLoss) <= 0)
				{
					CvString strBuffer;
					CvNotifications* pNotification = GET_PLAYER(getOwner()).GetNotifications();
					if (pNotification)
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_ATTRITION", getNameKey());
						Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_LOST");
						pNotification->Add(NOTIFICATION_UNIT_DIED, strBuffer, strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
					}

					kill(false);
				}
				else
				{
					GetReligionDataMutable()->SetReligiousStrength(iStrength - iStrengthLoss);
					if (pPlot && pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
					{
						Localization::String string = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_ATTRITION").c_str();

						char text[256];
						sprintf_s (text, "%s [COLOR_WHITE]-%d [ICON_PEACE][ENDCOLOR]", string.toUTF8(), iStrengthLoss);
						SHOW_PLOT_POPUP(plot(), getOwner(), text);
					}
				}
			}
		}
	}
}
//	--------------------------------------------------------------------------------
int CvUnit::GetDanger(const CvPlot* pAtPlot) const
{
	if (!pAtPlot)
		pAtPlot = plot();

	return GET_PLAYER( getOwner() ).GetPlotDanger(*pAtPlot,this,UnitIdContainer(),0);
}

int CvUnit::GetDanger(const CvPlot* pAtPlot, const UnitIdContainer& unitsToIgnore, int iExtraDamage) const
{
	if (!pAtPlot)
		pAtPlot = plot();

	return GET_PLAYER( getOwner() ).GetPlotDanger(*pAtPlot,this,unitsToIgnore,iExtraDamage);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirlift(const CvPlot* pPlot) const
{
	return (getAirliftFromPlot(pPlot) != NULL);
}

const CvPlot* CvUnit::getAirliftToPlot(const CvPlot* pPlot, bool bIncludeCities) const
{
	if (MOD_EVENTS_AIRLIFT)
	{
		if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanAirliftTo, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE)
		{
			return pPlot;
		}
	}

	// Is there a friendly improvement that AllowsAirliftTo
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry *pkEntry = GC.getImprovementInfo(eImprovement);
		if (pkEntry && pkEntry->IsAllowsAirliftTo() && !pPlot->IsImprovementPillaged() && pPlot->IsFriendlyTerritory(getOwner()) && !pPlot->isVisibleEnemyUnit(getOwner()))
		{
			return pPlot;
		}
	}

	if (!bIncludeCities)
		return NULL;

	CvCity* pEndCity = pPlot->getPlotCity();
	if (pEndCity == NULL)
	{
		pEndCity = pPlot->GetAdjacentCity();
		if (pEndCity == NULL)
		{
			return NULL;
		}
	}

	if (!pEndCity->CanAirlift())
	{
		return NULL;
	}
	if (pEndCity->getOwner() == getOwner() || (GET_PLAYER(pEndCity->getOwner()).isMinorCiv() && GET_PLAYER(pEndCity->getOwner()).GetMinorCivAI()->GetAlly() == getOwner()))
	{
		return pPlot;
	}
	return NULL;
}

const CvPlot* CvUnit::getAirliftFromPlot(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT

	// Early out if we're a trade unit
	if (isTrade())
	{
		return NULL;
	}

	if (MOD_EVENTS_AIRLIFT)
	{
		if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanAirliftFrom, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE)
		{
			return pPlot;
		}
	}

	if(getDomainType() != DOMAIN_LAND)
	{
		return NULL;
	}

	if(hasMoved())
	{
		return NULL;
	}

	// Are we stood on a friendly improvement that AllowsAirliftFrom
	ImprovementTypes eImprovement = pPlot->getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		CvImprovementEntry *pkEntry = GC.getImprovementInfo(eImprovement);
		if (pkEntry && pkEntry->IsAllowsAirliftFrom() && !pPlot->IsImprovementPillaged() && pPlot->IsFriendlyTerritory(getOwner()) && !pPlot->isVisibleEnemyUnit(getOwner()))
		{
			return pPlot;
		}
	}

	const CvPlot* pFromPlot = pPlot;

	if (pPlot->isWater())
	{
		return NULL;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return NULL;
		}
		else
		{
			pFromPlot = pCity->plot();
		}
	}

	if(pCity->getTeam() != getTeam())
	{
		return NULL;
	}

	if (!pCity->CanAirlift())
	{
		return NULL;
	}

	return pFromPlot;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAirliftAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	const CvPlot* pFromPlot = getAirliftFromPlot(pPlot);
	
	if(pFromPlot == NULL)
		return false;

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	int iMoveFlags = CvUnit::MOVEFLAG_DESTINATION;
	if(!pTargetPlot || !canMoveInto(*pTargetPlot, iMoveFlags) || pTargetPlot->isWater())
	{
		return false;
	}

	const CvPlot* pToPlot = getAirliftToPlot(pTargetPlot, pFromPlot->isCity());
	if(pToPlot == NULL)
		return false;

	if (pToPlot == pFromPlot)
		return false;

	// No enemy units adjacent
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(iX, iY, ((DirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			CvUnit* pDefender = pAdjacentPlot->getBestDefender(NO_PLAYER, getOwner(), NULL, true);
			if (pDefender)
			{
				return false;
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::airlift(int iX, int iY)
{
	VALIDATE_OBJECT
	CvPlot* pTargetPlot = NULL;

	if(!canAirliftAt(plot(), iX, iY))
	{
		return false;
	}

	pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);

	if (MOD_API_ACHIEVEMENTS)
	{
		//Here's Looking at You, Kid
		CvPlayerAI& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
		if (pTargetPlot != NULL && getOwner() == kActivePlayer.GetID() && kActivePlayer.isHuman())
		{
			//Easy checks out of the way, now for the ugly ones.

			//Current City Casablanca?
			CvPlot* pCurrentPlot = plot();
			if (pCurrentPlot != NULL)
			{
				CvCity* pCurrentCity = pCurrentPlot->getPlotCity();
				CvCity* pTargetCity = pTargetPlot->getPlotCity();
				if (pCurrentCity != NULL && pTargetCity != NULL &&
					strcmp(pCurrentCity->getNameKey(), "TXT_KEY_CITY_NAME_CASABLANCA") == 0 &&
					pTargetCity->IsOriginalCapital())
				{
					const PlayerTypes eOriginalOwner = pTargetCity->getOriginalOwner();
					CvPlayerAI& kOriginalPlayer = GET_PLAYER(eOriginalOwner);
					if (strcmp(kOriginalPlayer.getCivilizationTypeKey(), "CIVILIZATION_PORTUGAL") == 0)
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP2_22);
					}
				}
			}
		}
	}


	if(pTargetPlot != NULL)
	{
		finishMoves();
		setXY(pTargetPlot->getX(), pTargetPlot->getY());
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	CvPlot* pLoopPlot = NULL;
	int iDX = 0;
	int iDY = 0;

	if(!(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if(eTeam == getTeam())
	{
		return false;
	}

	int iBlastRadius = /*2*/ range(GD_INT_GET(NUKE_BLAST_RADIUS), 1, 5);

	for(iDX = -(iBlastRadius); iDX <= iBlastRadius; iDX++)
	{
		for(iDY = -(iBlastRadius); iDY <= iBlastRadius; iDY++)
		{
			pLoopPlot	= plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iBlastRadius);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getTeam() == eTeam)
				{
					return true;
				}

				if(pLoopPlot->plotCheck(PUF_isCombatTeam, eTeam, getTeam()) != NULL)
				{
					return true;
				}
			}
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canNuke() const
{
	VALIDATE_OBJECT
	return GetNukeDamageLevel() > 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canNukeAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT

	if(!canNuke())
	{
		return false;
	}

	int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), iX, iY);

	if(iDistance <= 0)
		return false;	// Don't let the user nuke the same plot they are on.

	int iRange  = GetRange();
	if(iRange > 0 && iDistance > iRange)  // Range 0 can hit anywhere
	{
		return false;
	}

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if(GET_TEAM((TeamTypes)iI).isAlive())
		{
			CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
			if(pTargetPlot)
			{
				TeamTypes eTheirTeam = (TeamTypes)iI;
				if(isNukeVictim(pTargetPlot, eTheirTeam))
				{
					CvTeam& myTeam = GET_TEAM(getTeam());
					if(!myTeam.isAtWar(eTheirTeam) && !myTeam.canDeclareWar(eTheirTeam, getOwner()))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canParadrop(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	VALIDATE_OBJECT
	if(getDropRange() <= 0)
	{
		return false;
	}

	// Things we check when we want to know if the unit can actually drop RIGHT NOW
	if(!bOnlyTestVisibility)
	{
		if(hasMoved())
		{
			return false;
		}

		if(isEmbarked())
		{
			return false;
		}

		if(pPlot->IsFriendlyTerritory(getOwner()))
		{
			// We're in friendly territory, call the event to see if we CAN'T start from here anyway
#if defined(MOD_EVENTS_PARADROPS)
			if (MOD_EVENTS_PARADROPS) {
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CannotParadropFrom, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE) {
					return false;
				}
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAll(pkScriptSystem, "CannotParadropFrom", args.get(), bResult))
				{
					if (bResult)
					{
						return false;
					}
				}
			}
#if defined(MOD_EVENTS_PARADROPS)
			}
#endif
		}
		else
		{
			// We're not in friendly territory, call the event to see if we CAN start from here anyway
#if defined(MOD_EVENTS_PARADROPS)
			if (MOD_EVENTS_PARADROPS) {
				if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanParadropFrom, getOwner(), GetID(), pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_TRUE) {
					return true;
				}
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) {
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pPlot->getX());
				args->Push(pPlot->getY());

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanParadropFrom", args.get(), bResult))
				{
					if (bResult)
					{
						return true;
					}
				}
			}
#if defined(MOD_EVENTS_PARADROPS)
			}
#endif

			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canParadropAt(const CvPlot* pPlot, int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canParadrop(pPlot, false))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	if(NULL == pTargetPlot || pTargetPlot == pPlot)
	{
		return false;
	}

	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	if(!canMoveInto(*pTargetPlot, MOVEFLAG_DESTINATION | MOVEFLAG_NO_EMBARK))
	{
		return false;
	}

	if(plotDistance(pPlot->getX(), pPlot->getY(), iX, iY) > getDropRange())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::paradrop(int iX, int iY)
{
	VALIDATE_OBJECT
	if(!canParadropAt(plot(), iX, iY))
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if(! pPlot)
	{
		return false;
	}

	CvPlot* fromPlot = plot();
	//JON: CHECK FOR INTERCEPTION HERE
#if defined(MOD_GLOBAL_PARATROOPS_AA_DAMAGE)
	if (MOD_GLOBAL_PARATROOPS_AA_DAMAGE) {
		if (CvUnitCombat::ParadropIntercept(*this, *pPlot)) {
			// Unit died during the drop
			return false;
		}
	}
#endif

	//play paradrop animation
	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitParadrop(pDllUnit.get());
	}

	//more first, update movement points later
	setXY(pPlot->getX(), pPlot->getY(), true, true, false);
	setMoves(GD_INT_GET(MOVE_DENOMINATOR)); //keep one move
	setMadeAttack(true);

#if defined(MOD_EVENTS_PARADROPS)
	if (MOD_EVENTS_PARADROPS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_ParadropAt, getOwner(), GetID(), fromPlot->getX(), fromPlot->getY(), pPlot->getX(), pPlot->getY());
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(((int)getOwner()));
		args->Push(GetID());
		args->Push(fromPlot->getX());
		args->Push(fromPlot->getY());
		args->Push(pPlot->getX());
		args->Push(pPlot->getY());

		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "ParadropAt", args.get(), bResult);
	}
#if defined(MOD_EVENTS_PARADROPS)
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMakeTradeRoute(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if (!isTrade())
	{
		return false;
	}

	if (isDelayedDeath())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMakeTradeRouteAt(const CvPlot* pPlot, int iX, int iY, TradeConnectionType eConnectionType) const
{
	if (!canMakeTradeRoute(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	// you must launch a trade mission from one of your own cities
	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	if (!GET_PLAYER(getOwner()).GetTrade()->CanCreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType, false))
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::makeTradeRoute(int iX, int iY, TradeConnectionType eConnectionType)
{
	if (!canMakeTradeRouteAt(plot(), iX, iY, eConnectionType))
	{
		return false;
	}

	CvCity* pFromCity = NULL;
	CvCity* pToCity = NULL;

	if (plot())
	{
		pFromCity = plot()->getPlotCity();
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);
	if (pToPlot)
	{
		pToCity = pToPlot->getPlotCity();
	}

	bool bResult = GET_PLAYER(getOwner()).GetTrade()->CreateTradeRoute(pFromCity, pToCity, getDomainType(), eConnectionType);

	if (bResult)
		kill(true);

	return bResult;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeTradeUnitHomeCity(const CvPlot* pPlot) const
{
	if (!isTrade())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeTradeUnitHomeCityAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeTradeUnitHomeCity(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	// you must launch a trade mission from one of your own cities
	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	// can't change cities with a city you don't own
	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
		// The path finder permits routes between cities on lakes,
		// so we'd better allow cargo ships to be relocated there!
		if (!pToCity->isCoastal(0))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::changeTradeUnitHomeCity(int iX, int iY)
{
	if (!canChangeTradeUnitHomeCityAt(plot(), iX, iY))
	{
		return false;
	}

	setXY(iX, iY);
	finishMoves();
	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeAdmiralPort(const CvPlot* pPlot) const
{
	bool bHasSkill = !MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && IsGreatAdmiral();
	bHasSkill = bHasSkill || (MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && m_pUnitInfo->IsCanChangePort());
	if (!bHasSkill)
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pCity->isCoastal())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canChangeAdmiralPortAt(const CvPlot* pPlot, int iX, int iY) const
{
	if (!canChangeAdmiralPort(pPlot))
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pFromCity = pPlot->getPlotCity();
	if (pFromCity == NULL)
	{
		return false;
	}

	if (pFromCity->getOwner() != getOwner())
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iX, iY);

	// Null plot...
	if(pToPlot == NULL)
	{
		return false;
	}

	// Same plot...
	if(pToPlot == pPlot)
	{
		return false;
	}

	CvCity* pToCity = pToPlot->getPlotCity();
	if (pToCity == NULL)
	{
		return false;
	}

	// can't change cities with a city you don't own
	if (pToCity->getOwner() != getOwner())
	{
		return false;
	}

	if (!pToCity->isCoastal())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::changeAdmiralPort(int iX, int iY)
{
	if (!canChangeAdmiralPortAt(plot(), iX, iY))
	{
		return false;
	}
	CvCity* pkPrevGarrisonedCity = GetGarrisonedCity();
	// update garrison status for old and new city, if applicable
	if (pkPrevGarrisonedCity)
	{
		//when moving out, another unit might be present to take over garrison duty
		pkPrevGarrisonedCity->SetGarrison(pkPrevGarrisonedCity->plot()->getBestGarrison(pkPrevGarrisonedCity->getOwner()));
		CvInterfacePtr<ICvCity1> pkDllCity(new CvDllCity(pkPrevGarrisonedCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
		pkPrevGarrisonedCity->updateYield();
	}

	setXY(iX, iY);
	finishMoves();

	CvCity* pkNewGarrisonedCity = GetGarrisonedCity();
	// update garrison status for old and new city, if applicable
	if (pkNewGarrisonedCity)
	{
		//when moving out, another unit might be present to take over garrison duty
		pkNewGarrisonedCity->SetGarrison(pkNewGarrisonedCity->plot()->getBestGarrison(pkNewGarrisonedCity->getOwner()));
		CvInterfacePtr<ICvCity1> pkDllCity(new CvDllCity(pkNewGarrisonedCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
		pkNewGarrisonedCity->updateYield();
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canPlunderTradeRoute(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	if (!IsCombatUnit())
	{
		return false;
	}

	if (pPlot == NULL)
	{
		return false;
	}

	if (isEmbarked())
	{
		return false;
	}
	
#if defined(MOD_GLOBAL_NO_OCEAN_PLUNDERING)
	if (MOD_GLOBAL_NO_OCEAN_PLUNDERING && pPlot->isWater() && !pPlot->isShallowWater())
	{
		return false;
	}
#endif

	if (GET_PLAYER(m_eOwner).GetTrade()->ContainsOpposingPlayerTradeUnit(pPlot))
	{
#if defined(MOD_BALANCE_CORE)
		std::vector<int> aiTradeUnitsAtPlot;
		aiTradeUnitsAtPlot = GET_PLAYER(m_eOwner).GetTrade()->GetOpposingTradeUnitsAtPlot(pPlot, true);

		for (uint uiTradeRoute = 0; uiTradeRoute < aiTradeUnitsAtPlot.size(); uiTradeRoute++)
		{
			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[uiTradeRoute]);
			if (eTradeUnitOwner == NO_PLAYER)
			{
				// invalid TradeUnit
				continue;
			}

			CorporationTypes eCorporation = GET_PLAYER(eTradeUnitOwner).GetCorporations()->GetFoundedCorporation();
			if (eCorporation != NO_CORPORATION)
			{
				CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
				if (pkCorporation && pkCorporation->IsTradeRoutesInvulnerable())
				{
					return false;
				}
			}

			TeamTypes eTeam = GET_PLAYER(eTradeUnitOwner).getTeam();
			if (!GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam) && !GET_PLAYER(m_eOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
				continue;

			if (GET_PLAYER(m_eOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
			{
				PlayerTypes eTradeUnitDest = GC.getGame().GetGameTrade()->GetDestFromID(aiTradeUnitsAtPlot[uiTradeRoute]);
				if (eTradeUnitDest == m_eOwner && !GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam))
				{
					return false;
				}
			}
		}
#endif
		if (!bOnlyTestVisibility)
		{
			if (aiTradeUnitsAtPlot.size() <= 0)
			{
				return false;
			}

			PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[0]);
			if (eTradeUnitOwner == NO_PLAYER)
			{
				// invalid TradeUnit
				return false;
			}


			CorporationTypes eCorporation = GET_PLAYER(eTradeUnitOwner).GetCorporations()->GetFoundedCorporation();
			if (eCorporation != NO_CORPORATION)
			{
				CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
				if (pkCorporation && pkCorporation->IsTradeRoutesInvulnerable())
				{
					return false;
				}
			}

			TeamTypes eTeam = GET_PLAYER(eTradeUnitOwner).getTeam();
			if (!GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam) && !GET_PLAYER(m_eOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::plunderTradeRoute()
{
	CvPlot* pPlot = plot();
	if (!canPlunderTradeRoute(pPlot, false))
	{
		return false;
	}

	std::vector<int> aiTradeUnitsAtPlot;
	CvPlayerTrade* pTrade = GET_PLAYER(m_eOwner).GetTrade();
	aiTradeUnitsAtPlot = pTrade->GetOpposingTradeUnitsAtPlot(pPlot, false);
	CvAssertMsg(aiTradeUnitsAtPlot.size() > 0, "aiTradeUnitsAtPlot is empty. Earlier check should have verified that this can't be the case");
	if (aiTradeUnitsAtPlot.size() <= 0)
	{
		return false;
	}

	// right now, plunder the first unit
	//No!
	bool bGood = false;
	for (uint uiTradeRoute = 0; uiTradeRoute < aiTradeUnitsAtPlot.size(); uiTradeRoute++)
	{
		PlayerTypes eTradeUnitOwner = GC.getGame().GetGameTrade()->GetOwnerFromID(aiTradeUnitsAtPlot[uiTradeRoute]);
		if (eTradeUnitOwner == NO_PLAYER)
		{
			// invalid TradeUnit
			continue;
		}
		CorporationTypes eCorporation = GET_PLAYER(eTradeUnitOwner).GetCorporations()->GetFoundedCorporation();
		if (eCorporation != NO_CORPORATION)
		{
			CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
			if (pkCorporation && pkCorporation->IsTradeRoutesInvulnerable())
			{
				continue;
			}
		}
		TeamTypes eTeam = GET_PLAYER(eTradeUnitOwner).getTeam();
		if (GET_TEAM(GET_PLAYER(m_eOwner).getTeam()).isAtWar(eTeam) || GET_PLAYER(m_eOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
		{
			pTrade->PlunderTradeRoute(aiTradeUnitsAtPlot[0], this);
			bGood = true;

			if (GC.getLogging() && GC.getAILogging() && GET_PLAYER(m_eOwner).GetPlayerTraits()->IsCanPlunderWithoutWar())
			{
				CvString strLogString;
				strLogString.Format("MOROCCO!! WE ARE PIRATES!!!");
				GET_PLAYER(getOwner()).GetHomelandAI()->LogHomelandMessage(strLogString);
			}
		}
	}
	if (bGood)
	{
		return true;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canCreateGreatWork(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	// Unit class has this flag?
	if(GetGreatWork() == NO_GREAT_WORK)
	{
		return false;
	}

	const TeamTypes eTeam = getTeam();
	if(GET_TEAM(eTeam).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	//If we're only testing whether this unit has the capabilty to ever perform the action, return true.
	if (bOnlyTestVisibility)
	{
		return true;
	}

	// In or adjacent to a city?
	CvCity* pCity = NULL;
	if (pPlot == NULL)
	{
		return false;
	}
	pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if (pCity == NULL)
		{
			return false;
		}
	}

	// ... is this a friendly city?
	if (eTeam != pCity->getTeam())
	{
		return false;
	}

	// Is there a slot of the right type somewhere in the empire?
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(GetGreatWork());
	CvPlayer &kPlayer = GET_PLAYER(getOwner());
	return kPlayer.GetCulture()->GetNumAvailableGreatWorkSlots(eGreatWorkSlot) > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::createGreatWork()
{
	CvPlot* pPlot = plot();
	if (!canCreateGreatWork(pPlot, false))
	{
		return false;
	}

	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
	if(pCulture == NULL)
	{
		CvAssertMsg(pCulture != NULL, "This should never happen.");
		return false;
	}
	
	CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
	BuildingClassTypes eBuildingClass = NO_BUILDINGCLASS; // Passed by reference below
	int iSlot = -1; // Passed by reference below
	GreatWorkType eGreatWorkType = GetGreatWork();
	GreatWorkClass eClass = CultureHelpers::GetGreatWorkClass(eGreatWorkType);
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eGreatWorkType);
	
	CvCity *pCity = kPlayer.GetCulture()->GetClosestAvailableGreatWorkSlot(getX(), getY(), eGreatWorkSlot, eBuildingClass, iSlot);
	if (pCity)
	{
		int iGWindex = pCulture->CreateGreatWork(eGreatWorkType, eClass, m_eOwner, kPlayer.GetCurrentEra(), getName());

#if defined(MOD_BALANCE_CORE)
		if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				CvCity* pLoopCity = NULL;
				int iCityLoop = 0;

				// Loop through owner's cities.
				for (pLoopCity = GET_PLAYER(getOwner()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iCityLoop))
				{
					if (pLoopCity != NULL)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
					}
				}
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA_GREAT_WORK");
					strText << iWLTKD << /*25*/ GD_INT_GET(WLTKD_GROWTH_MULTIPLIER);
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA_GREAT_WORK");
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
			}
		}
#endif
		pCity->GetCityBuildings()->SetBuildingGreatWork(eBuildingClass, iSlot, iGWindex);

		if(pPlot->isActiveVisible())
		{
			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
		}
#if defined(MOD_BALANCE_CORE)
		pCity->UpdateAllNonPlotYields(false);
#endif
		if(IsGreatPerson())
		{
			kPlayer.DoGreatPersonExpended(getUnitType(), this);
		}

		kill(true);

		bool bDontShowRewardPopup = GC.GetEngineUserInterface()->IsOptionNoRewardPopups();
		Localization::String localizedText;

		// Notification in MP games
		if(bDontShowRewardPopup || GC.getGame().isReallyNetworkMultiPlayer())
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				localizedText = Localization::Lookup("TXT_KEY_MISC_WONDER_COMPLETED");
				localizedText << kPlayer.getNameKey() << pCulture->GetGreatWorkName(iGWindex);
				pNotifications->Add(NOTIFICATION_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, localizedText.toUTF8(), localizedText.toUTF8(), getX(), getY(), iGWindex, kPlayer.GetID());
			}
		}
		// Popup in SP games
		else
		{
			if(kPlayer.GetID() == GC.getGame().getActivePlayer())
			{
				CvPopupInfo kPopup(BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, iGWindex);
				GC.GetEngineUserInterface()->AddPopup(kPopup);
			}
		}
#if defined(MOD_BALANCE_CORE)
		CvGameCulture *pCulture = GC.getGame().GetGameCulture();
		if(pCulture != NULL)
		{
			int iValue = (int)eGreatWorkType;
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_GreatWorkCreated, getOwner(), GetID(), iValue);
		}
#endif

		return true;
	}

	return false;
}
#if defined(MOD_BALANCE_CORE)
bool CvUnit::canGetFreeLuxury() const
{
	if(plot()->getOwner() == getOwner())
	{
		if(m_pUnitInfo->GetNumFreeLux() > 0)
		{
			return true;
		}
	}
	return false;
}
bool CvUnit::createFreeLuxury()
{
	int iNumLuxuries = m_pUnitInfo->GetNumFreeLux() + GET_PLAYER(getOwner()).GetAdmiralLuxuryBonus();
	bool bResult = false;
	if(iNumLuxuries > 0)
	{
		// Loop through all resources and see if we can find this many unique ones
		ResourceTypes eResourceToGive = NO_RESOURCE;
		int iBestFlavor = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if(GC.getMap().getNumResources(eResource) <= 0)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive != NO_RESOURCE)
		{
			GET_PLAYER(getOwner()).changeResourceFromGP(eResourceToGive, iNumLuxuries);
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			if(kOwner.isHuman())
			{
				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvResourceInfo* pInfo = GC.getResourceInfo(eResourceToGive);
					if (pInfo)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_FREE_RESOURCE");
						strText << getNameKey() << pInfo->GetTextKey() << iNumLuxuries;
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_FREE_RESOURCE_SUMMARY");
						strSummary << getNameKey();
						pNotifications->Add(NOTIFICATION_DISCOVERED_LUXURY_RESOURCE, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), eResourceToGive);
					}
				}
			}
			kOwner.DoGreatPersonExpended(getUnitType(), this);
			kill(true);
			bResult = true;
		}
	}

	return bResult;
}

int CvUnit::CreateFreeLuxuryCheckCopy()
{
	int iNumLuxuries = m_pUnitInfo->GetNumFreeLux() + GET_PLAYER(getOwner()).GetAdmiralLuxuryBonus();
	return iNumLuxuries;
}

int CvUnit::CreateFreeLuxuryCheck()
{
	int iNumLuxuries = m_pUnitInfo->GetNumFreeLux() + GET_PLAYER(getOwner()).GetAdmiralLuxuryBonus();
	if(iNumLuxuries > 0)
	{
		// Loop through all resources and see if we can find this many unique ones
		ResourceTypes eResourceToGive = NO_RESOURCE;
		int iBestFlavor = 0;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
			{
				if(GC.getMap().getNumResources(eResource) <= 0)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					//If we've already got this resource, divide the value by the amount.
					if(GET_PLAYER(getOwner()).getNumResourceTotal(eResource, false) > 0)
					{
						iRandomFlavor = 0;
					}
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive == NO_RESOURCE)
		{
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;
				CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
				if (pkResource != NULL && pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					int iRandomFlavor = GC.getGame().randRangeInclusive(1, 100, CvSeeder(iResourceLoop));
					if(iRandomFlavor > iBestFlavor)
					{
						eResourceToGive = eResource;
						iBestFlavor = iRandomFlavor;
					}
				}
			}
		}
		if (eResourceToGive != NO_RESOURCE)
		{
			return eResourceToGive;
		}
	}

	return NO_RESOURCE;
}
#endif
//	--------------------------------------------------------------------------------
int CvUnit::getNumExoticGoods() const
{
	return m_iNumExoticGoods;
}

//	--------------------------------------------------------------------------------
void CvUnit::setNumExoticGoods(int iValue)
{
	CvAssert(iValue >= 0);
	m_iNumExoticGoods = iValue;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNumExoticGoods(int iChange)
{
	setNumExoticGoods(getNumExoticGoods() + iChange);
}

//	--------------------------------------------------------------------------------
float CvUnit::calculateExoticGoodsDistanceFactor(const CvPlot* pPlot)
{
	float fDistanceFactor = 0.0f;

	CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
	if (pPlot && pCapital)
	{
		int iDistanceThreshold = (GC.getMap().getGridWidth() + GC.getMap().getGridHeight()) / 2;
		fDistanceFactor = plotDistance(pPlot->getX(), pPlot->getY(), pCapital->getX(), pCapital->getY()) / (float) iDistanceThreshold;
	}

	return fDistanceFactor;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canSellExoticGoods(const CvPlot* pPlot, bool bOnlyTestVisibility) const
{
	if (pPlot == NULL)
	{
		return false;
	}

	if (getNumExoticGoods() <= 0)
	{
		return false;
	}

	if (!bOnlyTestVisibility)
	{
		int iNumValidPlots = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL)
			{
				PlayerTypes eLoopPlotOwner = pLoopPlot->getOwner();
				if (eLoopPlotOwner != getOwner() && eLoopPlotOwner != NO_PLAYER)
				{
					if (!GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
					{
						iNumValidPlots++;
						break;
					}
				}
			}
		}

		if (iNumValidPlots <= 0)
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExoticGoodsGoldAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraGold = /*400*/ GD_INT_GET(EXOTIC_GOODS_GOLD_MAX) - /*100*/ GD_INT_GET(EXOTIC_GOODS_GOLD_MIN);
		iValue = GD_INT_GET(EXOTIC_GOODS_GOLD_MIN) + (int)(iExtraGold * fDistanceFactor);
		iValue = MIN(iValue, GD_INT_GET(EXOTIC_GOODS_GOLD_MAX));
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExoticGoodsXPAmount()
{
	int iValue = 0;
	if (canSellExoticGoods(plot()))
	{
		float fDistanceFactor = calculateExoticGoodsDistanceFactor(plot());

		int iExtraXP = /*30*/ GD_INT_GET(EXOTIC_GOODS_XP_MAX) - /*10*/ GD_INT_GET(EXOTIC_GOODS_XP_MIN);
		iValue = GD_INT_GET(EXOTIC_GOODS_XP_MIN) + (int)(iExtraXP * fDistanceFactor);
		iValue = MIN(iValue, GD_INT_GET(EXOTIC_GOODS_XP_MAX));
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::sellExoticGoods()
{
	if (canSellExoticGoods(plot()))
	{
		int iXP = getExoticGoodsXPAmount();
		int iGold = getExoticGoodsGoldAmount();
		changeExperienceTimes100(iXP * 100);
		GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iGold);
		char text[256] = {0};
		sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGold);
		SHOW_PLOT_POPUP(plot(), getOwner(), text);

		changeNumExoticGoods(-1);
#if defined(MOD_BALANCE_CORE)
		PlayerTypes ePlotOwner = NO_PLAYER;
		ImprovementTypes eFeitoria = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FEITORIA");
		if (eFeitoria != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eFeitoria) != NULL && GC.getImprovementInfo(eFeitoria)->GetRequiredCivilization() == getCivilizationType())
			{
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					CvPlot* pLoopPlotSearch = plotDirection(plot()->getX(), plot()->getY(), ((DirectionTypes)iI));
					if (pLoopPlotSearch != NULL)
					{
						PlayerTypes eLoopPlotOwner = pLoopPlotSearch->getOwner();
						if (eLoopPlotOwner != NO_PLAYER)
						{
							if (!GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eLoopPlotOwner).getTeam()))
							{
								if(GET_PLAYER(eLoopPlotOwner).isMinorCiv())
								{
									ePlotOwner = eLoopPlotOwner;
									break;
								}
							}
						}
					}
				}
				if (ePlotOwner != NO_PLAYER)
				{
					bool bAlreadyHere = false;
					CvPlot* pBestPlot = NULL;
					CvCity* pCity = GET_PLAYER(ePlotOwner).getCapitalCity();
					if (pCity != NULL)
					{

						for (int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
						{

							CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iCityPlotLoop);
							if (pLoopPlot != NULL && (pLoopPlot->getOwner() == ePlotOwner) && !pLoopPlot->isCity() && !pLoopPlot->isWater() && !pLoopPlot->isImpassable(getTeam()) && !pLoopPlot->IsNaturalWonder() && pLoopPlot->isCoastalLand() && (pLoopPlot->getResourceType(getTeam()) == NO_RESOURCE))
							{
								if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
								{
									if (pLoopPlot->getImprovementType() == eFeitoria)
									{
										bAlreadyHere = true;
										break;
									}

								}
							}
						}
						if (!bAlreadyHere)
						{
							for (int iCityPlotLoop = 0; iCityPlotLoop < pCity->GetNumWorkablePlots(); iCityPlotLoop++)
							{
								CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iCityPlotLoop);
								if (pLoopPlot != NULL && (pLoopPlot->getOwner() == ePlotOwner) && !pLoopPlot->isCity() && !pLoopPlot->isWater() && !pLoopPlot->isImpassable(getTeam()) && !pLoopPlot->IsNaturalWonder() && pLoopPlot->isCoastalLand() && (pLoopPlot->getResourceType(getTeam()) == NO_RESOURCE))
								{
									//If we can build on an empty spot, do so.
									if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
									{
										pBestPlot = pLoopPlot;
										break;
									}
									//If not, let's clear a basic improvement off.
									else
									{
										CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pLoopPlot->getImprovementType());
										if (pImprovementInfo && !pImprovementInfo->IsPermanent() && !pImprovementInfo->IsCreatedByGreatPerson())
										{
											pBestPlot = pLoopPlot;
										}
									}
								}
							}
						}
						if (pBestPlot != NULL && !bAlreadyHere)
						{
							pBestPlot->setImprovementType(NO_IMPROVEMENT);
							pBestPlot->setImprovementType(eFeitoria, getOwner());
							pBestPlot->SilentlyResetAllBuildProgress();

							IDInfo* pUnitNode = NULL;
							CvUnit* pLoopUnit = NULL;
							pUnitNode = pBestPlot->headUnitNode();
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::GetPlayerUnit(*pUnitNode);
								pUnitNode = pBestPlot->nextUnitNode(pUnitNode);
								if (pLoopUnit != NULL && pLoopUnit->GetMissionAIType() == MISSIONAI_BUILD && pLoopUnit->GetMissionAIPlot() == pBestPlot)
								{
									pLoopUnit->ClearMissionQueue();
								}
							}
						}
					}
				}
			}
		}
		return true;
#endif
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRebase(bool bForced) const
{
	// Must be an air unit
	if(getDomainType() != DOMAIN_AIR)
	{
		return false;
	}

	// Must be an "immobile" air unit (e.g. no helicopters)
	if(!IsImmobile())
	{
		return false;
	}

	// Must have movement points left this turn
	if (!bForced && getMoves() <= 0)
	{
		return false;
	}

	return true;
}

int CvUnit::getRebaseRange() const
{
	if (canRebase())
		return (GetRange() * /*200 in CP, 500 in VP*/ GD_INT_GET(AIR_UNIT_REBASE_RANGE_MULTIPLIER)) / 100;

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRebaseAt(int iXDest, int iYDest, bool bForced) const
{
	// If we can't rebase ANYWHERE then we definitely can't rebase at this X,Y
	if (!canRebase(bForced))
	{
		return false;
	}

	CvPlot* pToPlot = GC.getMap().plot(iXDest, iYDest);
	if(pToPlot == NULL)
	{
		return false;
	}

	if(plotDistance(getX(), getY(), iXDest, iYDest) > getRebaseRange())
	{
		return false;
	}

	// Rebase to a City?
	bool bCityToRebase = false;
	if(pToPlot->isCity())
	{
		// City must be owned by us
		if(pToPlot->getPlotCity()->getOwner() == getOwner())
		{
			bCityToRebase = true;
		}

		if (!bCityToRebase)
		{
#if defined(MOD_EVENTS_REBASE)
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanRebaseInCity, getOwner(), GetID(), iXDest, iYDest) == GAMEEVENTRETURN_TRUE) {
				bCityToRebase = true;
			} else {
#endif
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(getOwner());
				args->Push(GetID());
				args->Push(iXDest);
				args->Push(iYDest);

				// Attempt to execute the game events.
				// Will return false if there are no registered listeners.
				bool bResult = false;
				if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseInCity", args.get(), bResult))
				{
					// Check the result.
					if (bResult)
					{
						bCityToRebase = true;
					}
				}
			}
#if defined(MOD_EVENTS_REBASE)
			}
#endif
		}

		int iUnitsThere = pToPlot->countNumAirUnits(getTeam());
		//don't count ourselves!
		if (plot() == pToPlot)
			iUnitsThere--;

		if (iUnitsThere >= pToPlot->getPlotCity()->GetMaxAirUnits())
		{
			return false;
		}
	}

	// Rebase onto Unit which can hold cargo
	bool bUnitToRebase = false;

	if(!bCityToRebase)
	{
		IDInfo* pUnitNode = NULL;
		CvUnit* pLoopUnit = NULL;
		pUnitNode = pToPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pToPlot->nextUnitNode(pUnitNode);

			// Can this Unit hold us?
			if(canLoadUnit(*pLoopUnit, *pToPlot))
			{
				bUnitToRebase = true;

				break;
			}
		}
	}

	// No City or Unit at the target to rebase to
	if(!bCityToRebase && !bUnitToRebase)
	{
#if defined(MOD_GLOBAL_RELOCATION)
		// Is there a friendly improvement that AllowsRebaseTo
		ImprovementTypes eImprovement = pToPlot->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry *pkEntry = GC.getImprovementInfo(eImprovement);
			if (pkEntry && pkEntry->IsAllowsRebaseTo() && !pToPlot->IsImprovementPillaged() && pToPlot->IsFriendlyTerritory(getOwner()) && !pToPlot->isVisibleEnemyUnit(getOwner()))
			{
				// Check the loading limit for this improvement
				if (pToPlot->countNumAirUnits(getTeam()) < (/*3 in CP, 1 in VP*/ GD_INT_GET(BASE_CITY_AIR_STACKING) / 2))
				{
					return true;
				}
			}
		}
#endif

#if defined(MOD_EVENTS_REBASE)
		if (MOD_EVENTS_REBASE) {
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_CanRebaseTo, getOwner(), GetID(), iXDest, iYDest, bCityToRebase) == GAMEEVENTRETURN_TRUE) {
				return true;
			}
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());
			args->Push(iXDest);
			args->Push(iYDest);

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			if (LuaSupport::CallTestAny(pkScriptSystem, "CanRebaseTo", args.get(), bResult))
			{
				// Check the result.
				if (bResult)
				{
					return true;
				}
			}
		}
#if defined(MOD_EVENTS_REBASE)
		}
#endif

		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::rebase(int iX, int iY, bool bForced)
{
	if (!canRebaseAt(iX, iY, bForced))
	{
		return false;
	}

	CvPlot* oldPlot = plot();

	if (oldPlot->isCity())
	{
		CvCity* pCity = oldPlot->getPlotCity();
		if (pCity != NULL)
		{
			if (GET_PLAYER(getOwner()).GetFlatDefenseFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				pCity->updateStrengthValue();
			}

			if (GET_PLAYER(getOwner()).GetNeedsModifierFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				GET_PLAYER(getOwner()).CalculateNetHappiness();
			}
		}
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	CvAssert(pTargetPlot != NULL);
	if(pTargetPlot == NULL)
		return false;

	if (pTargetPlot->isCity())
	{
		CvCity* pCity = pTargetPlot->getPlotCity();
		if (pCity != NULL)
		{
			if (GET_PLAYER(getOwner()).GetFlatDefenseFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				pCity->updateStrengthValue();
			}

			if (GET_PLAYER(getOwner()).GetNeedsModifierFromAirUnits() != 0 && getUnitInfo().GetAirUnitCap() != 0)
			{
				GET_PLAYER(getOwner()).CalculateNetHappiness();
			}
		}
	}

	// Loses sight bonus for this turn
	setReconPlot(NULL);

	bool bShow = true;
	// Do the rebase first to keep the visualization in sequence
	if ((plot()->isVisibleToWatchingHuman() || pTargetPlot->isVisibleToWatchingHuman()) && !CvPreGame::quickMovement())
	{
		SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
		if(getSpecialUnitType() == eSpecialUnitPlane)
		{
			CvInterfacePtr<ICvPlot1> pDllOldPlot(new CvDllPlot(oldPlot));
			CvInterfacePtr<ICvPlot1> pDllTargetPlot(new CvDllPlot(pTargetPlot));

			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitRebased(pDllUnit.get(), pDllOldPlot.get(), pDllTargetPlot.get());
			bShow = false;		// Tell the setXY to not bother showing the move
		}
	}

#if defined(MOD_EVENTS_REBASE)
	if (MOD_EVENTS_REBASE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_RebaseTo, getOwner(), GetID(), iX, iY);
	} else {
#endif
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if (pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(iX);
		args->Push(iY);

		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "RebaseTo", args.get(), bResult);
	}
#if defined(MOD_EVENTS_REBASE)
	}
#endif

	setXY(pTargetPlot->getX(), pTargetPlot->getY(), false, bShow, false);
	finishMoves();

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canPillage(const CvPlot* pPlot) const
{
	if (!pPlot || !canMove())
		return false;

	if(isEmbarked())
		return false;

	if(!(getUnitInfo().IsPillage()))
		return false;

	if(pPlot->isOwned() && !isEnemy(pPlot->getTeam(), pPlot))
		return false;

	if(pPlot->getDomain() != getDomainType())
		return false;

	if (isBarbarian())
	{
		// barbarian boats not allowed to pillage, as they're too annoying :)
		if (getDomainType() == DOMAIN_SEA)
			return false;

		// barbs can't pillage camps yo
		if (pPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
			return false;
	}

	if(pPlot->getOwner() == NO_PLAYER && pPlot->isRoute())
	{
		PlayerTypes eRouteOwner = pPlot->GetPlayerResponsibleForRoute();
		if(eRouteOwner != NO_PLAYER && GET_PLAYER(eRouteOwner).isAlive())
		{
			if (!atWar(getTeam(), GET_PLAYER(eRouteOwner).getTeam()))
			{
				return false;
			}
		}
	}

	TechTypes ePillagePrereq = (TechTypes) getUnitInfo().GetPrereqPillageTech();
	if(ePillagePrereq != NO_TECH)
	{
		if(!GET_TEAM(GET_PLAYER(getOwner()).getTeam()).GetTeamTechs()->HasTech(ePillagePrereq))
		{
			return false;
		}
	}

	if(pPlot->isCity())
	{
		return false;
	}

	ImprovementTypes eImprovementType = pPlot->getImprovementType();

	// Either nothing to pillage or everything is pillaged to its max
	if((eImprovementType == NO_IMPROVEMENT || pPlot->IsImprovementPillaged()) &&
			(pPlot->getRouteType() == NO_ROUTE || pPlot->IsRoutePillaged() /* == GC.getPILLAGE_NUM_TURNS_DISABLED()*/))
	{
		return false;
	}

	if(eImprovementType == (ImprovementTypes)GD_INT_GET(RUINS_IMPROVEMENT))
	{
		return false;
	}
	else
	{
		//can't pillage what we built ourselves unless at war with the new owner ... stops exploits
		PlayerTypes eBuilder = pPlot->GetPlayerThatBuiltImprovement();
		if (eBuilder != NO_PLAYER && GET_PLAYER(eBuilder).getTeam() == getTeam())
			if (!GET_PLAYER(getOwner()).IsAtWarWith(pPlot->getOwner()))
				return false;

		//some improvements cannot be pillaged
		CvImprovementEntry* pImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
		if (pImprovementInfo)
		{
			if (pImprovementInfo->IsPermanent())
			{
				return MOD_PILLAGE_PERMANENT_IMPROVEMENTS;
			}
			
			if (pImprovementInfo->IsGoody())
			{
				return false;
			}

			// Special case: Feitoria can be in a city-state's lands, don't allow pillaging unless at war with its owner
			if (pImprovementInfo->GetLuxuryCopiesSiphonedFromMinor() > 0)
			{
				PlayerTypes eOwner = pPlot->getOwner();
				PlayerTypes eSiphoner = pPlot->GetPlayerThatBuiltImprovement();
				if (eOwner != NO_PLAYER && GET_PLAYER(eOwner).isMinorCiv())
				{
					if (eSiphoner != NO_PLAYER && GET_PLAYER(eSiphoner).isAlive())
					{
						if (!atWar(getTeam(), GET_PLAYER(eSiphoner).getTeam()))
						{
							return false;
						}
					}
				}
			}
		}
	}
	
#if defined(MOD_EVENTS_UNIT_ACTIONS)
	if (MOD_EVENTS_UNIT_ACTIONS) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanPillage, getOwner(), GetID(), eImprovementType, pPlot->getRouteType()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

bool CvUnit::shouldPillage(const CvPlot* pPlot, bool bConservative) const
{
	if (!canPillage(pPlot))
		return false;

	if (hasFreePillageMove() && pPlot->IsAdjacentCity())
		return true;

	if (pPlot->getOwningCity() != NULL && pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != BARBARIAN_PLAYER)
	{
		if (GET_PLAYER(m_eOwner).GetTacticalAI()->IsInFocusArea(pPlot))
		{
			ResourceTypes eResource = pPlot->getResourceType(getTeam());
			if (eResource != NO_RESOURCE)
			{
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo && (pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY))
					return true;
			}
		}

		if (GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetRawMilitaryStrengthComparedToUs(pPlot->getOwner()) > STRENGTH_AVERAGE || GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetRawTargetValue(pPlot->getOwner()) <= TARGET_VALUE_DIFFICULT)
			return true;

		if (!GET_PLAYER(m_eOwner).GetMilitaryAI()->IsPreferredAttackTarget(pPlot->getOwningCity()))
			return true;
	}

	ImprovementTypes eImprovement = pPlot->getImprovementType();

	// Citadel here?
	if (eImprovement != NO_IMPROVEMENT)
	{
		int iDamage = GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();

		if (iDamage != 0 && pPlot->getOwner() != NO_PLAYER && GET_TEAM(getTeam()).isAtWar(pPlot->getTeam()))
			return true;
	}

	CvCity* pOriginCity = getOriginCity();
	for (int iI = 0; iI < YIELD_GREAT_GENERAL_POINTS; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		if (GET_PLAYER(getOwner()).GetYieldFromPillage(eYield) > 0 || (pOriginCity && pOriginCity->GetYieldFromPillage(eYield) > 0))
			return true;
	}

	if (bConservative)
		return getDamage() > /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT);

	return getDamage() > 0;
}
//	--------------------------------------------------------------------------------
bool CvUnit::pillage()
{
	VALIDATE_OBJECT
	CvString strBuffer;

	ImprovementTypes eTempImprovement = NO_IMPROVEMENT;

	CvPlot* pPlot = plot();

	if(!canPillage(pPlot))
	{
		return false;
	}

	bool bImprovement = false;

	// Has an Improvement
	if(pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		if(!pPlot->IsImprovementPillaged())
		{
			bImprovement = true;
		}
	}

	// Is an Improvement here that hasn't already been pillaged?
	bool bSuccessfulNonRoadPillage = false;
	if(bImprovement)
	{
		eTempImprovement = pPlot->getImprovementType();
		CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
		if(pkImprovement)
		{
			if(pPlot->getTeam() != getTeam())
			{
				CvCity* pOriginCity = getOriginCity();
				if (pOriginCity == NULL)
					pOriginCity = GET_PLAYER(getOwner()).getCapitalCity();

				if (pPlot->getOwner() != NO_PLAYER)
				{
					GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_PILLAGE, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pOriginCity, pPlot->isWater());
					// call one for era scaling, and another for non-era scaling
					GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_PILLAGE_GLOBAL, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, NULL, pPlot->isWater());
					GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_PILLAGE_GLOBAL, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, NULL, pPlot->isWater());
					// and another for unit provided yields since it handles era scaling specially
					GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_PILLAGE_UNIT, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pOriginCity, pPlot->isWater(), true, false, NO_YIELD, this);
				}

				if((pPlot->getOwner() != NO_PLAYER && !isBarbarian() && !GET_PLAYER(pPlot->getOwner()).isBarbarian()) && GET_TEAM(getTeam()).isAtWar(GET_PLAYER(pPlot->getOwner()).getTeam()))
				{
					// Notify Diplo AI that damage has been done
					int iTileValue = /*40*/ GD_INT_GET(PILLAGED_TILE_BASE_WAR_VALUE);
					int iValueMultiplier = 0;
					bool bPillagedHighValueTile = false;

					if (pPlot->getResourceType(GET_PLAYER(pPlot->getOwner()).getTeam()) != NO_RESOURCE)
					{
						CvResourceInfo* pInfo = GC.getResourceInfo(pPlot->getResourceType(GET_PLAYER(pPlot->getOwner()).getTeam()));
						if (pInfo)
						{
							switch (pInfo->getResourceUsage())
							{
							case RESOURCEUSAGE_STRATEGIC:
								iValueMultiplier += 100;
								bPillagedHighValueTile = true;
								break;
							case RESOURCEUSAGE_LUXURY:
								iValueMultiplier += 50;
								bPillagedHighValueTile = true;
								break;
							case RESOURCEUSAGE_BONUS:
								iValueMultiplier += 20;
								break;
							}
						}
					}

					if (pkImprovement->GetDefenseModifier() > 0)
					{
						iValueMultiplier += pkImprovement->GetDefenseModifier();
						if (pPlot->IsChokePoint())
						{
							bPillagedHighValueTile = true;
							if (pkImprovement->IsNoFollowUp())
								iValueMultiplier += 20;
						}
					}

					if (pkImprovement->IsCreatedByGreatPerson())
					{
						iValueMultiplier += 100;
						bPillagedHighValueTile = true;
					}

					iTileValue *= (100 + iValueMultiplier);
					iTileValue /= 100;

					// Did the plot owner's master fail to protect their territory?
					if (!isBarbarian() && GET_PLAYER(pPlot->getOwner()).isMajorCiv() && GET_PLAYER(pPlot->getOwner()).IsVassalOfSomeone())
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if (GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->IsVassal(eLoopPlayer))
							{
								GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeVassalProtectValue(eLoopPlayer, -iTileValue);
							}
						}
					}

					GET_PLAYER(getOwner()).ApplyWarDamage(pPlot->getOwner(), iTileValue);

					if (GET_PLAYER(getOwner()).isMajorCiv())
					{
						int iWarProgressValue = /*10*/ GD_INT_GET(WAR_PROGRESS_PILLAGED_IMPROVEMENT);
						if (bPillagedHighValueTile)
						{
							iWarProgressValue *= /*200*/ GD_INT_GET(WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER);
							iWarProgressValue /= 100;
						}
						GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeWarProgressScore(pPlot->getOwner(), iWarProgressValue);
					}
					if (GET_PLAYER(pPlot->getOwner()).isMajorCiv())
					{
						int iWarProgressValue = /*-5*/ GD_INT_GET(WAR_PROGRESS_LOST_IMPROVEMENT);
						if (bPillagedHighValueTile)
						{
							iWarProgressValue *= /*200*/ GD_INT_GET(WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER);
							iWarProgressValue /= 100;
						}
						GET_PLAYER(pPlot->getOwner()).GetDiplomacyAI()->ChangeWarProgressScore(getOwner(), iWarProgressValue);
					}
				}

				int iPillageGold = 0;

				// TODO: add scripting support for "doPillageGold"

				if (MOD_EVENTS_UNIT_ACTIONS)
				{
					int iValue = 0;
					if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_UnitPillageGold, getOwner(), GetID(), eTempImprovement, pkImprovement->GetPillageGold()) == GAMEEVENTRETURN_VALUE)
					{
						iPillageGold = iValue;
						CUSTOMLOG("Pillage gold is %i", iPillageGold);
					}
				}
				if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
				{
					int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
					if (iEra <= 0)
						iEra = 1;

					iPillageGold += pkImprovement->GetPillageGold() * iEra * GC.getGame().randRangeInclusive(85, 100, CvSeeder(plot()->GetPseudoRandomSeed())) / 100;
					iPillageGold += getPillageChange() * iPillageGold / 100;
				}
				else
				{
					iPillageGold += GC.getGame().randRangeInclusive(0, pkImprovement->GetPillageGold(), CvSeeder(plot()->GetPseudoRandomSeed()));
					iPillageGold += getPillageChange() * iPillageGold / 100;
				}

				if (pPlot->getOwner() != NO_PLAYER)
				{
					if (GET_PLAYER(pPlot->getOwner()).isBorderGainlessPillage())
					{
						iPillageGold = 0;
					}
					else
					{
						CvCity* pCityOfThisOtherTeamsPlot = pPlot->getEffectiveOwningCity();
						if (pCityOfThisOtherTeamsPlot != NULL && pCityOfThisOtherTeamsPlot->IsLocalGainlessPillage())
						{
							iPillageGold = 0;
						}
					}
				}

				if (iPillageGold > 0)
				{
					iPillageGold *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
					iPillageGold /= 100;
					GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iPillageGold);

					if(getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, pkImprovement->GetTextKey());
						DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_PILLAGE", MESSAGE_TYPE_INFO, m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX(), pPlot->getY()*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(getOwner(), strBuffer.c_str(), plot(), plot()->getOwner());
					}

					if(pPlot->isOwned() && pPlot->getOwner() == GC.getGame().getActivePlayer())
					{
						strBuffer = GetLocalizedText("TXT_KEY_MISC_IMP_DESTROYED", pkImprovement->GetTextKey(), getNameKey(), getVisualCivAdjective(pPlot->getTeam()));
						DLLUI->AddPlotMessage(0, pPlot->GetPlotIndex(), pPlot->getOwner(), false, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, m_pUnitInfo->GetButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX(), pPlot->getY(), true, true*/);
						if (MOD_WH_MILITARY_LOG)
							MILITARYLOG(pPlot->getOwner(), strBuffer.c_str(), pPlot, plot()->getOwner());
					}
				}
			}

			//Unlock any possible achievements.
			if (MOD_API_ACHIEVEMENTS && getOwner() == GC.getGame().getActivePlayer() && strcmp(pkImprovement->GetType(), "IMPROVEMENT_FARM") == 0)
				CvAchievementUnlocker::FarmImprovementPillaged();

			// Improvement that's destroyed?
			bSuccessfulNonRoadPillage = true;
			if(pkImprovement->IsDestroyedWhenPillaged())
			{
				// If this improvement auto-added a route, we also need to remove the route
				ImprovementTypes eOldImprovement = pPlot->getImprovementType();
				
				// Find the build for this improvement
				for (int i = 0; i < GC.getNumBuildInfos(); i++) {
					CvBuildInfo* pkBuild = GC.getBuildInfo((BuildTypes)i);
					
					if (pkBuild && ((ImprovementTypes)pkBuild->getImprovement()) == eOldImprovement) {
						// Found it, but did it auto-add a route?
						if (pkBuild->getRoute() != NO_ROUTE) {
							// Yes, so remove the route as well
							pPlot->setRouteType(NO_ROUTE);
						}
						
						// Our work here is done
						break;
					}
				}

				pPlot->setImprovementType(NO_IMPROVEMENT);
			}
			// Improvement that's pillaged?
			else
			{
				pPlot->SetImprovementPillaged(true);
			}

			if (pkImprovement->IsDisplacePillager())
			{
				jumpToNearestValidPlot();
			}
		}
	}
	else if(pPlot->isRoute())
	{
		pPlot->SetRoutePillaged(true);
	}

	if(!hasFreePillageMove())
	{
		changeMoves(-GD_INT_GET(MOVE_DENOMINATOR));
	}

	if (IsGainsXPFromPillaging())
	{
		changeExperienceTimes100(/*500*/ GD_INT_GET(BALANCE_SCOUT_XP_BASE) * 500);
	}

	if(bSuccessfulNonRoadPillage)
	{
		if (getPillageBonusStrengthPercent() != 0)
		{
			SetBaseCombatStrength(getUnitInfo().GetCombat() + ((getPillageBonusStrengthPercent() * getUnitInfo().GetCombat()) / 100));			
		}

		DoAdjacentPlotDamage(pPlot, getAOEDamageOnPillage(), "TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_AOE_STRIKE_PILLAGE");

		//if the plot isn't guarded by a gainless pillage building for this player, nor this city
		if (!(pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).isBorderGainlessPillage()) )
		{
			CvCity* pCityOfThisPlot = pPlot->getEffectiveOwningCity();
			if ( pCityOfThisPlot == NULL || !(pCityOfThisPlot->IsLocalGainlessPillage()) )
			{
				if (hasHealOnPillage())
				{
					// completely heal unit
					changeDamage(-getDamage());
				}
				else
				{
					int iHealAmount = min(getDamage(), /*25*/ GD_INT_GET(PILLAGE_HEAL_AMOUNT));
					iHealAmount += getPartialHealOnPillage();
					changeDamage(-iHealAmount);
				}
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canFoundCity(const CvPlot* pPlot, bool bIgnoreDistanceToExistingCities, bool bIgnoreHappiness, bool bForAliveCheck) const
{
	if (pPlot)
	{
		if (!canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
			return false;

		if (!GET_PLAYER(getOwner()).canFoundCityExt(pPlot->getX(), pPlot->getY(), bIgnoreDistanceToExistingCities, bIgnoreHappiness))
			return false;
	}

	if(m_pUnitInfo->IsFound())
	{
		return true;
	}
	else if (m_pUnitInfo->IsFoundAbroad())
	{
		if (pPlot && GET_PLAYER(m_eOwner).getCapitalCity())
		{
			return GET_PLAYER(m_eOwner).getCapitalCity()->plot()->getLandmass() != pPlot->getLandmass();
		}
		else
		{
			return true;
		}
	}
	else if (CanFoundColony() && (bForAliveCheck || GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND))
	{
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::foundCity()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if (IsCanAttack()) 
	{
		CUSTOMLOG("Trying to found a city with combat unit %s at (%i, %i)", getName().c_str(), getX(), getY());
	}

	if(!canFoundCity(pPlot))
	{
		return false;
	}

	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(eActivePlayer == getOwner())
	{
		CvInterfacePtr<ICvPlot1> pDllPlot = GC.WrapPlotPointer(pPlot);
		DLLUI->lookAt(pDllPlot.get(), CAMERALOOKAT_NORMAL);
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);

	ReligionTypes eReligion = (MOD_GLOBAL_RELIGIOUS_SETTLERS && GetReligionData()->GetReligion() > RELIGION_PANTHEON) ? GetReligionData()->GetReligion() : NO_RELIGION;
	bool bForce = MOD_GLOBAL_RELIGIOUS_SETTLERS && eReligion == NO_RELIGION;
	kPlayer.foundCity(getX(), getY(), eReligion, bForce, m_pUnitInfo);

	if (IsCanAttack() && plot()->getPlotCity() != NULL) 
	{
		CUSTOMLOG("  ... success!  They founded %s", plot()->getPlotCity()->getName().c_str());
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());

		//Achievement
		if (MOD_API_ACHIEVEMENTS && eActivePlayer == getOwner() && kActivePlayer.getNumCities() >= 2 && kActivePlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_SECOND_CITY);
	}
#if defined(MOD_BALANCE_CORE)
	int iMaxRange = 3;
	for(int iDX = -iMaxRange; iDX <= iMaxRange; iDX++)
	{
		for(int iDY = -iMaxRange; iDY <= iMaxRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iMaxRange);
			if(pLoopPlot && pLoopPlot->getOwner() == getOwner())
			{
				pLoopPlot->verifyUnitValidPlot();
			}
		}
	}
	if (kPlayer.GetPlayerTraits()->IsBuyOwnedTiles())
	{
		kPlayer.SetNumPlotsBought(0);
		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if (pNotifications)
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_RESET_PLOT_COST");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_RESET_PLOT_COST");
			pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, plot()->getX(), plot()->getY(), -1, -1);
		}
	}
#endif
		
#if defined(MOD_EVENTS_UNIT_FOUNDED)
	if (MOD_EVENTS_UNIT_FOUNDED) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCityFounded, getOwner(), GetID(), getUnitType(), getX(), getY());
	}
#endif
	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitVisibility(pDllUnit.get(), false);
	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canJoinCity(const CvPlot* /*pPlot*/, SpecialistTypes /*eSpecialist*/) const
{
	VALIDATE_OBJECT

	// JON: Great People can no longer join Cities as Specialists 08/18/09
	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::joinCity(SpecialistTypes eSpecialist)
{
	VALIDATE_OBJECT

	if(!canJoinCity(plot(), eSpecialist))
	{
		return false;
	}

	kill(true);

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding) const
{
	VALIDATE_OBJECT
	CvCity* pCity = NULL;

	if(eBuilding == NO_BUILDING)
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(getTeam() != pCity->getTeam())
	{
		return false;
	}

	if(pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::construct(BuildingTypes eBuilding)
{
	VALIDATE_OBJECT
	CvCity* pCity = NULL;

	if(!canConstruct(plot(), eBuilding))
	{
		return false;
	}

	pCity = plot()->getPlotCity();

	if(pCity != NULL)
	{
		pCity->GetCityBuildings()->SetNumRealBuilding(eBuilding, pCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) + 1);
	}

	if(plot()->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanFoundReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}

	// If prophet has started spreading religion, can't do other functions (possible with India)
	if (m_pUnitInfo->IsSpreadReligion() && GetReligionData()->GetSpreadsUsed()>0)
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).GetReligions()->IsFoundingReligion())
	{
		return false;
	}

	if(pPlot==NULL || !pPlot->isCity() || getTeam() != pPlot->getTeam())
	{
		return false;
	}

	if(GET_TEAM(getTeam()).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(pReligions->GetNumReligionsStillToFound() <= 0 && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsAlwaysReligion())
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).GetReligions()->OwnsReligion())
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (MOD_EVENTS_FOUND_RELIGION)
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanFoundReligion, getOwner(), pCity->GetID()) == GAMEEVENTRETURN_FALSE)
		{
			return false;
		}
	}
	else
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(pCity->GetID());

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "PlayerCanFoundReligion", args.get(), bResult))
			{
				if (!bResult) 
				{
					return false;
				}
			}
		}
	}

	if (pReligions->GetAvailableFounderBeliefs(getOwner(), NO_RELIGION).size() < 1 || pReligions->GetAvailableFollowerBeliefs(getOwner(), NO_RELIGION).size() < 1)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoFoundReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanFoundReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			bool bIndiaException = false;
			if (kOwner.GetPlayerTraits()->IsProphetFervor())
			{
				GetReligionDataMutable()->IncrementSpreadsUsed();
				bIndiaException = true;
				finishMoves();
			}

			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RELIGION");
					pNotifications->Add(NOTIFICATION_FOUND_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}
				kOwner.GetReligions()->SetFoundingReligion(true);

				if (!bIndiaException)
				{
					kOwner.DoGreatPersonExpended(getUnitType(), this);
					kill(true);
				}
			}
			else
			{
				ReligionTypes eReligion = pReligions->GetReligionToFound(getOwner());
				if(eReligion != NO_RELIGION)
				{
					// Pick beliefs for this religion
					BeliefTypes eBeliefs[4];
					eBeliefs[2] = NO_BELIEF;
					eBeliefs[3] = NO_BELIEF;
					int iIndex = 0;
					if(!kOwner.GetReligions()->HasCreatedPantheon())
					{
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChoosePantheonBelief(kOwner.GetID());
						iIndex++;
					}

					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFounderBelief(kOwner.GetID(), eReligion);
					iIndex++;

					eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseFollowerBelief(kOwner.GetID(), eReligion);
					iIndex++;

					if(kOwner.GetPlayerTraits()->IsBonusReligiousBelief())
					{
						eBeliefs[iIndex] = kOwner.GetReligionAI()->ChooseBonusBelief(kOwner.GetID(), eReligion, eBeliefs[0], eBeliefs[1], eBeliefs[2]);
					}

#if defined(MOD_EVENTS_FOUND_RELIGION)
					if (MOD_EVENTS_FOUND_RELIGION) 
					{
						int iValue = 0;
						if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_GetBeliefToFound, kOwner.GetID(), eReligion) == GAMEEVENTRETURN_VALUE) {
							// Defend against modder stupidity!
							if (iValue > NO_BELIEF && iValue < GC.getNumBeliefInfos()) {
								eBeliefs[3] = (BeliefTypes)iValue;
							}
						}
					}
#endif

					pReligions->FoundReligion(getOwner(), eReligion, NULL, eBeliefs[0], eBeliefs[1], eBeliefs[2], eBeliefs[3], pkCity);

					if (!bIndiaException)
					{
						kOwner.DoGreatPersonExpended(getUnitType(), this);
						kill(true);
					}
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanEnhanceReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	if(!m_pUnitInfo->IsFoundReligion())
	{
		return false;
	}

	// If prophet has started spreading religion, can't do other functions
	if (m_pUnitInfo->IsSpreadReligion() && GetReligionData()->GetSpreadsUsed()>0)
	{
		return false;
	}

	if(pPlot == NULL || !pPlot->isCity() || getTeam() != pPlot->getTeam())
	{
		return false;
	}

	if(GET_TEAM(getTeam()).isMinorCiv())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!GET_PLAYER(getOwner()).GetReligions()->OwnsReligion())
	{
		return false;
	}

	ReligionTypes eReligion = GET_PLAYER(getOwner()).GetReligions()->GetOwnedReligion();
	const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
	if (pReligion->m_bEnhanced)
	{
		return false;
	}

	if(getX() != pReligion->m_iHolyCityX || getY() != pReligion->m_iHolyCityY)
	{
		return false;
	}

	if (pReligions->GetAvailableEnhancerBeliefs(getOwner(), eReligion).size() < 1 || pReligions->GetAvailableFollowerBeliefs(getOwner(), eReligion).size() < 1)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoEnhanceReligion()
{
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	CvPlot* pkPlot = plot();
	CvCity* pkCity = (NULL != pkPlot)? pkPlot->getPlotCity() : NULL;

	if(pkPlot != NULL && pkCity != NULL)
	{
		if(CanEnhanceReligion(pkPlot))
		{
			CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			bool bIndiaException = false;
			if (kOwner.GetPlayerTraits()->IsProphetFervor())
			{
				GetReligionDataMutable()->IncrementSpreadsUsed();
				if (GetReligionData()->GetSpreadsLeft(this) > 0)
					bIndiaException = true;

				finishMoves();
			}

			if(kOwner.isHuman())
			{
				CvAssertMsg(pkCity != NULL, "No City??");

				CvNotifications* pNotifications = kOwner.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENHANCE_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENHANCE_RELIGION");
					pNotifications->Add(NOTIFICATION_ENHANCE_RELIGION, strBuffer, strSummary, pkPlot->getX(), pkPlot->getY(), -1, pkCity->GetID());
				}

				if (!bIndiaException)
				{
					kOwner.DoGreatPersonExpended(getUnitType(), this);
					kill(true);
				}
			}
			else
			{
				ReligionTypes eReligion = GET_PLAYER(getOwner()).GetReligions()->GetOwnedReligion();
				if (eReligion != NO_RELIGION)
				{
					// Pick a belief for this religion
					BeliefTypes eBelief1 = kOwner.GetReligionAI()->ChooseFollowerBelief(kOwner.GetID(), eReligion); // temporary
					BeliefTypes eBelief2 = kOwner.GetReligionAI()->ChooseEnhancerBelief(kOwner.GetID(), eReligion); // temporary

					pReligions->EnhanceReligion(getOwner(), eReligion, eBelief1, eBelief2);

					if (!bIndiaException)
					{
						kOwner.DoGreatPersonExpended(getUnitType(), this);
						kill(true);
					}
				}
				else
				{
					CvAssertMsg(false, "No religions available to found.");
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanSpreadReligion(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(!m_pUnitInfo->IsSpreadReligion())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
	}

	if (GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsForeignReligionSpreadImmune() && (getOwner() != pCity->getOwner()))
	{
		if (GetReligionData()->GetReligion() != GET_PLAYER(pCity->getOwner()).GetReligions()->GetStateReligion())
		{
			return false;
		}
	}
	else if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = (GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->GetAlly());
		if (eAlly != NO_PLAYER)
		{
			if (GET_PLAYER(eAlly).GetPlayerTraits()->IsForeignReligionSpreadImmune() && getOwner() != eAlly)
			{
				if (GetReligionData()->GetReligion() != GET_PLAYER(eAlly).GetReligions()->GetStateReligion())
				{
					return false;
				}
			}
		}
	}

	// Blocked by Inquisitor?
	if (!MOD_BALANCE_CORE_INQUISITOR_TWEAKS)
	{
		if (getOwner() != pCity->getOwner() && pCity->GetCityReligions()->IsDefendedAgainstSpread(GetReligionData()->GetReligion()))
		{
			return false;
		}
	}

	if(isDelayedDeath())
	{
		return false;
	}
	
#if defined(MOD_EVENTS_RELIGION)
	if (MOD_EVENTS_RELIGION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanSpreadReligion, getOwner(), GetID(), plot()->getX(), plot()->getY()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoSpreadReligion()
{
#if defined(MOD_BALANCE_CORE_BELIEFS)
	int iCSInfluence = 0;
	int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
	if(iEra < 1)
	{
		iEra = 1;
	}
#endif

	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanSpreadReligion(plot()))
		{
			int iPreSpreadFollowers = 0;
			int iConversionStrength = GetConversionStrength(pCity);

			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			ReligionTypes eReligion = GetReligionData()->GetReligion();
			if (eReligion <= RELIGION_PANTHEON)
				return false;

			const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
			if(pReligion)
			{
				iPreSpreadFollowers = pCity->GetCityReligions()->GetNumFollowers(eReligion);
#if defined(MOD_BALANCE_CORE_BELIEFS)
				CvCity* pHolyCity = pReligion->GetHolyCity();
				iCSInfluence = (pReligion->m_Beliefs.GetMissionaryInfluenceCS(getOwner(), pHolyCity) * iEra);
#endif
			}

			CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
			if (pCity->getOwner() != m_eOwner)
			{
				int iOtherFollowers = pCity->GetCityReligions()->GetFollowersOtherReligions(eReligion, true);
				kPlayer.doInstantYield(INSTANT_YIELD_TYPE_F_SPREAD, false, NO_GREATPERSON, NO_BUILDING, iOtherFollowers, false, pCity->getOwner(), plot());
			}

			if(IsGreatPerson())
			{
				pCity->GetCityReligions()->AddProphetSpread(eReligion, iConversionStrength, getOwner());
			}
			else
			{
				pCity->GetCityReligions()->AddMissionarySpread(eReligion, iConversionStrength, getOwner());
			}

			GetReligionDataMutable()->IncrementSpreadsUsed();

			int iPostFollowers = pCity->GetCityReligions()->GetNumFollowers(eReligion);
			
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_SPREAD, false, NO_GREATPERSON, NO_BUILDING, iPostFollowers - iPreSpreadFollowers, false, pCity->getOwner(), plot());

			if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetReligionData()->GetReligion(), m_eOwner);
				Localization::String strReligionName = Localization::Lookup(pReligion->GetName());

				char text[256] = {0};
				sprintf_s(text, "[COLOR_WHITE]%s: %d [ICON_PEACE][ENDCOLOR]",
					strReligionName.toUTF8(),
					iConversionStrength / /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER));
				SHOW_PLOT_POPUP(pCity->plot(), getOwner(), text);
			}

#if defined(MOD_BALANCE_CORE_BELIEFS)
			if(iCSInfluence > 0)
			{
				CvPlayer &kPlayer = GET_PLAYER(m_eOwner);
				if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
				{
					kPlayer.doInstantYield(INSTANT_YIELD_TYPE_SPREAD, false, NO_GREATPERSON, NO_BUILDING, iCSInfluence, true, NO_PLAYER, NULL, false, pCity, false, true, false, YIELD_JFD_LOYALTY);
					GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->ChangeFriendshipWithMajor(kPlayer.GetID(), iCSInfluence, false);
					if (pCity->plot() && pCity->plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
					{
						char text[256] = {0};
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_INFLUENCE]", iCSInfluence);
						SHOW_PLOT_POPUP(pCity->plot(),getOwner(), text);
					}
				}
			}
#endif

			bool bShow = plot()->isActiveVisible();
			if(bShow)
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
			}

			if(GetReligionData()->GetSpreadsLeft(this) <= 0)
			{
				if(IsGreatPerson())
				{
					CvPlayer& kPlayer = GET_PLAYER(getOwner());
					kPlayer.DoGreatPersonExpended(getUnitType(), this);
				}

				kill(true);
			}
			else
			{
				if(bShow)
				{
					// Because the "Activate" animation will possibly put the animation state into a end-state, we will force a reset, since the unit will still be alive
					CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitResetAnimationState(pDllUnit.get());
				}

				finishMoves();
			}

			//Achievements
			if (MOD_API_ACHIEVEMENTS)
			{
				const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
				if (getOwner() == eActivePlayer && pCity->getOwner() != eActivePlayer)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_17);
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanRemoveHeresy(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(!m_pUnitInfo->IsRemoveHeresy())
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(GetReligionData()->GetReligion() == NO_RELIGION)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if(pCity == NULL)
	{
		pCity = pPlot->GetAdjacentCity();
		if(pCity == NULL)
		{
			return false;
		}
	}

	if(pCity->getOwner() != getOwner())
	{
		return false;
	}

	if(!pCity->GetCityReligions()->IsReligionHereOtherThan(GetReligionData()->GetReligion()))
	{
		return false;
	}

#if defined(MOD_EVENTS_RELIGION)
	if (MOD_EVENTS_RELIGION) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanRemoveHeresy, getOwner(), GetID(), plot()->getX(), plot()->getY()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoRemoveHeresy()
{
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(CanRemoveHeresy(plot()))
		{
			bool bShow = plot()->isActiveVisible();
			if(bShow)
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitActivate(pDllUnit.get());
			}

			int iBeforeFollowers = pCity->GetCityReligions()->GetNumFollowers(GetReligionData()->GetReligion());
			pCity->GetCityReligions()->RemoveOtherReligions(GetReligionData()->GetReligion(), getOwner());
			int iAfterFollowers = pCity->GetCityReligions()->GetNumFollowers(GetReligionData()->GetReligion());

			int iDelta = iAfterFollowers - iBeforeFollowers;
			if (iDelta>0)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetReligionData()->GetReligion(), getOwner());
				if (pReligion)
				{
					CvCity* pHolyCity = pReligion->GetHolyCity();
					if (pHolyCity != NULL)
						GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_REMOVE_HERESY, false, NO_GREATPERSON, NO_BUILDING, iDelta, true, NO_PLAYER, NULL, false, pHolyCity);
				}
			}

			//Achievements
			if (MOD_API_ACHIEVEMENTS && getOwner() == GC.getGame().getActivePlayer())
			{
				CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
				if(strcmp(kPlayer.getCivilizationTypeKey(), "CIVILIZATION_SPAIN") == 0)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_24);
				}
			}
			
			kill(true);
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumFollowersAfterInquisitor() const
{
	int iRtnValue = 0;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterInquisitor(GetReligionData()->GetReligion());
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
ReligionTypes CvUnit::GetMajorityReligionAfterInquisitor() const
{
	ReligionTypes eRtnValue = NO_RELIGION;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterInquisitor(GetReligionData()->GetReligion());
	}

	return eRtnValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumFollowersAfterSpread() const
{
	int iRtnValue = 0;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterProphetSpread(GetReligionData()->GetReligion(), GetConversionStrength(pCity));
		}
		else
		{
			iRtnValue = pCity->GetCityReligions()->GetNumFollowersAfterSpread(GetReligionData()->GetReligion(), GetConversionStrength(pCity));
		}
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
ReligionTypes CvUnit::GetMajorityReligionAfterSpread() const
{
	ReligionTypes eRtnValue = NO_RELIGION;
	CvCity* pCity = GetSpreadReligionTargetCity();

	if (pCity != NULL)
	{
		if(IsGreatPerson())
		{
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterProphetSpread(GetReligionData()->GetReligion(), GetConversionStrength(pCity));
		}
		else
		{
			eRtnValue = pCity->GetCityReligions()->GetMajorityReligionAfterSpread(GetReligionData()->GetReligion(), GetConversionStrength(pCity));
		}
	}

	return eRtnValue;
}

//	--------------------------------------------------------------------------------
CvCity *CvUnit::GetSpreadReligionTargetCity() const
{
	CvCity* pCity = NULL;
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		pCity = pPlot->getPlotCity();
		if(pCity == NULL)
		{
			pCity = pPlot->GetAdjacentCity();
			if(pCity == NULL)
			{
				return NULL;
			}
		}
	}
	return pCity;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetConversionStrength(const CvCity* pCity) const
{
	int iReligiousStrength = /*10*/ GD_INT_GET(RELIGION_MISSIONARY_PRESSURE_MULTIPLIER) * GetReligionData()->GetReligiousStrength();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eReligion = GetReligionData()->GetReligion();
	if(eReligion > RELIGION_PANTHEON)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eReligion, getOwner());
		if(pReligion)
		{
			CvCity* pHolyCity = pReligion->GetHolyCity();
			int iStrengthMod = 0;
			if (IsGreatPerson())
			{
				iStrengthMod = pReligion->m_Beliefs.GetProphetStrengthModifier(getOwner(), pHolyCity);
			}
			else
			{
				iStrengthMod = pReligion->m_Beliefs.GetMissionaryStrengthModifier(getOwner(), pHolyCity);
			}

			if(iStrengthMod != 0)
			{
				iReligiousStrength *= (100 + iStrengthMod);
				iReligiousStrength /= 100;
			}
		}
	}

	if (pCity != NULL && MOD_RELIGION_CONVERSION_MODIFIERS) 
	{
		// Modify iReligiousStrength based on city defenses, but only against hostile units (ie any not the same team as the city)
		PlayerTypes eFromPlayer = getOwner();
		PlayerTypes eToPlayer = pCity->getOwner();

		if (getTeam() != pCity->getTeam()) {
			CvPlayer& pToPlayer = GET_PLAYER(eToPlayer);
			int iCityModifier = pCity->GetConversionModifier();
			
			if (pToPlayer.isMinorCiv() && pToPlayer.GetMinorCivAI()->IsActiveQuestForPlayer(eFromPlayer, MINOR_CIV_QUEST_SPREAD_RELIGION)) {
				// The City State actively wants this religion
				iCityModifier *= -1;
			}

			iReligiousStrength *= (100 + (iCityModifier + pToPlayer.GetConversionModifier() + pToPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CONVERSION_MODIFIER))); 
			iReligiousStrength /= 100;
		}
	}
	// CUSTOMLOG("Unit conversion str: %i", iReligiousStrength);

	// Blocked by Inquisitor?
	if (MOD_BALANCE_CORE_INQUISITOR_TWEAKS && pCity != NULL && getOwner() != pCity->getOwner())
	{
		if (pCity->GetCityReligions()->IsDefendedAgainstSpread(GetReligionData()->GetReligion()))
		{
			iReligiousStrength /= max(/*2*/ GD_INT_GET(INQUISITOR_CONVERSION_REDUCTION_FACTOR), 1);
		}
	}

	return iReligiousStrength;
}

bool CvUnit::greatperson()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pPlayer) return false;
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());
	CvAssertMsg(pTeam, "Owner team of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pTeam) return false;

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		pPlayer->DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}

int CvUnit::GetScaleAmount(int iAmountToScale) const
{
	int iScaleTotal = iAmountToScale;
	int iExtra = 0;
	for (int i = 0; i < GC.getNumImprovementInfos(); i++)
	{
		iExtra = 0;
		ImprovementTypes eImprovement = (ImprovementTypes)i;
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		int iScaleAmount = getUnitInfo().GetScalingFromOwnedImprovements(eImprovement);
		if (iScaleAmount <= 0)
			continue;

		int iOwned = GET_PLAYER(getOwner()).getImprovementCount(eImprovement, true);
		iExtra = (iOwned * iScaleAmount) * iAmountToScale;
		iExtra /= 100;

		iScaleTotal += iExtra;
	}

	return iScaleTotal;
}

/// Great Scientist Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canDiscover(const CvPlot* /*pPlot*/, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(getUnitInfo().GetNumFreeTechs() == 0 && getUnitInfo().GetBaseBeakersTurnsToCount() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(GET_PLAYER(getOwner()).getNumCities() == 0)
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getDiscoverAmount()
{
	int iScience = 0;
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (pPlayer)
	{
		// Beakers boost based on previous turns (no base value)
		int iPreviousTurnsToCount = m_pUnitInfo->GetBaseBeakersTurnsToCount();
		if (iPreviousTurnsToCount == 0)
			return 0;
		
		iScience = pPlayer->getYieldPerTurnHistory(YIELD_SCIENCE, iPreviousTurnsToCount);

		// scale science by the number of academies
		if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
			iScience = GetScaleAmount(iScience);

		// scale with game speed
		iScience = iScience * GC.getGame().getGameSpeedInfo().getResearchPercent() / 100;
		// scale with player traits/policies etc.
		iScience = iScience * (100 + pPlayer->GetGreatScientistBeakerMod()) / 100;
	}
	return MAX(0, iScience);
}

//	--------------------------------------------------------------------------------
bool CvUnit::discover()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canDiscover(pPlot))
	{
		return false;
	}

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pPlayer) return false;
	CvTeam* pTeam = &GET_TEAM(pPlayer->getTeam());
	CvAssertMsg(pTeam, "Owner team of unit not expected to be NULL. Please send Anton your save file and version.");
	if (!pTeam) return false;

	// Beakers boost based on previous turns
	int iBeakersBonus = GetScienceBlastStrength();
	TechTypes eCurrentTech = pPlayer->GetPlayerTechs()->GetCurrentResearch();
	if(eCurrentTech == NO_TECH)
	{
		pPlayer->changeOverflowResearch(iBeakersBonus);
	}
	else
	{
		pTeam->GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, getOwner());
	}

	// Free techs
	int iNumFreeTechs = m_pUnitInfo->GetNumFreeTechs();
	if(!isHuman())
	{
		for(int iI = 0; iI < iNumFreeTechs; iI++)
		{
			pPlayer->AI_chooseFreeTech();
		}
	}
	else
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_GREAT_PERSON_CHOOSE_TECH");
		pPlayer->chooseTech(iNumFreeTechs, strBuffer.GetCString());
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		pPlayer->DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}

/// Great Engineer Functions
//	--------------------------------------------------------------------------------
/// Can this Unit rush a Building in pCity?
bool CvUnit::IsCanRushBuilding(CvCity* pCity, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(pCity == NULL)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if(!m_pUnitInfo->IsRushBuilding())
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(pCity->getProductionTurnsLeft() == 1)
		{
			return false;
		}

		if(!pCity->isProductionBuilding())
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Tells this Unit to rush a Building here (in a City)
bool CvUnit::DoRushBuilding()
{
	VALIDATE_OBJECT
	CvCity* pCity = plot()->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(!IsCanRushBuilding(pCity, false))
	{
		return false;
	}

	pCity->setProduction(pCity->getProductionNeeded());

	if(plot()->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	kill(true);

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canHurry(const CvPlot* pPlot, bool bTestVisible) const
{
	if(isDelayedDeath())
	{
		return false;
	}

	if(GetHurryStrength() == 0)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();

	if(pCity == NULL)
	{
		return false;
	}

	if(pCity->getProductionTurnsLeft() == 1)
	{
		return false;
	}

	if(!bTestVisible)
	{
		if(!(pCity->isProductionBuilding()))
		{
			if (!pCity->isProductionUnit())
			{
				return false;
			}
			else
			{
				UnitTypes eUnit = pCity->getProductionUnit();
				CvUnitEntry *pkUnit = GC.GetGameUnits()->GetEntry(eUnit);
				if (pkUnit && GET_PLAYER(pCity->getOwner()).IsEnablesSSPartHurry())
				{
					if (pkUnit->GetSpaceshipProject() != NO_PROJECT)
					{
						return true;
					}
				}
				return false;
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getMaxHurryProduction(CvCity* pCity) const
{
	int iProduction = 0;

	// Get base value from unit
	iProduction = m_pUnitInfo->GetBaseHurry();
	// Scale it by the Era Scaler (starts at Renaissance)
	if (MOD_GP_ERA_SCALING)
	{
		int EraModifiers[6] = { 100, 200, 250, 400, 475, 575 };
		int iIndex = MAX(0, GET_PLAYER(getOwner()).GetCurrentEra() - 2);
		iProduction = iProduction * EraModifiers[iIndex] / 100;
	}
	// Add production from population (does not scale with era)
	iProduction += (m_pUnitInfo->GetHurryMultiplier() * pCity->getPopulation());

	// Add production from average empire city production (does not scale with era)
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	if (pPlayer)
		iProduction += pPlayer->getYieldPerTurnHistory(YIELD_PRODUCTION, m_pUnitInfo->GetBaseProductionTurnsToCount());

	if (iProduction == 0)
		return 0;

	// Scale production with number of manufactories
	if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
		iProduction = GetScaleAmount(iProduction);

	// scale with game speed
	iProduction = iProduction * GC.getGame().getGameSpeedInfo().getUnitHurryPercent() / 100;
	// scale with player traits/policies etc.
	iProduction = iProduction * (100 + GET_PLAYER(getOwner()).GetGreatEngineerHurryMod()) / 100;
	
	return MAX(0, iProduction);
}

//	--------------------------------------------------------------------------------
int CvUnit::getHurryProduction(const CvPlot* pPlot) const
{
	CvCity* pCity = GET_PLAYER(getOwner()).getCity(m_iOriginCity);
	
	if (pCity == NULL)
		pCity = pPlot->getEffectiveOwningCity();

	if(pCity == NULL)
		return 0;

	return MAX(0, getMaxHurryProduction(pCity));
}

//	--------------------------------------------------------------------------------
bool CvUnit::hurry()
{
	CvCity* pCity = NULL;

	CvPlot* pPlot = plot();

	if(!canHurry(pPlot))
	{
		return false;
	}

	pCity = pPlot->getPlotCity();

	int iHurryValue = GetHurryStrength();

	iHurryValue = std::min(pCity->productionLeft(), iHurryValue);

	if (pCity != NULL)
	{
		pCity->changeProduction(iHurryValue);

		if (MOD_BALANCE_CORE_ENGINEER_HURRY && pCity->getProductionBuilding() != NO_BUILDING)
		{
			if (pCity->getProduction() >= pCity->getProductionNeeded())
			{
				pCity->popOrder(0, true, true);
			}
		}
	}

	if (pPlot->isActiveVisible())
	{
		//Achievement check
		if (MOD_API_ACHIEVEMENTS && pCity && pCity->getProductionBuilding() != NO_BUILDING)
		{
			CvBuildingEntry* pkProductionBuildinInfo = GC.getBuildingInfo(pCity->getProductionBuilding());
			if (pkProductionBuildinInfo)
			{
				if (isLimitedWonderClass(pkProductionBuildinInfo->GetBuildingClassInfo()))
				{
					if (pCity->isHuman() && !GC.getGame().isGameMultiPlayer())
					{
						const char* pLeaderChar = GET_PLAYER(pCity->getOwner()).getLeaderTypeKey();
						CvString szLeader = pLeaderChar;
						if (szLeader == "LEADER_RAMESSES")
							gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_SPHINX);
					}
				}
			}
		}
	}

	if(IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}


/// Great Merchant / Diplomat Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canTrade(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	if (GetGoldBlastStrength() == 0 && GetDiploMissionInfluence() == 0)
	{
		return false;
	}

	// Things that block usage but not visibility
	if(!bTestVisible)
	{
		// Not owned
		if(pPlot->getOwner() == NO_PLAYER)
			return false;

		// Owned by a non-minor civ
		if(!GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;

		if(GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getTradeGold() const
{
	VALIDATE_OBJECT
	int iGold = 0;
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	
	// Seed the gold value with some cash from the unit
	iGold = m_pUnitInfo->GetBaseGold();

	// Amount of Gold also increases with how far into the game we are
	if (pPlayer)
	{
		int iCurrentEra = pPlayer->GetCurrentEra();
		if (MOD_GP_ERA_SCALING)
		{
			int EraModifiers[6] = { 100, 200, 250, 400, 475, 575 };
			int iIndex = MAX(0, iCurrentEra - 2); // starts from renaissance
			iGold = iGold * EraModifiers[iIndex] / 100;
		}
		else
			iGold += (m_pUnitInfo->GetNumGoldPerEra() * iCurrentEra);

		// Add gold from empire gold per turn (does not scale with era)
		iGold += pPlayer->getYieldPerTurnHistory(YIELD_GOLD, m_pUnitInfo->GetBaseGoldTurnsToCount());
	}

	if (iGold == 0)
		return 0;

	// scale gold with the number of towns.
	if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
		iGold = GetScaleAmount(iGold);

	// scale with game speed
	iGold = iGold * GC.getGame().getGameSpeedInfo().getUnitTradePercent() / 100;
	// scale player policies
	iGold = iGold * (100 + GET_PLAYER(getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_TRADE_MISSION_GOLD_MODIFIER)) / 100;
	// scale with extra unit promotion etc.
	iGold = iGold * (100 + GetTradeMissionGoldModifier()) / 100;

	return MAX(0, iGold);
}

//	--------------------------------------------------------------------------------
int CvUnit::getTradeInfluence(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iInf = 0;
	if (pPlot && canTrade(pPlot))
	{
		PlayerTypes eMinor = pPlot->getOwner();
		if (GetDiploMissionInfluence() != 0)
		{
			iInf = GetDiploMissionInfluence();

			CvCity* pOriginCity = getOriginCity();
			if (!pOriginCity)
				pOriginCity = GET_PLAYER(getOwner()).getCapitalCity();

			if (pOriginCity)
				iInf += pOriginCity->GetDiplomatInfluenceBoost();
		}
		else
			iInf = /*30 in CP, 0 in VP*/ GD_INT_GET(MINOR_FRIENDSHIP_FROM_TRADE_MISSION);

		iInf += m_pUnitInfo->GetNumInfPerEra() * GET_TEAM(getTeam()).GetCurrentEra();

		iInf *= 100 + GetTradeMissionInfluenceModifier();
		iInf /= 100;

		iInf *= 100 + GET_PLAYER(getOwner()).GetMissionInfluenceModifier();
		iInf /= 100;

		if (GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(getOwner(), MINOR_CIV_QUEST_INFLUENCE))
		{
			iInf *= 100 + /*20*/ GD_INT_GET(INFLUENCE_MINOR_QUEST_BOOST);
			iInf /= 100;
		}
	}
	return iInf;
}

//	--------------------------------------------------------------------------------
bool CvUnit::trade()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if (!pPlot || !canTrade(pPlot))
		return false;

	// Improve relations with the Minor
	PlayerTypes eMinor = pPlot->getOwner();
	if (eMinor == NO_PLAYER || !GET_PLAYER(eMinor).isMinorCiv())
		return false;

	// Acquire Gold
	int iTradeGold = GetGoldBlastStrength();
	GET_PLAYER(getOwner()).GetTreasury()->ChangeGold(iTradeGold);

	// Acquire Influence
	int iInfluence = getTradeInfluence(pPlot);

	bool bGreatDiplomat = MOD_BALANCE_VP && m_pUnitInfo->GetNumInfPerEra() > 0 && m_pUnitInfo->GetRestingPointChange() != 0;

	// Save the original influence of every major civ for ally status change
	vector<int> viOriginalInfluence;
	viOriginalInfluence.reserve(MAX_MAJOR_CIVS);

	if (MOD_BALANCE_VP)
	{
		int iRestingPointChange = m_pUnitInfo->GetRestingPointChange();
		
		if (iRestingPointChange != 0)
		{
			GET_PLAYER(eMinor).GetMinorCivAI()->ChangeRestingPointChange(getOwner(), iRestingPointChange);
		}

		// Great Diplomat? Reduce everyone else's Influence and raise minimum Influence.
		if (bGreatDiplomat)
		{
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iPlayerLoop);
				viOriginalInfluence.push_back(0);
				if (GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).getTeam() != getTeam() && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(eMinor).getTeam()))
				{
					// Save the original influence
					viOriginalInfluence[iPlayerLoop] = GET_PLAYER(eMinor).GetMinorCivAI()->GetBaseFriendshipWithMajorTimes100(eLoopPlayer);

					// only reduce influence, but don't update ally status here. it could lead to unintended war declarations by the minor civ if their ally changes temporarily while looping through the players
					GET_PLAYER(eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(eLoopPlayer, -iInfluence, false, /*bUpdateStatus*/ false);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(getOwner(), 1);
					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if (pNotifications)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_OTHER");
						strText << getNameKey() << iInfluence << GET_PLAYER(eMinor).getNameKey() << GET_PLAYER(getOwner()).getCivilizationAdjectiveKey() << iRestingPointChange;
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_OTHER_SUMMARY");
						strSummary << getNameKey() << GET_PLAYER(eMinor).getNameKey();
						pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
					}
				}
			}
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT");
				strText << getNameKey() << iInfluence << GET_PLAYER(eMinor).getNameKey() << iRestingPointChange;
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_DIPLOMAT_SUMMARY");
				strSummary << getNameKey() << GET_PLAYER(eMinor).getNameKey();
				pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
			}
		}
	}

	// Great Merchant WLTKD bonus
	if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && m_pUnitInfo->GetBaseWLTKDTurns() > 0)
	{
		int iCap = m_pUnitInfo->GetBaseWLTKDTurns();

		//Let's make the GM a little more flexible.
		iCap = GetScaleAmount(iCap);
		iCap *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iCap /= 100;

		// Loop through owner's cities.
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(getOwner()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iCityLoop))
		{
			pLoopCity->ChangeWeLoveTheKingDayCounter(iCap);
		}
		if (GET_PLAYER(getOwner()).isHuman())
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_GREAT_MERCHANT");
				strText << getNameKey() << iCap;
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_GREAT_MERCHANT_SUMMARY");
				strSummary << getNameKey();
				pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), getUnitType());
			}
		}
	}

	GET_PLAYER(eMinor).GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iInfluence);

	// TODO: Add code here so only a unit with the right promotion triggers the accomplishment
	if (iInfluence > 0)
		GET_PLAYER(getOwner()).CompleteAccomplishment(ACCOMPLISHMENT_DIPLOMATIC_MISSION_BOOST);

	// Great Diplomat: now update the friend/ally status for the other players
	if (bGreatDiplomat)
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = static_cast<PlayerTypes>(iPlayerLoop);
			if (GET_PLAYER(eLoopPlayer).isMajorCiv() && GET_PLAYER(eLoopPlayer).getTeam() != getTeam() && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isHasMet(GET_PLAYER(eMinor).getTeam()))
			{
				int iFriendship = GET_PLAYER(eMinor).GetMinorCivAI()->GetBaseFriendshipWithMajor(eLoopPlayer);
				GET_PLAYER(eMinor).GetMinorCivAI()->DoFriendshipChangeEffects(eLoopPlayer, viOriginalInfluence[iPlayerLoop], iFriendship);
			}
		}
	}

	//This keeps minor civs in the black, financially.
	//GET_PLAYER(eMinor).GetTreasury()->ChangeGold((iInfluence / 2));

	if (getOwner() == GC.getGame().getActivePlayer())
	{
		if (iTradeGold == 0)
		{
			DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), GetLocalizedText("TXT_KEY_DIPLOMATIC_MISSION_RESULT_NO_GOLD", iInfluence));
		}
		else
		{
			DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), GetLocalizedText("TXT_KEY_MERCHANT_RESULT", iTradeGold, iInfluence));
		}
	}

	//there was a strange crash here where the unit suddenly was at an invalid plot
	if (pPlot->isActiveVisible() && plot()==pPlot)
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if (IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);
	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBuyCityState(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT

	if (isDelayedDeath())
		return false;

	if (!m_pUnitInfo->IsCanBuyCityState())
		return false;

	// Not allowed for humans in OCC games
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(getOwner()).isHuman())
		return false;

	// Things that block usage but not visibility
	if (!bTestVisible)
	{
		// If we don't have a capital, we can't purchase a city-state.
		if (GET_PLAYER(m_eOwner).getCapitalCity() == NULL)
			return false;

		// Not City-State territory
		if (pPlot->getOwner() == NO_PLAYER || !GET_PLAYER(pPlot->getOwner()).isMinorCiv())
			return false;

		// At war
		if (GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
			return false;

		if (MOD_EVENTS_MINORS_INTERACTION) 
		{
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBuyOut, m_eOwner, pPlot->getOwner()) == GAMEEVENTRETURN_FALSE)
				return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
// A Merchant of Venice buys a City-State
// CvUnit::buyCityState() is only ever called via CvTypes::getMISSION_BUY_CITY_STATE(), so this MUST be a "Merchant of Venice" type unit
bool CvUnit::buyCityState()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if (!canBuyCityState(pPlot))
		return false;

	PlayerTypes eMinor = pPlot->getOwner();

	// Perform the transfer
	GET_PLAYER(eMinor).GetMinorCivAI()->TransferUnitsAndCitiesToMajor(getOwner());

	if (getOwner() == GC.getGame().getActivePlayer())
	{
		DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), GetLocalizedText("TXT_KEY_VENETIAN_MERCHANT_BOUGHT_CITY_STATE"));
	}

	if (pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if (IsGreatPerson())
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}


/// Great Admiral Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canRepairFleet(const CvPlot* /*pPlot*/, bool /*bTestVisible*/) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	bool bHasSkill = !MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && IsGreatAdmiral();
	bHasSkill = bHasSkill || (MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL && m_pUnitInfo->IsCanRepairFleet());
	return bHasSkill;
}

//	--------------------------------------------------------------------------------
bool CvUnit::repairFleet()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canRepairFleet(pPlot))
		return false;

	// Do the repairing - first this hex
	int iUnitLoop = 0;
	for (iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
	{
		CvUnit *pUnit = pPlot->getUnitByIndex(iUnitLoop);
		if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
		{
			pUnit->changeDamage(-pUnit->getDamage());
		}
	}

	// And then all adjacent
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot *pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if(pAdjacentPlot != NULL)
		{
			for (iUnitLoop = 0; iUnitLoop < pAdjacentPlot->getNumUnits(); iUnitLoop++)
			{	
				CvUnit *pUnit = pAdjacentPlot->getUnitByIndex(iUnitLoop);
				if (pUnit->getOwner() == getOwner() && (pUnit->isEmbarked() || pUnit->getDomainType() == DOMAIN_SEA))
				{
					pUnit->changeDamage(-pUnit->getDamage());
				}
			}
		}
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	ChangeNumRepairCharges(-1);
	if (GetNumRepairCharges() <= 0)
	{
		if (IsGreatPerson())
		{
			CvPlayer& kPlayer = GET_PLAYER(getOwner());
			kPlayer.DoGreatPersonExpended(getUnitType(), this);
		}

		kill(true);
	}
	else
	{
		finishMoves();
	}

	return true;
}


/// Some Functions that let a unit create a spaceship part??
//	--------------------------------------------------------------------------------
/// Can this Unit build a spaceship part at pPlot??
bool CvUnit::CanBuildSpaceship(const CvPlot* pPlot, bool bVisible) const
{
	VALIDATE_OBJECT

	// Delayed death
	if(isDelayedDeath())
		return false;

	if(GAMESTATE_ON != GC.getGame().getGameState())
		return false;

	// Can this unit actually build a part?
	ProjectTypes eSpaceshipProject = (ProjectTypes) getUnitInfo().GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;

	// Things that block usage but not visibility
	if(!bVisible)
	{
		CvCity* pCity = pPlot->getPlotCity();

		// Not in a city
		if(pCity == NULL)
			return false;

		// Must be our city
		if(pCity->getOwner() != getOwner())
			return false;

		// Can only build SS part in our capital
		if(!pCity->isCapital())
			return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::DoBuildSpaceship()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!CanBuildSpaceship(pPlot, false))
		return false;

	CvCity* pCity = pPlot->getPlotCity();

	// If we're not in a city, bail out
	if(pCity == NULL)
		return false;

	// Can this unit actually build a part?
	ProjectTypes eSpaceshipProject = (ProjectTypes)m_pUnitInfo->GetSpaceshipProject();
	if(eSpaceshipProject == NO_PROJECT)
		return false;

	pCity->CreateProject(eSpaceshipProject);

	kill(true);

	return true;
}


/// Great General Functions (old Great Artist function)
//	--------------------------------------------------------------------------------
bool CvUnit::CanCultureBomb(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT

	if(m_pUnitInfo->GetCultureBombRadius() <= 0)
		return false;

	if(isDelayedDeath())
		return false;

	// Things we test for if we're going to perform this action RIGHT NOW
	if(!bTestVisible)
	{
		// Too soon?
		if(GET_PLAYER(getOwner()).getCultureBombTimer() > 0)
			return false;

		if(pPlot != NULL)
		{
			// Can't be inside someone else's territory
			if(pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != getOwner())
				return false;

			// We have to be in or next to friendly territory
			bool bFoundAdjacent = false;

			CvPlot* pLoopPlot = NULL;
			for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

				if(pLoopPlot != NULL)
				{
					if(pLoopPlot->getOwner() == getOwner())
					{
						bFoundAdjacent = true;
						break;
					}
				}
			}

			if(!bFoundAdjacent)
				return false;
		}
	}

	return true;
}
bool CvUnit::isCultureBomb() const
{
	VALIDATE_OBJECT
	return	m_pUnitInfo->GetCultureBombRadius() > 0;
}
//	--------------------------------------------------------------------------------
bool CvUnit::DoCultureBomb()
{
	// can we actually do this?
	CvPlot* pThisPlot = plot();
	if(!CanCultureBomb(pThisPlot))
		return false;

	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(getUnitType());
	if (pkUnitEntry)
	{
		// Cooldown
		int iCooldown = /*10*/ GD_INT_GET(CULTURE_BOMB_COOLDOWN);

		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
		kPlayer.changeCultureBombTimer(iCooldown);

		PerformCultureBomb(pkUnitEntry->GetCultureBombRadius() + GET_PLAYER(getOwner()).GetCultureBombBoost());

		if(pThisPlot->isActiveVisible())
		{
			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitActivate(pDllUnit.get());
		}

		setNumberOfCultureBombs(getNumberOfCultureBombs() - 1);
		if (getNumberOfCultureBombs() <= 0)
		{
			if (IsGreatPerson())
			{
				kPlayer.DoGreatPersonExpended(getUnitType(), this);
			}
			kill(true);
		}
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
//		this is for tile stealing, nowadays performed with citadels. doesn't have anything to do with culture anymore
//	--------------------------------------------------------------------------------
void CvUnit::PerformCultureBomb(int iRadius)
{
	CvPlot* pThisPlot = plot();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Figure out which City gets ownership of these plots
	int iBestCityID = -1;

	// Plot we're standing on belongs to a city already
	if(pThisPlot->getOwner() == getOwner() && pThisPlot->getOwningCityID() != -1)
	{
		iBestCityID = pThisPlot->getOwningCityID();
	}
	// Find closest city
	else
	{
		int iBestCityDistance = -1;
		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if(pPlot)
			{
				int iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

				if(iBestCityDistance == -1 || iDistance < iBestCityDistance)
				{
					iBestCityID = pLoopCity->GetID();
					iBestCityDistance = iDistance;
				}
			}
		}
	}

	// Keep track of got hit by this so we can figure the diplo ramifications later
	vector<bool> vePlayersBombed;
	vector<bool> vePlayersStoleHighValueTileFrom;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		vePlayersBombed.push_back(false);
		vePlayersStoleHighValueTileFrom.push_back(false);
	}

	// Change ownership of nearby plots
	int iBombRange = iRadius;
	CvPlot* pLoopPlot = NULL;
	for(int i = -iBombRange; i <= iBombRange; ++i)
	{
		for(int j = -iBombRange; j <= iBombRange; ++j)
		{
			pLoopPlot = ::plotXYWithRangeCheck(getX(), getY(), i, j, iBombRange);

			if (pLoopPlot == NULL)
				continue;

			// Can't flip Cities, sorry
			if (pLoopPlot->isCity())
				continue;

			PlayerTypes ePlotOwner = pLoopPlot->getOwner();

			// Can't be our plot
			if (ePlotOwner == getOwner())
				continue;

			// Can't steal blocked tiles
			if (pLoopPlot->IsStealBlockedByImprovement())
				continue;

			if (ePlotOwner != NO_PLAYER)
			{
				// Notify plot owner
				if (!vePlayersBombed[ePlotOwner])
				{
					CvNotifications* pNotifications = GET_PLAYER(pLoopPlot->getOwner()).GetNotifications();
					if (pNotifications)
					{
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
						pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), -1);
					}
				}
				vePlayersBombed[ePlotOwner] = true;

				bool bHighValueTile = false; // reference variable
				int iTileValue = pLoopPlot->GetStealPlotValue(getOwner(), bHighValueTile);
				if (bHighValueTile)
				{
					vePlayersStoleHighValueTileFrom[ePlotOwner] = true;
				}

				// Stole a major civ's embassy from a City-State?
				if (pLoopPlot->IsImprovementEmbassy() && GET_PLAYER(ePlotOwner).isMinorCiv())
				{
					PlayerTypes eEmbassyOwner = pLoopPlot->GetPlayerThatBuiltImprovement();
					if (GET_PLAYER(eEmbassyOwner).isAlive() && GET_PLAYER(eEmbassyOwner).isMajorCiv() && GET_PLAYER(eEmbassyOwner).getTeam() != GET_PLAYER(getOwner()).getTeam())
					{
						// Notify the embassy owner
						if (!vePlayersBombed[eEmbassyOwner])
						{
							CvNotifications* pNotifications = GET_PLAYER(eEmbassyOwner).GetNotifications();
							if (pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
								CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_ARTIST_STOLE_PLOT", GET_PLAYER(getOwner()).getNameKey());
								pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pLoopPlot->getX(), pLoopPlot->getY(), -1);
							}
						}
						vePlayersBombed[eEmbassyOwner] = true;
						vePlayersStoleHighValueTileFrom[eEmbassyOwner] = true;

						// Stole from the City-State's ally? The City-State is furious!
						if (GET_PLAYER(ePlotOwner).GetMinorCivAI()->GetAlly() == eEmbassyOwner)
						{
							GET_PLAYER(ePlotOwner).GetMinorCivAI()->SetFriendshipWithMajor(getOwner(), /*-60*/ GD_INT_GET(MINOR_FRIENDSHIP_AT_WAR));
						}
						// Stole from the City-State's friend and we're not their ally? Reset Influence to 0.
						else if (GET_PLAYER(ePlotOwner).GetMinorCivAI()->GetAlly() != getOwner() && GET_PLAYER(ePlotOwner).GetMinorCivAI()->IsFriends(eEmbassyOwner))
						{
							if (GET_PLAYER(ePlotOwner).GetMinorCivAI()->GetBaseFriendshipWithMajorTimes100(getOwner()) > 0)
							{
								GET_PLAYER(ePlotOwner).GetMinorCivAI()->SetFriendshipWithMajor(getOwner(), 0);
							}
						}
					}
				}

				// If the players are at war, this counts for war value!
				if (GET_PLAYER(getOwner()).IsAtWarWith(ePlotOwner))
				{
					GET_PLAYER(getOwner()).ApplyWarDamage(ePlotOwner, iTileValue);

					if (GET_PLAYER(getOwner()).isMajorCiv())
					{
						int iWarProgress = /*20*/ GD_INT_GET(WAR_PROGRESS_STOLE_TILE);
						if (bHighValueTile)
						{
							iWarProgress *= /*200*/ GD_INT_GET(WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER);
							iWarProgress /= 100;
						}
						GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeWarProgressScore(ePlotOwner, iWarProgress);
					}
					if (GET_PLAYER(ePlotOwner).isMajorCiv())
					{
						int iWarProgress = /*-10*/ GD_INT_GET(WAR_PROGRESS_LOST_TILE);
						if (bHighValueTile)
						{
							iWarProgress *= /*200*/ GD_INT_GET(WAR_PROGRESS_HIGH_VALUE_PILLAGE_MULTIPLIER);
							iWarProgress /= 100;
						}
						GET_PLAYER(getOwner()).GetDiplomacyAI()->ChangeWarProgressScore(ePlotOwner, iWarProgress);
					}
				}
			}

			// Instant yield from tiles gained by culture bombing
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes)iI;

				int iPassYield = 0;

				if (eYield == NO_YIELD)
					continue;

				TerrainTypes eTerrain = pLoopPlot->getTerrainType();

				if (eTerrain == NO_TERRAIN)
					continue;

				// Stole foreign tiles
				if (pLoopPlot->getOwner() != NO_PLAYER)
				{
					iPassYield += kPlayer.GetPlayerTraits()->GetYieldChangeFromTileStealCultureBomb(eTerrain, eYield);
				}
				// Obtained neutral tiles
				else
				{
					iPassYield += kPlayer.GetPlayerTraits()->GetYieldChangeFromTileCultureBomb(eTerrain, eYield);
				}

				CvCity* pBestCity = kPlayer.getCity(iBestCityID);
				if (pBestCity == NULL)
				{
					CvCity* pCapitalCity = kPlayer.getCapitalCity();
					if (pCapitalCity != NULL)
					{
						pBestCity = pCapitalCity;
					}
				}
				if (pBestCity != NULL)
				{
					kPlayer.doInstantYield(INSTANT_YIELD_TYPE_CULTURE_BOMB, false, NO_GREATPERSON, NO_BUILDING, iPassYield, true, NO_PLAYER, NULL, false, pBestCity, false, true, false, eYield);
				}
			}

			// Have to set owner after we do the above stuff
			pLoopPlot->setOwner(getOwner(), iBestCityID);
		}
	}

	bool bAlreadyShownLeader = false;

	// Now that we know who was hit, figure the diplo ramifications
	for (int iSlotLoop = 0; iSlotLoop < MAX_CIV_PLAYERS; iSlotLoop++)
	{
		if (vePlayersBombed[iSlotLoop])
		{
			CvPlayer* pPlayer = &GET_PLAYER((PlayerTypes) iSlotLoop);
			TeamTypes eOtherTeam = pPlayer->getTeam();
			
			if (pPlayer->isBarbarian())
				continue;

			if (!pPlayer->isAlive())
				continue;

			// Minor civ response
			if (pPlayer->isMinorCiv())
			{
				int iPenalty = vePlayersStoleHighValueTileFrom[iSlotLoop] ? /*-50*/ GD_INT_GET(CULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE) : /*-30*/ GD_INT_GET(CULTURE_BOMB_MINOR_FRIENDSHIP_CHANGE) * 3 / 5;

				if (MOD_BALANCE_CORE_MINORS)
				{
					int iEra = GET_PLAYER(getOwner()).GetCurrentEra();
					if (iEra <= 0)
						iEra = 1;

					iPenalty *= iEra;
				}

				pPlayer->GetMinorCivAI()->ChangeFriendshipWithMajor(getOwner(), iPenalty);
			}
			// Major civ response
			else if (pPlayer->isMajorCiv())
			{
				int iPenalty = vePlayersStoleHighValueTileFrom[iSlotLoop] ? 6 : 3;
				pPlayer->GetDiplomacyAI()->ChangeNumTimesCultureBombed(getOwner(), iPenalty);

				// Message for human
				if (!pPlayer->isHuman() && getTeam() != eOtherTeam && !GET_TEAM(eOtherTeam).isAtWar(getTeam()) && !CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == getOwner() && !bAlreadyShownLeader && !GC.getGame().IsInsultMessagesDisabled() && !GC.getGame().IsAllDiploStatementsDisabled())
				{
					bAlreadyShownLeader = true;

					DLLUI->SetForceDiscussionModeQuitOnBack(true);		// Set force quit so that when discuss mode pops up the Back button won't go to leader root
					const char* strText = pPlayer->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_CULTURE_BOMBED);
					gDLL->GameplayDiplomacyAILeaderMessage(pPlayer->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
				}
			}
		}
	}
}

int CvUnit::getNumberOfCultureBombs() const
{
	return m_iNumberOfCultureBombs;
}
void CvUnit::setNumberOfCultureBombs(const int iBombs)
{
	m_iNumberOfCultureBombs = iBombs;

	if (m_iNumberOfCultureBombs < 0)
		m_iNumberOfCultureBombs = 0;
}


/// Great Artist Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canGoldenAge(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!isGoldenAge() && GetGAPBlastStrength() <= 0)
	{
		return false;
	}

	// If prophet has started spreading religion, can't do other functions
	if (m_pUnitInfo->IsSpreadReligion() && GetReligionData()->GetSpreadsUsed()>0)
	{
		return false;
	}

	// Things we test ONLY when we're going to try to perform this action RIGHT NOW
	if(!bTestVisible)
	{
		// Must be in player's territory
		if(pPlot->getOwner() != getOwner())
			return false;
	}

	return true;
}

int CvUnit::getGAPBlast()
{
	int iValue = 0;
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	CvAssertMsg(pPlayer, "Owner of unit not expected to be NULL. Please send Anton your save file and version.");
	if (pPlayer)
	{
		// boost based on previous turns
		int iPreviousTurnsToCount = m_pUnitInfo->GetBaseTurnsForGAPToCount();
		if (iPreviousTurnsToCount == 0)
			return 0;

		iValue = pPlayer->getYieldPerTurnHistory(YIELD_TOURISM, iPreviousTurnsToCount);
		iValue += pPlayer->getYieldPerTurnHistory(YIELD_GOLDEN_AGE_POINTS, iPreviousTurnsToCount);

		//GA Mod
		if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && getUnitInfo().GetScaleFromNumThemes() > 0)
		{
			int iTotalThemes = 0;
			int iCityLoop = 0;
			// Loop through owner's cities.
			for (CvCity* pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
			{
				if (pLoopCity != NULL)
				{
					iTotalThemes += pLoopCity->GetCityBuildings()->GetTotalNumThemedBuildings();
				}
			}

			iTotalThemes = (iTotalThemes * getUnitInfo().GetScaleFromNumThemes());
			iValue *= (iTotalThemes + 100);
			iValue /= 100;
		}

		if (iValue <= /*250 in CP, 200 in VP*/ GD_INT_GET(GOLDEN_AGE_BASE_THRESHOLD_HAPPINESS) /2)
			iValue = GD_INT_GET(GOLDEN_AGE_BASE_THRESHOLD_HAPPINESS) / 2;

		// Modify based on game speed
		iValue *= GC.getGame().getGameSpeedInfo().getInstantYieldPercent();
		iValue /= 100;
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getGoldenAgeTurns() const
{
	int iGoldenAgeTurns = m_pUnitInfo->GetGoldenAgeTurns();

	// Unit cannot perform a GA
	if(iGoldenAgeTurns == 0)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iMinTurns = /*3*/ GD_INT_GET(MIN_UNIT_GOLDEN_AGE_TURNS);
	if(iGoldenAgeTurns < iMinTurns)
		iGoldenAgeTurns = iMinTurns;

	// Player mod
	int iLengthModifier = kPlayer.getGoldenAgeModifier();

	if(iLengthModifier > 0)
		iGoldenAgeTurns = iGoldenAgeTurns * (100 + iLengthModifier) / 100;

	//GA Mod
	if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && getUnitInfo().GetScaleFromNumThemes() > 0)
	{
		int iTotalThemes = 0;
		int iCityLoop = 0;
		// Loop through owner's cities.
		for (CvCity* pLoopCity = kPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iCityLoop))
		{
			if (pLoopCity != NULL)
			{
				iTotalThemes += pLoopCity->GetCityBuildings()->GetTotalNumThemedBuildings();
			}
		}

		iTotalThemes = (iTotalThemes * getUnitInfo().GetScaleFromNumThemes());
		iGoldenAgeTurns *= (iTotalThemes + 100);
		iGoldenAgeTurns /= 100;
	}

	// Game Speed mod
	iGoldenAgeTurns = GC.getGame().goldenAgeLength(iGoldenAgeTurns);

	if(iGoldenAgeTurns < 1)
		iGoldenAgeTurns = 1;

	return iGoldenAgeTurns;
}

//	--------------------------------------------------------------------------------
bool CvUnit::goldenAge()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canGoldenAge(pPlot))
	{
		return false;
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	if (GetGAPBlastStrength() > 0)
	{
		kPlayer.doInstantYield(INSTANT_YIELD_TYPE_INSTANT, false, NO_GREATPERSON, NO_BUILDING, GetGAPBlastStrength(), false, NO_PLAYER, plot(), false, getOriginCity(), false, true, true, YIELD_GOLDEN_AGE_POINTS);
	}


	int iGoldenAgeTurns = getGoldenAgeTurns();
	if (iGoldenAgeTurns > 0)
	{
		kPlayer.changeGoldenAgeTurns(iGoldenAgeTurns, true);
		kPlayer.changeNumUnitGoldenAges(1);
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}


/// Great Writer Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canGivePolicies(const CvPlot* /*pPlot*/, bool /*bTestVisible*/) const
{
	VALIDATE_OBJECT
	if(!isGivesPolicies() && getUnitInfo().GetBaseCultureTurnsToCount() == 0)
	{
		return false;
	}

	// If prophet has started spreading religion, can't do other functions
	if (m_pUnitInfo->IsSpreadReligion() && GetReligionData()->GetSpreadsUsed()>0)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::getGivePoliciesCulture()
{
	int iValue = 0;
#if !defined(MOD_BALANCE_CORE)
	CvPlot* pPlot = plot();
	if(canGivePolicies(pPlot))
#endif
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

		// Culture boost based on previous turns
		int iPreviousTurnsToCount = getUnitInfo().GetBaseCultureTurnsToCount();
		if (iPreviousTurnsToCount != 0)
		{
			// Calculate boost
			iValue = kPlayer.getYieldPerTurnHistory(YIELD_CULTURE, iPreviousTurnsToCount);
		}

		if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && getUnitInfo().GetScaleFromNumGWs() > 0)
		{
			int iNumGreatWorks = kPlayer.GetCulture()->GetNumGreatWorks();
			iNumGreatWorks = (iNumGreatWorks * getUnitInfo().GetScaleFromNumGWs());
			iValue *= (iNumGreatWorks + 100);
			iValue /= 100;
		}

		// Modify based on game speed
		iValue *= GC.getGame().getGameSpeedInfo().getCulturePercent();
		iValue /= 100;
	}
	return iValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::givePolicies()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();
	if(!canGivePolicies(pPlot))
	{
		return false;
	}
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Culture boost based on previous turns
	int iCultureBonus = GetCultureBlastStrength();
	if (iCultureBonus != 0)
	{
		kPlayer.changeJONSCulture(iCultureBonus);
		if (pPlot->getOwningCity() && pPlot->getOwner() == getOwner())
			pPlot->getOwningCity()->ChangeJONSCultureStored(iCultureBonus);

		// Refresh - we might get to pick a policy this turn
	}

	// Free policies
	int iFreePolicies = m_pUnitInfo->GetFreePolicies();
	if (iFreePolicies != 0)
	{
		kPlayer.ChangeNumFreePolicies(iFreePolicies);
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		kPlayer.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);

	return true;
}


/// Great Musician Functions
//	--------------------------------------------------------------------------------
bool CvUnit::canBlastTourism(const CvPlot* pPlot, bool bTestVisible) const
{
	VALIDATE_OBJECT
	if(!isBlastTourism())
	{
		return false;
	}

	//If we're just testing capability, then yes.
	if(bTestVisible)
	{
		return true;
	}

	// Must be a tile owned by another major civilization
	PlayerTypes eOwner = pPlot->getOwner();
	if (eOwner == NO_PLAYER)
	{
		return false;
	}

#if defined(MOD_BALANCE_CORE)
	//No tourism while at war!
	if (MOD_BALANCE_CORE_VICTORY_GAME_CHANGES && GET_TEAM(GET_PLAYER(eOwner).getTeam()).isAtWar(getTeam()))
	{
		return false;
	}

	//No blasts while influential or better!
	InfluenceLevelTypes eLevel = GET_PLAYER(getOwner()).GetCulture()->GetInfluenceLevel(eOwner);
	if (eLevel >= INFLUENCE_LEVEL_INFLUENTIAL)
	{
		return false;
	}
#endif

	CvPlayer &kTileOwner = GET_PLAYER(eOwner);
	return kTileOwner.isAlive() && !kTileOwner.isMinorCiv() && eOwner != getOwner();
}

//	--------------------------------------------------------------------------------
int CvUnit::getBlastTourism()
{
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	if (!pPlayer)
		return 0;

	if (!canBlastTourism(plot()))
	{
		return 0;
	}

	// Get base multiplier from unit
	int iMultiplier = getUnitInfo().GetOneShotTourism();

	if (iMultiplier <= 0)
		return 0;

	int iTourism = 0;
	
	if(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
	{
		// Tourism boost based on previous turns (no base value)
		iTourism = pPlayer->getYieldPerTurnHistory(YIELD_TOURISM, iMultiplier);

		// scale with number of great works
		CvCity *pLoopCity = NULL;
		int iLoop = 0;
		for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
		{
			iMultiplier += pLoopCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC());
		}
	}
	else
	{
		iTourism = iMultiplier * pPlayer->GetCulture()->GetTourism() / 100;
	}

	// Scale by game speed
	iTourism *= GC.getGame().getGameSpeedInfo().getCulturePercent();
	iTourism /= 100;

	return max(iTourism, /*100*/ GD_INT_GET(MINIMUM_TOURISM_BLAST_STRENGTH));
}

int CvUnit::getBlastTourismTurns()
{
	// Get Base number of turns from unit
	int iNumTurns = m_pUnitInfo->GetTourismBonusTurns();

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());
	if (pPlayer)
	{
		// scale with number of great works of music
		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
		{
			iNumTurns += pLoopCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC());
		}
	}

	// scale with game speed
	iNumTurns *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iNumTurns /= 100;

	return iNumTurns;
}
//	--------------------------------------------------------------------------------
bool CvUnit::blastTourism()
{
	VALIDATE_OBJECT

	CvPlot* pPlot = plot();

	if(!canBlastTourism(pPlot))
	{
		return false;
	}

	PlayerTypes eOwner = pPlot->getOwner();
	CvPlayer &kUnitOwner = GET_PLAYER(getOwner());
	int iTourismBlastAfterModifier = 0;

	if (GetTourismBlastLength() > 0)
	{
		kUnitOwner.ChangeTourismBonusTurns(GetTourismBlastLength());

		PlayerTypes eOwner = pPlot->getOwner();
		kUnitOwner.changeTourismBonusTurnsPlayer(eOwner, GetTourismBlastLength());

		// VP: Give Happiness to Musician owner
		int iCap = /*0*/ GD_INT_GET(GREAT_MUSICIAN_BLAST_HAPPINESS_CAPITAL);
		if (iCap > 0 && kUnitOwner.getCapitalCity() != NULL)
			kUnitOwner.getCapitalCity()->ChangeUnmoddedHappinessFromBuildings(iCap);

		iCap = /*0 in CP, 1 in VP*/ GD_INT_GET(GREAT_MUSICIAN_BLAST_HAPPINESS_GLOBAL);
		if (iCap > 0)
		{
			int iLoop = 0;
			for (CvCity* pLoopCity = kUnitOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kUnitOwner.nextCity(&iLoop))
			{
				pLoopCity->ChangeUnmoddedHappinessFromBuildings(iCap);
			}
		}

		// Send notifications
		CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
		if (pNotifications)
		{
			Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_TOUR_TURNS");
			localizedText << GET_PLAYER(eOwner).getCivilizationAdjectiveKey();
			localizedText << GetTourismBlastLength();
			Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_TOUR_TURNS_S");
			localizedSummary << GET_PLAYER(eOwner).getCivilizationAdjectiveKey();
			pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, localizedText.toUTF8(), localizedSummary.toUTF8(), getX(), getY(), getUnitType());
		}
		CvNotifications* pNotifications2 = GET_PLAYER(eOwner).GetNotifications();
		if (pNotifications2)
		{
			Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_TOUR_TURNS_TARGET");
			localizedText << kUnitOwner.getCivilizationAdjectiveKey();
			localizedText << kUnitOwner.getCivilizationShortDescriptionKey();
			localizedText << GetTourismBlastLength();
			Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_TOUR_TURNS_TARGET_S");
			localizedSummary << kUnitOwner.getCivilizationAdjectiveKey();
			pNotifications2->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, localizedText.toUTF8(), localizedSummary.toUTF8(), getX(), getY(), eOwner);
		}
	}

	int iTourismBlast = GetTourismBlastStrength();
	if (iTourismBlast > 0)
	{
		int iTourismBlastPercentOthers = m_pUnitInfo->GetOneShotTourismPercentOthers();
		// Apply to target, then save what is the amount we actually applied
		iTourismBlastAfterModifier = kUnitOwner.GetCulture()->ChangeInfluenceOn(eOwner, iTourismBlast, true, true);

#if defined(MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
		//Let's make the GM a little more flexible.
		if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES)
		{
			PlayerTypes eOwner = pPlot->getOwner();

			// Give happiness to Musician owner
			int iCap = /*2*/ GD_INT_GET(GREAT_MUSICIAN_BLAST_HAPPINESS_CAPITAL);

			if (GET_PLAYER(getOwner()).getCapitalCity() != NULL)
			{
				GET_PLAYER(getOwner()).getCapitalCity()->ChangeUnmoddedHappinessFromBuildings(iCap);
			}

			// Send notifications
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if (pNotifications)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_FAMILIAR_TOUR");
				localizedText << GET_PLAYER(eOwner).getCivilizationAdjectiveKey();
				localizedText << iTourismBlastAfterModifier;
				localizedText << iCap;
				Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_FAMILIAR_TOUR_S");
				localizedSummary << GET_PLAYER(eOwner).getCivilizationAdjectiveKey();
				pNotifications->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, localizedText.toUTF8(), localizedSummary.toUTF8(), getX(), getY(), getUnitType());
			}
			CvNotifications* pNotifications2 = GET_PLAYER(eOwner).GetNotifications();
			if (pNotifications2)
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_FAMILIAR_TOUR_TARGET");
				localizedText << GET_PLAYER(getOwner()).getCivilizationAdjectiveKey();
				localizedText << iTourismBlastAfterModifier;
				Localization::String localizedSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_MUSICIAN_FAMILIAR_TOUR_TARGET_S");
				localizedSummary << GET_PLAYER(getOwner()).getCivilizationAdjectiveKey();
				pNotifications2->Add(NOTIFICATION_CULTURE_VICTORY_SOMEONE_INFLUENTIAL, localizedText.toUTF8(), localizedSummary.toUTF8(), getX(), getY(), eOwner);
			}
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Foreign Tour, Familiar. Tourism gained: %d. Happiness they gained: %d", iTourismBlastAfterModifier, iCap);
				GET_PLAYER(getOwner()).GetHomelandAI()->LogHomelandMessage(strLogString);
			}
		}
#endif

		//store off this data
		GET_PLAYER(getOwner()).changeInstantTourismPerPlayerValue(eOwner, iTourismBlast);

		// Apply lesser amount to other civs
		int iTourismBlastOthers = iTourismBlast * iTourismBlastPercentOthers / 100;
		PlayerTypes eLoopPlayer;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes)iPlayerLoop;

			if (eLoopPlayer != eOwner && eLoopPlayer != getOwner() && kUnitOwner.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				kUnitOwner.GetCulture()->ChangeInfluenceOn(eLoopPlayer, iTourismBlastOthers, true, true);
			}
		}
	}

	if(pPlot->isActiveVisible())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitActivate(pDllUnit.get());
	}

	if(IsGreatPerson())
	{
		kUnitOwner.DoGreatPersonExpended(getUnitType(), this);
	}

	kill(true);
 
	// Show tourism spread
	if (iTourismBlastAfterModifier > 0 && pPlot->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF)
	{
		InfluenceLevelTypes eLevel = kUnitOwner.GetCulture()->GetInfluenceLevel(eOwner);
		SHOW_PLOT_POPUP(pPlot, getOwner(), CultureHelpers::GetInfluenceText(eLevel, iTourismBlastAfterModifier).c_str());
	}

	// Achievements
	if (MOD_API_ACHIEVEMENTS && GET_PLAYER(m_eOwner).isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		if (strcmp(GET_PLAYER(eOwner).getCivilizationTypeKey(),   "CIVILIZATION_AMERICA") == 0 &&
			strcmp(GET_PLAYER(m_eOwner).getCivilizationTypeKey(), "CIVILIZATION_ENGLAND") == 0)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_36);
		}

		ImprovementTypes eTempImprovement = pPlot->getImprovementType();
		if (eTempImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovement = GC.getImprovementInfo(eTempImprovement);
			if (strcmp(pkImprovement->GetType(), "IMPROVEMENT_KASBAH") == 0)
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_37);
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible, bool bTestGold, bool bTestEra) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eBuild < GC.getNumBuildInfos() && eBuild >= 0, "Index out of bounds");

	if (MOD_CIV6_WORKER && getBuilderStrength() <= 0)
		return false;

	if (!GET_PLAYER(getOwner()).GetPlayerTraits()->HasUnitClassCanBuild(eBuild, getUnitClassType()) && (!m_pUnitInfo->GetBuilds(eBuild) || GET_PLAYER(getOwner()).GetPlayerTraits()->IsNoBuild(eBuild)))
		return false;

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if (!pkBuildInfo)
		return false;

	// If prophet has started spreading religion, can't do other functions
	if (m_pUnitInfo->IsSpreadReligion() && GetReligionData()->GetSpreadsUsed() > 0)
		return false;

	if (!pPlot)
		return true;

	if (!(GET_PLAYER(getOwner()).canBuild(pPlot, eBuild, bTestEra, bTestVisible, bTestGold, true, this)))
		return false;

	bool bValidBuildPlot = isNativeDomain(pPlot);

	bValidBuildPlot |= (pkBuildInfo->IsWater() && getDomainType() == DOMAIN_LAND && pPlot->isWater() && IsHasEmbarkAbility());

	if(!bValidBuildPlot)
		return false;

	if (pPlot->isWater())
	{
		// Needs the associated SQL executing - UPDATE Builds SET CanBeEmbarked=1 WHERE Type='BUILD_FISHING_BOATS_NO_KILL';
		if (pPlot->needsEmbarkation(this) && !pkBuildInfo->IsCanBeEmbarked())
		{
			return false;
		}
	}

	if(!bTestVisible)
	{
		// check for any other units working in this plot
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		const CvUnit* pLoopUnit = NULL;

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit && pLoopUnit != this)
			{
				if(pLoopUnit->IsWork() && pLoopUnit->IsWorking())
				{
					return false;
				}
			}
		}
	}

	if (MOD_EVENTS_PLOT)
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanBuild, getOwner(), GetID(), getX(), getY(), eBuild) == GAMEEVENTRETURN_FALSE)
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
// Returns true if build finished...
bool CvUnit::build(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	bool bFinished = false;

	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");
	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	CvBuildInfo* pkBuildInfo = NULL;
	if(eBuild != NO_BUILD)
	{
		pkBuildInfo = GC.getBuildInfo(eBuild);
	}
	CvPlot* pPlot = plot();

	// Don't test Gold
	if(!canBuild(pPlot, eBuild, false, false))
	{
		return false;
	}

	if(GC.getLogging() && GC.getAILogging())
	{
		if(pPlot->getBuildProgress(eBuild) == 0)
		{
			LogWorkerEvent(eBuild, true);
		}
	}

	int iWorkRateWithMoves = workRate(false);

	int iStartedYet = pPlot->getBuildProgress(eBuild);

	bool NewBuild = false;
	// if we are starting something new wipe out the old thing immediately
	if(iStartedYet == 0)
	{
		ImprovementTypes eImprovement = NO_IMPROVEMENT;

		if(pkBuildInfo)
		{
			eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
		}

		if(eImprovement != NO_IMPROVEMENT)
		{
			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				CvImprovementEntry *pkImprovementEntry = GC.getImprovementInfo(eImprovement);
				if (!pkImprovementEntry || !pkImprovementEntry->IsRequiresImprovement())
				{
					pPlot->setImprovementType(NO_IMPROVEMENT);
				}
			}
		}

		NewBuild = true;
	}

	if (!MOD_CIV6_WORKER)
		bFinished = pPlot->changeBuildProgress(eBuild, iWorkRateWithMoves, getOwner(), NewBuild);

	if (MOD_EVENTS_PLOT)
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBuilding, getOwner(), GetID(), getX(), getY(), eBuild, (iStartedYet == 0));

	finishMoves(); // needs to be at bottom because movesLeft() can affect workRate()...

	if(bFinished)
	{
		// Update Resource info
		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if (pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();

				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if (pkImprovementInfo)
				{
					if (pkImprovementInfo->GetCultureBombRadius() > 0)
					{
						int iCultureBomb = pkImprovementInfo->GetCultureBombRadius();
						iCultureBomb += GET_PLAYER(getOwner()).GetCultureBombBoost();

						PerformCultureBomb(iCultureBomb);
					}
				}

				if (pkBuildInfo->IsFreeBestDomainUnit() && kPlayer.GetNumUnitsToSupply() > kPlayer.GetNumUnitsSupplied())
				{
					bool bWater = pkBuildInfo->IsWater();
					UnitTypes eUnit = NO_UNIT;
					if (bWater)
					{
						vector<int> viUnitCombat;
						viUnitCombat.push_back(GC.getInfoTypeForString("UNITCOMBAT_NAVALMELEE"));
						eUnit = kPlayer.GetCompetitiveSpawnUnitType(false, true, true, true, NULL, false, false, false, NULL, viUnitCombat);
					}
					else
					{
						eUnit = kPlayer.GetCompetitiveSpawnUnitType(false, false, true, true);
					}

					if (eUnit != NO_UNIT)
					{
						CvUnit* pUnit = kPlayer.initUnit(eUnit, pPlot->getX(), pPlot->getY());
						kPlayer.getCapitalCity()->addProductionExperience(pUnit);
						if (!pUnit->jumpToNearestValidPlot())
							pUnit->kill(false);
					}
				}

				if (pkBuildInfo->IsCultureBoost())
				{
					int iValue = kPlayer.GetTotalJONSCulturePerTurn() * 2;
					kPlayer.changeJONSCulture(iValue);
					if (kPlayer.getCapitalCity() != NULL)
						kPlayer.getCapitalCity()->ChangeJONSCultureStored(iValue);

					if (kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iValue);
						SHOW_PLOT_POPUP(plot(),kPlayer.GetID(), text);
					}
				}
			}

			// Can be both an improvement and a route
			if (pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = static_cast<RouteTypes>(pkBuildInfo->getRoute());
			}

			if(pkBuildInfo->isKill())
			{
				if (pPlot->isActiveVisible())
				{
					CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
					gDLL->GameplayUnitActivate(pDllUnit.get());
				}

				bool bIndiaException = false;
				if (m_pUnitInfo->IsFoundReligion() && GET_PLAYER(getOwner()).GetPlayerTraits()->IsProphetFervor())
				{
					GetReligionDataMutable()->IncrementSpreadsUsed();

					// Test for > 1 total spreads here, not spreads left, because it's possible to create a Holy Site before founding a religion
					if (getUnitInfo().GetReligionSpreads() > 1)
						bIndiaException = true;

					if (bIndiaException && pPlot->isActiveVisible())
					{
						// Because the "Activate" animation will possibly put the animation state into a end-state, we will force a reset, since the unit will still be alive
						CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
						gDLL->GameplayUnitResetAnimationState(pDllUnit.get());
					}
				}

				if (!bIndiaException)
				{
					if (IsGreatPerson())
					{
						if (MOD_CIV6_WORKER && getBuilderStrength() > 0)
						{
							int iBuildCost = pkBuildInfo->getBuilderCost();
							setBuilderStrength(getBuilderStrength() - iBuildCost);
							if (getBuilderStrength() <= 0)
							{
								kPlayer.DoGreatPersonExpended(getUnitType(), this);
								kill(true);
							}
						}
						else
						{
							kPlayer.DoGreatPersonExpended(getUnitType(), this);
						}
					}

					if (MOD_CIV6_WORKER)
					{
						if ((!pkBuildInfo->isKillOnlyCivilian() && !IsGreatPerson()) || (pkBuildInfo->isKillOnlyCivilian() && IsCivilianUnit() && !IsGreatPerson()))
						{
							kill(true);
						}
					}
					else
					{
						if (!pkBuildInfo->isKillOnlyCivilian() || (pkBuildInfo->isKillOnlyCivilian() && IsCivilianUnit()))
						{
							kill(true);
						}
					}
				}
			}

			//if we are a builder (something with builderstrength)
			if (MOD_CIV6_WORKER && getBuilderStrength() > 0)
			{
				if ((AI_getUnitAIType() == UNITAI_WORKER_SEA) || (AI_getUnitAIType() == UNITAI_WORKER) || IsCombatUnit())
				{
					//check the amount of work done
					int iBuildCost = pkBuildInfo->getBuilderCost();
					// remove this amount (and kill me if it's too high)
					setBuilderStrength(getBuilderStrength() - iBuildCost);
					if (getBuilderStrength() <= 0 && !IsCombatUnit())
					{
						//delete unit if it's not a combat unit
						kill(true);
					}
				}
			}

			// invalidate trade paths because plot was changed and it can affect trade route length
			GC.getGame().GetGameTrade()->InvalidateTradePathCache();

			if (eImprovement != NO_IMPROVEMENT)
			{
				kPlayer.changeTotalImprovementsBuilt(1);
				kPlayer.changeTotalImprovementsBuilt(eImprovement, 1);
			}
			else if ((RouteTypes)pkBuildInfo->getRoute() != NO_ROUTE)
			{
				kPlayer.changeTotalImprovementsBuilt(1); // This is here even though routes are not technically improvements, because Firaxis put this here (don't change behaviour of existing tables!)
			}

			if(GC.getLogging() && GC.getAILogging())
			{
				LogWorkerEvent(eBuild, false);
			}
		}


		if (MOD_EVENTS_PLOT)
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_PlayerBuilt, getOwner(), GetID(), getX(), getY(), eBuild);
	}
	else // we are not done doing this
	{
		if(pkBuildInfo)
		{
			ImprovementTypes eImprovement = NO_IMPROVEMENT;
			RouteTypes eRoute = NO_ROUTE;

			if(pkBuildInfo->getImprovement() != NO_IMPROVEMENT)
			{
				eImprovement = (ImprovementTypes) pkBuildInfo->getImprovement();
			}
			else if(pkBuildInfo->getRoute() != NO_ROUTE)
			{
				eRoute = (RouteTypes) pkBuildInfo->getRoute();
			}

			if(iStartedYet == 0)
			{
				if(pPlot->isVisible(GC.getGame().getActiveTeam(), true))
				{
					if(eImprovement != NO_IMPROVEMENT)
					{
						pPlot->setLayoutDirty(true);

					}
					else if(eRoute != NO_ROUTE)
					{
						pPlot->setLayoutDirty(true);
					}
				}
			}
		}
	}

	return bFinished;
}

//	--------------------------------------------------------------------------------
int CvUnit::getBuilderStrength() const
{
	return m_iBuilderStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::setBuilderStrength(const int newPower)
{
	m_iBuilderStrength = newPower;
	if (m_iBuilderStrength < 0) m_iBuilderStrength = 0;

}


//	--------------------------------------------------------------------------------
bool CvUnit::canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const
{
	VALIDATE_OBJECT
	if(iLeaderUnitId >= 0)
	{
		if(iLeaderUnitId == GetID())
		{
			return false;
		}

		// The command is always possible if it's coming from a Warlord unit that gives just experience points
		CvUnit* pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord &&
				NO_UNIT != pWarlord->getUnitType() &&
				pWarlord->getUnitInfo().GetLeaderExperience() > 0 &&
				NO_PROMOTION == pWarlord->getUnitInfo().GetLeaderPromotion() &&
				canAcquirePromotionAny())
		{
			return true;
		}
	}

	if(ePromotion == NO_PROMOTION)
	{
		return false;
	}

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}

	if(!canAcquirePromotion(ePromotion))
	{
		return false;
	}

	if(pkPromotionInfo->IsLeader())
	{
		if(iLeaderUnitId >= 0)
		{
			CvUnit* pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
			if(pWarlord && NO_UNIT != pWarlord->getUnitType())
			{
				return (pWarlord->getUnitInfo().GetLeaderPromotion() == ePromotion);
			}
		}
		return false;
	}
	else
	{
		if(!isPromotionReady())
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
void CvUnit::promote(PromotionTypes ePromotion, int iLeaderUnitId)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return;
	}

	if(!canPromote(ePromotion, iLeaderUnitId))
	{
		return;
	}

	if(iLeaderUnitId >= 0)
	{
		CvUnit* pWarlord = GET_PLAYER(getOwner()).getUnit(iLeaderUnitId);
		if(pWarlord)
		{
			pWarlord->giveExperience();
			if(!pWarlord->getNameNoDesc().empty())
			{
				setName(pWarlord->getName());
			}

			//update graphics models
			m_eLeaderUnitType = pWarlord->getUnitType();
		}
	}

	if(!pkPromotionInfo->IsLeader())
	{
		changeLevel(1);
	}

	// Insta-Heal: never earned
	if (pkPromotionInfo->IsInstaHeal())
	{
		changeDamage(/*-50*/ -GD_INT_GET(INSTA_HEAL_RATE));
	}
	// Set that we have this Promotion
	else
	{
		setHasPromotion(ePromotion, true);

		if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		{
			//Insta-heal removed, gain health with each promotion instead.
			changeDamage(/*-10*/ -GD_INT_GET(INSTA_HEAL_RATE) / 5);
		}
		if (getOriginCity() != NULL)
		{
			GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_LEVEL_UP, false, NO_GREATPERSON, NO_BUILDING, (getLevel() - 1), false, NO_PLAYER, NULL, false, getOriginCity(), getDomainType()==DOMAIN_SEA, true, false, NO_YIELD, this);
		}
		else if (getOriginCity() == NULL)
		{
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			if (pCapital)
			{
				GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_LEVEL_UP, false, NO_GREATPERSON, NO_BUILDING, (getLevel() - 1), false, NO_PLAYER, NULL, false, pCapital, getDomainType()==DOMAIN_SEA, true, false, NO_YIELD, this);
			}
		}

		if (MOD_EVENTS_UNIT_UPGRADES)
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPromoted, getOwner(), GetID(), ePromotion);
		}
		else
		{
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem)
			{
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(ePromotion);

				bool bResult = false;
				LuaSupport::CallHook(pkScriptSystem, "UnitPromoted", args.get(), bResult);
			}
		}
	}

	testPromotionReady();

	if(IsSelected())
	{
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else
	{
		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::lead(int iUnitId)
{
	VALIDATE_OBJECT
	if(!canLead(plot(), iUnitId))
	{
		return false;
	}

	PromotionTypes eLeaderPromotion = (PromotionTypes)m_pUnitInfo->GetLeaderPromotion();

	if(-1 == iUnitId)
	{
		return false;
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);

		if(!pUnit || !pUnit->canPromote(eLeaderPromotion, GetID()))
		{
			return false;
		}

		pUnit->promote(eLeaderPromotion, GetID());

		kill(true);

		return true;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::canLead(const CvPlot* pPlot, int iUnitId) const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return 0;
	}

	if(NO_UNIT == getUnitType())
	{
		return 0;
	}

	int iNumUnits = 0;
	CvUnitEntry& kUnitInfo = getUnitInfo();

	if(-1 == iUnitId)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
			{
				++iNumUnits;
			}
		}
	}
	else
	{
		const CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(iUnitId);
		if(pUnit && pUnit != this && pUnit->canPromote((PromotionTypes)kUnitInfo.GetLeaderPromotion(), GetID()))
		{
			iNumUnits = 1;
		}
	}
	return iNumUnits;
}


//	--------------------------------------------------------------------------------
int CvUnit::canGiveExperience(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	int iNumUnits = 0;

	if(NO_UNIT != getUnitType() && getUnitInfo().GetLeaderExperience() > 0)
	{
		const IDInfo* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			const CvUnit* pUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
			{
				++iNumUnits;
			}
		}
	}

	return iNumUnits;
}

//	--------------------------------------------------------------------------------
bool CvUnit::giveExperience()
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();

	if(pPlot)
	{
		int iNumUnits = canGiveExperience(pPlot);
		if(iNumUnits > 0)
		{
			int iTotalExperience = getStackExperienceToGive(iNumUnits);

			int iMinExperiencePerUnit = iTotalExperience / iNumUnits;
			int iRemainder = iTotalExperience % iNumUnits;

			IDInfo* pUnitNode = pPlot->headUnitNode();
			int i = 0;
			while(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if(pUnit && pUnit != this && pUnit->getOwner() == getOwner() && pUnit->canAcquirePromotionAny())
				{
					pUnit->changeExperienceTimes100(100 * (i < iRemainder ? iMinExperiencePerUnit + 1 : iMinExperiencePerUnit));
					pUnit->testPromotionReady();
				}

				i++;
			}

			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	VALIDATE_OBJECT
	return (getUnitInfo().GetLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * /*0*/ GD_INT_GET(WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT)))) / 100;
}

//	--------------------------------------------------------------------------------
/// Is the Unit currently in a state that allows it to upgrade?
bool CvUnit::isReadyForUpgrade() const
{
	VALIDATE_OBJECT

	return m_iMoves > 0;
}

//	--------------------------------------------------------------------------------
/// Can this Unit upgrade with anything right now?
bool CvUnit::CanUpgradeRightNow(bool bOnlyTestVisible) const
{
	VALIDATE_OBJECT
	// Is Unit in a state where it can upgrade?
	if(!isReadyForUpgrade())
		return false;

	return CanUpgradeTo(GetUpgradeUnitType(), bOnlyTestVisible);
}

bool CvUnit::CanUpgradeTo(UnitTypes eUpgradeUnitType, bool bOnlyTestVisible) const
{
	// Does the Unit actually upgrade into anything?
	if (eUpgradeUnitType == NO_UNIT)
		return false;

	CvUnitEntry* pkUpgradeUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if (!pkUpgradeUnitInfo)
		return false;

	// Tech requirement
	TechTypes ePrereqTech = static_cast<TechTypes>(pkUpgradeUnitInfo->GetPrereqAndTech());
	CvPlayer& kOwner = GET_PLAYER(getOwner());
	if (ePrereqTech != NO_TECH && !kOwner.HasTech(ePrereqTech))
		return false;

	// Project requirement
	ProjectTypes ePrereqProject = static_cast<ProjectTypes>(pkUpgradeUnitInfo->GetProjectPrereq());
	CvTeam& kTeam = GET_TEAM(getTeam());
	if (ePrereqProject != NO_PROJECT)
	{
		CvProjectEntry* pkProjectInfo = GC.getProjectInfo(ePrereqProject);
		if (pkProjectInfo && kTeam.getProjectCount(ePrereqProject) == 0)
			return false;
	}

	CvPlot* pPlot = plot();

	// Show the upgrade, but don't actually allow it
	if (!bOnlyTestVisible)
	{
		if (!canEndTurnAtPlot(pPlot))
			return false;

		if (!CanUpgradeInTerritory(bOnlyTestVisible))
			return false;

		if (isEmbarked() || pPlot->needsEmbarkation(this))
			return false;

		// Check max instances of unit class
		// Don't count units already in production; upgrading is prioritized
		UnitClassTypes eUpgradeUnitClassType = static_cast<UnitClassTypes>(pkUpgradeUnitInfo->GetUnitClassType());

		// Maxed out unit class for Game
		if (GC.getGame().isUnitClassMaxedOut(eUpgradeUnitClassType))
			return false;

		// Maxed out unit class for Team
		if (kTeam.isUnitClassMaxedOut(eUpgradeUnitClassType))
			return false;

		// Maxed out unit class for Player
		if (kOwner.isUnitClassMaxedOut(eUpgradeUnitClassType))
			return false;

		// Can't upgrade into something that our current transporter can't carry (e.g. base game Stealth Bomber)
		SpecialUnitTypes eSpecialUnit = static_cast<SpecialUnitTypes>(pkUpgradeUnitInfo->GetSpecialUnitType());
		if (isCargo() && !getTransportUnit()->CanHaveCargo(eSpecialUnit, pkUpgradeUnitInfo->GetDomainType()))
			return false;

		// Can't have cargo on board, in case it upgrades into something that can't carry cargo
		if (hasCargo())
			return false;

		// Player must have enough Gold
		if (kOwner.GetTreasury()->GetGold() < upgradePrice(eUpgradeUnitType) && !kOwner.isMinorCiv())
			return false;

		// Resource Requirements
		if (kOwner.isMajorCiv() && !kOwner.HasResourceForNewUnit(eUpgradeUnitType, false, false, getUnitType()))
			return false;

		if (getDomainType() == DOMAIN_AIR)
		{
			// Yes! upgrade if in territory and on a carrier. Community Patch knows how to Retrofit bitch on a Carrier!
			if (!pPlot->isCity() && !isCargo())
				return false;
		}
	}

	if (MOD_EVENTS_UNIT_UPGRADES)
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHaveAnyUpgrade, getOwner(), GetID()) == GAMEEVENTRETURN_FALSE)
			return false;

		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveAnyUpgrade, getOwner(), GetID()) == GAMEEVENTRETURN_FALSE)
			return false;
	}
	else
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			CvLuaArgsHandle args;
			args->Push(getOwner());
			args->Push(GetID());

			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveAnyUpgrade", args.get(), bResult))
			{
				if (!bResult)
					return false;
			}
		}
	}

	return true;
}

#if defined(MOD_GLOBAL_CS_UPGRADES)
/// Can this Unit upgrade with anything right now?
bool CvUnit::CanUpgradeInTerritory(bool bOnlyTestVisible) const
{
	VALIDATE_OBJECT

	// Show the upgrade, but don't actually allow it
	if(!bOnlyTestVisible)
	{
		CvPlot* pPlot = plot();
		const PlayerTypes kPlotOwner = pPlot->getOwner();

		// Must be in territory owned by the player or by a friendly City State
		if (kPlotOwner != getOwner()) 
		{
			if(MOD_GLOBAL_CS_UPGRADES && kPlotOwner != NO_PLAYER && GET_PLAYER(getOwner()).CanUpgradeCSVassalTerritory()) 
			{
				const CvPlayer& pPlotOwner = GET_PLAYER(kPlotOwner);
				if (!pPlotOwner.isMinorCiv())
				{
					if (!GET_TEAM(GET_PLAYER(kPlotOwner).getTeam()).IsVassal(getTeam()))
						return false;
				}
				else
				{
					if(!pPlotOwner.GetMinorCivAI()->IsFriends(getOwner()) || pPlotOwner.IsAtWarWith(getOwner()))
					{
						return false;
					}
				}
			}
			else 
			{
				return false;
			}
		}
	}

	return true;
}
#endif

//	--------------------------------------------------------------------------------
/// What UnitType does this Unit upgrade into?
UnitTypes CvUnit::GetUpgradeUnitType() const
{
	VALIDATE_OBJECT
	UnitTypes eUpgradeUnitType = NO_UNIT;
	CvPlayer& kOwner = GET_PLAYER(getOwner());

	// Determine what we're going to upgrade into
	for(int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		if (m_pUnitInfo->GetUpgradeUnitClass(iI))
		{
			eUpgradeUnitType = kOwner.GetSpecificUnitType(eUnitClass);

			if (MOD_EVENTS_UNIT_UPGRADES)
			{
				if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHaveUpgrade, getOwner(), GetID(), iI, eUpgradeUnitType) == GAMEEVENTRETURN_FALSE ||
					GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHaveUpgrade, getOwner(), GetID(), iI, eUpgradeUnitType) == GAMEEVENTRETURN_FALSE)
				{
					eUpgradeUnitType = NO_UNIT;
					continue;
				}
			}
			else
			{
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if (pkScriptSystem) 
				{
					CvLuaArgsHandle args;
					args->Push(getOwner());
					args->Push(GetID());
					args->Push(iI);
					args->Push(eUpgradeUnitType);

					bool bResult = false;
					if (LuaSupport::CallTestAll(pkScriptSystem, "CanHaveUpgrade", args.get(), bResult)) 
					{
						if (!bResult) 
						{
							eUpgradeUnitType = NO_UNIT;
							continue;
						}
					}
				}
			}

			break;
		}
	}
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		if (kOwner.GetPlayerTraits()->HasSpecialUnitUpgrade(eUnitClass, getUnitType()))
		{
			eUpgradeUnitType = kOwner.GetSpecificUnitType(eUnitClass);
			break;
		}
	}

	return eUpgradeUnitType;
}

//	--------------------------------------------------------------------------------
/// How much does it cost to upgrade this Unit to a shiny new eUnit?
int CvUnit::upgradePrice(UnitTypes eUnit) const
{
	VALIDATE_OBJECT
	int iPrice = 0;
	
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo == NULL)
	{
		return 0;
	}

	if(isBarbarian())
	{
		return 0;
	}

	if (GET_PLAYER(getOwner()).isMinorCiv())
		return 0;

	iPrice = /*10*/ GD_INT_GET(BASE_UNIT_UPGRADE_COST);

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iProductionBase = kPlayer.getProductionNeeded(getUnitType(), true);
	if (iProductionBase == 0)
	{
		iProductionBase = kPlayer.getProductionNeeded(eUnit, true) / 2;
	}

	iPrice += (std::max(0, (kPlayer.getProductionNeeded(eUnit, true) - iProductionBase)) * /*2 in CP, 1 in VP*/ GD_INT_GET(UNIT_UPGRADE_COST_PER_PRODUCTION));

	// Upgrades for later units are more expensive
	const TechTypes eTech = (TechTypes) pkUnitInfo->GetPrereqAndTech();
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo)
	{
		const EraTypes eUpgradeEra = (EraTypes) pkTechInfo->GetEra();

		double fMultiplier = 1.0f;
		fMultiplier += (eUpgradeEra* /*0.0f*/ GD_FLOAT_GET(UNIT_UPGRADE_COST_MULTIPLIER_PER_ERA));

		iPrice = int(iPrice * fMultiplier);
	}

	if (kPlayer.isMajorCiv())
	{
		iPrice *= kPlayer.getHandicapInfo().getUnitUpgradePercent();
		iPrice /= 100;

		iPrice *= std::max(0, ((kPlayer.getHandicapInfo().getUnitUpgradePerEraModifier() * GC.getGame().getCurrentEra()) + 100));
		iPrice /= 100;
		if (!isHuman())
		{
			iPrice *= GC.getGame().getHandicapInfo().getAIUnitUpgradePercent();
			iPrice /= 100;

			iPrice *= std::max(0, ((GC.getGame().getHandicapInfo().getAIUnitUpgradePerEraModifier() * GC.getGame().getCurrentEra()) + 100));
			iPrice /= 100;
		}
	}

	// Discount
	iPrice -= (iPrice * getUpgradeDiscount()) / 100;

	// Mod (Policies, etc.)
	int iMod = kPlayer.GetUnitUpgradeCostMod();
	iPrice *= (100 + iMod);
	iPrice /= 100;

	// Apply exponent
	iPrice = (int) pow((double) iPrice, (double) /*1.0f*/ GD_FLOAT_GET(UNIT_UPGRADE_COST_EXPONENT));

	// Make the number not be funky
	int iDivisor = /*5*/ GD_INT_GET(UNIT_UPGRADE_COST_VISIBLE_DIVISOR);
	iPrice /= iDivisor;
	iPrice *= iDivisor;

	return max(1, iPrice);
}

//	--------------------------------------------------------------------------------
/// Upgrades this Unit - returns a pointer to the newly created unit
CvUnit* CvUnit::DoUpgrade(bool bFree)
{
	VALIDATE_OBJECT
	return DoUpgradeTo(GetUpgradeUnitType(), bFree);
}

CvUnit* CvUnit::DoUpgradeTo(UnitTypes eUnitType, bool bFree)
{
	// Gold Cost
	int iUpgradeCost = upgradePrice(eUnitType);
	CvPlayerAI& thisPlayer = GET_PLAYER(getOwner());

	if (!bFree)
	{
		thisPlayer.GetTreasury()->LogExpenditure(getUnitInfo().GetText(), iUpgradeCost, 3);
		thisPlayer.GetTreasury()->ChangeGold(-iUpgradeCost);
	}

	// Add newly upgraded Unit & kill the old one
	CvUnit* pNewUnit = thisPlayer.initUnit(eUnitType, getX(), getY(), NO_UNITAI, REASON_UPGRADE, false, false, 0, 0, NO_CONTRACT, true, this);

	if(NULL != pNewUnit)
	{
		if(GC.getGame().getActivePlayer() == getOwner())
		{
			CvInterfacePtr<ICvUnit1> pDllNewUnit = GC.WrapUnitPointer(pNewUnit);
			DLLUI->selectUnit(pDllNewUnit.get(), true, false, false);
		}

		// MUST call the event before convert() as that kills the old unit
		if (MOD_EVENTS_UNIT_UPGRADES)
		{
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitUpgraded, getOwner(), GetID(), pNewUnit->GetID(), false);
		}
		else
		{
			ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
			if (pkScriptSystem) 
			{
				CvLuaArgsHandle args;
				args->Push(((int)getOwner()));
				args->Push(GetID());
				args->Push(pNewUnit->GetID());
				args->Push(false); // bGoodyHut

				bool bResult = false;
				LuaSupport::CallHook(pkScriptSystem, "UnitUpgraded", args.get(), bResult);
			}
		}

		if(isCultureFromExperienceDisbandUpgrade())
		{
			int iExperience = getExperienceTimes100() / 100;
			if(iExperience > 0)
			{
				GET_PLAYER(getOwner()).changeJONSCulture(iExperience);
				if (getOwner() == GC.getGame().getActivePlayer())
				{
					char text[256] = { 0 };
					sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iExperience);
					SHOW_PLOT_POPUP(plot(),getOwner(),text);
				}
			}
		}

		if (MOD_SQUADS && GetSquadNumber() > -1)
		{
			pNewUnit->AssignToSquad(GetSquadNumber());
		}

		pNewUnit->convert(this, true);
		pNewUnit->setupGraphical();
		
		// Can't move after upgrading
		if (MOD_GLOBAL_MOVE_AFTER_UPGRADE && pNewUnit->getUnitInfo().CanMoveAfterUpgrade())
		{
			pNewUnit->m_iAttacksMade = m_iAttacksMade;
			pNewUnit->m_iMadeInterceptionCount = m_iMadeInterceptionCount;
			pNewUnit->m_eActivityType = m_eActivityType;
			pNewUnit->m_bMovedThisTurn = m_bMovedThisTurn;
			pNewUnit->m_bHasWithdrawnThisTurn = m_bHasWithdrawnThisTurn;
			pNewUnit->m_bFortified = m_bFortified;
		}
		else
		{
			pNewUnit->finishMoves();
		}

		kill(true);
	}

	if (MOD_API_ACHIEVEMENTS && isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
		gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_UPGRADE);

	return pNewUnit;
}


//	--------------------------------------------------------------------------------
HandicapTypes CvUnit::getHandicapType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getHandicapType();
}


//	--------------------------------------------------------------------------------
const CvCivilizationInfo& CvUnit::getCivilizationInfo() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationInfo();
}

//	--------------------------------------------------------------------------------
CivilizationTypes CvUnit::getCivilizationType() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).getCivilizationType();
}

//	--------------------------------------------------------------------------------
const char* CvUnit::getVisualCivAdjective(TeamTypes eForTeam) const
{
	VALIDATE_OBJECT
	if(getVisualOwner(eForTeam) == getOwner())
	{
		return GET_PLAYER(getOwner()).getCivilizationInfo().getAdjectiveKey();
	}

	return "";
}

//	--------------------------------------------------------------------------------
SpecialUnitTypes CvUnit::getSpecialUnitType() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialUnitType()));
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatPerson() const
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	return (getSpecialUnitType() == eSpecialUnitGreatPerson);
}

//	--------------------------------------------------------------------------------
UnitTypes CvUnit::getCaptureUnitType(PlayerTypes eCapturingPlayer) const
{
	VALIDATE_OBJECT
	if (eCapturingPlayer == NO_PLAYER)
		return NO_UNIT;

	CvPlayer& kCapturingPlayer = GET_PLAYER(eCapturingPlayer);

	// Event override
	CivilizationTypes eCivilization = kCapturingPlayer.getCivilizationType();
	if (MOD_EVENTS_UNIT_CAPTURE)
	{
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_UnitCaptureType, eCapturingPlayer, GetID(), getUnitType(), eCivilization) == GAMEEVENTRETURN_VALUE)
		{
			// Defend against modder stupidity!
			UnitTypes eUnit = static_cast<UnitTypes>(iValue);
			if (eUnit != NO_UNIT && GC.getUnitInfo(eUnit) != NULL)
			{
				return eUnit;
			}
		}
	}

	UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(getUnitInfo().GetUnitCaptureClassType());
	if (eUnitClass == NO_UNITCLASS)
		return NO_UNIT;

	return kCapturingPlayer.GetSpecificUnitType(eUnitClass);
}


//	--------------------------------------------------------------------------------
int CvUnit::getUnitCombatType() const
{
	VALIDATE_OBJECT
	return m_eCombatType;
}

void CvUnit::setUnitCombatType(UnitCombatTypes eCombat)
{
	VALIDATE_OBJECT
	if(getUnitCombatType() != eCombat)
	{
		m_eCombatType = eCombat;
	}
}

#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
//	--------------------------------------------------------------------------------
UnitCombatTypes CvUnit::getUnitPromotionType() const
{
	VALIDATE_OBJECT
	return ((UnitCombatTypes)(m_pUnitInfo->GetUnitPromotionType()));
}
#endif

bool CvUnit::isNativeDomain(const CvPlot* pPlot) const
{
	if (!pPlot)
		return false;

	if (isCargo() && getDomainType() != DOMAIN_AIR)
		return false;

	switch (getDomainType())
	{
	case NO_DOMAIN:
		return true; // TODO: Should probably be an error, but maintains existing behavior.
	case DOMAIN_LAND:
		if (!pPlot->isWater())
			return true;
		else
		{
			ImprovementTypes eImprovement = pPlot->getImprovementType();
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
			if (pkImprovementInfo != NULL && pkImprovementInfo->IsAllowsWalkWater())
				return true;
			else
			{
				//let's treat ice like (usually impassable) land
				//same logic as in needsEmbarkation, which should be folded into this function anyway
				if (pPlot->isIce())
					return true;

#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
				//deep water embarkation means we treat shallow water like land
				if (IsEmbarkDeepWater() && pPlot->isShallowWater())
					return true;
#endif

				if (isNoAttackInOcean() && pPlot->isDeepWater())
					return false;

				if (canMoveAllTerrain())
					return true;

				//must be water
				return false;
			}
		}
		break;
	case DOMAIN_AIR:
		return true;
	case DOMAIN_SEA:
#if defined(MOD_SHIPS_FIRE_IN_CITIES_IMPROVEMENTS)
		if(MOD_SHIPS_FIRE_IN_CITIES_IMPROVEMENTS)
		{
			if (pPlot->isWater() || pPlot->isCity())
				return true;
			else
			{
				ImprovementTypes eImprovement = pPlot->getImprovementType();
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				if (pkImprovementInfo != NULL && pkImprovementInfo->IsMakesPassable())
					return true;

				//must be land
				return false;
			}
		}
#endif
		return (pPlot->isWater());
	case DOMAIN_HOVER:
		return true;
	case DOMAIN_IMMOBILE:
		return false;
	}

	return true;
}

bool CvUnit::canEndTurnAtPlot(const CvPlot * pPlot) const
{
	if (!pPlot)
		return false;

	if (isDelayedDeath())
		return true;

	if (isCargo())
		return true;

	if (isInCombat())
		return true;

	return canMoveInto(*pPlot, MOVEFLAG_DESTINATION);
}

//	---------------------------------------------------------------------------
DomainTypes CvUnit::getDomainType() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetDomainType();
}

//	---------------------------------------------------------------------------
int CvUnit::flavorValue(FlavorTypes eFlavor) const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->GetFlavorValue(eFlavor);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isBarbarian() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isBarbarian();
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHuman() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).isHuman();
}

//	--------------------------------------------------------------------------------
int CvUnit::visibilityRange() const
{
	VALIDATE_OBJECT

	//in general vision range needs to be at least one, otherwise there will be stacking issues
	int iVisionRange = 0;

	if (!IsGainsXPFromScouting() && !isTrade())
	{
		if (GET_PLAYER(m_eOwner).isMajorCiv())
		{
			iVisionRange += GET_PLAYER(m_eOwner).getHandicapInfo().getVisionBonus();
			iVisionRange += isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIVisionBonus();
		}
		else if (GET_PLAYER(m_eOwner).isMinorCiv())
		{
			iVisionRange += GC.getGame().getHandicapInfo().getCityStateVisionBonus();
		}
	}

	if (isEmbarked())
	{
		iVisionRange += max(1, /*1*/ GD_INT_GET(EMBARKED_VISIBILITY_RANGE) + m_iEmbarkExtraVisibility);
	}
	else if (isTrade())
	{
		//special rules for trade units, default range is zero!
		iVisionRange += GET_PLAYER(m_eOwner).GetTRVisionBoost();

		CorporationTypes eCorporation = GET_PLAYER(m_eOwner).GetCorporations()->GetFoundedCorporation();
		if (eCorporation != NO_CORPORATION)
		{
			CvCorporationEntry* pkCorporation = GC.getCorporationInfo(eCorporation);
			iVisionRange += pkCorporation ? pkCorporation->GetTradeRouteVisionBoost() : 0;
		}
	}
	else
	{
		iVisionRange += max(1, m_pUnitInfo->GetBaseSightRange() + m_iExtraVisibilityRange);
	}

	return iVisionRange;
}

#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
//	--------------------------------------------------------------------------------
int CvUnit::reconRange() const
{
	VALIDATE_OBJECT

	int iRtnValue = /*6*/ GD_INT_GET(RECON_VISIBILITY_RANGE);

	if(MOD_PROMOTIONS_VARIABLE_RECON)
	{
		iRtnValue += m_iExtraReconRange;
	}

	return iRtnValue;
}
#endif

//	---------------------------------------------------------------------------
// Get the base movement points for the unit.
int CvUnit::baseMoves(bool bPretendEmbarked) const
{
	VALIDATE_OBJECT
	CvTeam& thisTeam = GET_TEAM(getTeam());
	CvPlayer& thisPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = thisPlayer.GetPlayerTraits();
	DomainTypes eDomain = getDomainType();

	//hover units don't embark but movement speed may change!
	if(bPretendEmbarked)
	{
		CvPlayerPolicies* pPolicies = thisPlayer.GetPlayerPolicies();
		return max(1, /*2 in CP, 3 in VP*/ GD_INT_GET(EMBARKED_UNIT_MOVEMENT) + getExtraNavalMoves() + thisTeam.getEmbarkedExtraMoves() + thisTeam.getExtraMoves(eDomain) + pTraits->GetExtraEmbarkMoves() + pPolicies->GetNumericModifier(POLICYMOD_EMBARKED_EXTRA_MOVES));
	}

	int m_iExtraNavalMoves = 0;
	if(eDomain == DOMAIN_SEA)
	{
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
			for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
				if (pInfo && pInfo->getMonopolyMovementBonus() > 0)
				{
					m_iExtraNavalMoves += pInfo->getMonopolyMovementBonus();
				}
			}
		}
#endif
		// Work boats also get extra moves, and they don't have a combat class to receive a promotion from
		if(m_iBaseCombat == 0)
		{
			m_iExtraNavalMoves += pTraits->GetExtraEmbarkMoves();
		}
	}

	int iExtraGoldenAgeMoves = 0;
	if(thisPlayer.isGoldenAge())
	{
		iExtraGoldenAgeMoves = pTraits->GetGoldenAgeMoveChange();
	}

#if defined(MOD_BALANCE_CORE_POLICIES)
	if(!IsCombatUnit() && !IsCombatSupportUnit())
	{
		iExtraGoldenAgeMoves += GET_PLAYER(getOwner()).GetExtraMoves();
	}
#endif

	int iExtraUnitCombatTypeMoves = pTraits->GetMovesChangeUnitCombat((UnitCombatTypes)(m_pUnitInfo->GetUnitCombatType()));

#if defined(MOD_BALANCE_CORE)
	iExtraUnitCombatTypeMoves += pTraits->GetMovesChangeUnitClass((UnitClassTypes)(m_pUnitInfo->GetUnitClassType()));
#endif

	return max(1, (m_pUnitInfo->GetMoves() + getExtraMoves() + thisTeam.getExtraMoves(eDomain) + m_iExtraNavalMoves + iExtraGoldenAgeMoves + iExtraUnitCombatTypeMoves));
}


//	---------------------------------------------------------------------------
int CvUnit::maxMoves() const
{
	if (plot() == NULL)
		return 0;
	if (MOD_LINKED_MOVEMENT && (IsLinked() || IsGrouped() ))
		return GetLinkedMaxMoves();
	// WARNING: Depends on the current embark state of the unit!
	if (plot()->getOwner() == getOwner())
		return (baseMoves(isEmbarked()) + plot()->GetPlotMovesChange()) * GD_INT_GET(MOVE_DENOMINATOR);
	else
		return baseMoves(isEmbarked()) * GD_INT_GET(MOVE_DENOMINATOR);
}

//	--------------------------------------------------------------------------------
int CvUnit::movesLeft() const
{
	VALIDATE_OBJECT
	return std::max(0, getMoves());
}


//	--------------------------------------------------------------------------------
bool CvUnit::canMove() const
{
	VALIDATE_OBJECT
	return (getMoves() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::hasMoved()	const
{
	VALIDATE_OBJECT
	return m_bMovedThisTurn;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsLinked()	const
{
	VALIDATE_OBJECT
		return m_bIsLinked;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetIsLinked(bool bValue)
{
	VALIDATE_OBJECT

	if (m_bIsLinked != bValue)
	{
		int iMovesThisTurn = GetLinkedMaxMoves() - getMoves();
		m_bIsLinked = bValue;

		if (!bValue)
		{
			int iUnlinkedMaxMoves = maxMoves();

			if (iUnlinkedMaxMoves > iMovesThisTurn) {
				setMoves(iUnlinkedMaxMoves - iMovesThisTurn); // give back lost movement points
			}
			
			SetLinkedLeaderID(-1);
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsLinkedLeader()	const
{
	VALIDATE_OBJECT
		return m_bIsLinkedLeader;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetIsLinkedLeader(bool bValue)
{
	VALIDATE_OBJECT

	if (m_bIsLinkedLeader != bValue)
	{
		m_bIsLinkedLeader = bValue;

		if (!bValue)
		{
			UnitIdContainer LinkedUnitIDs = GetLinkedUnits();
			for (int iI = 0; iI < (int)LinkedUnitIDs.size(); iI++)
			{
				CvUnit* pLinkedUnit = GET_PLAYER(m_eOwner).getUnit(LinkedUnitIDs[iI]);
				pLinkedUnit->SetIsLinked(false);
			}

			SetIsLinked(false);
			m_LinkedUnitIDs.clear();
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGrouped() const
{
	VALIDATE_OBJECT
		return m_bIsGrouped;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetIsGrouped(bool bValue)
{
	VALIDATE_OBJECT
	if (m_bIsGrouped != bValue)
	{
		int iMovesThisTurn = GetLinkedMaxMoves() - getMoves();
		m_bIsGrouped = bValue;

		if (!bValue)
		{
			int iUnlinkedMaxMoves = maxMoves();

			if (iUnlinkedMaxMoves > iMovesThisTurn) {
				setMoves(iUnlinkedMaxMoves - iMovesThisTurn); // give back lost movement points
			}
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::SetLinkedUnits(const UnitIdContainer& LinkedUnits)
{
	VALIDATE_OBJECT
		m_LinkedUnitIDs = LinkedUnits;
}

//	--------------------------------------------------------------------------------
UnitIdContainer CvUnit::GetLinkedUnits()
{
	VALIDATE_OBJECT
		return m_LinkedUnitIDs;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetLinkedMaxMoves()	const
{
	VALIDATE_OBJECT
		return m_iLinkedMaxMoves;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetLinkedMaxMoves(int iValue)
{
	VALIDATE_OBJECT
		if (m_iLinkedMaxMoves != iValue)
		{
			m_iLinkedMaxMoves = iValue;
		}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetLinkedLeaderID()	const
{
	VALIDATE_OBJECT
		return m_iLinkedLeaderID;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetLinkedLeaderID(int iLinkedLeaderID)
{
	VALIDATE_OBJECT
		if (m_iLinkedLeaderID != iLinkedLeaderID)
		{
			m_iLinkedLeaderID = iLinkedLeaderID;
		}
}


//	--------------------------------------------------------------------------------
bool CvUnit::CanLinkUnits()
{
	VALIDATE_OBJECT

	const CvPlot* pCurrentPlot = plot();

	if (pCurrentPlot == NULL || !isHuman() || IsLinked())
		return false;

	const CvUnit* pThisUnit = this;
	const bool bIsOnSea = getDomainType() == DOMAIN_SEA || isEmbarked();

	if (pThisUnit == NULL || pThisUnit->isDelayedDeath() || pThisUnit->isTrade() || pThisUnit->getDomainType() == DOMAIN_AIR)
		return false;

	const IDInfo* pUnitNode = pCurrentPlot->headUnitNode();
	CvUnit* pLoopUnit = NULL;

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pCurrentPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwner() == getOwner() && !pLoopUnit->isDelayedDeath() && !pLoopUnit->isTrade() && pLoopUnit->getDomainType() != DOMAIN_AIR)
		{
			if (pLoopUnit != this)
			{
				if ( (bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->isEmbarked())) || (!bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_LAND && !pLoopUnit->isEmbarked())) )
				{
					return true;
				}
			}
		}
	}

	return false;
}
//	--------------------------------------------------------------------------------
void CvUnit::LinkUnits()
{
	VALIDATE_OBJECT

	const CvPlot* pCurrentPlot = plot();
		
	if (pCurrentPlot == NULL || !isHuman())
		return;

	const IDInfo* pUnitNode = pCurrentPlot->headUnitNode();
	CvUnit* pLoopUnit = NULL;
	vector<CvUnit*> v_unitvector;
	UnitIdContainer LinkedUnitIDs;
	int iLowestCurrentMoves = getMoves();
	int iLowestMaxMoves = (IsGrouped()) ? GetLinkedMaxMoves() : maxMoves();
	bool bLeaderAssigned = false;
	CvUnit* pLinkedLeader = this;
	const bool bIsOnSea = getDomainType() == DOMAIN_SEA || isEmbarked();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pCurrentPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwner() == getOwner() && !pLoopUnit->isDelayedDeath() && !pLoopUnit->isTrade() && pLoopUnit->getDomainType() != DOMAIN_AIR)
		{
			if ((bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->isEmbarked())) || (!bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_LAND && !pLoopUnit->isEmbarked())))
			{
				v_unitvector.push_back(pLoopUnit);
				int iLoopMoves = pLoopUnit->getMoves();
				int iLoopMaxMoves = pLoopUnit->maxMoves();

				if (iLoopMoves < iLowestCurrentMoves) {
					iLowestCurrentMoves = iLoopMoves;
				}
				if (iLoopMaxMoves < iLowestMaxMoves) {
					iLowestMaxMoves = iLoopMaxMoves;
				}
				if (!bLeaderAssigned && !bIsOnSea && pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER) { // workers are prioritized, allows them to ask for orders
					pLoopUnit->SetIsLinkedLeader(true);
					pLinkedLeader = pLoopUnit;
					bLeaderAssigned = true;
				}
			}
		}
	}

	if (!bLeaderAssigned)	{ // no workers found, ordering unit gets leadership
		SetIsLinkedLeader(true);
	}

	for (int iI = 0; iI < (int)v_unitvector.size(); iI++)
	{
		CvUnit* pUnit = v_unitvector[iI];

		pUnit->SetIsLinked(true);
		pUnit->setMoves(iLowestCurrentMoves);
		pUnit->SetLinkedMaxMoves(iLowestMaxMoves);

		if (pUnit->IsLinkedLeader()) {
			continue;
		}
		else {
			LinkedUnitIDs.push_back(pUnit->GetID());
			pUnit->SetLinkedLeaderID(pLinkedLeader->GetID());

			if (pLoopUnit->canFortify(pCurrentPlot)) {
				pLoopUnit->PushMission(CvTypes::getMISSION_FORTIFY());
			}
			else {
				pLoopUnit->PushMission(CvTypes::getMISSION_SLEEP());
			}
		}
	}
	pLinkedLeader->SetLinkedUnits(LinkedUnitIDs);
}

//	--------------------------------------------------------------------------------
void CvUnit::UnlinkUnits() //mostly for the lua call
{
	VALIDATE_OBJECT

	if (!IsLinked()) {
		return;
	}
	else
	{
		if (IsLinkedLeader()) {
			SetIsLinkedLeader(false); // the actual unlinking happens here
		}
		else {
			CvUnit* pLinkedLeader = GET_PLAYER(m_eOwner).getUnit(GetLinkedLeaderID());
			pLinkedLeader->SetIsLinkedLeader(false);
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::MoveLinkedLeader(CvPlot* pDestPlot)
{
	VALIDATE_OBJECT

	CvUnit* pLinkedLeader = GET_PLAYER(m_eOwner).getUnit(GetLinkedLeaderID());
	pLinkedLeader->PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED);

}
//	--------------------------------------------------------------------------------
void CvUnit::DoGroupMovement(CvPlot* pDestPlot)
{
	VALIDATE_OBJECT

	LinkUnits();
	const CvPlot* pCurrentPlot = plot();

	if (pCurrentPlot == NULL)
		return;

	vector<CvUnit*> v_unitvector;
	int iLowestCurrentMoves = getMoves();
	int iLowestMaxMoves = maxMoves();

	const bool bIsOnSea = getDomainType() == DOMAIN_SEA || isEmbarked();

	for (int i = RING0_PLOTS; i < RING_PLOTS[1]; i++) // first get the units and calculate group base move
	{
		CvPlot* pLoopPlot = iterateRingPlots(pCurrentPlot, i);
		if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0)
		{
			const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
			CvUnit* pLoopUnit = NULL;
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				if ( pLoopUnit != NULL && pLoopUnit->getOwner() == getOwner() && !pLoopUnit->isDelayedDeath() && 
					( !pLoopUnit->IsCivilianUnit() || pLoopUnit->IsGreatAdmiral() || pLoopUnit->IsGreatGeneral() ) 
					&&
					( ( bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->isEmbarked()) ) || 
					  (!bIsOnSea && (pLoopUnit->getDomainType() == DOMAIN_LAND && !pLoopUnit->isEmbarked())) ) )
				{
					v_unitvector.push_back(pLoopUnit);
					int iLoopMoves = pLoopUnit->getMoves();
					int iLoopMaxMoves = pLoopUnit->maxMoves();

					if (iLoopMoves < iLowestCurrentMoves) {
						iLowestCurrentMoves = iLoopMoves;
					}
					if (iLoopMaxMoves < iLowestMaxMoves) {
						iLowestMaxMoves = iLoopMaxMoves;
					}
				}
			}
		}
	}

	for (int iI = 0; iI < (int)v_unitvector.size(); iI++) // then move the units
	{
		CvUnit* pUnit = v_unitvector[iI];

		int iXDiff = getX() - pUnit->getX(); // to get the relative position to the central unit
		int iYDiff = getY() - pUnit->getY();
		CvPlot* pFirstTargetPlot = GC.getMap().plot(pDestPlot->getX() - iXDiff, pDestPlot->getY() - iYDiff); // ideal plot

		pUnit->SetIsGrouped(true);
		pUnit->setMoves(iLowestCurrentMoves);
		pUnit->SetLinkedMaxMoves(iLowestMaxMoves);
		// first we try to move the unit and keep its relative position, if fails, we try to move the unit to ring1 and then ring2.
		if (pUnit->canMoveInto(*pFirstTargetPlot, CvUnit::MOVEFLAG_DESTINATION)) {
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pFirstTargetPlot->getX(), pFirstTargetPlot->getY(), CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED);
		}
		else if (pUnit->canMoveInto(*pDestPlot, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_APPROX_TARGET_RING1)) {
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED | CvUnit::MOVEFLAG_APPROX_TARGET_RING1);
		}
		else if (pUnit->canMoveInto(*pDestPlot, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_APPROX_TARGET_RING2)) {
			pUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED | CvUnit::MOVEFLAG_APPROX_TARGET_RING2);
		}
		else {
			pUnit->SetIsGrouped(false); // cannot move the unit, kick it out of the group
		}
	}

	SetIsGrouped(true); 
	setMoves(iLowestCurrentMoves);	
	SetLinkedMaxMoves(iLowestMaxMoves);
	PushMission(CvTypes::getMISSION_MOVE_TO(), pDestPlot->getX(), pDestPlot->getY(), CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED); // the iterator doesn't include the current plot, so move the ordering unit & and its stack here
}

int CvUnit::GetSquadNumber() const
{
	VALIDATE_OBJECT

	return m_iSquadNumber;
}
//	--------------------------------------------------------------------------------
void CvUnit::AssignToSquad(int iNewSquadNumber)
{
	VALIDATE_OBJECT

	m_iSquadNumber = iNewSquadNumber;
}
//	--------------------------------------------------------------------------------
void CvUnit::RemoveFromSquad()
{
	VALIDATE_OBJECT

	m_iSquadNumber = -1;
}
//	--------------------------------------------------------------------------------
struct ScoredUnit
{
	int distanceToPlot;
	CvUnit* unit;

	ScoredUnit(int distanceToPlot, CvUnit* unit) : distanceToPlot(distanceToPlot), unit(unit) {}

	bool operator > (const ScoredUnit& o) const
	{
		return (distanceToPlot > o.distanceToPlot);
	}
};

// Used to hold results of all eligible plots for a particular unit
struct ScoredPlot
{
	int distanceToPlot;
	CvPlot* plot;

	ScoredPlot(int distanceToPlot, CvPlot* plot) : distanceToPlot(distanceToPlot), plot(plot) {}

	bool operator > (const ScoredPlot& o) const
	{
		return (distanceToPlot > o.distanceToPlot);
	}

  bool operator < (const ScoredPlot& o) const
  {
    return (distanceToPlot < o.distanceToPlot);
  }
};

void CvUnit::DoSquadMovement(CvPlot* pDestPlot)
{
	VALIDATE_OBJECT

	int squadNumber = GetSquadNumber();
	if (GetSquadNumber() == -1)
	{
		return;
	}

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());

	bool isDestWater = pDestPlot->isWater();

	// First, construct a list of all units in squad eligible to go to target tile
	std::vector<CvUnit*> stackingUnits;
	std::vector<CvUnit*> eligibleUnits;
	int iLoop = 0;
	CvUnit* pLoopUnit = NULL;
	for(pLoopUnit = pPlayer->firstUnitInSquad(&iLoop, squadNumber); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnitInSquad(&iLoop, squadNumber))
	{
		if (pLoopUnit->canMoveInto(*pDestPlot))
		{
			// If this unit is going to move later, clear its path cache. Otherwise, due to the 
			// use of MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT it can try to continue on its old path
			// instead of calculating a new route to the destination plot
			pLoopUnit->ClearMissionQueue(false);

			if (!pLoopUnit->IsCombatUnit() || pLoopUnit->IsStackingUnit())
			{
				stackingUnits.push_back(pLoopUnit);
			}
			else
			{
				eligibleUnits.push_back(pLoopUnit);
			}
		}
	}

	// Refuse to handle this absurdidity
	if (eligibleUnits.size() > 90)
	{
		return;
	}

	// Generate a list of units to move and their distance to the destination plot
	std::vector<ScoredUnit> unitsToMoveByDistance;
	for (std::vector<CvUnit*>::iterator it = eligibleUnits.begin(); it != eligibleUnits.end(); ++it)
	{
		CvUnit* pLoopUnit = *it;
		int dist = plotDistance(*pLoopUnit->plot(), *pDestPlot);
		unitsToMoveByDistance.push_back(ScoredUnit(dist, pLoopUnit));
	}
	std::stable_sort(unitsToMoveByDistance.begin(), unitsToMoveByDistance.end(), greater<ScoredUnit>());

	// Generate a list of eligible plots for these units, only adding another
	// ring when there are still insufficient plots for the current selection
	std::vector<CvPlot*> eligiblePlots;
	int currRingEndIdx = 1;
	for (int targetPlotIdx = 0; targetPlotIdx < RING_PLOTS[currRingEndIdx]; targetPlotIdx++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pDestPlot, targetPlotIdx);
		if (!pLoopPlot)
			continue;

		if (canMoveInto(*pLoopPlot) && (isDestWater == pLoopPlot->isWater()))
		{
			eligiblePlots.push_back(pLoopPlot);
		}

		if (targetPlotIdx == (RING_PLOTS[currRingEndIdx] - 1) && eligiblePlots.size() < unitsToMoveByDistance.size())
		{
			currRingEndIdx++;
		}
	}

	// Give priority to furthest units so they won't have to walk around closer units
	// that arrive to shortest distance plots first
	std::map<CvUnit*, CvPlot*> unitToPlot;
	for (std::vector<ScoredUnit>::iterator it = unitsToMoveByDistance.begin(); it != unitsToMoveByDistance.end(); ++it)
	{
		CvUnit* pLoopUnit = (*it).unit;

		CvPlot* closestPlot = NULL;
		int closestDistance = INT_MAX;

		for (std::vector<CvPlot*>::iterator it = eligiblePlots.begin(); it != eligiblePlots.end(); ++it)
		{
			CvPlot* pLoopPlot = *it;

			if (closestPlot)
			{
				int dist = plotDistance(*pLoopUnit->plot(), *pLoopPlot);
				if (dist < closestDistance && pLoopUnit->canMoveInto(*pLoopPlot) && (isDestWater == pLoopPlot->isWater()))
				{
					closestDistance = dist;
					closestPlot = pLoopPlot;
				}
			}
			else
			{
				closestPlot = pLoopPlot;
			}
		}

		if (closestPlot)
		{
			// Insert into a map and move all at once instead of moving one by one so a unit doesn't
			// occupy a valid tile as an intermediate step to a further tile
			unitToPlot.insert(std::make_pair(pLoopUnit, closestPlot));
			eligiblePlots.erase(std::find(eligiblePlots.begin(), eligiblePlots.end(), closestPlot));
		}
	}

	SetSquadDestination(pDestPlot);

	// Send all stacking units to original destination plot
	for (std::vector<CvUnit*>::iterator it = stackingUnits.begin(); it != stackingUnits.end(); ++it)
	{
		CvUnit* pLoopUnit = *it;
		pLoopUnit->PushMission(
			CvTypes::getMISSION_MOVE_TO(),
			pDestPlot->getX(),
			pDestPlot->getY(),
			CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED | CvUnit::MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT);
	}

	// Now that we have all the units to move and tiles for them, send the move missions
	for (std::vector<ScoredUnit>::iterator it = unitsToMoveByDistance.begin(); it != unitsToMoveByDistance.end(); ++it)
	{
		CvUnit* pLoopUnit = (*it).unit;
		CvPlot* targetPlot = NULL;

		std::map<CvUnit*, CvPlot*>::iterator mit = unitToPlot.find(pLoopUnit);
		if (mit != unitToPlot.end())
		{
			targetPlot = mit->second;
		}
		else
		{
			targetPlot = pDestPlot;
		}

		pLoopUnit->PushMission(
			CvTypes::getMISSION_MOVE_TO(),
			targetPlot->getX(),
			targetPlot->getY(),
			CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED | CvUnit::MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT);
	}
}
//	--------------------------------------------------------------------------------
bool CvUnit::IsUnitInActiveMoveMission()
{
	return GetHeadMissionData() &&
		GetHeadMissionData()->eMissionType == CvTypes::getMISSION_MOVE_TO() &&
		(GetPathLastPlot() == NULL || // Case when mission is still ongoing but needs recalculation
			!atPlot(*this->GetPathLastPlot()));
}
//	--------------------------------------------------------------------------------
bool CvUnit::IsSquadMoving()
{
	VALIDATE_OBJECT

	int squadNumber = GetSquadNumber();
	if (GetSquadNumber() == -1 || !HasSquadDestination())
	{
		return false;
	}

	CvPlayer* pPlayer = &GET_PLAYER(getOwner());

	// If any units are moving, the whole squad is still moving
	int iLoop = 0;
	CvUnit* pLoopUnit = NULL;
	for(pLoopUnit = pPlayer->firstUnitInSquad(&iLoop, squadNumber); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnitInSquad(&iLoop, squadNumber))
	{
		if (pLoopUnit->IsUnitInActiveMoveMission())
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvUnit::TryEndSquadMovement()
{
	VALIDATE_OBJECT

	if (GetSquadNumber() == -1 || !HasSquadDestination())
	{
		return;
	}

	if (!IsSquadMoving() && GetSquadEndMovementType() == WAKE_ON_ALL_ARRIVED)
	{
		// When squad is done moving, wake up all units
		CvPlayer* pPlayer = &GET_PLAYER(getOwner());
		int iLoop = 0;
		CvUnit* pLoopUnit = NULL;
		for(pLoopUnit = pPlayer->firstUnitInSquad(&iLoop, GetSquadNumber()); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnitInSquad(&iLoop, GetSquadNumber()))
		{
			pLoopUnit->SetActivityType(ACTIVITY_AWAKE);
		}
		SetSquadDestination();
		DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else if (!IsUnitInActiveMoveMission())
	{
		if (GetSquadEndMovementType() != WAKE_ON_EACH_UNIT_ARRIVED)
		{
			if (canSentry(plot()))
			{
				PushMission(CvTypes::getMISSION_ALERT());
			}
			else
			{
				PushMission(CvTypes::getMISSION_SLEEP());
			}
		}
	}
}

//  --------------------------------------------------------------------------------
void CvUnit::SetSquadDestination(CvPlot* pDestPlot)
{
	CvPlayer* pPlayer = &GET_PLAYER(getOwner());

	if (pDestPlot)
	{
		m_iSquadDestinationX = pDestPlot->getX();
		m_iSquadDestinationY = pDestPlot->getY();
	}
	else
	{
		m_iSquadDestinationX = -1;
		m_iSquadDestinationY = -1;
	}

	CvUnit* pLoopUnit = NULL;
	int iLoop = 0;
	for(pLoopUnit = pPlayer->firstUnitInSquad(&iLoop, GetSquadNumber()); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnitInSquad(&iLoop, GetSquadNumber()))
	{
		pLoopUnit->m_iSquadDestinationX = m_iSquadDestinationX;
		pLoopUnit->m_iSquadDestinationY = m_iSquadDestinationY;
	}
}

//  --------------------------------------------------------------------------------
bool CvUnit::HasSquadDestination()
{
	return m_iSquadDestinationX >= 0 && m_iSquadDestinationY >= 0;
}

//  --------------------------------------------------------------------------------
CvPlot* CvUnit::GetSquadDestination()
{
	VALIDATE_OBJECT
	if (HasSquadDestination())
	{
		return GC.getMap().plotCheckInvalid(m_iSquadDestinationX, m_iSquadDestinationY);
	}
	else
	{
		return NULL;
	}
}

//  --------------------------------------------------------------------------------
SquadsEndMovementType CvUnit::GetSquadEndMovementType() const
{
	VALIDATE_OBJECT

	return static_cast<SquadsEndMovementType>(m_SquadEndMovementType);
}

//  --------------------------------------------------------------------------------
void CvUnit::SetSquadEndMovementType(SquadsEndMovementType endMovementType)
{
	VALIDATE_OBJECT

	m_SquadEndMovementType = endMovementType;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetRange() const
{
	VALIDATE_OBJECT

	return (m_pUnitInfo->GetRange() + m_iExtraRange);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNukeDamageLevel() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetNukeDamageLevel();
}

//	--------------------------------------------------------------------------------
bool CvUnit::canBuildRoute() const
{
	VALIDATE_OBJECT

	if(m_bEmbarked)
	{
		return false;
	}

	CvTeam& thisTeam = GET_TEAM(getTeam());

	CvTeamTechs* pTeamTechs = thisTeam.GetTeamTechs();

	int iNumBuildInfos = GC.getNumBuildInfos();
	for(int iI = 0; iI < iNumBuildInfos; iI++)
	{
		CvBuildInfo* thisBuildInfo = GC.getBuildInfo((BuildTypes)iI);

		// Don't count routes that come as part of an improvement
		if(NULL != thisBuildInfo && thisBuildInfo->getImprovement() == NO_IMPROVEMENT && thisBuildInfo->getRoute() != NO_ROUTE)
		{
			if(m_pUnitInfo->GetBuilds(iI))
			{
				if(pTeamTechs->HasTech((TechTypes)(thisBuildInfo->getTechPrereq())))
				{
					return true;
				}
#if defined(MOD_BALANCE_CORE)
				if(thisBuildInfo->getTechObsolete() != NO_TECH && !pTeamTechs->HasTech((TechTypes)(thisBuildInfo->getTechObsolete())))
				{
					return true;
				}
#endif
			}
		}
	}

	return false;
}

//	----------------------------------------------------------------------------
BuildTypes CvUnit::getBuildType() const
{
	VALIDATE_OBJECT
	const MissionData* pkMissionNode = HeadMissionData();
	if(pkMissionNode != NULL)
	{
		if(pkMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO())
		{
			BuildTypes eBestBuild = NO_BUILD;
			GetBestBuildRouteForRoadTo(plot(), &eBestBuild);
			return eBestBuild;
		}
		else if(pkMissionNode->eMissionType == CvTypes::getMISSION_BUILD())
		{
			return (BuildTypes)pkMissionNode->iData1;
		}
	}

	return NO_BUILD;
}

//	----------------------------------------------------------------------------
bool CvUnit::IsWorking() const
{
	VALIDATE_OBJECT
	const MissionData* pkMissionNode = HeadMissionData();
	return pkMissionNode && (pkMissionNode->eMissionType == CvTypes::getMISSION_ROUTE_TO() || pkMissionNode->eMissionType == CvTypes::getMISSION_BUILD());
}

//	--------------------------------------------------------------------------------
int CvUnit::workRate(bool bMax, BuildTypes /*eBuild*/) const
{
	VALIDATE_OBJECT
	if(!bMax)
	{
		if(!canMove())
		{
			return 0;
		}
	}

	int iRate = m_pUnitInfo->GetWorkRate();

#if defined(MOD_BALANCE_CORE)
	if(iRate <= 0)
	{
		for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			const BuildTypes eBuild = static_cast<BuildTypes>(iI);
			CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
			if(pkBuildInfo)
			{
				if(GET_PLAYER(getOwner()).GetPlayerTraits()->HasUnitClassCanBuild(eBuild, getUnitClassType()))
				{
					iRate = 100;
					break;
				}
			}
		}
	}

	int Modifiers = 0;
	if (MOD_BALANCE_CORE_BARBARIAN_THEFT && GetWorkRateMod() != 0)
	{
		Modifiers += GetWorkRateMod();
	}
#endif

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	Modifiers += kPlayer.getWorkerSpeedModifier() + kPlayer.GetPlayerTraits()->GetWorkerSpeedModifier();

	if (kPlayer.isMajorCiv())
	{
		Modifiers += kPlayer.getHandicapInfo().getWorkRateModifier();
		Modifiers += kPlayer.isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIWorkRateModifier();
	}
	else if (kPlayer.isMinorCiv())
	{
		Modifiers += GC.getGame().getHandicapInfo().getCityStateWorkRateModifier();
	}

	iRate *= Modifiers + 100;
	iRate /= 100;

	if (MOD_CIV6_WORKER)
	{
		iRate = 1;
	}

	return iRate;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoBadGoodies() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsNoBadGoodies();
}

//	--------------------------------------------------------------------------------
int CvUnit::getRivalTerritoryCount() const
{
	VALIDATE_OBJECT
	return m_iRivalTerritoryCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isRivalTerritory() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsRivalTerritory() || getRivalTerritoryCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeRivalTerritoryCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRivalTerritoryCount = (m_iRivalTerritoryCount + iChange);
	CvAssert(getRivalTerritoryCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFound() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFound();
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundAbroad() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundAbroad();
}

#if defined(MOD_BALANCE_CORE_SETTLER_ADVANCED)
//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundMid() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundMid();
}
//	--------------------------------------------------------------------------------
bool CvUnit::IsFoundLate() const
{
	VALIDATE_OBJECT
	return m_pUnitInfo->IsFoundLate();
}

bool CvUnit::CanFoundColony() const
{
	VALIDATE_OBJECT
	if (m_pUnitInfo->GetNumColonyFound() <= 0)
		return false;

	int iNumFoundedPuppets = 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iLoop = 0;
	CvCity *pLoopCity = NULL;
	for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if (pLoopCity->getOriginalOwner() == kPlayer.GetID() && pLoopCity->IsPuppet())
			iNumFoundedPuppets++;
	}

	return iNumFoundedPuppets < m_pUnitInfo->GetNumColonyFound();
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsWork() const
{
	VALIDATE_OBJECT
	return (m_pUnitInfo->GetWorkRate() > 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isGoldenAge() const
{
	VALIDATE_OBJECT
	if(isDelayedDeath())
	{
		return false;
	}

	return m_pUnitInfo->GetGoldenAgeTurns() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isGivesPolicies() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

		return m_pUnitInfo->GetFreePolicies() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isBlastTourism() const
{
	VALIDATE_OBJECT
		if(isDelayedDeath())
		{
			return false;
		}

	return m_pUnitInfo->GetOneShotTourism() > 0 || m_pUnitInfo->GetTourismBonusTurns() > 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canCoexistWithEnemyUnit(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eTeam)
		return false;

	if (isDelayedDeath())
		return true;

	if(isInvisible(eTeam, false))
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIsSlowInEnemyLandCount() const
{
	VALIDATE_OBJECT
	return m_iIsSlowInEnemyLandCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isSlowInEnemyLand() const
{
	VALIDATE_OBJECT
	return getIsSlowInEnemyLandCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIsSlowInEnemyLandCount(int iChange)
{
	VALIDATE_OBJECT
	m_iIsSlowInEnemyLandCount = (m_iIsSlowInEnemyLandCount + iChange);
	CvAssert(getIsSlowInEnemyLandCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getRangedSupportFireCount() const
{
	VALIDATE_OBJECT
	return m_iRangedSupportFireCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isRangedSupportFire() const
{
	VALIDATE_OBJECT
	return getRangedSupportFireCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeRangedSupportFireCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRangedSupportFireCount = (m_iRangedSupportFireCount + iChange);
	CvAssert(getRangedSupportFireCount() >= 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFighting() const
{
	VALIDATE_OBJECT
	return (getCombatUnit() != NULL || getCombatCity() != NULL);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAttacking() const
{
	VALIDATE_OBJECT
	return (getAttackPlot() != NULL && !isDelayedDeath());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isDefending() const
{
	VALIDATE_OBJECT
	return (isFighting() && !isAttacking());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInCombat() const
{
	VALIDATE_OBJECT
	return (isFighting() || isAttacking());
}


//	--------------------------------------------------------------------------------
int CvUnit::GetMaxHitPoints() const
{
	VALIDATE_OBJECT

	int iMaxHP = getMaxHitPointsBase();

	iMaxHP *= (100 + getMaxHitPointsModifier());
	iMaxHP /= 100;
	
	iMaxHP += getMaxHitPointsChange();

	return iMaxHP;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetCurrHitPoints()	const
{
	VALIDATE_OBJECT
	return (GetMaxHitPoints() - getDamage());
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHurt() const
{
	VALIDATE_OBJECT
	return (getDamage() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsDead() const
{
	VALIDATE_OBJECT
	return (getDamage() >= GetMaxHitPoints());
}

//	--------------------------------------------------------------------------------
/// Over strategic resource limit?
int CvUnit::GetStrategicResourceCombatPenalty() const
{
	if (MOD_BALANCE_CORE_MILITARY_RESOURCES)
	{
		// units cannot heal anymore, but strength is unaffected
		return 0;
	}

	int iPenalty = 0;

	// barbs and city states don't require resources
	if (isBarbarian() || GET_PLAYER(getOwner()).isMinorCiv())
		return iPenalty;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// Loop through all resources
	ResourceTypes eResource;
	int iNumResourceInfos = GC.getNumResourceInfos();
	for(int iResourceLoop = 0; iResourceLoop < iNumResourceInfos; iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		int iAvailable = kPlayer.getNumResourceAvailable(eResource);

		// Over resource limit?
		if(iAvailable < 0)
		{
			if(m_pUnitInfo->GetResourceQuantityRequirement(eResource) > 0)
			{
				int iUsed = kPlayer.getNumResourceUsed(eResource);
				int iMissing = iUsed - kPlayer.getNumResourceTotal(eResource);

				CvAssertMsg(iUsed > 0, "Number of used resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iUsed <= 0)
					continue;
				CvAssertMsg(iMissing > 0, "Number of missing resources is zero or negative, this is unexpected. Please send Anton your save file and version.");
				if (iMissing <= 0)
					continue;

				double dDeficit = (double) iMissing / (double) iUsed;
				iPenalty += (int) floor((dDeficit) * /*-50 in CP, 0 in VP*/ GD_INT_GET(STRATEGIC_RESOURCE_EXHAUSTED_PENALTY)); // round down (for larger negative penalty)
			}
		}
	}

	//value is negative!
	return max(iPenalty, /*-50 in CP, 0 in VP*/ GD_INT_GET(STRATEGIC_RESOURCE_EXHAUSTED_PENALTY));
}

//	--------------------------------------------------------------------------------
/// Unhappy?
int CvUnit::GetUnhappinessCombatPenalty() const
{
	return GET_PLAYER(getOwner()).GetUnhappinessCombatStrengthPenalty();
}

//	--------------------------------------------------------------------------------
void CvUnit::SetBaseCombatStrength(int iCombat)
{
	VALIDATE_OBJECT
	m_iBaseCombat = iCombat;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetBaseCombatStrength() const
{
	return m_iBaseCombat;
}

int CvUnit::GetBestAttackStrength() const
{
	int iRangedStrength = GetMaxRangedCombatStrength(NULL,NULL,true,NULL,NULL,true,true);
	int iMeleeStrength = GetMaxAttackStrength(NULL,NULL,NULL,true,true);

	return max(iRangedStrength,iMeleeStrength);
}

//typically negative
int CvUnit::GetDamageCombatModifier(bool bForDefenseAgainstRanged, int iAssumedDamage) const
{
	int iDamageValueToUse = iAssumedDamage > 0 ? iAssumedDamage : getDamage();
	int iWoundedDamageMultiplier = /*33*/ GD_INT_GET(WOUNDED_DAMAGE_MULTIPLIER);
	int iRtnValue = iDamageValueToUse > 0 ? GET_PLAYER(getOwner()).GetWoundedUnitDamageMod() * -1 : 0; // usually 0, +25% with vanilla Elite Forces if wounded

	// Unit is stronger when damaged (Tenacity)
	if (iDamageValueToUse > 0 && IsStrongerDamaged())
	{
		iRtnValue += (iDamageValueToUse * iWoundedDamageMultiplier) / 100;
		return iRtnValue;
	}

	// Option: Damage modifier does not apply for defense against ranged attack (fewer targets -> harder to hit)
	// Units that fight well damaged do not take a penalty from being wounded
	if (bForDefenseAgainstRanged && MOD_BALANCE_CORE_RANGED_ATTACK_PENALTY)
	{
		return iRtnValue;
	}

	// How much does damage weaken the effectiveness of the Unit?
	if (iDamageValueToUse > 0 && !IsFightWellDamaged() && !GET_PLAYER(getOwner()).GetPlayerTraits()->IsFightWellDamaged())
	{
		iRtnValue -= (iDamageValueToUse * iWoundedDamageMultiplier) / 100;
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
/// What are the generic strength modifiers for this Unit?
int CvUnit::GetGenericMeleeStrengthModifier(const CvUnit* pOtherUnit, const CvPlot* pBattlePlot, bool bAttacking,
				bool bIgnoreUnitAdjacencyBoni, const CvPlot* pFromPlot, bool bQuickAndDirty) const
{
	VALIDATE_OBJECT

	if (pFromPlot == NULL)
		pFromPlot = plot();

	// Generic combat bonus
	int iModifier = getExtraCombatPercent() + GetStrategicResourceCombatPenalty();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eStateReligion = kPlayer.GetReligions()->GetStateReligion();

	// If the empire is unhappy, then Units get a combat penalty
	if (kPlayer.IsEmpireUnhappy())
	{
		iModifier += GetUnhappinessCombatPenalty();
	}

#if defined(MOD_BALANCE_CORE)
	int iCSStrengthMod = 0;
	if(GET_PLAYER(getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(getOwner()).GetMinorCivAI()->GetAlly();
		if(eAlly != NO_PLAYER)
		{
			int iCSBonus = GET_PLAYER(eAlly).GetPlayerTraits()->GetAllianceCSStrength();
			if(iCSBonus > 0)
			{
				int iNumAllies = GET_PLAYER(eAlly).GetNumCSAllies();
				if(iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
				{
					iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
				}
				iCSStrengthMod = (iCSBonus * iNumAllies);
			}
		}
	}
	else
	{
		int iCSBonus = GET_PLAYER(getOwner()).GetPlayerTraits()->GetAllianceCSStrength();
		if(iCSBonus > 0)
		{
			int iNumAllies = GET_PLAYER(getOwner()).GetNumCSAllies();
			if(iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
			{
				iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
			}
			iCSStrengthMod = (iCSBonus * iNumAllies);
		}
	}
	iModifier += iCSStrengthMod;
#endif

	//sometimes we ignore the finer points
	if (!bQuickAndDirty)
	{
		const CvPlot* pMyPlot = bAttacking ? pFromPlot : pBattlePlot;

		// Great General nearby
		if (!IsIgnoreGreatGeneralBenefit())
		{
			iModifier += kPlayer.GetAreaEffectModifier(AE_GREAT_GENERAL, getDomainType(), pMyPlot);
		}

		// Reverse Great General nearby
		iModifier += GetReverseGreatGeneralModifier(pMyPlot);

		// Improvement with combat bonus (from trait) nearby
		iModifier += GetNearbyImprovementModifier(pMyPlot);

		// UnitClass grants a combat bonus if nearby
		iModifier += GetNearbyUnitClassModifierFromUnitClass(pMyPlot);

		// Near city bonus
		iModifier += GetNearbyCityBonusCombatMod(pMyPlot);

		// NearbyUnit gives a Combat Modifier?
		if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
		{
			iModifier += GetGiveCombatModToUnit(pMyPlot);
		}

		// Modifier if no adjacent friendly unit
		if (!bIgnoreUnitAdjacencyBoni && !pMyPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
		{
			iModifier += GetNoAdjacentUnitModifier();
		}
		// Adjacent Friendly military Unit?
		if (!bIgnoreUnitAdjacencyBoni && pMyPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
		{
			iModifier += GetAdjacentModifier();
			for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++) // Stuff for per adjacent unit combat
			{
				const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
				int iModPerAdjacent = getCombatModPerAdjacentUnitCombatModifier(eUnitCombat);
				if (iModPerAdjacent != 0)
				{
					int iNumFriendliesAdjacent = pMyPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(getTeam(), eUnitCombat, NULL);
					iModifier += (iNumFriendliesAdjacent * iModPerAdjacent);
				}
			}
		}
	}

	// Our empire fights well in Golden Ages?
	if(kPlayer.isGoldenAge())
		iModifier += kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier();

	// Anti-Warmonger Fervor
	if (pOtherUnit != NULL)
		iModifier += GetResistancePower(pOtherUnit);

	// Stacked with Great General
	if (GetGreatGeneralCombatModifier() != 0 && IsStackedGreatGeneral())
	{
		iModifier += GetGreatGeneralCombatModifier();
	}

	////////////////////////
	// KNOWN BATTLE PLOT
	////////////////////////

	if(pBattlePlot != NULL)
	{
		// Open Ground
		if (pBattlePlot->isOpenGround())
			iModifier += getExtraOpenFromPercent();
		// Rough Ground
		else if (pBattlePlot->isRoughGround())
			iModifier += getExtraRoughFromPercent();

		// Bonuses for fighting in one's lands
		if(pBattlePlot->IsFriendlyTerritory(getOwner()))
		{
			iModifier += getFriendlyLandsModifier();

			// Founder Belief bonus
			CvCity* pPlotCity = bQuickAndDirty ? NULL : pBattlePlot->getOwningCity();
			if(pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if(eReligion != NO_RELIGION && eReligion == eStateReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if(pCityReligion)
					{
						CvCity* pHolyCity = pCityReligion->GetHolyCity();
						iModifier += pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities(getOwner(), pHolyCity);
					}
				}
			}
		}
		// Bonuses outside one's lands
		else
		{
			iModifier += getOutsideFriendlyLandsModifier();

			// Bonus against city states?
			if(pBattlePlot->isCity() && pBattlePlot->getOwner()!=NO_PLAYER && GET_PLAYER(pBattlePlot->getOwner()).isMinorCiv())
			{
				iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
#if defined(MOD_BALANCE_CORE)
				iModifier += kPlayer.GetCityStateCombatModifier();
#endif
			}
#if defined(MOD_BALANCE_CORE)
			// Bonus against Major Civ Cities
			if(pBattlePlot->isCity() && pBattlePlot->getOwner()!=NO_PLAYER && GET_PLAYER(pBattlePlot->getOwner()).isMajorCiv())
			{
				if(kPlayer.isGoldenAge())
				{
					iModifier += kPlayer.GetPlayerTraits()->GetConquestOfTheWorldCityAttack();
				}
			}
#endif
			// Founder Belief bonus
			CvCity* pPlotCity = bQuickAndDirty ? NULL : pBattlePlot->getOwningCity();
			if(pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if(eReligion != NO_RELIGION && eReligion == eStateReligion)
				{
					const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
					if(pCityReligion)
					{
						CvCity* pHolyCity = pCityReligion->GetHolyCity();
						iModifier += pCityReligion->m_Beliefs.GetCombatModifierEnemyCities(getOwner(), pHolyCity);
					}
				}
			}
		}

		if (MOD_BALANCE_CORE_BELIEFS && pOtherUnit != NULL && getDomainType() == DOMAIN_LAND)
		{
			if (!pOtherUnit->isBarbarian() && !GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv() && pOtherUnit->getOwner() != NO_PLAYER)
			{
				ReligionTypes eOwnedReligion = kPlayer.GetReligions()->GetOwnedReligion();
				ReligionTypes eTheirReligion = GET_PLAYER(pOtherUnit->getOwner()).GetReligions()->GetStateReligion();

				if (eOwnedReligion != NO_RELIGION)
				{
					const CvReligion* pReligion = pReligions->GetReligion(eOwnedReligion, getOwner());
					if (pReligion)
					{
						CvCity* pHolyCity = pReligion->GetHolyCity();

						//Full bonus against different religion
						int iScaler = (eTheirReligion != eOwnedReligion) ? 1 : 2;
						int iOtherOwn = pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands(getOwner(), pHolyCity);
						int iOtherTheir = pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands(getOwner(), pHolyCity);

						// Bonus in own land
						if((iOtherOwn > 0) && pBattlePlot->IsFriendlyTerritory(getOwner()))
							iModifier += iOtherOwn/iScaler;
						//Bonus in their land
						if((iOtherTheir > 0) && pBattlePlot->IsFriendlyTerritory(pOtherUnit->getOwner()))
							iModifier += iOtherTheir/iScaler;
					}
				}
			}
		}

		// Capital Defense
		if (GetCapitalDefenseModifier() > 0 || GetCapitalDefenseFalloff() > 0)
		{
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			if(pCapital)
			{
				int iDistanceToCapital = plotDistance(pBattlePlot->getX(), pBattlePlot->getY(), pCapital->getX(), pCapital->getY());
				int iTempModifier = GetCapitalDefenseModifier() + iDistanceToCapital * GetCapitalDefenseFalloff();
				if (iTempModifier > 0)
				{
					iModifier += iTempModifier;
				}
			}
		}

		// Trait (player level) bonus against higher tech units
		// Only applies defending friendly territory
		if(pBattlePlot->getOwner() == getOwner())
		{
			int	iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsHigherTech();
			if (iTempModifier > 0)
			{
				// Check tech levels too
				UnitTypes eMyUnitType = getUnitType();
				if (pOtherUnit && pOtherUnit->IsHigherTechThan(eMyUnitType))
				{
					iModifier += iTempModifier;
				}
			}
		}
	}

	////////////////////////
	// OTHER UNIT IS KNOWN
	////////////////////////

	if(pOtherUnit != NULL)
	{
		CvAssertMsg(pOtherUnit != this, "Compared combat strength against one's own pointer. This is weird and probably wrong.");

		// Generic Unit Class Modifier
		iModifier += getUnitClassModifier(pOtherUnit->getUnitClassType());

		// Unit Combat type Modifier
		UnitCombatTypes combatType = (UnitCombatTypes)pOtherUnit->getUnitCombatType();
		if(combatType != NO_UNITCOMBAT)
		{
			int iTempModifier = unitCombatModifier(combatType);

			//hack: mounted units can have secondary combat class
			UnitCombatTypes mountedCombat = (UnitCombatTypes)2; //hardcoded
			if (pOtherUnit->getUnitInfo().IsMounted() && combatType != mountedCombat)
				iTempModifier += unitCombatModifier(mountedCombat);

			iModifier += iTempModifier;
		}

		// Domain Modifier
		iModifier += domainModifier(pOtherUnit->getDomainType());

		// Bonus against city states?
		if(GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += kPlayer.GetPlayerTraits()->GetCityStateCombatModifier();
			iModifier += kPlayer.GetCityStateCombatModifier();
		}

		// OTHER UNIT is a Barbarian
		if(pOtherUnit->isBarbarian())
		{
			// Generic Barb Combat Bonus
			iModifier += kPlayer.GetBarbarianCombatBonus(false);
			iModifier += GetBarbarianCombatBonus();
		}

		// Trait (player level) bonus against larger civs
		if (pOtherUnit->IsLargerCivThan(this))
		{
			iModifier += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
		}
	}

	return iModifier;
}

//	--------------------------------------------------------------------------------
/// What is the max strength of this Unit when attacking?
int CvUnit::GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender, 
								bool bIgnoreUnitAdjacencyBoni, bool bQuickAndDirty, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT
	if(GetBaseCombatStrength() == 0)
		return 0;

	int iModifier = GetGenericMeleeStrengthModifier(pDefender, pToPlot, true, bIgnoreUnitAdjacencyBoni, pFromPlot, bQuickAndDirty);

	// Generic Attack bonus
	iModifier += getAttackModifier();

	// Damage modifier always applies for melee attack
	iModifier += GetDamageCombatModifier(false, getDamage() + iAssumeExtraDamage);

	// Temporary attack bonus (Policies, etc.)
	if (GET_PLAYER(getOwner()).GetAttackBonusTurns() > 0)
		iModifier += /*25*/ GD_INT_GET(POLICY_ATTACK_BONUS_MOD);

	//resource monopolies
	iModifier += GET_PLAYER(getOwner()).GetCombatAttackBonusFromMonopolies();

	// Adjacent Friendly military Unit? (attack mod only)
	if (pFromPlot != NULL && !bIgnoreUnitAdjacencyBoni && !bQuickAndDirty && pFromPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
	{
		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++) // Stuff for per adjacent unit combat
		{
			const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
			int iModPerAdjacent = getCombatModPerAdjacentUnitCombatAttackMod(eUnitCombat);
			if (pkUnitCombatInfo && iModPerAdjacent != 0)
			{
				int iNumFriendliesAdjacent = 0;
				iNumFriendliesAdjacent += pFromPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(getTeam(), eUnitCombat, NULL);
				iModifier += (iNumFriendliesAdjacent * iModPerAdjacent);
			}
		}
	}

	////////////////////////
	// KNOWN DESTINATION PLOT
	////////////////////////

	if(pToPlot != NULL)
	{
		// Attacking a City
		if(pToPlot->isCity())
		{
			iModifier += cityAttackModifier();

			// City is blockaded
			if (pToPlot->getPlotCity()->IsBlockadedWaterAndLand())
				iModifier += max(0, /*0 in CP, 20 in VP*/ GD_INT_GET(BLOCKADED_CITY_ATTACK_MODIFIER));

			// Nearby unit sapping this city
			iModifier += GET_PLAYER(getOwner()).GetAreaEffectModifier(AE_SAPPER, NO_DOMAIN, pToPlot);

			//bonus for attacking same unit over and over in a turn?
			int iTempModifier = getMultiAttackBonus() + GET_PLAYER(getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
			if (iTempModifier != 0)
			{
				iTempModifier *= pToPlot->getPlotCity()->GetNumTimesAttackedThisTurn(getOwner());
				iModifier += iTempModifier;
			}

			// City Defending against a Barbarian
			if (isBarbarian())
				iModifier += /*0*/ GD_INT_GET(BARBARIAN_CITY_ATTACK_MODIFIER);
		}
		// Some bonuses only apply when NOT attacking a city
		else
		{
			// Attacking into Hills
			if(pToPlot->isHills())
				iModifier += hillsAttackModifier();

			// Attacking into Open Ground
			if(pToPlot->isOpenGround())
				iModifier += openAttackModifier();

			// Attacking into Rough Ground
			if(pToPlot->isRoughGround())
				iModifier += roughAttackModifier();

			// Attacking into a Feature
			if(pToPlot->getFeatureType() != NO_FEATURE)
				iModifier += featureAttackModifier(pToPlot->getFeatureType());
			// No Feature - Use Terrain Attack Mod
			else
			{
				iModifier += terrainAttackModifier(pToPlot->getTerrainType());

				// Tack on Hills Attack Mod
				if(pToPlot->isHills())
					iModifier += terrainAttackModifier(TERRAIN_HILL);
			}
		}

		////////////////////////
		// KNOWN ORIGIN PLOT
		////////////////////////

		if(pFromPlot != NULL)
		{
			// Attacking across a river
			if(!isRiverCrossingNoPenalty())
			{
				if(pFromPlot->isAdjacent(pToPlot) && pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
					iModifier += /*-20*/ GD_INT_GET(RIVER_ATTACK_MODIFIER);
			}

			// Amphibious attack
			if(!isAmphibious())
			{
				if(!isNativeDomain(pFromPlot))
					iModifier += /*-50*/ GD_INT_GET(AMPHIB_ATTACK_MODIFIER);
			}

			// Flanking
			if(!bIgnoreUnitAdjacencyBoni && !bQuickAndDirty)
			{
				iModifier += pFromPlot->GetEffectiveFlankingBonus(this, pDefender, pToPlot);
			}

			if (pFromPlot->IsFriendlyTerritory(getOwner()))
				iModifier += getFriendlyLandsAttackModifier();
		}
	}

	////////////////////////
	// KNOWN DEFENDER
	////////////////////////

	if(pDefender != NULL)
	{
		CvAssertMsg(pDefender != this, "Compared attack strength against one's own pointer. This is weird and probably wrong.");

		// Unit Class Attack Modifier
		iModifier += unitClassAttackModifier(pDefender->getUnitClassType());

		// Domain Attack Modifier
		iModifier += getExtraDomainAttack(pDefender->getDomainType());

		// Bonus VS fortified
		if(pDefender->IsFortified())
			iModifier += attackFortifiedModifier();

		// Bonus VS wounded
		if (pDefender->getDamage() > 0)
			iModifier += attackWoundedModifier();
		else
			iModifier += attackFullyHealedModifier();

		//More than half?
		if (pDefender->getDamage() < (pDefender->GetMaxHitPoints()/2))
			iModifier += attackAbove50HealthModifier();
		else
			iModifier += attackBelow50HealthModifier();

		//Heavy charge without escape
		if (IsCanHeavyCharge() && pDefender->GetNumFallBackPlotsAvailable(*this)==0)
		{
			if (MOD_ATTRITION)
				iModifier += 25;
			else
				iModifier += 50;
		}
		//bonus for attacking same unit over and over in a turn?
		int iTempModifier = getMultiAttackBonus() + GET_PLAYER(getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
		if (iTempModifier != 0)
		{
			iTempModifier *= pDefender->GetNumTimesAttackedThisTurn(getOwner());
			iModifier += iTempModifier;
		}
	}

	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	int iCombat = GetBaseCombatStrength() * (iModifier + 100);

	return std::max(1, iCombat);
}

//	--------------------------------------------------------------------------------
/// What is the max strength of this Unit when defending?
int CvUnit::GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker, const CvPlot* pFromPlot,
								bool bFromRangedAttack, bool bQuickAndDirty, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT

	//no modifiers for embarked defense
	if ( (!pInPlot && isEmbarked()) || (pInPlot && pInPlot->needsEmbarkation(this) && CanEverEmbark()) )
		return GetEmbarkedUnitDefense();

	int iCombat = GetBaseCombatStrength();
	if (iCombat==0)
		return 0;

	int iModifier = GetGenericMeleeStrengthModifier(pAttacker, pInPlot, false, false, pFromPlot, bQuickAndDirty);

	// Generic Defense Bonus
	iModifier += getDefenseModifier(bQuickAndDirty);

	// Fortification
	iModifier += fortifyModifier();

	// Defense against Ranged
	if (bFromRangedAttack)
		iModifier += rangedDefenseModifier();

	// this may be always zero for defense against ranged
	iModifier += GetDamageCombatModifier(bFromRangedAttack, getDamage() + iAssumeExtraDamage);

	//resource monopolies
	iModifier += GET_PLAYER(getOwner()).GetCombatDefenseBonusFromMonopolies();

#if defined(MOD_BALANCE_CORE)
	// Adjacent Friendly military Unit? (defense mod only)
	if (pInPlot != NULL && !bQuickAndDirty && pInPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
	{
		for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++) // Stuff for per adjacent unit combat
		{
			const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
			int iModPerAdjacent = getCombatModPerAdjacentUnitCombatDefenseMod(eUnitCombat);
			if (pkUnitCombatInfo && iModPerAdjacent != 0)
			{
				int iNumFriendliesAdjacent = 0;
				iNumFriendliesAdjacent += pInPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(getTeam(), eUnitCombat, NULL);
				iModifier += (iNumFriendliesAdjacent * iModPerAdjacent);
			}
		}
	}
#endif

	////////////////////////
	// KNOWN DEFENSE PLOT
	////////////////////////

	if(pInPlot != NULL)
	{
		// No TERRAIN bonuses for this Unit?
		int iTempModifier = pInPlot->defenseModifier(getTeam(), false, false);
		if (noDefensiveBonus() && iTempModifier>0)
			//only forts & citadels have an effect
			iTempModifier -= pInPlot->defenseModifier(getTeam(), true, false);
		iModifier += iTempModifier;

		// City Defense
		if(pInPlot->isCity())
			iModifier += cityDefenseModifier();

		// Hill Defense
		if(pInPlot->isHills())
			iModifier += hillsDefenseModifier();

		// Open Ground Defense
		if(pInPlot->isOpenGround())
			iModifier += openDefenseModifier();

		// Rough Ground Defense
		if(pInPlot->isRoughGround())
			iModifier += roughDefenseModifier();

		// Feature Defense
		if(pInPlot->getFeatureType() != NO_FEATURE)
			iModifier += featureDefenseModifier(pInPlot->getFeatureType());
		// No Feature - use Terrain Defense Mod
		else
		{
			iModifier += terrainDefenseModifier(pInPlot->getTerrainType());

			// Tack on Hills Defense Mod
			if(pInPlot->isHills())
				iModifier += terrainDefenseModifier(TERRAIN_HILL);
		}

		// Flanking
		if (pFromPlot && !bFromRangedAttack && !bQuickAndDirty)
			iModifier += pInPlot->GetEffectiveFlankingBonus(this, pAttacker, pFromPlot);
	}

	////////////////////////
	// KNOWN ATTACKER
	////////////////////////

	if(pAttacker != NULL)
	{
		// Unit Class Defense Modifier
		iModifier += unitClassDefenseModifier(pAttacker->getUnitClassType());

		// Domain Defense Modifier
		iModifier += getExtraDomainDefense(pAttacker->getDomainType());
	}

	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	// finally apply the modifier
	iCombat *= (iModifier + 100);

	// Boats do more damage VS one another
	if (pAttacker && pAttacker->getDomainType() == DOMAIN_SEA && getDomainType() == DOMAIN_SEA)
	{
		iCombat *= /*100*/ GD_INT_GET(NAVAL_COMBAT_DEFENDER_STRENGTH_MULTIPLIER);
		iCombat /= 100;
	}

	return std::max(1, iCombat);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetEmbarkedUnitDefense() const
{
	//embarked strength is simply base combat strength without any promotions
	int iRtnValue = GetBaseCombatStrength() * 100;

	//except for one
	int iModifier = GetEmbarkDefensiveModifier();
	if(iModifier != 0)
	{
		iRtnValue = iRtnValue * (100+iModifier);
		iRtnValue /= 100;
	}

	return iRtnValue;
}
//	--------------------------------------------------------------------------------
int CvUnit::GetResistancePower(const CvUnit* pOtherUnit) const
{
	if (pOtherUnit->getOwner() == NO_PLAYER)
		return 0;

	if (pOtherUnit->isBarbarian() || isBarbarian())
		return 0;

	if (GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv() || GET_PLAYER(getOwner()).isMinorCiv())
		return 0;

	// No bonus if we're attacking in their territory
	if (plot()->getOwner() == pOtherUnit->getOwner())
		return 0;

	int iResistance = GET_PLAYER(getOwner()).GetDominationResistance(pOtherUnit->getOwner());

	// Not our territory?
	if (!plot()->IsFriendlyTerritory(getOwner()))
		iResistance /= 2;

	return iResistance;
}
//	--------------------------------------------------------------------------------
bool CvUnit::canSiege(TeamTypes eTeam) const
{
	VALIDATE_OBJECT
	if(!IsCanDefend())
	{
		return false;
	}

	if(!isEnemy(eTeam))
	{
		return false;
	}

	if(!isNeverInvisible())
	{
		return false;
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetBaseRangedCombatStrength() const
{
	VALIDATE_OBJECT
	return m_iBaseRangedCombat;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetBaseRangedCombatStrength(int iStrength)
{
	VALIDATE_OBJECT

	m_iBaseRangedCombat = iStrength;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetMaxRangedCombatStrength(const CvUnit* pOtherUnit, const CvCity* pCity, bool bAttacking,
	const CvPlot* pMyPlot, const CvPlot* pOtherPlot, bool bIgnoreUnitAdjacencyBoni, bool bQuickAndDirty, int iAssumeExtraDamage) const
{
	VALIDATE_OBJECT

	if (pMyPlot == NULL)
		pMyPlot = plot();

	if (pOtherPlot == NULL)
	{
		if (pOtherUnit != NULL)
		{
			pOtherPlot = pOtherUnit->plot();
		}
		else if (pCity != NULL)
		{
			pOtherPlot = pCity->plot();
		}
	}

	const CvPlot* pTargetPlot = bAttacking ? pOtherPlot : pMyPlot;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	CvPlayerTraits* pTraits = kPlayer.GetPlayerTraits();
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	ReligionTypes eStateReligion = kPlayer.GetReligions()->GetStateReligion();

	int iBaseStrength = GetBaseRangedCombatStrength();

	//fake ranged unit (impi)
	if (isRangedSupportFire())
		iBaseStrength = GetBaseCombatStrength() / 2;

	if (iBaseStrength == 0)
	{
		return 0;
	}

	// Extra combat percent
	int iModifier = getExtraCombatPercent() + GetStrategicResourceCombatPenalty();

	// If the empire is unhappy, then Units get a combat penalty
	if (kPlayer.IsEmpireUnhappy())
	{
		iModifier += GetUnhappinessCombatPenalty();
	}

	// sometimes we want to ignore the finer points
	if (!bQuickAndDirty)
	{
		if (!bIgnoreUnitAdjacencyBoni)
		{
			// Adjacent Friendly military Unit?
			if (pMyPlot->IsFriendlyUnitAdjacent(getTeam(), /*bCombatUnit*/ true))
			{
				iModifier += GetAdjacentModifier();
				for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++) // Stuff for per adjacent unit combat
				{
					const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
					int iModPerAdjacent = getCombatModPerAdjacentUnitCombatModifier(eUnitCombat);
					iModPerAdjacent += bAttacking ? getCombatModPerAdjacentUnitCombatAttackMod(eUnitCombat) : getCombatModPerAdjacentUnitCombatDefenseMod(eUnitCombat);
					if (iModPerAdjacent != 0)
					{
						int iNumFriendliesAdjacent = pMyPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(getTeam(), eUnitCombat, NULL);
						iModifier += (iNumFriendliesAdjacent * iModPerAdjacent);
					}
				}
			}
			else
			{
				// no friendly military unit adjacent
				iModifier += GetNoAdjacentUnitModifier();
			}

			// Great General nearby
			if (!IsIgnoreGreatGeneralBenefit())
			{
				iModifier += kPlayer.GetAreaEffectModifier(AE_GREAT_GENERAL, getDomainType(), pMyPlot);
			}

			// Reverse Great General nearby
			iModifier += GetReverseGreatGeneralModifier(pMyPlot);

			// NearbyUnit gives a Combat Modifier?
			if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
			{
				iModifier += GetGiveCombatModToUnit(pMyPlot);
			}
		}

		// Improvement with combat bonus (from trait) nearby
		iModifier += GetNearbyImprovementModifier();

		// UnitClass grants a combat bonus if nearby
		iModifier += GetNearbyUnitClassModifierFromUnitClass();

		// Near city bonus
		iModifier += GetNearbyCityBonusCombatMod(pMyPlot);
	}

	// Our empire fights well in Golden Ages?
	if(kPlayer.isGoldenAge())
		iModifier += pTraits->GetGoldenAgeCombatModifier();

	// Anti-Warmonger Fervor
	if (pOtherUnit != NULL)
		iModifier += GetResistancePower(pOtherUnit);
	
	// Trait bonus when allied with City States
	int iCSStrengthMod = 0;
	if (GET_PLAYER(getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(getOwner()).GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER)
		{
			int iCSBonus = GET_PLAYER(eAlly).GetPlayerTraits()->GetAllianceCSStrength();
			if (iCSBonus > 0)
			{
				int iNumAllies = GET_PLAYER(eAlly).GetNumCSAllies();
				if(iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
				{
					iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
				}
				iCSStrengthMod = iCSBonus * iNumAllies;
			}
		}
	}
	else
	{
		int iCSBonus = GET_PLAYER(getOwner()).GetPlayerTraits()->GetAllianceCSStrength();
		if (iCSBonus > 0)
		{
			int iNumAllies = GET_PLAYER(getOwner()).GetNumCSAllies();
			if (iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
			{
				iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
			}
			iCSStrengthMod = iCSBonus * iNumAllies;
		}
	}
	iModifier += iCSStrengthMod;

	// Stacked with Great General
	if (GetGreatGeneralCombatModifier() != 0 && IsStackedGreatGeneral())
	{
		iModifier += GetGreatGeneralCombatModifier();
	}

	// Resource monopoly
	if (bAttacking)
		iModifier += GET_PLAYER(getOwner()).GetCombatAttackBonusFromMonopolies();
	else
		iModifier += GET_PLAYER(getOwner()).GetCombatDefenseBonusFromMonopolies();

	if (pTargetPlot)
	{
		////////////////////////
		// KNOWN BATTLE PLOT
		////////////////////////

		// ATTACKING
		if (bAttacking)
		{
			// Flanking
			if(IsRangedFlankAttack() && !bIgnoreUnitAdjacencyBoni && !bQuickAndDirty)
			{
				iModifier += pTargetPlot->GetEffectiveFlankingBonusAtRange(this, pOtherUnit);
			}

			// These only work when not attacking a city
			if (pCity == NULL)
			{
				// Attacking into Hills
				if (pTargetPlot->isHills())
					iModifier += hillsAttackModifier();

				// Attacking into Open Terrain
				if (pTargetPlot->isOpenGround())
					iModifier += openRangedAttackModifier();
				// Attacking into Rough Terrain
				else if (pTargetPlot->isRoughGround())
					iModifier += roughRangedAttackModifier();

				// Attacking into a Feature
				if (pTargetPlot->getFeatureType() != NO_FEATURE)
				{
					iModifier += featureAttackModifier(pTargetPlot->getFeatureType());
				}
				// No Feature - Use Terrain Attack Mod
				else
				{
					iModifier += terrainAttackModifier(pTargetPlot->getTerrainType());

					// Tack on Hills Attack Mod
					if (pTargetPlot->isHills())
						iModifier += terrainAttackModifier(TERRAIN_HILL);
				}
			}
		}
		else
		{
			// City Defense
			if (pMyPlot->isCity())
				iModifier += cityDefenseModifier();

			// Hill Defense
			if (pMyPlot->isHills())
				iModifier += hillsDefenseModifier();

			// Open Ground Defense
			if (pMyPlot->isOpenGround())
				iModifier += openDefenseModifier();

			// Rough Ground Defense
			if (pMyPlot->isRoughGround())
				iModifier += roughDefenseModifier();

			// Feature Defense
			if (pMyPlot->getFeatureType() != NO_FEATURE)
			{
				iModifier += featureDefenseModifier(pMyPlot->getFeatureType());
			}
			// No Feature - use Terrain Defense Mod
			else
			{
				iModifier += terrainDefenseModifier(pMyPlot->getTerrainType());

				// Tack on Hills Defense Mod
				if (pMyPlot->isHills())
					iModifier += terrainDefenseModifier(TERRAIN_HILL);
			}
		}

		// Bonus for fighting in one's lands
		if (pMyPlot->IsFriendlyTerritory(getOwner()))
		{
			iModifier += getFriendlyLandsModifier();
			if (bAttacking)
				iModifier += getFriendlyLandsAttackModifier();

			// Founder Belief bonus
			CvCity* pPlotCity = bQuickAndDirty ? NULL : pTargetPlot->getOwningCity();
			if (pPlotCity)
			{
				ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
				if (eReligion != NO_RELIGION && eReligion == eStateReligion)
				{
					const CvReligion* pCityReligion = pReligions->GetReligion(eReligion, pPlotCity->getOwner());
					if (pCityReligion)
					{
						CvCity* pHolyCity = pCityReligion->GetHolyCity();
						iModifier += pCityReligion->m_Beliefs.GetCombatModifierFriendlyCities(getOwner(), pHolyCity);
					}
				}
			}
		}

		// Bonus for fighting outside one's lands
		else
		{
			iModifier += getOutsideFriendlyLandsModifier();

			// Founder Belief bonus (this must be a city controlled by an enemy)
			CvCity* pPlotCity = bQuickAndDirty ? NULL : pTargetPlot->getOwningCity();
			if (pPlotCity)
			{
				if (atWar(getTeam(), pPlotCity->getTeam()))
				{
					ReligionTypes eReligion = pPlotCity->GetCityReligions()->GetReligiousMajority();
					if (eReligion != NO_RELIGION && eReligion == eStateReligion)
					{
						const CvReligion* pCityReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pPlotCity->getOwner());
						if (pCityReligion)
						{
							CvCity* pHolyCity = pCityReligion->GetHolyCity();
							iModifier += pCityReligion->m_Beliefs.GetCombatModifierEnemyCities(getOwner(), pHolyCity);
						}
					}
				}
			}
		}

		if (MOD_BALANCE_CORE_BELIEFS && pOtherUnit != NULL && getDomainType() == DOMAIN_LAND && !bQuickAndDirty)
		{
			if (!pOtherUnit->isBarbarian() && !GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv() && pOtherUnit->getOwner() != NO_PLAYER)
			{
				ReligionTypes eOwnedReligion = kPlayer.GetReligions()->GetOwnedReligion();
				ReligionTypes eTheirReligion = GET_PLAYER(pOtherUnit->getOwner()).GetReligions()->GetStateReligion();

				if (eOwnedReligion != NO_RELIGION)
				{
					const CvReligion* pReligion = pReligions->GetReligion(eOwnedReligion, getOwner());
					if (pReligion)
					{
						CvCity* pHolyCity = pReligion->GetHolyCity();

						//Full bonus against different religion
						int iScaler = (eTheirReligion != eOwnedReligion) ? 1 : 2;
						int iOtherOwn = pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands(getOwner(), pHolyCity);
						int iOtherTheir = pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands(getOwner(), pHolyCity);

						// Bonus in own land
						if((iOtherOwn > 0) && pTargetPlot->IsFriendlyTerritory(getOwner()))
							iModifier += iOtherOwn/iScaler;
						//Bonus in their land
						if((iOtherTheir > 0) && pTargetPlot->IsFriendlyTerritory(pOtherUnit->getOwner()))
							iModifier += iOtherTheir/iScaler;
					}
				}
			}
		}
	}

	////////////////////////
	// OTHER UNIT IS KNOWN
	////////////////////////

	if(NULL != pOtherUnit)
	{
		// Unit Class Mod
		iModifier += getUnitClassModifier(pOtherUnit->getUnitClassType());

		// Unit combat modifier VS other unit
		UnitCombatTypes eUnitCombat = (UnitCombatTypes) pOtherUnit->getUnitCombatType();
		if (eUnitCombat != NO_UNITCOMBAT)
		{
			int iTempModifier = unitCombatModifier(eUnitCombat);
			if (pOtherUnit->getUnitInfo().IsMounted())
			{
				UnitCombatTypes eMountedCombat = (UnitCombatTypes) GC.getInfoTypeForString("UNITCOMBAT_MOUNTED", true);
				if (eMountedCombat != eUnitCombat)
					iTempModifier += unitCombatModifier(eMountedCombat);
			}
			iModifier += iTempModifier;
		}

		// Domain modifier VS other unit
		iModifier += domainModifier(pOtherUnit->getDomainType());

		// Bonus against city states?
		if(GET_PLAYER(pOtherUnit->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
			iModifier += kPlayer.GetCityStateCombatModifier();
		}

		// OTHER UNIT is a Barbarian
		if(pOtherUnit->isBarbarian())
		{
			// Generic Barb Combat Bonus
			iModifier += kPlayer.GetBarbarianCombatBonus(false);
			iModifier += GetBarbarianCombatBonus();
		}

		// Trait bonus against higher tech units (only applies in owned territory)
		if (pMyPlot->getOwner() == getOwner())
		{
			int	iTempModifier = GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsHigherTech();
			if (iTempModifier > 0)
			{
				// Check tech levels too
				UnitTypes eMyUnitType = getUnitType();
				if (pOtherUnit->IsHigherTechThan(eMyUnitType))
				{
					iModifier += iTempModifier;
				}
			}
		}

		// Trait bonus against larger civs
		if (pOtherUnit->IsLargerCivThan(this))
		{
			iModifier += GET_PLAYER(getOwner()).GetPlayerTraits()->GetCombatBonusVsLargerCiv();
		}

		// ATTACKING
		if(bAttacking)
		{
			// Bonus VS fortified
			if (pOtherUnit->IsFortified())
				iModifier += attackFortifiedModifier();

			// Bonus VS wounded
			if (pOtherUnit->getDamage() > 0)
				iModifier += attackWoundedModifier();
			else
				iModifier += attackFullyHealedModifier();

			// More/less than half HP
			if (pOtherUnit->getDamage() < (pOtherUnit->GetMaxHitPoints() + 1) / 2)
				iModifier += attackAbove50HealthModifier();
			else
				iModifier += attackBelow50HealthModifier();

			// Bonus for attacking same unit over and over in a turn
			int iTempModifier = getMultiAttackBonus() + GET_PLAYER(getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
			if (iTempModifier != 0)
			{
				iTempModifier *= pOtherUnit->GetNumTimesAttackedThisTurn(getOwner());
				iModifier += iTempModifier;
			}

			// Unit Class Attack Mod
			iModifier += unitClassAttackModifier(pOtherUnit->getUnitClassType());

			// Domain Attack Mod
			iModifier += getExtraDomainAttack(pOtherUnit->getDomainType());
		}

		// Ranged DEFENSE
		else
		{
			// Unit Class Defense Mod
			iModifier += unitClassDefenseModifier(pOtherUnit->getUnitClassType());

			// Domain Defense Mod
			iModifier += getExtraDomainDefense(pOtherUnit->getDomainType());
		}
	}

	// Open Ground
	if (pMyPlot->isOpenGround())
		iModifier += getExtraOpenFromPercent();
	// Rough Ground
	else if (pMyPlot->isRoughGround())
		iModifier += getExtraRoughFromPercent();

	// Fighting near capital
	if (GetCapitalDefenseModifier() > 0 || GetCapitalDefenseFalloff() > 0)
	{
		CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
		if (pCapital)
		{
			int iDistanceToCapital = plotDistance(pMyPlot->getX(), pMyPlot->getY(), pCapital->getX(), pCapital->getY());
			int iTempModifier = GetCapitalDefenseModifier() + iDistanceToCapital * GetCapitalDefenseFalloff();
			if (iTempModifier > 0)
			{
				iModifier += iTempModifier;
			}
		}
	}

	////////////////////////
	// ATTACKING A CITY
	////////////////////////

	if(pCity != NULL)
	{
		// Attacking a City
		if (bAttacking)
		{
			iModifier += cityAttackModifier();

			// City is blockaded?
			if (pCity->IsBlockadedWaterAndLand())
				iModifier += max(0, /*0 in CP, 20 in VP*/ GD_INT_GET(BLOCKADED_CITY_ATTACK_MODIFIER));

			// Nearby unit sapping this city
			iModifier += kPlayer.GetAreaEffectModifier(AE_SAPPER, NO_DOMAIN, pOtherPlot);

			//bonus for attacking same unit over and over in a turn?
			int iTempModifier = getMultiAttackBonus() + GET_PLAYER(getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
			if (iTempModifier != 0)
			{
				iTempModifier *= pCity->GetNumTimesAttackedThisTurn(getOwner());
				iModifier += iTempModifier;
			}
		}

		// Bonus against city states?
		if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			iModifier += pTraits->GetCityStateCombatModifier();
			iModifier += kPlayer.GetCityStateCombatModifier();
		}
		// Bonus against Major Civ Cities
		if(GET_PLAYER(pCity->getOwner()).isMajorCiv())
		{
			if(kPlayer.isGoldenAge())
			{
				iModifier += pTraits->GetConquestOfTheWorldCityAttack();
			}
		}
	}

	// Ranged attack mod
	if(bAttacking)
	{
		iModifier += GetRangedAttackModifier();
		iModifier += getAttackModifier();
		if (pMyPlot->isCity())
		{
			CvCity* pMyCity = pMyPlot->getPlotCity();
			iModifier += pMyCity->getGarrisonRangedAttackModifier();
		}
	}
	else
	{
		// Regular defense modifier
		iModifier += getDefenseModifier(bQuickAndDirty);

		// Ranged Defense Mod
		iModifier += rangedDefenseModifier();

		// Fortification
		iModifier += fortifyModifier();

		// No TERRAIN bonuses for this Unit?
		int iTempModifier = pMyPlot->defenseModifier(getTeam(), false, false);

		if (noDefensiveBonus() && iTempModifier>0)
			//only forts & citadels have an effect
			iTempModifier -= pMyPlot->defenseModifier(getTeam(), true, false);

		iModifier += iTempModifier;
	}

	//this may be always zero when defending (on defense -> fewer targets, harder to hit)
	iModifier += GetDamageCombatModifier(!bAttacking, getDamage() + iAssumeExtraDamage);
	
	// Unit can't drop below 10% strength
	if(iModifier < -90)
		iModifier = -90;

	return std::max(1, iBaseStrength * (iModifier + 100));
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAirDefend(const CvPlot* pPlot) const
{
	VALIDATE_OBJECT
	if(pPlot == NULL)
	{
		pPlot = plot();
	}

	if(getInterceptChance() == 0)
	{
		return false;
	}

	if(getDomainType() != DOMAIN_AIR)
	{
		if(!isNativeDomain(pPlot))
		{
			return false;
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetAirCombatDamage(const CvUnit* pDefender, const CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage, 
						const CvPlot* pTargetPlot, const CvPlot* pFromPlot, bool bQuickAndDirty) const
{
	return GetRangeCombatDamage(pDefender,pCity,bIncludeRand,iAssumeExtraDamage,pTargetPlot,pFromPlot,false,bQuickAndDirty);
}


//	--------------------------------------------------------------------------------
int CvUnit::GetRangeCombatDamage(const CvUnit* pDefender, const CvCity* pCity, bool bIncludeRand, int iAssumeExtraDamage, 
	const CvPlot* pTargetPlot, const CvPlot* pFromPlot, bool bIgnoreUnitAdjacencyBoni, bool bQuickAndDirty) const
{
	VALIDATE_OBJECT
	if (pFromPlot == NULL)
	{
		pFromPlot = plot();
	}
	if (pTargetPlot == NULL)
	{
		if (pDefender != NULL)
		{
			pTargetPlot = pDefender->plot();
		}
		else if (pCity != NULL)
		{
			pTargetPlot = pCity->plot();
		}
	}

	int iAttackerStrength = GetMaxRangedCombatStrength(pDefender, pCity, true, 
								pFromPlot, pTargetPlot, bIgnoreUnitAdjacencyBoni, bQuickAndDirty);
	if (iAttackerStrength==0)
		return 0;

	int iDefenderStrength = 0;

	//in case of a missile, look up the target (the generic call in case of a city target doesn't pass the defender)
	if (AI_getUnitAIType() == UNITAI_MISSILE_AIR && pTargetPlot && pDefender == NULL)
	{
		pDefender = rangeStrikeTarget(*pTargetPlot, true);
		if (!pDefender)
			return 0;
	}

	// City is Defender - unless it's a missile
	if (pCity != NULL && AI_getUnitAIType() != UNITAI_MISSILE_AIR)
	{
		iDefenderStrength = pCity->getStrengthValue(false,ignoreBuildingDefense());
		//note: extra damage doesn't affect city strength
	}
	// Unit is Defender
	else if (pDefender != NULL)
	{
		// If this is a defenseless unit, do a fixed amount of damage
		if (!pDefender->IsCanDefend())
		{
			//can assassinate any civilian with one missile hit
			if (AI_getUnitAIType() == UNITAI_MISSILE_AIR)
				return pDefender->GetCurrHitPoints();
			else
				return /*40*/ GD_INT_GET(NONCOMBAT_UNIT_RANGED_DAMAGE);
		}

		// Use Ranged combat value for defender (except impi)
		if (pDefender->IsCanAttackRanged() && !pDefender->isRangedSupportFire())
		{
			//have to consider embarkation explicitly
			if ( (!pTargetPlot && pDefender->isEmbarked()) || (pTargetPlot && pTargetPlot->needsEmbarkation(pDefender) && pDefender->CanEverEmbark()) )
				iDefenderStrength = pDefender->GetEmbarkedUnitDefense();
			else
			{
				iDefenderStrength = pDefender->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, pTargetPlot, pFromPlot, false, bQuickAndDirty, iAssumeExtraDamage);
			}
		}
		else
		{
			//this considers embarkation implicitly
			iDefenderStrength = pDefender->GetMaxDefenseStrength(pTargetPlot, this, pFromPlot, /*bFromRangedAttack*/ true, bQuickAndDirty, iAssumeExtraDamage);
		}
	}
	else
	{
		//defender unknown - assume equal strength unit ...
		iDefenderStrength = GetBaseCombatStrength()*100;
	}

	CvSeeder randomSeed;
	if (bIncludeRand)
	{
		randomSeed
			.mixAssign(plot()->GetPseudoRandomSeed())
			.mixAssign(GetID())
			.mixAssign(iAttackerStrength)
			.mixAssign(iDefenderStrength);
	}

	int iDamage = CvUnitCombat::DoDamageMath(
		iAttackerStrength,
		iDefenderStrength,
		/*2400*/ GD_INT_GET(RANGE_ATTACK_SAME_STRENGTH_MIN_DAMAGE), //ignore the min part, it's misleading
		/*1200*/ GD_INT_GET(RANGE_ATTACK_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE),
		bIncludeRand,
		randomSeed,
		0 ) / 100;

	//extra damage with special promotion
	if (GetMoraleBreakChance() != 0 && pDefender && pDefender->GetNumFallBackPlotsAvailable(*this) == 0)
		iDamage = (iDamage * 150) / 100;

	return iDamage;
}

int CvUnit::GetRangeCombatSplashDamage(const CvPlot* pTargetPlot) const
{
	int iTotal = 0;

	// Add splash damage, if any
	if (getSplashDamage() != 0)
	{
		CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pTargetPlot);
		for (int i = 0; i < 6; i++)
		{
			CvPlot* pNeighbor = aNeighbors[i];
			if (pNeighbor && canEverRangeStrikeAt(pNeighbor->getX(), pNeighbor->getY(),plot(),false))
			{
				for (int iUnitLoop = 0; iUnitLoop < pNeighbor->getNumUnits(); iUnitLoop++)
				{
					CvUnit* pOtherUnit = pNeighbor->getUnitByIndex(iUnitLoop);
					//damage is applied to enemy civilians also, but it's collateral damage, don't count them here
					if (pOtherUnit && pOtherUnit->IsCombatUnit() && pOtherUnit->isEnemy(getTeam()))
						iTotal += getSplashDamage();
				}
			}
		}
	}

	return iTotal;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAirStrikeDefenseDamage(const CvUnit* pAttacker, bool bIncludeRand, const CvPlot* /*pTargetPlot*/) const
{
	//base value
	if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
	{
		int iMaxRandom = 5;
		int iBaseValue = getUnitInfo().GetBaseLandAirDefense() + getLandAirDefenseValue();

		if (pAttacker != NULL)
		{
			//value is negative if good!
			int iReduction = pAttacker->GetInterceptionDefenseDamageModifier();

			iBaseValue = iBaseValue * max(0, 100 + iReduction);
			iBaseValue /= 100;
		}

		if (bIncludeRand)
			return iBaseValue + GC.getGame().randRangeExclusive(0, iMaxRandom, CvSeeder(plot()->GetPseudoRandomSeed()));
		else
			return iBaseValue;
	}
	else
	{
		int iVal = 10;
		if (pAttacker != NULL && pAttacker->GetInterceptionDefenseDamageModifier() != 0)
		{
			iVal = iVal * (100 + pAttacker->GetInterceptionDefenseDamageModifier());
			iVal /= 100;
		}

		if (bIncludeRand)
			return iVal + GC.getGame().randRangeExclusive(0, iVal / 2, CvSeeder(plot()->GetPseudoRandomSeed()));
		else
			return iVal+2;
	}	
}

//	--------------------------------------------------------------------------------
/// Amount of damage done by this unit when intercepting pAttacker
int CvUnit::GetInterceptionDamage(const CvUnit* pInterceptedAttacker, bool bIncludeRand, const CvPlot* pTargetPlot) const
{
	if (pTargetPlot == NULL)
		pTargetPlot = plot();

	//interception happens at the target plot
	int iInterceptedAttackerStrength = 0;
	switch (pInterceptedAttacker->getDomainType())
	{
		case DOMAIN_AIR:
			//interception happens at the target plot
			iInterceptedAttackerStrength = pInterceptedAttacker->GetMaxRangedCombatStrength(this, /*pCity*/ NULL, false, pTargetPlot, pTargetPlot);
			break;

		case DOMAIN_SEA:
		case DOMAIN_LAND:
		case DOMAIN_IMMOBILE:
		case DOMAIN_HOVER:
			iInterceptedAttackerStrength = pInterceptedAttacker->GetMaxDefenseStrength(pTargetPlot, this, pTargetPlot, false);
			break;

		default:
			CvAssert(false);
			break;
	}

	int iInterceptorStrength = 0;
	switch (getDomainType())
	{
		case DOMAIN_AIR:
			//interception happens at the target plot
			iInterceptorStrength = GetMaxRangedCombatStrength(pInterceptedAttacker, /*pCity*/ NULL, true, pTargetPlot, pTargetPlot);
			break;

		case DOMAIN_SEA:
		case DOMAIN_LAND:
		case DOMAIN_IMMOBILE:
		case DOMAIN_HOVER:
			iInterceptorStrength = GetMaxAttackStrength(NULL, NULL, pInterceptedAttacker);
			break;

		default:
			CvAssert(false);
			break;
	}

	// Mod to interceptor strength
	iInterceptorStrength *= (100 + GetInterceptionCombatModifier());
	iInterceptorStrength /= 100;
	
	CvSeeder randomSeed;
	if (bIncludeRand)
	{
		randomSeed
			.mixAssign(pTargetPlot->GetPseudoRandomSeed())
			.mixAssign(GetID())
			.mixAssign(iInterceptorStrength)
			.mixAssign(iInterceptedAttackerStrength);
	}
	return CvUnitCombat::DoDamageMath(
		iInterceptorStrength,
		iInterceptedAttackerStrength,
		/*2400*/ GD_INT_GET(INTERCEPTION_SAME_STRENGTH_MIN_DAMAGE), //ignore the min part, it's misleading
		/*1200*/ GD_INT_GET(INTERCEPTION_SAME_STRENGTH_POSSIBLE_EXTRA_DAMAGE),
		bIncludeRand,
		randomSeed,
		pInterceptedAttacker->GetInterceptionDefenseDamageModifier() ) / 100;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isWaiting() const
{
	VALIDATE_OBJECT

	ActivityTypes eActivityType = GetActivityType();

	return ((eActivityType == ACTIVITY_HOLD)	||
			(eActivityType == ACTIVITY_SLEEP)   ||
			(eActivityType == ACTIVITY_HEAL)    ||
			(eActivityType == ACTIVITY_SENTRY)  ||
			(eActivityType == ACTIVITY_INTERCEPT));
}

//	--------------------------------------------------------------------------------
/// Can this Unit EVER fortify? (may be redundant with some other stuff)
bool CvUnit::IsEverFortifyable() const
{
	return (IsCombatUnit() && !noDefensiveBonus() && ((getDomainType() == DOMAIN_LAND) || (getDomainType() == DOMAIN_IMMOBILE)));
}

//	--------------------------------------------------------------------------------
int CvUnit::fortifyModifier() const
{
	VALIDATE_OBJECT
	int iValue = 0;
	if( IsFortified() )
	{
		iValue = /*20*/ GD_INT_GET(FORTIFY_MODIFIER_PER_TURN);
	
		// Apply modifiers
		int iMod = GET_PLAYER(getOwner()).getUnitFortificationModifier();
		if(iMod != 0)
		{
			iValue = ((100 + iMod) / 100) * iValue;
		}

		if(iValue < 0)
		{
			CvAssertMsg(false, "Calculated a negative combat mod for a fortified unit. Resetting to 0 instead.");
			iValue = 0;
		}
	}

	return iValue;
}


//	--------------------------------------------------------------------------------
int CvUnit::experienceNeeded() const
{
	VALIDATE_OBJECT

	const int iLevel = getLevel();

	// Sum up the levels to get our multiplier (1:1, 2:3, 3:6, 4:10, etc.)
	int iExperienceMultiplier = 0;
	for(int iLevelLoop = 1; iLevelLoop <= iLevel; iLevelLoop++)
	{
		iExperienceMultiplier += iLevelLoop;
	}

	int iExperienceNeeded = /*10*/ GD_INT_GET(EXPERIENCE_PER_LEVEL) * iExperienceMultiplier;

	const int iModifier = GET_PLAYER(getOwner()).getLevelExperienceModifier();
	if (iModifier != 0)
	{
		float fTemp = (float) iExperienceNeeded;
		fTemp *= (100 + iModifier);
		fTemp /= 100;
		iExperienceNeeded = (int) ceil(fTemp); // Round up
	}

	if (MOD_BALANCE_CORE_SCALING_XP)
	{
		iExperienceNeeded *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iExperienceNeeded /= 100;
	}

	return iExperienceNeeded;
}

//	--------------------------------------------------------------------------------
int CvUnit::maxXPValue() const
{
	VALIDATE_OBJECT
	int iMaxValue = MAX_INT; // negative values mean no XP limit, and MAX_INT is the same in effect

	if (isBarbarian())
	{
		iMaxValue = std::min(iMaxValue, /*30 in CP, 45 in VP*/ GD_INT_GET(BARBARIAN_MAX_XP_VALUE));
	}
	if (GET_PLAYER(getOwner()).isMinorCiv())
	{
		iMaxValue = std::min(iMaxValue, /*-1 in CP, 70 in VP*/ GD_INT_GET(MINOR_MAX_XP_VALUE));
	}

	if (MOD_BALANCE_CORE_SCALING_XP && iMaxValue > 0)
	{
		iMaxValue *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iMaxValue /= 100;
	}

	return iMaxValue;
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreBuildingDefense() const
{
	//return m_pUnitInfo->IsIgnoreBuildingDefense();
	return false; //always return false - this is too powerful
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreTerrainCost() const
{
	VALIDATE_OBJECT
	return getIgnoreTerrainCostCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainCostCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreTerrainCostCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreTerrainCostCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::ignoreTerrainDamage() const
{
	VALIDATE_OBJECT
	return getIgnoreTerrainDamageCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainDamageCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreTerrainDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreTerrainDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::ignoreFeatureDamage() const
{
	VALIDATE_OBJECT
	return getIgnoreFeatureDamageCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreFeatureDamageCount() const
{
	VALIDATE_OBJECT
	return m_iIgnoreFeatureDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreFeatureDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iIgnoreFeatureDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::extraTerrainDamage() const
{
	VALIDATE_OBJECT
	return getExtraTerrainDamageCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainDamageCount() const
{
	VALIDATE_OBJECT
	return m_iExtraTerrainDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iExtraTerrainDamageCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::extraFeatureDamage() const
{
	VALIDATE_OBJECT
	return getExtraFeatureDamageCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureDamageCount() const
{
	VALIDATE_OBJECT
	return m_iExtraFeatureDamageCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureDamageCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iExtraFeatureDamageCount += iValue;
	}
}

#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
int CvUnit::GetNearbyImprovementCombatBonus() const
{
	return m_iNearbyImprovementCombatBonus;
}

void CvUnit::SetNearbyImprovementCombatBonus(int iCombatBonus)
{
	m_iNearbyImprovementCombatBonus = iCombatBonus;
}

int CvUnit::GetNearbyImprovementBonusRange() const
{
	return m_iNearbyImprovementBonusRange;
}

void CvUnit::SetNearbyImprovementBonusRange(int iBonusRange)
{
	m_iNearbyImprovementBonusRange = iBonusRange;
}

ImprovementTypes CvUnit::GetCombatBonusImprovement() const
{
	return m_eCombatBonusImprovement;
}

void CvUnit::SetCombatBonusImprovement(ImprovementTypes eImprovement)
{
	m_eCombatBonusImprovement = eImprovement;
}
#endif
#if defined(MOD_BALANCE_CORE)
int CvUnit::getNearbyUnitClassBonus() const
{
	return m_iNearbyUnitClassBonus;
}

void CvUnit::SetNearbyUnitClassBonus(int iCombatBonus)
{
	m_iNearbyUnitClassBonus = iCombatBonus;
}

int CvUnit::getNearbyUnitClassBonusRange() const
{
	return m_iNearbyUnitClassBonusRange;
}

void CvUnit::SetNearbyUnitClassBonusRange(int iBonusRange)
{
	m_iNearbyUnitClassBonusRange = iBonusRange;
}

UnitClassTypes CvUnit::getCombatBonusFromNearbyUnitClass() const
{
	return (UnitClassTypes)m_iCombatBonusFromNearbyUnitClass;
}

void CvUnit::SetCombatBonusFromNearbyUnitClass(UnitClassTypes eUnitClass)
{
	m_iCombatBonusFromNearbyUnitClass = eUnitClass;
}
void CvUnit::ChangeNearbyPromotion(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyPromotion += iValue;
}
int CvUnit::GetNearbyPromotion() const
{
	VALIDATE_OBJECT
	return	m_iNearbyPromotion;
}
bool CvUnit::isNearbyPromotion() const
{
	VALIDATE_OBJECT
	return GetNearbyPromotion() > 0;
}
int CvUnit::GetNearbyUnitPromotionsRange() const
{
	VALIDATE_OBJECT
	return m_iNearbyUnitPromotionRange;
}
void CvUnit::ChangeNearbyUnitPromotionRange(int iBonusRange)
{
	VALIDATE_OBJECT
	m_iNearbyUnitPromotionRange += iBonusRange;
}
void CvUnit::ChangeNearbyCityCombatMod(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyCityCombatMod += iValue;
}
int CvUnit::getNearbyCityCombatMod() const
{
	VALIDATE_OBJECT
	return	m_iNearbyCityCombatMod;
}
void CvUnit::ChangeNearbyFriendlyCityCombatMod(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyFriendlyCityCombatMod += iValue;
}
int CvUnit::getNearbyFriendlyCityCombatMod() const
{
	VALIDATE_OBJECT
	return	m_iNearbyFriendlyCityCombatMod;
}
void CvUnit::ChangeNearbyEnemyCityCombatMod(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCityCombatMod += iValue;
}
int CvUnit::getNearbyEnemyCityCombatMod() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCityCombatMod;
}
int CvUnit::getPillageBonusStrengthPercent() const
{
	VALIDATE_OBJECT
	return m_iPillageBonusStrengthPercent;
}
void CvUnit::ChangePillageBonusStrengthPercent(int iBonus)
{
	VALIDATE_OBJECT
	m_iPillageBonusStrengthPercent += iBonus;
}
int CvUnit::getStackedGreatGeneralExperience() const
{
	VALIDATE_OBJECT
	return m_iStackedGreatGeneralExperience;
}
void CvUnit::ChangeStackedGreatGeneralExperience(int iExperience)
{
	VALIDATE_OBJECT
	m_iStackedGreatGeneralExperience += iExperience;
}
int CvUnit::getWonderProductionModifier() const
{
	VALIDATE_OBJECT
	return m_iWonderProductionModifier;
}
void CvUnit::ChangeWonderProductionModifier(int iValue)
{
	VALIDATE_OBJECT
	m_iWonderProductionModifier += iValue;
}
int CvUnit::getMilitaryProductionModifier() const
{
	VALIDATE_OBJECT
	return m_iUnitProductionModifier;
}
void CvUnit::ChangeMilitaryProductionModifier(int iValue)
{
	VALIDATE_OBJECT
	m_iUnitProductionModifier += iValue;
}
int CvUnit::getNearbyEnemyDamage() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyDamage;
}
void CvUnit::ChangeNearbyEnemyDamage(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyDamage += iValue;
}
int CvUnit::GetAdjacentCityDefenseMod() const
{
	VALIDATE_OBJECT
		return m_iAdjacentCityDefenseMod;
}
void CvUnit::ChangeAdjacentCityDefenseMod(int iValue)
{
	VALIDATE_OBJECT
		m_iAdjacentCityDefenseMod += iValue;
}
int CvUnit::GetGGGAXPPercent() const
{
	VALIDATE_OBJECT
	return m_iGGGAXPPercent;
}
void CvUnit::ChangeGGGAXPPercent(int iValue)
{
	VALIDATE_OBJECT
	m_iGGGAXPPercent += iValue;
}
int CvUnit::getGiveCombatMod() const
{
	VALIDATE_OBJECT
	return m_iGiveCombatMod;
}
void CvUnit::ChangeGiveCombatMod(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveCombatMod += iValue;
}
int CvUnit::getGiveHPIfEnemyKilled() const
{
	VALIDATE_OBJECT
	return m_iGiveHPIfEnemyKilled;
}
void CvUnit::ChangeGiveHPIfEnemyKilled(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveHPIfEnemyKilled += iValue;
}
int CvUnit::getGiveExperiencePercent() const
{
	VALIDATE_OBJECT
	return m_iGiveExperiencePercent;
}
void CvUnit::ChangeGiveExperiencePercent(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveExperiencePercent += iValue;
}
int CvUnit::getGiveOutsideFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iGiveOutsideFriendlyLandsModifier;
}
void CvUnit::ChangeGiveOutsideFriendlyLandsModifier(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveOutsideFriendlyLandsModifier += iValue;
}
const DomainTypes CvUnit::getGiveDomain() const
{
	VALIDATE_OBJECT
	return (DomainTypes)(int)m_eGiveDomain;
}
void CvUnit::ChangeGiveDomain(DomainTypes eDomain)
{
	VALIDATE_OBJECT
	m_eGiveDomain = eDomain;
}
int CvUnit::getGiveExtraAttacks() const
{
	VALIDATE_OBJECT
	return m_iGiveExtraAttacks;
}
void CvUnit::ChangeGiveExtraAttacks(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveExtraAttacks += iValue;
}
int CvUnit::getGiveDefenseMod() const
{
	VALIDATE_OBJECT
	return m_iGiveDefenseMod;
}
void CvUnit::ChangeGiveDefenseMod(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveDefenseMod += iValue;
}
int CvUnit::getNearbyHealEnemyTerritory() const
{
	VALIDATE_OBJECT
	return m_iNearbyHealEnemyTerritory;
}
void CvUnit::ChangeNearbyHealEnemyTerritory(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyHealEnemyTerritory += iValue;
}
int CvUnit::getNearbyHealNeutralTerritory() const
{
	VALIDATE_OBJECT
	return m_iNearbyHealNeutralTerritory;
}
void CvUnit::ChangeNearbyHealNeutralTerritory(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyHealNeutralTerritory += iValue;
}
int CvUnit::getNearbyHealFriendlyTerritory() const
{
	VALIDATE_OBJECT
	return m_iNearbyHealFriendlyTerritory;
}
void CvUnit::ChangeNearbyHealFriendlyTerritory(int iValue)
{
	VALIDATE_OBJECT
	m_iNearbyHealFriendlyTerritory += iValue;
}
void CvUnit::ChangeIsGiveInvisibility(int iValue)
{
	VALIDATE_OBJECT
	m_iGiveInvisibility += iValue;
}
int CvUnit::GetIsGiveInvisibility() const
{
	VALIDATE_OBJECT
	return	m_iGiveInvisibility;
}
bool CvUnit::isGiveInvisibility() const
{
	VALIDATE_OBJECT
	return GetIsGiveInvisibility() > 0;
}
bool CvUnit::isGiveOnlyOnStartingTurn() const
{
	VALIDATE_OBJECT
	return m_bGiveOnlyOnStartingTurn;
}
void CvUnit::SetIsGiveOnlyOnStartingTurn(bool bNewValue)
{
	VALIDATE_OBJECT
	m_bGiveOnlyOnStartingTurn = bNewValue;
}
void CvUnit::ChangeIsConvertUnit(int iValue)
{
	VALIDATE_OBJECT
	m_iConvertUnit += iValue;
}
inline int CvUnit::getIsConvertUnit() const
{
	VALIDATE_OBJECT
	return	m_iConvertUnit;
}
bool CvUnit::isConvertUnit() const
{
	VALIDATE_OBJECT
	return getIsConvertUnit() > 0;
}
const DomainTypes CvUnit::getConvertDomain() const
{
	VALIDATE_OBJECT
	return (DomainTypes)(int)m_eConvertDomain;
}
void CvUnit::ChangeConvertDomain(DomainTypes eDomain)
{
	VALIDATE_OBJECT
	m_eConvertDomain = eDomain;
}
const UnitTypes CvUnit::getConvertDomainUnitType() const
{
	VALIDATE_OBJECT
	return m_eConvertDomainUnit;
}
void CvUnit::ChangeConvertDomainUnit(UnitTypes eUnit)
{
	VALIDATE_OBJECT
	m_eConvertDomainUnit = eUnit;
}
void CvUnit::ChangeIsConvertEnemyUnitToBarbarian(int iValue)
{
	VALIDATE_OBJECT
	m_iConvertEnemyUnitToBarbarian += iValue;
}
int CvUnit::getIsConvertEnemyUnitToBarbarian() const
{
	VALIDATE_OBJECT
	return m_iConvertEnemyUnitToBarbarian;
}
bool CvUnit::isConvertEnemyUnitToBarbarian() const
{
	VALIDATE_OBJECT
	return getIsConvertEnemyUnitToBarbarian() > 0;
}
void CvUnit::ChangeIsConvertOnFullHP(int iValue)
{
	VALIDATE_OBJECT
	m_bConvertOnFullHP = (iValue>0);
}
bool CvUnit::isConvertOnFullHP() const
{
	VALIDATE_OBJECT
	return m_bConvertOnFullHP;
}
void CvUnit::ChangeIsConvertOnDamage(int iValue)
{
	VALIDATE_OBJECT
	m_bConvertOnDamage = (iValue>0);
}
bool CvUnit::isConvertOnDamage() const
{
	VALIDATE_OBJECT
	return	m_bConvertOnDamage;
}
int CvUnit::getDamageThreshold() const
{
	VALIDATE_OBJECT
	return m_iDamageThreshold;
}
void CvUnit::ChangeDamageThreshold(int iValue)
{
	VALIDATE_OBJECT
	m_iDamageThreshold += iValue;
}
const UnitTypes CvUnit::getConvertDamageOrFullHPUnit() const
{
	VALIDATE_OBJECT
	return m_eConvertDamageOrFullHPUnit;
}

void CvUnit::ChangeConvertDamageOrFullHPUnit(UnitTypes eUnit)
{
	VALIDATE_OBJECT
	m_eConvertDamageOrFullHPUnit = eUnit;
}

int CvUnit::GetAirInterceptRange() const
{
	return m_pUnitInfo->GetAirInterceptRange() + GetExtraAirInterceptRange();
}

bool CvUnit::canIntercept() const
{
	if (GetAirInterceptRange() > 0 && GetNumInterceptions() > 0 && getInterceptChance() > 0 && !isEmbarked())
	{
		// Must either be a non-air Unit, or an air Unit that hasn't moved this turn and is on intercept duty
		return getDomainType() != DOMAIN_AIR || (GetActivityType() == ACTIVITY_INTERCEPT);
	}

	return false;
}
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossMountains() const
{
	VALIDATE_OBJECT
	return (MOD_PROMOTIONS_CROSS_MOUNTAINS && getCanCrossMountainsCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossMountainsCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossMountainsCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossMountainsCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossMountainsCount += iValue;
	}
}
#endif


#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossOceans() const
{
	VALIDATE_OBJECT
	return getCanCrossOceansCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossOceansCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossOceansCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossOceansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossOceansCount += iValue;
	}
}
#endif


#if defined(MOD_PROMOTIONS_CROSS_ICE)
//	--------------------------------------------------------------------------------
bool CvUnit::canCrossIce() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).GetPlayerTraits()->IsAbleToCrossIce() || (MOD_PROMOTIONS_CROSS_ICE && getCanCrossIceCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanCrossIceCount() const
{
	VALIDATE_OBJECT
	return m_iCanCrossIceCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanCrossIceCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanCrossIceCount += iValue;
	}
}
#endif
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumTilesRevealedThisTurn(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNumTilesRevealedThisTurn += iValue;
	}
}
//	--------------------------------------------------------------------------------
void CvUnit::SetNumTilesRevealedThisTurn(int iValue)
{
	VALIDATE_OBJECT
	m_iNumTilesRevealedThisTurn = iValue;
}
//	--------------------------------------------------------------------------------
int CvUnit::GetNumTilesRevealedThisTurn()
{
	VALIDATE_OBJECT
	return m_iNumTilesRevealedThisTurn;
}

bool CvUnit::HasSpottedEnemy() const
{
	VALIDATE_OBJECT
	return m_bSpottedEnemy;
}

void CvUnit::SetSpottedEnemy(bool bValue)
{
	VALIDATE_OBJECT
	m_bSpottedEnemy = bValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGainsYieldFromScouting() const
{
	VALIDATE_OBJECT
	for (int iI = 0; iI < YIELD_GREAT_ADMIRAL_POINTS; iI++)
	{
		if (getYieldFromScouting((YieldTypes)iI) > 0)
		{
			return true;
		}
	}
	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGainsXPFromScouting() const
{
	VALIDATE_OBJECT
	return (GetGainsXPFromScouting() > 0);
}



//	--------------------------------------------------------------------------------
int CvUnit::GetGainsXPFromScouting() const
{
	VALIDATE_OBJECT
	return m_iGainsXPFromScouting;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGainsXPFromScouting(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iGainsXPFromScouting += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGainsXPFromSpotting() const
{
	VALIDATE_OBJECT
		return (GetGainsXPFromSpotting() > 0);
}



//	--------------------------------------------------------------------------------
int CvUnit::GetGainsXPFromSpotting() const
{
	VALIDATE_OBJECT
		return m_iGainsXPFromSpotting;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGainsXPFromSpotting(int iValue)
{
	VALIDATE_OBJECT
		if (iValue != 0)
		{
			m_iGainsXPFromSpotting += iValue;
		}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGainsXPFromPillaging() const
{
	VALIDATE_OBJECT
		return (GetGainsXPFromPillaging() > 0);
}



//	--------------------------------------------------------------------------------
int CvUnit::GetGainsXPFromPillaging() const
{
	VALIDATE_OBJECT
		return m_iGainsXPFromPillaging;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGainsXPFromPillaging(int iValue)
{
	VALIDATE_OBJECT
		if (iValue != 0)
		{
			m_iGainsXPFromPillaging += iValue;
		}
}



#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
//	--------------------------------------------------------------------------------
bool CvUnit::isGGFromBarbarians() const
{
	VALIDATE_OBJECT
	return GET_PLAYER(getOwner()).GetPlayerTraits()->IsGGFromBarbarians() || (MOD_PROMOTIONS_GG_FROM_BARBARIANS && getGGFromBarbariansCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getGGFromBarbariansCount() const
{
	VALIDATE_OBJECT
	return m_iGGFromBarbariansCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeGGFromBarbariansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iGGFromBarbariansCount += iValue;
	}
}
#endif
int CvUnit::GetCaptureDefeatedEnemyChance() const
{
	return m_iCaptureDefeatedEnemyChance;
}
void CvUnit::ChangeCaptureDefeatedEnemyChance(int iValue)
{
	m_iCaptureDefeatedEnemyChance += iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetBarbarianCombatBonus() const
{
	VALIDATE_OBJECT
	return m_iBarbCombatBonus;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeBarbarianCombatBonus(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iBarbCombatBonus += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAdjacentEnemySapMovement() const
{
	VALIDATE_OBJECT
		return m_iAdjacentEnemySapMovement;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAdjacentEnemySapMovement(int iValue)
{
	VALIDATE_OBJECT
	m_iAdjacentEnemySapMovement += iValue;
}

#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsRoughTerrainEndsTurn() const
{
	VALIDATE_OBJECT
	return GetRoughTerrainEndsTurnCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetRoughTerrainEndsTurnCount() const
{
	VALIDATE_OBJECT
	return m_iRoughTerrainEndsTurnCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRoughTerrainEndsTurnCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRoughTerrainEndsTurnCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCapturedUnitsConscripted() const
{
	VALIDATE_OBJECT
	return GetCapturedUnitsConscriptedCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCapturedUnitsConscriptedCount() const
{
	VALIDATE_OBJECT
	return m_iCapturedUnitsConscriptedCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCapturedUnitsConscriptedCount(int iValue)
{
	VALIDATE_OBJECT
	if (iValue != 0)
	{
		m_iCapturedUnitsConscriptedCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHoveringUnit() const
{
	return (MOD_CORE_HOVERING_UNITS && (GetHoveringUnitCount() > 0 || getDomainType() == DOMAIN_HOVER));
}

//	--------------------------------------------------------------------------------
int CvUnit::GetHoveringUnitCount() const
{
	VALIDATE_OBJECT
	return m_iHoveringUnitCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeHoveringUnitCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHoveringUnitCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::flatMovementCost() const
{
	VALIDATE_OBJECT
	return getFlatMovementCostCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getFlatMovementCostCount() const
{
	VALIDATE_OBJECT
	return m_iFlatMovementCostCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFlatMovementCostCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iFlatMovementCostCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveImpassable() const
{
	VALIDATE_OBJECT
	return getCanMoveImpassableCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveImpassableCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveImpassableCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveImpassableCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iCanMoveImpassableCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveAllTerrainCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAllTerrainCount += iValue;
	CvAssert(getCanMoveAllTerrainCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveAllTerrain() const
{
	VALIDATE_OBJECT
	return (getCanMoveAllTerrainCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveAllTerrainCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAllTerrainCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCanMoveAfterAttackingCount(int iValue)
{
	VALIDATE_OBJECT
	m_iCanMoveAfterAttackingCount += iValue;
	CvAssert(getCanMoveAfterAttackingCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canMoveAfterAttacking() const
{
	VALIDATE_OBJECT

	// Units with blitz can move after attacking (otherwise there isn't much point!)
	if(isBlitz())
	{
		return true;
	}

	return (getCanMoveAfterAttackingCount() > 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getCanMoveAfterAttackingCount() const
{
	VALIDATE_OBJECT
	return m_iCanMoveAfterAttackingCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::hasFreePillageMove() const
{
	VALIDATE_OBJECT
	return getFreePillageMoveCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFreePillageMoveCount(int iValue)
{
	VALIDATE_OBJECT
	m_iFreePillageMoveCount += iValue;
	CvAssert(getFreePillageMoveCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getFreePillageMoveCount() const
{
	VALIDATE_OBJECT
	return m_iFreePillageMoveCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::hasHealOnPillage() const
{
	VALIDATE_OBJECT
	return getHealOnPillageCount() > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHealOnPillageCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHealOnPillageCount += iValue;
	CvAssert(getHealOnPillageCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getHealOnPillageCount() const
{
	VALIDATE_OBJECT
	return m_iHealOnPillageCount;
}

//	--------------------------------------------------------------------------------
int CvUnit::getHPHealedIfDefeatEnemy() const
{
	VALIDATE_OBJECT
	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return m_iHPHealedIfDefeatEnemy + GetGiveHPIfEnemyKilledToUnit();
	else
		return m_iHPHealedIfDefeatEnemy;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHPHealedIfDefeatEnemy(int iValue)
{
	VALIDATE_OBJECT
	m_iHPHealedIfDefeatEnemy += iValue;
	CvAssert(getHPHealedIfDefeatEnemy() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsHealIfDefeatExcludeBarbarians() const
{
	VALIDATE_OBJECT
	return GetHealIfDefeatExcludeBarbariansCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetHealIfDefeatExcludeBarbariansCount() const
{
	VALIDATE_OBJECT
	return m_iHealIfDefeatExcludeBarbariansCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeHealIfDefeatExcludeBarbariansCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iHealIfDefeatExcludeBarbariansCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGoldenAgeValueFromKills() const
{
	VALIDATE_OBJECT
	return m_iGoldenAgeValueFromKills;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGoldenAgeValueFromKills(int iValue)
{
	VALIDATE_OBJECT
	m_iGoldenAgeValueFromKills += iValue;
	CvAssert(GetGoldenAgeValueFromKills() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isOnlyDefensive() const
{
	VALIDATE_OBJECT
	return getOnlyDefensiveCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getOnlyDefensiveCount() const
{
	VALIDATE_OBJECT
	return m_iOnlyDefensiveCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeOnlyDefensiveCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iOnlyDefensiveCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoAttackInOcean() const
{
	VALIDATE_OBJECT
	return getNoAttackInOceanCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoAttackInOceanCount() const
{
	VALIDATE_OBJECT
	return m_iNoAttackInOceanCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoAttackInOceanCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNoAttackInOceanCount += iValue;
	CvAssert(getNoAttackInOceanCount() >= 0);
}

//	--------------------------------------------------------------------------------
bool CvUnit::noDefensiveBonus() const
{
	VALIDATE_OBJECT
	// Boats get no bonus for fortifying
	if(getDomainType() == DOMAIN_SEA)
	{
		return true;
	}

	return getNoDefensiveBonusCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoDefensiveBonusCount() const
{
	VALIDATE_OBJECT
	return m_iNoDefensiveBonusCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoDefensiveBonusCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoDefensiveBonusCount += iValue;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoCapture() const
{
	VALIDATE_OBJECT
	return getNoCaptureCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoCaptureCount() const
{
	VALIDATE_OBJECT
	return m_iNoCaptureCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoCaptureCount(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iNoCaptureCount += iValue;
	}
}


//	--------------------------------------------------------------------------------
bool CvUnit::isNeverInvisible() const
{
	VALIDATE_OBJECT
	return (getInvisibleType() == NO_INVISIBLE);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo) const
{
	VALIDATE_OBJECT
	if(bDebug && GC.getGame().isDebugMode())
	{
		return false;
	}

	if(GET_PLAYER(getOwner()).getTeam() == eTeam)
	{
		return false;
	}

	if(bCheckCargo)
	{
		if((m_transportUnit.eOwner >= 0) && m_transportUnit.eOwner < MAX_PLAYERS)
		{
			if(GET_PLAYER((PlayerTypes)m_transportUnit.eOwner).getUnit(m_transportUnit.iID))
			{
				return true;
			}
		}
	}
	if (IsHiddenByNearbyUnit() && eTeam != NO_TEAM && !plot()->isInvisibleVisibleUnit(eTeam))
	{
		return true;
	}
	
	if(m_eInvisibleType == NO_INVISIBLE)
	{
		return false;
	}

	if (eTeam!=NO_TEAM)
		return !(plot()->isInvisibleVisible(eTeam, getInvisibleType()));
	else
		return true; //just assume it is invisible
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNukeImmune() const
{
	VALIDATE_OBJECT
	return (getNukeImmuneCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNukeImmuneCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNukeImmuneCount += iValue;
	CvAssert(getNukeImmuneCount() >= 0);

}

//	--------------------------------------------------------------------------------
int CvUnit::getNukeImmuneCount() const
{
	VALIDATE_OBJECT
	return m_iNukeImmuneCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isHiddenNationality() const
{
	VALIDATE_OBJECT
	return (getHiddenNationalityCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::changeHiddenNationalityCount(int iValue)
{
	VALIDATE_OBJECT
	m_iHiddenNationalityCount += iValue;
	CvAssert(getHiddenNationalityCount() >= 0);

}

//	--------------------------------------------------------------------------------
int CvUnit::getHiddenNationalityCount() const
{
	VALIDATE_OBJECT
	return m_iHiddenNationalityCount;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoRevealMap() const
{
	VALIDATE_OBJECT
	return (getNoRevealMapCount() > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoRevealMapCount(int iValue)
{
	VALIDATE_OBJECT
	m_iNoRevealMapCount += iValue;
	CvAssert(getNoRevealMapCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getNoRevealMapCount() const
{
	VALIDATE_OBJECT
	return m_iNoRevealMapCount;
}

//	--------------------------------------------------------------------------------
int CvUnit::interceptionProbability() const
{
	return (getInterceptChance() * GetCurrHitPoints()) / GetMaxHitPoints();
}

//	--------------------------------------------------------------------------------
int CvUnit::evasionProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraEvasion());
}

//	--------------------------------------------------------------------------------
int CvUnit::withdrawalProbability() const
{
	VALIDATE_OBJECT
	return std::max(0, getExtraWithdrawal());
}

//	--------------------------------------------------------------------------------
/// How many enemy Units are adjacent to this guy?
int CvUnit::GetNumEnemyUnitsAdjacent(const CvUnit* pUnitToExclude) const
{
	if (plot() != NULL)
	{
		return plot()->GetNumEnemyUnitsAdjacent(getTeam(), plot()->getDomain(), pUnitToExclude);
	}
	return 0;
}

//	--------------------------------------------------------------------------------
/// How many friendly Units are adjacent to this guy?
int CvUnit::GetNumFriendlyUnitsAdjacent(const CvUnit* pUnitToExclude) const
{
	if (plot() != NULL)
	{
		return plot()->GetNumFriendlyUnitsAdjacent(getTeam(), plot()->getDomain(), true, pUnitToExclude);
	}
	return 0;
}

//	--------------------------------------------------------------------------------
/// Is a particular enemy city next to us?
bool CvUnit::IsEnemyCityAdjacent(const CvCity* pSpecifyCity) const
{
	CvAssertMsg(pSpecifyCity, "City is NULL when checking if it is adjacent to a unit");
	if (plot() != NULL)
	{
		return plot()->IsEnemyCityAdjacent(getTeam(), pSpecifyCity);
	}
	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNumOwningPlayerUnitsAdjacent(const CvUnit* pUnitToExclude, const CvUnit* pExampleUnitType, bool bCombatOnly) const
{
	if (plot() != NULL)
	{
		return plot()->GetNumSpecificPlayerUnitsAdjacent(getOwner(), pUnitToExclude, pExampleUnitType, bCombatOnly);
	}
	return 0;
}

//	--------------------------------------------------------------------------------
/// Is there a friendly Unit adjacent to us?
bool CvUnit::IsFriendlyUnitAdjacent(bool bCombatUnit) const
{
	if (plot() != NULL)
	{
		return plot()->IsFriendlyUnitAdjacent(getTeam(), bCombatUnit);
	}
	return false;
}

bool CvUnit::IsCoveringFriendlyCivilian() const
{
	CvPlot* myPlot = plot();
	if (!myPlot || myPlot->isCity())
		return false;

	IDInfo* pUnitNode = myPlot->headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = myPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTeam() == getTeam())
			if(!pLoopUnit->IsCanDefend() && pLoopUnit->TurnProcessed())
				return !GET_PLAYER(getOwner()).GetPossibleAttackers(*myPlot, getTeam()).empty();
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAdjacentModifier() const
{
	VALIDATE_OBJECT
	return m_iAdjacentModifier;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetNoAdjacentUnitModifier() const
{
	VALIDATE_OBJECT
		return m_iNoAdjacentUnitModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAdjacentModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAdjacentModifier += iValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeNoAdjacentUnitModifier(int iValue)
{
	VALIDATE_OBJECT
		if (iValue != 0)
		{
			m_iNoAdjacentUnitModifier += iValue;
		}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iRangedAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeRangedAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iRangedAttackModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetInterceptionCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeInterceptionCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionCombatModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetInterceptionDefenseDamageModifier() const
{
	VALIDATE_OBJECT
	return m_iInterceptionDefenseDamageModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeInterceptionDefenseDamageModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iInterceptionDefenseDamageModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetAirSweepCombatModifier() const
{
	VALIDATE_OBJECT
	return m_iAirSweepCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAirSweepCombatModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAirSweepCombatModifier += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeAttackModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iAttackModifier += iValue;
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getDefenseModifier(bool bQuick) const
{
	VALIDATE_OBJECT
	if (!bQuick && MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return m_iDefenseModifier + GetGiveDefenseModToUnit();
	else
		return m_iDefenseModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeDefenseModifier(int iValue)
{
	VALIDATE_OBJECT
	if(iValue != 0)
	{
		m_iDefenseModifier += iValue;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getGroundAttackDamage() const
{
	VALIDATE_OBJECT
		return m_iGroundAttackDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeGroundAttackDamage(int iValue)
{
	VALIDATE_OBJECT
	if (iValue != 0)
	{
		m_iGroundAttackDamage += iValue;
	}
}



//	--------------------------------------------------------------------------------
int CvUnit::cityAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityAttackPercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::cityDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraCityDefensePercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::rangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRangedDefenseModifier());
}


//	--------------------------------------------------------------------------------
int CvUnit::hillsAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsAttackPercent());
}


//	--------------------------------------------------------------------------------
int CvUnit::hillsDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraHillsDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::openAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenAttackPercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::openRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenRangedAttackMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughAttackPercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughRangedAttackModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughRangedAttackMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackFortifiedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackFortifiedMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackWoundedModifier() const
{
	VALIDATE_OBJECT
	return (getExtraAttackWoundedMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackFullyHealedModifier() const
{
	VALIDATE_OBJECT
		return (getExtraAttackFullyHealedMod());
}

//	--------------------------------------------------------------------------------
int CvUnit::attackAbove50HealthModifier() const
{
	VALIDATE_OBJECT
		return (getExtraAttackAboveHealthMod());
}
//	--------------------------------------------------------------------------------
int CvUnit::attackBelow50HealthModifier() const
{
	VALIDATE_OBJECT
		return (getExtraAttackBelowHealthMod());
}
//	--------------------------------------------------------------------------------
int CvUnit::openDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraOpenDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::roughDefenseModifier() const
{
	VALIDATE_OBJECT
	return (getExtraRoughDefensePercent());
}

//	--------------------------------------------------------------------------------
int CvUnit::terrainAttackModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainAttackPercent(eTerrain));
}


//	--------------------------------------------------------------------------------
int CvUnit::terrainDefenseModifier(TerrainTypes eTerrain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (getExtraTerrainDefensePercent(eTerrain));
}


//	--------------------------------------------------------------------------------
int CvUnit::featureAttackModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureAttackPercent(eFeature));
}

//	--------------------------------------------------------------------------------
int CvUnit::featureDefenseModifier(FeatureTypes eFeature) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	CvAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (getExtraFeatureDefensePercent(eFeature));
}

//	--------------------------------------------------------------------------------
int CvUnit::unitClassAttackModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return getUnitClassAttackMod(eUnitClass);
}


//	--------------------------------------------------------------------------------
int CvUnit::unitClassDefenseModifier(UnitClassTypes eUnitClass) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return getUnitClassDefenseMod(eUnitClass);
}


//	--------------------------------------------------------------------------------
int CvUnit::unitCombatModifier(UnitCombatTypes eUnitCombat) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative (invalid Index)");
	CvAssertMsg(eUnitCombat < GC.getNumUnitCombatClassInfos(), "eUnitCombat is expected to be within maximum bounds (invalid Index)");
	return (getExtraUnitCombatModifier(eUnitCombat));
}


//	--------------------------------------------------------------------------------
int CvUnit::domainModifier(DomainTypes eDomain) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	CvAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return (getExtraDomainModifier(eDomain));
}

//	--------------------------------------------------------------------------------
int CvUnit::GetYieldModifier(YieldTypes eYield) const
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	return m_YieldModifier[eYield];
}
//	--------------------------------------------------------------------------------
void CvUnit::SetYieldModifier(YieldTypes eYield, int iValue)
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	m_YieldModifier[eYield] = (m_YieldModifier[eYield] + iValue);
}

int CvUnit::GetYieldChange(YieldTypes eYield) const
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	return m_YieldChange[eYield];
}
//	--------------------------------------------------------------------------------
void CvUnit::SetYieldChange(YieldTypes eYield, int iValue)
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	m_YieldChange[eYield] = (m_YieldChange[eYield] + iValue);
}
// Similar to above code but is only for combat units and scales per the units combat strength : yield is placed on the plot itself
int CvUnit::GetGarrisonYieldChange(YieldTypes eYield) const
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	return m_iGarrisonYieldChange[eYield];
}
//	--------------------------------------------------------------------------------
void CvUnit::SetGarrisonYieldChange(YieldTypes eYield, int iValue)
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	m_iGarrisonYieldChange[eYield] = (m_iGarrisonYieldChange[eYield] + iValue);
}

int CvUnit::GetFortificationYieldChange(YieldTypes eYield) const
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	return m_iFortificationYieldChange[eYield];
}
//	--------------------------------------------------------------------------------
void CvUnit::SetFortificationYieldChange(YieldTypes eYield, int iValue)
{
	VALIDATE_OBJECT
		CvAssertMsg(eYield >= 0, "eYield is expected to be non-negative (invalid Index)");
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "eYield is expected to be within maximum bounds (invalid Index)");
	m_iFortificationYieldChange[eYield] = (m_iFortificationYieldChange[eYield] + iValue);
}

//	--------------------------------------------------------------------------------
#if defined(MOD_CARGO_SHIPS)
SpecialUnitTypes CvUnit::specialUnitCargoLoad() const
{
	VALIDATE_OBJECT
	return((SpecialUnitTypes)(m_pUnitInfo->GetSpecialUnitCargoLoad()));
}
#endif

//	--------------------------------------------------------------------------------
SpecialUnitTypes CvUnit::specialCargo() const
{
	VALIDATE_OBJECT
	return ((SpecialUnitTypes)(m_pUnitInfo->GetSpecialCargo()));
}


//	--------------------------------------------------------------------------------
DomainTypes CvUnit::domainCargo() const
{
	VALIDATE_OBJECT
	return ((DomainTypes)(m_pUnitInfo->GetDomainCargo()));
}


//	--------------------------------------------------------------------------------
int CvUnit::cargoSpace() const
{
	VALIDATE_OBJECT
	return m_iCargoCapacity;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeCargoSpace(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iCargoCapacity += iChange;
		CvAssert(m_iCargoCapacity >= 0);
		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFull() const
{
	VALIDATE_OBJECT
	return (getCargo() >= cargoSpace());
}

//	--------------------------------------------------------------------------------
int CvUnit::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	VALIDATE_OBJECT
	if (!CanHaveCargo(eSpecialCargo, eDomainCargo))
		return 0;

	return std::max(0, (cargoSpace() - getCargo()));
}

// This ignores number of slots
bool CvUnit::CanHaveCargo(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	VALIDATE_OBJECT
	if (specialCargo() != NO_SPECIALUNIT && specialCargo() != eSpecialCargo)
	{
		if (!MOD_CARGO_SHIPS || specialUnitCargoLoad() == NO_SPECIALUNIT || specialUnitCargoLoad() != eSpecialCargo)
		{
			return false;
		}
	}

	if (domainCargo() != NO_DOMAIN && domainCargo() != eDomainCargo)
		return false;

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::hasCargo() const
{
	VALIDATE_OBJECT
	return (getCargo() > 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::canCargoAllMove() const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode = NULL;
	const CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if(!(pLoopUnit->canMove()))
				{
					return false;
				}
			}
		}
	}

	return true;
}


//	--------------------------------------------------------------------------------
int CvUnit::getUnitAICargo(UnitAITypes eUnitAI) const
{
	VALIDATE_OBJECT
	const IDInfo* pUnitNode = NULL;
	const CvUnit* pLoopUnit = NULL;
	CvPlot* pPlot = NULL;
	int iCount = 0;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while(pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				iCount++;
			}
		}
	}

	return iCount;
}

//	--------------------------------------------------------------------------------
IDInfo CvUnit::GetIDInfo() const
{
	VALIDATE_OBJECT
	IDInfo unit(getOwner(), GetID());
	return unit;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetID(int iID)
{
	VALIDATE_OBJECT
	m_iID = iID;
}


//	--------------------------------------------------------------------------------
int CvUnit::getHotKeyNumber() const
{
	VALIDATE_OBJECT
	return m_iHotKeyNumber;
}


//	--------------------------------------------------------------------------------
void CvUnit::setHotKeyNumber(int iNewValue)
{
	VALIDATE_OBJECT
	CvUnit* pLoopUnit = NULL;
	int iLoop = 0;

	CvAssert(getOwner() != NO_PLAYER);

	if(getHotKeyNumber() != iNewValue)
	{
		if(iNewValue != -1)
		{
			for(pLoopUnit = GET_PLAYER(getOwner()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwner()).nextUnit(&iLoop))
			{
				if(pLoopUnit->getHotKeyNumber() == iNewValue)
				{
					pLoopUnit->setHotKeyNumber(-1);
				}
			}
		}

		m_iHotKeyNumber = iNewValue;
	}
}

//	-----------------------------------------------------------------------------------------------------------
void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible, bool bNoMove)
{
	VALIDATE_OBJECT
	IDInfo* pUnitNode = 0;
	CvCity* pOldCity = 0;
	CvCity* pNewCity = 0;
	CvUnit* pTransportUnit = 0;
	CvUnit* pLoopUnit = 0;
	vector<IDInfo> oldUnitList;
	vector<CvUnitCaptureDefinition> kCaptureUnitList;
	ActivityTypes eOldActivityType = NO_ACTIVITY;
	TeamTypes activeTeam = GC.getGame().getActiveTeam();
	TeamTypes eOurTeam = getTeam();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	bool bOwnerIsActivePlayer = GC.getGame().getActivePlayer() == getOwner();
	// Delay any popups that might be caused by our movement (goody huts, natural wonders, etc.) so the unit movement event gets sent before the popup event.
	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(true);

	//avoid recursion with jump to valid plot
	if (at(iX, iY))
		return;

	CvAssert(!isFighting());
	CvAssert((iX == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getX() == iX));
	CvAssert((iY == INVALID_PLOT_COORD) || (GC.getMap().plot(iX, iY)->getY() == iY));

	eOldActivityType = GetActivityType();

	/* ---------
	// no longer used!
	// ---------
	if(isSetUpForRangedAttack())
	{
		setSetUpForRangedAttack(false);
	}
	*/

	if(!bGroup || isCargo())
	{
		bShow = false;
	}

	//sanity check
	CvPlot* pNewPlot = GC.getMap().plot(iX, iY);
	if (pNewPlot)
	{
		if ( !pNewPlot->needsEmbarkation(this) && isEmbarked() && !isCargo() )
			setEmbarked(false);
		if ( pNewPlot->needsEmbarkation(this) && CanEverEmbark() && !isEmbarked() && !isCargo() )
			setEmbarked(true);
	}

	if(pNewPlot != NULL && !bNoMove)
	{
		pTransportUnit = getTransportUnit();

		if(pTransportUnit != NULL)
		{
			if(!(pTransportUnit->atPlot(*pNewPlot)))
			{
				setTransportUnit(NULL);
			}
		}

		if(IsCombatUnit() && !isDelayedDeath())
		{
			oldUnitList.clear();

			pUnitNode = pNewPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				oldUnitList.push_back(*pUnitNode);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			}

			int iUnitListSize = (int) oldUnitList.size();
			for(int iVectorLoop = 0; iVectorLoop < iUnitListSize; ++iVectorLoop)
			{
				pLoopUnit = ::GetPlayerUnit(oldUnitList[iVectorLoop]);
				{
					if(pLoopUnit && !pLoopUnit->isDelayedDeath())
					{
						if(isEnemy(pLoopUnit->getTeam(), pNewPlot) || pLoopUnit->isEnemy(eOurTeam))
						{
							if(!pLoopUnit->canCoexistWithEnemyUnit(eOurTeam))
							{
								// Unit somehow ended up on top of an enemy combat unit
								if(NO_UNITCLASS == pLoopUnit->getUnitInfo().GetUnitCaptureClassType() && pLoopUnit->IsCanDefend())
								{
									if(!pNewPlot->isCity())
									{
										if (!pLoopUnit->jumpToNearestValidPlot())
											pLoopUnit->kill(false, getOwner());
									}
									else
									{
										kPlayer.DoYieldsFromKill(this, pLoopUnit);
										pLoopUnit->kill(false, getOwner());
									}
								}
								// Ran into a noncombat unit
								else
								{
									bool bDisplaced = false;
									ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
									if(pkScriptSystem)
									{
										CvLuaArgsHandle args;
										args->Push(pLoopUnit->getOwner());
										args->Push(pLoopUnit->GetID());  // captured unit

										bool bResult = false;
										if (LuaSupport::CallTestAny(pkScriptSystem, "CanDisplaceCivilian", args.get(), bResult))
										{
											// Check the result.
											if(bResult)
											{
												bDisplaced = true;
											}
										}
									}

									if (!bDisplaced)
									{
										bool bDoCapture = false;
										bool bDoEvade = false;
										if (pLoopUnit->IsCivilianUnit() && pLoopUnit->CheckWithdrawal(*this) && pLoopUnit->DoFallBack(*this, true))
										{
											bDoEvade = true;
											CvNotifications* pNotification = GET_PLAYER(pLoopUnit->getOwner()).GetNotifications();
											if (pNotification)
											{
												Localization::String strMessage;
												Localization::String strSummary;
												strMessage = Localization::Lookup("TXT_KEY_UNIT_WITHDREW_DETAILED_PIONEER");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_WITHDREW_PIONEER");
												pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pLoopUnit->getX(), pLoopUnit->getY(), (int)pLoopUnit->getUnitType(), pLoopUnit->getOwner());
											}
										}

										// Some units can't capture civilians. Embarked units are also not captured, they're simply killed. And some aren't a type that gets captured.
										if( (!pLoopUnit->isEmbarked() || pLoopUnit->getUnitInfo().IsCaptureWhileEmbarked()) && 
											pLoopUnit->getCaptureUnitType(getOwner()) != NO_UNIT && 
											!bDoEvade )
										{
											bDoCapture = true;

											Localization::String strMessage;
											Localization::String strSummary;
											if(isBarbarian())
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_BARBS");
											}
											else
											{
												strMessage = Localization::Lookup("TXT_KEY_UNIT_CAPTURED_DETAILED");
												strMessage << pLoopUnit->getUnitInfo().GetTextKey() << GET_PLAYER(getOwner()).getNameKey();
												strSummary = Localization::Lookup("TXT_KEY_UNIT_CAPTURED");
											}
										}
										// Unit was killed instead

										if (MOD_EVENTS_UNIT_CAPTURE) {
											GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitCaptured, getOwner(), GetID(), pLoopUnit->getOwner(), pLoopUnit->GetID(), !bDoCapture, 0);
										}
										if (!bDoEvade)
										{
											if (!bDoCapture)
											{
												CvString strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameKey(), 0, pLoopUnit->getNameKey());
												DLLUI->AddUnitMessage(0, GetIDInfo(), getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
												if (MOD_WH_MILITARY_LOG)
													MILITARYLOG(getOwner(), strBuffer.c_str(), plot(), pLoopUnit->getOwner());
											}

											kPlayer.DoYieldsFromKill(this, pLoopUnit);
											kPlayer.DoUnitKilledCombat(this, pLoopUnit->getOwner(), pLoopUnit->getUnitType());
											CvNotifications* pNotification = GET_PLAYER(pLoopUnit->getOwner()).GetNotifications();
											if (pNotification)
											{
												Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_LOST");
												Localization::String strSummary = strMessage;
												pNotification->Add(NOTIFICATION_UNIT_DIED, strMessage.toUTF8(), strSummary.toUTF8(), pLoopUnit->getX(), pLoopUnit->getY(), (int)pLoopUnit->getUnitType(), pLoopUnit->getOwner());
											}

											// If we're capturing the unit, we want to delay the capture, else as the unit is converted to our side, it will be the first unit on our
											// side in the plot and can end up taking over a city, rather than the advancing unit
											CvUnitCaptureDefinition kCaptureDef;
											if(bDoCapture)
											{
												if(pLoopUnit->getCaptureDefinition(&kCaptureDef, getOwner()))
													kCaptureUnitList.push_back(kCaptureDef);
												pLoopUnit->setCapturingPlayer(NO_PLAYER);	// Make absolutely sure this is not valid so the kill does not do the capture.
											}

											pLoopUnit->kill(false, getOwner());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	CvPlot* pOldPlot = plot();
	CvCity* pkPrevGarrisonedCity = GetGarrisonedCity();

	if(pOldPlot != NULL)
	{
		pOldPlot->removeUnit(this, bUpdate);
		// if leaving a city, reveal the unit
		if (pOldPlot->isCity())
		{
			// if pNewPlot is a valid pointer, we are leaving the city and need to visible
			// if pNewPlot is NULL than we are "dead" (e.g. a settler) and need to blend out
			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitVisibility(pDllUnit.get(), pNewPlot != NULL && !this->isInvisible(activeTeam, false));
		}

		if (canChangeVisibility())
		{
			pOldPlot->changeAdjacentSight(eOurTeam, visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);
		}

		if (m_iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			if (isNativeDomain(pOldPlot))
				pOldPlot->area()->changeUnitsPerPlayer(getOwner(), -1);
			setLastMoveTurn(GC.getGame().getGameTurn());
			pOldCity = pOldPlot->getPlotCity();
		}
	}

	if(pNewPlot != NULL)
	{
		m_iX = pNewPlot->getX();
		m_iY = pNewPlot->getY();
	}
	else
	{
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
	}

	CvAssertMsg(plot() == pNewPlot, "plot is expected to equal pNewPlot");

	if(pNewPlot != NULL)
	{
		//update area effects
		kPlayer.UpdateAreaEffectUnit(this);

		//update facing direction
		if(pOldPlot != NULL)
		{
			DirectionTypes newDirection = directionXY(pOldPlot, pNewPlot);
			if(newDirection != NO_DIRECTION)
				m_eFacingDirection = newDirection;
		}

		//update cargo mission animations
		if(isCargo())
		{
			if(eOldActivityType != ACTIVITY_MISSION)
			{
				SetActivityType(eOldActivityType);
			}
		}

		// if entering a city, hide the unit
		if(pNewPlot->isCity())
		{
			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitVisibility(pDllUnit.get(), false /*bVisible*/);
		}

		// safety if ever we call setXY directly without a mission
		SetFortified(false);

		if (canChangeVisibility())
		{
			pNewPlot->changeAdjacentSight(eOurTeam, visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true), this); // needs to be here so that the square is considered visible when we move into it...
		}

		if (m_iMapLayer == DEFAULT_UNIT_MAP_LAYER)
		{
			pNewPlot->addUnit(this, bUpdate);
			if (isNativeDomain(pNewPlot))
				pNewPlot->area()->changeUnitsPerPlayer(getOwner(), +1);
			pNewCity = pNewPlot->getPlotCity();
		}
		else
		{
			GC.getMap().plotManager().AddUnit(GetIDInfo(), iX, iY, m_iMapLayer);
		}
		if (isGiveInvisibility())
		{
			int iRange = GetNearbyUnitPromotionsRange();
			int iMax = maxMoves();
			int iLoop = 0;
			for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if (!pLoopUnit->IsCombatUnit())
					continue;

				if (getGiveDomain() != NO_DOMAIN && (getGiveDomain() != pLoopUnit->getDomainType()))
					continue;

				if (plotDistance(pNewPlot->getX(), pNewPlot->getY(), pLoopUnit->getX(), pLoopUnit->getY()) > (iRange + iMax +1))
					continue;
				if (pLoopUnit->IsHiddenByNearbyUnit(pLoopUnit->plot()))
				{
					pLoopUnit->plot()->updateVisibility();
				}
				else
				{
					if (pLoopUnit->getInvisibleType() == NO_INVISIBLE && eOurTeam != activeTeam)
					{
						CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(pLoopUnit));
						gDLL->GameplayUnitVisibility(pDllUnit.get(), true, true);
					}
				}
			}
		}
		if (isConvertUnit())
		{
			if (getConvertDomain()!= NO_DOMAIN && (pNewPlot->getDomain() == getConvertDomain()))
			{
				if (getConvertDomainUnitType() != NO_UNIT)
				{
					UnitAITypes eAIType = NO_UNITAI;
					const CvUnitEntry* pkUnitType = GC.getUnitInfo(getConvertDomainUnitType());
					if (pkUnitType != NULL)
					{
						//! Be sure the unit your initializing has as its unitinfo (GetDomainType() == IsConvertDomain(pNewPlot->getDomain())
						eAIType = pkUnitType->GetDefaultUnitAIType();
						CvUnit* pNewUnit = GET_PLAYER(getOwner()).initUnit(getConvertDomainUnitType(), getX(), getY(), eAIType, REASON_CONVERT, true, true, 0, 0, NO_CONTRACT, false);
						kill(true, NO_PLAYER);
						pNewUnit->finishMoves();
					}
				}
			}
		}

		if(IsCombatUnit())
		{
			pUnitNode = pNewPlot->headUnitNode();
			while(pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
				if (pLoopUnit != NULL)
				{
#if defined(MOD_GLOBAL_STACKING_RULES)
					if(MOD_GLOBAL_STACKING_RULES)
					{
						if(pLoopUnit->IsStackingUnit())
						{
							if(pNewPlot->GetNumCombatUnits()>1)
							{
								pLoopUnit->SetBaseCombatStrength(pLoopUnit->getUnitInfo().GetCombat() + pLoopUnit->getUnitInfo().StackCombat());
							}
							else
							{
								pLoopUnit->SetBaseCombatStrength(pLoopUnit->getUnitInfo().GetCombat());
							}
						}
					}
#endif
				}
			}
		}

		DoNearbyUnitPromotion(pNewPlot);
		if(getAoEDamageOnMove() != 0)
			DoAdjacentPlotDamage(pNewPlot, getAoEDamageOnMove(), "TXT_KEY_MISC_YOU_UNIT_WAS_DAMAGED_AOE_STRIKE_ON_MOVE");

		// Moving into a City (friend or foe)
		if(pNewCity != NULL)
		{
			if(isEnemy(pNewCity->getTeam()))
			{
				PlayerTypes eNewOwner = GET_PLAYER(getOwner()).pickConqueredCityOwner(*pNewCity);

				if(NO_PLAYER != eNewOwner)
				{
					if (MOD_API_ACHIEVEMENTS)
					{
						//Test for City Achievements here since we need to know about the capturing unit.
						if (getOwner() == GC.getGame().getActivePlayer())
						{
							//Expansion 1 - Capture the city that built Petra using a Landship
							if(strcmp(getUnitInfo().GetType(), "UNIT_WWI_TANK") == 0)
							{
								const int numBuildings = GC.getNumBuildingInfos();
								for(int i = 0; i < numBuildings; ++i)
								{
									const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
									const CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

									if(pkBuildingInfo != NULL && strcmp(pkBuildingInfo->GetType(), "BUILDING_PETRA") == 0)
									{
										if(pNewCity->GetCityBuildings()->GetNumRealBuilding(eBuilding) > 0)
										{
											gDLL->UnlockAchievement(ACHIEVEMENT_XP1_26);
										}
										break;
									}
								}
							}

							//Expansion 1 - Capture a Spanish Coastal City with a Dutch Sea Begger
							if(strcmp(getUnitInfo().GetType(), "UNIT_DUTCH_SEA_BEGGAR") == 0)
							{
								if(pNewCity->isCoastal())
								{
									if(strcmp(pNewCity->getCivilizationInfo().GetType(), "CIVILIZATION_SPAIN") == 0)
									{
										gDLL->UnlockAchievement(ACHIEVEMENT_XP1_28);
									}
								}
							}

							// Expansion 2 - Capture Venice's holy city as Venice
							// if venice
							//   find venice religion
							//   compare religion to if city is holy city of conquered
							if (!GC.getGame().isGameMultiPlayer())
							{
								if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_VENICE") == 0)
								{
									CvCity* pCity = GET_PLAYER(m_eOwner).getCapitalCity();
									ReligionTypes eReligion = pCity->GetCityReligions()->GetReligiousMajority();
									if (eReligion != NO_RELIGION)
									{
										if (pNewCity->GetCityReligions()->IsHolyCityForReligion(eReligion))
										{
											gDLL->UnlockAchievement(ACHIEVEMENT_XP2_26);
										}
									}
								}
							}
						}

						bool bUsingXP1Scenario2 = gDLL->IsModActivated(CIV5_XP1_SCENARIO2_MODID);
						if(bUsingXP1Scenario2)
						{
							if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_SONGHAI") == 0)
							{
								const int iRomeX = 33;
								const int iRomeY = 31;

								//Did we, as vandals, just capture Rome with a boat?
								if(pNewCity->getX() == iRomeX && pNewCity->getY() == iRomeY && getDomainType() == DOMAIN_SEA)
								{
									gDLL->UnlockAchievement(ACHIEVEMENT_XP1_52);
								}
							}
						}
					}

					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false, false); // will delete the pointer
					pNewCity = NULL;

					//it might happen that we liberate the city right after acquiring it
					//in that case our unit is teleported somewhere or killed if that fails
					//if it's killed, we have an invalid pointer here, let's hope that the plot() check catches it
					//if it was teleported, setXY was called again from acquireCity and we don't want to overwrite what happened there
					if (plot() == NULL || plot() != pNewPlot)
						return;
				}
			}
		}

		if(shouldLoadOnMove(pNewPlot))
		{
			load();
		}
#if defined(MOD_CARGO_SHIPS)
		if(IsCombatUnit())
		{
			if(MOD_CARGO_SHIPS)
			{
				pUnitNode = pNewPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					pLoopUnit = ::GetPlayerUnit(*pUnitNode);
					pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
					if (pLoopUnit != NULL)
					{				
						if(pLoopUnit->IsCargoCombatUnit() && pLoopUnit->hasCargo())
						{
							DoCargoPromotions(*pLoopUnit);
						}
					}
				}
			}
		}
#endif

		bool bZero = false;
		if(IsGainsXPFromScouting())
		{
			int iExperience = /*1*/ GD_INT_GET(BALANCE_SCOUT_XP_BASE);
			iExperience += GetNumTilesRevealedThisTurn();
			iExperience /= 6;
			if(iExperience > 0)
			{
				//Up to max barb value - rest has to come through combat!
				changeExperienceTimes100(iExperience * 100);
				bZero = true;
			}
		}
		if(IsGainsYieldFromScouting())
		{
			bool bSea = false;
			if(getDomainType() == DOMAIN_SEA)
			{
				bSea = true;
			}
			CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), getTeam());

			if(pCity == NULL)
			{
				pCity = GET_PLAYER(getOwner()).getCapitalCity();
			}
			if(pCity != NULL)
			{
				GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_SCOUTING, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pCity, bSea, true, false, NO_YIELD, this);
				bZero = true;
			}
		}
		if(bZero)
		{
			SetNumTilesRevealedThisTurn(0);
		}

		// Can someone can see the plot we moved our Unit into?
		for(int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			TeamTypes eTeamLoop = (TeamTypes) iI;

			CvTeam& kLoopTeam = GET_TEAM(eTeamLoop);

			if(kLoopTeam.isAlive())
			{
				// Human can't be met by an AI spotting him.  If both players are human then they don't need to see one another
				if(!isHuman() || kLoopTeam.isHuman())
				{
					if(!isInvisible(eTeamLoop, false))
					{
						if(pNewPlot->isVisible(eTeamLoop))
						{
							kLoopTeam.meet(eOurTeam, false);
						}
					}
				}
			}
		}

		// if I was invisible to the active team but won't be any more or vice versa
		InvisibleTypes eInvisoType = getInvisibleType();
		if(eOurTeam != activeTeam && eInvisoType != NO_INVISIBLE)
		{
			bool bOldInvisibleVisible = false;
			if(pOldPlot)
				bOldInvisibleVisible = pOldPlot->isInvisibleVisible(activeTeam, eInvisoType);
			bool bNewInvisibleVisible = pNewPlot->isInvisibleVisible(activeTeam, eInvisoType);
			if(bOldInvisibleVisible != bNewInvisibleVisible)
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisible, true);
			}
		}
		if (eOurTeam != activeTeam && (IsHiddenByNearbyUnit(pNewPlot)))
		{
			bool bOldInvisibleVisibleUnit = false;
			if (pOldPlot)
				bOldInvisibleVisibleUnit = pOldPlot->isInvisibleVisibleUnit(activeTeam);
			bool bNewInvisibleVisibleUnit = pNewPlot->isInvisibleVisibleUnit(activeTeam);
			if (bOldInvisibleVisibleUnit != bNewInvisibleVisibleUnit)
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), bNewInvisibleVisibleUnit, true);
			}
		}
		else if (eOurTeam != activeTeam && pOldPlot && (IsHiddenByNearbyUnit(pOldPlot)))
		{
			if (!IsHiddenByNearbyUnit(pNewPlot))
			{
				CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
				gDLL->GameplayUnitVisibility(pDllUnit.get(), true, true);
			}
		}

		CvTeam& kOurTeam = GET_TEAM(eOurTeam);

		// If a Unit is adjacent to someone's borders, meet them
		CvPlot* pAdjacentPlot = NULL;
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				// Owned by someone
				if(pAdjacentPlot->getTeam() != NO_TEAM)
				{
					kOurTeam.meet(pAdjacentPlot->getTeam(), false);
				}

				if(pAdjacentPlot->isCity())
				{
					CvCity* pAdjCity = pAdjacentPlot->getPlotCity();
					{
						if(pAdjCity != NULL && !isEnemy(pAdjCity->getTeam()))
						{
							pAdjCity->updateStrengthValue();
						}
					}
				}

				// Have a naval unit here?
				if(isBarbarian() && getDomainType() == DOMAIN_SEA && pAdjacentPlot->isWater())
				{
					CvUnit* pAdjacentUnit = pAdjacentPlot->getBestDefender(NO_PLAYER, BARBARIAN_PLAYER, NULL, true);
					if(pAdjacentUnit)
					{
						GET_PLAYER(pAdjacentUnit->getOwner()).GetPlayerTraits()->CheckForBarbarianConversion(this, pAdjacentPlot);
					}
				}

				// Is this a missionary and do we have a belief that converts barbarians here?
				UnitClassTypes eMissionary = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MISSIONARY", true);
				if (eMissionary != NO_UNITCLASS)
				{
					if (getUnitClassType() == eMissionary)
					{
						ReligionTypes eOwnedReligion = kPlayer.GetReligions()->GetOwnedReligion();
						const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eOwnedReligion, kPlayer.GetID());

						if (pReligion)
						{
							CvCity* pHolyCity = pReligion->GetHolyCity();

							if (pReligion->m_Beliefs.IsConvertsBarbarians(getOwner(), pHolyCity))
							{
								CvUnit* pAdjacentUnit = pAdjacentPlot->getBestDefender(BARBARIAN_PLAYER);
								if (pAdjacentUnit)
								{
									CvBeliefHelpers::ConvertBarbarianUnit(this, pAdjacentUnit);
								}
							}
						}
					}
					// Or other way around, barbarian moving up to a missionary
					else if (isBarbarian())
					{
						if(pAdjacentPlot->getNumUnits() > 0)
						{
							for(int iNearbyUnitLoop = 0; iNearbyUnitLoop < pAdjacentPlot->getNumUnits(); iNearbyUnitLoop++)
							{
								const CvUnit* const adjUnit = pAdjacentPlot->getUnitByIndex(iNearbyUnitLoop);
								if (adjUnit && adjUnit->getUnitClassType() == eMissionary)
								{
									CvPlayer &adjUnitPlayer = GET_PLAYER(adjUnit->getOwner());
									ReligionTypes eFoundedReligion = adjUnitPlayer.GetReligions()->GetOwnedReligion();
									const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eFoundedReligion, adjUnitPlayer.GetID());

									if (pReligion)
									{
										CvCity* pHolyCity = pReligion->GetHolyCity();

										if (pReligion->m_Beliefs.IsConvertsBarbarians(getOwner(), pHolyCity))
										{
											if (CvBeliefHelpers::ConvertBarbarianUnit(adjUnit, this))
											{
												ClearMissionQueue();
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (pOldPlot != NULL)
		{
			DoLocationPromotions(false, pOldPlot, pNewPlot);
			/*DoConvertEnemyUnitToBarbarian(pNewPlot);*/
		}

		if(pOldPlot != NULL && getDomainType() == DOMAIN_SEA)
		{
			kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(this, pNewPlot);
		}

		if(GC.IsGraphicsInitialized())
		{
			//override bShow if check plot visible
			if(bCheckPlotVisible && (pNewPlot->isVisibleToWatchingHuman() || (pOldPlot && pOldPlot->isVisibleToWatchingHuman())))
				bShow = true;

			if(CvPreGame::quickMovement())
			{
				bShow = false;
			}

			if(bShow)
			{
				QueueMoveForVisualization(pNewPlot);
			}
			else
			{
				SetPosition(pNewPlot);
			}
		}

	}

	if(pOldPlot != NULL)
	{
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentOldPlot = plotDirection(pOldPlot->getX(), pOldPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentOldPlot != NULL)
			{
				if(pAdjacentOldPlot->isCity())
				{
					CvCity* pAdjOldCity = pAdjacentOldPlot->getPlotCity();
					{
						if(pAdjOldCity != NULL && !isEnemy(pAdjOldCity->getTeam()))
						{
							pAdjOldCity->updateStrengthValue();
						}
					}
				}
			}
		}

		if(hasCargo())
		{
			pUnitNode = pOldPlot->headUnitNode();

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);

				if(pLoopUnit && pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->setXY(iX, iY, bGroup, bUpdate);

					// Reset to head node since we just moved some cargo around, and the unit storage in the plot is going to be different now
					pUnitNode = pOldPlot->headUnitNode();
				}
			}
		}

		if(IsCombatUnit())
		{
			pUnitNode = pOldPlot->headUnitNode();
			while(pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);
				if (pLoopUnit != NULL)
				{
#if defined(MOD_GLOBAL_STACKING_RULES)
					if(MOD_GLOBAL_STACKING_RULES)
					{
						if(pLoopUnit->IsStackingUnit())
						{
							if(pOldPlot->GetNumCombatUnits()>1)
							{
								pLoopUnit->SetBaseCombatStrength(pLoopUnit->getUnitInfo().GetCombat() + pLoopUnit->getUnitInfo().StackCombat());
							}
							else
							{
								pLoopUnit->SetBaseCombatStrength(pLoopUnit->getUnitInfo().GetCombat());
							}
						}
					}
#endif
#if defined(MOD_CARGO_SHIPS)
					if(MOD_CARGO_SHIPS)
					{
						if(pLoopUnit->IsCargoCombatUnit())
						{
							RemoveCargoPromotions(*pLoopUnit);
						}
					}
#endif
				}
			}
		}
	}

	if(pNewPlot != NULL && m_iMapLayer == DEFAULT_UNIT_MAP_LAYER)
	{
		if(!bNoMove)
		{
			if(pNewPlot->isGoody(getTeam()))
			{
				GET_PLAYER(getOwner()).doGoody(pNewPlot, this);
			}
			if(!isBarbarian())
			{
				if(pNewPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
				{
					// See if we need to remove a temporary focus of attention
					kPlayer.GetTacticalAI()->DeleteFocusArea(pNewPlot);
					
					if (!kPlayer.GetPlayerTraits()->CheckForBarbarianConversion(this, pNewPlot))
					{
						CvBarbarians::DoBarbCampCleared(pNewPlot, getOwner(), this);
					}
					else if (kPlayer.GetPlayerTraits()->GetLandBarbarianConversionPercent() > 0)
					{
						int iBestCityID = -1;

						int iBestCityDistance = -1;

						int iDistance = 0;

						CvCity* pLoopCity = NULL;
						int iLoop = 0;
						if (pNewPlot)
						{
							for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
							{
								CvPlot* pCityPlot = pLoopCity->plot();
								if (pCityPlot)
								{
									iDistance = plotDistance(pNewPlot->getX(), pNewPlot->getY(), pCityPlot->getX(), pCityPlot->getY());

									if (iBestCityDistance == -1 || iDistance < iBestCityDistance)
									{
										iBestCityID = pLoopCity->GetID();
										iBestCityDistance = iDistance;
									}
								}
							}
						}

						CvCity* pBestCity = kPlayer.getCity(iBestCityID);
						if (pBestCity == NULL)
							pBestCity = kPlayer.getCapitalCity();

						if (pBestCity != NULL)
						{

							if (kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) > 0 || kPlayer.GetBarbarianCombatBonus(true) > 0)
							{
								int iCulturePoints = (kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 5);
								if (iCulturePoints <= 0)
								{
									iCulturePoints = kPlayer.GetBarbarianCombatBonus(true);
								}

								kPlayer.doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, iCulturePoints, true, NO_PLAYER, NULL, false, pBestCity, false, true, false, YIELD_CULTURE, this);

								if (getOwner() == GC.getGame().getActivePlayer())
								{
									char text[256] = { 0 };

									sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCulturePoints);
									SHOW_PLOT_POPUP(pNewPlot, getOwner(), text);
								}
							}
						}
					}

					pNewPlot->setImprovementType(NO_IMPROVEMENT);
					// Set who last cleared the camp here
					pNewPlot->SetPlayerThatClearedBarbCampHere(getOwner());
	
				}
			}
		}

		// New plot location is owned by someone
		PlayerTypes ePlotOwner = pNewPlot->getOwner();
		if(ePlotOwner != NO_PLAYER)
		{
			// Are we in enemy territory? If so, give notification to owner
			if(GET_TEAM(getTeam()).isAtWar(GET_PLAYER(ePlotOwner).getTeam()) && !isInvisible(GET_PLAYER(ePlotOwner).getTeam(),false,false))
			{
				CvNotifications* pNotifications = GET_PLAYER(ePlotOwner).GetNotifications();
				if(pNotifications)
				{
					Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY");
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_ENEMY_SUMMARY");
					pNotifications->Add(NOTIFICATION_ENEMY_IN_TERRITORY, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, getUnitType(), getOwner());
				}
			}
		}
	}

	// Create any units we captured, now that we own the destination
	for(uint uiCaptureIndex = 0; uiCaptureIndex < kCaptureUnitList.size(); ++uiCaptureIndex)
	{
		createCaptureUnit(kCaptureUnitList[uiCaptureIndex]);
	}

	CvAssert(pOldPlot != pNewPlot);

	if(IsSelected())
		gDLL->GameplayMinimapUnitSelect(iX, iY);

	setInfoBarDirty(true);

	// if there is an enemy city nearby, alert any scripts to this
	int iAttackRange = MOD_EVENTS_CITY_BOMBARD ? /*2*/ GD_INT_GET(MAX_CITY_ATTACK_RANGE) : /*2*/ GD_INT_GET(CITY_ATTACK_RANGE);
	for(int iDX = -iAttackRange; iDX <= iAttackRange; iDX++)
	{
		for(int iDY = -iAttackRange; iDY <= iAttackRange; iDY++)
		{
			CvPlot* pTargetPlot = plotXYWithRangeCheck(getX(), getY(), iDX, iDY, iAttackRange);
			if(pTargetPlot && pTargetPlot->isCity())
			{
				if(isEnemy(pTargetPlot->getTeam()))
				{
					// do it
					CvCity* pkPlotCity = pTargetPlot->getPlotCity();
					if (!MOD_EVENTS_CITY_BOMBARD || plotXYWithRangeCheck(getX(), getY(), iDX, iDY, pkPlotCity->getBombardRange()))
					{
						CvInterfacePtr<ICvCity1> pPlotCity = GC.WrapCityPointer(pkPlotCity);
						DLLUI->SetSpecificCityInfoDirty(pPlotCity.get(), CITY_UPDATE_TYPE_ENEMY_IN_RANGE);
					}
				}
			}
		}
	}

	if (bOwnerIsActivePlayer)
		DLLUI->SetDontShowPopups(false);

	// update garrison status for old and new city, if applicable
	if (pkPrevGarrisonedCity)
	{
		//when moving out, another unit might be present to take over garrison duty
		pkPrevGarrisonedCity->SetGarrison( pkPrevGarrisonedCity->plot()->getBestGarrison( pkPrevGarrisonedCity->getOwner() ) );
		CvInterfacePtr<ICvCity1> pkDllCity(new CvDllCity(pkPrevGarrisonedCity));
		DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
		pkPrevGarrisonedCity->updateYield();
	}
	else if(pNewPlot)
	{
		if (pNewPlot->isCity())
		{
			//when moving in, see if we're better than the previous garrison
			CvUnit* pBestGarrison = pNewPlot->getBestGarrison(pNewPlot->getOwner());
			if (pBestGarrison==this)
			{
				CvCity* pkNewGarrisonedCity = pNewPlot->getPlotCity();
				pkNewGarrisonedCity->SetGarrison(this);
				CvInterfacePtr<ICvCity1> pkDllCity(new CvDllCity(pkNewGarrisonedCity));
				DLLUI->SetSpecificCityInfoDirty(pkDllCity.get(), CITY_UPDATE_TYPE_GARRISON);
				pkNewGarrisonedCity->updateYield();
			}
		}
		else //no city
		{
			//normally re-setting the garrison in pkPrevGarrisonedCity should have set this already, but better be safe
			SetGarrisonedCity(-1);
		}
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(getOwner());
		args->Push(GetID());
		args->Push(getX());
		args->Push(getY());

		bool bResult = false;
		LuaSupport::CallHook(pkScriptSystem, "UnitSetXY", args.get(), bResult);
	}

	//Dr. Livingstone I presume?
	if (MOD_API_ACHIEVEMENTS && isHuman() && !isDelayedDeath())
	{
		if(strcmp(getCivilizationInfo().GetType(), "CIVILIZATION_BRAZIL") == 0)
		{
			UnitTypes eExplorer = (UnitTypes) GC.getInfoTypeForString("UNIT_EXPLORER", true /*bHideAssert*/); 
			if(getUnitType() == eExplorer && strcmp(getNameNoDesc(), "TXT_KEY_EXPLORER_STANLEY") == 0 )
			{
				for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					CvPlot* pAdjacentPlot = plotDirection(pNewPlot->getX(), pNewPlot->getY(), ((DirectionTypes)iI));

					if(pAdjacentPlot != NULL)
					{
						for(int iJ = 0; iJ < pAdjacentPlot->getNumUnits(); iJ++)
						{
							if(pAdjacentPlot->getUnitByIndex(iJ)->getUnitType() ==  eExplorer && strcmp(pAdjacentPlot->getUnitByIndex(iJ)->getNameNoDesc(), "TXT_KEY_EXPLORER_LIVINGSTON") == 0)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP2_52);
							}
						}
					}
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------
bool CvUnit::at(int iX, int iY) const
{
	VALIDATE_OBJECT
	return((getX() == iX) && (getY() == iY));
}


//	--------------------------------------------------------------------------------
bool CvUnit::atPlot(const CvPlot& checkPlot) const
{
	VALIDATE_OBJECT
	return((getX() == checkPlot.getX()) && (getY() == checkPlot.getY()));
}


//	--------------------------------------------------------------------------------
CvPlot* CvUnit::plot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(getX(), getY());
}

//	--------------------------------------------------------------------------------
bool CvUnit::onMap() const
{
	VALIDATE_OBJECT
	return (m_iX != INVALID_PLOT_COORD && m_iY != INVALID_PLOT_COORD && m_iX != -1 && m_iY != -1);
}

//	--------------------------------------------------------------------------------
CvCity* CvUnit::getOriginCity() const
{
	VALIDATE_OBJECT
	if(getOwner() == NO_PLAYER)
		return NULL;

	if (m_iOriginCity == -1 && GET_PLAYER(getOwner()).getCapitalCity() != NULL)
		return GET_PLAYER(getOwner()).getCapitalCity();

	if (m_iOriginCity == -1)
		return NULL;

	return GET_PLAYER(getOwner()).getCity(m_iOriginCity);
}


//	--------------------------------------------------------------------------------
void CvUnit::setOriginCity(int iNewValue)
{
	VALIDATE_OBJECT
	m_iOriginCity = iNewValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getLastMoveTurn() const
{
	VALIDATE_OBJECT
	return m_iLastMoveTurn;
}


//	--------------------------------------------------------------------------------
void CvUnit::setLastMoveTurn(int iNewValue)
{
	VALIDATE_OBJECT
	m_iLastMoveTurn = iNewValue;
	CvAssert(getLastMoveTurn() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCycleOrder() const
{
	VALIDATE_OBJECT
	return m_iCycleOrder;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetCycleOrder(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCycleOrder = iNewValue;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsRecentlyDeployedFromOperation() const
{
	return m_iDeployFromOperationTurn + /*5*/ GD_INT_GET(AI_TACTICAL_MAP_TEMP_ZONE_TURNS) >= GC.getGame().getGameTurn();
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsRecon() const
{
	return GetReconCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetReconCount() const
{
	return m_iReconCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeReconCount(int iChange)
{
	if(iChange != 0)
	{
		m_iReconCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
CvPlot* CvUnit::getReconPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iReconX, m_iReconY);
}


//	--------------------------------------------------------------------------------
void CvUnit::setReconPlot(CvPlot* pNewValue)
{
	VALIDATE_OBJECT
	CvPlot* pOldPlot = NULL;

	pOldPlot = getReconPlot();

	if(pOldPlot != pNewValue)
	{
		if(pOldPlot != NULL)
		{
			if (canChangeVisibility())
				pOldPlot->changeAdjacentSight(getTeam(), reconRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);

			pOldPlot->changeReconCount(-1); // changeAdjacentSight() tests for getReconCount()
		}

		if(pNewValue == NULL)
		{
			m_iReconX = INVALID_PLOT_COORD;
			m_iReconY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iReconX = pNewValue->getX();
			m_iReconY = pNewValue->getY();

			pNewValue->changeReconCount(1); // changeAdjacentSight() tests for getReconCount()
			if (canChangeVisibility())
				pNewValue->changeAdjacentSight(getTeam(), reconRange(), true, getSeeInvisibleType(), getFacingDirection(true), this);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getGameTurnCreated() const
{
	VALIDATE_OBJECT
	return m_iGameTurnCreated;
}


//	--------------------------------------------------------------------------------
void CvUnit::setGameTurnCreated(int iNewValue)
{
	VALIDATE_OBJECT
	m_iGameTurnCreated = iNewValue;
	CvAssert(getGameTurnCreated() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getDamage() const
{
	//for debugging - hook in here and mouse over a unit to get the current tactical zone info
	;

	VALIDATE_OBJECT
	return m_iDamage;
}


//	--------------------------------------------------------------------------------
/**
	Sets the damage.

	@param	iNewValue				New damage value
	@param	ePlayer					The player doing the damage, can be NO_PLAYER.
	@param	fAdditionalTextDelay	The additional text delay. UNUSED
	@param [in]	pAppendText 	If non-null, the text to append to the popup text.

	@return	The difference in the damage.
 */
int CvUnit::setDamage(int iNewValue, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText, bool bDontShow)
{
	VALIDATE_OBJECT

	// Avoid a build warning for unreferenced formal parameter. azum4roll fix this pls
	if (fAdditionalTextDelay > 0.0f)
		fAdditionalTextDelay = 0.0f;

	int iOldValue = getDamage();

	m_iDamage = range(iNewValue, 0, GetMaxHitPoints());
	int iDiff = m_iDamage - iOldValue;

	CvAssertMsg(GetCurrHitPoints() >= 0, "currHitPoints() is expected to be non-negative (invalid Index)");

	if(iOldValue != getDamage())
	{
		if(IsGarrisoned())
		{
			CvCity* pCity = GetGarrisonedCity();
			if(pCity != NULL)
			{
				pCity->updateStrengthValue();
			}
		}

		setInfoBarDirty(true);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		CvInterfacePtr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		const int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;

		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}

		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitSetDamage(pDllUnit.get(), m_iDamage, iOldValue);


		// send the popup text if the player can see this plot
		if(plot()->GetActiveFogOfWarMode() == FOGOFWARMODE_OFF && !bDontShow)
		{
			if(!IsDead())
			{
				CvString text;
				iNewValue = MIN(GetMaxHitPoints(),iNewValue);
				int iDiff = iOldValue - iNewValue;
				if(iNewValue < iOldValue)
				{
					text.Format("[COLOR_GREEN]+%d", iDiff);
				}
				else
				{
					text.Format("[COLOR_RED]%d", iDiff);
				}
				if(pAppendText != NULL)
				{
					text += " ";
					text += *pAppendText;
				}
				text += "[ENDCOLOR]";

				int iX = m_iX;
				int iY = m_iY;

				// Where do we display this text? - put it where the unit is moving TO if it's this unit's turn
				if(GET_PLAYER(getOwner()).isTurnActive())
				{
					const MissionData* pkMissionData = GetHeadMissionData();
					if(pkMissionData != NULL)
					{
						if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
						{
							int iTempX = pkMissionData->iData1;
							int iTempY = pkMissionData->iData2;

							CvPlot* pPlot = GC.getMap().plot(iTempX, iTempY);

							// No enemies in our destination, and it's not a city OR it's a city we own
							if(pPlot->getNumVisibleEnemyDefenders(this) == 0 && (!pPlot->isCity() || pPlot->getPlotCity()->getOwner() == getOwner()))
							{
								iX = iTempX;
								iY = iTempY;
							}
						}
					}
				}

				if (!isSuicide())	// Show the HP lost, except if it is a suicide unit (missile, etc.)
					SHOW_PLOT_POPUP(GC.getMap().plot(iX, iY), getOwner(), text.c_str());
			}
		}
	}

	if(IsDead())
	{
		CvString szMsg;
		CvString lastMission = tacticalMoveNames[m_eTacticalMove];
		CvString unitName = getName(); unitName.Replace(' ', '_');  unitName.Replace('\'', '_'); //no spaces etc
		szMsg.Format("KilledInCombat %s, Domain %d, LastMove %s, KilledBy %d", unitName.c_str(), getDomainType(), lastMission.c_str(), ePlayer);
		GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg);

		//--- finally ----------------------------
		kill(true, ePlayer);
		//----------------------------------------

		if(ePlayer != NO_PLAYER)
		{
			//for barbarian conversion trait
			if(m_eOwner == BARBARIAN_PLAYER && plot()->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
			{
				GET_PLAYER(ePlayer).GetPlayerTraits()->SetDefeatedBarbarianCampGuardType(getUnitType());
			}

			GET_PLAYER(ePlayer).DoUnitKilledCombat(NULL, getOwner(), getUnitType());
		}
	}

	return iDiff;
}

//    -------------------------------------------------------------------------------- 
/**
	Change the damage by a delta. 
	Damage is measured as an integer percent, from 100 (full health) to 0 (dead)

	@param	iChange					Delta added to the current damage.
	@param	ePlayer					The player doing the damage, can be NO_PLAYER
	@param	fAdditionalTextDelay	The additional text delay. UNUSED
	@param [in]	pAppendText 		If non-null, the text to append to the popup.

	@return	the final delta change to the units damage.
 */
int CvUnit::changeDamage(int iChange, PlayerTypes ePlayer, float fAdditionalTextDelay, const CvString* pAppendText)
{
	VALIDATE_OBJECT

	//if (GC.getLogging() && GC.getAILogging())
	//{
	//	CvString info = CvString::format( "%03d;%s;id;0x%08X;owner;%02d;army;0x%08X;%s;arg1;%d;arg2;%d;flags;0x%08X;at;%d;%d\n", 
	//		GC.getGame().getElapsedGameTurns(),this->getNameKey(),this->GetID(),this->getOwner(),this->getArmyID(),"DAMAGE",m_iDamage.get(),m_iDamage+iChange,0,getX(),getY() );
	//	FILogFile* pLog=LOGFILEMGR.GetLog( "unit-missions.csv", FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );
	//	pLog->Msg( info.c_str() );
	//}

	return setDamage((getDamage() + iChange), ePlayer, fAdditionalTextDelay, pAppendText);
}


//	--------------------------------------------------------------------------------
int CvUnit::getMoves() const
{
	VALIDATE_OBJECT
	return m_iMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::setMoves(int iNewValue)
{
	if(m_iMoves != iNewValue)
	{
		m_iMoves = max(0,iNewValue);

		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		gDLL->GameplayUnitShouldDimFlag(pDllUnit.get(), /*bDim*/ getMoves() <= 0);

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		CvInterfacePtr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;

		CvPlot* pPlot = plot();
		if(pPlot && pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeMoves(int iChange)
{
	VALIDATE_OBJECT
	setMoves(getMoves() + iChange);

	if (iChange < 0)
	{
		m_bMovedThisTurn = true;
		SetFortified(false);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::restoreFullMoves()
{
	if (IsGreatGeneral() || (MOD_PROMOTIONS_FLAGSHIP && IsGreatAdmiral()))
	{
		setMoves( GetGreatGeneralStackMovement() );
	}
	else
		setMoves( maxMoves() );

	m_bMovedThisTurn = false;
}

//	--------------------------------------------------------------------------------
// do not call this to end the turn for a unit!
// it will not heal and not receive fortification bonuses!
// use SetTurnProcessed() instead
//	--------------------------------------------------------------------------------
void CvUnit::finishMoves()
{
	VALIDATE_OBJECT
	setMoves(0);
	m_bMovedThisTurn = true;
	SetFortified(false);

	PublishQueuedVisualizationMoves();
}

//	--------------------------------------------------------------------------------
/// Is this unit capable of moving on its own?
bool CvUnit::IsImmobile() const
{
	if(getDomainType() == DOMAIN_IMMOBILE)
	{
		return true;
	}

	return m_bImmobile;
}

/// Is this unit capable of moving on its own?
void CvUnit::SetImmobile(bool bValue)
{
	if(IsImmobile() != bValue)
	{
		m_bImmobile = bValue;
	}
}

//	--------------------------------------------------------------------------------
/// Is this Unit in foreign territory?
bool CvUnit::IsInForeignOwnedTerritory() const
{
	VALIDATE_OBJECT
		
	return plot()->isOwned() && plot()->getTeam() != getTeam();
}

//	--------------------------------------------------------------------------------
int CvUnit::getExperienceTimes100() const
{
	VALIDATE_OBJECT
	return m_iExperienceTimes100;
}


//	--------------------------------------------------------------------------------
void CvUnit::setExperienceTimes100(int iNewValueTimes100, int iMax, bool bDontShow)
{
	VALIDATE_OBJECT

	// Checking limits.h for the values of MAX_INT and MAX_LONG they are the same, so we need to use "long long" and hence MAX_LLONG
	long long lMaxTimes100 = (iMax < 0) ? INT_MAX : (iMax * 100LL);
	int iMaxTimes100 = (lMaxTimes100 > ((long long) INT_MAX)) ? INT_MAX : (int) lMaxTimes100;
	
	if ((getExperienceTimes100() != iNewValueTimes100) && (getExperienceTimes100() < iMaxTimes100))
	{
		int iExperienceChange = (iNewValueTimes100 / 100) - (getExperienceTimes100() / 100);

		m_iExperienceTimes100 = std::min(iMaxTimes100, iNewValueTimes100);
		CvAssert(getExperienceTimes100() >= 0);

		if(getOwner() == GC.getGame().getActivePlayer() && !bDontShow && iExperienceChange > 0)
		{
			// Don't show XP for unit that's about to bite the dust
			if(!IsDead())
			{
				Localization::String localizedText = Localization::Lookup("TXT_KEY_EXPERIENCE_POPUP");
				localizedText << iExperienceChange;

				int iX = m_iX;
				int iY = m_iY;

				// Where do we display this text?
				const MissionData* pkMissionData = GetHeadMissionData();
				if(pkMissionData != NULL)
				{
					if(pkMissionData->eMissionType == CvTypes::getMISSION_MOVE_TO())
					{
						int iTempX = pkMissionData->iData1;
						int iTempY = pkMissionData->iData2;

						if(GC.getMap().plot(iTempX, iTempY)->getNumVisibleEnemyDefenders(this) == 0)
						{
							iX = iTempX;
							iY = iTempY;
						}
					}
				}

				//possibly iX and iY are invalid if the unit is not initialized yet, but the macro catches this
				SHOW_PLOT_POPUP(GC.getMap().plot(iX, iY), getOwner(), localizedText.toUTF8());

				if(IsSelected())
				{
					DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
				}
			}
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExperienceTimes100(int iChangeTimes100, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal, bool bFromHuman)
{
	VALIDATE_OBJECT
	// Barbs don't get XP or Promotions
	if (isBarbarian() && iChangeTimes100 > 0)
	{
		return;
	}

	if (MOD_BALANCE_CORE_SCALING_XP && !bFromCombat)
	{
		iChangeTimes100 *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iChangeTimes100 /= 100;
	}

	// Checking limits.h for the values of MAX_INT and MAX_LONG they are the same, so we need to use "long long" and hence MAX_LLONG
	long long lMaxTimes100 = (iMax < 0) ? INT_MAX : (iMax * 100LL);
	int iMaxTimes100 = (lMaxTimes100 > ((long long)INT_MAX)) ? INT_MAX : (int)lMaxTimes100;
	
	int iUnitExperienceTimes100 = iChangeTimes100;

	PromotionTypes eNewPromotion = NO_PROMOTION;

	if(bFromCombat)
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwner());

		// Promotion that changes after combat?
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
			if(pkPromotionInfo)
			{
				if (pkPromotionInfo->HasPostCombatPromotions() && m_Promotions.HasPromotion(eLoopPromotion))
				{
					eNewPromotion = m_Promotions.ChangePromotionAfterCombat(eLoopPromotion,this);
					setHasPromotion(eLoopPromotion, false);

					if (eNewPromotion != NO_PROMOTION)
					{
						setHasPromotion(eNewPromotion, true);

#if defined(MOD_EVENTS_UNIT_UPGRADES)
						if (MOD_EVENTS_UNIT_UPGRADES) {
							GAMEEVENTINVOKE_HOOK(GAMEEVENT_UnitPromoted, getOwner(), GetID(), eNewPromotion);
						}
#endif
						if (kPlayer.GetID() == GC.getGame().getActivePlayer())
						{
							CvPromotionEntry* pkNewPromotionInfo = GC.getPromotionInfo(eNewPromotion);
							Localization::String localizedText = Localization::Lookup(pkNewPromotionInfo->GetDescriptionKey());
							SHOW_PLOT_POPUP(plot(), getOwner(), localizedText.toUTF8());
						}
					}
				}
			}
		}

		// Player Great General/Admiral mod
		int iCombatExperienceMod = 0;
		if (getDomainType() == DOMAIN_SEA)
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatAdmiralRateModifier();
		}
		else
		{
			iCombatExperienceMod = 100 + kPlayer.getGreatGeneralRateModifier();
		}

		// Unit XP mod
		int iPlayerBonusXpTimes100 = (iChangeTimes100 * kPlayer.getExpModifier()) / 100;
		iUnitExperienceTimes100 += iPlayerBonusXpTimes100;
		if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
		{
			const std::vector<ResourceTypes>& vStrategicMonopolies = GET_PLAYER(getOwner()).GetStrategicMonopolies();
			for (size_t iResourceLoop = 0; iResourceLoop < vStrategicMonopolies.size(); iResourceLoop++)
			{
				ResourceTypes eResourceLoop = vStrategicMonopolies[iResourceLoop];
				CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
				if (pInfo && pInfo->getMonopolyXPBonus() > 0)
				{
					iUnitExperienceTimes100 += (pInfo->getMonopolyXPBonus() * 100);
				}
			}
		}

		// Great General & Unit XP mod in borders
		if (bInBorders)
		{
			// In-borders GG mod
			if (getDomainType() == DOMAIN_LAND)
			{
				iCombatExperienceMod += kPlayer.getDomesticGreatGeneralRateModifier();
			}

			int iBorderBonusXpTimes100 = (iChangeTimes100 * kPlayer.getExpInBorderModifier()) / 100;
			iUnitExperienceTimes100 += iBorderBonusXpTimes100;
		}

		if(bUpdateGlobal)
		{
			// Add Unit GG mod
			if (getDomainType() == DOMAIN_LAND) 
			{
				iCombatExperienceMod += getGreatGeneralModifier();
			}

			if(iMax < 0)
			{
				if(getDomainType() == DOMAIN_SEA)
				{
					kPlayer.changeNavalCombatExperienceTimes100((iChangeTimes100 * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
				}
				else
				{
					kPlayer.changeCombatExperienceTimes100((iChangeTimes100 * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
				}
			}
			else
			{
				int iModdedChangeTimes100 = min(iMaxTimes100 - m_iExperienceTimes100, iChangeTimes100);
				if (iModdedChangeTimes100 > 0)
				{
					if(getDomainType() == DOMAIN_SEA)
					{
						kPlayer.changeNavalCombatExperienceTimes100((iModdedChangeTimes100 * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
					}
					else
					{
						kPlayer.changeCombatExperienceTimes100((iModdedChangeTimes100 * iCombatExperienceMod) / 100, (MOD_GLOBAL_LOCAL_GENERALS ? this : NULL));
					}
				}
			}
		}

		int iExperiencePercent = getExperiencePercent();
		if (bFromHuman && GET_PLAYER(getOwner()).isMajorCiv())
		{
			iExperiencePercent += GET_PLAYER(getOwner()).getHandicapInfo().getFreeXPPercentVSHuman();
			iExperiencePercent += GET_PLAYER(getOwner()).isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIFreeXPPercentVSHuman();
		}

		if (iExperiencePercent != 0)
		{
			int iUnitBonusXpTimes100 = (iUnitExperienceTimes100 * iExperiencePercent) / 100;
			iUnitExperienceTimes100 += iUnitBonusXpTimes100;
		}

		int iRealExperienceTimes100 = min(iMaxTimes100 - m_iExperienceTimes100, iUnitExperienceTimes100);
		CvCity* pOriginCity = getOriginCity();
		if (pOriginCity)
		{
			GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE, false, NO_GREATPERSON, NO_BUILDING, iRealExperienceTimes100, false, NO_PLAYER, NULL, false, pOriginCity, getDomainType() == DOMAIN_SEA, true, false, NO_YIELD, this);
		}
		else
		{
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			if (pCapital)
			{
				GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_COMBAT_EXPERIENCE, false, NO_GREATPERSON, NO_BUILDING, iRealExperienceTimes100, false, NO_PLAYER, NULL, false, pCapital, getDomainType() == DOMAIN_SEA, true, false, NO_YIELD, this);
			}
		}
	}

	setExperienceTimes100((getExperienceTimes100() + iUnitExperienceTimes100), iMax);
}


//	--------------------------------------------------------------------------------
int CvUnit::getLevel() const
{
	VALIDATE_OBJECT
	return m_iLevel;
}


//	--------------------------------------------------------------------------------
void CvUnit::setLevel(int iNewValue)
{
	VALIDATE_OBJECT
	if(getLevel() != iNewValue)
	{
		m_iLevel = iNewValue;
		CvAssert(getLevel() >= 0);

		if(getLevel() > GET_PLAYER(getOwner()).getHighestUnitLevel())
		{
			GET_PLAYER(getOwner()).setHighestUnitLevel(getLevel());
		}

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::changeLevel(int iChange)
{
	VALIDATE_OBJECT
	setLevel(getLevel() + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getCargo() const
{
	VALIDATE_OBJECT
	return m_iCargo;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCargo(int iChange)
{
	VALIDATE_OBJECT
	m_iCargo += iChange;
	CvAssert(getCargo() >= 0);
}


//	--------------------------------------------------------------------------------
CvPlot* CvUnit::getAttackPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iAttackPlotX, m_iAttackPlotY);
}


//	--------------------------------------------------------------------------------
void CvUnit::setAttackPlot(const CvPlot* pNewValue, bool bAirCombat)
{
	VALIDATE_OBJECT
	if(getAttackPlot() != pNewValue)
	{
		if(pNewValue != NULL)
		{
			m_iAttackPlotX = pNewValue->getX();
			m_iAttackPlotY = pNewValue->getY();
		}
		else
		{
			m_iAttackPlotX = INVALID_PLOT_COORD;
			m_iAttackPlotY = INVALID_PLOT_COORD;
		}
	}

	m_bAirCombat = bAirCombat;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isAirCombat() const
{
	VALIDATE_OBJECT
	return m_bAirCombat;
}


//	--------------------------------------------------------------------------------
int CvUnit::getCombatTimer() const
{
	VALIDATE_OBJECT
	return m_iCombatTimer;
}


//	--------------------------------------------------------------------------------
void CvUnit::setCombatTimer(int iNewValue)
{
	VALIDATE_OBJECT
	m_iCombatTimer = iNewValue;
	CvAssert(getCombatTimer() >= 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatTimer(int iChange)
{
	VALIDATE_OBJECT
	setCombatTimer(getCombatTimer() + iChange);
}

bool CvUnit::CanGarrison() const
{
	return GetMapLayer()==DEFAULT_UNIT_MAP_LAYER && getDomainType()!=DOMAIN_AIR && IsCanDefend() && !isDelayedDeath();
}

int CvUnit::GetMapLayer() const
{
	return m_iMapLayer;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGarrisoned(void) const
{
	VALIDATE_OBJECT

	return (m_iGarrisonCityID != -1);
}

void CvUnit::SetGarrisonedCity(int iCityID)
{
	m_iGarrisonCityID = iCityID;
}

//	--------------------------------------------------------------------------------
CvCity* CvUnit::GetGarrisonedCity() const
{
	if(IsGarrisoned())
		return GET_PLAYER(getOwner()).getCity(m_iGarrisonCityID);

	return NULL;
}

//	--------------------------------------------------------------------------------
void CvUnit::triggerFortifyAnimation(bool bState)
{
	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	gDLL->GameplayUnitFortify(pDllUnit.get(), bState);

	setInfoBarDirty(true);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsFortified() const
{
	VALIDATE_OBJECT
	return m_bFortified;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetFortified(bool bValue)
{
	if(!IsEverFortifyable())
		return;

	if (m_bFortified != bValue)
	{
		triggerFortifyAnimation(bValue);
		m_bFortified = bValue;
	}
}

bool CvUnit::getHasWithdrawnThisTurn() const
{
	VALIDATE_OBJECT
		return m_bHasWithdrawnThisTurn;
}

void CvUnit::setHasWithdrawnThisTurn(bool bNewValue)
{
	VALIDATE_OBJECT
		m_bHasWithdrawnThisTurn = bNewValue;
}

int CvUnit::DoAdjacentPlotDamage(CvPlot* pWhere, int iValue, const char* chTextKey)
{
	if (iValue < 1 || pWhere == NULL)
		return 0;

	int iCount = 0;
	for (int i=RING0_PLOTS; i<RING1_PLOTS; i++)
	{
		CvPlot* pSplashPlot = iterateRingPlots(pWhere, i);
		if (!pSplashPlot)
			continue;

		//splash damage only for range-attackable plots around the target
		//if the target is the current unit plot (for roman pilum / overrun promo) check is not needed
		if (!at(pWhere->getX(),pWhere->getY()) && !canEverRangeStrikeAt(pSplashPlot->getX(), pSplashPlot->getY(), plot(), false))
			continue;

		for (int iJ = 0; iJ < pSplashPlot->getNumUnits(); iJ++)
		{
			CvUnit* pEnemyUnit = pSplashPlot->getUnitByIndex(iJ);
			//logically we should damage non-enemy units as well? but that is too complex to consider ... 
			if (pEnemyUnit != NULL && pEnemyUnit->isEnemy(getTeam()) && !pEnemyUnit->isTrade())
			{
				//no splash damage in cities/forts
				if (pSplashPlot->isFortification(pEnemyUnit->getTeam()))
					continue;

				if (iValue + pEnemyUnit->getDamage() >= pEnemyUnit->GetMaxHitPoints())
				{
					// Earn bonuses for kills?
					CvPlayer& kAttackingPlayer = GET_PLAYER(getOwner());
					kAttackingPlayer.DoYieldsFromKill(this, pEnemyUnit);
				}

				if (chTextKey)
				{
					CvString strAppendText = GetLocalizedText(chTextKey);
					pEnemyUnit->changeDamage(iValue, getOwner(), 0.0, &strAppendText);
				}
				else
					pEnemyUnit->changeDamage(iValue, getOwner(), 0.0);

				iCount++;
			}
		}
	}
	
	return iCount;
}

//	--------------------------------------------------------------------------------
int CvUnit::getBlitzCount() const
{
	VALIDATE_OBJECT
	return m_iBlitzCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isBlitz() const
{
	VALIDATE_OBJECT
	if(GetRange() > 0)
	{
		return false;
	}
	return (getBlitzCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeBlitzCount(int iChange)
{
	VALIDATE_OBJECT
	m_iBlitzCount = (m_iBlitzCount + iChange);
	CvAssert(getBlitzCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAmphibCount() const
{
	VALIDATE_OBJECT
	return m_iAmphibCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAmphibious() const
{
	VALIDATE_OBJECT
	return (getAmphibCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAmphibCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAmphibCount = (m_iAmphibCount + iChange);
	CvAssert(getAmphibCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getRiverCrossingNoPenaltyCount() const
{
	VALIDATE_OBJECT
	return m_iRiverCrossingNoPenaltyCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isRiverCrossingNoPenalty() const
{
	VALIDATE_OBJECT
	return (getRiverCrossingNoPenaltyCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeRiverCrossingNoPenaltyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRiverCrossingNoPenaltyCount = (m_iRiverCrossingNoPenaltyCount + iChange);
	CvAssert(getRiverCrossingNoPenaltyCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getEnemyRouteCount() const
{
	VALIDATE_OBJECT
	return m_iEnemyRouteCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isEnemyRoute() const
{
	VALIDATE_OBJECT
	return (getEnemyRouteCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyRouteCount(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyRouteCount = (m_iEnemyRouteCount + iChange);
	CvAssert(getEnemyRouteCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAlwaysHealCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHealCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAlwaysHeal() const
{
	VALIDATE_OBJECT
	return (getAlwaysHealCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAlwaysHealCount(int iChange)
{
	VALIDATE_OBJECT
	m_iAlwaysHealCount = (m_iAlwaysHealCount + iChange);
	CvAssert(getAlwaysHealCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getHealOutsideFriendlyCount() const
{
	VALIDATE_OBJECT
	return m_iHealOutsideFriendlyCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHealOutsideFriendly() const
{
	VALIDATE_OBJECT
	return (getHealOutsideFriendlyCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeHealOutsideFriendlyCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHealOutsideFriendlyCount = (m_iHealOutsideFriendlyCount + iChange);
	CvAssert(getHealOutsideFriendlyCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getHillsDoubleMoveCount() const
{
	VALIDATE_OBJECT
	return m_iHillsDoubleMoveCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isHillsDoubleMove() const
{
	VALIDATE_OBJECT
	return (getHillsDoubleMoveCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeHillsDoubleMoveCount(int iChange)
{
	VALIDATE_OBJECT
	m_iHillsDoubleMoveCount = (m_iHillsDoubleMoveCount + iChange);
	CvAssert(getHillsDoubleMoveCount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getRiverDoubleMoveCount() const
{
	VALIDATE_OBJECT
		return m_iRiverDoubleMoveCount;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isRiverDoubleMove() const
{
	VALIDATE_OBJECT
		return (getRiverDoubleMoveCount() > 0);
}


//	--------------------------------------------------------------------------------
void CvUnit::changeRiverDoubleMoveCount(int iChange)
{
	VALIDATE_OBJECT
	m_iRiverDoubleMoveCount = (m_iRiverDoubleMoveCount + iChange);
	CvAssert(getRiverDoubleMoveCount() >= 0);
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::getMountainsDoubleMoveCount() const
{
	VALIDATE_OBJECT
	return m_iMountainsDoubleMoveCount;
}
//	--------------------------------------------------------------------------------
bool CvUnit::isMountainsDoubleMove() const
{
	VALIDATE_OBJECT
	return (getMountainsDoubleMoveCount() > 0);
}
//	--------------------------------------------------------------------------------
void CvUnit::changeMountainsDoubleMoveCount(int iChange)
{
	VALIDATE_OBJECT
	m_iMountainsDoubleMoveCount = (m_iMountainsDoubleMoveCount + iChange);
	CvAssert(getMountainsDoubleMoveCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getEmbarkFlatCostCount() const
{
	VALIDATE_OBJECT
		return m_iEmbarkFlatCostCount;
}
//	--------------------------------------------------------------------------------
bool CvUnit::isEmbarkFlatCost() const
{
	VALIDATE_OBJECT
		return (getEmbarkFlatCostCount() > 0);
}
//	--------------------------------------------------------------------------------
void CvUnit::changeEmbarkFlatCostCount(int iChange)
{
	VALIDATE_OBJECT
		m_iEmbarkFlatCostCount = (m_iEmbarkFlatCostCount + iChange);
	CvAssert(getEmbarkFlatCostCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getDisembarkFlatCostCount() const
{
	VALIDATE_OBJECT
		return m_iDisembarkFlatCostCount;
}
//	--------------------------------------------------------------------------------
bool CvUnit::isDisembarkFlatCost() const
{
	VALIDATE_OBJECT
		return (getDisembarkFlatCostCount() > 0);
}
//	--------------------------------------------------------------------------------
void CvUnit::changeDisembarkFlatCostCount(int iChange)
{
	VALIDATE_OBJECT
		m_iDisembarkFlatCostCount = (m_iDisembarkFlatCostCount + iChange);
	CvAssert(getDisembarkFlatCostCount() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getAOEDamageOnKill() const
{
	VALIDATE_OBJECT
	return m_iAOEDamageOnKill;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeAOEDamageOnKill(int iChange)
{
	VALIDATE_OBJECT
	m_iAOEDamageOnKill = (m_iAOEDamageOnKill + iChange);
	CvAssert(getAOEDamageOnKill() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getAOEDamageOnPillage() const
{
	VALIDATE_OBJECT
		return m_iAOEDamageOnPillage;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeAOEDamageOnPillage(int iChange)
{
	VALIDATE_OBJECT
		m_iAOEDamageOnPillage = (m_iAOEDamageOnPillage + iChange);
	CvAssert(getAOEDamageOnPillage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getAoEDamageOnMove() const
{
	VALIDATE_OBJECT
	return m_iAoEDamageOnMove;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeAoEDamageOnMove(int iChange)
{
	VALIDATE_OBJECT
	m_iAoEDamageOnMove = (m_iAoEDamageOnMove + iChange);
	CvAssert(getAoEDamageOnMove() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getPartialHealOnPillage() const
{
	VALIDATE_OBJECT
		return m_iPartialHealOnPillage;
}
//	--------------------------------------------------------------------------------
void CvUnit::changePartialHealOnPillage(int iChange)
{
	VALIDATE_OBJECT
		m_iPartialHealOnPillage = (m_iPartialHealOnPillage + iChange);
	CvAssert(getPartialHealOnPillage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getSplashDamage() const
{
	VALIDATE_OBJECT
		return m_iSplashDamage;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeSplashDamage(int iChange)
{
	VALIDATE_OBJECT
		m_iSplashDamage = (m_iSplashDamage + iChange);
	CvAssert(getSplashDamage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getMultiAttackBonus() const
{
	VALIDATE_OBJECT
		return m_iMultiAttackBonus;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeMultiAttackBonus(int iChange)
{
	VALIDATE_OBJECT
		m_iMultiAttackBonus = (m_iMultiAttackBonus + iChange);
	CvAssert(getMultiAttackBonus() >= 0);
}
//	--------------------------------------------------------------------------------
int CvUnit::getLandAirDefenseValue() const
{
	VALIDATE_OBJECT
		return m_iLandAirDefenseValue;
}
//	--------------------------------------------------------------------------------
void CvUnit::changeLandAirDefenseValue(int iChange)
{
	VALIDATE_OBJECT
		m_iLandAirDefenseValue = (m_iLandAirDefenseValue + iChange);
	CvAssert(getLandAirDefenseValue() >= 0);
}




#endif

//	--------------------------------------------------------------------------------
int CvUnit::getExtraVisibilityRange() const
{
	VALIDATE_OBJECT
	return m_iExtraVisibilityRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraVisibilityRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		CvPlot* pkPlot = plot();
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);
		m_iExtraVisibilityRange = (m_iExtraVisibilityRange + iChange);
		if (canChangeVisibility())
			pkPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true), this);
	}
}


#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
//	--------------------------------------------------------------------------------
int CvUnit::getExtraReconRange() const
{
	VALIDATE_OBJECT
	return m_iExtraReconRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraReconRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		CvPlot* pkPlot = plot();
		pkPlot->changeAdjacentSight(getTeam(), reconRange(), false, getSeeInvisibleType(), getFacingDirection(true), this);
		m_iExtraReconRange = (m_iExtraReconRange + iChange);
		pkPlot->changeAdjacentSight(getTeam(), reconRange(), true, getSeeInvisibleType(), getFacingDirection(true), this);
	}
}
#endif


//	--------------------------------------------------------------------------------
int CvUnit::getExtraMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoves += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraNavalMoves() const
{
	VALIDATE_OBJECT
	return m_iExtraNavalMoves;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraNavalMoves(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNavalMoves += iChange;
	CvAssert(getExtraNavalMoves() >= 0);
}



//	--------------------------------------------------------------------------------
int CvUnit::getExtraMoveDiscount() const
{
	VALIDATE_OBJECT
	return m_iExtraMoveDiscount;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraMoveDiscount(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraMoveDiscount = (m_iExtraMoveDiscount + iChange);
	CvAssert(getExtraMoveDiscount() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRange() const
{
	VALIDATE_OBJECT
	return m_iExtraRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRange(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraRange += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getInterceptChance() const
{
	VALIDATE_OBJECT
	return m_iInterceptChance;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeInterceptChance(int iChange)
{
	VALIDATE_OBJECT
	m_iInterceptChance += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraEvasion() const
{
	VALIDATE_OBJECT
	return m_iExtraEvasion;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraEvasion(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEvasion += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraWithdrawal() const
{
	VALIDATE_OBJECT
	return m_iExtraWithdrawal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraWithdrawal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraWithdrawal = (m_iExtraWithdrawal + iChange);
	CvAssert(getExtraWithdrawal() >= 0);
}

//	--------------------------------------------------------------------------------
/// Returns a vector containing the plagues that a unit can attempt to inflict
vector<int> CvUnit::GetInflictedPlagueIDs() const
{
	vector<int> result;

	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo && pkPromotionInfo->GetPlagueChance() > 0)
		{
			PromotionTypes eInflictedPlague = (PromotionTypes)pkPromotionInfo->GetPlaguePromotion();
			if (eInflictedPlague != NO_PROMOTION && isHasPromotion(ePromotion))
			{
				CvPromotionEntry* pkPlaguePromotionInfo = GC.getPromotionInfo(eInflictedPlague);
				if (pkPlaguePromotionInfo)
				{
					int iPlagueID = pkPlaguePromotionInfo->GetPlagueID();
					if (std::find(result.begin(), result.end(), iPlagueID) == result.end())
						result.push_back(iPlagueID);
				}
			}
		}
	}

	return result;
}

/// What specific promotion can be inflicted by this unit with iPlagueID?
PromotionTypes CvUnit::GetInflictedPlague(int iPlagueID, int& iPlagueChance) const
{
	int iHighestPriority = -1;
	PromotionTypes eHighestPlague = NO_PROMOTION;

	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo && pkPromotionInfo->GetPlagueChance() > 0)
		{
			PromotionTypes eInflictedPlague = (PromotionTypes)pkPromotionInfo->GetPlaguePromotion();
			if (eInflictedPlague != NO_PROMOTION && isHasPromotion(ePromotion))
			{
				CvPromotionEntry* pkPlaguePromotionInfo = GC.getPromotionInfo(eInflictedPlague);
				if (pkPlaguePromotionInfo)
				{
					int iPromotionPlagueID = pkPlaguePromotionInfo->GetPlagueID();
					if (iPlagueID == iPromotionPlagueID)
					{
						int iPlaguePriority = pkPlaguePromotionInfo->GetPlaguePriority();
						if (iPlaguePriority > iHighestPriority)
						{
							iHighestPriority = iPlaguePriority;
							eHighestPlague = eInflictedPlague;
							iPlagueChance = pkPromotionInfo->GetPlagueChance();
						}
					}
				}
			}
		}
	}

	return eHighestPlague;
}

/// Does this unit have a plague?
/// iPlagueID = With a specific plague ID only? Defaults to -1 (any plague).
/// iMinimumPriority = With a specific priority or higher only? Defaults to -1 (any priority). Requires iPlagueID to be specified.
bool CvUnit::HasPlague(int iPlagueID, int iMinimumPriority) const
{
	if (iPlagueID == -1)
		iMinimumPriority = -1;

	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo)
		{
			int iPromotionPlagueID = pkPromotionInfo->GetPlagueID();
			if (iPromotionPlagueID != -1 && isHasPromotion(ePromotion))
			{
				if (iPlagueID == -1 || iPlagueID == iPromotionPlagueID)
				{
					if (iMinimumPriority == -1 || pkPromotionInfo->GetPlaguePriority() >= iMinimumPriority)
						return true;
				}
			}
		}
	}

	return false;
}

/// Removes plague promotions from a unit
/// iPlagueID = With a specific Plague ID only. Defaults to -1 (any plague).
/// iHigherPriority = Below a specific priority only. Defaults to -1 (any priority).
void CvUnit::RemovePlague(int iPlagueID, int iHigherPriority)
{
	if (iPlagueID == -1)
		iHigherPriority = -1;

	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo)
		{
			int iPromotionPlagueID = pkPromotionInfo->GetPlagueID();
			if (iPromotionPlagueID != -1)
			{
				if (iPlagueID == -1 || iPlagueID == iPromotionPlagueID)
				{
					if (iHigherPriority == -1 || pkPromotionInfo->GetPlaguePriority() < iHigherPriority)
						setHasPromotion(ePromotion, false);
				}
			}
		}
	}
}

/// Is this unit immune to a plague?
/// iPlagueID = With a specific plague ID only? Defaults to -1 (any plague).
bool CvUnit::ImmuneToPlague(int iPlagueID) const
{
	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if (pkPromotionInfo)
		{
			int iPlagueImmunityID = pkPromotionInfo->GetPlagueIDImmunity();
			if (iPlagueImmunityID != -1 && isHasPromotion(ePromotion))
			{
				if (iPlagueID == -1 || iPlagueID == iPlagueImmunityID)
					return true;
			}
		}
	}

	return false;
}

bool CvUnit::CanPlague(CvUnit* pOtherUnit) const
{
	if (pOtherUnit == NULL || pOtherUnit->isDelayedDeath())
		return false;

	if (getDomainType() != pOtherUnit->getDomainType())
		return false;

	return true;
}

void CvUnit::setContractUnit(ContractTypes eContract)
{
	if(eContract != m_eUnitContract)
	{
		m_eUnitContract = eContract;
	}
}
bool CvUnit::isContractUnit() const
{
	return (m_eUnitContract != NO_CONTRACT);
}
ContractTypes CvUnit::getContract() const
{
	return m_eUnitContract;
}

bool CvUnit::IsNoMaintenance() const
{
	VALIDATE_OBJECT
	return m_bIsNoMaintenance;
}
void CvUnit::SetNoMaintenance(bool bValue)
{
	VALIDATE_OBJECT
	m_bIsNoMaintenance = bValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraEnemyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraEnemyHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraEnemyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraNeutralHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraNeutralHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraNeutralHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraFriendlyHeal() const
{
	VALIDATE_OBJECT
	return m_iExtraFriendlyHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	CvAssert(getExtraFriendlyHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getSameTileHeal() const
{
	VALIDATE_OBJECT
	return m_iSameTileHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeSameTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iSameTileHeal = (m_iSameTileHeal + iChange);
	CvAssert(getSameTileHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getAdjacentTileHeal() const
{
	VALIDATE_OBJECT
	return m_iAdjacentTileHeal;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAdjacentTileHeal(int iChange)
{
	VALIDATE_OBJECT
	m_iAdjacentTileHeal = (m_iAdjacentTileHeal + iChange);
	CvAssert(getAdjacentTileHeal() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getEnemyDamageChance() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamageChance;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamageChance = (m_iEnemyDamageChance + iChange);
	CvAssert(getEnemyDamageChance() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getNeutralDamageChance() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamageChance;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNeutralDamageChance(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamageChance = (m_iNeutralDamageChance + iChange);
	CvAssert(getNeutralDamageChance() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getEnemyDamage() const
{
	VALIDATE_OBJECT
	return m_iEnemyDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeEnemyDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iEnemyDamage = (m_iEnemyDamage + iChange);
}

//	--------------------------------------------------------------------------------
int CvUnit::getNeutralDamage() const
{
	VALIDATE_OBJECT
	return m_iNeutralDamage;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNeutralDamage(int iChange)
{
	VALIDATE_OBJECT
	m_iNeutralDamage = (m_iNeutralDamage + iChange);
	CvAssert(getNeutralDamage() >= 0);
}

//	--------------------------------------------------------------------------------
int CvUnit::getNearbyEnemyCombatMod() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNearbyEnemyCombatMod(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatMod = (m_iNearbyEnemyCombatMod + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getNearbyEnemyCombatRange() const
{
	VALIDATE_OBJECT
	return m_iNearbyEnemyCombatRange;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeNearbyEnemyCombatRange(int iChange)
{
	VALIDATE_OBJECT
	m_iNearbyEnemyCombatRange = (m_iNearbyEnemyCombatRange + iChange);
	CvAssert(getNearbyEnemyCombatRange() >= 0);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCombatPercent() const
{
	VALIDATE_OBJECT
	int iRtnValue = m_iExtraCombatPercent;

	// Add bonus from difficulty level
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	if (kPlayer.isMajorCiv())
	{
		iRtnValue += kPlayer.getHandicapInfo().getCombatBonus();
		iRtnValue += kPlayer.isHuman() ? 0 : GC.getGame().getHandicapInfo().getAICombatBonus();
	}
	else if (kPlayer.isMinorCiv())
	{
		iRtnValue += GC.getGame().getHandicapInfo().getCityStateCombatBonus();
	}

	return iRtnValue;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCombatPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCombatPercent = (m_iExtraCombatPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCityAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCityAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityAttackPercent = (m_iExtraCityAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraCityDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraCityDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraCityDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraCityDefensePercent = (m_iExtraCityDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRangedDefenseModifier() const
{
	VALIDATE_OBJECT
	return m_iExtraRangedDefenseModifier;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRangedDefenseModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRangedDefenseModifier = (m_iExtraRangedDefenseModifier + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraHillsAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraHillsAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsAttackPercent = (m_iExtraHillsAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraHillsDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraHillsDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraHillsDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraHillsDefensePercent = (m_iExtraHillsDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenAttackPercent = (m_iExtraOpenAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenRangedAttackMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenRangedAttackMod = (m_iExtraOpenRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughAttackPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughAttackPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughAttackPercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughAttackPercent = (m_iExtraRoughAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughRangedAttackMod() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughRangedAttackMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughRangedAttackMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughRangedAttackMod = (m_iExtraRoughRangedAttackMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackFortifiedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackFortifiedMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackFortifiedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackFortifiedMod = (m_iExtraAttackFortifiedMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackWoundedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackWoundedMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackWoundedMod(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraAttackWoundedMod = (m_iExtraAttackWoundedMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackFullyHealedMod() const
{
	VALIDATE_OBJECT
	return m_iExtraFullyHealedMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackFullyHealedMod(int iChange)
{
	VALIDATE_OBJECT
	if (iChange != 0)
	{
		m_iExtraFullyHealedMod = (m_iExtraFullyHealedMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackAboveHealthMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackAboveHealthMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackAboveHealthMod(int iChange)
{
	VALIDATE_OBJECT
	if (iChange != 0)
	{
		m_iExtraAttackAboveHealthMod = (m_iExtraAttackAboveHealthMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraAttackBelowHealthMod() const
{
	VALIDATE_OBJECT
	return m_iExtraAttackBelowHealthMod;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttackBelowHealthMod(int iChange)
{
	VALIDATE_OBJECT
	if (iChange != 0)
	{
		m_iExtraAttackBelowHealthMod = (m_iExtraAttackBelowHealthMod + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
//	Can this ranged unit benefit from flanking?
bool CvUnit::IsRangedFlankAttack() const
{
	return m_iRangedFlankAttack > 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::ChangeRangedFlankAttackCount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iRangedFlankAttack = (m_iRangedFlankAttack + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
//	How many units does this unit count as when supporting a flank?
int CvUnit::GetFlankPower() const
{
	VALIDATE_OBJECT
	return m_iFlankPower;
}


//	--------------------------------------------------------------------------------
void CvUnit::ChangeFlankPower(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFlankPower = (m_iFlankPower + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetFlankAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFlankAttackModifier;
}


//	--------------------------------------------------------------------------------
void CvUnit::ChangeFlankAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFlankAttackModifier = (m_iFlankAttackModifier + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraOpenDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraOpenDefensePercent = (m_iExtraOpenDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraOpenFromPercent() const
{
	VALIDATE_OBJECT
		return m_iExtraOpenFromPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraOpenFromPercent(int iChange)
{
	VALIDATE_OBJECT
		if (iChange != 0)
		{
			m_iExtraOpenFromPercent = (m_iExtraOpenFromPercent + iChange);

			setInfoBarDirty(true);
		}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughDefensePercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughDefensePercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughDefensePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExtraRoughDefensePercent = (m_iExtraRoughDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraRoughFromPercent() const
{
	VALIDATE_OBJECT
	return m_iExtraRoughFromPercent;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraRoughFromPercent(int iChange)
{
	VALIDATE_OBJECT
	if (iChange != 0)
	{
		m_iExtraRoughFromPercent = (m_iExtraRoughFromPercent + iChange);

		setInfoBarDirty(true);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getNumAttacks() const
{
	VALIDATE_OBJECT
	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return m_iNumAttacks + GetGiveExtraAttacksToUnit();
	else
		return m_iNumAttacks;
}

int CvUnit::getNumAttacksMadeThisTurn() const
{
	VALIDATE_OBJECT
	return m_iAttacksMade;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExtraAttacks(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iNumAttacks += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::GetNearbyCityBonusCombatMod(const CvPlot* pAtPlot) const
{
	VALIDATE_OBJECT
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}

	//this is not 100% accurate in case of ties ... but good enough
	int iRange = GetNearbyUnitPromotionsRange();
	if (GC.getGame().GetClosestCityDistanceInPlots(pAtPlot) > iRange)
		return 0;

	if (getNearbyCityCombatMod() != 0)
	{
		return getNearbyCityCombatMod();
	}
	else if (getNearbyFriendlyCityCombatMod() != 0)
	{
		CvCity* pCity = GC.getGame().GetClosestCityByPlots(pAtPlot);
		if (pCity && pCity->plot()->IsFriendlyTerritory(getOwner()))
			return getNearbyFriendlyCityCombatMod();
	}
	else if (getNearbyEnemyCityCombatMod() != 0)
	{
		CvCity* pCity = GC.getGame().GetClosestCityByPlots(pAtPlot);
		if (pCity && isEnemy(pCity->getTeam()))
			return getNearbyEnemyCityCombatMod();
	}

	return 0;
}


//	--------------------------------------------------------------------------------
// Golden Age? And have a general near us that gives us additional Exp?
int CvUnit::GetGoldenAgeGeneralExpPercent() const
{
	VALIDATE_OBJECT
	int iGreatGeneralRange = /*2*/ GD_INT_GET(GREAT_GENERAL_RANGE);
	int iExperience = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPositiveUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->GetGGGAXPPercent() != 0 && GET_PLAYER(pUnit->getOwner()).isGoldenAge() && pUnit != this)
		{
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iGreatGeneralRange)
			{
				if (pUnit->getDomainType() == getDomainType())
				{
					iExperience = pUnit->GetGGGAXPPercent();
				}
			}
		}
	}
	return iExperience;
}
// NearbyUnit Promotion gives additional XP Percent to a Unit?
int CvUnit::GetGiveExperiencePercentToUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iExperience = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveExperiencePercent() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iExperience = pUnit->getGiveExperiencePercent();
					}
				}
			}
		}
	}
	return iExperience;
}
int CvUnit::GetGiveCombatModToUnit(const CvPlot* pAtPlot) const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iMod = 0;
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveCombatMod() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(pAtPlot->getX(), pAtPlot->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iMod = pUnit->getGiveCombatMod();
					}
				}
			}
		}
	}
	return iMod;
}
int CvUnit::GetGiveDefenseModToUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iMod = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveDefenseMod() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iMod = pUnit->getGiveDefenseMod();
					}
				}
			}
		}
	}
	return iMod;
}
int CvUnit::GetHealEnemyTerritoryFromNearbyUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iHeal = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getNearbyHealEnemyTerritory() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					iHeal = pUnit->getNearbyHealEnemyTerritory();
				}
			}
		}
	}
	return iHeal;
}
int CvUnit::GetHealNeutralTerritoryFromNearbyUnit() const
{
	VALIDATE_OBJECT
		int iRange = 0;
	int iHeal = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getNearbyHealNeutralTerritory() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					iHeal = pUnit->getNearbyHealNeutralTerritory();
				}
			}
		}
	}
	return iHeal;
}
int CvUnit::GetHealFriendlyTerritoryFromNearbyUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iHeal = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getNearbyHealFriendlyTerritory() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					iHeal = pUnit->getNearbyHealFriendlyTerritory();
				}
			}
		}
	}
	return iHeal;
}

bool CvUnit::IsHiddenByNearbyUnit(const CvPlot* pAtPlot) const
{
	if (!MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return false;

	VALIDATE_OBJECT
	int iRange = 0;
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return false;
	}

	if (!IsCombatUnit())
		return false;

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->isGiveInvisibility() && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(pAtPlot->getX(), pAtPlot->getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()) && (pUnit->getTeam() == getTeam()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
int CvUnit::GetGiveOutsideFriendlyLandsModifierToUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iMod = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveOutsideFriendlyLandsModifier() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				// Only Giving it out to a specific domain?
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iMod = pUnit->getGiveOutsideFriendlyLandsModifier();
					}
				}
			}
		}
	}
	return iMod;
}
int CvUnit::GetGiveExtraAttacksToUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iExtraAttacks = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveExtraAttacks() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				// Only Giving it out to a specific domain?
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()))
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iExtraAttacks = pUnit->getGiveExtraAttacks();
					}
				}
			}
		}
	}
	return iExtraAttacks;
}
int CvUnit::GetGiveHPIfEnemyKilledToUnit() const
{
	VALIDATE_OBJECT
	int iRange = 0;
	int iHP = 0;
	if (plot() == NULL)
	{
		return 0;
	}

	const std::vector<std::pair<int, int>>& possibleUnits = GET_PLAYER(getOwner()).GetAreaEffectPromotionUnits();
	for (std::vector<std::pair<int, int>>::const_iterator it = possibleUnits.begin(); it != possibleUnits.end(); ++it)
	{
		//first quick check with a large, fixed distance
		CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
		if (plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), getX(), getY()) > 4)
			continue;

		CvUnit* pUnit = GET_PLAYER(getOwner()).getUnit(it->first);
		if (pUnit && pUnit->getGiveHPIfEnemyKilled() != 0 && pUnit != this)
		{
			iRange = pUnit->GetNearbyUnitPromotionsRange();
			if (plotDistance(getX(), getY(), pUnit->getX(), pUnit->getY()) <= iRange)
			{
				if (pUnit->getGiveDomain() != NO_DOMAIN && (pUnit->getGiveDomain() == getDomainType()) && !IsCanAttackRanged())
				{
					if (!pUnit->isGiveOnlyOnStartingTurn() || (pUnit->getMoves() >= pUnit->maxMoves()))
					{
						iHP = pUnit->getGiveHPIfEnemyKilled();
					}
				}
			}
		}
	}
	return iHP;
}

//	--------------------------------------------------------------------------------
/// Great General close enough to give us a bonus?
#if defined(MOD_BALANCE_CORE_MILITARY)
bool CvUnit::IsNearCityAttackSupport(const CvPlot* pAtPlot, const CvUnit* pIgnoreThisGeneral) const
{
	VALIDATE_OBJECT

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return false;
	}

	return (GET_PLAYER(getOwner()).GetAreaEffectModifier(AE_SIEGETOWER, getDomainType(), pAtPlot, pIgnoreThisGeneral) > 0);
}

//	--------------------------------------------------------------------------------
/// Great General close enough to give us a bonus?
bool CvUnit::IsNearGreatGeneral(const CvPlot* pAtPlot, const CvUnit* pIgnoreThisGeneral) const
{
	VALIDATE_OBJECT
	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return false;
	}
	
	return (GET_PLAYER(getOwner()).GetAreaEffectModifier(AE_GREAT_GENERAL, getDomainType(), pAtPlot, pIgnoreThisGeneral) > 0);
}
#endif

//	--------------------------------------------------------------------------------
/// Great General in our hex?
bool CvUnit::IsStackedGreatGeneral(const CvPlot* pLoopPlot, const CvUnit* pIgnoreThisGeneral) const
{
	if (pLoopPlot == NULL)
	{
		pLoopPlot = plot();
		if (pLoopPlot == NULL)
			return false;
	}
	const IDInfo* pUnitNode = NULL;
	CvUnit* pLoopUnit = NULL;

	// If there are Units here, loop through them
	if(pLoopPlot->getNumUnits() > 0)
	{
		pUnitNode = pLoopPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit && pLoopUnit != pIgnoreThisGeneral)
			{
				// Great General unit
				// IsNearGreatGeneral() includes GAs, so we'll add them here as well
				if(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral())
				{
					// Same domain
					if(pLoopUnit->getDomainType() == getDomainType())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatGeneralStackMovement(const CvPlot* pLoopPlot) const
{
	int iRtnValue = maxMoves();

	if (pLoopPlot == NULL)
		pLoopPlot = plot();

	// If there are Units here, loop through them
	if(pLoopPlot->getNumUnits() > 0)
	{
		const IDInfo* pUnitNode = pLoopPlot->headUnitNode();

		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::GetPlayerUnit(*pUnitNode);
			pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

			if(pLoopUnit)
			{
				// Give movement to great general?
				if(pLoopUnit->IsGreatGeneralReceivesMovement())
				{
					// Same domain
					if(pLoopUnit->getDomainType() == getDomainType())
					{
						iRtnValue = std::max(iRtnValue, pLoopUnit->maxMoves());
						break;
					}
				}
			}
		}
	}

	return iRtnValue;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
int CvUnit::GetReverseGreatGeneralModifier(const CvPlot* pAtPlot) const
{
	VALIDATE_OBJECT

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}
	
	int iMaxMod = 0;
	const std::vector<PlayerTypes>& vEnemies = GET_PLAYER(getOwner()).GetPlayersAtWarWith();
	for(std::vector<PlayerTypes>::const_iterator it=vEnemies.begin(); it!=vEnemies.end(); ++it)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER(*it);
		const std::vector<std::pair<int,int>>& possibleUnits = kLoopPlayer.GetAreaEffectNegativeUnits();
		for(std::vector<std::pair<int,int>>::const_iterator it = possibleUnits.begin(); it!=possibleUnits.end(); ++it)
		{
			//performance: very rough distance check first without looking up the unit pointer ...
			//do not reuse the plot below
			{
				CvPlot* pUnitPlot = GC.getMap().plotByIndexUnchecked(it->second);
				if (plotDistance(*pUnitPlot, *pAtPlot) > 8)
					continue;
			}

			CvUnit* pUnit = kLoopPlayer.getUnit(it->first);
			//catch weird problems
			if (pUnit == NULL || pUnit->isDelayedDeath() || pUnit->plot() == NULL)
				continue;

			// Unit with a combat modifier against the enemy
			int iMod = pUnit->getNearbyEnemyCombatMod();
			if(iMod != 0)
			{
				// Same domain
				if(((pUnit->getDomainType() == getDomainType()) && (pUnit->getDomainType() == DOMAIN_LAND) && !pUnit->isEmbarked()) || ((pUnit->getDomainType() == getDomainType()) && (pUnit->getDomainType() == DOMAIN_SEA)))
				{
					// Within range?
					int iDistance = plotDistance(*pAtPlot, *pUnit->plot());
					if(iDistance <= pUnit->getNearbyEnemyCombatRange())
					{
						// Don't assume the first one found is the worst!
						// Combat modifiers are negative, as applied against the defender (and not for the attacker)
						if (iMod < iMaxMod)
						{
							iMaxMod = iMod;
						}
					}
				}
			}
		}
	}

	return iMaxMod;
}
#endif

//	--------------------------------------------------------------------------------
int CvUnit::GetNearbyImprovementModifier(const CvPlot* pAtPlot) const
{
	if(pAtPlot == NULL)
		pAtPlot = plot();

	return std::max(GetNearbyImprovementModifierFromTraits(pAtPlot), GetNearbyImprovementModifierFromPromotions(pAtPlot));
}

int CvUnit::GetNearbyImprovementModifierFromTraits(const CvPlot* pAtPlot) const
{
	CvPlayer& kPlayer = GET_PLAYER(m_eOwner);
	CvPlayerTraits* playerTraits = kPlayer.GetPlayerTraits();

	int iImprovementRange = playerTraits->GetNearbyImprovementBonusRange();
	int iImprovementModifier = playerTraits->GetNearbyImprovementCombatBonus();

	if (pAtPlot == NULL)
	{
		pAtPlot = plot();
		if (pAtPlot == NULL)
			return 0;
	}

	const std::vector<int>& possiblePlots = GET_PLAYER(getOwner()).GetAreaEffectPositiveFromTraitsPlots();
	for(std::vector<int>::const_iterator it = possiblePlots.begin(); it!=possiblePlots.end(); ++it)
	{
		CvPlot* pCandidatePlot = GC.getMap().plotByIndexUnchecked(*it);

		if (pCandidatePlot)
		{
			if (plotDistance( pAtPlot->getX(),pAtPlot->getY(),pCandidatePlot->getX(),pCandidatePlot->getY() ) <= iImprovementRange)
				return iImprovementModifier;
		}

	}

	return 0;
}

int CvUnit::GetNearbyImprovementModifierFromPromotions(const CvPlot* pAtPlot) const
{
	return GetNearbyImprovementModifier(GetCombatBonusImprovement(), GetNearbyImprovementBonusRange(), GetNearbyImprovementCombatBonus(), pAtPlot);
}

int CvUnit::GetNearbyImprovementModifier(ImprovementTypes eBonusImprovement, int iImprovementRange, int iImprovementModifier, const CvPlot* pAtPlot) const
{
	if(iImprovementModifier != 0)
	{
		if (pAtPlot == NULL)
		{
			pAtPlot = plot();
			if (pAtPlot == NULL)
				return false;
		}

		CvPlot* pLoopPlot = NULL;

		// Look around this Unit to see if there's an improvement nearby
		for(int iX = -iImprovementRange; iX <= iImprovementRange; iX++)
		{
			for(int iY = -iImprovementRange; iY <= iImprovementRange; iY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pAtPlot->getX(), pAtPlot->getY(), iX, iY, iImprovementRange);
				if(pLoopPlot != NULL)
				{
					// Is the right improvement here?
					if(pLoopPlot->getImprovementType() == eBonusImprovement)
					{
						return iImprovementModifier;
					}
				}
			}
		}
	}

	return 0;
}

#if defined(MOD_BALANCE_CORE)
int CvUnit::GetNearbyUnitClassModifierFromUnitClass(const CvPlot* pAtPlot) const
{
	return GetNearbyUnitClassModifier(getCombatBonusFromNearbyUnitClass(), getNearbyUnitClassBonusRange(), getNearbyUnitClassBonus(), pAtPlot);
}

int CvUnit::GetNearbyUnitClassModifier(UnitClassTypes eUnitClass, int iUnitClassRange, int iUnitClassModifier, const CvPlot* pAtPlot) const
{
	if(iUnitClassModifier != 0)
	{
		if (pAtPlot == NULL)
		{
			pAtPlot = plot();
			if (pAtPlot == NULL)
				return false;
		}

		CvPlot* pLoopPlot = NULL;
		// Look around this Unit to see if there's a nearby UnitClass that will give us the modifier
		for(int iX = -iUnitClassRange; iX <= iUnitClassRange; iX++)
		{
			for(int iY = -iUnitClassRange; iY <= iUnitClassRange; iY++)
			{
				pLoopPlot = plotXYWithRangeCheck(pAtPlot->getX(), pAtPlot->getY(), iX, iY, iUnitClassRange);
				if (pLoopPlot != NULL && pLoopPlot->getNumUnits() != 0)
				{
					for (int iK = 0; iK < pLoopPlot->getNumUnits(); iK++)
					{
						CvUnit*	pLoopUnit = pLoopPlot->getUnitByIndex(iK);
						if (pLoopUnit != NULL)
						{
							if (pLoopUnit->getUnitClassType() == eUnitClass)
							{
								if(GET_PLAYER(pLoopUnit->getOwner()).getTeam() == GET_PLAYER(getOwner()).getTeam())
								{
									return iUnitClassModifier;
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

void CvUnit::DoNearbyUnitPromotion(const CvPlot* pPlot)
{
	VALIDATE_OBJECT
	if (pPlot == NULL)
	{
		pPlot = plot();
	}
	if(pPlot != NULL)
	{
		if (m_pUnitInfo->GetFriendlyLandsPromotion() != NO_PROMOTION)
		{
			const PromotionTypes eFriendlyLandsPromotion = m_pUnitInfo->GetFriendlyLandsPromotion();
			if (pPlot->IsFriendlyTerritory(getOwner())
				|| (pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).HasSameIdeology(getOwner()) && GET_TEAM(pPlot->getTeam()).IsAllowsOpenBordersToTeam(getTeam())))

			{
				if (!isHasPromotion(eFriendlyLandsPromotion))
				{
					setHasPromotion(eFriendlyLandsPromotion, true);
				}
			}
			else
			{
				if (isHasPromotion(eFriendlyLandsPromotion))
				{
					setHasPromotion(eFriendlyLandsPromotion, false);
				}
			}
		}
		CvPlayer& pUnitPlayer = GET_PLAYER(getOwner());
		const std::vector<PlayerTypes>& vEnemies = pUnitPlayer.GetPlayersAtWarWith();
		for (std::vector<PlayerTypes>::const_iterator it = vEnemies.begin(); it != vEnemies.end(); ++it)
		{
			CvPlayer& kPlayer = GET_PLAYER(*it);
			if (kPlayer.GetPlayerTraits()->IsWarsawPact())
			{
				const PromotionTypes eWarSawPactPromotion = kPlayer.GetPlayerTraits()->GetEnemyWarSawPactPromotion();
				if ((pPlot->getTeam() != NO_TEAM && pPlot->getTeam() == GET_TEAM(kPlayer.getTeam()).GetID())
					|| (pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).isMinorCiv() && GET_PLAYER(pPlot->getOwner()).GetMinorCivAI()->GetFriendshipLevelWithMajor(kPlayer.GetID()) >= 1)
					|| (pPlot->getOwner() != NO_PLAYER && GET_PLAYER(pPlot->getOwner()).HasSameIdeology(kPlayer.GetID()) && GET_TEAM(pPlot->getTeam()).IsAllowsOpenBordersToTeam(GET_TEAM(kPlayer.getTeam()).GetID())))
				{
					if(eWarSawPactPromotion != NO_PROMOTION && !isHasPromotion(eWarSawPactPromotion))
						setHasPromotion(eWarSawPactPromotion, true);
				}
				else
				{
					if (eWarSawPactPromotion != NO_PROMOTION && isHasPromotion(eWarSawPactPromotion))
						setHasPromotion(kPlayer.GetPlayerTraits()->GetEnemyWarSawPactPromotion(), false);
				}
			}
		}
		if (getPillageBonusStrengthPercent() != 0)
		{
			if (pPlot->IsImprovementPillaged())
			{
				SetBaseCombatStrength(getUnitInfo().GetCombat() + ((getPillageBonusStrengthPercent() * getUnitInfo().GetCombat()) / 100));
			}
			else
			{
				SetBaseCombatStrength(getUnitInfo().GetCombat());
			}
		}
	}
}
void CvUnit::DoImprovementExperience(const CvPlot* pPlot)
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}
	if(pPlot != NULL)
	{
		int iExperience = 0;
		if (pPlot->IsUnitPlotExperience() && pPlot->getOwner() == getOwner() && !pPlot->IsImprovementPillaged())
		{
			if (pPlot->GetUnitPlotExperience() > 0)
			{
				iExperience += pPlot->GetUnitPlotExperience();
			}
			if (pPlot->GetUnitPlotGAExperience() > 0 && GET_PLAYER(pPlot->getOwner()).isGoldenAge())
			{
				iExperience += pPlot->GetUnitPlotGAExperience();
			}
			if (IsCombatUnit() && pPlot == plot())
			{
				changeExperienceTimes100(iExperience * 100);
			}
		}
	}
}

void CvUnit::DoStackedGreatGeneralExperience(const CvPlot* pPlot)
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}
	if(pPlot != NULL)
	{
		int iExperience = getStackedGreatGeneralExperience();
		if(IsStackedGreatGeneral(pPlot) && iExperience > 0)
		{
			changeExperienceTimes100(iExperience * 100);
		}
	}
}

void CvUnit::DoConvertOnDamageThreshold(const CvPlot* pPlot)
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}
	if(pPlot != NULL)
	{
		if (isConvertOnDamage())
		{
			int iDamage = getDamageThreshold();
			if(getDamage() > iDamage)
			{
				// let's not do the conversion unless we are in our native domain
				if(getConvertDamageOrFullHPUnit() != NO_UNIT && pPlot->getDomain() == getDomainType())
				{
					UnitAITypes eAIType = NO_UNITAI;
					const CvUnitEntry* pkUnitType = GC.getUnitInfo(getConvertDamageOrFullHPUnit());
					if(pkUnitType != NULL)
					{
						eAIType = pkUnitType->GetDefaultUnitAIType();
						CvUnit* pNewUnit = GET_PLAYER(getOwner()).initUnit(getConvertDamageOrFullHPUnit(), getX(), getY(), eAIType);
						pNewUnit->convert(this, true);
						pNewUnit->finishMoves();
						pNewUnit->restoreFullMoves();
					}
				}
			}
		}
		else if (isConvertOnFullHP())
		{
			if (getDamage() == 0)
			{
				// let's not do the conversion unless we are in our native domain
				if (getConvertDamageOrFullHPUnit() != NO_UNIT && pPlot->getDomain() == getDomainType())
				{
					UnitAITypes eAIType = NO_UNITAI;
					const CvUnitEntry* pkUnitType = GC.getUnitInfo(getConvertDamageOrFullHPUnit());
					if (pkUnitType != NULL)
					{
						eAIType = pkUnitType->GetDefaultUnitAIType();
						CvUnit* pNewUnit = GET_PLAYER(getOwner()).initUnit(getConvertDamageOrFullHPUnit(), getX(), getY(), eAIType);
						pNewUnit->convert(this, true);
						pNewUnit->finishMoves();
						pNewUnit->restoreFullMoves();
					}
				}
			}
		}
	}
}
// Have a unit that'll convert an enemy unit into a Barbarian?
void CvUnit::DoConvertEnemyUnitToBarbarian(const CvPlot* pPlot)
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}
	if(pPlot != NULL)
	{
		CvPlot* pAdjacentPlot = NULL;
		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot != NULL)
			{
				if(isConvertEnemyUnitToBarbarian())
				{
					CvUnit* pConvertUnit = NULL;
					int iDamageTheshold = getDamageThreshold();
					CvUnit* pAdjacentUnit = pAdjacentPlot->getBestDefender(NO_PLAYER);
					if(pAdjacentUnit != NULL && pAdjacentUnit->IsCombatUnit() && !pAdjacentUnit->isBarbarian() && !pAdjacentPlot->isEnemyCity(*this))
					{
						int iExistingDamage = pAdjacentUnit->getDamage();
						if(GET_PLAYER(getOwner()).IsAtWarWith(pAdjacentUnit->getOwner()))
						{
							if(iExistingDamage > iDamageTheshold)
							{
								CvPlayer* pBarbPlayer = &GET_PLAYER(BARBARIAN_PLAYER);
								pConvertUnit = pBarbPlayer->initUnit(pAdjacentUnit->getUnitType(), pAdjacentUnit->getX(), pAdjacentUnit->getY(), pAdjacentUnit->AI_getUnitAIType(), REASON_CONVERT, true /*bNoMove*/, false);
								pConvertUnit->convert(pAdjacentUnit, false);
								pConvertUnit->setupGraphical();
								pConvertUnit->setDamage(iExistingDamage, BARBARIAN_PLAYER);
								pConvertUnit->finishMoves();

								CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
								if(pNotifications)
								{
									CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHAOS_CONVERSION");
									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHAOS_CONVERSION");
									pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, pAdjacentUnit->getX(), pAdjacentUnit->getY(), -1);
								}
							}
						}
					}
				}/*
				// Is this unit running into a unit that might convert it into a barbarian??
				if(IsCombatUnit() && !isBarbarian() && !pPlot->isFriendlyCity(*this))
				{
					CvUnit* pConvertUnit = NULL;
					int iExistingDamage = getDamage();
					if(pAdjacentPlot->getNumUnits() > 0)
					{
						for(int iNearbyUnitLoop = 0; iNearbyUnitLoop < pAdjacentPlot->getNumUnits(); iNearbyUnitLoop++)
						{
							const CvUnit* const adjUnit = pAdjacentPlot->getUnitByIndex(iNearbyUnitLoop);
							if(adjUnit != NULL && adjUnit->isConvertEnemyUnitToBarbarian())
							{
								int iDamageTheshold = adjUnit->getDamageThreshold();
								if(GET_PLAYER(getOwner()).IsAtWarWith(adjUnit->getOwner()))
								{
									if(iExistingDamage > iDamageTheshold)
									{

										CvPlayer* pBarbPlayer = &GET_PLAYER(BARBARIAN_PLAYER);
										pConvertUnit = pBarbPlayer->initUnit(getUnitType(), getX(), getY(), AI_getUnitAIType(), NO_DIRECTION, true /*bNoMove, false);
										pConvertUnit->convert(this, false);
										pConvertUnit->setupGraphical();
										pConvertUnit->setDamage(iExistingDamage, BARBARIAN_PLAYER);
										pConvertUnit->finishMoves();

										CvNotifications* pNotifications = GET_PLAYER(adjUnit->getOwner()).GetNotifications();
										if(pNotifications)
										{
											CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_CHAOS_CONVERSION");
											CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_CHAOS_CONVERSION");
											pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, getX(), getY(), -1);
										}
									}
								}
							}
						}
					}
				}*/
			}
		}
	}
}

void CvUnit::DoConvertReligiousUnitsToMilitary(const CvPlot* pPlot)
{
	if (!pPlot)
		pPlot = plot();

	if (!pPlot)
		return;

	if (!isReligiousUnit())
		return;

	if (pPlot->getOwner() == NO_PLAYER)
		return;

	CvPlayer& kPlayer = GET_PLAYER(pPlot->getOwner());
	int iChanceToConvertReligiousUnit = kPlayer.GetPlayerTraits()->GetChanceToConvertReligiousUnits();
	if (iChanceToConvertReligiousUnit <= 0)
		return;

	if (getTeam() == kPlayer.getTeam())
		return;

	if (GC.getGame().randRangeInclusive(1, 100, CvSeeder(pPlot->GetPseudoRandomSeed())) <= iChanceToConvertReligiousUnit)
	{
		UnitTypes eUnit = kPlayer.GetCompetitiveSpawnUnitType(false, false, true, true);
		if (eUnit == NO_UNIT)
			return;

		CvUnit* pConvertUnit = kPlayer.initUnit(eUnit, this->getX(), this->getY(), NO_UNITAI, REASON_CONVERT, true);
		pConvertUnit->convert(this, false);
		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if (pNotifications)
		{
			// These text keys don't even exist! Please add before use
			Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_HUSSITE_CONVERSION");
			strText << pConvertUnit->getNameKey() << kPlayer.getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_HUSSITE_CONVERSION");
			strSummary << kPlayer.getNameKey();
			pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pConvertUnit->getX(), pConvertUnit->getY(), -1);
		}
	}
}

//finish improvements (mostly roads) at the beginning of the turn so we can use them immediately
void CvUnit::DoFinishBuildIfSafe()
{
	if (isHuman() && !IsAutomated())
		return;

	BuildTypes eBuild = getBuildType();
	if (eBuild != NO_BUILD)
	{
		int iBuildTimeLeft = plot()->getBuildTurnsLeft(eBuild, getOwner(), 0, 0);
		if (iBuildTimeLeft == 0 && canMove() && GetDanger() == 0)
		{
			BuilderDirective eDirective = GET_PLAYER(m_eOwner).GetBuilderTaskingAI()->GetAssignedDirective(this);
			if (eDirective.m_sX != m_iX || eDirective.m_sY != m_iY || eDirective.m_eBuild != eBuild)
				return;

			CvUnitMission::ContinueMission(this);
		}
	}
}
#endif 

bool CvUnit::IsCombatSupportUnit() const
{
	return IsGreatGeneral() || IsGreatAdmiral() || IsCityAttackSupport() || IsSapper();
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatGeneral() const
{
	VALIDATE_OBJECT

	if (IsCombatUnit())
		return false;

	if (getUnitInfo().GetUnitAIType(UNITAI_GENERAL))
		return true;

	return GetGreatGeneralCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatGeneralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatGeneralCount += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatAdmiral() const
{
	VALIDATE_OBJECT

	if (IsCombatUnit())
		return false;

	if (getUnitInfo().GetUnitAIType(UNITAI_ADMIRAL))
		return true;

	return GetGreatAdmiralCount() > 0;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatAdmiralCount() const
{
	VALIDATE_OBJECT
	return m_iGreatAdmiralCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatAdmiralCount(int iChange)
{
	VALIDATE_OBJECT
	m_iGreatAdmiralCount += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAuraRangeChange() const
{
	VALIDATE_OBJECT
	return m_iAuraRangeChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAuraRangeChange(int iChange)
{
	VALIDATE_OBJECT
	m_iAuraRangeChange += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetAuraEffectChange() const
{
	VALIDATE_OBJECT
	return m_iAuraEffectChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeAuraEffectChange(int iChange)
{
	VALIDATE_OBJECT
	m_iAuraEffectChange += iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::GetNumRepairCharges() const
{
	VALIDATE_OBJECT
		return m_iNumRepairCharges;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumRepairCharges(int iChange)
{
	VALIDATE_OBJECT
		m_iNumRepairCharges += iChange;
}

int CvUnit::GetMilitaryCapChange() const
{
	VALIDATE_OBJECT
		return m_iMilitaryCapChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeMilitaryCapChange(int iChange)
{
	VALIDATE_OBJECT
		m_iMilitaryCapChange += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getGreatGeneralModifier() const
{
	VALIDATE_OBJECT
	return m_iGreatGeneralModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeGreatGeneralModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iGreatGeneralModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsGreatGeneralReceivesMovement() const
{
	return m_iGreatGeneralReceivesMovementCount > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralReceivesMovementCount(int iChange)
{
	m_iGreatGeneralReceivesMovementCount += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGreatGeneralCombatModifier() const
{
	return m_iGreatGeneralCombatModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGreatGeneralCombatModifier(int iChange)
{
	m_iGreatGeneralCombatModifier += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsIgnoreGreatGeneralBenefit() const
{
	return m_iIgnoreGreatGeneralBenefit > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIgnoreGreatGeneralBenefitCount(int iChange)
{
	m_iIgnoreGreatGeneralBenefit += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isNoSupply() const
{
	// I have a promotion, or granted from the unit class
	return (m_iNoSupply > 0) || getUnitInfo().IsNoSupply();
}

//	--------------------------------------------------------------------------------
void CvUnit::changeNoSupply(int iChange)
{
	m_iNoSupply += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getMaxHitPointsBase() const
{
	return m_iMaxHitPointsBase;
}

void CvUnit::setMaxHitPointsBase(int iMaxHitPoints)
{
	// Do NOT allow max hit points to be less than 1
	m_iMaxHitPointsBase = max(1, iMaxHitPoints);

	// Do NOT allow changing base max HP to kill the unit
	setDamage(min(getDamage(), max(1, GetMaxHitPoints()-1)));

	setInfoBarDirty(true);
}

void CvUnit::changeMaxHitPointsBase(int iChange)
{
	setMaxHitPointsBase(getMaxHitPointsBase() + iChange);
}

//	--------------------------------------------------------------------------------
int CvUnit::getMaxHitPointsChange() const
{
	return m_iMaxHitPointsChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeMaxHitPointsChange(int iChange)
{
	m_iMaxHitPointsChange += iChange;

	setInfoBarDirty(true);
}

//	--------------------------------------------------------------------------------
int CvUnit::getMaxHitPointsModifier() const
{
	return m_iMaxHitPointsModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeMaxHitPointsModifier(int iChange)
{
	m_iMaxHitPointsModifier += iChange;

	setInfoBarDirty(true);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsIgnoreZOC() const
{
	return m_iIgnoreZOC > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIgnoreZOCCount(int iChange)
{
	m_iIgnoreZOC += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsSapper() const
{
	return (m_iSapperCount > 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeSapperCount(int iChange)
{
	m_iSapperCount += iChange;
}

#if defined(MOD_BALANCE_CORE)
// No penalty when wounded, on top of that gain a combat bonus the more wounded the unit is
//	--------------------------------------------------------------------------------
bool CvUnit::IsStrongerDamaged() const
{
	return m_iStrongerDamaged> 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIsStrongerDamaged(int iChange)
{
	m_iStrongerDamaged += iChange;
}

// No penalty from being wounded, no combat bonus
//	--------------------------------------------------------------------------------
bool CvUnit::IsFightWellDamaged() const
{
	return m_iFightWellDamaged > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeIsFightWellDamaged(int iChange)
{
	m_iFightWellDamaged += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGoodyHutYieldBonus() const
{
	return m_iGoodyHutYieldBonus;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeGoodyHutYieldBonus(int iChange)
{
	m_iGoodyHutYieldBonus += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetReligiousPressureModifier() const
{
	return m_iReligiousPressureModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeReligiousPressureModifier(int iChange)
{
	m_iReligiousPressureModifier += iChange;
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsCanHeavyCharge() const
{
	return m_iCanHeavyCharge > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeCanHeavyChargeCount(int iChange)
{
	m_iCanHeavyCharge += iChange;
}
#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::GetMoraleBreakChance() const
{
	return m_iCanMoraleBreak;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeMoraleBreakChance(int iChange)
{
	if (iChange < 0)
	{
		m_iCanMoraleBreak = MIN_INT;
		return;
	}
	m_iCanMoraleBreak += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetDamageAoEFortified() const
{
	return m_iDamageAoEFortified;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeDamageAoEFortified(int iChange)
{
	m_iDamageAoEFortified += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetWorkRateMod() const
{
	return m_iWorkRateMod;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeWorkRateMod(int iChange)
{
	m_iWorkRateMod += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetDamageReductionCityAssault() const
{
	return m_iDamageReductionCityAssault;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeDamageReductionCityAssault(int iChange)
{
	m_iDamageReductionCityAssault += iChange;
}

#endif
//	--------------------------------------------------------------------------------
int CvUnit::getFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getFriendlyLandsAttackModifier() const
{
	VALIDATE_OBJECT
	return m_iFriendlyLandsAttackModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFriendlyLandsAttackModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iFriendlyLandsAttackModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getOutsideFriendlyLandsModifier() const
{
	VALIDATE_OBJECT
	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return m_iOutsideFriendlyLandsModifier + GetGiveOutsideFriendlyLandsModifierToUnit();
	else
		return m_iOutsideFriendlyLandsModifier;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeOutsideFriendlyLandsModifier(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iOutsideFriendlyLandsModifier += iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getPillageChange() const
{
	VALIDATE_OBJECT
	return m_iPillageChange;
}

//	--------------------------------------------------------------------------------
void CvUnit::changePillageChange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iPillageChange += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getUpgradeDiscount() const
{
	VALIDATE_OBJECT
	return m_iUpgradeDiscount;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeUpgradeDiscount(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iUpgradeDiscount += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExperiencePercent() const
{
	VALIDATE_OBJECT
	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
		return m_iExperiencePercent + GetGoldenAgeGeneralExpPercent() + GetGiveExperiencePercentToUnit();
	else
		return m_iExperiencePercent;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeExperiencePercent(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iExperiencePercent += iChange;

		setInfoBarDirty(true);
	}
}

//	--------------------------------------------------------------------------------
DirectionTypes CvUnit::getFacingDirection(bool checkLineOfSightProperty) const
{
	VALIDATE_OBJECT
	if(checkLineOfSightProperty)
	{
		return NO_DIRECTION; //look in all directions - LOS property has been removed from units (wasn't being used)
	}
	else
	{
		return m_eFacingDirection;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::setFacingDirection(DirectionTypes facingDirection)
{
	VALIDATE_OBJECT
	if(facingDirection != m_eFacingDirection)
	{
		//remove old fog
		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));

		//change direction
		m_eFacingDirection = facingDirection;

		//clear new fog
		if (canChangeVisibility())
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));

		DLLUI->setDirty(ColoredPlots_DIRTY_BIT, true);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::rotateFacingDirectionClockwise()
{
	VALIDATE_OBJECT
	//change direction
	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}

//	--------------------------------------------------------------------------------
void CvUnit::rotateFacingDirectionCounterClockwise()
{
	VALIDATE_OBJECT
	//change direction
	DirectionTypes newDirection = (DirectionTypes)((m_eFacingDirection + NUM_DIRECTION_TYPES - 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(newDirection);
}

int CvUnit::GetBlockadeRange() const
{
	switch (getDomainType())
	{
	case NO_DOMAIN:
		return 0; // TODO: Should probably be an error, but maintains existing behavior.
	case DOMAIN_LAND:
	case DOMAIN_HOVER:
		if (MOD_ADJACENT_BLOCKADE)
			return 1;
		else
			return 0;
	case DOMAIN_SEA:
		return range(/*2 in CP, 1 in VP*/ GD_INT_GET(NAVAL_PLOT_BLOCKADE_RANGE), 0, 3);
	case DOMAIN_AIR:
	case DOMAIN_IMMOBILE:
		return 0;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isOutOfAttacks(bool bIgnoreMoves) const
{
	VALIDATE_OBJECT
	if (!bIgnoreMoves && !canMove())
		return true;

	// Units with blitz don't run out of attacks!
	if(isBlitz())
		return false;

	return getNumAttacksMadeThisTurn() >= getNumAttacks();
}


//	--------------------------------------------------------------------------------
void CvUnit::setMadeAttack(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue)
	{
		m_iAttacksMade++;
		m_bMovedThisTurn = true; //failsafe: attacking means no more fortification bonus, no matter what happens with the moves
	}
	else
	{
		m_iAttacksMade = 0;
		//note: m_bMovedThisTurn is reset in restoreFullMoves
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::GetNumInterceptions() const
{
	VALIDATE_OBJECT
	return m_iNumInterceptions;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeNumInterceptions(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
		m_iNumInterceptions += iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetExtraAirInterceptRange() const // JJ: NEW
{
	VALIDATE_OBJECT
	return m_iExtraAirInterceptRange;
}

//	--------------------------------------------------------------------------------
void CvUnit::ChangeExtraAirInterceptRange(int iChange) // JJ: NEW
{
	VALIDATE_OBJECT
	m_iExtraAirInterceptRange += iChange;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canInterceptNow() const
{
	VALIDATE_OBJECT
	//check embarkation status also because that might change after we collect the area effect units!
	return canIntercept() && getMadeInterceptionCount() < GetNumInterceptions();
}

//	--------------------------------------------------------------------------------
int CvUnit::getMadeInterceptionCount() const
{
	return m_iMadeInterceptionCount;
}

//	--------------------------------------------------------------------------------
void CvUnit::increaseInterceptionCount()
	{
		m_iMadeInterceptionCount++;
		m_bMovedThisTurn = true; //failsafe: intercepting means no more healing, no matter what happens with the moves
	}

void CvUnit::resetInterceptionCount()
	{
		m_iMadeInterceptionCount = 0;
	}

//	--------------------------------------------------------------------------------
bool CvUnit::isPromotionReady() const
{
	VALIDATE_OBJECT
	return m_bPromotionReady;
}

//	--------------------------------------------------------------------------------
void CvUnit::setPromotionReady(bool bNewValue)
{
	VALIDATE_OBJECT
	if(isPromotionReady() != bNewValue)
	{
		m_bPromotionReady = bNewValue;

		if(m_bPromotionReady)
		{
			SetAutomateType(NO_AUTOMATE);
			ClearMissionQueue();
			SetActivityType(ACTIVITY_AWAKE);
		}

		if(bNewValue)
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_UNIT_CAN_GET_PROMOTION");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_UNIT_CAN_GET_PROMOTION");
				pNotifications->Add(NOTIFICATION_UNIT_PROMOTION, strBuffer, strSummary, -1, -1, getUnitType(), GetID());

				if (MOD_API_ACHIEVEMENTS && isHuman() && !GC.getGame().isGameMultiPlayer() && GET_PLAYER(GC.getGame().getActivePlayer()).isLocalPlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_UNIT_PROMOTE);
				}
			}
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::testPromotionReady()
{
	VALIDATE_OBJECT
	
	setPromotionReady(((getExperienceTimes100() / 100) >= experienceNeeded()) && !isOutOfAttacks(true) && canAcquirePromotionAny());
}


//	--------------------------------------------------------------------------------
bool CvUnit::isDelayedDeath() const
{
	VALIDATE_OBJECT

	//some failsafes on top ...
	if (!m_pUnitInfo || !onMap())
		return true;

	return m_bDeathDelay;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isDelayedDeathExported() const
{
	VALIDATE_OBJECT
	return m_bDeathDelay;
}


//	--------------------------------------------------------------------------------
void CvUnit::startDelayedDeath()
{
	VALIDATE_OBJECT
	m_bDeathDelay = true;
}


//	--------------------------------------------------------------------------------
// Returns true if killed...
bool CvUnit::doDelayedDeath()
{
	VALIDATE_OBJECT
	if(m_bDeathDelay && !isFighting() && !IsBusy())
	{
		kill(false);
		return true;
	}

	return false;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isCombatFocus() const
{
	VALIDATE_OBJECT
	return m_bCombatFocus;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isInfoBarDirty() const
{
	VALIDATE_OBJECT
	return m_bInfoBarDirty;
}


//	--------------------------------------------------------------------------------
void CvUnit::setInfoBarDirty(bool bNewValue)
{
	VALIDATE_OBJECT

	DLLUI->setDirty(UnitInfo_DIRTY_BIT, bNewValue);
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsNotConverting() const
{
	VALIDATE_OBJECT
	return m_bNotConverting;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetNotConverting(bool bNewValue)
{
	VALIDATE_OBJECT
	if(bNewValue != IsNotConverting())
	{
		m_bNotConverting = bNewValue;
	}
}

// --------------------------------------------------------------------------------
// some units 'camouflage' as barbarians visually
// --------------------------------------------------------------------------------
PlayerTypes CvUnit::getVisualOwner(TeamTypes eFromPerspectiveOfTeam) const
{
	VALIDATE_OBJECT
	if(NO_TEAM == eFromPerspectiveOfTeam)
	{
		eFromPerspectiveOfTeam = GC.getGame().getActiveTeam();
	}

	if(getTeam() != eFromPerspectiveOfTeam && eFromPerspectiveOfTeam != BARBARIAN_TEAM)
	{
		if(isHiddenNationality())
		{
			if(!plot()->isCity())
			{
				return BARBARIAN_PLAYER;
			}
		}
	}

	return getOwner();
}


// --------------------------------------------------------------------------------
// some units 'camouflage' as barbarians for combat (ie war state)
// --------------------------------------------------------------------------------
PlayerTypes CvUnit::getCombatOwner(TeamTypes eFromPerspectiveOfTeam, const CvPlot& assumedUnitPlot) const
{
	VALIDATE_OBJECT
	if(eFromPerspectiveOfTeam != NO_TEAM && getTeam() != eFromPerspectiveOfTeam && eFromPerspectiveOfTeam != BARBARIAN_TEAM)
	{
		if(isAlwaysHostile(assumedUnitPlot))
		{
			return BARBARIAN_PLAYER;
		}
	}

	return getOwner();
}

//	--------------------------------------------------------------------------------
TeamTypes CvUnit::getTeam() const
{
	VALIDATE_OBJECT
	return ::getTeam( getOwner() );
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::GetOriginalOwner() const
{
	return m_eOriginalOwner;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetOriginalOwner(PlayerTypes ePlayer)
{
	m_eOriginalOwner = ePlayer;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::GetGiftedByPlayer() const
{
	return m_eGiftedByPlayer;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGiftedByPlayer(PlayerTypes ePlayer)
{
	m_eGiftedByPlayer = ePlayer;
}

//	--------------------------------------------------------------------------------
PlayerTypes CvUnit::getCapturingPlayer() const
{
	VALIDATE_OBJECT
	return m_eCapturingPlayer;
}

//	--------------------------------------------------------------------------------
void CvUnit::setCapturingPlayer(PlayerTypes eNewValue)
{
	VALIDATE_OBJECT
	m_eCapturingPlayer = eNewValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCapturedAsIs() const
{
	VALIDATE_OBJECT
	return m_bCapturedAsIs;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetCapturedAsIs(bool bSetValue)
{
	VALIDATE_OBJECT
	m_bCapturedAsIs = bSetValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCapturedAsConscript() const
{
	VALIDATE_OBJECT
	return m_bCapturedAsConscript;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetCapturedAsConscript(bool bSetValue)
{
	VALIDATE_OBJECT
	m_bCapturedAsConscript = bSetValue;
}

//	--------------------------------------------------------------------------------
const UnitTypes CvUnit::getUnitType() const
{
	VALIDATE_OBJECT
	return m_eUnitType;
}

//	--------------------------------------------------------------------------------
CvUnitEntry& CvUnit::getUnitInfo() const
{
	VALIDATE_OBJECT
	return *m_pUnitInfo;
}

bool CvUnit::isUnitAI(UnitAITypes eType) const
{
	return m_pUnitInfo->GetDefaultUnitAIType() == eType;
}

//	--------------------------------------------------------------------------------
UnitClassTypes CvUnit::getUnitClassType() const
{
	VALIDATE_OBJECT
	return (UnitClassTypes)getUnitInfo().GetUnitClassType();
}

//	--------------------------------------------------------------------------------
const UnitTypes CvUnit::getLeaderUnitType() const
{
	VALIDATE_OBJECT
	return m_eLeaderUnitType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setLeaderUnitType(UnitTypes leaderUnitType)
{
	VALIDATE_OBJECT
	if(m_eLeaderUnitType != leaderUnitType)
	{
		m_eLeaderUnitType = leaderUnitType;
	}
}

//	--------------------------------------------------------------------------------
const InvisibleTypes CvUnit::getInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eInvisibleType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eInvisibleType != InvisibleType)
	{
		m_eInvisibleType = InvisibleType;
	}
}

//	--------------------------------------------------------------------------------
const InvisibleTypes CvUnit::getSeeInvisibleType() const
{
	VALIDATE_OBJECT
	return m_eSeeInvisibleType;
}

//	--------------------------------------------------------------------------------
void CvUnit::setSeeInvisibleType(InvisibleTypes InvisibleType)
{
	VALIDATE_OBJECT
	if(m_eSeeInvisibleType != InvisibleType)
	{
		CvPlot* pPlot = GC.getMap().plotCheckInvalid(getX(), getY());
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, getSeeInvisibleType(), getFacingDirection(true));
		}
		m_eSeeInvisibleType = InvisibleType;
		if(pPlot && canChangeVisibility())
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, getSeeInvisibleType(), getFacingDirection(true));
		}
	}
}

//	--------------------------------------------------------------------------------
const CvUnit* CvUnit::getCombatUnit() const
{
	VALIDATE_OBJECT
	return GetPlayerUnit(m_combatUnit);
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::getCombatUnit()
{
	VALIDATE_OBJECT
	return GetPlayerUnit(m_combatUnit);
}


//	--------------------------------------------------------------------------------
void CvUnit::setCombatUnit(CvUnit* pCombatUnit, bool bAttacking)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatUnit != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
		m_bCombatFocus = (
			bAttacking && 
			!(DLLUI->isFocusedWidget()) && 
			(
				(getOwner() == GC.getGame().getActivePlayer()) || 
				(
					(pCombatUnit->getOwner() == GC.getGame().getActivePlayer()) && 
					!(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns())
				)
			)
		);
		m_combatUnit = pCombatUnit->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}

//	---------------------------------------------------------------------------
const CvCity* CvUnit::getCombatCity() const
{
	VALIDATE_OBJECT
	return ::GetPlayerCity(m_combatCity);
}

//	---------------------------------------------------------------------------
CvCity* CvUnit::getCombatCity()
{
	VALIDATE_OBJECT
	return ::GetPlayerCity(m_combatCity);
}

//	---------------------------------------------------------------------------
void CvUnit::setCombatCity(CvCity* pCombatCity)
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	if(pCombatCity != NULL)
	{
		CvAssertMsg(getCombatUnit() == NULL && getCombatCity() == NULL, "Combat Unit or City is not expected to be assigned");
		m_bCombatFocus = (!(DLLUI->isFocusedWidget()) && ((getOwner() == GC.getGame().getActivePlayer()) || ((pCombatCity->getOwner() == GC.getGame().getActivePlayer()) && !(GET_PLAYER(GC.getGame().getActivePlayer()).isSimultaneousTurns()))));
		m_combatCity = pCombatCity->GetIDInfo();
	}
	else
	{
		clearCombat();
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(true);
	}
}

//	----------------------------------------------------------------------------
void CvUnit::clearCombat()
{
	VALIDATE_OBJECT
	if(isCombatFocus())
	{
		DLLUI->setCombatFocus(false);
	}

	m_bCombatFocus = false;
	if(getCombatUnit() != NULL || getCombatCity() != NULL)
	{
		CvAssertMsg(plot()->isUnitFighting(), "plot()->isUnitFighting is expected to be true");
		m_combatCity.reset();
		m_combatUnit.reset();

		if(IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		CvInterfacePtr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
		if(plot()->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}

//	----------------------------------------------------------------------------
const CvUnit* CvUnit::getTransportUnit() const
{
	VALIDATE_OBJECT
	return GetPlayerUnit(m_transportUnit);
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::getTransportUnit()
{
	VALIDATE_OBJECT
	return GetPlayerUnit(m_transportUnit);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isCargo() const
{
	VALIDATE_OBJECT
	return !m_transportUnit.isInvalid();
}


//	--------------------------------------------------------------------------------
void CvUnit::setTransportUnit(CvUnit* pTransportUnit)
{
	VALIDATE_OBJECT
	CvUnit* pOldTransportUnit = NULL;

	pOldTransportUnit = getTransportUnit();

	if(pOldTransportUnit != pTransportUnit)
	{
		if(pOldTransportUnit != NULL)
		{
			pOldTransportUnit->changeCargo(-1);
		}

		if(pTransportUnit != NULL)
		{
			CvAssertMsg(pTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) > 0, "Cargo space is expected to be available");

			m_transportUnit = pTransportUnit->GetIDInfo();

			if (!MOD_CARGO_SHIPS && getDomainType() != DOMAIN_AIR)
			{
				SetActivityType(ACTIVITY_SLEEP);
			}

			if(GC.getGame().isFinalInitialized() && (!MOD_CARGO_SHIPS || getDomainType() == DOMAIN_AIR))
			{
				finishMoves();
			}
			pTransportUnit->changeCargo(1);
			pTransportUnit->SetActivityType(ACTIVITY_AWAKE);
		}
		else
		{
			m_transportUnit.reset();

			SetActivityType(ACTIVITY_AWAKE);
		}
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraDomainModifier(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraDomainModifiers[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraDomainModifiers[eIndex] = (m_extraDomainModifiers[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraDomainAttack(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraDomainAttacks[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraDomainAttack(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraDomainAttacks[eIndex] = (m_extraDomainAttacks[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
int CvUnit::getExtraDomainDefense(DomainTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraDomainDefenses[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraDomainDefense(DomainTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraDomainDefenses[eIndex] = (m_extraDomainDefenses[eIndex] + iChange);
}


//	--------------------------------------------------------------------------------
const CvString CvUnit::getName() const
{
	VALIDATE_OBJECT
	CvString strBuffer;

	if(m_strName.IsEmpty())
	{
		// For units, getUnitInfo().GetTextKey() (ie getNameKey()) and getUnitInfo().GetDescription() are the same
		return GetLocalizedText(getNameKey());
	}

	Localization::String name = Localization::Lookup(m_strName);
	// For units, getUnitInfo().GetTextKey() (ie getNameKey()) and getUnitInfo().GetDescription() are the same
	strBuffer.Format("%s (%s)", name.toUTF8(), GetLocalizedText(getNameKey()).c_str());

	return strBuffer;
}


//	--------------------------------------------------------------------------------
const char* CvUnit::getNameKey() const
{
	VALIDATE_OBJECT
#if defined(MOD_PROMOTIONS_UNIT_NAMING)
	if (MOD_PROMOTIONS_UNIT_NAMING) {
		if (!m_strUnitName.IsEmpty()) {
			return m_strUnitName.c_str();
		}
	}
#endif

	return getUnitInfo().GetTextKey();
}


#if defined(MOD_PROMOTIONS_UNIT_NAMING)
//	--------------------------------------------------------------------------------
const CvString CvUnit::getUnitName() const
{
	VALIDATE_OBJECT
	return m_strUnitName.GetCString();
}
//	--------------------------------------------------------------------------------
void CvUnit::setUnitName(const CvString& strNewValue)
{
	VALIDATE_OBJECT

	m_strUnitName = strNewValue;
	DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
}
#endif

//	--------------------------------------------------------------------------------
const CvString CvUnit::getNameNoDesc() const
{
	VALIDATE_OBJECT
	return m_strName.GetCString();
}


//	--------------------------------------------------------------------------------
void CvUnit::setName(CvString strNewValue)
{
	VALIDATE_OBJECT
	gDLL->stripSpecialCharacters(strNewValue);

	m_strName = strNewValue;
	DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
}
#if defined(MOD_GLOBAL_NO_LOST_GREATWORKS)
//	--------------------------------------------------------------------------------
const CvString CvUnit::getGreatName() const
{
	VALIDATE_OBJECT
	return m_strGreatName.GetCString();
}

//	--------------------------------------------------------------------------------
void CvUnit::setGreatName(const CvString& strName)
{
	VALIDATE_OBJECT
	m_strGreatName = strName;
}
#endif
//	--------------------------------------------------------------------------------
GreatWorkType CvUnit::GetGreatWork() const
{
	return m_eGreatWork;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGreatWork(GreatWorkType eGreatWork)
{
	VALIDATE_OBJECT
	m_eGreatWork = eGreatWork;
}

//	--------------------------------------------------------------------------------
bool CvUnit::HasGreatWork() const
{
	return (m_eGreatWork != NO_GREAT_WORK);
}

//	--------------------------------------------------------------------------------
bool CvUnit::HasUnusedGreatWork() const
{
	return (HasGreatWork() && !GC.getGame().GetGameCulture()->IsGreatWorkCreated(m_eGreatWork));
}

//	--------------------------------------------------------------------------------
int CvUnit::GetTourismBlastStrength() const
{
	return m_iTourismBlastStrength;
}
//	--------------------------------------------------------------------------------
void CvUnit::SetTourismBlastStrength(int iValue)
{
	m_iTourismBlastStrength = iValue;
}

int CvUnit::GetTourismBlastLength() const
{
	return m_iTourismBlastLength;
}
//	--------------------------------------------------------------------------------
void CvUnit::SetTourismBlastLength(int iValue)
{
	m_iTourismBlastLength = iValue;
}


#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::GetScienceBlastStrength() const
{
	return m_iScienceBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetScienceBlastStrength(int iValue)
{
	m_iScienceBlastStrength = iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetHurryStrength() const
{
	return m_iHurryStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetHurryStrength(int iValue)
{
	m_iHurryStrength = iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGoldBlastStrength() const
{
	return m_iGoldBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGoldBlastStrength(int iValue)
{
	m_iGoldBlastStrength = iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetCultureBlastStrength() const
{
	return m_iCultureBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetCultureBlastStrength(int iValue)
{
	m_iCultureBlastStrength = iValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::GetGAPBlastStrength() const
{
	return m_iGAPBlastStrength;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGAPBlastStrength(int iValue)
{
	m_iGAPBlastStrength = iValue;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetPromotionEverObtained(PromotionTypes eIndex, bool bValue)
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumPromotionInfos());

	m_abPromotionEverObtained[eIndex] =  bValue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsPromotionEverObtained(PromotionTypes eIndex) const
{
	return m_abPromotionEverObtained[eIndex];
}
#endif

//	--------------------------------------------------------------------------------
std::string CvUnit::getScriptData() const
{
	VALIDATE_OBJECT
	return m_strScriptData;
}

//	--------------------------------------------------------------------------------
void CvUnit::setScriptData(const std::string& strNewValue)
{
	VALIDATE_OBJECT
	m_strScriptData = strNewValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getScenarioData() const
{
	VALIDATE_OBJECT
	return m_iScenarioData;
}

//	--------------------------------------------------------------------------------
void CvUnit::setScenarioData(int iNewValue)
{
	VALIDATE_OBJECT
	m_iScenarioData = iNewValue;
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainCostInCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_ignoreTerrainCostInCount.begin(); it != m_ignoreTerrainCostInCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainCostInCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_ignoreTerrainCostInCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_ignoreTerrainCostInCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreTerrainCostFromCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_ignoreTerrainCostFromCount.begin(); it != m_ignoreTerrainCostFromCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreTerrainCostFromCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_ignoreTerrainCostFromCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_ignoreTerrainCostFromCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getTerrainDoubleMoveCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_terrainDoubleMoveCount.begin(); it != m_terrainDoubleMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_terrainDoubleMoveCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_terrainDoubleMoveCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreFeatureCostInCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_ignoreFeatureCostInCount.begin(); it != m_ignoreFeatureCostInCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreFeatureCostInCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_ignoreFeatureCostInCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_ignoreFeatureCostInCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getIgnoreFeatureCostFromCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_ignoreFeatureCostFromCount.begin(); it != m_ignoreFeatureCostFromCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeIgnoreFeatureCostFromCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_ignoreFeatureCostFromCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_ignoreFeatureCostFromCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_featureDoubleMoveCount.begin(); it != m_featureDoubleMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_featureDoubleMoveCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_featureDoubleMoveCount.push_back(make_pair(eIndex, iChange));
}


#if defined(MOD_PROMOTIONS_HALF_MOVE)
//	--------------------------------------------------------------------------------
int CvUnit::getTerrainHalfMoveCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_terrainHalfMoveCount.begin(); it != m_terrainHalfMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainHalfMoveCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_terrainHalfMoveCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_terrainHalfMoveCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getTerrainExtraMoveCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_terrainExtraMoveCount.begin(); it != m_terrainExtraMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainExtraMoveCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_terrainExtraMoveCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_terrainExtraMoveCount.push_back(make_pair(eIndex, iChange));
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureHalfMoveCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_featureHalfMoveCount.begin(); it != m_featureHalfMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureHalfMoveCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_featureHalfMoveCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_featureHalfMoveCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getFeatureExtraMoveCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_featureExtraMoveCount.begin(); it != m_featureExtraMoveCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureExtraMoveCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_featureExtraMoveCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_featureExtraMoveCount.push_back(make_pair(eIndex, iChange));
}
#endif

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::getTerrainDoubleHeal(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_terrainDoubleHeal.begin(); it != m_terrainDoubleHeal.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainDoubleHeal(TerrainTypes eIndex) const
{
	return getTerrainDoubleHeal(eIndex) > 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainDoubleHeal(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_terrainDoubleHeal;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_terrainDoubleHeal.push_back(make_pair(eIndex, iChange));
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureDoubleHeal(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_featureDoubleHeal.begin(); it != m_featureDoubleHeal.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureDoubleHeal(FeatureTypes eIndex) const
{
	return getFeatureDoubleHeal(eIndex) > 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureDoubleHeal(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_featureDoubleHeal;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_featureDoubleHeal.push_back(make_pair(eIndex, iChange));
}

void CvUnit::ChangeNumTimesAttackedThisTurn(PlayerTypes ePlayer, int iValue)
{
	VALIDATE_OBJECT
		CvAssertMsg(ePlayer >= 0, "ePlayer expected to be >= 0");
	CvAssertMsg(ePlayer < REALLY_MAX_PLAYERS, "ePlayer expected to be < NUM_DOMAIN_TYPES");
	m_aiNumTimesAttackedThisTurn[ePlayer] =  m_aiNumTimesAttackedThisTurn[ePlayer] + iValue;
}
int CvUnit::GetNumTimesAttackedThisTurn(PlayerTypes ePlayer) const
{
	VALIDATE_OBJECT
		CvAssertMsg(ePlayer >= 0, "eIndex expected to be >= 0");
	CvAssertMsg(ePlayer < REALLY_MAX_PLAYERS, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_aiNumTimesAttackedThisTurn[ePlayer];
}
#endif

//	--------------------------------------------------------------------------------
int CvUnit::getTerrainImpassableCount(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_terrainImpassableCount.begin(); it != m_terrainImpassableCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isTerrainImpassable(TerrainTypes eIndex) const
{
	return getTerrainImpassableCount(eIndex) > 0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeTerrainImpassableCount(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_terrainImpassableCount;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_terrainImpassableCount.push_back(make_pair(eIndex, iChange));
}


//	--------------------------------------------------------------------------------
int CvUnit::getFeatureImpassableCount(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_featureImpassableCount.begin(); it != m_featureImpassableCount.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isFeatureImpassable(FeatureTypes eIndex) const
{
	return getFeatureImpassableCount(eIndex)>0;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeFeatureImpassableCount(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_featureImpassableCount;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_featureImpassableCount.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_extraTerrainAttackPercent.begin(); it != m_extraTerrainAttackPercent.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_extraTerrainAttackPercent;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraTerrainAttackPercent.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	for (TerrainTypeCounter::const_iterator it = m_extraTerrainDefensePercent.begin(); it != m_extraTerrainDefensePercent.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	TerrainTypeCounter& mVec = m_extraTerrainDefensePercent;
	for (TerrainTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraTerrainDefensePercent.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_extraFeatureAttackPercent.begin(); it != m_extraFeatureAttackPercent.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_extraFeatureAttackPercent;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraFeatureAttackPercent.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	for (FeatureTypeCounter::const_iterator it = m_extraFeatureDefensePercent.begin(); it != m_extraFeatureDefensePercent.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	FeatureTypeCounter& mVec = m_extraFeatureDefensePercent;
	for (FeatureTypeCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraFeatureDefensePercent.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getUnitClassAttackMod(UnitClassTypes eIndex) const
{
	for (UnitClassCounter::const_iterator it = m_extraUnitClassAttackMod.begin(); it != m_extraUnitClassAttackMod.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeUnitClassAttackMod(UnitClassTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	UnitClassCounter& mVec = m_extraUnitClassAttackMod;
	for (UnitClassCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraUnitClassAttackMod.push_back(make_pair(eIndex, iChange));
}

//	--------------------------------------------------------------------------------
int CvUnit::getUnitClassDefenseMod(UnitClassTypes eIndex) const
{
	for (UnitClassCounter::const_iterator it = m_extraUnitClassDefenseMod.begin(); it != m_extraUnitClassDefenseMod.end(); ++it)
	{
		if (it->first == eIndex)
			return it->second;
	}

	return 0;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeUnitClassDefenseMod(UnitClassTypes eIndex, int iChange)
{
	if (iChange == 0)
		return;

	UnitClassCounter& mVec = m_extraUnitClassDefenseMod;
	for (UnitClassCounter::iterator it = mVec.begin(); it != mVec.end(); ++it)
	{
		if (it->first == eIndex)
		{
			it->second += iChange;

			if (it->second == 0)
				mVec.erase(it);

			return;
		}
	}

	m_extraUnitClassDefenseMod.push_back(make_pair(eIndex, iChange));
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
int CvUnit::getCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_iCombatModPerAdjacentUnitCombatModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatModPerAdjacentUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_iCombatModPerAdjacentUnitCombatModifier[eIndex] =  m_iCombatModPerAdjacentUnitCombatModifier[eIndex] + iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_iCombatModPerAdjacentUnitCombatAttackMod[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatModPerAdjacentUnitCombatAttackMod(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_iCombatModPerAdjacentUnitCombatAttackMod[eIndex] =  m_iCombatModPerAdjacentUnitCombatAttackMod[eIndex] + iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_iCombatModPerAdjacentUnitCombatDefenseMod[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeCombatModPerAdjacentUnitCombatDefenseMod(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_iCombatModPerAdjacentUnitCombatDefenseMod[eIndex] =  m_iCombatModPerAdjacentUnitCombatDefenseMod[eIndex] + iChange;
}

//	--------------------------------------------------------------------------------
int CvUnit::getYieldFromScouting(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_yieldFromScouting[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromScouting(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_yieldFromScouting[eIndex] =  m_yieldFromScouting[eIndex] + iChange;
	}
}
#endif
//	--------------------------------------------------------------------------------
int CvUnit::getYieldFromKills(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_yieldFromKills[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromKills(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_yieldFromKills[eIndex] =  m_yieldFromKills[eIndex] + iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getYieldFromBarbarianKills(YieldTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_yieldFromBarbarianKills[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromBarbarianKills(YieldTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange != 0)
	{
		m_yieldFromBarbarianKills[eIndex] =  m_yieldFromBarbarianKills[eIndex] + iChange;
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_extraUnitCombatModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitCombatClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_extraUnitCombatModifier[eIndex] =  m_extraUnitCombatModifier[eIndex] + iChange;
}


//	--------------------------------------------------------------------------------
int CvUnit::getUnitClassModifier(UnitClassTypes eIndex) const
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_unitClassModifier[eIndex];
}


//	--------------------------------------------------------------------------------
void CvUnit::changeUnitClassModifier(UnitClassTypes eIndex, int iChange)
{
	VALIDATE_OBJECT
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_unitClassModifier[eIndex] =  m_unitClassModifier[eIndex] + iChange;
}

//	--------------------------------------------------------------------------------
std::pair<int, int> CvUnit::getYieldFromPillage(YieldTypes eYield) const {
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Yield index out of bounds");
	CvAssertMsg(eYield > NO_YIELD, "Yield index out of bounds");

	const std::map<int, std::pair<int, int>>::const_iterator it = m_yieldFromPillage.find(static_cast<int>(eYield));
	if (it != m_yieldFromPillage.end())
	{
		return it->second;
	}

	return std::make_pair(0, 0);
}

//	--------------------------------------------------------------------------------
void CvUnit::changeYieldFromPillage(YieldTypes eYield, std::pair<int, int> change) {
	CvAssertMsg(eYield < NUM_YIELD_TYPES, "Yield index out of bounds");
	CvAssertMsg(eYield > NO_YIELD, "Yield index out of bounds");

	if ((change.first != 0 || change.second != 0) && eYield > NO_YIELD && eYield < NUM_YIELD_TYPES)
	{
		const int yieldIndex = static_cast<int>(eYield);
		const std::map<int, std::pair<int, int>>::iterator it = m_yieldFromPillage.find(yieldIndex);
		if (it != m_yieldFromPillage.end())
		{
			std::pair<int, int>& yieldValues = it->second;
			yieldValues.first += change.first;
			yieldValues.second += change.second;
			if (yieldValues.first == 0 && yieldValues.second == 0)
			{
				m_yieldFromPillage.erase(it);
			}
		}
		else
		{
			m_yieldFromPillage.insert(it, std::make_pair(yieldIndex, change));
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT
	CvAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	CvAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		return false;
	}

	if(isHasPromotion(ePromotion))
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	PromotionTypes ePromotionRoughTerrain = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_ROUGH_TERRAIN_ENDS_TURN");
	if(ePromotion == ePromotionRoughTerrain && kPlayer.GetPlayerTraits()->IsConquestOfTheWorld())
	{
		return false;
	}

	if (pkPromotionInfo->IsMountedOnly() && !getUnitInfo().IsMounted())
		return false;
#endif
	//Out-ranged?
	if (pkPromotionInfo->GetMinRange() != 0 && pkPromotionInfo->GetMinRange() > GetRange())
		return false;

	if (pkPromotionInfo->GetMaxRange() != 0 && GetRange() > 0 && pkPromotionInfo->GetMaxRange() < GetRange())
		return false;

	if (GetAirInterceptRange() <= 0)
	{
		if (pkPromotionInfo->GetInterceptChanceChange() != 0)
			return false;

		if (pkPromotionInfo->GetInterceptionCombatModifier() != 0)
			return false;

		if (pkPromotionInfo->GetNumInterceptionChange() != 0)
			return false;
	}
		

	// AND prereq
	if(pkPromotionInfo->GetPrereqPromotion() != NO_PROMOTION)
	{
		if(!isHasPromotion((PromotionTypes)(pkPromotionInfo->GetPrereqPromotion())))
		{
			return false;
		}
	}

	// OR prereqs
	bool bLacksOrPrereq = false;

	PromotionTypes ePromotion1 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion1();
	if(ePromotion1 != NO_PROMOTION)
	{
		if(!isHasPromotion(ePromotion1))
			bLacksOrPrereq = true;
	}

	// OR Promotion 2
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion2 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion2();
		if(ePromotion2 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion2))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 3
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion3 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion3();
		if(ePromotion3 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion3))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 4
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion4 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion4();
		if(ePromotion4 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion4))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 5
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion5 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion5();
		if(ePromotion5 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion5))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 6
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion6 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion6();
		if(ePromotion6 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion6))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 7
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion7 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion7();
		if(ePromotion7 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion7))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 8
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion8 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion8();
		if(ePromotion8 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion8))
				bLacksOrPrereq = false;
		}
	}

	// OR Promotion 9
	if(bLacksOrPrereq)
	{
		PromotionTypes ePromotion9 = (PromotionTypes) pkPromotionInfo->GetPrereqOrPromotion9();
		if(ePromotion9 != NO_PROMOTION)
		{
			if(isHasPromotion(ePromotion9))
				bLacksOrPrereq = false;
		}
	}

	if(bLacksOrPrereq)
	{
		return false;
	}

	if(pkPromotionInfo->GetTechPrereq() != NO_TECH)
	{
		if(!(GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes)(pkPromotionInfo->GetTechPrereq()))))
		{
			return false;
		}
	}

	if(!isPromotionValid(ePromotion))
	{
		return false;
	}

	if (MOD_EVENTS_UNIT_UPGRADES)
	{
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_CanHavePromotion, getOwner(), GetID(), ePromotion) == GAMEEVENTRETURN_FALSE)
			return false;

		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_UnitCanHavePromotion, getOwner(), GetID(), ePromotion) == GAMEEVENTRETURN_FALSE)
			return false;
	}
	else
	{
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(((int)getOwner()));
			args->Push(GetID());
			args->Push(ePromotion);

			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "CanHavePromotion", args.get(), bResult))
			{
				if (!bResult) 
				{
					return false;
				}
			}
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	VALIDATE_OBJECT

	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);
	if(promotionInfo == NULL)
	{
		return false;
	}

	if(!::isPromotionValid(ePromotion, getUnitType(), true))
		return false;

	// Insta-heal - must be damaged
	if(promotionInfo->IsInstaHeal())
	{
		if(getDamage() == 0)
			return false;
	}
#if defined(MOD_BALANCE_CORE)
	if(promotionInfo->IsBarbarianOnly())
	{
		if(!isBarbarian())
		{
			return false;
		}
	}
	if(promotionInfo->IsCityStateOnly())
	{
		if(!GET_PLAYER(getOwner()).isMinorCiv())
		{
			return false;
		}
	}
#endif

	// Can't acquire interception promotion if unit can't intercept!
	if(promotionInfo->GetInterceptionCombatModifier() != 0)
	{
		if(!canAirDefend())
			return false;
	}

	// Can't acquire Air Sweep promotion if unit can't air sweep!
	if(promotionInfo->GetAirSweepCombatModifier() != 0)
	{
		if(!IsAirSweepCapable())
			return false;
	}

	// Max Interception
	if(promotionInfo->GetInterceptChanceChange() > 0)
	{
		if(promotionInfo->GetInterceptChanceChange() + getInterceptChance() > /*100*/ GD_INT_GET(MAX_INTERCEPTION_PROBABILITY))
			return false;
	}

	// Max evasion
	if(promotionInfo->GetEvasionChange() > 0)
	{
		if(promotionInfo->GetEvasionChange() + evasionProbability() > /*90*/ GD_INT_GET(MAX_EVASION_PROBABILITY))
			return false;
	}

	// Hovering units (e.g. Helis) cannot embark
	if(IsHoveringUnit() && promotionInfo->IsAllowsEmbarkation())
		return false;

	if (isConvertUnit() && promotionInfo->IsAllowsEmbarkation())
		return false;
		
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
	// Only hovering units can deep water embark
	if(!IsHoveringUnit() && promotionInfo->IsEmbarkedDeepWater())
		return false;
#endif

	return true;
}


//	--------------------------------------------------------------------------------
bool CvUnit::canAcquirePromotionAny() const
{
	VALIDATE_OBJECT

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (canAcquirePromotion((PromotionTypes)iI))
		{
			return true;
		}
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	VALIDATE_OBJECT
	return m_Promotions.HasPromotion(eIndex);
}


//	--------------------------------------------------------------------------------
void CvUnit::setHasPromotion(PromotionTypes eIndex, bool bNewValue)
{
	VALIDATE_OBJECT
	int iChange = 0;
	int iI = 0;

	if (eIndex == NO_PROMOTION || eIndex >= GC.getNumPromotionInfos())
		return;

	CvPromotionEntry& thisPromotion = *GC.getPromotionInfo(eIndex);
	if (isHasPromotion(eIndex) != bNewValue)
	{
		if (bNewValue && MOD_GLOBAL_CANNOT_EMBARK && getUnitInfo().CannotEmbark())
		{
			if (thisPromotion.IsAllowsEmbarkation() || thisPromotion.IsEmbarkedAllWater())
				return;
			else if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && thisPromotion.IsEmbarkedDeepWater())
				return;
		}

		// Firaxis only use the CanMoveAllTerrain promotion in the Smokey Skies scenario,
		// which doesn't have the situation where the player discovers Optics ...
		if (bNewValue && (canMoveAllTerrain() || isConvertUnit()))
		{
			if (thisPromotion.IsAllowsEmbarkation() || thisPromotion.IsEmbarkedAllWater())
				return;
			else if (MOD_PROMOTIONS_DEEP_WATER_EMBARKATION && thisPromotion.IsEmbarkedDeepWater())
				return;
		}

		// Polynesia's units have immunity to Water Impeding promotions
		if (bNewValue && GET_PLAYER(getOwner()).GetPlayerTraits()->IsEmbarkedAllWater())
		{
			if (thisPromotion.GetTerrainImpassable(TERRAIN_OCEAN) ||
				thisPromotion.GetTerrainHalfMove  (TERRAIN_OCEAN) ||
				thisPromotion.GetTerrainExtraMove (TERRAIN_OCEAN) ||
				thisPromotion.GetTerrainImpassable(TERRAIN_COAST) ||
				thisPromotion.GetTerrainHalfMove  (TERRAIN_COAST) ||
				thisPromotion.GetTerrainExtraMove (TERRAIN_COAST))
			{
				return;
			}
		}

		// Plague Stuff
		int iPlagueID = thisPromotion.GetPlagueID();
		int iPlagueImmunityID = thisPromotion.GetPlagueIDImmunity();

		// ERROR! This should not happen.
		if (iPlagueID > -1 && iPlagueID == iPlagueImmunityID)
			return;

		if (bNewValue)
		{
			if (iPlagueID > -1)
			{
				// Is this a plague that we're immune to? Abort!
				if (ImmuneToPlague(iPlagueID))
					return;

				// If we just got a plague, remove any weaker version of the plague
				RemovePlague(iPlagueID, thisPromotion.GetPlaguePriority());
			}
			// If we just got immunity to a plague, remove the plague
			if (iPlagueImmunityID > -1)
			{
				RemovePlague(iPlagueImmunityID, -1);
			}
		}

		m_Promotions.SetPromotion(eIndex, bNewValue);
		iChange = ((isHasPromotion(eIndex)) ? 1 : -1);

		// Promotions will set Invisibility once but not change it later
		if(getInvisibleType() == NO_INVISIBLE && thisPromotion.GetInvisibleType() != NO_INVISIBLE)
		{
			setInvisibleType((InvisibleTypes) thisPromotion.GetInvisibleType());
		}
		if(getSeeInvisibleType() == NO_INVISIBLE && thisPromotion.GetSeeInvisibleType() != NO_INVISIBLE)
		{
			setSeeInvisibleType((InvisibleTypes) thisPromotion.GetSeeInvisibleType());
		}

		changeBlitzCount((thisPromotion.IsBlitz()) ? iChange : 0);
		changeAmphibCount((thisPromotion.IsAmphib()) ? iChange : 0);
		changeRiverCrossingNoPenaltyCount((thisPromotion.IsRiver()) ? iChange : 0);
		changeEnemyRouteCount((thisPromotion.IsEnemyRoute()) ? iChange : 0);
		changeRivalTerritoryCount((thisPromotion.IsRivalTerritory()) ? iChange : 0);
		changeIsSlowInEnemyLandCount((thisPromotion.IsMustSetUpToRangedAttack()) ? iChange : 0); //intended. promotion purpose was redefined
		changeRangedSupportFireCount((thisPromotion.IsRangedSupportFire()) ? iChange : 0);
		changeAlwaysHealCount((thisPromotion.IsAlwaysHeal()) ? iChange : 0);
		changeHealOutsideFriendlyCount((thisPromotion.IsHealOutsideFriendly()) ? iChange : 0);
		changeHillsDoubleMoveCount((thisPromotion.IsHillsDoubleMove()) ? iChange : 0);
		changeRiverDoubleMoveCount((thisPromotion.IsRiverDoubleMove()) ? iChange : 0);
		changeIgnoreTerrainCostCount((thisPromotion.IsIgnoreTerrainCost()) ? iChange : 0);
		changeIgnoreTerrainDamageCount((thisPromotion.IsIgnoreTerrainDamage()) ? iChange : 0);
		changeIgnoreFeatureDamageCount((thisPromotion.IsIgnoreFeatureDamage()) ? iChange : 0);
		changeExtraTerrainDamageCount((thisPromotion.IsExtraTerrainDamage()) ? iChange : 0);
		changeExtraFeatureDamageCount((thisPromotion.IsExtraFeatureDamage()) ? iChange : 0);

#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
		if (MOD_PROMOTIONS_IMPROVEMENT_BONUS) {
			if (thisPromotion.GetNearbyImprovementCombatBonus() > 0) {
				SetNearbyImprovementCombatBonus(thisPromotion.GetNearbyImprovementCombatBonus());
				SetNearbyImprovementBonusRange(thisPromotion.GetNearbyImprovementBonusRange());
				SetCombatBonusImprovement(thisPromotion.GetCombatBonusImprovement());
			}
		}
#endif
#if defined(MOD_BALANCE_CORE)
		if (thisPromotion.GetNearbyUnitClassBonus() > 0)
		{
			SetNearbyUnitClassBonus(thisPromotion.GetNearbyUnitClassBonus());
			SetNearbyUnitClassBonusRange(thisPromotion.GetNearbyUnitClassBonusRange());
			SetCombatBonusFromNearbyUnitClass(thisPromotion.GetCombatBonusFromNearbyUnitClass());
		}
		ChangeNearbyPromotion(thisPromotion.IsNearbyPromotion() ? iChange : 0);
		ChangeNearbyUnitPromotionRange(thisPromotion.GetNearbyRange() * iChange);
		ChangeNearbyCityCombatMod((thisPromotion.GetNearbyCityCombatMod()) * iChange);
		ChangeNearbyFriendlyCityCombatMod((thisPromotion.GetNearbyFriendlyCityCombatMod()) * iChange);
		ChangeNearbyEnemyCityCombatMod((thisPromotion.GetNearbyEnemyCityCombatMod()) * iChange);
		ChangePillageBonusStrengthPercent(thisPromotion.GetPillageBonusStrengthPercent() * iChange);
		ChangeStackedGreatGeneralExperience(thisPromotion.GetStackedGreatGeneralExperience() * iChange);
		ChangeWonderProductionModifier(thisPromotion.GetWonderProductionModifier() * iChange);
		ChangeMilitaryProductionModifier(thisPromotion.GetMilitaryProductionModifier() * iChange);
		ChangeNearbyEnemyDamage(thisPromotion.GetNearbyEnemyDamage() * iChange);
		ChangeAdjacentCityDefenseMod(thisPromotion.GetAdjacentCityDefenseMod() * iChange);
		ChangeGGGAXPPercent(thisPromotion.GetGeneralGoldenAgeExpPercent() * iChange);
		ChangeGiveCombatMod(thisPromotion.GetGiveCombatMod() * iChange);
		ChangeGiveHPIfEnemyKilled(thisPromotion.GetGiveHPIfEnemyKilled() * iChange);
		ChangeGiveExperiencePercent(thisPromotion.GetGiveExperiencePercent() * iChange);
		ChangeGiveOutsideFriendlyLandsModifier(thisPromotion.GetGiveOutsideFriendlyLandsModifier() * iChange);
		ChangeGiveExtraAttacks(thisPromotion.GetGiveExtraAttacks() * iChange);
		ChangeGiveDefenseMod(thisPromotion.GetGiveDefenseMod() * iChange);
		ChangeIsGiveInvisibility((thisPromotion.IsGiveInvisibility()) ? iChange : 0);
		ChangeNearbyHealEnemyTerritory(thisPromotion.GetNearbyHealEnemyTerritory() * iChange);
		ChangeNearbyHealNeutralTerritory(thisPromotion.GetNearbyHealNeutralTerritory() * iChange);
		ChangeNearbyHealFriendlyTerritory(thisPromotion.GetNearbyHealFriendlyTerritory() * iChange);
		SetIsGiveOnlyOnStartingTurn(thisPromotion.IsGiveOnlyOnStartingTurn() ? iChange>0 : false);
		ChangeIsConvertUnit((thisPromotion.IsConvertUnit()) ? iChange : 0);
		ChangeIsConvertEnemyUnitToBarbarian((thisPromotion.IsConvertEnemyUnitToBarbarian()) ? iChange : 0);
		ChangeIsConvertOnFullHP((thisPromotion.IsConvertOnFullHP()) ? iChange : 0);
		ChangeIsConvertOnDamage((thisPromotion.IsConvertOnDamage()) ? iChange : 0);
		ChangeDamageThreshold(thisPromotion.GetDamageThreshold() * iChange);
		if (getConvertDamageOrFullHPUnit() == NO_UNIT && thisPromotion.GetConvertDamageOrFullHPUnit() != NO_UNIT)
		{
			ChangeConvertDamageOrFullHPUnit((UnitTypes)thisPromotion.GetConvertDamageOrFullHPUnit());
		}
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
		if (MOD_PROMOTIONS_CROSS_MOUNTAINS) {
			changeCanCrossMountainsCount((thisPromotion.CanCrossMountains()) ? iChange : 0);
		}
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
		changeCanCrossOceansCount((thisPromotion.CanCrossOceans()) ? iChange : 0);
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
		if (MOD_PROMOTIONS_CROSS_ICE) {
			changeCanCrossIceCount((thisPromotion.CanCrossIce()) ? iChange : 0);
		}
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
		if (MOD_PROMOTIONS_GG_FROM_BARBARIANS) {
			changeGGFromBarbariansCount((thisPromotion.IsGGFromBarbarians()) ? iChange : 0);
		}
#endif
		ChangeRoughTerrainEndsTurnCount((thisPromotion.IsRoughTerrainEndsTurn()) ? iChange : 0);
		ChangeCapturedUnitsConscriptedCount((thisPromotion.IsCapturedUnitsConscripted()) ? iChange : 0);
		ChangeHoveringUnitCount((thisPromotion.IsHoveringUnit()) ? iChange : 0);
		changeFlatMovementCostCount((thisPromotion.IsFlatMovementCost()) ? iChange : 0);
		changeCanMoveImpassableCount((thisPromotion.IsCanMoveImpassable()) ? iChange : 0);
		changeOnlyDefensiveCount((thisPromotion.IsOnlyDefensive()) ? iChange : 0);
		changeNoAttackInOceanCount((thisPromotion.IsNoAttackInOcean()) ? iChange : 0);
		changeNoDefensiveBonusCount((thisPromotion.IsNoDefensiveBonus()) ? iChange : 0);
		changeNoCaptureCount((thisPromotion.IsNoCapture()) ? iChange : 0);
		changeNukeImmuneCount((thisPromotion.IsNukeImmune()) ? iChange: 0);
		changeHiddenNationalityCount((thisPromotion.IsHiddenNationality()) ? iChange: 0);
		changeAlwaysHostileCount((thisPromotion.IsAlwaysHostile()) ? iChange: 0);
		changeNoRevealMapCount((thisPromotion.IsNoRevealMap()) ? iChange: 0);
		ChangeReconCount((thisPromotion.IsRecon()) ? iChange: 0);
		changeCanMoveAllTerrainCount((thisPromotion.CanMoveAllTerrain()) ? iChange: 0);
		changeCanMoveAfterAttackingCount((thisPromotion.IsCanMoveAfterAttacking()) ? iChange: 0);
		ChangeAirSweepCapableCount((thisPromotion.IsAirSweepCapable()) ? iChange: 0);
		ChangeEmbarkAbilityCount((thisPromotion.IsAllowsEmbarkation()) ? iChange: 0);
		ChangeRangeAttackIgnoreLOSCount((thisPromotion.IsRangeAttackIgnoreLOS()) ? iChange: 0);
		ChangeHealIfDefeatExcludeBarbariansCount((thisPromotion.IsHealIfDefeatExcludeBarbarians()) ? iChange: 0);
		changeHealOnPillageCount((thisPromotion.IsHealOnPillage()) ? iChange : 0);
		changeFreePillageMoveCount((thisPromotion.IsFreePillageMoves()) ? iChange: 0);
		ChangeEmbarkAllWaterCount((thisPromotion.IsEmbarkedAllWater()) ? iChange: 0);
#if defined(MOD_PROMOTIONS_DEEP_WATER_EMBARKATION)
		ChangeEmbarkDeepWaterCount((thisPromotion.IsEmbarkedDeepWater()) ? iChange: 0);
#endif
		ChangeCityAttackOnlyCount((thisPromotion.IsCityAttackSupport()) ? iChange: 0);
		ChangeCaptureDefeatedEnemyCount((thisPromotion.IsCaptureDefeatedEnemy()) ? iChange : 0);
#if defined(MOD_BALANCE_CORE)
		changeAOEDamageOnKill(thisPromotion.GetAOEDamageOnKill() *  iChange);
		changeAOEDamageOnPillage(thisPromotion.GetAOEDamageOnPillage() * iChange);
		changeAoEDamageOnMove(thisPromotion.GetAoEDamageOnMove() *  iChange);
		changePartialHealOnPillage(thisPromotion.GetPartialHealOnPillage() * iChange);
		changeSplashDamage(thisPromotion.GetSplashDamage() *  iChange);
		changeMultiAttackBonus(thisPromotion.GetMultiAttackBonus() *  iChange);
		changeLandAirDefenseValue(thisPromotion.GetLandAirDefenseValue() *  iChange);
		changeMountainsDoubleMoveCount((thisPromotion.IsMountainsDoubleMove()) ? iChange : 0);
		changeEmbarkFlatCostCount((thisPromotion.IsEmbarkFlatCost()) ? iChange : 0);
		changeDisembarkFlatCostCount((thisPromotion.IsDisembarkFlatCost()) ? iChange : 0);
		ChangeCaptureDefeatedEnemyChance((thisPromotion.GetCaptureDefeatedEnemyChance()) * iChange);
		ChangeBarbarianCombatBonus((thisPromotion.GetBarbarianCombatBonus()) * iChange);
		ChangeAdjacentEnemySapMovement((thisPromotion.GetAdjacentEnemySapMovement()) * iChange);
		ChangeGainsXPFromScouting((thisPromotion.IsGainsXPFromScouting()) ? iChange: 0);
		ChangeGainsXPFromPillaging((thisPromotion.IsGainsXPFromPillaging()) ? iChange : 0);
		ChangeGainsXPFromSpotting((thisPromotion.IsGainsXPFromSpotting()) ? iChange : 0);
		ChangeCannotBeCapturedCount((thisPromotion.CannotBeCaptured()) ? iChange: 0);
		ChangeForcedDamageValue((thisPromotion.ForcedDamageValue()) * iChange);
		ChangeChangeDamageValue((thisPromotion.ChangeDamageValue()) * iChange);
		ChangeMoraleBreakChance((thisPromotion.GetMoraleBreakChance()) * iChange);
		ChangeDamageAoEFortified((thisPromotion.GetDamageAoEFortified()) * iChange);
		ChangeWorkRateMod((thisPromotion.GetWorkRateMod()) * iChange);
		ChangeDamageReductionCityAssault((thisPromotion.GetDamageReductionCityAssault()) * iChange);
		if(thisPromotion.PromotionDuration() != 0)
		{
			if(bNewValue)
			{
				//SETS promotion duration, as we don't want to change it every time we get the promotion (this just stores the max length of the promotion)
				ChangePromotionDuration(eIndex, (thisPromotion.PromotionDuration() * iChange) - getPromotionDuration(eIndex));
				if(getPromotionDuration(eIndex) > 0)
				{
					SetTurnPromotionGained(eIndex, GC.getGame().getGameTurn());
				}
			}
			else
			{
				ChangePromotionDuration(NO_PROMOTION, 0);
			}
		}
		if(thisPromotion.NegatesPromotion() != NO_PROMOTION)
		{
			if(bNewValue)
			{
				SetNegatorPromotion(thisPromotion.NegatesPromotion());
			}
			else
			{
				SetNegatorPromotion(-1);
			}
		}
		ChangeIsStrongerDamaged(thisPromotion.IsStrongerDamaged() ? iChange : 0);
		ChangeIsFightWellDamaged(thisPromotion.IsFightWellDamaged() ? iChange : 0);
		ChangeGoodyHutYieldBonus(thisPromotion.GetGoodyHutYieldBonus() * iChange);
		ChangeReligiousPressureModifier(thisPromotion.GetReligiousPressureModifier() * iChange);
#endif
		ChangeCanHeavyChargeCount((thisPromotion.IsCanHeavyCharge()) ? iChange : 0);

		ChangeEmbarkExtraVisibility((thisPromotion.GetEmbarkExtraVisibility()) * iChange);
		ChangeEmbarkDefensiveModifier((thisPromotion.GetEmbarkDefenseModifier()) * iChange);
		ChangeCapitalDefenseModifier((thisPromotion.GetCapitalDefenseModifier()) * iChange);
		ChangeCapitalDefenseFalloff((thisPromotion.GetCapitalDefenseFalloff()) * iChange);
		ChangeCityAttackPlunderModifier((thisPromotion.GetCityAttackPlunderModifier()) *  iChange);
		ChangeReligiousStrengthLossRivalTerritory((thisPromotion.GetReligiousStrengthLossRivalTerritory()) *  iChange);
		ChangeTradeMissionInfluenceModifier((thisPromotion.GetTradeMissionInfluenceModifier()) * iChange);
		ChangeTradeMissionGoldModifier((thisPromotion.GetTradeMissionGoldModifier()) * iChange);
		ChangeDiploMissionInfluence((thisPromotion.GetDiploMissionInfluence()) * iChange);
		changeDropRange(thisPromotion.GetDropRange() * iChange);
		changeExtraVisibilityRange(thisPromotion.GetVisibilityChange() * iChange);
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
		changeExtraReconRange(thisPromotion.GetReconChange() * iChange);
#endif
		changeExtraMoves(thisPromotion.GetMovesChange() * iChange);
		changeExtraMoveDiscount(thisPromotion.GetMoveDiscountChange() * iChange);
		changeExtraNavalMoves(thisPromotion.GetExtraNavalMoves() * iChange);
		changeHPHealedIfDefeatEnemy(thisPromotion.GetHPHealedIfDefeatEnemy() * iChange);
		ChangeGoldenAgeValueFromKills(thisPromotion.GetGoldenAgeValueFromKills() * iChange);
		changeExtraWithdrawal(thisPromotion.GetExtraWithdrawal() * iChange);
		changeExtraRange(thisPromotion.GetRangeChange() * iChange);
		ChangeRangedAttackModifier(thisPromotion.GetRangedAttackModifier() * iChange);
		ChangeInterceptionCombatModifier(thisPromotion.GetInterceptionCombatModifier() * iChange);
		ChangeInterceptionDefenseDamageModifier(thisPromotion.GetInterceptionDefenseDamageModifier() * iChange);
		ChangeAirSweepCombatModifier(thisPromotion.GetAirSweepCombatModifier() * iChange);
		changeInterceptChance(thisPromotion.GetInterceptChanceChange() * iChange);
		changeExtraEvasion(thisPromotion.GetEvasionChange() * iChange);
		changeExtraEnemyHeal(thisPromotion.GetEnemyHealChange() * iChange);
		changeExtraNeutralHeal(thisPromotion.GetNeutralHealChange() * iChange);
		changeExtraFriendlyHeal(thisPromotion.GetFriendlyHealChange() * iChange);
		changeSameTileHeal(thisPromotion.GetSameTileHealChange() * iChange);
		changeAdjacentTileHeal(thisPromotion.GetAdjacentTileHealChange() * iChange);
		changeEnemyDamageChance(thisPromotion.GetEnemyDamageChance() * iChange);
		changeNeutralDamageChance(thisPromotion.GetNeutralDamageChance() * iChange);
		changeEnemyDamage(thisPromotion.GetEnemyDamage() * iChange);
		changeNeutralDamage(thisPromotion.GetNeutralDamage() * iChange);
		changeNearbyEnemyCombatMod(thisPromotion.GetNearbyEnemyCombatMod() * iChange);
		changeNearbyEnemyCombatRange(thisPromotion.GetNearbyEnemyCombatRange() * iChange);
		ChangeAdjacentModifier(thisPromotion.GetAdjacentMod() * iChange);
		ChangeNoAdjacentUnitModifier(thisPromotion.GetNoAdjacentUnitMod() * iChange);
		changeAttackModifier(thisPromotion.GetAttackMod() * iChange);
		changeDefenseModifier(thisPromotion.GetDefenseMod() * iChange);
		changeGroundAttackDamage(thisPromotion.GetGroundAttackDamage() * iChange);
		changeExtraCombatPercent(thisPromotion.GetCombatPercent() * iChange);
		changeExtraCityAttackPercent(thisPromotion.GetCityAttackPercent() * iChange);
		changeExtraCityDefensePercent(thisPromotion.GetCityDefensePercent() * iChange);
		changeExtraRangedDefenseModifier(thisPromotion.GetRangedDefenseMod() * iChange);
		changeExtraHillsAttackPercent(thisPromotion.GetHillsAttackPercent() * iChange);
		changeExtraHillsDefensePercent(thisPromotion.GetHillsDefensePercent() * iChange);
		changeExtraOpenAttackPercent(thisPromotion.GetOpenAttackPercent() * iChange);
		changeExtraOpenRangedAttackMod(thisPromotion.GetOpenRangedAttackMod() * iChange);
		changeExtraRoughAttackPercent(thisPromotion.GetRoughAttackPercent() * iChange);
		changeExtraRoughRangedAttackMod(thisPromotion.GetRoughRangedAttackMod() * iChange);
		changeExtraAttackFortifiedMod(thisPromotion.GetAttackFortifiedMod() * iChange);
		changeExtraAttackWoundedMod(thisPromotion.GetAttackWoundedMod() * iChange);
		changeExtraAttackFullyHealedMod(thisPromotion.GetAttackFullyHealedMod() * iChange);
		changeExtraAttackAboveHealthMod(thisPromotion.GetAttackAboveHealthMod() * iChange);
		changeExtraAttackBelowHealthMod(thisPromotion.GetAttackBelowHealthMod() * iChange);
		ChangeRangedFlankAttackCount(thisPromotion.IsRangedFlankAttack() ? iChange : 0);
		ChangeFlankPower(thisPromotion.GetExtraFlankPower() * iChange);
		ChangeFlankAttackModifier(thisPromotion.GetFlankAttackModifier() * iChange);
		changeExtraOpenDefensePercent(thisPromotion.GetOpenDefensePercent() * iChange);
		changeExtraRoughDefensePercent(thisPromotion.GetRoughDefensePercent() * iChange);
		changeExtraOpenFromPercent(thisPromotion.GetOpenFromPercent() * iChange);
		changeExtraRoughFromPercent(thisPromotion.GetRoughFromPercent() * iChange);
		changeExtraAttacks(thisPromotion.GetExtraAttacks() * iChange);
		ChangeNumInterceptions(thisPromotion.GetNumInterceptionChange() * iChange);

#if defined(MOD_BALANCE_CORE) // JJ: New
		ChangeExtraAirInterceptRange(thisPromotion.GetAirInterceptRangeChange() * iChange);
#endif

		ChangeGreatGeneralCount(thisPromotion.IsGreatGeneral() ? iChange: 0);
		ChangeGreatAdmiralCount(thisPromotion.IsGreatAdmiral() ? iChange: 0);

		ChangeAuraRangeChange(thisPromotion.GetAuraRangeChange() * iChange);
		ChangeAuraEffectChange(thisPromotion.GetAuraEffectChange() * iChange);
		ChangeNumRepairCharges(thisPromotion.GetNumRepairCharges() * iChange);
		ChangeMilitaryCapChange(thisPromotion.GetMilitaryCapChange() * iChange);

		changeGreatGeneralModifier(thisPromotion.GetGreatGeneralModifier() * iChange);
		ChangeGreatGeneralReceivesMovementCount(thisPromotion.IsGreatGeneralReceivesMovement() ? iChange: 0);
		ChangeGreatGeneralCombatModifier(thisPromotion.GetGreatGeneralCombatModifier() * iChange);

		ChangeIgnoreGreatGeneralBenefitCount(thisPromotion.IsIgnoreGreatGeneralBenefit() ? iChange: 0);
		ChangeIgnoreZOCCount(thisPromotion.IsIgnoreZOC() ? iChange: 0);

		changeNoSupply(thisPromotion.IsNoSupply() ? iChange : 0);

		changeMaxHitPointsChange(thisPromotion.GetMaxHitPointsChange() * iChange);
		changeMaxHitPointsModifier(thisPromotion.GetMaxHitPointsModifier() * iChange);

		ChangeSapperCount((thisPromotion.IsSapper() ? iChange: 0));

		changeFriendlyLandsModifier(thisPromotion.GetFriendlyLandsModifier() * iChange);
		changeFriendlyLandsAttackModifier(thisPromotion.GetFriendlyLandsAttackModifier() * iChange);
		changeOutsideFriendlyLandsModifier(thisPromotion.GetOutsideFriendlyLandsModifier() * iChange);
		changeUpgradeDiscount(thisPromotion.GetUpgradeDiscount() * iChange);
		changeExperiencePercent(thisPromotion.GetExperiencePercent() * iChange);
		changeCargoSpace(thisPromotion.GetCargoChange() * iChange);

#if defined(MOD_PROMOTIONS_UNIT_NAMING)
		if (MOD_PROMOTIONS_UNIT_NAMING) {
			if (thisPromotion.IsUnitNaming(getUnitType())) {
				thisPromotion.GetUnitName(getUnitType(), m_strUnitName);
			}
		}
#endif

		for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			changeExtraTerrainAttackPercent(((TerrainTypes)iI), (thisPromotion.GetTerrainAttackPercent(iI) * iChange));
			changeExtraTerrainDefensePercent(((TerrainTypes)iI), (thisPromotion.GetTerrainDefensePercent(iI) * iChange));
			changeIgnoreTerrainCostInCount(((TerrainTypes)iI), ((thisPromotion.GetIgnoreTerrainCostIn(iI)) ? iChange : 0));
			changeIgnoreTerrainCostFromCount(((TerrainTypes)iI), ((thisPromotion.GetIgnoreTerrainCostFrom(iI)) ? iChange : 0));
			changeTerrainDoubleMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainDoubleMove(iI)) ? iChange : 0));
#if defined(MOD_PROMOTIONS_HALF_MOVE)
			changeTerrainHalfMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainHalfMove(iI)) ? iChange : 0));
			changeTerrainExtraMoveCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainExtraMove(iI)) ? iChange : 0));
#endif
#if defined(MOD_BALANCE_CORE)
			changeTerrainDoubleHeal(((TerrainTypes)iI), ((thisPromotion.GetTerrainDoubleHeal(iI)) ? iChange : 0));		
#endif
			changeTerrainImpassableCount(((TerrainTypes)iI), ((thisPromotion.GetTerrainImpassable(iI)) ? iChange : 0));
		}

		for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			changeExtraFeatureAttackPercent(((FeatureTypes)iI), (thisPromotion.GetFeatureAttackPercent(iI) * iChange));
			changeExtraFeatureDefensePercent(((FeatureTypes)iI), (thisPromotion.GetFeatureDefensePercent(iI) * iChange));
			changeIgnoreFeatureCostInCount(((FeatureTypes)iI), ((thisPromotion.GetIgnoreFeatureCostIn(iI)) ? iChange : 0));
			changeIgnoreFeatureCostFromCount(((FeatureTypes)iI), ((thisPromotion.GetIgnoreFeatureCostFrom(iI)) ? iChange : 0));
			changeFeatureDoubleMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureDoubleMove(iI)) ? iChange : 0));
#if defined(MOD_PROMOTIONS_HALF_MOVE)
			changeFeatureHalfMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureHalfMove(iI)) ? iChange : 0));
			changeFeatureExtraMoveCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureExtraMove(iI)) ? iChange : 0));
#endif
#if defined(MOD_BALANCE_CORE)
			changeFeatureDoubleHeal(((FeatureTypes)iI), ((thisPromotion.GetFeatureDoubleHeal(iI)) ? iChange : 0));		
#endif
			changeFeatureImpassableCount(((FeatureTypes)iI), ((thisPromotion.GetFeatureImpassable(iI)) ? iChange : 0));
		}

		for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			const YieldTypes eYield = static_cast<YieldTypes>(iI);
			SetYieldModifier(eYield, (thisPromotion.GetYieldModifier(iI) * iChange));
			SetYieldChange(eYield, (thisPromotion.GetYieldChange(iI) * iChange));
			SetGarrisonYieldChange(eYield, (thisPromotion.GetGarrisonYield(iI) * iChange));
			SetFortificationYieldChange(eYield, (thisPromotion.GetFortificationYield(iI) * iChange));
			changeYieldFromKills(eYield, (thisPromotion.GetYieldFromKills(iI) * iChange));
			changeYieldFromBarbarianKills(eYield, (thisPromotion.GetYieldFromBarbarianKills(iI) * iChange));
			changeYieldFromScouting(eYield, (thisPromotion.GetYieldFromScouting(iI) * iChange));
			{
				std::pair<int, int> pillageYields = thisPromotion.GetYieldFromPillage(eYield);
				pillageYields.first *= iChange;
				pillageYields.second *= iChange;
				changeYieldFromPillage(eYield, pillageYields);
			}
#if defined(MOD_BALANCE_CORE)
			if (bNewValue && !IsPromotionEverObtained(eIndex))
			{
				if (thisPromotion.GetInstantYields(iI).first > 0)
				{
					CvCity* pCity = getOriginCity();
					if (pCity != NULL)
					{
						GET_PLAYER(getOwner()).doInstantYield(INSTANT_YIELD_TYPE_PROMOTION_OBTAINED, false, NO_GREATPERSON, NO_BUILDING, thisPromotion.GetInstantYields(iI).first, thisPromotion.GetInstantYields(iI).second, NO_PLAYER, NULL, false, pCity, false, true, false, eYield, this);
					}
				}
			}
#endif
		}

		for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			changeExtraUnitCombatModifier(((UnitCombatTypes)iI), (thisPromotion.GetUnitCombatModifierPercent(iI) * iChange));
#if defined(MOD_BALANCE_CORE)
			changeCombatModPerAdjacentUnitCombatModifier(((UnitCombatTypes)iI), (thisPromotion.GetCombatModPerAdjacentUnitCombatModifierPercent(iI) * iChange));
			changeCombatModPerAdjacentUnitCombatAttackMod(((UnitCombatTypes)iI), (thisPromotion.GetCombatModPerAdjacentUnitCombatAttackModifier(iI) * iChange));
			changeCombatModPerAdjacentUnitCombatDefenseMod(((UnitCombatTypes)iI), (thisPromotion.GetCombatModPerAdjacentUnitCombatDefenseModifier(iI) * iChange));
#endif
		}

		for(iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iI);
			if(!pkUnitClassInfo)
			{
				continue;
			}

			changeUnitClassModifier(((UnitClassTypes)iI), (thisPromotion.GetUnitClassModifierPercent(iI) * iChange));
			changeUnitClassAttackMod(((UnitClassTypes)iI), (thisPromotion.GetUnitClassAttackModifier(iI) * iChange));
			changeUnitClassDefenseMod(((UnitClassTypes)iI), (thisPromotion.GetUnitClassDefenseModifier(iI) * iChange));
		}

		for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			changeExtraDomainModifier(((DomainTypes)iI), (thisPromotion.GetDomainModifierPercent(iI) * iChange));
			changeExtraDomainAttack(((DomainTypes)iI), (thisPromotion.GetDomainAttackPercent(iI) * iChange));
			changeExtraDomainDefense(((DomainTypes)iI), (thisPromotion.GetDomainDefensePercent(iI) * iChange));
		}
		if (getGiveDomain() == NO_DOMAIN && thisPromotion.GetGiveDomain() != NO_DOMAIN)
		{
			ChangeGiveDomain((DomainTypes)thisPromotion.GetGiveDomain());
		}
		if (getConvertDomain() == NO_DOMAIN && thisPromotion.GetConvertDomain() != NO_DOMAIN)
		{
			ChangeConvertDomain((DomainTypes)thisPromotion.GetConvertDomain());
		}
		if (getConvertDomainUnitType() == NO_UNIT && thisPromotion.GetConvertDomainUnit() != NO_UNIT)
		{
			ChangeConvertDomainUnit((UnitTypes)thisPromotion.GetConvertDomainUnit());
		}

		if(IsSelected())
		{
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
			if (thisPromotion.GetEmbarkExtraVisibility() || thisPromotion.IsNoRevealMap() || thisPromotion.GetVisibilityChange())
				GC.getMap().updateDeferredFog();
		}

		if (MOD_API_ACHIEVEMENTS)
		{
			PromotionTypes eBuffaloChest =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_CHEST", true /*bHideAssert*/);
			PromotionTypes eBuffaloLoins =(PromotionTypes) GC.getInfoTypeForString("PROMOTION_BUFFALO_LOINS", true /*bHideAssert*/);

			const PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
			if (getOwner() == eActivePlayer && ((eIndex == eBuffaloChest && isHasPromotion(eBuffaloLoins)) || (eIndex == eBuffaloLoins && isHasPromotion(eBuffaloChest))))
			{
				gDLL->UnlockAchievement(ACHIEVEMENT_XP2_27);
			}
		}

#if defined(MOD_BALANCE_CORE)
		// Set promotion IsEverObtained to true. This should be the last statement.
		if (bNewValue && !IsPromotionEverObtained(eIndex))
		{
			SetPromotionEverObtained(eIndex, bNewValue);
		}
#endif

		//promotion changes may invalidate some caches
		GET_PLAYER(getOwner()).UpdateAreaEffectUnit(this);
	}
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitCount() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetGroupSize();
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitsAlive() const
{
	VALIDATE_OBJECT
	return getSubUnitsAlive(getDamage());
}


//	--------------------------------------------------------------------------------
int CvUnit::getSubUnitsAlive(int iDamage) const
{
	VALIDATE_OBJECT
	if(iDamage >= GetMaxHitPoints())
	{
		return 0;
	}
	else
	{
		return std::max(1, (((getUnitInfo().GetGroupSize() * (GetMaxHitPoints() - iDamage)) + (GetMaxHitPoints() / ((getUnitInfo().GetGroupSize() * 2) + 1))) / GetMaxHitPoints()));
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanPushOutUnitHere(const CvPlot& pushPlot) const
{
	return GetPotentialUnitToPushOut(pushPlot) != NULL;
}

//this should only be used by AI; includes sanity checks (humans have GetPotentialUnitToSwapWith)
CvUnit* CvUnit::GetPotentialUnitToPushOut(const CvPlot& pushPlot, CvPlot** ppToPlot) const
{
	//sanity
	if (ppToPlot)
		*ppToPlot = NULL;

	//this is AI only, humans have to do it manually
	if (isHuman())
		return NULL;

	//no problem if we can stack
	if (CanStackUnitAtPlot(&pushPlot))
		return NULL;

	if (!isNativeDomain(&pushPlot))
		return NULL;

	const IDInfo* pUnitNode = NULL;
	CvUnit* pLoopUnit = NULL;
	pUnitNode = pushPlot.headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::GetPlayerUnit(*pUnitNode);
		pUnitNode = pushPlot.nextUnitNode(pUnitNode);

		//this should be the blocking unit
		if (pLoopUnit->IsCombatUnit() && pLoopUnit->getDomainType() == getDomainType())
		{
			//is it idle right now?
			if (pLoopUnit->canMove() && pLoopUnit->GetNumEnemyUnitsAdjacent()==0 && !pLoopUnit->shouldHeal(false))
			{
				//make sure we're not getting the pushed unit killed
				int iDangerLimit = pLoopUnit->GetCurrHitPoints();
				int iLeastDanger = INT_MAX;

				//does it have a free plot
				CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(&pushPlot);
				for (int i = 0; i < 6; i++)
				{
					CvPlot* pNeighbor = aNeighbors[i];
					if (!pNeighbor)
						continue;

					//valid plot
					if (pLoopUnit->isNativeDomain(pNeighbor) && pLoopUnit->canMoveInto(*pNeighbor, CvUnit::MOVEFLAG_DESTINATION))
					{
						int iUnitDanger = pLoopUnit->GetDanger(pNeighbor);

						//swap needed / possible
						if (at(pNeighbor->getX(), pNeighbor->getY()) && pNeighbor->GetNumCombatUnits() == 1 && pLoopUnit->canMoveInto(*pNeighbor, CvUnit::MOVEFLAG_DESTINATION | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF))
						{
							//often the incoming unit is retreating from the frontline, so ideally we want to swap the neighboring unit right in
							if (iUnitDanger < iDangerLimit)
							{
								//todo: check the distance to the next enemy
								//todo: also might want to do a ranged opportunity attack before moving pLoopUnit ...
								if (!pLoopUnit->IsCanAttackRanged() || TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pLoopUnit, pNeighbor, true, false).size() > 0)
								{
									if (ppToPlot)
										*ppToPlot = pNeighbor;
									return pLoopUnit;
								}
							}
						}

						//else go to the lowest danger plot ... todo: should we try to use the other unit as cover?
						if (iUnitDanger < iDangerLimit && iUnitDanger < iLeastDanger)
						{
							iLeastDanger = iUnitDanger;
							if (ppToPlot)
								*ppToPlot = pNeighbor;
						}
					}
				}

				if (iLeastDanger < INT_MAX)
					return pLoopUnit;
			}
		}
	}

	return NULL;
}


//move into the plot which is assumed to be blocked by another unit
//push the other unit to a neighboring plot
bool CvUnit::PushBlockingUnitOutOfPlot(const CvPlot & atPlot)
{
	CvPlot* pToPlot = NULL;
	CvUnit* pBlock = GetPotentialUnitToPushOut(atPlot, &pToPlot);
	if (!pBlock || !pToPlot)
		return false;

	//special: swap into our current plot
	if (at(pToPlot->getX(), pToPlot->getY()))
	{
		pBlock->SetActivityType(ACTIVITY_AWAKE);
		pBlock->PushMission(CvTypes::getMISSION_SWAP_UNITS(), pToPlot->getX(), pToPlot->getY());
		return true;
	}
	else
	{
		pBlock->SetActivityType(ACTIVITY_AWAKE);
		pBlock->PushMission(CvTypes::getMISSION_MOVE_TO(), pToPlot->getX(), pToPlot->getY());
		PushMission(CvTypes::getMISSION_MOVE_TO(), atPlot.getX(), atPlot.getY());
		return true;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanSwapWithUnitHere(const CvPlot& swapPlot) const
{
	return GetPotentialUnitToSwapWith(swapPlot) != NULL;
}

//method for human players, no sanity checks
CvUnit* CvUnit::GetPotentialUnitToSwapWith(const CvPlot & swapPlot) const
{
	//AI shouldn't swap into a frontline plot
	if (!isHuman() && swapPlot.GetNumEnemyUnitsAdjacent(getTeam(), getDomainType()) > 0)
		return NULL;

	if (getDomainType() == DOMAIN_LAND || getDomainType() == DOMAIN_SEA)
	{
		if (IsCombatUnit() && canEnterTerrain(swapPlot, CvUnit::MOVEFLAG_DESTINATION))
		{
			// Can I get there this turn?
			SPathFinderUserData data(this, CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF, 1);

			//need to be careful here, this method may be called from GUI and gamecore, this can lead to a deadlock
			SPath path = GC.GetPathFinder().GetPath(plot(), &swapPlot, data);
			if (!!path)
			{
				CvPlot* pEndTurnPlot = PathHelpers::GetPathEndFirstTurnPlot(path);
				if (pEndTurnPlot == &swapPlot)
				{
					if (!CanStackUnitAtPlot(&swapPlot))
					{
						const IDInfo* pUnitNode = NULL;
						CvUnit* pLoopUnit = NULL;
						pUnitNode = swapPlot.headUnitNode();
						while (pUnitNode != NULL)
						{
							pLoopUnit = ::GetPlayerUnit(*pUnitNode);
							pUnitNode = swapPlot.nextUnitNode(pUnitNode);

							// A unit can't swap with itself (slewis)
							if (!pLoopUnit || pLoopUnit == this)
								continue;

							//can't swap with alternate domains (don't need to).
							if (pLoopUnit->getDomainType() != getDomainType())
								continue;

							// Make sure units belong to the same player
							if (pLoopUnit && pLoopUnit->getOwner() == getOwner())
							{
								if (pLoopUnit->IsCombatUnit() && pLoopUnit->ReadyToSwap())
								{
									if (pLoopUnit->canEnterTerrain(*plot(), CvUnit::MOVEFLAG_DESTINATION) && pLoopUnit->canEnterTerritory(plot()->getTeam(),true) && pLoopUnit->ReadyToSwap())
									{
										// Can the unit I am swapping with get to me this turn?
										SPathFinderUserData data(pLoopUnit, CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF, 1);
										SPath path = GC.GetPathFinder().GetPath(pLoopUnit->plot(), plot(), data);
										if (!!path)
										{
											CvPlot* pPathEndTurnPlot = PathHelpers::GetPathEndFirstTurnPlot(path);
											if (pPathEndTurnPlot == plot())
												return pLoopUnit;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

//	--------------------------------------------------------------------------------
template<typename Unit, typename Visitor>
void CvUnit::Serialize(Unit& unit, Visitor& visitor)
{
	const bool bLoading = visitor.isLoading();
	const bool bSaving = visitor.isSaving();

	// FIXME - Values in this chunk were formerly FAutoVariables. Remove any that shouldn't be saved.
	visitor(unit.m_eOwner);
	visitor(unit.m_eOriginalOwner);
	visitor(unit.m_eGiftedByPlayer);
	visitor(unit.m_iX);
	visitor(unit.m_iY);
	visitor(unit.m_iID);
	visitor(unit.m_iDamage);
	visitor(unit.m_iMoves);
	visitor(unit.m_bIsLinked);
	visitor(unit.m_bIsLinkedLeader);
	visitor(unit.m_bIsGrouped);
	visitor(unit.m_iLinkedMaxMoves);
	visitor(unit.m_iLinkedLeaderID);
	visitor(unit.m_LinkedUnitIDs);
	visitor(unit.m_iSquadNumber);
	visitor(unit.m_iSquadDestinationX);
	visitor(unit.m_iSquadDestinationY);
	visitor(unit.m_SquadEndMovementType);
	visitor(unit.m_iArmyId);
	visitor(unit.m_iBaseCombat);
	visitor(unit.m_iBaseRangedCombat);
	visitor(unit.m_iHotKeyNumber);
	visitor(unit.m_iDeployFromOperationTurn);
	visitor(unit.m_iLastMoveTurn);
	visitor(unit.m_iReconX);
	visitor(unit.m_iReconY);
	visitor(unit.m_iReconCount);
	visitor(unit.m_iGameTurnCreated);
	visitor(unit.m_bImmobile);
	visitor(unit.m_iExperienceTimes100);
	visitor(unit.m_iLevel);
	visitor(unit.m_iCargo);
	visitor(unit.m_iCargoCapacity);
	visitor(unit.m_iAttackPlotX);
	visitor(unit.m_iAttackPlotY);
	visitor(unit.m_iCombatTimer);
	visitor(unit.m_bMovedThisTurn);
	visitor(unit.m_bHasWithdrawnThisTurn);
	visitor(unit.m_bFortified);
	visitor(unit.m_iBlitzCount);
	visitor(unit.m_iAmphibCount);
	visitor(unit.m_iRiverCrossingNoPenaltyCount);
	visitor(unit.m_iEnemyRouteCount);
	visitor(unit.m_iRivalTerritoryCount);
	visitor(unit.m_iIsSlowInEnemyLandCount);
	visitor(unit.m_iRangeAttackIgnoreLOSCount);
	visitor(unit.m_iCityAttackOnlyCount);
	visitor(unit.m_iCaptureDefeatedEnemyCount);
	visitor(unit.m_iOriginCity);
	visitor(unit.m_iCannotBeCapturedCount);
	visitor(unit.m_iForcedDamage);
	visitor(unit.m_iChangeDamage);
	visitor(unit.m_PromotionDuration);
	visitor(unit.m_TurnPromotionGained);
	visitor(unit.m_iRangedSupportFireCount);
	visitor(unit.m_iAlwaysHealCount);
	visitor(unit.m_iHealOutsideFriendlyCount);
	visitor(unit.m_iHillsDoubleMoveCount);
	visitor(unit.m_iRiverDoubleMoveCount);
	visitor(unit.m_iMountainsDoubleMoveCount);
	visitor(unit.m_iEmbarkFlatCostCount);
	visitor(unit.m_iDisembarkFlatCostCount);
	visitor(unit.m_iAOEDamageOnKill);
	visitor(unit.m_iAOEDamageOnPillage);
	visitor(unit.m_iAoEDamageOnMove);
	visitor(unit.m_iPartialHealOnPillage);
	visitor(unit.m_iSplashDamage);
	visitor(unit.m_iMultiAttackBonus);
	visitor(unit.m_iLandAirDefenseValue);
	visitor(unit.m_iExtraVisibilityRange);
	visitor(unit.m_iExtraReconRange);
	visitor(unit.m_iExtraMoves);
	visitor(unit.m_iExtraMoveDiscount);
	visitor(unit.m_iExtraRange);
	visitor(unit.m_iInterceptChance);
	visitor(unit.m_iExtraEvasion);
	visitor(unit.m_iExtraWithdrawal);
	visitor(unit.m_eUnitContract);
	visitor(unit.m_iNegatorPromotion);
	visitor(unit.m_bIsNoMaintenance);
	visitor(unit.m_iExtraEnemyHeal);
	visitor(unit.m_iExtraNeutralHeal);
	visitor(unit.m_iExtraFriendlyHeal);
	visitor(unit.m_iEnemyDamageChance);
	visitor(unit.m_iNeutralDamageChance);
	visitor(unit.m_iEnemyDamage);
	visitor(unit.m_iNeutralDamage);
	visitor(unit.m_iNearbyEnemyCombatMod);
	visitor(unit.m_iNearbyEnemyCombatRange);
	visitor(unit.m_iSameTileHeal);
	visitor(unit.m_iAdjacentTileHeal);
	visitor(unit.m_iAdjacentModifier);
	visitor(unit.m_iNoAdjacentUnitModifier);
	visitor(unit.m_iRangedAttackModifier);
	visitor(unit.m_iInterceptionCombatModifier);
	visitor(unit.m_iInterceptionDefenseDamageModifier);
	visitor(unit.m_iAirSweepCombatModifier);
	visitor(unit.m_iAttackModifier);
	visitor(unit.m_iDefenseModifier);
	visitor(unit.m_iGroundAttackDamage);
	visitor(unit.m_iExtraCombatPercent);
	visitor(unit.m_iExtraCityAttackPercent);
	visitor(unit.m_iExtraCityDefensePercent);
	visitor(unit.m_iExtraRangedDefenseModifier);
	visitor(unit.m_iExtraHillsAttackPercent);
	visitor(unit.m_iExtraHillsDefensePercent);
	visitor(unit.m_iExtraOpenAttackPercent);
	visitor(unit.m_iExtraOpenRangedAttackMod);
	visitor(unit.m_iExtraRoughAttackPercent);
	visitor(unit.m_iExtraRoughRangedAttackMod);
	visitor(unit.m_iExtraAttackFortifiedMod);
	visitor(unit.m_iExtraAttackWoundedMod);
	visitor(unit.m_iExtraFullyHealedMod);
	visitor(unit.m_iExtraAttackAboveHealthMod);
	visitor(unit.m_iExtraAttackBelowHealthMod);
	visitor(unit.m_iRangedFlankAttack);
	visitor(unit.m_iFlankPower);
	visitor(unit.m_iFlankAttackModifier);
	visitor(unit.m_iExtraOpenDefensePercent);
	visitor(unit.m_iExtraRoughDefensePercent);
	visitor(unit.m_iExtraOpenFromPercent);
	visitor(unit.m_iExtraRoughFromPercent);
	visitor(unit.m_iPillageChange);
	visitor(unit.m_iUpgradeDiscount);
	visitor(unit.m_iExperiencePercent);
	visitor(unit.m_iDropRange);
	visitor(unit.m_iAirSweepCapableCount);
	visitor(unit.m_iExtraNavalMoves);
	visitor(unit.m_eFacingDirection);
	visitor(unit.m_iIgnoreTerrainCostCount);
	visitor(unit.m_iIgnoreTerrainDamageCount);
	visitor(unit.m_iIgnoreFeatureDamageCount);
	visitor(unit.m_iExtraTerrainDamageCount);
	visitor(unit.m_iExtraFeatureDamageCount);
	visitor(unit.m_iNearbyImprovementCombatBonus);
	visitor(unit.m_iNearbyImprovementBonusRange);
	visitor(unit.m_eCombatBonusImprovement);
	visitor(unit.m_iNearbyUnitClassBonus);
	visitor(unit.m_iNearbyUnitClassBonusRange);
	visitor(unit.m_iCombatBonusFromNearbyUnitClass);
	visitor(unit.m_iNearbyPromotion);
	visitor(unit.m_iNearbyUnitPromotionRange);
	visitor(unit.m_iNearbyCityCombatMod);
	visitor(unit.m_iNearbyFriendlyCityCombatMod);
	visitor(unit.m_iNearbyEnemyCityCombatMod);
	visitor(unit.m_iPillageBonusStrengthPercent);
	visitor(unit.m_iStackedGreatGeneralExperience);
	visitor(unit.m_iWonderProductionModifier);
	visitor(unit.m_iUnitProductionModifier);
	visitor(unit.m_iNearbyEnemyDamage);
	visitor(unit.m_iAdjacentCityDefenseMod);
	visitor(unit.m_iGGGAXPPercent);
	visitor(unit.m_iGiveCombatMod);
	visitor(unit.m_iGiveHPIfEnemyKilled);
	visitor(unit.m_iGiveExperiencePercent);
	visitor(unit.m_iGiveOutsideFriendlyLandsModifier);
	visitor(unit.m_eGiveDomain);
	visitor(unit.m_iGiveExtraAttacks);
	visitor(unit.m_iGiveDefenseMod);
	visitor(unit.m_iGiveInvisibility);
	visitor(unit.m_bGiveOnlyOnStartingTurn);
	visitor(unit.m_iConvertUnit);
	visitor(unit.m_eConvertDomain);
	visitor(unit.m_eConvertDomainUnit);
	visitor(unit.m_iConvertEnemyUnitToBarbarian);
	visitor(unit.m_bConvertOnFullHP);
	visitor(unit.m_bConvertOnDamage);
	visitor(unit.m_iDamageThreshold);
	visitor(unit.m_eConvertDamageOrFullHPUnit);
	visitor(unit.m_iNumberOfCultureBombs);
	visitor(unit.m_iNearbyHealEnemyTerritory);
	visitor(unit.m_iNearbyHealNeutralTerritory);
	visitor(unit.m_iNearbyHealFriendlyTerritory);
	visitor(unit.m_iCanCrossMountainsCount);
	visitor(unit.m_iCanCrossOceansCount);
	visitor(unit.m_iCanCrossIceCount);
	visitor(unit.m_iNumTilesRevealedThisTurn);
	visitor(unit.m_bSpottedEnemy);
	visitor(unit.m_iGainsXPFromScouting);
	visitor(unit.m_iGainsXPFromPillaging);
	visitor(unit.m_iGainsXPFromSpotting);
	visitor(unit.m_iCaptureDefeatedEnemyChance);
	visitor(unit.m_iBarbCombatBonus);
	visitor(unit.m_iAdjacentEnemySapMovement);
	visitor(unit.m_iGGFromBarbariansCount);
	visitor(unit.m_iAuraRangeChange);
	visitor(unit.m_iAuraEffectChange);
	visitor(unit.m_iNumRepairCharges);
	visitor(unit.m_iMilitaryCapChange);
	visitor(unit.m_iRoughTerrainEndsTurnCount);
	visitor(unit.m_iCapturedUnitsConscriptedCount);
	visitor(unit.m_iEmbarkAbilityCount);
	visitor(unit.m_iHoveringUnitCount);
	visitor(unit.m_iFlatMovementCostCount);
	visitor(unit.m_iCanMoveImpassableCount);
	visitor(unit.m_iOnlyDefensiveCount);
	visitor(unit.m_iNoAttackInOceanCount);
	visitor(unit.m_iNoDefensiveBonusCount);
	visitor(unit.m_iNoCaptureCount);
	visitor(unit.m_iNukeImmuneCount);
	visitor(unit.m_iHiddenNationalityCount);
	visitor(unit.m_iAlwaysHostileCount);
	visitor(unit.m_iNoRevealMapCount);
	visitor(unit.m_iCanMoveAllTerrainCount);
	visitor(unit.m_iCanMoveAfterAttackingCount);
	visitor(unit.m_iFreePillageMoveCount);
	visitor(unit.m_iHealOnPillageCount);
	visitor(unit.m_iHPHealedIfDefeatEnemy);
	visitor(unit.m_iGoldenAgeValueFromKills);
	visitor(unit.m_iTacticalAIPlotX);
	visitor(unit.m_iTacticalAIPlotY);
	visitor(unit.m_iGarrisonCityID);
	visitor(unit.m_iNumAttacks);
	visitor(unit.m_iAttacksMade);
	visitor(unit.m_iGreatGeneralCount);
	visitor(unit.m_iGreatAdmiralCount);
	visitor(unit.m_iGreatGeneralModifier);
	visitor(unit.m_iGreatGeneralReceivesMovementCount);
	visitor(unit.m_iGreatGeneralCombatModifier);
	visitor(unit.m_iIgnoreGreatGeneralBenefit);
	visitor(unit.m_iIgnoreZOC);
	visitor(unit.m_iNoSupply);
	visitor(unit.m_iMaxHitPointsChange);
	visitor(unit.m_iMaxHitPointsModifier);
	visitor(unit.m_iFriendlyLandsModifier);
	visitor(unit.m_iFriendlyLandsAttackModifier);
	visitor(unit.m_iOutsideFriendlyLandsModifier);
	visitor(unit.m_iHealIfDefeatExcludeBarbariansCount);
	visitor(unit.m_iNumInterceptions);
	visitor(unit.m_iExtraAirInterceptRange);
	visitor(unit.m_iMadeInterceptionCount);
	visitor(unit.m_iEverSelectedCount);
	visitor(unit.m_iSapperCount);
	visitor(unit.m_iCanHeavyCharge);
	visitor(unit.m_iStrongerDamaged);
	visitor(unit.m_iFightWellDamaged);
	visitor(unit.m_iCanMoraleBreak);
	visitor(unit.m_iDamageAoEFortified);
	visitor(unit.m_iWorkRateMod);
	visitor(unit.m_iDamageReductionCityAssault);
	visitor(unit.m_iGoodyHutYieldBonus);
	visitor(unit.m_iReligiousPressureModifier);
	visitor(unit.m_iNumExoticGoods);
	visitor(unit.m_bPromotionReady);
	visitor(unit.m_bDeathDelay);
	visitor(unit.m_bCombatFocus);
	visitor(unit.m_bInfoBarDirty);
	visitor(unit.m_bNotConverting);
	visitor(unit.m_bAirCombat);
	visitor(unit.m_bEmbarked);
	visitor(unit.m_bPromotedFromGoody);
	visitor(unit.m_bAITurnProcessed);
	visitor(unit.m_iDamageTakenThisTurn);
	visitor(unit.m_iDamageTakenLastTurn);
	visitor(unit.m_eCapturingPlayer);
	visitor(unit.m_bCapturedAsIs);
	visitor(unit.m_bCapturedAsConscript);
	visitor(unit.m_eLeaderUnitType);
	visitor(unit.m_eInvisibleType);
	visitor(unit.m_eSeeInvisibleType);
	visitor(unit.m_eGreatPeopleDirectiveType);
	visitor(unit.m_strScriptData);
	visitor(unit.m_iScenarioData);
	visitor(unit.m_iBuilderStrength);
	visitor(unit.m_ignoreTerrainCostInCount);
	visitor(unit.m_ignoreTerrainCostFromCount);
	visitor(unit.m_ignoreFeatureCostInCount);
	visitor(unit.m_ignoreFeatureCostFromCount);
	visitor(unit.m_terrainDoubleMoveCount);
	visitor(unit.m_featureDoubleMoveCount);
	visitor(unit.m_terrainHalfMoveCount);
	visitor(unit.m_featureHalfMoveCount);
	visitor(unit.m_terrainExtraMoveCount);
	visitor(unit.m_featureExtraMoveCount);
	visitor(unit.m_terrainDoubleHeal);
	visitor(unit.m_featureDoubleHeal);
	visitor(unit.m_terrainImpassableCount);
	visitor(unit.m_featureImpassableCount);
	visitor(unit.m_extraTerrainAttackPercent);
	visitor(unit.m_extraTerrainDefensePercent);
	visitor(unit.m_extraFeatureAttackPercent);
	visitor(unit.m_extraFeatureDefensePercent);
	visitor(unit.m_extraUnitClassAttackMod);
	visitor(unit.m_extraUnitClassDefenseMod);
	visitor(unit.m_aiNumTimesAttackedThisTurn);
	visitor(unit.m_yieldFromScouting);
	visitor(unit.m_yieldFromKills);
	visitor(unit.m_yieldFromBarbarianKills);
	visitor(unit.m_extraUnitCombatModifier);
	visitor(unit.m_unitClassModifier);
	visitor(unit.m_iCombatModPerAdjacentUnitCombatModifier);
	visitor(unit.m_iCombatModPerAdjacentUnitCombatAttackMod);
	visitor(unit.m_iCombatModPerAdjacentUnitCombatDefenseMod);
	visitor(unit.m_yieldFromPillage);
	visitor(unit.m_iMissionTimer);
	visitor(unit.m_iMissionAIX);
	visitor(unit.m_iMissionAIY);
	visitor(unit.m_eMissionAIType);
	visitor(unit.m_eActivityType);
	visitor(unit.m_eAutomateType);
	visitor(unit.m_eUnitAIType);
	visitor(unit.m_eCombatType);
	visitor(unit.m_iEmbarkedAllWaterCount);
	visitor(unit.m_iEmbarkedDeepWaterCount);
	visitor(unit.m_iEmbarkExtraVisibility);
	visitor(unit.m_iEmbarkDefensiveModifier);
	visitor(unit.m_iCapitalDefenseModifier);
	visitor(unit.m_iCapitalDefenseFalloff);
	visitor(unit.m_iCityAttackPlunderModifier);
	visitor(unit.m_iReligiousStrengthLossRivalTerritory);
	visitor(unit.m_iTradeMissionInfluenceModifier);
	visitor(unit.m_iTradeMissionGoldModifier);
	visitor(unit.m_iDiploMissionInfluence);
	visitor(unit.m_iMapLayer);
	visitor(unit.m_iNumGoodyHutsPopped);
	visitor(unit.m_iTourismBlastStrength);
	visitor(unit.m_iTourismBlastLength);
	visitor(unit.m_iHurryStrength);
	visitor(unit.m_iGoldBlastStrength);
	visitor(unit.m_iScienceBlastStrength);
	visitor(unit.m_iCultureBlastStrength);
	visitor(unit.m_iGAPBlastStrength);
	visitor(unit.m_abPromotionEverObtained);
	visitor(unit.m_eTacticalMove);
	visitor(unit.m_iTacticalMoveSetTurn);
	visitor(unit.m_eHomelandMove);
	visitor(unit.m_iHomelandMoveSetTurn);

	visitor(unit.m_eUnitType);
	visitor.loadAssign(unit.m_pUnitInfo, (NO_UNIT != unit.m_eUnitType) ? GC.getUnitInfo(unit.m_eUnitType) : NULL);

	visitor(unit.m_Promotions);
	visitor(unit.m_combatUnit.eOwner);
	visitor(unit.m_combatUnit.iID);
	visitor(unit.m_transportUnit.eOwner);
	visitor(unit.m_transportUnit.iID);
	visitor(unit.m_missionAIUnit.eOwner);
	visitor(unit.m_missionAIUnit.iID);
	visitor(unit.m_extraDomainModifiers);
	visitor(unit.m_extraDomainAttacks);
	visitor(unit.m_extraDomainDefenses);
	visitor(unit.m_YieldModifier);
	visitor(unit.m_YieldChange);
	visitor(unit.m_iGarrisonYieldChange);
	visitor(unit.m_iFortificationYieldChange);
	visitor(unit.m_iMaxHitPointsBase);

	//CBP NOTE: Deleted repeat save data here to reduce bloat and manage memory better for MP
	visitor(unit.m_strGreatName);
	visitor(unit.m_strName);
	visitor(unit.m_Religion);

	visitor(unit.m_eGreatWork);

	// Mission queue
	{
		uint32 uSize = 0;
		if (bSaving)
			uSize = unit.m_missionQueue.getLength();

		visitor(uSize);

		for (uint32 uIdx = 0; uIdx < uSize; ++uIdx)
		{
			MissionData node;
			if (bSaving)
				node = *unit.m_missionQueue.getAt(uIdx);

			visitor(node.eMissionType);
			visitor(node.iData1);
			visitor(node.iData2);
			visitor(node.iFlags);
			visitor(node.iPushTurn);

			if (bLoading)
				unit.m_missionQueue.insertAtEnd(&node);
		}
	}

	// Last path cache
	{
		visitor(unit.m_kLastPath);
		visitor(unit.m_uiLastPathCacheOrigin);
		visitor(unit.m_uiLastPathCacheDestination);
		visitor(unit.m_uiLastPathFlags);
		visitor(unit.m_uiLastPathTurnSlice);
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::read(FDataStream& kStream)
{
	VALIDATE_OBJECT
	// Init data before load
	reset();

	// Perform shared serialize
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);

	// Re-attach to the map layer.
	if (m_iMapLayer != DEFAULT_UNIT_MAP_LAYER)
	{
		GC.getMap().plotManager().AddUnit(GetIDInfo(), m_iX, m_iY, m_iMapLayer);
	}
}


//	--------------------------------------------------------------------------------
void CvUnit::write(FDataStream& kStream) const
{
	VALIDATE_OBJECT

	// Perform shared serialize
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRangeStrike() const
{
	VALIDATE_OBJECT
	if(isEmbarked())
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	/* ---------
	// no longer used!
	// ---------
	if(isMustSetUpToRangedAttack())
	{
		if(!isSetUpForRangedAttack())
		{
			return false;
		}
	}
	*/

	if(isOutOfAttacks())
		return false;

#if defined(MOD_BALANCE_RANGED_ATTACK_ONLY_IN_NATIVE_DOMAIN)
	//this concerns not only embarked units but also ships in harbor!
    if(!isNativeDomain(plot()))
        return false;
#endif

	if (!canEndTurnAtPlot(plot()))
		return false;

	return true;
}

//	--------------------------------------------------------------------------------
int CvUnit::CountStackingUnitsAtPlot(const CvPlot* pPlot) const
{
	if (!pPlot)
		return 0;

	//this also catches air units!
	//they have separate checks (canRebaseAt)
	if (!IsCombatUnit())
		return 0;

	if (IsStackingUnit())
		return 0;

	int iNumUnitsOfSameType = 0;

	CvTeam& kUnitTeam = GET_TEAM(getTeam());
	const IDInfo* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		const CvUnit*  pLoopUnit = GetPlayerUnit(*pUnitNode);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && !pLoopUnit->isDelayedDeath())
		{
			// Ignore the unit itself if it's already in the plot
			if (pLoopUnit == this)
				continue;

			// Ignore units with special abilities
			if (pLoopUnit->IsStackingUnit())
				continue;

			// Don't include an enemy unit, or else it won't let us attack it :)
			if (kUnitTeam.isAtWar(pLoopUnit->getTeam()))
				continue;

			// Ignore cargo
			if (pLoopUnit->isCargo())
				continue;

			// Combat units are in conflict
			if (pLoopUnit->IsCombatUnit())
			{
				//inside of cities mixing domains is ok
				if (pPlot->isCoastalCityOrPassableImprovement(getOwner(),true,true))
				{
					if (getDomainType()==pLoopUnit->getDomainType())
						iNumUnitsOfSameType++;
				}
				//special rule for land units at sea
				else if (pPlot->isWater())
				{
					//we can embark under naval units, but not other land units!
					if (getDomainType() == pLoopUnit->getDomainType())
						iNumUnitsOfSameType++;
				}
				else
					//outside of cities domain doesn't matter
					iNumUnitsOfSameType++;
			}
		}
	}

	return iNumUnitsOfSameType;
}

bool CvUnit::CanStackUnitAtPlot(const CvPlot* pPlot) const
{
	if (pPlot)
		return CountStackingUnitsAtPlot(pPlot) < pPlot->getUnitLimit();

	return false;
}

bool CvUnit::canEverRangeStrikeAt(int iX, int iY) const
{
	return canEverRangeStrikeAt(iX,iY,NULL,false);
}

bool CvUnit::canEverRangeStrikeAt(int iX, int iY, const CvPlot* pSourcePlot, bool bIgnoreVisibility) const
{
	const CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);
	if(!pTargetPlot)
		return false;

	if (pSourcePlot)
	{
		// In Range?
		if (plotDistance(*pSourcePlot, *pTargetPlot) > GetRange())
		{
			return false;
		}

		// Plot not visible?
		if (!bIgnoreVisibility)
		{
			bool bCanSeeTarget = pTargetPlot->isVisible(getTeam()) || pSourcePlot->canSeePlot(pTargetPlot, getTeam(), visibilityRange(), getFacingDirection(true));
			if (!bCanSeeTarget)
				return false;
		}

#if defined(MOD_BALANCE_CORE)
		bool bWouldNeedEmbark = pSourcePlot->needsEmbarkation(this) && CanEverEmbark();
		bool bIsEmbarkedAttackingLand = !pTargetPlot->needsEmbarkation(this) && bWouldNeedEmbark;
		if (bIsEmbarkedAttackingLand)
			return false;
#endif

		// Can only bombard in domain?
		if (getUnitInfo().IsRangeAttackOnlyInDomain())
		{
			//check "landmasses" not domain types because we want to prevent subs from shooting into lakes
			//note that getArea does not work, one ocean can have multiple areas!
			bool bForbidden = (pTargetPlot->getLandmass() != pSourcePlot->getLandmass());
	
			//subs should be able to attack cities (they're on the coast, they've got ports, etc.)
			if (MOD_BALANCE_VP && pTargetPlot->isCity() && pTargetPlot->getPlotCity()->HasAccessToLandmass(pSourcePlot->getLandmass()))
				bForbidden = false;

			if (bForbidden)
				return false;
		}

		// Ignores LoS or can see the plot directly?
		if (!IsRangeAttackIgnoreLOS() && getDomainType() != DOMAIN_AIR)
			//not a typo, we need attack range here, not sight range
			if (!pSourcePlot->canSeePlot(pTargetPlot, getTeam(), GetRange(), getFacingDirection(true)))
				return false;

#if defined(MOD_BALANCE_RANGED_ATTACK_ONLY_IN_NATIVE_DOMAIN)
		if (!isNativeDomain(pSourcePlot))
			return false;
#endif
	}
	else //no source plot given, do only the most basic checks
	{
		// Plot not visible?
		if(!bIgnoreVisibility && !pTargetPlot->isVisible(getTeam()))
			return false;

		// Can only bombard in domain?
		if (getUnitInfo().IsRangeAttackOnlyInDomain())
		{
			bool bForbidden = (getDomainType() != pTargetPlot->getDomain());
			if (pTargetPlot->isCity() && pTargetPlot->isCoastalLand())
				bForbidden = false;

			if (bForbidden)
				return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::canRangeStrikeAt(int iX, int iY, bool bNeedWar, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT

	if(!canRangeStrike())
	{
		return false;
	}

	if(!canEverRangeStrikeAt(iX, iY, plot(), false))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	// If it's NOT a city, see if there are any units to aim for
	if(!pTargetPlot->isCity())
	{
		if(bNeedWar)
		{
			const CvUnit* pDefender = rangeStrikeTarget(*pTargetPlot, bNoncombatAllowed);
			if(NULL == pDefender)
			{
#if defined(MOD_EVENTS_UNIT_RANGEATTACK)
				if (MOD_EVENTS_UNIT_RANGEATTACK) {
					if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_UnitCanRangeAttackAt, getOwner(), GetID(), iX, iY, bNeedWar) == GAMEEVENTRETURN_TRUE) {
						return true;
					}
				}
#endif
				return false;
			}

#if defined(MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY)
				// if the defender is a sub
				if (MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY && pDefender->getInvisibleType() == 0) {
					// if the plot is ice, unless the attacker is also a sub, return false
					if (pTargetPlot->getFeatureType() == FEATURE_ICE && this->getInvisibleType() != 0) {
						return false;
					}

					// if the attacker is an archer type unit, return false
					if (this->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true)) {
						return false;
					}
				}
#endif
		}
		// We don't need to be at war (yet) with a Unit here, so let's try to find one
		else
		{
			const IDInfo* pUnitNode = pTargetPlot->headUnitNode();
			const CvUnit* pLoopUnit = NULL;
			bool bFoundUnit = false;

			CvTeam& myTeam = GET_TEAM(getTeam());

			while(pUnitNode != NULL)
			{
				pLoopUnit = ::GetPlayerUnit(*pUnitNode);
				pUnitNode = pTargetPlot->nextUnitNode(pUnitNode);

				if (!pLoopUnit)
					continue;

#if defined(MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY)
				// if the defender is a sub
				if (MOD_GLOBAL_SUBS_UNDER_ICE_IMMUNITY && pLoopUnit->getInvisibleType() == 0)
				{
					// if the plot is ice, unless the attacker is also a sub, ignore them
					if (pTargetPlot->getFeatureType() == FEATURE_ICE && this->getInvisibleType() != 0)
					{
						continue;
					}

					// if the attacker is an archer type unit, ignore them
					if (this->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true))
					{
						continue;
					}
				}
#endif

				if(!pLoopUnit) continue;

				TeamTypes loopTeam = pLoopUnit->getTeam();

				// Make sure it's a valid Team
				if(myTeam.isAtWar(loopTeam) || myTeam.canDeclareWar(loopTeam, getOwner()))
				{
					bFoundUnit = true;
					break;
				}
			}

			if(!bFoundUnit)
			{
#if defined(MOD_EVENTS_UNIT_RANGEATTACK)
				if (MOD_EVENTS_UNIT_RANGEATTACK) {
					if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_UnitCanRangeAttackAt, getOwner(), GetID(), iX, iY, bNeedWar) == GAMEEVENTRETURN_TRUE) {
						return true;
					}
				}
#endif
				return false;
			}
		}
	}
	// If it is a City, only consider those we're at war with
	else
	{
		CvAssert(pTargetPlot->getPlotCity() != NULL);

		// If you're already at war don't need to check
		if(!atWar(getTeam(), pTargetPlot->getPlotCity()->getTeam()))
		{
			if(bNeedWar)
			{
				return false;
			}
			// Don't need to be at war with this City's owner (yet)
			else
			{
				if(!GET_TEAM(getTeam()).canDeclareWar(pTargetPlot->getPlotCity()->getTeam(), getOwner()))
				{
					return false;
				}
			}
		}

		// Missiles need a unit to attack, cannot hit cities without a unit inside
		if (AI_getUnitAIType() == UNITAI_MISSILE_AIR)
		{
			const CvUnit* pDefender = rangeStrikeTarget(*pTargetPlot, bNoncombatAllowed);
			if (!pDefender)
				return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
bool CvUnit::IsAirSweepCapable() const
{
	return GetAirSweepCapableCount() > 0;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
int CvUnit::GetAirSweepCapableCount() const
{
	return m_iAirSweepCapableCount;
}

//	--------------------------------------------------------------------------------
/// Can this Unit air sweep to eliminate interceptors?
void CvUnit::ChangeAirSweepCapableCount(int iChange)
{
	if(iChange != 0)
	{
		m_iAirSweepCapableCount += iChange;
	}
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::canAirSweep() const
{
	VALIDATE_OBJECT

	if(!IsAirSweepCapable())
	{
		return false;
	}

	if(isEmbarked())
	{
		return false;
	}

	if(GetRange() <= 0)
	{
		return false;
	}

	if(GetBaseRangedCombatStrength() <= 0)
	{
		return false;
	}

	if(isOutOfAttacks())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::canAirSweepAt(int iX, int iY) const
{
	VALIDATE_OBJECT
	if(!canAirSweep())
	{
		return false;
	}

	CvPlot* pSourcePlot = plot();

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	if(NULL == pTargetPlot)
	{
		return false;
	}

	// Have to be able to see the target plot
	if(!pTargetPlot->isVisible(getTeam()))
	{
		return false;
	}

	// Target plot must be within this unit's air range
	if(plotDistance(pSourcePlot->getX(), pSourcePlot->getY(), pTargetPlot->getX(), pTargetPlot->getY()) > GetRange())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Sweep the skies and clear out interceptors
bool CvUnit::airSweep(int iX, int iY)
{
	if(!canAirSweepAt(iX, iY))
	{
		return false;
	}

	CvPlot* pTargetPlot = GC.getMap().plot(iX, iY);

	CvUnitCombat::AttackAirSweep(*this, *pTargetPlot, CvUnitCombat::ATTACK_OPTION_NONE);

	return true;
}

bool CvUnit::attemptGroundAttacks(const CvPlot& pPlot)
{
	bool bFoundSomething = false;
	if (!IsAirSweepCapable())
		return bFoundSomething;

	int iAirSweepDamage = getGroundAttackDamage();

	CvString strAppendText = GetLocalizedText("TXT_KEY_PROMOTION_AIR_SWEEP");

	int iRange = 1;
	for (int i = -iRange; i <= iRange; ++i)
	{
		for (int j = -iRange; j <= iRange; ++j)
		{
			CvPlot* pLoopPlot = ::plotXYWithRangeCheck(pPlot.getX(), pPlot.getY(), i, j, iRange);
			if (NULL != pLoopPlot)
			{
				pLoopPlot->changeVisibilityCount(getTeam(), 1, NO_INVISIBLE, false, false, this);

				if (pLoopPlot->getOwner() == NO_PLAYER || pLoopPlot->getOwner() == getOwner())
					continue;

				const IDInfo* pUnitNode = pLoopPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = GetPlayerUnit(*pUnitNode);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit == NULL)
						continue;
					if (!pLoopUnit->canIntercept() && pLoopUnit->getDomainType() != DOMAIN_AIR)
						continue;
					if (pLoopUnit->isSuicide())
						continue;
					if (pLoopUnit->isCargo())
						continue;
					if (!GET_TEAM(pLoopUnit->getTeam()).isAtWar(getTeam()))
						continue;

					pLoopUnit->changeDamage(iAirSweepDamage, getOwner(), 0.25f, &strAppendText);

					if (pLoopUnit->IsDead())
					{
						CvString strBuffer;
						int iActivePlayerID = GC.getGame().getActivePlayer();

						if (iActivePlayerID == getOwner())
						{
							strBuffer = GetLocalizedText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameKey(), iAirSweepDamage, pLoopUnit->getNameKey());
							GC.GetEngineUserInterface()->AddMessage(0, getOwner(), true, /*10*/ GD_INT_GET(EVENT_MESSAGE_TIME), strBuffer/*, GC.getEraInfo(GC.getGame().getCurrentEra())->getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pkTargetPlot->getX(), pkTargetPlot->getY()*/);
						}

						CvUnitCombat::ApplyPostKillTraitEffects(this, pLoopUnit);
					}

					bFoundSomething = true;
				}
			}
		}
	}
	return bFoundSomething;
}

//	---------------------------------------------------------------------------
bool CvUnit::isEnemy(TeamTypes eFromPerspectiveOfTeam, const CvPlot* pAssumedUnitPlot) const
{
	VALIDATE_OBJECT
	if(!pAssumedUnitPlot)
		pAssumedUnitPlot = plot();

	//failsafe ...
	if(!pAssumedUnitPlot)
		return false;

	PlayerTypes ePretendOwner = getCombatOwner(eFromPerspectiveOfTeam, *pAssumedUnitPlot);
	return atWar(GET_PLAYER(ePretendOwner).getTeam(), eFromPerspectiveOfTeam);
}

//	--------------------------------------------------------------------------------
bool CvUnit::isSuicide() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsSuicide();
}

//	--------------------------------------------------------------------------------
int CvUnit::getDropRange() const
{
	VALIDATE_OBJECT
	return m_iDropRange;
}

//	--------------------------------------------------------------------------------
void CvUnit::changeDropRange(int iChange)
{
	VALIDATE_OBJECT
	if(iChange != 0)
	{
		m_iDropRange += iChange;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::isTrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsTrade();
}

//	--------------------------------------------------------------------------------
int CvUnit::getAlwaysHostileCount() const
{
	VALIDATE_OBJECT
	return m_iAlwaysHostileCount;
}


//	--------------------------------------------------------------------------------
void CvUnit::changeAlwaysHostileCount(int iValue)
{
	VALIDATE_OBJECT
	m_iAlwaysHostileCount += iValue;
	CvAssert(getAlwaysHostileCount() >= 0);
}


//	--------------------------------------------------------------------------------
bool CvUnit::isAlwaysHostile(const CvPlot& plot) const
{
	VALIDATE_OBJECT
	if(plot.isCity())
	{
		return false;
	}

	return (getAlwaysHostileCount() > 0);
}

bool CvUnit::shouldHeal(bool bBeforeAttacks) const
{
	if (isDelayedDeath() || !IsHurt())
		return false;

	//sometimes we should heal but we have to fight instead
	if (bBeforeAttacks)
	{
		//sometimes there is no other option
		int iHardHpLimit = 13;
		if (GetCurrHitPoints() < iHardHpLimit)
			return true;

		//also depends on what we can do with the unit
		int iSoftHpLimit = GetMaxHitPoints() / 3;
		return GetCurrHitPoints() < iSoftHpLimit && canHeal(plot(),false) && TacticalAIHelpers::GetTargetsInRange(this, true, false).empty();
	}
	else 
	{
		if (GetNumEnemyUnitsAdjacent()>0)
		{
			//only run away if strictly necessary
			return isProjectedToDieNextTurn() || (GetDanger() > GetCurrHitPoints() && getDamage() > 0);
		}
		else
		{
			//we might lack a resource for healing
			CvCity* pCapital = GET_PLAYER(getOwner()).getCapitalCity();
			int iMaxHealRate = pCapital ? healRate(pCapital->plot()) : healRate(plot());
			bool bAllowMoreDamage = GET_PLAYER(getOwner()).GetPlayerTraits()->IsFightWellDamaged() || IsStrongerDamaged() || IsFightWellDamaged() || isBarbarian();

			//typically want to start healing before health becomes critical
			int iAcceptableDamage = 20;
			if (bAllowMoreDamage || iMaxHealRate == 0)
				iAcceptableDamage = 50;

			return getDamage() > iAcceptableDamage;
		}
	}
}

//	--------------------------------------------------------------------------------
int CvUnit::getArmyID() const
{
	VALIDATE_OBJECT
	return m_iArmyId;
}


//	--------------------------------------------------------------------------------
void CvUnit::setArmyID(int iNewArmyID)
{
	VALIDATE_OBJECT

	if (iNewArmyID!=-1 && shouldHeal(true))
	{
		//shouldn't happen
		OutputDebugString("warning: damaged unit recruited into army!\n");
	}
		
	m_iArmyId = iNewArmyID;
}

CvString CvUnit::getTacticalZoneInfo() const
{
	CvTacticalDominanceZone* pZone = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByPlot(plot());
	if (pZone)
	{
		const char* dominance[] = { "no units", "friendly", "hostile", "contested" };
		return CvString::format("zone %d, %s, %s", pZone->GetZoneID(), dominance[pZone->GetOverallDominanceFlag()], postureNames[pZone->GetPosture()]);
	}

	return CvString("no tactical zone");
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsSelected() const
{
	VALIDATE_OBJECT
	return DLLUI->IsUnitSelected(getOwner(), GetID());
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsFirstTimeSelected() const
{
	if (GC.getGame().isNetworkMultiPlayer() || GC.getGame().isReallyNetworkMultiPlayer())
		return false;

	VALIDATE_OBJECT
	return IsSelected() && m_iEverSelectedCount == 1;
}

//	--------------------------------------------------------------------------------
void CvUnit::IncrementFirstTimeSelected()
{
	if (GC.getGame().isNetworkMultiPlayer() || GC.getGame().isReallyNetworkMultiPlayer())
		return;

	VALIDATE_OBJECT
	if(m_iEverSelectedCount < 2)
	{
		m_iEverSelectedCount++;
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::HasQueuedVisualizationMoves() const
{
	return !m_unitMoveLocs.empty();
}

//	--------------------------------------------------------------------------------
void CvUnit::QueueMoveForVisualization(CvPlot* pkPlot)
{
	VALIDATE_OBJECT
	if(pkPlot)
	{
		m_unitMoveLocs.push_back(pkPlot);
		if(m_unitMoveLocs.size() == 20)
		{
			PublishQueuedVisualizationMoves();
		}
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::PublishQueuedVisualizationMoves()
{
	VALIDATE_OBJECT
	if(HasQueuedVisualizationMoves())
	{
		CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
		CvPlotIndexVector kPlotArray;

		kPlotArray.reserve(m_unitMoveLocs.size());
		for(std::vector<CvPlot*>::const_iterator itr = m_unitMoveLocs.begin(); itr != m_unitMoveLocs.end(); ++itr)
		{
			kPlotArray.push_back((*itr)->GetPlotIndex());
		}

		gDLL->GameplayUnitMoved(pDllUnit.get(), kPlotArray);
		m_unitMoveLocs.clear();
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::SetPosition(CvPlot* pkPlot)
{
	VALIDATE_OBJECT

	CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
	CvInterfacePtr<ICvPlot1> pDllPlot(new CvDllPlot(pkPlot));
	gDLL->GameplayUnitTeleported(pDllUnit.get(), pDllPlot.get());

	m_unitMoveLocs.clear();
}

//	--------------------------------------------------------------------------------
const CvSyncArchive<CvUnit>& CvUnit::getSyncArchive() const
{
	VALIDATE_OBJECT
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
CvSyncArchive<CvUnit>& CvUnit::getSyncArchive()
{
	VALIDATE_OBJECT
	return m_syncArchive;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsDoingPartialMove() const
{
	VALIDATE_OBJECT
	const MissionData* pkMissionNode = HeadMissionData();
	if(!pkMissionNode)
	{
		return false;
	}

	if(m_kLastPath.empty())
	{
		return false;
	}

	CvPlot* pEndTurnPlot = m_kLastPath.GetTurnDestinationPlot(0);
	CvPlot* pEndPathPlot = m_kLastPath.GetFinalPlot();

	if(plot() == pEndTurnPlot && plot() != pEndPathPlot && (pkMissionNode->iData1 == pEndPathPlot->getX() && pkMissionNode->iData2 == pEndPathPlot->getY()))
	{
		if(getMoves() < maxMoves() && getMoves() > 0)
		{
			return true;
		}
	}

	return false;
}


//	--------------------------------------------------------------------------------
ActivityTypes CvUnit::GetActivityType() const
{
	VALIDATE_OBJECT
	return m_eActivityType;
}


//	--------------------------------------------------------------------------------
void CvUnit::SetActivityType(ActivityTypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);
	ActivityTypes eOldActivity = GetActivityType();

	if(eOldActivity != eNewValue)
	{
		CvPlot* pPlot = plot();

		m_eActivityType = eNewValue;

		CvInterfacePtr<ICvPlot1> pDllSelectionPlot(DLLUI->getSelectionPlot());
		int iSelectionPlotIndex = (pDllSelectionPlot.get() != NULL)? pDllSelectionPlot->GetPlotIndex() : -1;
		if(pPlot->GetPlotIndex() == iSelectionPlotIndex)
		{
			DLLUI->setDirty(PlotListButtons_DIRTY_BIT, true);
			DLLUI->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

//	--------------------------------------------------------------------------------
AutomateTypes CvUnit::GetAutomateType() const
{
	VALIDATE_OBJECT
	return m_eAutomateType;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsAutomated() const
{
	VALIDATE_OBJECT
	return (GetAutomateType() != NO_AUTOMATE);
}


//	--------------------------------------------------------------------------------
void CvUnit::SetAutomateType(AutomateTypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	if(GetAutomateType() != eNewValue)
	{
		//anything the AI did previously is now forgotten
		SetTurnProcessed(false);

		AutomateTypes eOldAutomateType = GetAutomateType();
		m_eAutomateType = eNewValue;

		ClearMissionQueue();
		SetActivityType(ACTIVITY_AWAKE);
		if(eOldAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->UpdateExplorePlotsFromScratch(); // these need to be rebuilt
		}

		// if canceling automation, cancel on cargo as well
		if(eNewValue == NO_AUTOMATE)
		{
			CvPlot* pPlot = plot();
			if(pPlot != NULL)
			{
				IDInfo* pUnitNode = pPlot->headUnitNode();
				while(pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = ::GetPlayerUnit(*pUnitNode);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if(pTransportUnit != NULL && pTransportUnit == this)
					{
						pCargoUnit->SetAutomateType(NO_AUTOMATE);
						pCargoUnit->SetActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
		else if(m_eAutomateType == AUTOMATE_EXPLORE)
		{
			GET_PLAYER(getOwner()).GetEconomicAI()->UpdateExplorePlotsFromScratch(); // these need to be rebuilt
		}
	}
}

//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToSelect() const
{
	//note that units with the turn processed flag are ok here!
	return (ReadyToMove() && !IsAutomated() && !isDelayedDeath());
}


//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToMove() const
{
	VALIDATE_OBJECT
	if(!canMove())
	{
		return false;
	}

	if (GetLengthMissionQueue() > 0)
	{
		return false;
	}

	if(GetActivityType() != ACTIVITY_AWAKE)
	{
		return false;
	}

	if (GetAutomateType() != NO_AUTOMATE)
	{
		return false;
	}

	if(IsBusy())
	{
		return false;
	}

	if (MOD_LINKED_MOVEMENT && IsLinked() && !IsLinkedLeader()) // do not ask orders for units following another
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToSwap() const
{
	VALIDATE_OBJECT
	if (!canMove())
	{
		return false;
	}

	//allow a single move mission
	if (GetLengthMissionQueue() > 1)
	{
		return false;
	}

	if (GetLengthMissionQueue() == 1 && GetHeadMissionData()->eMissionType != CvTypes::getMISSION_MOVE_TO())
	{
		return false;
	}

	if (GetAutomateType() != NO_AUTOMATE)
	{
		return false;
	}

	if (IsBusy())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::ReadyToAuto() const
{
	VALIDATE_OBJECT
	return (canMove() && (GetLengthMissionQueue() != 0));
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsBusy() const
{
	VALIDATE_OBJECT
	if(GetMissionTimer() > 0)
	{
		return true;
	}

	if(isInCombat())
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
bool CvUnit::SentryAlert(bool bAllowAttacks) const
{
	if (bAllowAttacks)
	{
		int iRange = visibilityRange();
		if (getDomainType() == DOMAIN_AIR)
			iRange = GetRange();

		for(int iX = -iRange; iX <= iRange; ++iX)
		{
			for(int iY = -iRange; iY <= iRange; ++iY)
			{
				CvPlot* pPlot = ::plotXYWithRangeCheck(getX(), getY(), iX, iY, iRange);
				if(pPlot && pPlot->isEnemyUnit(getOwner(),false,true))
				{
					return true;
				}
			}
		}
	}

	//combat units should wake as soon as enemies are around
	//civilians ignore non-lethal danger like fallout
	int iDangerLimit = healRate(plot());

	//if we're on the move, check the plot we're going to, not the one we're currently at
	if (GetHeadMissionData() && GetHeadMissionData()->eMissionType == CvTypes::getMISSION_MOVE_TO() && IsCachedPathValid())
	{
		CvPlot* pTurnDestination = GetPathEndFirstTurnPlot();
		return GetDanger(pTurnDestination) > iDangerLimit;
	}

	return GetDanger() > iDangerLimit || m_iDamageTakenLastTurn > iDangerLimit;
}

//	--------------------------------------------------------------------------------
RouteTypes CvUnit::GetBestBuildRouteForRoadTo(CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	VALIDATE_OBJECT

	if(peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	int iBestValue = 0;
	if (pPlot->getRouteType() != NO_ROUTE && !pPlot->IsRoutePillaged())
		iBestValue = GC.getRouteInfo(pPlot->getRouteType())->getValue();
	RouteTypes eBestRoute = NO_ROUTE;
	
	bool bOnlyRoad = false;
	if (MOD_GLOBAL_QUICK_ROUTES && GET_PLAYER(getOwner()).isHuman())
	{
		// If there is no road to the mission end plot, bOnlyRoad is true
		bOnlyRoad = !IsPlotConnectedToPlot(getOwner(), pPlot, LastMissionPlot(), ROUTE_ROAD, false, false);
	}

	for(int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			const RouteTypes eRoute = (RouteTypes)pkBuildInfo->getRoute();
			if(eRoute != NO_ROUTE)
			{
				CvRouteInfo* pkRouteInfo = GC.getRouteInfo(eRoute);
				if(pkRouteInfo)
				{
					if(canBuild(pPlot, eBuild))
					{
						int iValue = pkRouteInfo->getValue();
						
						if (bOnlyRoad && eRoute != ROUTE_ROAD) {
							iValue = 0;
						}

						if(iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
							if(peBestBuild != NULL)
							{
								*peBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}

//	--------------------------------------------------------------------------------
void CvUnit::PlayActionSound()
{
	VALIDATE_OBJECT
}

int CvUnit::ComputePath(const CvPlot* pToPlot, int iFlags, int iMaxTurns, bool bCacheResult)
{
	SPathFinderUserData data(this,iFlags,iMaxTurns);
	const CvPlot* pDestPlot = pToPlot;
	SPath newPath;
	int x = pToPlot->getX();
	int y = pToPlot->getY();

	if ((iFlags & CvUnit::MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT) && !m_kLastPath.empty())
	{
		x = m_kLastPath.back().m_iX;
		y = m_kLastPath.back().m_iY;
		pDestPlot = m_kLastPath.GetFinalPlot();
	}

	// If no path exists but continue to closest plot flag is set try to move to an adjacent plot
	if (MOD_SQUADS)
	{
		newPath = GC.GetPathFinder().GetPath(getX(), getY(), x, y, data);

		if ((iFlags & CvUnit::MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT) && (!newPath || !CanStackUnitAtPlot(pToPlot)))
		{
			const CvPlot* pTargetPlot = pToPlot;

			// We want to find a new plot relative to the squads destination, not this move missions destination
			if (HasSquadDestination())
			{
				pTargetPlot = GetSquadDestination();
			}

			// We want to reroute the unit to a plot within the closest available ring to the squad movement
			// plot, while also minimizing distance from it's current position
			std::vector<ScoredPlot> eligiblePlots;
			int currRingEndIdx = 1;
			for (int i = 0; i < RING_PLOTS[currRingEndIdx]; i++)
			{
				CvPlot* pLoopPlot = iterateRingPlots(pTargetPlot, i);
				if (!pLoopPlot)
					continue;

				if ((pToPlot->isWater() == pLoopPlot->isWater()) && canMoveInto(*pLoopPlot, iFlags | CvUnit::MOVEFLAG_DESTINATION))
				{
					newPath = GC.GetPathFinder().GetPath(getX(), getY(), pLoopPlot->getX(), pLoopPlot->getY(), data);
					if (!newPath)
					{
						// Unit can technically move into this tile but no path exists so try another one
						continue;
					}
					// Don't want to take forever rerouting if we're already inside the ring of consideration
					else if (plotDistance(*plot(), *pTargetPlot) <= currRingEndIdx && newPath.iTotalTurns > 2)
					{
						continue;
					}
					else
					{
						eligiblePlots.push_back(ScoredPlot(newPath.iTotalCost, pLoopPlot));
					}
				}

				if (i == (RING_PLOTS[currRingEndIdx] - 1) && eligiblePlots.empty())
				{
					currRingEndIdx++;
				}
			}

			std::stable_sort(eligiblePlots.begin(), eligiblePlots.end(), less<ScoredPlot>());
			if (!eligiblePlots.empty())
			{
				pDestPlot = eligiblePlots.front().plot;
			}
		}
	}

	if (pDestPlot)
		newPath = GC.GetPathFinder().GetPath(getX(), getY(), pDestPlot->getX(), pDestPlot->getY(), data);

	//now copy the new path
	if (bCacheResult)
	{
		ClearPathCache();

		// This helps in preventing us from trying to re-path to the same unreachable location.
		if (!!newPath)
		{
			m_uiLastPathCacheOrigin = plot()->GetPlotIndex();
			m_uiLastPathCacheDestination = pDestPlot->GetPlotIndex();
			m_uiLastPathFlags = (iFlags & PATHFINDER_FLAG_MASK); //ignore this one flag as it's added only when executing the path!
			m_uiLastPathTurnSlice = GC.getGame().getTurnSlice();

			for (size_t i = 0; i < newPath.vPlots.size(); i++)
			{
				CvPathNode nextNode( newPath.vPlots[i] );

				//skip the first node (if it's not a stop node), it's the current unit plot
				//important: this means that an empty m_kLastPath is valid!
				if (i == 0 && nextNode.m_iMoves > 0)
					continue;

				m_kLastPath.push_back(nextNode);
			}
		}
	}

	if (!newPath)
	{
		return INT_MAX;
	}
	else
	{		
		if ( (iFlags & CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN) && newPath.vPlots.back().moves == 0 )
			return newPath.vPlots.back().turns + 1;
		else
			return newPath.vPlots.back().turns;
	}
}

//	---------------------------------------------------------------------------
bool CvUnit::VerifyCachedPath(const CvPlot* pDestPlot, int iFlags, int iMaxTurns)
{
	// this method is only called from ContinueMission
	// all previous path data is deleted when a mission is started
	// we can assume that other than the unit that is moving, nothing on the map will change
	// so we can re-use the cached path data most of the time

	if (m_kLastPath.empty() || !HaveCachedPathTo(pDestPlot,iFlags))
		return ComputePath(pDestPlot, iFlags, iMaxTurns, true) != INT_MAX;

	return IsCachedPathValid();
}

bool CvUnit::CheckDOWNeededForMove(int iX, int iY, bool bPopup)
{
	CvMap& kMap = GC.getMap();
	CvPlot* pDestPlot = kMap.plot(iX, iY);

	// Important
	if (at(iX, iY))
		return false;

	// No war declarations from automated units
	if (IsAutomated())
		return false;

	// Test if this attack requires war to be declared first
	if (pDestPlot && isHuman() && getOwner() == GC.getGame().getActivePlayer() && pDestPlot->isVisible(getTeam()))
	{
		TeamTypes eRivalTeam = GetDeclareWarMove(*pDestPlot);

		if (eRivalTeam != NO_TEAM)
		{
			if (bPopup) {
				CvPopupInfo kPopup(BUTTONPOPUP_DECLAREWARMOVE);
				kPopup.iData1 = eRivalTeam;
				kPopup.iData2 = pDestPlot->getX();
				kPopup.iData3 = pDestPlot->getY();
				kPopup.bOption1 = false;
				kPopup.bOption2 = pDestPlot->getTeam() != eRivalTeam;
				if (pDestPlot->isCity())
				{
					kPopup.iFlags = DOW_MOVE_ONTO_CITY;
				}
				// If a unit was present, put up the standard DOW message
				else if (pDestPlot->isVisibleOtherUnit(m_eOwner))
				{
					kPopup.iFlags = DOW_MOVE_ONTO_UNIT;
				}
				// Otherwise put out one about entering enemy territory
				else
				{
					kPopup.iFlags = DOW_MOVE_INTO_TERRITORY;
				}
				DLLUI->AddPopup(kPopup);
			}
			return true;
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
// Returns +1 if attack was made...
// Return 0 if no target in range
// Return -1 if attack is not possible
CvUnit::MoveResult CvUnit::UnitAttackWithMove(int iX, int iY, int iFlags)
{
	VALIDATE_OBJECT
	CvMap& kMap = GC.getMap();
	CvPlot* pDestPlot = kMap.plot(iX, iY);

	CvAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");
	if(!pDestPlot)
		return CvUnit::MOVE_RESULT_CANCEL;

	// Air mission
	if(getDomainType() == DOMAIN_AIR)
	{
		if (GetBaseCombatStrength() == 0 && canRangeStrikeAt(iX, iY))
		{
			CvUnitCombat::AttackAir(*this, *pDestPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);
			return CvUnit::MOVE_RESULT_ATTACK;
		}
		else
			return CvUnit::MOVE_RESULT_NO_TARGET;
	}

	//other domains - look at the next plot in the path
	CvPlot* pPathPlot = m_kLastPath.GetFirstPlot();
	if (!pPathPlot)
		return CvUnit::MOVE_RESULT_CANCEL;

	//nothing to attack?
	bool bIsEnemyCity = pPathPlot->isEnemyCity(*this);
	CvUnit* pBestDefender = pPathPlot->getBestDefender(NO_PLAYER, getOwner(), this, true);
	if ( !bIsEnemyCity && !pBestDefender )
		return CvUnit::MOVE_RESULT_NO_TARGET;

	//we only want to attack at the target plot, not random units we may reveal on the way
	if (pPathPlot != pDestPlot)
		return CvUnit::MOVE_RESULT_CANCEL;

	//can we even attack?
	if(!IsCanAttackWithMove() || isOutOfAttacks())
		return CvUnit::MOVE_RESULT_CANCEL;

	//support units cannot attack themselves
	if (IsCityAttackSupport() && !bIsEnemyCity)
		return CvUnit::MOVE_RESULT_CANCEL;

	//embarked can't do a move-attack
	if(pPathPlot->needsEmbarkation(this))
		return CvUnit::MOVE_RESULT_CANCEL;

	//don't allow an attack if we already have one
	if(isFighting() || pPathPlot->isFighting())
		return CvUnit::MOVE_RESULT_CANCEL;

	//check consistency - we know there's a target there!
	if (!canMoveInto(*pPathPlot,iFlags|MOVEFLAG_ATTACK))
	{
		return CvUnit::MOVE_RESULT_CANCEL;
	}

	// Publish any queued moves so that the attack doesn't appear to happen out of order
	PublishQueuedVisualizationMoves();

	// City combat
	if (bIsEnemyCity)
	{
		CvUnitCombat::AttackCity(*this, *pPathPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);

		if (pPathPlot->isCity() && pPathPlot->isBarbarian())
		{
			CvBarbarians::DoBarbSpawnerAttacked(pPathPlot);
		}

		return CvUnit::MOVE_RESULT_ATTACK;
	}
	// Normal unit combat
	else if (pBestDefender)
	{
		CvUnitCombat::Attack(*this, *pPathPlot, (iFlags &  MOVEFLAG_NO_DEFENSIVE_SUPPORT)?CvUnitCombat::ATTACK_OPTION_NO_DEFENSIVE_SUPPORT:CvUnitCombat::ATTACK_OPTION_NONE);

		// Barb camp here that was attacked?
		if (pPathPlot->getImprovementType() == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
		{
			CvBarbarians::DoBarbSpawnerAttacked(pPathPlot);
		}

		return CvUnit::MOVE_RESULT_ATTACK;
	}

	//cannot happen
	return CvUnit::MOVE_RESULT_NO_TARGET;
}

//	---------------------------------------------------------------------------
bool CvUnit::UnitMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	VALIDATE_OBJECT
	bool bCanMoveIntoPlot = false;
	if(canMove() && pPlot)
	{
		// if it's a combat and the attacking unit can capture things or the city is an enemy city.
		if(bCombat && (!(isNoCapture()) || !(pPlot->isEnemyCity(*this))))
		{
			bCanMoveIntoPlot = canMoveOrAttackInto(*pPlot);
		}
		else
		{
			bCanMoveIntoPlot = canMoveInto(*pPlot);
		}
	}

	bool bIsNoCapture = isNoCapture();
	bool bEnemyCity = pPlot && pPlot->isEnemyCity(*this);

	bool bIsCombatUnit = (this == pCombatUnit);
	bool bExecuteMove = false;

	if(pPlot && (bCanMoveIntoPlot || (bIsCombatUnit && !(bIsNoCapture && bEnemyCity))))
	{
		// execute move
		LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("UnitMove() : player ") << GET_PLAYER(getOwner()).getName(); << std::string(" ") << getName() << std::string(" id=") << GetID() << std::string(" moving to ") << pPlot->getX() << std::string(", ") << pPlot->getY());
		move(*pPlot, true);
	}
	else
	{
		bExecuteMove = true;
	}

	//execute move
	if(bEndMove || !canMove() || IsDoingPartialMove())
	{
		bExecuteMove = true;
	}

	if(CvPreGame::quickMovement())
	{
		bExecuteMove = false;
	}

	if(bExecuteMove)
	{
		PublishQueuedVisualizationMoves();
	}

	return bCanMoveIntoPlot;
}

//	---------------------------------------------------------------------------
// Returns the number of turns it will take to reach the target or a MOVE_RESULT indicating a problem
int CvUnit::UnitPathTo(int iX, int iY, int iFlags)
{
	VALIDATE_OBJECT

	CvPlot* pPathPlot = NULL;
	CvMap& kMap = GC.getMap();
	CvPlot* pDestPlot = kMap.plot(iX, iY);
	if (!pDestPlot)
		return MOVE_RESULT_CANCEL;

	if(getDomainType() == DOMAIN_AIR)
	{
		if(!canMoveInto(*pDestPlot) || at(iX,iY))
			return MOVE_RESULT_CANCEL;

		pPathPlot = pDestPlot;
		ClearPathCache(); // Not used by air units, keep it clear.
	}
	else
	{
		pPathPlot = m_kLastPath.GetFirstPlot();

		//wait until next turn if the pathfinder inserted a stop node because the next plot is occupied
		if (pPathPlot && m_kLastPath.front().m_iTurns>0)
		{
			finishMoves();
			return MOVE_RESULT_NEXT_TURN;
		}

		//can happen if we don't really move. (ie we try to move to a plot we are already in or the approximate target is just one plot away)
		if (pPathPlot == NULL)
			pPathPlot = pDestPlot;

		//the given target may be different from the actual target
		if ((iFlags & MOVEFLAG_APPROX_TARGET_RING1) || (iFlags & MOVEFLAG_APPROX_TARGET_RING2) || (iFlags & MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT))
			pDestPlot = m_kLastPath.GetFinalPlot();

		//check if we are there yet
		if (pDestPlot && pDestPlot->getX() == getX() && pDestPlot->getY() == getY())
		{
		  return MOVE_RESULT_CANCEL;
		}
	}

	if(!m_kLastPath.empty())
	{
		//because of the fog it's totally valid to stumble into eg neutral units
		//make sure we have at least one plot where we can end this turn, meaning it's visible and we can stack
		CvPlot* pTurnDest = m_kLastPath.GetTurnDestinationPlot(0);

		//assume no good plot
		bool bRejectMove = true;

		//start iterating at zero although we know it's blocked but it might be the turn destination already!
		for (size_t iIndex=0; iIndex<m_kLastPath.size(); iIndex++)
		{
			CvPlot* pTestPlot = m_kLastPath.GetPlotByIndex(iIndex);

			//very rarely there are unexplained bugs with visibility, but we know adjacent plots must be visible ...
			//make sure our units don't get stuck in the fog of war (assume zero-vis units have no stacking problems)
			bool bCanSee = pTestPlot->isVisible(getTeam()) || plotDistance(pTestPlot->getX(), pTestPlot->getY(), getX(), getY())<=1;

			//it's possible that there is an enemy civilian we want to capture so we need to allow attacking
			//(real attacks are handled in UnitAttackWithMove)
			if (bCanSee && canMoveOrAttackInto(*pTestPlot, iFlags | MOVEFLAG_DESTINATION))
			{
				bRejectMove = false;
				break;
			}

			//look only at this turn's plots!
			if (pTestPlot == pTurnDest)
				break;
		} 

		if(bRejectMove)
		{
			ClearPathCache();
			PublishQueuedVisualizationMoves();
			// If the path is non-empty but all plots on it aren't valid moves this turn,
			// wait and recalculate and try again next turn
			if (MOD_SQUADS && (iFlags & CvUnit::MOVEFLAG_CONTINUE_TO_CLOSEST_PLOT) && CanStackUnitAtPlot(plot()))
			{
				finishMoves();
				return MOVE_RESULT_NEXT_TURN;
			}
			else
			{
				return MOVE_RESULT_CANCEL;
			}
		}
	}

	bool bDone = (pPathPlot == pDestPlot);
	if ((iFlags & CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER) && (pPathPlot == m_kLastPath.GetTurnDestinationPlot(0) && !bDone))
	{
		int iOldDanger = GetDanger();
		int iNewDanger = GetDanger(pPathPlot);
		//don't knowingly move into danger. mostly relevant for civilians
		if (iNewDanger > GetCurrHitPoints() && (iNewDanger > iOldDanger || iNewDanger==INT_MAX))
		{
			ClearPathCache();
			return MOVE_RESULT_CANCEL;
		}
	}

	//hack ...
	SetSpottedEnemy(false);

	//todo: consider movement flags here. especially turn destination, not only path destination
	bool bMoved = UnitMove(pPathPlot, IsCombatUnit(), NULL, bDone);

	if (iFlags & CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED)
	{
		if (HasSpottedEnemy())
		{
			ClearPathCache();
			return MOVE_RESULT_CANCEL;
		}
	}

	int iETA = 0;
	if (!m_kLastPath.empty())
	{
		iETA = m_kLastPath.back().m_iTurns;

		if (bMoved)
		{
			//this plot has now been consumed
			m_kLastPath.pop_front();

			//keep the cache in sync
			m_uiLastPathCacheOrigin = plot()->GetPlotIndex();

			//have we used up all plots or at least for this turn?
			if (m_kLastPath.empty())
			{
				return MOVE_RESULT_NO_TARGET;
			}
			else if (m_kLastPath.front().m_iTurns > 0)
			{
				//we will need to recalculate next turn anyway, but deleting the path would abort the mission
				for (size_t i=0; i<m_kLastPath.size(); i++)
					m_kLastPath[i].m_iTurns--;

				//means we're done for this turn
				finishMoves();
				return MOVE_RESULT_NEXT_TURN;
			}
		}
		else
		{
			ClearPathCache(); //failed to move, recalculate.
			return MOVE_RESULT_CANCEL;
		}
	}

	//return value must be greater than zero
	return max(0,iETA);
}

//	---------------------------------------------------------------------------
// Returns true if we want to continue next turn or false if we are done
bool CvUnit::UnitRoadTo(int iX, int iY, int iFlags)
{
	//do we have movement points left?
	if (!canMove())
		return true; //continue next turn

	//first check if we can continue building on the current plot
	BuildTypes eBestRouteBuildInPlot = NO_BUILD;
	const RouteTypes eBestRouteInPlot = GetBestBuildRouteForRoadTo(plot(), &eBestRouteBuildInPlot);
	const CvPlayer& kPlayer = GET_PLAYER(getOwner());

	bool bGetSameBenefitFromTrait = kPlayer.GetSameRouteBenefitFromTrait(plot(), eBestRouteInPlot);
	if(!bGetSameBenefitFromTrait && eBestRouteBuildInPlot != NO_BUILD && UnitBuild(eBestRouteBuildInPlot))
		return true;

	//are we at the target plot? then there's nothing else to do
	if (at(iX, iY))
		return false;

	RouteTypes ePlayerBestRoute = kPlayer.getBestRoute();

	if (MOD_GLOBAL_QUICK_ROUTES && kPlayer.isHuman() && ePlayerBestRoute > ROUTE_ROAD) {
		if (!IsPlotConnectedToPlot(getOwner(), plot(), LastMissionPlot(), ROUTE_ROAD, false, false))
			ePlayerBestRoute = ROUTE_ROAD;
	}

	const BuildTypes ePlayerBestRouteBuild = kPlayer.GetBuilderTaskingAI()->GetBuildRoute(ePlayerBestRoute);

	//ok apparently we both can move and need to move
	//do not use the path cache here, the step finder tells us where to put the route
	SPathFinderUserData data(getOwner(), PT_BUILD_ROUTE, ePlayerBestRouteBuild, ePlayerBestRoute, PURPOSE_MANUAL, false);
	SPath path = GC.GetStepFinder().GetPath(getX(), getY(), iX, iY, data);

	//index zero is the current plot!
	CvPlot* pNextPlot = path.vPlots.size() > 1 ? path.get(1) : NULL;
	if(!pNextPlot || !canMoveInto(*pNextPlot, iFlags | MOVEFLAG_DESTINATION))
	{
		// add route interrupted notification
		CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
		if(pNotifications && isHuman())
		{
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ROUTE_TO_CANCELLED");
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ROUTE_TO_CANCELLED");
			pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strSummary, getX(), getY(), -1);
		}

#ifdef LOG_UNIT_MOVES
		if(!pPathPlot)
			LOG_UNIT_MOVES_MESSAGE("pPathPlot is NULL");
		else
			LOG_UNIT_MOVES_MESSAGE_OSTR(std::string("Cannot move into pPathPlot ") << pPathPlot->getX() << std::string(" ") << pPathPlot->getY());
#endif
		return false;
	}

	return UnitMove(pNextPlot, IsCombatUnit(), NULL, true);
}


//	--------------------------------------------------------------------------------
// Returns true if build should continue...
bool CvUnit::UnitBuild(BuildTypes eBuild)
{
	VALIDATE_OBJECT
	CvPlot* pPlot = plot();
	bool bContinue = false;

	CvAssert(getOwner() != NO_PLAYER);
	CvAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
	if (pkBuildInfo)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(IsAutomated())
			{
				if((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != (ImprovementTypes)(GD_INT_GET(RUINS_IMPROVEMENT))))
				{
					ResourceTypes eResource = (ResourceTypes)pPlot->getNonObsoleteResourceType(GET_PLAYER(getOwner()).getTeam());
					if ((eResource == NO_RESOURCE) || !GC.getImprovementInfo(eImprovement)->IsConnectsResource(eResource))
					{
						if(GC.getImprovementInfo(eImprovement)->GetImprovementPillage() != NO_IMPROVEMENT)
						{
							return false;
						}
					}
				}
			}
		}
	}

	CvAssertMsg(atPlot(*pPlot), "Unit is expected to be at pPlot");

	// Don't check for Gold cost here (2nd false) because this function is called from continueMission... we spend the Gold then check to see if we can Build
	if(canBuild(pPlot, eBuild, false, false))
	{
		bContinue = true;

		if(build(eBuild))
		{
			bContinue = false;
		}
	}

	return bContinue;
}

//	--------------------------------------------------------------------------------
bool CvUnit::CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility)
{
	VALIDATE_OBJECT
	CvAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if(IsBusy())
	{
		return false;
	}

	switch(eInterfaceMode)
	{
	case INTERFACEMODE_MOVE_TO:
		if((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE) && (!IsImmobile()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_MOVE_TO_TYPE:
		break;

	case INTERFACEMODE_MOVE_TO_ALL:
		if (MOD_LINKED_MOVEMENT && IsCombatUnit() && getDomainType() != DOMAIN_AIR && GetNumOwningPlayerUnitsAdjacent() > 0)
		{
			return true;
		}
		break;

	case INTERFACEMODE_ROUTE_TO:
		if(canBuildRoute())
		{
			return true;
		}
		break;

	case INTERFACEMODE_AIRLIFT:
		if(canAirlift(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_NUKE:
		if(canNuke())
		{
			return true;
		}
		break;

	case INTERFACEMODE_PARADROP:
		if(canParadrop(plot(), bTestVisibility))
		{
			return true;
		}
		break;

	case INTERFACEMODE_RANGE_ATTACK:
		if(canRangeStrike() && getDomainType() != DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_CITY_RANGE_ATTACK:
		break;

	case INTERFACEMODE_ATTACK:
		if(IsCanAttackWithMove() && !isOutOfAttacks())
		{
			if(!GetPlotsWithEnemyInMovementRange(false, IsCityAttackSupport()).empty() || bTestVisibility)
			{
				return true;
			}
		}

		break;

	case INTERFACEMODE_AIRSTRIKE:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canRangeStrike())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_AIR_SWEEP:
		if(getDomainType() == DOMAIN_AIR)
		{
			if(canAirSweep())
			{
				return true;
			}
		}
		break;

	case INTERFACEMODE_REBASE:
		if(getDomainType() == DOMAIN_AIR)
		{
			return true;
		}
		break;

	case INTERFACEMODE_EMBARK:
		if(canEmbarkAtPlot(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_DISEMBARK:
		if(canDisembarkAtPlot(plot()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_GIFT_UNIT:
		if(CanDistanceGift((PlayerTypes) DLLUI->GetInterfaceModeValue()))
		{
			return true;
		}
		break;

	case INTERFACEMODE_ESTABLISH_TRADE_ROUTE:
		if (canMakeTradeRoute(plot()))
		{
			return true;
		}

	default:
		break;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// MISSION ROUTINES
//////////////////////////////////////////////////////////////////////////

#if defined(MOD_BALANCE_CORE_MILITARY)

const char* aTrTypes[] = {
	"Gold", 
	"Food", 
	"Production",
	"Resource"
};

const char* CvUnit::GetMissionInfo()
{
	m_strMissionInfoString.clear();
	getUnitAIString( m_strMissionInfoString, getUnitInfo().GetDefaultUnitAIType() );

	m_strMissionInfoString += " // ";

	if ( (m_eTacticalMove==AI_TACTICAL_MOVE_NONE) && (m_eHomelandMove==AI_HOMELAND_MOVE_NONE) )
		m_strMissionInfoString += "no move assigned";
	else
	{
		if (m_eHomelandMove==AI_HOMELAND_MOVE_NONE)
			m_strMissionInfoString += tacticalMoveNames[m_eTacticalMove];

	if (m_eTacticalMove==AI_TACTICAL_MOVE_NONE)
			m_strMissionInfoString += homelandMoveNames[m_eHomelandMove];
	}

	if (IsCombatUnit())
	{
		m_strMissionInfoString += " : ";
		m_strMissionInfoString += getTacticalZoneInfo();
	}
	else if (m_eGreatPeopleDirectiveType!=NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		m_strMissionInfoString += " // ";
		m_strMissionInfoString += directiveNames[m_eGreatPeopleDirectiveType];
	}
	else if (isTrade())
	{
		CvGameTrade* pTrade = GC.getGame().GetGameTrade();
		int iTrIndex = pTrade->GetIndexFromUnitID(GetID(),getOwner());
		if (iTrIndex>=0)
		{
			const TradeConnection* pTradeConnection = pTrade->GetConnectionFromIndex(iTrIndex);
			if (pTradeConnection)
			{
				CvCity* pFromCity = GET_PLAYER(pTradeConnection->m_eOriginOwner).getCity(pTradeConnection->m_iOriginID);
				CvCity* pToCity = GET_PLAYER(pTradeConnection->m_eDestOwner).getCity(pTradeConnection->m_iDestID);
				CvString strTemp0 = CvString::format("%s from %s to %s, %d turns to go", 
					pTradeConnection->m_eConnectionType<NUM_TRADE_CONNECTION_TYPES ? aTrTypes[pTradeConnection->m_eConnectionType] : "unknown",
					pFromCity ? pFromCity->getName().c_str() : "unknown", pToCity ? pToCity->getName().c_str() : "unknown", 
					pTradeConnection->m_iTurnRouteComplete-GC.getGame().getGameTurn());

				m_strMissionInfoString += " // ";
				m_strMissionInfoString += strTemp0;
			}
		}
	}

	CvString strTemp;
	getActivityTypeString(strTemp, GetActivityType());
	m_strMissionInfoString += " // ";
	m_strMissionInfoString += strTemp;

	if (m_iMissionAIX!=INVALID_PLOT_COORD && m_iMissionAIY!=INVALID_PLOT_COORD)
	{
		getMissionAIString(strTemp, GetMissionAIType());
		m_strMissionInfoString += " // ";
		m_strMissionInfoString += strTemp;
		strTemp.Format(" target: %d,%d", m_iMissionAIX, m_iMissionAIY);
		m_strMissionInfoString += strTemp;
	}

	if (GetHeadMissionData())
	{
		strTemp.Format(" // Mission %d -> %d,%d", GetHeadMissionData()->eMissionType, 
			GetHeadMissionData()->iData1, GetHeadMissionData()->iData2);
		m_strMissionInfoString += strTemp;
	}

	m_strMissionInfoString += " -----------------------------";

	//this works because it's a member variable!
	return m_strMissionInfoString.c_str();
}

//for debugging: calculate unit danger in all the plots around us
void CvUnit::DumpDangerInNeighborhood()
{
	CvString fname = CvString::format( "Unit%08d_Turn%03d_Danger.txt", GetID(), GC.getGame().getGameTurn() );
	FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
	if (!pLog)
		return;

	pLog->Msg( "#x,y,revealed,terrain,state,danger\n" );

	int iRange = 9;
	for (int x = -iRange; x<=iRange; x++)
	for (int y = -iRange; y<=iRange; y++)
	{
		CvPlot* pPlot = GC.getMap().plot( m_iX+x, m_iY+y );
		if (!pPlot)
			continue;

		int iDanger = GetDanger(pPlot);
		bool bVisible = pPlot->isVisible( GET_PLAYER(m_eOwner).getTeam() );
		bool bHasVisibleEnemyDefender = pPlot->isVisibleEnemyDefender(this);
		bool bHasEnemyUnit = pPlot->getBestDefender(NO_PLAYER,m_eOwner,this)!=NULL;
		bool bHasEnemyCity = pPlot->isEnemyCity(*this);

		int iState = (((int)bHasVisibleEnemyDefender)<<3) + (((int)bHasEnemyCity)<<2) + (((int)bHasEnemyUnit)<<1);

		pLog->Msg( CvString::format( "%03d,%03d,%d,%d,%d,%d\n", 
			pPlot->getX(), pPlot->getY(), bVisible, pPlot->getTerrainType(), iState, iDanger ).c_str() );
	}

	pLog->Close();
}

#endif

//	--------------------------------------------------------------------------------
/// Queue up a new mission
void CvUnit::PushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	VALIDATE_OBJECT

	//for debugging
	if (g_bFreezeUnits)
	{
		finishMoves();
		return;
	}

	//plausi check
	if (eMission == CvTypes::getMISSION_RANGE_ATTACK() && !canRangeStrikeAt(iData1, iData2))
	{
		CUSTOMLOG("illegal range strike target (%d,%d)!\n",iData1,iData2);
		return;
	}

	//comfort feature for humans
	if (eMission == CvTypes::getMISSION_MOVE_TO() && isHuman())
		iFlags |= MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;

	//safety check - air units should not use ranged attack missions (for unknown reasons)
	if ( getDomainType()==DOMAIN_AIR && eMission==CvTypes::getMISSION_RANGE_ATTACK() )
		eMission = CvTypes::getMISSION_MOVE_TO();

	static bool bMissionActive = false;
	if (bMissionActive)
		OutputDebugString("warning, unit mission being pushed while a mission is being executed\n");
	else
		bMissionActive = true;

	CvUnitMission::PushMission(this, eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit);

	//done
	bMissionActive = false;
}

//	--------------------------------------------------------------------------------
/// Retrieve next mission
void CvUnit::PopMission()
{
	VALIDATE_OBJECT
	CvUnitMission::PopMission(this);
}

//	--------------------------------------------------------------------------------
/// Perform automated mission
void CvUnit::AutoMission()
{
	VALIDATE_OBJECT
	CvUnitMission::AutoMission(this);
}

//	--------------------------------------------------------------------------------
/// Periodic update routine to advance the state of missions
void CvUnit::UpdateMission()
{
	VALIDATE_OBJECT
	CvUnitMission::UpdateMission(this);
}

//	--------------------------------------------------------------------------------
/// Where does this mission end?
CvPlot* CvUnit::LastMissionPlot() const
{
	VALIDATE_OBJECT
	return CvUnitMission::LastMissionPlot(this);
}

//	--------------------------------------------------------------------------------
/// Eligible to start a new mission?
bool CvUnit::CanStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible)
{
	VALIDATE_OBJECT
	return CvUnitMission::CanStartMission(this, iMission, iData1, iData2, pPlot, bTestVisible);
}

//	--------------------------------------------------------------------------------
/// Retrieve new value for mission timer
int CvUnit::GetMissionTimer() const
{
	VALIDATE_OBJECT
	return m_iMissionTimer;
}

//	--------------------------------------------------------------------------------
/// Set value from mission timer
void CvUnit::SetMissionTimer(int iNewValue)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	int iOldTimer = m_iMissionTimer;

	m_iMissionTimer = iNewValue;
	CvAssert(GetMissionTimer() >= 0);

	int iNewTimer = m_iMissionTimer;

	if((iOldTimer == 0) != (iNewTimer == 0))
	{
		if((getOwner() == GC.getGame().getActivePlayer()) && IsSelected())
		{
			DLLUI->setDirty(UnitInfo_DIRTY_BIT, true);
		}

		if(iNewTimer == 0)
		{
			CvInterfacePtr<ICvUnit1> pDllUnit(new CvDllUnit(this));
			gDLL->GameplayUnitMissionEnd(pDllUnit.get());
		}
	}
}

//	--------------------------------------------------------------------------------
/// Change mission timer by a delta value
void CvUnit::ChangeMissionTimer(int iChange)
{
	VALIDATE_OBJECT
	SetMissionTimer(GetMissionTimer() + iChange);
}

//	--------------------------------------------------------------------------------
/// Clear all queued missions
void CvUnit::ClearMissionQueue(bool bKeepPathCache, int iUnitCycleTimer)
{
	VALIDATE_OBJECT
	CvAssert(getOwner() != NO_PLAYER);

	//make sure to show the latest status in the GUI
	PublishQueuedVisualizationMoves();
	
	CvUnitMission::ClearMissionQueue(this, bKeepPathCache, iUnitCycleTimer);
}

//	--------------------------------------------------------------------------------
/// How may missions are in the queue
int CvUnit::GetLengthMissionQueue()	const
{
	VALIDATE_OBJECT
	return m_missionQueue.getLength();
}

//	---------------------------------------------------------------------------
/// Retrieve the data for the first mission in the queue (const correct version)
const MissionData* CvUnit::GetHeadMissionData() const
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength())
		return (m_missionQueue.head());
	return NULL;
}

//	---------------------------------------------------------------------------
const MissionData* CvUnit::GetMissionData(int iIndex) const
{
	VALIDATE_OBJECT
	if(iIndex >= 0 && iIndex < m_missionQueue.getLength())
		return m_missionQueue.getAt(iIndex);

	return NULL;
}

//	--------------------------------------------------------------------------------
/// Retrieve the first mission in the queue (const correct version)
const MissionData* CvUnit::HeadMissionData() const
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}

//	--------------------------------------------------------------------------------
/// Retrieve the first mission in the queue
MissionData* CvUnit::HeadMissionData()
{
	VALIDATE_OBJECT
	if(m_missionQueue.getLength() > 0)
	{
		return m_missionQueue.head();
	}
	else
	{
		return NULL;
	}
}

//	--------------------------------------------------------------------------------
/// The plot saved with the mission AI data
CvPlot* CvUnit::GetMissionAIPlot() const
{
	VALIDATE_OBJECT
	return GC.getMap().plotCheckInvalid(m_iMissionAIX, m_iMissionAIY);
}

//	--------------------------------------------------------------------------------
/// What is the AI type of the mission?
MissionAITypes CvUnit::GetMissionAIType() const
{
	VALIDATE_OBJECT
	return m_eMissionAIType;
}

//	--------------------------------------------------------------------------------
/// Set AI type of the mission
void CvUnit::SetMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	VALIDATE_OBJECT
	m_eMissionAIType = eNewMissionAI;

	if(pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX();
		m_iMissionAIY = pNewPlot->getY();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if(pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->GetIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}
}

//	--------------------------------------------------------------------------------
/// Which unit is saved with the mission AI data
CvUnit* CvUnit::GetMissionAIUnit()
{
	VALIDATE_OBJECT
	return GetPlayerUnit(m_missionAIUnit);
}

// COMBAT ELIGIBILITY ROUTINES

//	--------------------------------------------------------------------------------
/// Is this unit able to ground attack?
bool CvUnit::IsCanAttackWithMove() const
{
	//ranged units may have melee strength but only for defending
	return IsCombatUnit() && !IsCanAttackRanged() && !isOnlyDefensive();
}

//	--------------------------------------------------------------------------------
/// Does this unit have a ranged attack?
bool CvUnit::IsCanAttackRanged() const
{
	VALIDATE_OBJECT
	//need the range check to exclude impi
	return (GetRange() > 0 && GetBaseRangedCombatStrength() > 0);
}

//	--------------------------------------------------------------------------------
/// Can the unit attack either by range or ground attack?
bool CvUnit::IsCanAttack() const
{
	VALIDATE_OBJECT
	return IsCanAttackRanged() || (IsCombatUnit() && !isOnlyDefensive());
}

//	--------------------------------------------------------------------------------
/// Any special conditions on the unit right now that prevents an attack?
bool CvUnit::IsCanAttackWithMoveNow() const
{
	VALIDATE_OBJECT
	return canMove() && IsCanAttackWithMove() && canEndTurnAtPlot(plot());
}

//	--------------------------------------------------------------------------------
// Unit able to fight back when attacked?
//	--------------------------------------------------------------------------------
bool CvUnit::IsCanDefend() const
{
	// This will catch noncombatants (and air units!)
	if(GetBaseCombatStrength() == 0)
	{
		return false;
	}

	if(isDelayedDeath())
	{
		return false;
	}

	if (isCargo())
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsCivilianUnit() const
{
	//aircraft are not combat units but can attack ranged ...
	return !IsCombatUnit() && !IsCanAttackRanged() && !canNuke();
}

// Combat eligibility routines
bool CvUnit::IsCombatUnit() const
{
	return (m_iBaseCombat > 0);
}


//	--------------------------------------------------------------------------------
// get all tiles a unit can reach in one turn - this ignores friendly stacking. need to check the result by hand!
//	--------------------------------------------------------------------------------
ReachablePlots CvUnit::GetAllPlotsInReachThisTurn(bool bCheckTerritory, bool bCheckZOC, bool bAllowEmbark, int iMinMovesLeft) const
{
	int iFlags = CvUnit::MOVEFLAG_IGNORE_STACKING_SELF | CvUnit::MOVEFLAG_IGNORE_DANGER;

	if (!bCheckTerritory)
		iFlags |= CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE;
	if (!bCheckZOC)
		iFlags |= CvUnit::MOVEFLAG_IGNORE_ZOC;
	if (!bAllowEmbark)
		iFlags |= CvUnit::MOVEFLAG_NO_EMBARK;

	return TacticalAIHelpers::GetAllPlotsInReachThisTurn(this, plot(), iFlags, iMinMovesLeft);
}

//	--------------------------------------------------------------------------------
vector<int> CvUnit::GetPlotsWithEnemyInMovementRange(bool bOnlyFortified, bool bOnlyCities, int iMaxPathLength)
{
	vector<int> result;

	VALIDATE_OBJECT
	ReachablePlots plots = GetAllPlotsInReachThisTurn(true, true, false);
	for (ReachablePlots::const_iterator it=plots.begin(); it!=plots.end(); ++it)
	{
		if (it->iPathLength > iMaxPathLength)
			continue;

		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
		if(pPlot->isVisible(getTeam()) && (pPlot->isVisibleEnemyUnit(this) || pPlot->isEnemyCity(*this)))
		{
			if (canMoveInto(*pPlot, CvUnit::MOVEFLAG_ATTACK))
			{
				if(bOnlyFortified)
				{
					CvUnit* pEnemyUnit = pPlot->getVisibleEnemyDefender(getOwner());
					if (pEnemyUnit && pEnemyUnit->IsFortified())
						result.push_back(it->iPlotIndex);
				}
				else if(bOnlyCities)
				{
					if(pPlot->isEnemyCity(*this))
						result.push_back(it->iPlotIndex);
				}
				else
					result.push_back(it->iPlotIndex);
			}
		}
	}

	return result;
}

// PATH-FINDING ROUTINES
bool CvUnit::IsCachedPathValid() const
{
	return m_uiLastPathCacheOrigin != -1;
}

bool CvUnit::HaveCachedPathTo(const CvPlot* pToPlot, int iFlags) const
{
	return (
		m_uiLastPathCacheOrigin == plot()->GetPlotIndex() &&
		m_uiLastPathCacheDestination == pToPlot->GetPlotIndex() && 
		m_uiLastPathFlags == (iFlags & PATHFINDER_FLAG_MASK) &&
		m_uiLastPathTurnSlice == GC.getGame().getTurnSlice()
		);
}

//	--------------------------------------------------------------------------------
/// Use pathfinder to create a path (protected, allows caching)
bool CvUnit::GeneratePath(const CvPlot* pToPlot, int iFlags, int iMaxTurns, int* piPathTurns)
{
	if(pToPlot == NULL)
		return false;

	//can we re-use the old path?
	if (HaveCachedPathTo(pToPlot, iFlags))
	{
		if (piPathTurns == NULL)
			return true;
		else
		{
			if (m_kLastPath.empty())
				//we're already there (at least approximately)!
				*piPathTurns = 0;
			else
			{
				*piPathTurns = m_kLastPath.back().m_iTurns;
				if ((iFlags & CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN) && m_kLastPath.back().m_iMoves == 0)
					(*piPathTurns)++;
			}

			return true;
		}
	}
	else
	{
		int iTurns = ComputePath(pToPlot, iFlags, iMaxTurns, true);
		if (piPathTurns)
			*piPathTurns = iTurns;
		return iTurns!=INT_MAX;
	}
}

//	--------------------------------------------------------------------------------
/// Clear the pathing cache
void CvUnit::ClearPathCache()
{
	m_kLastPath.clear();
	m_uiLastPathCacheOrigin = -1;
	m_uiLastPathCacheDestination = -1;
	m_uiLastPathFlags = 0xFFFFFFFF;
	m_uiLastPathTurnSlice = -1;
}

//	--------------------------------------------------------------------------------
/// Where do we end this next move?
CvPlot* CvUnit::GetPathEndFirstTurnPlot() const
{
	if (IsCachedPathValid())
		return m_kLastPath.GetTurnDestinationPlot(0);

	return NULL;
}

CvPlot* CvUnit::GetPathFirstPlot() const
{
	if (IsCachedPathValid())
		return m_kLastPath.GetFirstPlot();

	return NULL;
}

CvPlot* CvUnit::GetPathLastPlot() const
{
	if (!IsCachedPathValid())
		return NULL;

	if (m_kLastPath.empty()) //with approximate pathfinding this is a valid case
		return plot();
	else
		return m_kLastPath.GetFinalPlot();
}

int CvUnit::GetMovementPointsAtCachedTarget() const
{
	if (!IsCachedPathValid() || m_kLastPath.empty())
		return -1;

	return m_kLastPath.back().m_iMoves;
}

CvPlot * CvUnit::GetLastValidDestinationPlotInCachedPath() const
{
	if (!IsCachedPathValid() || m_kLastPath.empty())
		return NULL;

	for (size_t i = m_kLastPath.size() - 1; i > 0; i--)
	{
		CvPlot* pPlot = m_kLastPath.GetPlotByIndex(i);
		if (pPlot && canMoveInto(*pPlot, CvUnit::MOVEFLAG_DESTINATION))
			return pPlot;
	}

	return NULL;
}

const CvPathNodeArray& CvUnit::GetLastPath() const
{
	return m_kLastPath;
}

bool CvUnit::CachedPathIsSafeForCivilian() const
{
	//check if the unit can be captured this turn
	CvPlot* pCheck = GetPathEndFirstTurnPlot();
	return !pCheck || GET_PLAYER(m_eOwner).GetPlotDanger(*pCheck,false) < GetCurrHitPoints();
}

// PRIVATE METHODS

//	--------------------------------------------------------------------------------
bool CvUnit::canAdvance(const CvPlot& plot, int iThreshold) const
{
	VALIDATE_OBJECT
	CvAssert(IsCombatUnit());
	CvAssert(getDomainType() != DOMAIN_AIR);
	CvAssert(getDomainType() != DOMAIN_IMMOBILE);

	if(plot.getNumVisibleEnemyDefenders(this) > iThreshold)
	{
		return false;
	}

	if(isNoCapture())
	{
		if(plot.isEnemyCity(*this))
		{
			return false;
		}
	}

	return true;
}

//	--------------------------------------------------------------------------------
CvUnit* CvUnit::rangeStrikeTarget(const CvPlot& targetPlot, bool bNoncombatAllowed) const
{
	VALIDATE_OBJECT
	// All defaults, except test for war, and allow noncombat units
	CvUnit* pDefender = targetPlot.getBestDefender(NO_PLAYER, getOwner(), this, true, false, false, bNoncombatAllowed);

	if(pDefender && !pDefender->IsDead() && !pDefender->isDelayedDeath())
		return pDefender;

	return NULL;
}

void CvUnit::DoPlagueTransfer(CvUnit& defender)
{
	//We got here without being able to plague someone? Abort!
	if (!CanPlague(&defender))
		return;

	vector<int> vInflictedPlagues = GetInflictedPlagueIDs();

	// This unit does not inflict plagues.
	if (vInflictedPlagues.size() == 0)
		return;

	int iCount = vInflictedPlagues.size() + 1;

	for (std::vector<int>::iterator it = vInflictedPlagues.begin(); it != vInflictedPlagues.end(); it++)
	{
		int iPlagueChance = 0;
		PromotionTypes eInflictedPlague = GetInflictedPlague(*it, iPlagueChance);
		CvPromotionEntry* pkPlaguePromotionInfo = GC.getPromotionInfo(eInflictedPlague);

		// Is the plague inflicted?
		int iRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x09527e40).mix(GetID()).mix(defender.GetID()).mix(iCount));
		iCount--;
		if (iRoll > iPlagueChance)
			return;

		// Is the defender immune to the plague?
		if (defender.ImmuneToPlague(*it))
			return;

		// Does the defender already have a stronger version of this plague?
		if (defender.HasPlague(*it, pkPlaguePromotionInfo->GetPlaguePriority() + 1))
			return;

		// This might kill ye a little!
		defender.setHasPromotion(eInflictedPlague, true);

		// Remove any excess movement the defender now has (plagues can reduce max moves)
		if (defender.getMoves() > defender.maxMoves())
			defender.setMoves(defender.maxMoves());

		const char* szPromotionDesc = pkPlaguePromotionInfo->GetDescription();
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString szMsg;
			szMsg.Format("Promotion, %s, Transferred by %s to %s in melee",
							szPromotionDesc, getName().GetCString(), defender.getName().GetCString());
			GET_PLAYER(m_eOwner).GetTacticalAI()->LogTacticalMessage(szMsg);
		}
		
		if (MOD_BALANCE_CORE_JFD)
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if (pNotifications)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_PLAGUE_TRANSFER");
				strMessage << getUnitInfo().GetTextKey();
				strMessage << defender.getUnitInfo().GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_PLAGUE_TRANSFER_S");
				strSummary << getUnitInfo().GetTextKey();
				pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
			}
			CvNotifications* pNotificationsOther = GET_PLAYER(defender.getOwner()).GetNotifications();
			if (pNotificationsOther)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_PLAGUE_TRANSFER_THEM");
				strMessage << getUnitInfo().GetTextKey();
				strMessage << defender.getUnitInfo().GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_PLAGUE_TRANSFER_THEM_S");
				strSummary << defender.getUnitInfo().GetTextKey();

				pNotificationsOther->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
			}
		}
		else
		{
			CvNotifications* pNotifications = GET_PLAYER(getOwner()).GetNotifications();
			if (pNotifications && iPlagueChance < 100)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_PROMOTION_TRANSFER");
				strMessage << getUnitInfo().GetTextKey();
				strMessage << defender.getUnitInfo().GetTextKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_PROMOTION_TRANSFER_S");
				strSummary << getUnitInfo().GetTextKey();
				pNotifications->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
			}
			CvNotifications* pNotificationsOther = GET_PLAYER(defender.getOwner()).GetNotifications();
			if (pNotificationsOther)
			{
				Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_PROMOTION_TRANSFER_THEM");
				strMessage << getUnitInfo().GetTextKey();
				strMessage << defender.getUnitInfo().GetTextKey();
				strMessage << szPromotionDesc;
				Localization::String strSummary = Localization::Lookup("TXT_KEY_UNIT_PROMOTION_TRANSFER_S_THEM_S");
				strSummary << defender.getUnitInfo().GetTextKey();

				pNotificationsOther->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), getX(), getY(), (int)getUnitType(), getOwner());
			}
		}
	}
}

//	--------------------------------------------------------------------------------

int CvUnit::GetNumFallBackPlotsAvailable(const CvUnit& attacker) const
{
	VALIDATE_OBJECT

	// this should not happen. fixme
	if (!onMap() || !attacker.onMap())
		return 0;

	if (isEmbarked() && getDomainType() == DOMAIN_LAND)
		return 0;

	// Are some of the retreat hexes away from the attacker blocked?
	int iFreeHexes = 3;
	DirectionTypes eAttackDirection = directionXY(attacker.plot(), plot());
	int iBiases[3] = { 0,-1,1 };

	for (int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + iBiases[i]) % NUM_DIRECTION_TYPES;
		CvPlot* pDestPlot = plotDirection(getX(), getY(), (DirectionTypes)iMovementDirection);

		if (pDestPlot && !canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION | MOVEFLAG_NO_EMBARK))
		{
			iFreeHexes--;
		}
	}

	return iFreeHexes;
}

int CvUnit::GetWithdrawChance(const CvUnit& attacker) const
{
	VALIDATE_OBJECT
	int iWithdrawChance = getExtraWithdrawal();
	if (iWithdrawChance == 0)
		return 0;

	int iNumFallBackPlotsAvailable = GetNumFallBackPlotsAvailable(attacker);

	if (iNumFallBackPlotsAvailable == 0)
		return 0;	

	if (MOD_BALANCE_VP)
	{
		return getHasWithdrawnThisTurn() ? 0 : iWithdrawChance;
	}
	else
	{
		// Does attacker have a greater speed than defender? Reduce withdrawal chance for each point the attacker is faster
		int iDefenderMovementRange = baseMoves(isEmbarked());
		int iAttackerMovementRange = attacker.baseMoves(attacker.isEmbarked());
		if (iAttackerMovementRange > iDefenderMovementRange)
			iWithdrawChance += (/*-20*/ GD_INT_GET(WITHDRAW_MOD_ENEMY_MOVES) * (iAttackerMovementRange - iDefenderMovementRange));

		iWithdrawChance += (/*-20*/ GD_INT_GET(WITHDRAW_MOD_BLOCKED_TILE) * (3 - iNumFallBackPlotsAvailable));
		return iWithdrawChance;
	}
}

// returns true if the unit should withdraw from a melee attack
bool CvUnit::CheckWithdrawal(const CvUnit& attacker) const
{
	VALIDATE_OBJECT
	int iWithdrawChance = GetWithdrawChance(attacker);
	if (iWithdrawChance == 0)
		return false;

	if (iWithdrawChance == 100)
		return true;

	//include damage so the result changes for each attack
	int iRoll = GC.getGame().randRangeExclusive(0, 10, CvSeeder(plot()->GetPseudoRandomSeed()).mix(GetID()).mix(getDamage())) * 10;
	return iRoll < iWithdrawChance;
}
//	--------------------------------------------------------------------------------
bool CvUnit::DoFallBack(const CvUnit& attacker, bool bWithdraw, bool bCaptured)
{
	VALIDATE_OBJECT

	CvPlot* pDestPlot = NULL;
	CvPlot* pAttackerFromPlot = attacker.plot();
	DirectionTypes eAttackDirection = directionXY(pAttackerFromPlot, plot());
	std::vector<CvUnit*> aEscortedUnits;
	std::vector<CvPlot*> aValidPlotList;

	// Store escorted units for later
	if (MOD_CIVILIANS_RETREAT_WITH_MILITARY || bCaptured)
	{
		CvPlot* pUnitPlot = plot();
		IDInfoVector currentUnits;
		if (pUnitPlot->getUnits(&currentUnits) > 0)
		{
			for (IDInfoVector::const_iterator itr = currentUnits.begin(); itr != currentUnits.end(); ++itr)
			{
				CvUnit* pLoopUnit = (CvUnit*)GetPlayerUnit(*itr);

				if (pLoopUnit && pLoopUnit != this && !pLoopUnit->isDelayedDeath())
				{
					// If this unit was captured, only stacked military units should withdraw - civilians should be captured as well
					if (!bCaptured || !pLoopUnit->IsCivilianUnit())
						aEscortedUnits.push_back(pLoopUnit);
				}
			}
		}
	}

	// Performance improvement if the unit is captured and can't or doesn't escort anything
	if (bCaptured && aEscortedUnits.empty())
		return true;

	// possible plots to withdraw to are the plot opposite to the attacker and the two plots next to that plot
	for (int i = 0; i < 3; i++)
	{
		int iMovementDirection = (NUM_DIRECTION_TYPES + eAttackDirection + i - 1) % NUM_DIRECTION_TYPES; 
		CvPlot* pDirectionPlot = plotDirection(getX(), getY(), (DirectionTypes)iMovementDirection);

		if (pDirectionPlot && (bCaptured || (isNativeDomain(pDirectionPlot) && canMoveInto(*pDirectionPlot, MOVEFLAG_DESTINATION | MOVEFLAG_NO_EMBARK))))
		{
			// Do not withdraw if we're escorting a unit who can't move here!
			if (aEscortedUnits.size() > 0)
			{
				bool bNoRetreat = false;
				for (size_t j = 0; j < aEscortedUnits.size(); j++)
				{
					if (!aEscortedUnits[j]->canMoveInto(*pDirectionPlot, MOVEFLAG_DESTINATION) || (aEscortedUnits[j]->isEmbarked() && aEscortedUnits[j]->isNativeDomain(pDirectionPlot)) || (!aEscortedUnits[j]->isEmbarked() && !aEscortedUnits[j]->canMoveInto(*pDirectionPlot, MOVEFLAG_NO_EMBARK)))
					{
						bNoRetreat = true;
						break;
					}
				}
				if (bNoRetreat)
					continue;
			}

			aValidPlotList.push_back(pDirectionPlot);
		}
	}
	if (aValidPlotList.size() == 0)
		return false;
	else if (aValidPlotList.size() == 1)
		pDestPlot = aValidPlotList[0];
	else
	{
		// if there's more than one valid plot, select the plot with the fewest number of enemies
		multimap<int, CvPlot*> aNumEnemies;
		for (vector<CvPlot*>::iterator it = aValidPlotList.begin(); it != aValidPlotList.end(); ++it)
		{
			aNumEnemies.insert(make_pair((*it)->GetNumEnemyUnitsAdjacent(getTeam(), getDomainType()), (*it)));
		}

		if (!aNumEnemies.empty())
		{
			// remove plots that have more enemies than the minimum
			multimap<int, CvPlot*>::iterator itRemove = aNumEnemies.lower_bound(aNumEnemies.begin()->first + 1);
			aNumEnemies.erase(itRemove, aNumEnemies.end());

			if (aNumEnemies.size() == 1)
				pDestPlot = aNumEnemies.begin()->second;
			else
			{
				// if there is still more than one possible plot, select the plot with the highest number of adjacent friendly units
				multimap<int, CvPlot*> aNumFriendlies;
				for (multimap<int, CvPlot*>::iterator it = aNumEnemies.begin(); it != aNumEnemies.end(); ++it)
				{
					aNumFriendlies.insert(make_pair(it->second->GetNumFriendlyUnitsAdjacent(getTeam(), getDomainType(), true, this), it->second));
				}

				if (!aNumFriendlies.empty())
				{
					// remove plots that have fewer friendlies than the maximum
					itRemove = aNumFriendlies.lower_bound(aNumFriendlies.rbegin()->first); // highest key is the first key in reverse order
					aNumFriendlies.erase(aNumFriendlies.begin(), itRemove);

					// make a random selection from the remaining plots
					multimap<int, CvPlot*>::iterator itChosenPlot = aNumFriendlies.begin();
					if (aNumFriendlies.size() > 1)
						advance(itChosenPlot, GC.getGame().urandLimitExclusive(aNumFriendlies.size(), CvSeeder(plot()->GetPseudoRandomSeed()).mix(GetID()).mix(getDamage())));
					pDestPlot = itChosenPlot->second;
				}
			}
		}
	}

	if (!pDestPlot)
		return false;

	// Actually do the withdrawal if this unit isn't captured
	if (!bCaptured)
		setXY(pDestPlot->getX(), pDestPlot->getY(), true, true, true, true);

	if (aEscortedUnits.size() > 0)
	{
		for (size_t i = 0; i < aEscortedUnits.size(); i++)
		{
			// Civilian and embarked units also retreat (if possible)
			// Need to check whether the unit can enter the plot again, because it might have changed (stacking rules)
			if (aEscortedUnits[i]->canMoveInto(*pDestPlot, MOVEFLAG_DESTINATION))
			{
				aEscortedUnits[i]->setXY(pDestPlot->getX(), pDestPlot->getY(), true, true, true, true);
				aEscortedUnits[i]->PublishQueuedVisualizationMoves(); // Display the civilians retreating before the escort does
			}
		}
	}

	if (!bCaptured)
	{
		PublishQueuedVisualizationMoves();
		if (bWithdraw)
			m_bHasWithdrawnThisTurn = true;
	}

	return true;
}

//	--------------------------------------------------------------------------------
UnitAITypes CvUnit::AI_getUnitAIType() const
{
	VALIDATE_OBJECT
	return m_eUnitAIType;
}

//	--------------------------------------------------------------------------------
void CvUnit::AI_setUnitAIType(UnitAITypes eNewValue)
{
	VALIDATE_OBJECT
	CvAssertMsg(eNewValue != NO_UNITAI, "NewValue is not assigned a valid value");

	if(AI_getUnitAIType() != eNewValue)
	{
		m_eUnitAIType = eNewValue;
	}
}

//	--------------------------------------------------------------------------------
void CvUnit::AI_promote()
{
	VALIDATE_OBJECT
	PromotionTypes eBestPromotion = NO_PROMOTION;
	int iBestValue = 0;
	int iNumValidPromotions = 0;
	int iLevel = getLevel();
	
	for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		const PromotionTypes ePromotion(static_cast<PromotionTypes>(iI));

		CvPromotionEntry* pkPromotionEntry = GC.getPromotionInfo(ePromotion);

		if (pkPromotionEntry == NULL)
			continue;

		if(canPromote(ePromotion, -1))
		{
			iNumValidPromotions++;
			int iValue = AI_promotionValue(ePromotion);

			//value lower-level promotions a bit less.
			if (pkPromotionEntry->GetPrereqOrPromotion1() == NO_PROMOTION)
			{
				if (iLevel >= 2)
				{
					iValue = iValue * (iLevel - 1);
					iValue /= iLevel + 1;
					// At level 2 has 1/3 chance of picking base promotion again, chance increases back towards 1 as leveled further.
				}
			}
			
			int iNextValue = 0;
			int iBestNextValue = 0;

			for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
			{
				const PromotionTypes eNextPromotion(static_cast<PromotionTypes>(iJ));
				CvPromotionEntry* pkNextPromotionEntry = GC.getPromotionInfo(eNextPromotion);
				if (!pkNextPromotionEntry)
					continue;

				//for some basic forward planning, look at promotions this promotion unlocks and add those to the value.
				if (   pkNextPromotionEntry->GetPrereqOrPromotion1() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion2() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion3() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion4() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion5() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion6() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion7() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion8() == ePromotion
					|| pkNextPromotionEntry->GetPrereqOrPromotion9() == ePromotion)
				{
					iNextValue = AI_promotionValue(eNextPromotion);
					if (iNextValue > iBestNextValue)
					{
						iBestNextValue = iNextValue;
					}
				}

			}
			
			iValue += iBestNextValue / 2;
	
			if(GC.getLogging() && GC.getAILogging())
			{
				CvPromotionEntry* pkPromotionEntry = GC.getPromotionInfo(ePromotion);
				CvString strPromotionDesc = (pkPromotionEntry != NULL) ? pkPromotionEntry->GetDescription() : "Unknown Promotion";
				CvString strUnitName = getName();
				CvString strCivName = GET_PLAYER(getOwner()).getName();
				strPromotionDesc.Replace(' ', '_'); strPromotionDesc.Replace('(', '_'); strPromotionDesc.Replace(')', '_');
				strUnitName.Replace(' ', '_');
				strCivName.Replace(' ', '_');

				FILogFile* pLog = LOGFILEMGR.GetLog("PromotionLog.csv", FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );
				CvString strLog;
				strLog.Format("%03d, %s, ", GC.getGame().getElapsedGameTurns(), strCivName.c_str());
				CvString strMsg;
				strMsg.Format("Promotion, %s, For %s, Ranged %d, Domain %d, Value: %d, Damage: %d", strPromotionDesc.c_str(), strUnitName.c_str(), IsCanAttackRanged() ? 1 : 0, getDomainType(), iValue, getDamage());

				strLog += strMsg;
				pLog->Msg(strLog);
			}

			//add some randomness
			if (iValue > 0)
				iValue += GC.getGame().randRangeExclusive(0, iValue, CvSeeder::fromRaw(0xe1bc5321).mix(plot()->GetPseudoRandomSeed()).mix(iI));

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromotion = ePromotion;
			}
		}
	}

	if(eBestPromotion != NO_PROMOTION)
	{
		promote(eBestPromotion, -1);

		CvPromotionEntry* pkPromoInfo = GC.getPromotionInfo(eBestPromotion);
		if (pkPromoInfo && GC.getLogging() && GC.getAILogging())
		{
			CvString strPromotionDesc = pkPromoInfo->GetDescription();
			CvString strUnitName = getName();
			CvString strCivName = GET_PLAYER(getOwner()).getName();
			strPromotionDesc.Replace(' ', '_'); strPromotionDesc.Replace('(', '_'); strPromotionDesc.Replace(')', '_');
			strUnitName.Replace(' ', '_');
			strCivName.Replace(' ', '_');

			FILogFile* pLog = LOGFILEMGR.GetLog("PromotionLog.csv", FILogFile::kDontTimeStamp | FILogFile::kDontFlushOnWrite );
			CvString strLog;
			strLog.Format("%03d, %s, ", GC.getGame().getElapsedGameTurns(), strCivName.c_str());
			CvString strMsg;
			strMsg.Format("--> Chosen Promotion, %s, Received by %s, ID %d, Lvl %d, XP %d, Ranged %d, Domain %d, X: %d, Y: %d, Damage: %d", 
				strPromotionDesc.c_str(), strUnitName.c_str(), GetID(), getLevel(), getExperienceTimes100(), IsCanAttackRanged() ? 1 : 0, getDomainType(), getX(), getY(), getDamage());

			strLog += strMsg;
			pLog->Msg(strLog);
		}

		//do it again until we're done
		AI_promote();
	}
}

//	--------------------------------------------------------------------------------
// XXX make sure we include any new UnitAITypes...
int CvUnit::AI_promotionValue(PromotionTypes ePromotion)
{
	VALIDATE_OBJECT

	CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
	if(pkPromotionInfo == NULL)
	{
		//This function really really really should not be called with an invalid promotion type.
		CvAssert(pkPromotionInfo);
		return 0;
	}

	double iValue = 0;

	double iExtra;
	int iTemp = 0;
	int iI = 0;

	bool bWarTimePromotion = plot()->getOwner() != NO_PLAYER && GET_PLAYER(plot()->getOwner()).IsAtWarWith(getOwner());

	// Get flavor info we can use
	CvFlavorManager* pFlavorMgr = GET_PLAYER(m_eOwner).GetFlavorManager();
	int iFlavorOffense = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")), 1, 20);
	int iFlavorDefense = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE")), 1, 20);
	int iFlavorCityDefense = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CITY_DEFENSE")), 1, 20);
	int iFlavorRanged = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED")), 1, 20);
	int iFlavorRecon = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON")), 1, 20);
	int iFlavorMobile = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE")), 1, 20);
	int iFlavorNaval = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL")), 1, 20);
	int iFlavorNavalRecon = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON")), 1, 20);
	int iFlavorAir = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR")), 1, 20);
	int iFlavorAntiAir = range(pFlavorMgr->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_ANTIAIR")), 1, 20);

	// If we are damaged, insta heal is the way to go
	if(pkPromotionInfo->IsInstaHeal())
	{
		// Half health or less?
		if(getDamage() >= (GetMaxHitPoints() / 2))
		{
			iValue += 1000;   // Enough to usually lock this one up
		}
	}
#if defined(MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)             // Don't know when if defined is needed
	// Key: M = Melee, mM = Mounted Melee, nM = Naval Melee, S = Siege
	// R = Ranged, mR = Mounted Ranged, nR = Naval Ranged, C = Carrier
	// aB = Air Bomber, aF = Air Fighter, AA = Anti-Air, H = Helicopter.
	
	iTemp = pkPromotionInfo->GetCombatPercent();
	// M + mM: +10 Drill 1-3,Shock 1-3. 	nM: +15 Boarding Party 1-3, +10 Dreadnought 1-3 (coastal raider).
	if(iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		iValue += iExtra;

		// This gives 1 Value per CombatPercent increase * 3 Flavors 
		// Other promotions should be balanced with different flavors and by multiplying by usufulness of the promotion.
		// Also synergistic promotions can be factored in as well as different usefulness for different units eg (melee vs range)
	}

			// General Offense


	iTemp = pkPromotionInfo->GetAttackMod();
	// Sub: +30 Wolfpack 1 - 3.
	if (iTemp != 0)
	{
		iExtra = getAttackModifier();
		iExtra = (iExtra + iTemp) * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		if (IsCanAttackRanged())
			iExtra *= 0.7f;
		else
			iExtra *= 0.4f;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetRangedAttackModifier();
	// R: +10 Accuracy 1-3, +10 Barrage 1-3. 	nR: +10 Bombardment 1-3. 	S: +10 Siege 1-3, Field 1-3. 
	// R + S: -10 Indirect Fire, -20 Range. 	R + mR +nR +S: -30 Logistics.
	if(iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense  + iFlavorDefense + iFlavorCityDefense);
		iExtra *= 0.7f;
		iValue += iExtra;
	}


			// General Defense

	

	iTemp = pkPromotionInfo->GetDefenseMod();
	// Scout: +25 Survivalism 1 - 2.	C: +25 Armor Plating 1 - 3.
	// M: +35 Stalwart (Drill 4).	nR + Sub: +25 Indomitable (targeting 4).
	// mR: -15 March (skirmisher march). 	R: -15 March.
	// M + mM + nM + R + mR + Scout: -10 Medic 1 - 2.
	if(iTemp != 0)
	{
		iExtra = (iTemp) * (2 * iFlavorOffense + iFlavorDefense);
		if (IsCanAttackRanged())
		{
			iExtra *= 0.35f;
		}
		else
			iExtra *= 0.7f;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->ChangeDamageValue();     // modifies damage in taken in each combat.
	// nM: -5 Dauntless (Damage reduction).
	if (iTemp != 0)
	{	
		iExtra = iTemp * ( 2 * iFlavorOffense + iFlavorDefense);
		iExtra *= -6;    			// not sure about this
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetMaxHitPointsChange();
	// nM: +10 Dreanought 1 (coastal raider 1), +15 Dreadnought 2 - 3.
	if (iTemp != 0)
	{
		if (IsCanAttackRanged())
		{
			iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
			iExtra *= 0.5;
		}
		else
		{
			iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
			iExtra *= 1;
		}
		iValue += iExtra;
		
	}
	

	iTemp = pkPromotionInfo->GetRangedDefenseMod();
	// M + nM + R + mR + S: +25 Cover 1 - 2. 	nM: +25 Blockade (coastal raider 4).	
	// M + nM + R + mR + nR: +10 Air Defense 1 - 3.		Zulu: +10 Buffalo Chest & Buffalo Horns.
	// nM: -10 Minelayer.
	if(iTemp != 0)
	{
		iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
		iExtra /= max(1,baseMoves(false));
		if ( IsCanAttackRanged() )
		{
			iExtra /= max(1,GetRange());
		}
		iValue += iExtra;
	}

			// Terrain modifiers
	
	


	iTemp = pkPromotionInfo->GetOpenAttackPercent();    
	// mM: +10 Charge 1,2.		S: +10 Field 1-3.	// does this apply to field promotion?? I believe it does.
	if(iTemp != 0)
	{
		iExtra = getExtraOpenAttackPercent();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorMobile);
		iExtra *= 0.3;
		if(getUnitInfo().IsMounted())
		{
			iExtra *= 2;
		}
		iValue += iExtra;
	}

	

	iTemp = pkPromotionInfo->GetOpenRangedAttackMod();
	if(iTemp != 0 && IsCanAttackRanged())
	{
		iExtra = getExtraOpenRangedAttackMod();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorMobile);
		iExtra *= 0.35;
		if(getUnitInfo().IsMounted())
		{
			iExtra *= 2;
		}
		iValue += iExtra;
	}

	

	iTemp = pkPromotionInfo->GetOpenDefensePercent();
	// M + mM: +15 Formation 1, 2. 
	if(iTemp != 0)
	{
		iExtra = getExtraOpenDefensePercent();
		iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 0.3;
		iValue += iExtra;	
	}


	iTemp = pkPromotionInfo->GetRoughAttackPercent();
	// M: +15 Ambush 1 - 2.
	if(iTemp != 0)
	{
		iExtra = getExtraRoughAttackPercent();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + 2 * iFlavorDefense);
		iExtra *= 0.3;
		if(getUnitInfo().IsMounted())
		{
			iExtra *= 0.5;
		}
		iValue += iExtra;
	}


	iTemp = pkPromotionInfo->GetRoughRangedAttackMod();
	if(iTemp != 0 && IsCanAttackRanged())
	{
		iExtra = getExtraRoughRangedAttackMod();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + 2 * iFlavorDefense);
		iExtra *= 0.35;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetRoughDefensePercent();    
	// M: +10 Woodsman.			// Shouldn't this be just forest?
	if(iTemp != 0)
	{
		iExtra = getExtraRoughDefensePercent();
		iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 0.3;
		if(getUnitInfo().IsMounted())
		{
			iExtra *= 0.5;
		}
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetOutsideFriendlyLandsModifier();
	// Scout: +20 Trailblazer 3. 	R + mR + S: +25 Infiltrators (barrage 4).
	if (iTemp != 0)
	{

		iExtra = getOutsideFriendlyLandsModifier();
		iExtra = ( iTemp + iExtra ) * ( iFlavorMobile + 2 * iFlavorOffense );
		iExtra *= 0.7;
		if (bWarTimePromotion)
			iExtra *= 1.5;
		iValue += iExtra;
	}

			// Other unit modifiers

	


	iTemp = pkPromotionInfo->GetAttackAboveHealthMod();
	// R + mR: +10 Accuracy 1 - 3, +25 Firing Doctrine (accuracy 4).
		
	if (iTemp != 0)
	{
		iExtra = getExtraAttackAboveHealthMod();
		iExtra = ( iTemp + iExtra ) * ( iFlavorDefense + 2 * iFlavorCityDefense);
		if (IsCanAttackRanged())
			iExtra *= 0.45;
		else
			iExtra *= 0.2;
		iValue += iExtra;
	}
	iTemp = pkPromotionInfo->GetAttackBelowHealthMod();
	// R + mR: +10 Barrage 1 - 3.	nR: +10 Targeting 1 - 3.
	// mR: +30 Coup De Grace (skirmisher power)
	if (iTemp != 0)
	{
		iExtra = getExtraAttackBelowHealthMod() + getHPHealedIfDefeatEnemy();
		iExtra = ( iTemp + iExtra ) * ( iFlavorDefense + 2 * iFlavorOffense);
		if (IsCanAttackRanged())
			iExtra *= 0.25;
		else
			iExtra *= 0.4;
		if (getUnitInfo().IsMounted())
			iExtra *= 1.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetAttackWoundedMod();
	// mM: +20 Charge 1 - 2.	R + mR + S: +25 Infiltrators (Barrage 4).
	if(iTemp != 0)
	{
		iExtra = getExtraAttackWoundedMod();
		iExtra = ( iTemp + iExtra ) * ( iFlavorOffense + iFlavorDefense + iFlavorMobile );
		if (IsCanAttackRanged())
			iExtra *= 0.5;
		else
		{
			iExtra *= 0.4;
			if (getUnitInfo().IsMounted())
			{
				iExtra *= 1.6;
			}
		}
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetAdjacentMod();
	// currently not used
	if (iTemp != 0)
	{
		iExtra = (iTemp) * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		iExtra *= 1.6;
		iExtra /= max(1, baseMoves(false));
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetNoAdjacentUnitMod();
	// R + mR + S: +10 Infiltrators (barrage 4).
	if (iTemp != 0)
	{
		iExtra = (iTemp) * (iFlavorMobile + 2 * iFlavorOffense);
		iExtra /= 1.6;
		iExtra *= max(1, baseMoves(false));
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetAdjacentCityDefenseMod();
	// can't be picked currently
	if (iTemp != 0)
	{
		iExtra = (iTemp) * (iFlavorDefense + 4 * iFlavorCityDefense);
		iExtra *= 100 + GetAdjacentCityDefenseMod();
		iExtra /= 100;
		iExtra *= 0.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetFlankAttackModifier();
	// M + mM: +5 Shock 1 - 3, +25 Overrun (Shock 4).	Zulu: +10 Buffalo Chest & Horns.
	// nM: +10 Pincer (Boarding Party) 4.	
	if(iTemp > 0)
	{
		iExtra  = (iTemp) * ( iFlavorDefense + iFlavorOffense + iFlavorMobile);
		iExtra *= 100 + GetFlankAttackModifier();
		iExtra /= 100;
		iExtra *= 0.5;
		iExtra *= (2 + baseMoves(false)) / 2;    		// gives max moves, false = not embarked	
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetExtraFlankPower();
	// Screening: Only recon units can use it now however, so it's not that high of a value.
	// If you have access to flanking bonuses however.. always choose this over other flanking promotions!
	if (iTemp > 0)
	{
		iExtra = (iTemp) * (iFlavorOffense + iFlavorMobile + iFlavorRecon);
		iExtra *= 100 + GetFlankAttackModifier();
		iExtra /= 100;
		iExtra *= (2 + baseMoves(false)) / 2;    		// gives max moves, false = not embarked	
		iValue += iExtra;
	}

	if (pkPromotionInfo->IsRangedFlankAttack() && !IsRangedFlankAttack() && IsCanAttackRanged())
	// Envelopment: Only available to Skirmishers (Mounted Ranged)
	// This is really strong, but if my ranged attacks exceed 1 range, I can't use this as effectively.
	{
		iExtra = (iFlavorOffense + iFlavorMobile);
		iExtra *= 2;
		iExtra *= max(1, baseMoves(false));
		if (canMoveAfterAttacking())
			iExtra *= 5;
		if (GetRange() > 1)
			iExtra /= 10;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetAttackFortifiedMod();
	// S: +50 Volley.
	if(iTemp != 0)
	{
		if (IsCanAttackRanged())
		{
			iExtra = getExtraAttackFortifiedMod();
			iExtra = (iTemp + iExtra) * (2 * iFlavorRanged + iFlavorOffense);
			iExtra *= 0.3;
		}
		else
		{
			iExtra = getExtraAttackFortifiedMod();
			iExtra = (iTemp + iExtra) * (3 * iFlavorOffense);
			iExtra *= 0.15;
		}
		iValue += iExtra;
	}

	// Other modifiers

	iTemp = pkPromotionInfo->GetHPHealedIfDefeatEnemy();
	// nM: +10 Encirclement.
	if (iTemp != 0)
	{
		iExtra = getExtraAttackBelowHealthMod();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + 2 * iFlavorDefense);
		iExtra *= 1.5;
		iExtra *= 0.5 + 0.5 * getDamage() / max(1,GetMaxHitPoints());
		iValue += iExtra;

	}

	iTemp = pkPromotionInfo->GetAOEDamageOnKill();
	// M, mM: +10 Shock 4.		nM: +15 Breacher
	if (iTemp != 0)
	{
		iExtra = getExtraAttackBelowHealthMod();
		iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorMobile);
		iValue += iExtra;

	}

	iTemp = pkPromotionInfo->GetExtraWithdrawal();
	// Scout: +60 Trailblazer (woodland trailblazer) 3. nM: +50 Piracy.
	// Sub: +40 Wolfpack 3.
	if (iTemp != 0)
	{
		iExtra = - 2 * getDefenseModifier();
		iExtra = (iTemp + iExtra) * (2 * iFlavorMobile + iFlavorDefense);
		iExtra *= 0.2;
		if (IsCanAttackRanged())
			iExtra *= 2;
		iValue += iExtra;
	}
	


	iTemp = pkPromotionInfo->GetPlagueChance();
	// nM: +100 Boarding Party 1, Boarding Party 3.
	if (iTemp != 0)
	{
		iExtra = iTemp  * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		iExtra *= 0.08;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetPlagueIDImmunity();
	// nM: +1 Boarding Party 2. 	nR: +1 Indomidable (targeting 4).
	if (iTemp != 0)
	{
		iExtra = (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 8;
		iValue += iExtra;
	}
	
	iTemp = pkPromotionInfo->GetAdjacentEnemySapMovement();
	// nM: +120 Minelayer.                                       
	if (iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		iExtra *= 0.1;
		if (IsCanAttackRanged())
		{
			iExtra /= max(1, GetRange());
		}
		iValue += iExtra;
	}



#endif

	


			// City modifiers


	

	iTemp = pkPromotionInfo->GetCityAttackPercent();
	// M + mM: +25 Drill 1-3, +33 siege.	S: +15 siege 1-3, +50 Volley.	nM: +75 Naval Siege, + 100(125) Vanguard (coastal terror).
	// nR: +30 Bombardment 1-3, +40 Broadside (bombardment 4).	aB: +33 Air Siege 1-3.			
	if(iTemp != 0)
	{	
		iExtra = (iTemp)  * ( 3 * iFlavorOffense);
		iExtra *= 100 + getExtraCityAttackPercent();
		iExtra /= 100;
		iExtra *= 100 + GetDamageReductionCityAssault();
		iExtra /= 100;
		iExtra *= 100 + GetCityAttackPlunderModifier();
		iExtra /= 100;
		iExtra *= max(1, getNumAttacks());
		iExtra *= 0.2;
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetCityAttackPlunderModifier();
	// nM: +100 Blockade (coastal raider 4).
	if (iTemp != 0)
	{
		iExtra = (iTemp) * ( 3 * iFlavorOffense);
		iExtra *= 100 + getExtraCityAttackPercent();
		iExtra /= 100;
		iExtra *= max(1, getNumAttacks());
		iExtra *= 0.08;
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;

	}

	iTemp = pkPromotionInfo->GetDamageReductionCityAssault();
	// M: +33 Siege.	nM: +50 Vanguard (coastal terror). 
	if (iTemp != 0)
	{

		iExtra = (iTemp) * ( 3 * iFlavorOffense);
		iExtra *= 100 + getExtraCityAttackPercent();
		iExtra /= 100;
		iExtra *= 100 + GetDamageReductionCityAssault();
		iExtra /= 100;
		iExtra *= 0.25;
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;
	}
	

			// Ranged Attack Helpers
	
	


	iTemp = pkPromotionInfo->GetExtraAttacks();
	// M + mM + nM: +1 Blitz.	R + mR + nR + S: +1 Logistics.
	// aB: Air logistics.		nM + Sub: +1 	// Second Attack ??? appears in XML but not in game
	if(iTemp != 0)
	{
		if (IsCanAttackRanged())
			{
			iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
			iExtra *= 20;
			}
		else
			{
			iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
			iExtra *= 10;
			}
		iExtra *= baseMoves(false);
		iValue += iExtra;
	}




	iTemp = pkPromotionInfo->GetRangeChange();
	// R + S: +1 Range. 	aF : +1 Ace Pilot (interception) 3, +1 Sortie.
	// aB + aF: +2 Range (air range).
	if(iTemp != 0 && IsCanAttackRanged())
	{
		iExtra = iTemp * ( 3 * iFlavorRanged );
		iExtra *= 100;
		iExtra /= max(1,GetRange());
		iValue += iExtra;
	
	}

	if(pkPromotionInfo->IsRangeAttackIgnoreLOS() && IsCanAttackRanged() && !IsRangeAttackIgnoreLOS())
	// R + S: Indirect Fire.
	{
		iExtra = (iFlavorRanged * 2 + iFlavorOffense);
		iExtra *= 20;
		iExtra *= GetRange();
		iValue += iExtra;
	}





	iTemp = pkPromotionInfo->GetSplashDamage();
	// nR: +5 Splash 1.	S: +5 Splash 1 - 2.
	if (iTemp != 0)
	{
		iExtra = getSplashDamage();
		if (IsCanAttackRanged())
		{
			iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
			iExtra *= 1.5;
			iExtra *= GetRange();
		}
		else
		{
			iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
			iExtra *= 1.5;
		}
		iValue += iExtra;
			
	}

		// Melee Attack Helpers

	iTemp = pkPromotionInfo->IsIgnoreZOC();
	// Scout: Trailblazer (woodland trailblazer) 3.		nM: Pincer (boarding party 4).
	// mR: Skirmisher Doctrine (skirmisher mobility).
	if (iTemp != 0 && !IsIgnoreZOC())
	{
		iExtra =  iTemp * (2 * iFlavorMobile + iFlavorOffense);
		iExtra *= 10;
		iExtra *= max(1,getNumAttacks());
		iValue += iExtra;

	}

	if(pkPromotionInfo->IsBlitz() && !isBlitz())
	// M + mM + nM: Blitz.
	{
		// This should be covered in extra attacks
		iValue += 0;
	}





	if(pkPromotionInfo->IsCanMoveAfterAttacking() && !canMoveAfterAttacking())
	// M + mM + nM: Blitz.	
	{
		iExtra = (2 * iFlavorMobile + iFlavorOffense);
		iExtra *= 5;
		iExtra *= baseMoves(false);
		iValue += iExtra;

	}

			// Healing
	
	


	iTemp = pkPromotionInfo->GetSameTileHealChange();
	// M + mM + nM + Scout + R + mR: +5 Medic 1 - 2.	nM: +10 Dauntless (damage reduction).
	if (iTemp != 0)
	{
		iExtra = getSameTileHeal();
		iExtra = (iTemp + iExtra) * (iFlavorNaval + iFlavorOffense + iFlavorDefense);
		if (isAlwaysHeal())
			iExtra *= 6;
		if (bWarTimePromotion)
			iExtra *= 1.5;
		iExtra *= 0.7 + 0.3* getDamage() / max(1,GetMaxHitPoints());
		iValue += iExtra;
	}
	
	iTemp = pkPromotionInfo->GetAdjacentTileHealChange();
	// M + mM + nM + Scout + R + mR: +5 Medic 1 - 2.
	if (iTemp != 0)
	{
		iExtra = 3 * getAdjacentTileHeal();
		iExtra = (iTemp + iExtra) * (iFlavorNaval + iFlavorOffense + iFlavorDefense);
		iExtra *= 1.8;
		if (bWarTimePromotion)
			iExtra *= 1.5;
		iValue += iExtra;
		
	}

	iTemp = pkPromotionInfo->GetEnemyHealChange();
	// M + mM + nM + Scout + R + mR: +5 Medic 2.		nR + nM: +5 Supply. 	Scout: +5 Survivalism 1 - 2.
	if (iTemp != 0)
	{
		iExtra = getExtraEnemyHeal() + getSameTileHeal();
		iExtra = (iTemp + iExtra) * (iFlavorNaval + 2 * iFlavorOffense);
		iExtra *= 0.5;
		iExtra *= 0.7 + 0.3 * getDamage() / max(1,GetMaxHitPoints());
		if (isAlwaysHeal())
			iExtra *= 6;
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;

	}

	iTemp = pkPromotionInfo->GetNeutralHealChange();
	// M + mM + nM + Scout + R + mR: +5 Medic 2.		nR + nM: +5 Supply.	Scout: +5 Survivalism 1 - 2.
	if (iTemp != 0)
	{
		iExtra = getExtraNeutralHeal() + getSameTileHeal();
		iExtra = (iTemp + iExtra) * (iFlavorNaval + iFlavorOffense + iFlavorDefense);
		iExtra *= 0.5;
		iExtra *= 0.7 + 0.3 * getDamage() / max(1,GetMaxHitPoints());
		if (isAlwaysHeal())
			iExtra *= 6;
		iValue += iExtra;
	}




	if(pkPromotionInfo->IsAlwaysHeal() && !isAlwaysHeal())
	// aF: Air repair.	Scout: Survivalism 3.	mR: March (skirmisher march).
	// M + mM: March.			
	{
		iExtra = 10 + getSameTileHeal();
		iExtra += (getExtraFriendlyHeal() + getExtraNeutralHeal() + getExtraEnemyHeal()) / 3;
		iExtra *= iFlavorOffense + 2 * iFlavorMobile;
		iExtra *= 4;
		iExtra *= 0.7 + 0.3 * getDamage() / max(1,GetMaxHitPoints());
		iValue += iExtra;
	}

	if (pkPromotionInfo->IsHealOutsideFriendly() && getDomainType() == DOMAIN_SEA && !isHealOutsideFriendly())
	// nM + nR: Supply.	nM: Naval Siege.
	{
		iExtra = 10 + getSameTileHeal();
		iExtra += (getExtraFriendlyHeal() + getExtraNeutralHeal() + getExtraEnemyHeal()) / 3;
		iExtra *= iFlavorOffense + 2 * iFlavorNaval;
		iExtra *= 0.5;
		iExtra *= 0.7 + 0.3 * getDamage() / max(1,GetMaxHitPoints());
		if (bWarTimePromotion)
			iExtra *= 2;
		if (isAlwaysHeal())
			iExtra *= 5;
		iValue += iExtra;
	}

	
	iTemp = pkPromotionInfo->IsFreePillageMoves();
	// nM: +1 Press Gangs.
	if (iTemp != 0 && !hasFreePillageMove())
	{	
		iExtra = iTemp * (iFlavorOffense + 2 * iFlavorMobile);
		iExtra *= 0.7 + 0.3 * getDamage() / max(1,GetMaxHitPoints());
		iExtra *= 8;
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->IsHealOnPillage();
	// nM: +1 Press Gangs.
	if (iTemp != 0 && !hasHealOnPillage())
	{
		iExtra = iTemp * (2 * iFlavorOffense + iFlavorMobile);
		iExtra *= 0.2 + 0.8 * getDamage() / max(1,GetMaxHitPoints());
		iExtra *= 25;		
		if (bWarTimePromotion)
			iExtra *= 2;
		iValue += iExtra;
	}	
	

			// Scouting



	iTemp = pkPromotionInfo->GetMovesChange();
	// M + mM + nR + Sub + C: +1 Mobility.	Scout: +1 Scouting 3.	H: +1 Mobility (Heli Mobility) 1 - 2.
	// Sub: +1 Wolfpack 2.	Zulu: +1 Buffalo Horns.		mR: +1 Skirmisher Doctrine (skirmisher mobility).
	// nM: +1 Navigator (naval sentry) 1 - 2.	
	if(iTemp > 0)
	{
		iExtra = iTemp * (iFlavorMobile * 2 + iFlavorNavalRecon);
		iExtra *= 45;
		iExtra *= 4 / (max (1, 6 + baseMoves(false)));
		iExtra *= max(1,getNumAttacks());
		if (IsGainsXPFromScouting())
			iExtra *= 3;
		iValue += iExtra;

	}



	if(pkPromotionInfo->IsAmphib() && !isAmphibious())
	// M: Amphibious.
	{
		iExtra = (iFlavorNaval * 2 + iFlavorOffense);
		iExtra *= 6;
		iValue += iExtra;
	}

	if(pkPromotionInfo->IsRiver() && !(isRiverCrossingNoPenalty() || ignoreTerrainCost()))
	// M: Amphibious.
	{
		iExtra = (iFlavorMobile * 2 + iFlavorOffense);
		iExtra *= 15;
		iValue += iExtra;	
	}




	iTemp = pkPromotionInfo->GetVisibilityChange();
	// Scout: +1 Scouting 1,2. 	mM: +1 sentry.	mR +nR: +1 Sentry.	
	// nM: +1 Navigation 1,2 (naval sentry). 	Sub: +1 Wolfpack 1.
	if(iTemp != 0)
	{
		iExtra = iTemp * (2 * iFlavorRecon + iFlavorNavalRecon);
		iExtra *= 5;
		if (IsGainsXPFromScouting())
			iExtra *= 5;
		iValue += iExtra;
	}

			// Carrier and Air Units



	iTemp = pkPromotionInfo->GetCargoChange();
	// C: +1 Flight Deck 1 - 3.
	if (iTemp != 0)
	{
		iExtra = iTemp * (2 * iFlavorMobile + iFlavorOffense);
		iExtra *= 20;
		iValue += iExtra;
	}


	iTemp = pkPromotionInfo->GetLandAirDefenseValue();	// Is this a flat value or combat modifier?
	// M + nM + R + mR + nR + AA: +15 (antiair land) 1, +20 2 , +25 3.
	if (iTemp != 0)
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_AIR");
		if(GET_PLAYER(getOwner()).GetMilitaryAI()->IsUsingStrategy(eStrategy))
		{
			iTemp *= 2;		// Not mine but I assume it's useful
		}

		// iTemp += getLandAirDefenseValue() + getUnitInfo().GetBaseLandAirDefense();
		
		iExtra = iTemp * (3 * iFlavorAntiAir);
		iExtra *= 0.4;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetInterceptionCombatModifier();
	// aF: +33 Ace Pilot (Interception) 2 - 3, +34 Ace Pilot 4.
	if ((iTemp != 0 && canAirPatrol(NULL)) || getInterceptChance() > 0)		// not sure about this
	{

		iExtra = (iTemp) * (iFlavorDefense + 2 * iFlavorAntiAir);
		iExtra *= getInterceptChance() + 100;
		iExtra /= 100;
		iExtra *= GetInterceptionCombatModifier() + 100;
		iExtra /= 100;
		iExtra *= max(1,GetNumInterceptions());
		iExtra *= 0.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetInterceptChanceChange();
	// AA + aF + nM + C: +25 Interceptor (interception) I - IV, +25 Ace Pilot (interception) 1 - 3.
	if ((iTemp != 0 && GetAirInterceptRange() > 0) || getInterceptChance() > 0)
	{
		iExtra = iTemp * (2 * iFlavorAntiAir + iFlavorDefense);
		iExtra *= getInterceptChance() + 100;
		iExtra /= 100;
		iExtra *= GetInterceptionCombatModifier() + 100;
		iExtra /= 100;
		iExtra *= max(1, GetNumInterceptions());
		iExtra *= 0.2;
		iExtra *= GetAirInterceptRange();
		iValue += iExtra;
	}
		
		// This seems to be unused as normal range is used
	
	iTemp = pkPromotionInfo->GetAirInterceptRangeChange();
	if (iTemp != 0)
	{
		iExtra = GetExtraAirInterceptRange() * 5;
		iValue += iTemp + iExtra + iFlavorAir;
	}

	iTemp = pkPromotionInfo->GetAirSweepCombatModifier();
	// aF: +33 Dogfighting 1 - 3.
	if(iTemp != 0 && canAirSweep())
	{
		iExtra = GetAirSweepCombatModifier();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + 2 * iFlavorAir);
		iExtra *= 0.2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetEvasionChange();
	// aB: +33 Air Penetration (Evasion) I - II.
	if (iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense + 2 * iFlavorAir);
		iExtra *= 0.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetNumInterceptionChange();
	// aF: +1 Ace Pilot 4,
	if(iTemp != 0)
	{
		iExtra = getInterceptChance() + GetInterceptionCombatModifier();
		iExtra = iExtra * (2 * iFlavorAntiAir + iFlavorDefense);
		iExtra *= GetAirInterceptRange();
		iExtra *= 0.2;
		iValue += iExtra;
		
		 
	}

	iTemp = pkPromotionInfo->GetInterceptionDefenseDamageModifier();
	// aB: -50 Evasion.
	if(iTemp != 0 && getDomainType() == DOMAIN_AIR)
	{
		iExtra = iTemp * (iFlavorOffense + 2 * iFlavorAir);
		iExtra *= 0.5;
		iValue += iExtra;
	}

			// Final Complex modifiers 

	for(iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		const TerrainTypes eTerrain = static_cast<TerrainTypes>(iI);
		CvTerrainInfo* pkTerrainInfo = GC.getTerrainInfo(eTerrain);
		if(pkTerrainInfo)
		{
			iTemp = pkPromotionInfo->GetTerrainAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraTerrainAttackPercent(eTerrain);
				iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
				iExtra *= 0.2;
				iValue += iExtra;
			}

			iTemp = pkPromotionInfo->GetTerrainDefensePercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraTerrainDefensePercent(eTerrain);
				iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
				iExtra *= 0.2;
				iValue += iExtra;
				
			}

			iTemp = pkPromotionInfo->GetTerrainDoubleHeal(iI);
			if (iTemp != 0)
			{
				iExtra = 10 + getSameTileHeal();
				iExtra += (getExtraFriendlyHeal() + getExtraNeutralHeal() + getExtraEnemyHeal()) / 3;
				iExtra *= iFlavorOffense + 2 * iFlavorDefense;
				iExtra *= 0.5;
				iExtra *= 0.5 + 0.5 * getDamage() / max(1,GetMaxHitPoints());
				if (isAlwaysHeal())
					iExtra *= 5;
				iValue += iExtra;
			}

			if (pkTerrainInfo->getMovementCost() > 1 && !isIgnoreTerrainCostIn(eTerrain) && !ignoreTerrainCost())
			{
				if (pkPromotionInfo->GetIgnoreTerrainCostIn(eTerrain))
					// Scout: Hill Woodland Trailblazer 1, Snow/Desert Woodland Trailblazer 2.
				{
					iExtra = (iFlavorMobile * 2 + iFlavorRecon);
					iExtra *= 8;
					if (IsGainsXPFromScouting())
						iExtra *= 3;
					if (isIgnoreTerrainCostFrom(eTerrain))
						iExtra *= 0.5;
					iValue += iExtra;
				}

				if (pkPromotionInfo->GetIgnoreTerrainCostFrom(eTerrain))
				{
					iExtra = (iFlavorMobile * 2 + iFlavorRecon);
					iExtra *= 4;
					if (IsGainsXPFromScouting())
						iExtra *= 3;
					iValue += iExtra;
				}
			}

			if(pkPromotionInfo->GetTerrainDoubleMove(iI) && !isTerrainDoubleMove(eTerrain))
			{
				iExtra = (iFlavorMobile * 2 + iFlavorRecon);
				iExtra *= 8;
				if (IsGainsXPFromScouting())
					iExtra *= 3;
				if (ignoreTerrainCost())
					iExtra *= 0.5;
				iValue += iExtra;
			}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			if(pkPromotionInfo->GetTerrainHalfMove(iI) && !isTerrainHalfMove(eTerrain))
			{
				iExtra = (iFlavorMobile * 3);
				iExtra *= -8;
				if (IsGainsXPFromScouting())
					iExtra *= 2;
				iValue += iExtra;
			}

			if (pkPromotionInfo->GetTerrainExtraMove(iI) && !isTerrainExtraMove(eTerrain))
			{
				iExtra = (iFlavorMobile * 3);
				iExtra *= -5;
				if (IsGainsXPFromScouting())
					iExtra *= 1.8;
				iValue += iExtra;
			}
#endif
		}
	}

	for(iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		const FeatureTypes eFeature = static_cast<FeatureTypes>(iI);
		CvFeatureInfo* pkFeatureInfo = GC.getFeatureInfo(eFeature);
		if(pkFeatureInfo)
		{
			iTemp = pkPromotionInfo->GetFeatureAttackPercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureAttackPercent(eFeature);
				iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
				iExtra *= 0.2;
				iValue += iExtra;
				
			}

			iTemp = pkPromotionInfo->GetFeatureDefensePercent(iI);
			if(iTemp != 0)
			{
				iExtra = getExtraFeatureDefensePercent(eFeature);
				iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
				iExtra *= 0.2;
				iValue += iExtra;
				
			}

			iTemp = pkPromotionInfo->GetFeatureDoubleHeal(iI);
			if (iTemp != 0)
			{
				iExtra = 10 + getSameTileHeal();
				iExtra += (getExtraFriendlyHeal() + getExtraNeutralHeal() + getExtraEnemyHeal()) / 3;
				iExtra *= iFlavorOffense + 2 * iFlavorDefense;
				iExtra *= 0.5;
				iExtra *= 0.5 + 0.5 * getDamage() / max(1,GetMaxHitPoints());
				if (isAlwaysHeal())
					iExtra *= 5;
				iValue += iExtra;
				
			}

			if (pkFeatureInfo->getMovementCost() > 1 && !isIgnoreFeatureCostIn(eFeature) && !ignoreTerrainCost())
			{
				if (pkPromotionInfo->GetIgnoreFeatureCostIn(eFeature))
					// Scout: Forest/Jungle Woodland Trailblazer 1, Marsh Woodland Trailblazer 2.
				{
					iExtra = (iFlavorMobile * 2 + iFlavorRecon);
					iExtra *= 8;
					if (IsGainsXPFromScouting())
						iExtra *= 3;
					if (isIgnoreFeatureCostFrom(eFeature))
						iExtra *= 0.5;
					iValue += iExtra;
				}

				if (pkPromotionInfo->GetIgnoreFeatureCostFrom(eFeature))
				{
					iExtra = (iFlavorMobile * 2 + iFlavorRecon);
					iExtra *= 4;
					if (IsGainsXPFromScouting())
						iExtra *= 3;
					iValue += iExtra;
				}
			}

			if(pkPromotionInfo->GetFeatureDoubleMove(iI) && !isFeatureDoubleMove(eFeature))
			// M: Forest/Jungle Woodsman.
			{
				iExtra = (2 * iFlavorMobile + iFlavorRecon);
				iExtra *= 8;
				if (IsGainsXPFromScouting())
					iExtra *= 3;
				if (ignoreTerrainCost())
					iExtra *= 0.5;
				iValue += iExtra;
			}

#if defined(MOD_PROMOTIONS_HALF_MOVE)
			if(pkPromotionInfo->GetFeatureHalfMove(iI) && !isFeatureHalfMove(eFeature))
			{
				iExtra = (2 * iFlavorMobile + iFlavorRecon);
				iExtra *= -5;
				iExtra *= max(1,getNumAttacks());
				if (IsGainsXPFromScouting())
					iExtra *= 2;
				iValue += iExtra;
			}

			if (pkPromotionInfo->GetFeatureExtraMove(iI) && !isFeatureExtraMove(eFeature))
			{
				iExtra = (2 * iFlavorMobile + iFlavorRecon);
				iExtra *= -5;
				iExtra *= max(1,getNumAttacks());
				if (IsGainsXPFromScouting())
					iExtra *= 1.8;
				iValue += iExtra;
			}
#endif
		}
	}
	

			// This part is hard to balance		

	for(iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
		if(pkUnitCombatInfo)
		{
			iTemp = pkPromotionInfo->GetUnitCombatModifierPercent(iI);
				// M: +33 vs Mounted, formation 1,2.	AA, aF: vs aF and aB: +100 Air supremacy (anti air) 2.
									// Could this be changed to a DOMAIN_AIR combat modifier?
			
			iTemp += 2 * pkPromotionInfo->GetCombatModPerAdjacentUnitCombatModifierPercent(iI);
			iTemp +=  pkPromotionInfo->GetCombatModPerAdjacentUnitCombatAttackModifier(iI);
				// nM: + 10 vs sub, nM, nR, C, encirclement.
			iTemp +=  pkPromotionInfo->GetCombatModPerAdjacentUnitCombatDefenseModifier(iI);
				// nM: + 10 vs sub, nM, nR, C, Breacher.

			// Would probably make more sense if the adjacent modifiers were base on domain instead of combat classes as well

			if (iTemp <= 0)
				continue;

			iExtra = iTemp * ( 2 * iFlavorOffense + iFlavorDefense);
			iExtra *= 0.3;
			if (IsCanAttackRanged())
				iExtra *= 0.6;
			if (getDomainType() == DOMAIN_SEA)	// required for balance
				iExtra *= 0.8;
			if (GetAirInterceptRange() > 0)		// Value for air supremacy will be high but that's probably correct
				iExtra *= 1.5;
			iValue += iExtra;

		}
	}

	for(iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		iTemp = pkPromotionInfo->GetDomainModifierPercent(iI) * 2;
		iTemp += pkPromotionInfo->GetDomainAttackPercent(iI);
		iTemp += pkPromotionInfo->GetDomainDefensePercent(iI);
		// nR: Land + Sea: +10 targeting 1 - 3.		aB: Land + Sea: +15 air targeting 1 - 2, +25 air targeting 3.
		if (iTemp <= 0)
			continue;

		if (DomainTypes(iI) == DOMAIN_SEA)
		{
			iExtra = iTemp * (iFlavorDefense + 2 * iFlavorNaval);
			iExtra *= 0.5;
			iValue += iExtra;
		}
		else if (DomainTypes(iI) == DOMAIN_LAND)
		{
			iExtra = iTemp * (iFlavorDefense + iFlavorOffense + iFlavorCityDefense);
			iExtra *= 0.5;
			iValue += iExtra;
		}
		else if (DomainTypes(iI) == DOMAIN_AIR)
		{
			iExtra = iTemp * (3 * iFlavorAntiAir);
			iExtra *= 0.5;
			iValue += iExtra;
		}

	}

			// Unused in VP
	

	iTemp = pkPromotionInfo->GetOpenFromPercent();
	if (iTemp != 0)
	{
		iExtra = getExtraOpenFromPercent();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + iFlavorDefense + iFlavorMobile);
		iExtra *= 0.3;
		if(getUnitInfo().IsMounted())
		{
			iExtra *= 2;
		}
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetRoughFromPercent();
	if (iTemp != 0)
	{
		iExtra = getExtraRoughFromPercent();
		iExtra = (iTemp + iExtra) * (iFlavorOffense + 2 * iFlavorDefense);
		iExtra *= 0.3;
		if(getExtraOpenFromPercent())
		{
			iExtra *= 0.5;
		}
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetReconChange();		// This is air fighter units recon?
	if (iTemp != 0)
	{
		iExtra = iTemp * (3 * iFlavorRecon);
		iExtra *= 5;
		iValue += iExtra;
	}



	iTemp = pkPromotionInfo->GetGoldenAgeValueFromKills();
	if (iTemp != 0)
	{
		iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 0.2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetAttackFullyHealedMod();
	if (iTemp != 0)
	{
		iExtra = getExtraAttackFullyHealedMod();
		iExtra = ( iTemp + iExtra ) * ( iFlavorDefense + 2 * iFlavorCityDefense);
		if (IsCanAttackRanged())
			iExtra *= 0.3;
		else
			iExtra *= 0.15;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetMaxHitPointsModifier();
	if (iTemp != 0)
	{
		if (IsCanAttackRanged())
		{
			iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
			iExtra *= 0.5;
		}
		else
		{
			iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
			iExtra *= 1;
		}
		iValue += iExtra;
	}


	if (pkPromotionInfo->IsNoSupply() && !isNoSupply())
	{
		iExtra = iFlavorNaval + 2 * iFlavorOffense;
		iExtra *= 5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetFriendlyLandsModifier();
	if (iTemp != 0)
	{
		iExtra = getFriendlyLandsModifier();
		iExtra = ( iTemp + iExtra ) * ( iFlavorDefense + 2 * iFlavorCityDefense );
		iExtra *= 0.3;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetCapitalDefenseModifier();
	if (iTemp != 0)
	{
	
		iExtra = GetCapitalDefenseModifier();
		iExtra = ( iTemp + iExtra ) * (3 * iFlavorCityDefense );
		iExtra *= 0.2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetFriendlyLandsAttackModifier();
	if (iTemp != 0)
	{
		iExtra = getFriendlyLandsAttackModifier();
		iExtra = ( iTemp + iExtra ) * ( iFlavorDefense + 2 * iFlavorCityDefense );
		iExtra *= 0.15;
		iValue += iExtra;
	}

	if (pkPromotionInfo->IsGainsXPFromSpotting() && !IsGainsXPFromSpotting())
	{
		iExtra = visibilityRange() + baseMoves(false);
		iExtra = iExtra * (3 * iFlavorRecon);
		iExtra *= 5;
		iValue += iExtra;
	}



	if (pkPromotionInfo->IsGainsXPFromPillaging() && !IsGainsXPFromPillaging())
	{
		iExtra = (iFlavorOffense + 2 * iFlavorMobile);
		iExtra *= 5;
		iExtra *= baseMoves(false);
		if (hasFreePillageMove())
			iExtra *= 2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetGoodyHutYieldBonus();
	if (iTemp != 0 && !GC.getGame().isOption(GAMEOPTION_NO_GOODY_HUTS))
	{
		iExtra = iTemp * (3 * iFlavorRecon);
		iExtra *= baseMoves(false);
		iExtra *= 0.5;
		iValue += iTemp + iFlavorRecon;
	}

	iTemp = pkPromotionInfo->GetCaptureDefeatedEnemyChance();
	if (iTemp != 0)
	{
		iExtra = iTemp * (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 0.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetFriendlyHealChange();
	if(iTemp != 0)
	{
		iExtra = getExtraFriendlyHeal() + getSameTileHeal();
		iExtra = (iTemp + iExtra) * (iFlavorNaval + 2 * iFlavorCityDefense);
		iExtra *= 0.5 + 0.5 * getDamage() / max(1,GetMaxHitPoints());
		if (isAlwaysHeal())
			iExtra *= 4;
		if (bWarTimePromotion)
			iExtra *= 0.5;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetCityDefensePercent();
	if(iTemp != 0)
	{
		
		iExtra = getExtraCityDefensePercent();
		iExtra = (iTemp + iExtra) * (3 * iFlavorCityDefense);
		iExtra *= 0.2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetHillsAttackPercent();
	if (iTemp != 0)
	{
		iExtra = getExtraHillsAttackPercent();
		iExtra = (iTemp + iExtra) * (2 * iFlavorOffense + iFlavorDefense);
		iExtra *= 0.2;
		iValue += iExtra;
		
	}
	iTemp = pkPromotionInfo->GetHillsDefensePercent();
	if (iTemp != 0)
	{
		iExtra = getExtraHillsDefensePercent();
		iExtra = (iTemp + iExtra) * (2 * iFlavorDefense + iFlavorCityDefense);
		iExtra *= 0.2;
		iValue += iExtra;
	}

	iTemp = pkPromotionInfo->GetNearbyEnemyCombatMod();
	if (iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense + iFlavorDefense + iFlavorCityDefense);
		iExtra *= 2;
		if (IsCanAttackRanged())
			iExtra *= 0.5;
		iValue += iExtra;
	}
	iTemp = pkPromotionInfo->GetNearbyEnemyCombatRange();
	if (iTemp != 0)
	{
		iExtra = iTemp * (iFlavorOffense + 2 * iFlavorRanged);
		iExtra *= 20;
		if (IsCanAttackRanged())
			iExtra *= 0.5;
		iValue += iExtra;
	}

	return (int)iValue;
}


//	--------------------------------------------------------------------------------
GreatPeopleDirectiveTypes CvUnit::GetGreatPeopleDirective() const
{
	return m_eGreatPeopleDirectiveType;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetGreatPeopleDirective(GreatPeopleDirectiveTypes eDirective)
{
	m_eGreatPeopleDirectiveType = eDirective;
}


//	--------------------------------------------------------------------------------
bool CvUnit::IsHasBeenPromotedFromGoody() const
{
	return m_bPromotedFromGoody;
}

//	--------------------------------------------------------------------------------
void CvUnit::SetBeenPromotedFromGoody(bool bBeenPromoted)
{
	m_bPromotedFromGoody = bBeenPromoted;
}

#if defined(MOD_BALANCE_CORE)
//	--------------------------------------------------------------------------------
bool CvUnit::IsHigherPopThan(const CvUnit* pOtherUnit) const
{
	int iMyPop = 0;
	int iOtherPop = 0;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	iMyPop = kPlayer.getTotalPopulation();

	CvPlayer& kOtherPlayer = GET_PLAYER(pOtherUnit->getOwner());
	iOtherPop = kOtherPlayer.getTotalPopulation();

	return iMyPop > iOtherPop;
}
#endif

//	--------------------------------------------------------------------------------
bool CvUnit::IsHigherTechThan(UnitTypes otherUnit) const
{
	int iMyTechCost = 0;
	int iOtherTechCost = 0;

	TechTypes eMyTech = (TechTypes)getUnitInfo().GetPrereqAndTech();
	if(eMyTech != NO_TECH)
	{
		CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eMyTech);
		if(pEntry)
		{
			iMyTechCost = pEntry->GetResearchCost();
		}
	}

	CvUnitEntry* pUnitEntry = GC.GetGameUnits()->GetEntry(otherUnit);
	if(pUnitEntry)
	{
		TechTypes eOtherTech = (TechTypes)pUnitEntry->GetPrereqAndTech();
		if(eOtherTech != NO_TECH)
		{
			CvTechEntry* pEntry = GC.GetGameTechs()->GetEntry(eOtherTech);
			if(pEntry)
			{
				iOtherTechCost = pEntry->GetResearchCost();
			}
		}
	}

	return iMyTechCost > iOtherTechCost;
}

//	--------------------------------------------------------------------------------
bool CvUnit::IsLargerCivThan(const CvUnit* pOtherUnit) const
{
	int iMyCities = 0;
	int iOtherCities = 0;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	iMyCities = kPlayer.getNumCities();

	CvPlayer& kOtherPlayer = GET_PLAYER(pOtherUnit->getOwner());
	iOtherCities = kOtherPlayer.getNumCities();

	return iMyCities > iOtherCities;
}

//	--------------------------------------------------------------------------------
void CvUnit::dispatchingNetMessage(bool dispatchingMessage)
{
	s_dispatchingNetMessage = dispatchingMessage;
}

//	--------------------------------------------------------------------------------
bool CvUnit::dispatchingNetMessage()
{
	return s_dispatchingNetMessage;
}

//	---------------------------------------------------------------------------------
//	Return true if the unit should change/update the map visibility.
bool CvUnit::canChangeVisibility() const
{
	return m_iMapLayer == DEFAULT_UNIT_MAP_LAYER || m_iMapLayer == TRADE_UNIT_MAP_LAYER;
}

//	--------------------------------------------------------------------------------
std::string CvUnit::debugDump(const FAutoVariableBase& /*var*/) const
{
	// TODO: This... just can't be correct.  Surely, there must be something useful
	// this function is supposed to do.  Unfortunately, I don't know what that is.
	return EmptyString;
}

//	--------------------------------------------------------------------------------
std::string CvUnit::stackTraceRemark(const FAutoVariableBase& /*var*/) const
{
	return EmptyString;
}

bool CvUnit::IsCivilization(CivilizationTypes iCivilizationType) const
{
	return (GET_PLAYER(getOwner()).getCivilizationType() == iCivilizationType);
}

bool CvUnit::HasPromotion(PromotionTypes iPromotionType) const
{
	return isHasPromotion(iPromotionType);
}

bool CvUnit::IsUnit(UnitTypes iUnitType) const
{
	return (getUnitType() == iUnitType);
}

bool CvUnit::IsUnitClass(UnitClassTypes iUnitClassType) const
{
	return (getUnitClassType() == iUnitClassType);
}

bool CvUnit::IsOnFeature(FeatureTypes iFeatureType) const
{
	if (plot() != NULL)
	{
		return plot()->HasFeature(iFeatureType);
	}
	return false;
}

bool CvUnit::IsAdjacentToFeature(FeatureTypes iFeatureType) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToFeature(iFeatureType);
	}
	return false;
}

bool CvUnit::IsWithinDistanceOfFeature(FeatureTypes iFeatureType, int iDistance) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfFeature(iFeatureType, iDistance);
	}
	return false;
}
#if defined(MOD_BALANCE_CORE)
bool CvUnit::IsWithinDistanceOfUnit(UnitTypes eOtherUnit, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfUnit(getOwner(), eOtherUnit, iDistance, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsWithinDistanceOfCity(int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfCity(this, iDistance, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsWithinDistanceOfUnitClass(UnitClassTypes eUnitClass, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfUnitClass(getOwner(), eUnitClass, iDistance, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsWithinDistanceOfUnitCombatType(UnitCombatTypes eUnitCombat, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfUnitCombatType(getOwner(), eUnitCombat, iDistance, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsWithinDistanceOfUnitPromotion(PromotionTypes eUnitPromotion, int iDistance, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfUnitPromotion(getOwner(), eUnitPromotion, iDistance, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsAdjacentToUnit(UnitTypes eOtherUnit, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToUnit(getOwner(), eOtherUnit, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsAdjacentToUnitClass(UnitClassTypes eUnitClass, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToUnitClass(getOwner(), eUnitClass, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsAdjacentToUnitCombatType(UnitCombatTypes eUnitCombat, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToUnitCombatType(getOwner(), eUnitCombat, bIsFriendly, bIsEnemy);
	}
	return false;
}
bool CvUnit::IsAdjacentToUnitPromotion(PromotionTypes eUnitPromotion, bool bIsFriendly, bool bIsEnemy) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToUnitPromotion(getOwner(), eUnitPromotion, bIsFriendly, bIsEnemy);
	}
	return false;
}
#endif
bool CvUnit::IsOnImprovement(ImprovementTypes iImprovementType) const
{
	if (plot() != NULL)
	{
		return plot()->HasImprovement(iImprovementType);
	}
	return false;
}

bool CvUnit::IsAdjacentToImprovement(ImprovementTypes iImprovementType) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToImprovement(iImprovementType);
	}
	return false;
}

bool CvUnit::IsWithinDistanceOfImprovement(ImprovementTypes iImprovementType, int iDistance) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfImprovement(iImprovementType, iDistance);
	}
	return false;
}

bool CvUnit::IsOnPlotType(PlotTypes iPlotType) const
{
	if (plot() != NULL)
	{
		return plot()->HasPlotType(iPlotType);
	}
	return false;
}

bool CvUnit::IsAdjacentToPlotType(PlotTypes iPlotType) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToPlotType(iPlotType);
	}
	return false;
}

bool CvUnit::IsWithinDistanceOfPlotType(PlotTypes iPlotType, int iDistance) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfPlotType(iPlotType, iDistance);
	}
	return false;
}

bool CvUnit::IsOnResource(ResourceTypes iResourceType) const
{
	if (plot() != NULL)
	{
		return plot()->HasResource(iResourceType);
	}
	return false;
}

bool CvUnit::IsAdjacentToResource(ResourceTypes iResourceType) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToResource(iResourceType);
	}
	return false;
}

bool CvUnit::IsWithinDistanceOfResource(ResourceTypes iResourceType, int iDistance) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfResource(iResourceType, iDistance);
	}
	return false;
}

bool CvUnit::IsOnTerrain(TerrainTypes iTerrainType) const
{
	if (plot() != NULL)
	{
		return plot()->HasTerrain(iTerrainType);
	}
	return false;
}

bool CvUnit::IsAdjacentToTerrain(TerrainTypes iTerrainType) const
{
	if (plot() != NULL)
	{
		return plot()->IsAdjacentToTerrain(iTerrainType);
	}
	return false;
}

bool CvUnit::IsWithinDistanceOfTerrain(TerrainTypes iTerrainType, int iDistance) const
{
	if (plot() != NULL)
	{
		return plot()->IsWithinDistanceOfTerrain(iTerrainType, iDistance);
	}
	return false;
}


//	--------------------------------------------------------------------------------
/// Can a unit reach this destination in "X" turns of movement? (pass in 0 if need to make it in 1 turn with movement left)
bool CvUnit::CanSafelyReachInXTurns(const CvPlot* pTarget, int iTurns)
{
	if (!pTarget)
		return false;

	//stop pathfinding if we need to end the turn on a dangerous plot
	return (TurnsToReachTarget(pTarget, CvUnit::MOVEFLAG_AI_ABORT_IN_DANGER, iTurns) <= iTurns);
}

//	--------------------------------------------------------------------------------
/// How many turns will it take a unit to get to a target plot (returns MAX_INT if can't reach at all; returns 0 if makes it in 1 turn and has movement left)
int CvUnit::TurnsToReachTarget(const CvPlot* pTarget, bool bIgnoreUnits, bool bIgnoreStacking, int iTargetTurns)
{
	int iFlags = CvUnit::MOVEFLAG_TURN_END_IS_NEXT_TURN;

	if(bIgnoreUnits)
	{
		iFlags |= CvUnit::MOVEFLAG_IGNORE_STACKING_SELF;
		iFlags |= CvUnit::MOVEFLAG_IGNORE_ZOC;
		iFlags |= CvUnit::MOVEFLAG_IGNORE_DANGER;
	}

	if(bIgnoreStacking) //ignore our own units
		iFlags |= CvUnit::MOVEFLAG_IGNORE_STACKING_SELF;

	return TurnsToReachTarget(pTarget,iFlags,iTargetTurns);
}

int CvUnit::TurnsToReachTarget(const CvPlot* pTarget, int iFlags, int iTargetTurns)
{
	if(!pTarget)
		return INT_MAX;

	//make sure that iTargetTurns is valid
	iTargetTurns = max(1,iTargetTurns);

	//performance optimization
	if (iTargetTurns!=INT_MAX)
	{
		//see how far we could move in optimal circumstances
		int	iDistance = plotDistance(getX(), getY(), pTarget->getX(), pTarget->getY());

		//sometimes we don't need to go all the way
		if (iFlags & CvUnit::MOVEFLAG_APPROX_TARGET_RING2)
			iDistance -= 2;
		else if (iFlags & CvUnit::MOVEFLAG_APPROX_TARGET_RING1)
			iDistance -= 1;

		//default range
		int iMoves = baseMoves(isEmbarked()) + getExtraMoves();
		int iRange = iMoves * iTargetTurns;

		//catch stupid cases
		if (iRange<=0)
			return pTarget==plot() ? INT_MAX : 0;

		//but routes increase it
		if (getDomainType()==DOMAIN_LAND && !flatMovementCost() )
		{
			CvTeam& kTeam = GET_TEAM(getTeam());
			RouteTypes eBestRouteType = kTeam.GetBestPossibleRoute();
			CvRouteInfo* pRouteInfo = GC.getRouteInfo(eBestRouteType);
			if (pRouteInfo)
			{
				int iMoveCost = pRouteInfo->getMovementCost() + kTeam.getRouteChange(eBestRouteType);

				if (iMoveCost>0)
				{
					//times 100 to reduce rounding errors
					int iMultiplier = 100 * GD_INT_GET(MOVE_DENOMINATOR) / iMoveCost;

					if (plot()->getRouteType()!=NO_ROUTE)
						//standing directly on a route
						iRange = (iMoves * iTargetTurns * iMultiplier) / 100;
					else
						//need to move at least one plot in the first turn at full cost to get to the route
						//speed optimization for railroad and low turn count
						iRange = (100 + (iMoves-1)*iMultiplier + iMoves*(iTargetTurns-1)*iMultiplier) / 100;
				}
			}
		}

		if (iDistance>iRange)
			return INT_MAX;
	}

	if(pTarget == plot())
		return 0;

	//don't cache the result here
	return ComputePath(pTarget, iFlags, iTargetTurns, false);
}

//	--------------------------------------------------------------------------------
FDataStream& operator<<(FDataStream& saveTo, const CvUnit& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

//	--------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& loadFrom, CvUnit& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}
//	--------------------------------------------------------------------------------
#if defined(MOD_BALANCE_CORE)
bool CvUnit::isCultureFromExperienceDisbandUpgrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsCultureFromExperienceDisbandUpgrade();
}
bool CvUnit::isFreeUpgrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsFreeUpgrade();
}
bool CvUnit::isUnitEraUpgrade() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsUnitEraUpgrade();
}
bool CvUnit::isReligiousUnit() const
{
	VALIDATE_OBJECT
	return (m_pUnitInfo->IsFoundReligion() || m_pUnitInfo->IsSpreadReligion() || m_pUnitInfo->IsRemoveHeresy());
}
bool CvUnit::isWLKTKDOnBirth() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsWLTKDFromBirth();
}
bool CvUnit::isGoldenAgeOnBirth() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsGoldenAgeFromBirth();
}
bool CvUnit::isCultureBoost() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsCultureBoost();
}
bool CvUnit::isExtraAttackHealthOnKill() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsExtraAttackHealthOnKill();
}
bool CvUnit::isHighSeaRaiderUnit() const
{
	VALIDATE_OBJECT
	return getUnitInfo().IsHighSeaRaider();
}
#endif
#if defined(MOD_GLOBAL_STACKING_RULES)
int CvUnit::getNumberStackingUnits() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetNumberStackingUnits();
}
bool CvUnit::IsStackingUnit() const
{
	VALIDATE_OBJECT
	return getUnitInfo().GetNumberStackingUnits()>0;
}
bool CvUnit::IsCargoCombatUnit() const
{
	VALIDATE_OBJECT
	return getUnitInfo().CargoCombat()>0;
}
//	--------------------------------------------------------------------------------
#endif

void CvUnit::DoCargoPromotions(CvUnit& cargounit)
{
	if(cargounit.IsCargoCombatUnit() && cargounit.hasCargo())
	{
		cargounit.SetBaseCombatStrength(cargounit.getUnitInfo().GetCombat() + ((cargounit.getUnitInfo().GetCombat() * cargounit.getUnitInfo().CargoCombat() * cargounit.getCargo()) / 100));
		PromotionTypes ePromotionPrizeShips = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_PRIZE_SHIPS", true);
		PromotionTypes ePromotionArmySupport = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_SUPPORT", true);
		if(cargounit.isHasPromotion(ePromotionPrizeShips))
		{
			cargounit.setHasPromotion(ePromotionArmySupport, true);
		}
		if(isCargo())
		{
			UnitCombatTypes eUnitCombatReconType = (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true);
			PromotionTypes ePromotionEmbarkAllWater = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ALLWATER_EMBARKATION", true);
			PromotionTypes ePromotionPolynesiaCargo = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_POLYNESIA_CARGO", true);
			PromotionTypes ePromotionScoutShip = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_SCOUT_SHIP", true);
			PromotionTypes ePromotionArmyOnShip = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_ON_SHIP", true);
			PromotionTypes ePromotionRangePenalty = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_RANGE_PENALTY", true);
			setHasPromotion(ePromotionArmyOnShip, true);
			if(IsCanAttackRanged())
			{
				setHasPromotion(ePromotionRangePenalty, true);
			}
			if(getUnitCombatType() == eUnitCombatReconType)
			{
				cargounit.setHasPromotion(ePromotionScoutShip, true);
			}
			else
			{
				cargounit.setHasPromotion(ePromotionScoutShip, false);
			}
			if(isHasPromotion(ePromotionEmbarkAllWater))
			{
				cargounit.setHasPromotion(ePromotionPolynesiaCargo, true);
			}
			else
			{
				cargounit.setHasPromotion(ePromotionPolynesiaCargo, false);
			}
		}
	}
}

void CvUnit::RemoveCargoPromotions(CvUnit& cargounit)
{
	bool scoutcheck = false;
	bool scoutPromcheck = false;
	bool allembark = false;
	bool allembarkProm = false;
	if(isCargo())
	{
		UnitCombatTypes eUnitCombatReconType = (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true);
		PromotionTypes ePromotionEmbarkAllWater = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ALLWATER_EMBARKATION", true);
		if(getUnitCombatType() == eUnitCombatReconType)
		{
			scoutcheck = true;
		}
		if(isHasPromotion(ePromotionEmbarkAllWater))
		{
			allembark = true;
		}
	}
	else if(!isCargo())
	{
		PromotionTypes ePromotionArmyOnShip = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_ON_SHIP", true);
		PromotionTypes ePromotionRangePenalty = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_RANGE_PENALTY", true);
		if(isHasPromotion(ePromotionArmyOnShip))
		{
			setHasPromotion(ePromotionArmyOnShip, false);
		}
		if(isHasPromotion(ePromotionRangePenalty))
		{
			setHasPromotion(ePromotionRangePenalty, false);
		}
	}
	PromotionTypes ePromotionArmySupport = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_SUPPORT", true);
	if(cargounit.getCargo() >= 0)
	{
		PromotionTypes ePromotionScoutShip = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_ARMY_SCOUT_SHIP", true);
		if(cargounit.isHasPromotion(ePromotionScoutShip))
		{
			scoutPromcheck = true;
		}
		if(!scoutcheck && scoutPromcheck)
		{
			cargounit.setHasPromotion(ePromotionScoutShip, false);
		}
		PromotionTypes ePromotionPolynesiaCargo = (PromotionTypes) GC.getInfoTypeForString("PROMOTION_POLYNESIA_CARGO", true);
		if(cargounit.isHasPromotion(ePromotionPolynesiaCargo))
		{
			allembarkProm = true;
		}
		if(!allembark && allembarkProm)
		{
			cargounit.setHasPromotion(ePromotionPolynesiaCargo, false);
		}
		cargounit.SetBaseCombatStrength(cargounit.getUnitInfo().GetCombat() + ((cargounit.getUnitInfo().GetCombat() * cargounit.getUnitInfo().CargoCombat() * cargounit.getCargo()) / 100));
	}
	if(!cargounit.hasCargo())
	{
		if(cargounit.isHasPromotion(ePromotionArmySupport))
		{
			cargounit.setHasPromotion(ePromotionArmySupport, false);
		}
		cargounit.SetBaseCombatStrength(cargounit.getUnitInfo().GetCombat());
	}
}

void CvUnit::DoGreatPersonSpawnBonus(CvCity* pSpawnCity)
{
	CvAssert(pSpawnCity);

	if (!IsGreatPerson())
		return;

	CvPlayer& kPlayer = GET_PLAYER(getOwner());

	// Start local WLTKD
	if (kPlayer.GetPlayerTraits()->IsGPWLTKD())
	{
		int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
		iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iWLTKD /= 100;

		if (iWLTKD > 0)
		{
			pSpawnCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA");
				strText << getNameKey() << pSpawnCity->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA");
				strSummary << pSpawnCity->getNameKey();
				pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pSpawnCity->getX(), pSpawnCity->getY(), -1);
			}
		}
	}

	// Start global WLTKD
	if (isWLKTKDOnBirth())
	{
		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UNIT");
					strText << getNameKey() << pLoopCity->getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UNIT");
					strSummary << pLoopCity->getNameKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pLoopCity->getX(), pLoopCity->getY(), -1);
				}
			}
		}
	}

	// Start Golden Age
	if (isGoldenAgeOnBirth())
	{
		kPlayer.changeGoldenAgeTurns(kPlayer.getGoldenAgeLength());
	}

	// Give Culture to Capital
	// TODO: Change it to an instant yield?
	if (isCultureBoost())
	{
		// Can't possibly have no capital when there's a spawn city
		CvAssert(kPlayer.getCapitalCity());

		int iValue = kPlayer.GetTotalJONSCulturePerTurn() * 4;
		kPlayer.changeJONSCulture(iValue);
		kPlayer.getCapitalCity()->ChangeJONSCultureStored(iValue);

		CvNotifications* pNotifications = kPlayer.GetNotifications();
		if (pNotifications)
		{
			Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_UNIT");
			strText << getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CULTURE_UNIT");
			strSummary << getNameKey();
			pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), getX(), getY(), -1);
		}
	}
}
