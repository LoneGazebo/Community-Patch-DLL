/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvUnitClasses.h"

// include this after all other headers!
#include "LintFree.h"

/// Constructor
CvPromotionEntry::CvPromotionEntry():
	m_iLayerAnimationPath(ANIMATIONPATH_NONE),
	m_iPrereqPromotion(NO_PROMOTION),
	m_iPrereqOrPromotion1(NO_PROMOTION),
	m_iPrereqOrPromotion2(NO_PROMOTION),
	m_iPrereqOrPromotion3(NO_PROMOTION),
	m_iPrereqOrPromotion4(NO_PROMOTION),
	m_iPrereqOrPromotion5(NO_PROMOTION),
	m_iPrereqOrPromotion6(NO_PROMOTION),
	m_iTechPrereq(NO_TECH),
	m_iInvisibleType(NO_INVISIBLE),
	m_iSeeInvisibleType(NO_INVISIBLE),
	m_iVisibilityChange(0),
	m_iMovesChange(0),
	m_iMoveDiscountChange(0),
	m_iRangeChange(0),
	m_iRangedAttackModifier(0),
	m_iInterceptionCombatModifier(0),
	m_iInterceptionDefenseDamageModifier(0),
	m_iAirSweepCombatModifier(0),
	m_iInterceptChanceChange(0),
	m_iNumInterceptionChange(0),
	m_iEvasionChange(0),
	m_iCargoChange(0),
	m_iEnemyHealChange(0),
	m_iNeutralHealChange(0),
	m_iFriendlyHealChange(0),
	m_iSameTileHealChange(0),
	m_iAdjacentTileHealChange(0),
	m_iEnemyDamageChance(0),
	m_iNeutralDamageChance(0),
	m_iEnemyDamage(0),
	m_iNeutralDamage(0),
	m_iCombatPercent(0),
	m_iCityAttackPercent(0),
	m_iCityDefensePercent(0),
	m_iRangedDefenseMod(0),
	m_iHillsAttackPercent(0),
	m_iHillsDefensePercent(0),
	m_iOpenAttackPercent(0),
	m_iOpenRangedAttackMod(0),
	m_iRoughAttackPercent(0),
	m_iRoughRangedAttackMod(0),
	m_iAttackFortifiedMod(0),
	m_iAttackWoundedMod(0),
	m_iFlankAttackModifier(0),
	m_iNearbyEnemyCombatMod(0),
	m_iNearbyEnemyCombatRange(0),
	m_iOpenDefensePercent(0),
	m_iRoughDefensePercent(0),
	m_iExtraAttacks(0),
	m_bGreatGeneral(false),
	m_bGreatAdmiral(false),
	m_iGreatGeneralModifier(0),
	m_bGreatGeneralReceivesMovement(false),
	m_iGreatGeneralCombatModifier(0),
	m_iFriendlyLandsModifier(0),
	m_iFriendlyLandsAttackModifier(0),
	m_iOutsideFriendlyLandsModifier(0),
	m_iCommandType(NO_COMMAND),
	m_iUpgradeDiscount(0),
	m_iExperiencePercent(0),
	m_iAdjacentMod(0),
	m_iAttackMod(0),
	m_iDefenseMod(0),
	m_iDropRange(0),
	m_iExtraNavalMoves(0),
	m_iHPHealedIfDefeatEnemy(0),
	m_iExtraWithdrawal(0),
	m_iEmbarkExtraVisibility(0),
	m_iEmbarkDefenseModifier(0),
	m_iCapitalDefenseModifier(0),
	m_iCapitalDefenseFalloff(0),
	m_iCityAttackPlunderModifier(0),
	m_iReligiousStrengthLossRivalTerritory(0),
	m_bCannotBeChosen(false),
	m_bLostWithUpgrade(false),
	m_bInstaHeal(false),
	m_bLeader(false),
	m_bBlitz(false),
	m_bAmphib(false),
	m_bRiver(false),
	m_bEnemyRoute(false),
	m_bRivalTerritory(false),
	m_bMustSetUpToRangedAttack(false),
	m_bRangedSupportFire(false),
	m_bAlwaysHeal(false),
	m_bHealOutsideFriendly(false),
	m_bHillsDoubleMove(false),
	m_bIgnoreTerrainCost(false),
	m_bRoughTerrainEndsTurn(false),
	m_bHoveringUnit(false),
	m_bFlatMovementCost(false),
	m_bCanMoveImpassable(false),
	m_bNoCapture(false),
	m_bOnlyDefensive(false),
	m_bNoDefensiveBonus(false),
	m_bNukeImmune(false),
	m_bHiddenNationality(false),
	m_bAlwaysHostile(false),
	m_bNoRevealMap(false),
	m_bRecon(false),
	m_bCanMoveAllTerrain(false),
	m_bCanMoveAfterAttacking(false),
	m_bAirSweepCapable(false),
	m_bAllowsEmbarkation(false),
	m_bRangeAttackIgnoreLOS(false),
	m_bFreePillageMoves(false),
	m_bHealOnPillage(false),
	m_bHealIfDefeatExcludesBarbarians(false),
	m_bEmbarkedAllWater(false),
	m_bCityAttackOnly(false),
	m_bCaptureDefeatedEnemy(false),
	m_bIgnoreGreatGeneralBenefit(false),
	m_piTerrainAttackPercent(NULL),
	m_piTerrainDefensePercent(NULL),
	m_piFeatureAttackPercent(NULL),
	m_piFeatureDefensePercent(NULL),
	m_piUnitCombatModifierPercent(NULL),
	m_piUnitClassModifierPercent(NULL),
	m_piDomainModifierPercent(NULL),
	m_piFeaturePassableTech(NULL),
	m_piUnitClassAttackModifier(NULL),
	m_piUnitClassDefenseModifier(NULL),
	m_pbTerrainDoubleMove(NULL),
	m_pbFeatureDoubleMove(NULL),
	m_pbTerrainImpassable(NULL),
	m_piTerrainPassableTech(NULL),
	m_pbFeatureImpassable(NULL),
	m_pbUnitCombat(NULL),
	m_pbCivilianUnitType(NULL)
{
}

/// Destructor
CvPromotionEntry::~CvPromotionEntry(void)
{
	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piUnitClassModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_piFeaturePassableTech);
	SAFE_DELETE_ARRAY(m_piUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbTerrainImpassable);
	SAFE_DELETE_ARRAY(m_piTerrainPassableTech);
	SAFE_DELETE_ARRAY(m_pbFeatureImpassable);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	SAFE_DELETE_ARRAY(m_pbCivilianUnitType);
}
//------------------------------------------------------------------------------
bool CvPromotionEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvHotKeyInfo::CacheResults(kResults, kUtility))
		return false;

	SetSound(kResults.GetText("Sound"));

	//Basic Properties
	m_bCannotBeChosen = kResults.GetBool("CannotBeChosen");
	m_bLostWithUpgrade = kResults.GetBool("LostWithUpgrade");
	m_bInstaHeal = kResults.GetBool("InstaHeal");
	m_bLeader = kResults.GetBool("Leader");
	m_bBlitz = kResults.GetBool("Blitz");
	m_bAmphib = kResults.GetBool("Amphib");
	m_bRiver = kResults.GetBool("River");
	m_bEnemyRoute = kResults.GetBool("EnemyRoute");
	m_bRivalTerritory = kResults.GetBool("RivalTerritory");
	m_bMustSetUpToRangedAttack = kResults.GetBool("MustSetUpToRangedAttack");
	m_bRangedSupportFire= kResults.GetBool("RangedSupportFire");
	m_bAlwaysHeal = kResults.GetBool("AlwaysHeal");
	m_bHealOutsideFriendly = kResults.GetBool("HealOutsideFriendly");
	m_bHillsDoubleMove = kResults.GetBool("HillsDoubleMove");
	m_bIgnoreTerrainCost = kResults.GetBool("IgnoreTerrainCost");
	m_bRoughTerrainEndsTurn = kResults.GetBool("RoughTerrainEndsTurn");
	m_bHoveringUnit = kResults.GetBool("HoveringUnit");
	m_bFlatMovementCost = kResults.GetBool("FlatMovementCost");
	m_bCanMoveImpassable = kResults.GetBool("CanMoveImpassable");
	m_bNoCapture = kResults.GetBool("NoCapture");
	m_bOnlyDefensive = kResults.GetBool("OnlyDefensive");
	m_bNoDefensiveBonus = kResults.GetBool("NoDefensiveBonus");
	m_bNukeImmune = kResults.GetBool("NukeImmune");
	m_bHiddenNationality = kResults.GetBool("HiddenNationality");
	m_bAlwaysHostile = kResults.GetBool("AlwaysHostile");
	m_bNoRevealMap = kResults.GetBool("NoRevealMap");
	m_bRecon = kResults.GetBool("Recon");
	m_bCanMoveAllTerrain = kResults.GetBool("CanMoveAllTerrain");
	m_bCanMoveAfterAttacking = kResults.GetBool("CanMoveAfterAttacking");
	m_bAirSweepCapable = kResults.GetBool("AirSweepCapable");
	m_bAllowsEmbarkation = kResults.GetBool("AllowsEmbarkation");
	m_bRangeAttackIgnoreLOS = kResults.GetBool("RangeAttackIgnoreLOS");
	m_bFreePillageMoves = kResults.GetBool("FreePillageMoves");
	m_bHealOnPillage = kResults.GetBool("HealOnPillage");
	m_bHealIfDefeatExcludesBarbarians = kResults.GetBool("HealIfDestroyExcludesBarbarians");
	m_bEmbarkedAllWater = kResults.GetBool("EmbarkedAllWater");
	m_bCityAttackOnly = kResults.GetBool("CityAttackOnly");
	m_bCaptureDefeatedEnemy = kResults.GetBool("CaptureDefeatedEnemy");
	m_bIgnoreGreatGeneralBenefit = kResults.GetBool("IgnoreGreatGeneralBenefit");

	m_iVisibilityChange = kResults.GetInt("VisibilityChange");
	m_iMovesChange = kResults.GetInt("MovesChange");
	m_iMoveDiscountChange = kResults.GetInt("MoveDiscountChange");
	m_iRangeChange = kResults.GetInt("RangeChange");
	m_iRangedAttackModifier = kResults.GetInt("RangedAttackModifier");
	m_iInterceptionCombatModifier = kResults.GetInt("InterceptionCombatModifier");
	m_iInterceptionDefenseDamageModifier = kResults.GetInt("InterceptionDefenseDamageModifier");
	m_iAirSweepCombatModifier = kResults.GetInt("AirSweepCombatModifier");
	m_iInterceptChanceChange = kResults.GetInt("InterceptChanceChange");
	m_iNumInterceptionChange = kResults.GetInt("NumInterceptionChange");
	m_iEvasionChange = kResults.GetInt("EvasionChange");
	m_iCargoChange = kResults.GetInt("CargoChange");
	m_iEnemyHealChange = kResults.GetInt("EnemyHealChange");
	m_iNeutralHealChange = kResults.GetInt("NeutralHealChange");
	m_iFriendlyHealChange = kResults.GetInt("FriendlyHealChange");
	m_iSameTileHealChange = kResults.GetInt("SameTileHealChange");
	m_iAdjacentTileHealChange = kResults.GetInt("AdjacentTileHealChange");
	m_iEnemyDamageChance = kResults.GetInt("EnemyDamageChance");
	m_iNeutralDamageChance = kResults.GetInt("NeutralDamageChance");
	m_iEnemyDamage = kResults.GetInt("EnemyDamage");
	m_iNeutralDamage = kResults.GetInt("NeutralDamage");
	m_iCombatPercent = kResults.GetInt("CombatPercent");
	m_iCityAttackPercent = kResults.GetInt("CityAttack");
	m_iCityDefensePercent = kResults.GetInt("CityDefense");
	m_iRangedDefenseMod = kResults.GetInt("RangedDefenseMod");
	m_iHillsAttackPercent = kResults.GetInt("HillsAttack");
	m_iHillsDefensePercent = kResults.GetInt("HillsDefense");
	m_iOpenAttackPercent = kResults.GetInt("OpenAttack");
	m_iOpenRangedAttackMod = kResults.GetInt("OpenRangedAttackMod");
	m_iRoughAttackPercent = kResults.GetInt("RoughAttack");
	m_iRoughRangedAttackMod = kResults.GetInt("RoughRangedAttackMod");
	m_iAttackFortifiedMod = kResults.GetInt("AttackFortifiedMod");
	m_iAttackWoundedMod = kResults.GetInt("AttackWoundedMod");
	m_iFlankAttackModifier = kResults.GetInt("FlankAttackModifier");
	m_iNearbyEnemyCombatMod = kResults.GetInt("NearbyEnemyCombatMod");
	m_iNearbyEnemyCombatRange = kResults.GetInt("NearbyEnemyCombatRange");
	m_iOpenDefensePercent = kResults.GetInt("OpenDefense");
	m_iRoughDefensePercent = kResults.GetInt("RoughDefense");
	m_iExtraAttacks = kResults.GetInt("ExtraAttacks");
	m_bGreatGeneral = kResults.GetBool("GreatGeneral");
	m_bGreatAdmiral = kResults.GetBool("GreatAdmiral");
	m_iGreatGeneralModifier = kResults.GetInt("GreatGeneralModifier");
	m_bGreatGeneralReceivesMovement = kResults.GetBool("GreatGeneralReceivesMovement");
	m_iGreatGeneralCombatModifier = kResults.GetInt("GreatGeneralCombatModifier");
	m_iFriendlyLandsModifier = kResults.GetInt("FriendlyLandsModifier");
	m_iFriendlyLandsAttackModifier = kResults.GetInt("FriendlyLandsAttackModifier");
	m_iOutsideFriendlyLandsModifier = kResults.GetInt("OutsideFriendlyLandsModifier");
	m_iUpgradeDiscount = kResults.GetInt("UpgradeDiscount");
	m_iExperiencePercent = kResults.GetInt("ExperiencePercent");
	m_iAdjacentMod = kResults.GetInt("AdjacentMod");
	m_iAttackMod = kResults.GetInt("AttackMod");
	m_iDefenseMod = kResults.GetInt("DefenseMod");
	m_iDropRange = kResults.GetInt("DropRange");
	m_iExtraNavalMoves = kResults.GetInt("ExtraNavalMovement");
	m_iHPHealedIfDefeatEnemy = kResults.GetInt("HPHealedIfDestroyEnemy");
	m_iExtraWithdrawal = kResults.GetInt("ExtraWithdrawal");
	m_iEmbarkExtraVisibility = kResults.GetInt("EmbarkExtraVisibility");
	m_iEmbarkDefenseModifier = kResults.GetInt("EmbarkDefenseModifier");
	m_iCapitalDefenseModifier = kResults.GetInt("CapitalDefenseModifier");
	m_iCapitalDefenseFalloff = kResults.GetInt("CapitalDefenseFalloff");
	m_iCityAttackPlunderModifier = kResults.GetInt("CityAttackPlunderModifier");
	m_iReligiousStrengthLossRivalTerritory = kResults.GetInt("ReligiousStrengthLossRivalTerritory");

	//References
	const char* szLayerAnimationPath = kResults.GetText("LayerAnimationPath");
	m_iLayerAnimationPath = GC.getInfoTypeForString(szLayerAnimationPath, true);

	const char* szTechPrereq = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTechPrereq, true);

	const char* szInvisible = kResults.GetText("Invisible");
	m_iInvisibleType = GC.getInfoTypeForString(szInvisible, true);

	const char* szSeeInvisible = kResults.GetText("SeeInvisible");
	m_iSeeInvisibleType = GC.getInfoTypeForString(szSeeInvisible, true);

	const char* szPromotionPrereq = kResults.GetText("PromotionPrereq");
	m_iPrereqPromotion = GC.getInfoTypeForString(szPromotionPrereq, true);

	const char* szPromotionPrereqOr1 = kResults.GetText("PromotionPrereqOr1");
	m_iPrereqOrPromotion1 = GC.getInfoTypeForString(szPromotionPrereqOr1, true);

	const char* szPromotionPrereqOr2 = kResults.GetText("PromotionPrereqOr2");
	m_iPrereqOrPromotion2 = GC.getInfoTypeForString(szPromotionPrereqOr2, true);

	const char* szPromotionPrereqOr3 = kResults.GetText("PromotionPrereqOr3");
	m_iPrereqOrPromotion3 = GC.getInfoTypeForString(szPromotionPrereqOr3, true);

	const char* szPromotionPrereqOr4 = kResults.GetText("PromotionPrereqOr4");
	m_iPrereqOrPromotion4 = GC.getInfoTypeForString(szPromotionPrereqOr4, true);

	const char* szPromotionPrereqOr5 = kResults.GetText("PromotionPrereqOr5");
	m_iPrereqOrPromotion5 = GC.getInfoTypeForString(szPromotionPrereqOr5, true);

	const char* szPromotionPrereqOr6 = kResults.GetText("PromotionPrereqOr6");
	m_iPrereqOrPromotion6 = GC.getInfoTypeForString(szPromotionPrereqOr6, true);

	//Arrays
	const int iNumUnitClasses = kUtility.MaxRows("UnitClasses");
	const int iNumTerrains = GC.getNumTerrainInfos();
	const int iNumFeatures = GC.getNumFeatureInfos();
	const int iNumDomains = kUtility.MaxRows("Domains");
	DEBUG_VARIABLE(iNumDomains);
	const int iNumUnitCombatClasses = kUtility.MaxRows("UnitCombatInfos");
	const int iNumUnitTypes = kUtility.MaxRows("Units");

	const char* szPromotionType = GetType();

	//UnitPromotions_Terrains
	{
		kUtility.InitializeArray(m_piTerrainAttackPercent, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainDefensePercent, iNumTerrains, 0);
		kUtility.InitializeArray(m_piTerrainPassableTech, iNumTerrains, NO_TECH);
		kUtility.InitializeArray(m_pbTerrainDoubleMove, iNumTerrains, false);
		kUtility.InitializeArray(m_pbTerrainImpassable, iNumTerrains, false);

		std::string sqlKey = "UnitPromotions_Terrains";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Terrains.ID as TerrainID, coalesce(Technologies.ID, -1) as PassableTechID, UnitPromotions_Terrains.* from UnitPromotions_Terrains inner join Terrains on TerrainType = Terrains.Type left join Technologies on PassableTech = Technologies.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iTerrainID = pResults->GetInt("TerrainID");
			CvAssert(iTerrainID > -1 && iTerrainID < iNumTerrains);

			const int iTerrainAttack = pResults->GetInt("Attack");
			m_piTerrainAttackPercent[iTerrainID] = iTerrainAttack;

			const int iTerrainDefense = pResults->GetInt("Defense");
			m_piTerrainDefensePercent[iTerrainID] = iTerrainDefense;

			const bool bDoubleMove = pResults->GetBool("DoubleMove");
			m_pbTerrainDoubleMove[iTerrainID] = bDoubleMove;

			const bool bImpassable = pResults->GetBool("Impassable");
			m_pbTerrainImpassable[iTerrainID] = bImpassable;

			const int iPassableTechID = pResults->GetInt("PassableTechID");
			m_piTerrainPassableTech[iTerrainID] = iPassableTechID;
		}
	}

	//UnitPromotions_Features
	{
		kUtility.InitializeArray(m_piFeatureAttackPercent, iNumFeatures, 0);
		kUtility.InitializeArray(m_piFeatureDefensePercent, iNumFeatures, 0);
		kUtility.InitializeArray(m_piFeaturePassableTech, iNumFeatures, NO_TECH);
		kUtility.InitializeArray(m_pbFeatureDoubleMove, iNumFeatures, false);
		kUtility.InitializeArray(m_pbFeatureImpassable, iNumFeatures, false);

		std::string sqlKey = "UnitPromotions_Features";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Features.ID as FeatureID, coalesce(Technologies.ID, -1) as PassableTechID, UnitPromotions_Features.* from UnitPromotions_Features inner join Features on FeatureType = Features.Type left join Technologies on PassableTech = Technologies.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iFeatureID = pResults->GetInt("FeatureID");
			CvAssert(iFeatureID > -1 && iFeatureID < iNumFeatures);

			const int iFeatureAttack = pResults->GetInt("Attack");
			m_piFeatureAttackPercent[iFeatureID] = iFeatureAttack;

			const int iFeatureDefense = pResults->GetInt("Defense");
			m_piFeatureDefensePercent[iFeatureID] = iFeatureDefense;

			const bool bDoubleMove = pResults->GetBool("DoubleMove");
			m_pbFeatureDoubleMove[iFeatureID] = bDoubleMove;

			const bool bImpassable = pResults->GetBool("Impassable");
			m_pbFeatureImpassable[iFeatureID] = bImpassable;

			const int iPassableTech = pResults->GetInt("PassableTechID");
			m_piFeaturePassableTech[iFeatureID] = iPassableTech;
		}
	}

	//UnitPromotions_UnitClasses
	{
		kUtility.InitializeArray(m_piUnitClassModifierPercent, iNumUnitClasses, 0);
		kUtility.InitializeArray(m_piUnitClassAttackModifier, iNumUnitClasses, 0);
		kUtility.InitializeArray(m_piUnitClassDefenseModifier, iNumUnitClasses, 0);

		std::string sqlKey = "UnitPromotions_UnitClasses";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitClasses.ID, Modifier, Attack, Defense from UnitPromotions_UnitClasses inner join UnitClasses on UnitClassType = UnitClasses.Type where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitClassID = pResults->GetInt(0);
			CvAssert(iUnitClassID > -1 && iUnitClassID  < iNumUnitClasses);

			const int iModifier = pResults->GetInt("Modifier");
			m_piUnitClassModifierPercent[iUnitClassID] = iModifier;

			const int iAttack = pResults->GetInt("Attack");
			m_piUnitClassAttackModifier[iUnitClassID] = iAttack;

			const int iDefense = pResults->GetInt("Defense");
			m_piUnitClassDefenseModifier[iUnitClassID] = iDefense;
		}

		pResults->Reset();
	}

	//UnitPromotions_Domains
	{
		kUtility.InitializeArray(m_piDomainModifierPercent, NUM_DOMAIN_TYPES, 0);

		std::string sqlKey = "m_piDomainModifierPercent";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Domains.ID, Modifier from UnitPromotions_Domains inner join Domains on DomainType = Domains.Type where PromotionType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iDomainID = pResults->GetInt(0);
			CvAssert(iDomainID > -1 && iDomainID < iNumDomains);

			const int iDomainMod = pResults->GetInt(1);
			if (iDomainID > -1 && iDomainID < NUM_DOMAIN_TYPES)
				m_piDomainModifierPercent[iDomainID] = iDomainMod;
		}

		pResults->Reset();
	}

	//UnitPromotions_UnitCombatMods
	{
		kUtility.InitializeArray(m_piUnitCombatModifierPercent, iNumUnitCombatClasses, 0);

		std::string sqlKey = "m_piUnitCombatModifierPercent";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID, Modifier from UnitPromotions_UnitCombatMods inner join UnitCombatInfos on UnitCombatInfos.Type = UnitCombatType where PromotionType = ?;";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitCombatID = pResults->GetInt(0);
			CvAssert(iUnitCombatID > -1 && iUnitCombatID < iNumUnitCombatClasses);

			const int iUnitCombatMod = pResults->GetInt(1);
			m_piUnitCombatModifierPercent[iUnitCombatID] = iUnitCombatMod;
		}

		pResults->Reset();
	}

	//UnitPromotions_UnitCombats
	{
		kUtility.InitializeArray(m_pbUnitCombat, iNumUnitCombatClasses, false);

		std::string sqlKey = "m_pbUnitCombat";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select UnitCombatInfos.ID from UnitPromotions_UnitCombats inner join UnitCombatInfos On UnitCombatInfos.Type = UnitCombatType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnitCombatInfoID = pResults->GetInt(0);
			CvAssert(iUnitCombatInfoID < iNumUnitCombatClasses);

			m_pbUnitCombat[iUnitCombatInfoID] = true;
		}

		pResults->Reset();
	}

	//UnitPromotions_CivilianUnitType
	{
		kUtility.InitializeArray(m_pbCivilianUnitType, iNumUnitTypes, false);

		std::string sqlKey = "m_pbCivilianUnitType";
		Database::Results* pResults = kUtility.GetResults(sqlKey);
		if(pResults == NULL)
		{
			const char* szSQL = "select Units.ID from UnitPromotions_CivilianUnitType inner join Units On Units.Type = UnitType where PromotionType = ?";
			pResults = kUtility.PrepareResults(sqlKey, szSQL);
		}

		CvAssert(pResults);
		if(!pResults) return false;

		pResults->Bind(1, szPromotionType);

		while(pResults->Step())
		{
			const int iUnit = (UnitTypes)pResults->GetInt(0);
			CvAssert(iUnit < iNumUnitTypes);

			m_pbCivilianUnitType[iUnit] = true;
		}

		pResults->Reset();
	}

	return true;
}

/// Accessor: The granny animation index used to indicate that a unit has leveled up
int CvPromotionEntry::GetLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}

/// Accessor: The promotion required before this promotion is available
int CvPromotionEntry::GetPrereqPromotion() const
{
	return m_iPrereqPromotion;
}

/// Set the promotion required before this promotion is available
void CvPromotionEntry::SetPrereqPromotion(int i)
{
	m_iPrereqPromotion = i;
}

/// Accessor: Gets promotion 1 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion1() const
{
	return m_iPrereqOrPromotion1;
}

/// Accessor: Sets promotion 1 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion1(int i)
{
	m_iPrereqOrPromotion1 = i;
}

/// Accessor: Gets promotion 2 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion2() const
{
	return m_iPrereqOrPromotion2;
}

/// Accessor: Sets promotion 2 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion2(int i)
{
	m_iPrereqOrPromotion2 = i;
}

/// Accessor: Gets promotion 3 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion3() const
{
	return m_iPrereqOrPromotion3;
}

/// Accessor: Sets promotion 3 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion3(int i)
{
	m_iPrereqOrPromotion3 = i;
}

/// Accessor: Gets promotion 4 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion4() const
{
	return m_iPrereqOrPromotion4;
}

/// Accessor: Sets promotion 4 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion4(int i)
{
	m_iPrereqOrPromotion4 = i;
}

/// Accessor: Gets promotion 5 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion5() const
{
	return m_iPrereqOrPromotion5;
}

/// Accessor: Sets promotion 5 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion5(int i)
{
	m_iPrereqOrPromotion5 = i;
}

/// Accessor: Gets promotion 6 of an either/or promotion prerequisite.
int CvPromotionEntry::GetPrereqOrPromotion6() const
{
	return m_iPrereqOrPromotion6;
}

/// Accessor: Sets promotion 6 of an either/or promotion prerequisite.
void CvPromotionEntry::SetPrereqOrPromotion6(int i)
{
	m_iPrereqOrPromotion6 = i;
}

/// Accessor: Gets the tech prerequisite for this promotion
int CvPromotionEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Accessor: What type of invisible this unit is
int CvPromotionEntry::GetInvisibleType() const
{
	return m_iInvisibleType;
}

/// Accessor: What type of invisible this unit can see
int CvPromotionEntry::GetSeeInvisibleType() const
{
	return m_iSeeInvisibleType;
}

/// Accessor: How many additional tiles this promotion allows a unit to see (can be negative)
int CvPromotionEntry::GetVisibilityChange() const
{
	return m_iVisibilityChange;
}

/// Accessor: How many additional tiles a unit may move
int CvPromotionEntry::GetMovesChange() const
{
	return m_iMovesChange;
}

/// Accessor: How much movement is discounted
int CvPromotionEntry::GetMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

/// Accessor: How much the air range of the unit is modified
int CvPromotionEntry::GetRangeChange() const
{
	return m_iRangeChange;
}

/// Accessor: How much the ranged strength of the unit is modified
int CvPromotionEntry::GetRangedAttackModifier() const
{
	return m_iRangedAttackModifier;
}

/// Accessor: How much the strength of the unit is modified when intercepting
int CvPromotionEntry::GetInterceptionCombatModifier() const
{
	return m_iInterceptionCombatModifier;
}

/// Accessor: Mod to Damage taken if intercepted
int CvPromotionEntry::GetInterceptionDefenseDamageModifier() const
{
	return m_iInterceptionDefenseDamageModifier;
}

/// Accessor: How much the strength of the unit is modified when air sweeping
int CvPromotionEntry::GetAirSweepCombatModifier() const
{
	return m_iAirSweepCombatModifier;
}

/// Accessor: How much the intercept chance changes for an air unit
int CvPromotionEntry::GetInterceptChanceChange() const
{
	return m_iInterceptChanceChange;
}

/// Accessor: Numbe rof extra interceptions that may be made on one turn
int CvPromotionEntry::GetNumInterceptionChange() const
{
	return m_iNumInterceptionChange;
}

/// Accessor: How well an air unit can evade interception
int CvPromotionEntry::GetEvasionChange() const
{
	return m_iEvasionChange;
}

/// Accessor: The amount of cargo a unit can carry
int CvPromotionEntry::GetCargoChange() const
{
	return m_iCargoChange;
}

/// Accessor: Damage/health bonus per turn in enemy lands
int CvPromotionEntry::GetEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

/// Accessor: Damage/health bonus per turn in neutral lands
int CvPromotionEntry::GetNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

/// Accessor: Damage/health bonus per turn in friendly lands
int CvPromotionEntry::GetFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

/// Accessor: Heals units in the same tile
int CvPromotionEntry::GetSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

/// Accessor: Heals units in adjacent tiles
int CvPromotionEntry::GetAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

/// Accessor: Chance per turn to take damage in enemy lands
int CvPromotionEntry::GetEnemyDamageChance() const
{
	return m_iEnemyDamageChance;
}

/// Accessor: Chance per turn to take damage in neutral lands
int CvPromotionEntry::GetNeutralDamageChance() const
{
	return m_iNeutralDamageChance;
}

/// Accessor: Amount of damage taken by being in enemy lands
int CvPromotionEntry::GetEnemyDamage() const
{
	return m_iEnemyDamage;
}

/// Accessor: Amount of damage taken by being in neutral lands
int CvPromotionEntry::GetNeutralDamage() const
{
	return m_iNeutralDamage;
}

/// Accessor: Bonus strength percent
int CvPromotionEntry::GetCombatPercent() const
{
	return m_iCombatPercent;
}

/// Accessor: Bonus city attack percent
int CvPromotionEntry::GetCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

/// Accessor: Bonus city defense percent
int CvPromotionEntry::GetCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

/// Accessor: Bonus ranged defense percent
int CvPromotionEntry::GetRangedDefenseMod() const
{
	return m_iRangedDefenseMod;
}

/// Accessor: Bonus hills attack percent
int CvPromotionEntry::GetHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

/// Accessor: Bonus hills defense percent
int CvPromotionEntry::GetHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

/// Accessor: Bonus open terrain attack percent
int CvPromotionEntry::GetOpenAttackPercent() const
{
	return m_iOpenAttackPercent;
}

/// Accessor: Bonus open terrain ranged attack mod
int CvPromotionEntry::GetOpenRangedAttackMod() const
{
	return m_iOpenRangedAttackMod;
}

/// Accessor: Bonus open terrain attack percent
int CvPromotionEntry::GetRoughAttackPercent() const
{
	return m_iRoughAttackPercent;
}

/// Accessor: Bonus rough terrain ranged attack mod
int CvPromotionEntry::GetRoughRangedAttackMod() const
{
	return m_iRoughRangedAttackMod;
}

/// Accessor: Bonus when attacking fortified units
int CvPromotionEntry::GetAttackFortifiedMod() const
{
	return m_iAttackFortifiedMod;
}

/// Accessor: Bonus when attacking wounded units
int CvPromotionEntry::GetAttackWoundedMod() const
{
	return m_iAttackWoundedMod;
}

/// Accessor: Bonus when making a flank attack
int CvPromotionEntry::GetFlankAttackModifier() const
{
	return m_iFlankAttackModifier;
}

/// Accessor: Modifier on nearby enemy combat units
int CvPromotionEntry::GetNearbyEnemyCombatMod() const
{
	return m_iNearbyEnemyCombatMod;
}

/// Accessor: Range of modifier on nearby enemy combat units
int CvPromotionEntry::GetNearbyEnemyCombatRange() const
{
	return m_iNearbyEnemyCombatRange;
}

/// Accessor: Bonus open terrain defense percent
int CvPromotionEntry::GetOpenDefensePercent() const
{
	return m_iOpenDefensePercent;
}

/// Accessor: Bonus open terrain defense percent
int CvPromotionEntry::GetRoughDefensePercent() const
{
	return m_iRoughDefensePercent;
}

/// Accessor: Does the unit have more than 1 attack?
int CvPromotionEntry::GetExtraAttacks() const
{
	return m_iExtraAttacks;
}

/// Accessor: Does this Promotion make a Unit a Great General?
bool CvPromotionEntry::IsGreatGeneral() const
{
	return m_bGreatGeneral;
}

/// Accessor: Does this Promotion make a Unit a Great Admiral?
bool CvPromotionEntry::IsGreatAdmiral() const
{
	return m_bGreatAdmiral;
}

/// Accessor: Increase in rate of great general creation
int CvPromotionEntry::GetGreatGeneralModifier() const
{
	return m_iGreatGeneralModifier;
}

/// Accessor: Does this Promotion make a Great General move at this unit's rate?
bool CvPromotionEntry::IsGreatGeneralReceivesMovement() const
{
	return m_bGreatGeneralReceivesMovement;
}

/// Accessor: Combat bonus when stacked with Great General
int CvPromotionEntry::GetGreatGeneralCombatModifier() const
{
	return m_iGreatGeneralCombatModifier;
}

/// Accessor: Combat mod for fighting in friendly territory
int CvPromotionEntry::GetFriendlyLandsModifier() const
{
	return m_iFriendlyLandsModifier;
}

/// Accessor: Attack mod for fighting in friendly territory
int CvPromotionEntry::GetFriendlyLandsAttackModifier() const
{
	return m_iFriendlyLandsAttackModifier;
}

/// Accessor: Combat mod for fighting outside friendly territory
int CvPromotionEntry::GetOutsideFriendlyLandsModifier() const
{
	return m_iOutsideFriendlyLandsModifier;
}

/// Accessor: Returns the command type for this HotKeyInfo class (which would be COMMAND_PROMOTION)
int CvPromotionEntry::GetCommandType() const
{
	return m_iCommandType;
}

/// Sets the command type for this class.
void CvPromotionEntry::SetCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

/// Accessor: How much upgrading this unit is discounted
int CvPromotionEntry::GetUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

/// Accessor: Increased rate of gaining experience
int CvPromotionEntry::GetExperiencePercent() const
{
	return m_iExperiencePercent;
}

/// Accessor: Bonus when adjacent to a friendly Unit
int CvPromotionEntry::GetAdjacentMod() const
{
	return m_iAdjacentMod;
}

/// Accessor: Percent attack change
int CvPromotionEntry::GetAttackMod() const
{
	return m_iAttackMod;
}

/// Accessor: Percent defense change
int CvPromotionEntry::GetDefenseMod() const
{
	return m_iDefenseMod;
}

/// Accessor: Number of tiles away a unit may paradrop
int CvPromotionEntry::GetDropRange() const
{
	return m_iDropRange;
}

/// Accessor: Number of extra tiles of movement for naval vessels
int CvPromotionEntry::GetExtraNavalMoves() const
{
	return m_iExtraNavalMoves;
}

/// Accessor: HP healed after defeating enemy (up to 10 for full heal)
int CvPromotionEntry::GetHPHealedIfDefeatEnemy() const
{
	return m_iHPHealedIfDefeatEnemy;
}

/// Accessor: maximum chance to withdraw before melee
int CvPromotionEntry::GetExtraWithdrawal() const
{
	return m_iExtraWithdrawal;
}

/// Accessor: extra sight range when embarked
int CvPromotionEntry::GetEmbarkExtraVisibility() const
{
	return m_iEmbarkExtraVisibility;
}

/// Accessor: extra defense when embarked
int CvPromotionEntry::GetEmbarkDefenseModifier() const
{
	return m_iEmbarkDefenseModifier;
}

/// Accessor: bonus defending near capital
int CvPromotionEntry::GetCapitalDefenseModifier() const
{
	return m_iCapitalDefenseModifier;
}

/// Accessor: drop per hex away from capital of CapitalDefenseModifier
int CvPromotionEntry::GetCapitalDefenseFalloff() const
{
	return m_iCapitalDefenseFalloff;
}

/// Accessor: gold earned from damage on an attacked city
int CvPromotionEntry::GetCityAttackPlunderModifier() const
{
	return m_iCityAttackPlunderModifier;
}

/// Accessor: loss of strength at end of each turn in rival territory without open borders
int CvPromotionEntry::GetReligiousStrengthLossRivalTerritory() const
{
	return m_iReligiousStrengthLossRivalTerritory;
}

/// Accessor: Can this Promotion be earned through normal leveling?
bool CvPromotionEntry::IsCannotBeChosen() const
{
	return m_bCannotBeChosen;
}

/// Accessor: Is this lost with upgrading?
bool CvPromotionEntry::IsLostWithUpgrade() const
{
	return m_bLostWithUpgrade;
}

/// Accessor: Does this promotion instantly heal a Unit?
bool CvPromotionEntry::IsInstaHeal() const
{
	return m_bInstaHeal;
}

/// Accessor: Does this unit have a warlord attached to it?
bool CvPromotionEntry::IsLeader() const
{
	return m_bLeader;
}

/// Accessor: Can this unit attack multiple times a turn?
bool CvPromotionEntry::IsBlitz() const
{
	return m_bBlitz;
}

/// Accessor: Can this unit attack from the sea without penalty?
bool CvPromotionEntry::IsAmphib() const
{
	return m_bAmphib;
}

/// Accessor: Can this unit attack across rivers without penalty?
bool CvPromotionEntry::IsRiver() const
{
	return m_bRiver;
}

/// Accessor: Can this unit use enemy roads?
bool CvPromotionEntry::IsEnemyRoute() const
{
	return m_bEnemyRoute;
}

/// Accessor: Can enter rival territory
bool CvPromotionEntry::IsRivalTerritory() const
{
	return m_bRivalTerritory;
}

/// Accessor: Must this unit set up multiple turns for a ranged attack
bool CvPromotionEntry::IsMustSetUpToRangedAttack() const
{
	return m_bMustSetUpToRangedAttack;
}

/// Accessor: Can this Unit provide Ranged Support Fire?
bool CvPromotionEntry::IsRangedSupportFire() const
{
	return m_bRangedSupportFire;
}

/// Accessor: Unit can heal while moving
bool CvPromotionEntry::IsAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

/// Accessor: Unit can heal outside friendly territory (naval units)
bool CvPromotionEntry::IsHealOutsideFriendly() const
{
	return m_bHealOutsideFriendly;
}

/// Accessor: Double movement in hills
bool CvPromotionEntry::IsHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

/// Accessor: Ignores terrain movement penalties
bool CvPromotionEntry::IsIgnoreTerrainCost() const
{
	return m_bIgnoreTerrainCost;
}

/// Accessor: Entering rough terrain uses up a unit's movement? (regardless of how many moves he has)
bool CvPromotionEntry::IsRoughTerrainEndsTurn() const
{
	return m_bRoughTerrainEndsTurn;
}

/// Accessor: Unit may pass over coast and Mountains
bool CvPromotionEntry::IsHoveringUnit() const
{
	return m_bHoveringUnit;
}

/// Accessor: All terrain costs 1 to enter
bool CvPromotionEntry::IsFlatMovementCost() const
{
	return m_bFlatMovementCost;
}

/// Accessor: May enter ice tiles
bool CvPromotionEntry::IsCanMoveImpassable() const
{
	return m_bCanMoveImpassable;
}

/// Accessor: Unable to capture cities
bool CvPromotionEntry::IsNoCapture() const
{
	return m_bNoCapture;
}

/// Accessor: Unable to attack enemy units
bool CvPromotionEntry::IsOnlyDefensive() const
{
	return m_bOnlyDefensive;
}

/// Accessor: No defensive bonuses
bool CvPromotionEntry::IsNoDefensiveBonus() const
{
	return m_bNoDefensiveBonus;
}

/// Accessor: Cannot be hurt by nukes?
bool CvPromotionEntry::IsNukeImmune() const
{
	return m_bNukeImmune;
}

/// Accessor: Nationality is hidden from other players
bool CvPromotionEntry::IsHiddenNationality() const
{
	return m_bHiddenNationality;
}

/// Accessor: Can attack without declaring war
bool CvPromotionEntry::IsAlwaysHostile() const
{
	return m_bAlwaysHostile;
}

/// Accessor: Cannot reveal terrain unless it is inside a player's territory
bool CvPromotionEntry::IsNoRevealMap() const
{
	return m_bNoRevealMap;
}

/// Accessor: Is this a Recon unit? (free sight all around it)
bool CvPromotionEntry::IsRecon() const
{
	return m_bRecon;
}

/// Accessor: Can move through all terrain?
bool CvPromotionEntry::CanMoveAllTerrain() const
{
	return m_bCanMoveAllTerrain;
}

/// Accessor: Can move after attacking another Unit?
bool CvPromotionEntry::IsCanMoveAfterAttacking() const
{
	return m_bCanMoveAfterAttacking;
}

/// Accessor: Can this unit air sweep?
bool CvPromotionEntry::IsAirSweepCapable() const
{
	return m_bAirSweepCapable;
}

/// Accessor: Can this unit embark?
bool CvPromotionEntry::IsAllowsEmbarkation() const
{
	return m_bAllowsEmbarkation;
}

/// Accessor: Can this unit shoot over obstacles?
bool CvPromotionEntry::IsRangeAttackIgnoreLOS() const
{
	return m_bRangeAttackIgnoreLOS;
}

/// Accessor: Lose no movement when pillaging?
bool CvPromotionEntry::IsFreePillageMoves() const
{
	return m_bFreePillageMoves;
}

/// Accessor: When pillaging, does the unit heal?
bool CvPromotionEntry::IsHealOnPillage() const
{
	return m_bHealOnPillage;
}

/// Accessor: Do we only get healed after a combat win if fighting a real civ or minor?
bool CvPromotionEntry::IsHealIfDefeatExcludeBarbarians() const
{
	return m_bHealIfDefeatExcludesBarbarians;
}

/// Accessor: Do we not get run over by naval units when embarked at sea?
bool CvPromotionEntry::IsEmbarkedAllWater() const
{
	return m_bEmbarkedAllWater;
}

/// Accessor: Does this unit only attack cities
bool CvPromotionEntry::IsCityAttackOnly() const
{
	return m_bCityAttackOnly;
}

/// Accessor: Is a defeated enemy captured?
bool CvPromotionEntry::IsCaptureDefeatedEnemy() const
{
	return m_bCaptureDefeatedEnemy;
}

/// Accessor: Should this infantry ignore benefits from great generals
bool CvPromotionEntry::IsIgnoreGreatGeneralBenefit() const
{
	return m_bIgnoreGreatGeneralBenefit;
}

/// Accessor: Sound to play when the promotion is gained
const char* CvPromotionEntry::GetSound() const
{
	return m_strSound;
}

/// Set the sound to be played when the promotion is gained
void CvPromotionEntry::SetSound(const char* szVal)
{
	m_strSound = szVal;
}

// ARRAYS

/// Returns an array of bonuses when attacking a tile of a given terrain
int CvPromotionEntry::GetTerrainAttackPercent(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainAttackPercent)
	{
		return m_piTerrainAttackPercent[i];
	}

	return -1;
}

/// Returns an array of bonuses when defending a tile of a given terrain
int CvPromotionEntry::GetTerrainDefensePercent(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainDefensePercent)
	{
		return m_piTerrainDefensePercent[i];
	}

	return-1;
}

/// Returns an array of bonuses when attacking a tile with a terrain feature
int CvPromotionEntry::GetFeatureAttackPercent(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeatureAttackPercent)
	{
		return m_piFeatureAttackPercent[i];
	}

	return -1;
}

/// Returns an array of bonuses when defending a tile with a terrain feature
int CvPromotionEntry::GetFeatureDefensePercent(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeatureDefensePercent)
	{
		return m_piFeatureDefensePercent[i];
	}

	return -1;
}

/// Returns an array of bonuses when fighting against a certain unit
int CvPromotionEntry::GetUnitCombatModifierPercent(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_piUnitCombatModifierPercent)
	{
		return m_piUnitCombatModifierPercent[i];
	}

	return -1;
}

/// Returns an array of bonuses when fighting against a type of unit
int CvPromotionEntry::GetUnitClassModifierPercent(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassModifierPercent)
	{
		return m_piUnitClassModifierPercent[i];
	}

	return -1;
}

/// Returns an array of bonuses when I have no idea
int CvPromotionEntry::GetDomainModifierPercent(int i) const
{
	CvAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < NUM_DOMAIN_TYPES && m_piDomainModifierPercent)
	{
		return m_piDomainModifierPercent[i];
	}

	return -1;
}

/// Percentage bonus when attacking a specific unit class
int CvPromotionEntry::GetUnitClassAttackModifier(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassAttackModifier)
	{
		return m_piUnitClassAttackModifier[i];
	}

	return -1;
}

/// Percentage bonus when defending against a specific unit class
int CvPromotionEntry::GetUnitClassDefenseModifier(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitClassInfos() && m_piUnitClassDefenseModifier)
	{
		return m_piUnitClassDefenseModifier[i];
	}

	return -1;
}

/// Returns an array that indicates if a feature type is traversable by the unit
int CvPromotionEntry::GetFeaturePassableTech(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_piFeaturePassableTech)
	{
		return m_piFeaturePassableTech[i];
	}

	return -1;
}

/// Returns an array that indicates if a unit can move twice in a type of terrain
bool CvPromotionEntry::GetTerrainDoubleMove(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainDoubleMove)
	{
		return m_pbTerrainDoubleMove[i];
	}

	return false;
}

/// Returns an array that indicates if a unit can move twice in a type of terrain feature
bool CvPromotionEntry::GetFeatureDoubleMove(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureDoubleMove)
	{
		return m_pbFeatureDoubleMove[i];
	}

	return false;
}

/// Returns an array that indicates if a terrain type is impassable
bool CvPromotionEntry::GetTerrainImpassable(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_pbTerrainImpassable)
	{
		return m_pbTerrainImpassable[i];
	}

	return false;
}

/// Returns an array that indicates what tech is needed to pass through a terrain type
int CvPromotionEntry::GetTerrainPassableTech(int i) const
{
	CvAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumTerrainInfos() && m_piTerrainPassableTech)
	{
		return m_piTerrainPassableTech[i];
	}

	return -1;
}

/// Returns an array that indicates what tech is needed to pass through a terrain feature type
bool CvPromotionEntry::GetFeatureImpassable(int i) const
{
	CvAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumFeatureInfos() && m_pbFeatureImpassable)
	{
		return m_pbFeatureImpassable[i];
	}

	return false;
}

/// Returns the combat classes that this promotion is available for
bool CvPromotionEntry::GetUnitCombatClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitCombatClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitCombatClassInfos() && m_pbUnitCombat)
	{
		return m_pbUnitCombat[i];
	}

	return false;
}

/// Returns the civilian unit type that this promotion is available for
bool CvPromotionEntry::GetCivilianUnitType(int i) const
{
	CvAssertMsg(i < GC.getNumUnitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");

	if(i > -1 && i < GC.getNumUnitInfos() && m_pbCivilianUnitType)
	{
		return m_pbCivilianUnitType[i];
	}

	return false;
}

//=====================================
// CvPromotionEntryXMLEntries
//=====================================
/// Constructor
CvPromotionXMLEntries::CvPromotionXMLEntries(void)
{

}

/// Destructor
CvPromotionXMLEntries::~CvPromotionXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of promotion entries
std::vector<CvPromotionEntry*>& CvPromotionXMLEntries::GetPromotionEntries()
{
	return m_paPromotionEntries;
}

/// Number of defined projects
int CvPromotionXMLEntries::GetNumPromotions()
{
	return m_paPromotionEntries.size();
}

/// Get a specific entry
CvPromotionEntry* CvPromotionXMLEntries::GetEntry(int index)
{
	return m_paPromotionEntries[index];
}

/// Clear promotion entries
void CvPromotionXMLEntries::DeleteArray()
{
	for(std::vector<CvPromotionEntry*>::iterator it = m_paPromotionEntries.begin(); it != m_paPromotionEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paPromotionEntries.clear();
}

//=====================================
// CvUnitPromotions
//=====================================
/// Constructor
CvUnitPromotions::CvUnitPromotions():
	m_pPromotions(NULL),
	m_pUnit(NULL)
{
}

/// Destructor
CvUnitPromotions::~CvUnitPromotions(void)
{
}

/// Initialize
void CvUnitPromotions::Init(CvPromotionXMLEntries* pPromotions, CvUnit* pUnit)
{
	CvAssert(pPromotions);
	if(!pPromotions)
		return;

	CvAssert(pUnit);
	if(!pUnit)
		return;

	m_pPromotions = pPromotions;
	m_pUnit = pUnit;

	Reset();
}

/// Deallocate memory created in initialize
void CvUnitPromotions::Uninit()
{
	m_kHasPromotion.SetSize(0);
}

/// Reset unit promotion array to all false
void CvUnitPromotions::Reset()
{
	m_kHasPromotion.SetSize(0);
}

/// Serialization read
void CvUnitPromotions::Read(FDataStream& kStream)
{
	Reset();

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	// Read number of promotions
	int iNumPromotions;
	if(uiVersion >= 2)
	{
		kStream >> iNumPromotions;
	}
	else
	{
		iNumPromotions = 142;   // Get old saves to work however we can
		m_kHasPromotion.SetBit(142, false);
	}
	CvAssertMsg(m_pPromotions != NULL && m_pPromotions->GetNumPromotions() > 0, "Number of promotions to serialize is expected to greater than 0");

	if(uiVersion >= 4)
	{
		PromotionArrayHelpers::Read(kStream, m_kHasPromotion);
	}
	else
	if(uiVersion == 3)
	{
		PromotionArrayHelpers::ReadV3(kStream, m_kHasPromotion);
	}
	else
	{
		bool abHasPromotion[143];
		ArrayWrapper<bool> wrapm_abHasPromotion(143, (bool*)(&abHasPromotion[ 0 ]));
		kStream >> wrapm_abHasPromotion;
		for (int i = 0; i < 143; ++i)
		{
			if (abHasPromotion[i])
				m_kHasPromotion.SetBit(i, true);
		}
	}
}

/// Serialization write
void CvUnitPromotions::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 4;
	kStream << uiVersion;

	// Write out number of promotions to save
	int iNumPromotions = m_pPromotions->GetNumPromotions();
	kStream << iNumPromotions;
	CvAssertMsg(iNumPromotions > 0, "Number of promotions to serialize is expected to greater than 0");

	PromotionArrayHelpers::Write(kStream, m_kHasPromotion, iNumPromotions);
}

/// Accessor: Unit object
CvUnit* CvUnitPromotions::GetUnit()
{
	return m_pUnit;
}

/// Accessor: Does the unit have a certain promotion
bool CvUnitPromotions::HasPromotion(PromotionTypes eIndex) const
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		return m_kHasPromotion.GetBit(eIndex);
	}

	return false;
}

/// Sets the promotion to a certain value
void CvUnitPromotions::SetPromotion(PromotionTypes eIndex, bool bValue)
{
	CvAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	CvAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(eIndex >= 0 && eIndex < GC.getNumPromotionInfos())
	{
		m_kHasPromotion.SetBit(eIndex, bValue);
	}
}

/// determines if the terrain feature is passable given the unit's current promotions
bool CvUnitPromotions::GetAllowFeaturePassable(FeatureTypes eFeatureType) const
{
	CvTeamTechs* teamTechs = GET_TEAM(m_pUnit->getTeam()).GetTeamTechs();
	CvAssert(teamTechs);
	if(!teamTechs) return false;

	int iNumPromos = GC.getNumPromotionInfos();
	for(int iLoop = 0; iLoop < iNumPromos; iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes) iLoop;
		if(m_kHasPromotion.GetBit(ePromotion))
		{
			CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
			if(promotion)
			{
				TechTypes eTech = (TechTypes) promotion->GetFeaturePassableTech(eFeatureType);
				if(eTech != NO_TECH && teamTechs->HasTech(eTech))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// determines if the terrain type is passable given the unit's current promotions
bool CvUnitPromotions::GetAllowTerrainPassable(TerrainTypes eTerrainType) const
{
	CvTeamTechs* teamTechs = GET_TEAM(m_pUnit->getTeam()).GetTeamTechs();
	CvAssert(teamTechs);
	if(!teamTechs) return false;

	int iNumPromos = GC.getNumPromotionInfos();
	for(int iLoop = 0; iLoop < iNumPromos; iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes) iLoop;
		if(m_kHasPromotion.GetBit(ePromotion))
		{
			CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);

			if(promotion)
			{
				TechTypes eTech = (TechTypes) promotion->GetTerrainPassableTech(eTerrainType);
				if(eTech != NO_TECH && teamTechs->HasTech(eTech))
				{
					return true;
				}
			}
		}
	}
	return false;
}

/// returns the advantage percent when attacking the specified unit class
int CvUnitPromotions::GetUnitClassAttackMod(UnitClassTypes eUnitClass) const
{
	int iSum = 0;
	for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iLoop;
		CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
		if(promotion && HasPromotion(ePromotion))
		{
			iSum += promotion->GetUnitClassAttackModifier(eUnitClass);
		}
	}
	return iSum;
}

/// returns the advantage percent when defending against the specified unit class
int CvUnitPromotions::GetUnitClassDefenseMod(UnitClassTypes eUnitClass) const
{
	int iSum = 0;
	for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iLoop;
		CvPromotionEntry* promotion = GC.getPromotionInfo(ePromotion);
		if(promotion && HasPromotion(ePromotion))
		{
			iSum += promotion->GetUnitClassDefenseModifier(eUnitClass);
		}
	}
	return iSum;
}

// Read the saved promotions.  Entries are saved as string values, all entries are saved.
void PromotionArrayHelpers::ReadV3(FDataStream& kStream, CvBitfield& kPromotions)
{
	int iNumEntries;
	FStringFixedBuffer(sTemp, 64);
	int iType;

	kStream >> iNumEntries;

	kPromotions.SetSize( iNumEntries );

	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if(sTemp == "PROMOTION_OLIGARCHY")
		{
			bool bTemp;
			kStream >> bTemp;
		}
		else
		{
			iType = GC.getInfoTypeForString(sTemp);
			if(iType != -1)
			{
				bool bValue;
				kStream >> bValue;
				kPromotions.SetBit(iType, bValue);
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Promotion Type not found: %s", sTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bool bDummy;
				kStream >> bDummy;
			}
		}
	}
}

// Read the saved promotions.  Entries are saved as hash values
void PromotionArrayHelpers::Read(FDataStream& kStream, CvBitfield& kPromotions)
{
	int iNumEntries;
	int iType;

	kStream >> iNumEntries;

	kPromotions.SetSize( iNumEntries );

	uint uiHashTemp;
	uint uiOligarchyHash = FString::Hash("PROMOTION_OLIGARCHY");
	for(int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> uiHashTemp;
		if(uiHashTemp == uiOligarchyHash)
		{
			bool bTemp;
			kStream >> bTemp;
		}
		else
		{
			iType = GC.getInfoTypeForHash(uiHashTemp);
			if(iType != -1)
			{
				bool bValue;
				kStream >> bValue;
				kPromotions.SetBit(iType, bValue);
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Promotion Type not found for hash: %u", uiHashTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				bool bDummy;
				kStream >> bDummy;
			}
		}
	}
}

// Save the promotions.  Entries are saved as hash values and only the entries that are 'on' are saved
void PromotionArrayHelpers::Write(FDataStream& kStream, const CvBitfield& kPromotions, int iArraySize)
{
	// We are only going to save the 'on' bit, so we have to count them
	int iCount = 0;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		if (kPromotions.GetBit(ePromotion) && GC.getPromotionInfo(ePromotion) != NULL)
			++iCount;
	}

	kStream << iCount;

	for(int iI = 0; iI < iArraySize; iI++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iI);
		bool bValue = kPromotions.GetBit(ePromotion);
		if (bValue)
		{
			CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
			if(pkPromotionInfo)
			{
				uint uiHash = FString::Hash( pkPromotionInfo->GetType() );		// Save just the hash
				kStream << uiHash;
				kStream << bValue;
			}
		}
	}
}
