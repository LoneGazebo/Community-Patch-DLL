/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"

#include "LintFree.h"

/// Constructor
CvUnitEntry::CvUnitEntry(void) :
m_iProductionCost(0),
m_iHurryCostModifier(0),
m_iAdvancedStartCost(0),
m_iMinAreaSize(0),
m_iMoves(0),
m_bMoves(false),
m_iBaseSightRange(0),
m_iRange(0),
m_iAirUnitCap(0),
m_iNukeDamageLevel(0),
m_iWorkRate(0),
m_iNumFreeTechs(0),
m_iBaseHurry(0),
m_iHurryMultiplier(0),
m_bRushBuilding(false),
m_iBaseGold(0),
m_iNumGoldPerEra(0),
m_bSpreadReligion(false),
m_bIsReligious(false),
m_iCombat(0),
m_iCombatLimit(0),
m_iRangedCombat(0),
m_iRangedCombatLimit(0),
m_iXPValueAttack(0),
m_iXPValueDefense(0),
m_iSpecialCargo(0),
m_iDomainCargo(0),
m_iConscriptionValue(0),
m_iExtraMaintenanceCost(0),
m_bNoMaintenance(false),
m_iUnhappiness(0),
m_iUnitClassType(NO_UNITCLASS),
m_iSpecialUnitType(NO_SPECIALUNIT),
m_iUnitCaptureClassType(NO_UNITCLASS),
m_iUnitCombatType(NO_UNITCOMBAT),
m_iDomainType(NO_DOMAIN),
m_iCivilianAttackPriority(NO_CIVILIAN_ATTACK_PRIORITY),
m_iDefaultUnitAIType(NO_UNITAI),
m_iPrereqAndTech(NO_TECH),
m_iObsoleteTech(NO_TECH),
m_iGoodyHutUpgradeUnitClass(NO_UNITCLASS),
m_iGroupSize(0),
m_iGroupDefinitions(0),
m_iUnitMeleeWaveSize(0),
m_iUnitRangedWaveSize(0),
m_iNumUnitNames(0),
m_iCommandType(NO_COMMAND),
m_bFoodProduction(false),
m_bNoBadGoodies(false),
m_bRivalTerritory(false),
m_bMilitarySupport(false),
m_bMilitaryProduction(false),
m_bPillage(false),
m_bFound(false),
m_bFoundAbroad(false),
m_iCultureBombRadius(0),
m_iGoldenAgeTurns(0),
m_bIgnoreBuildingDefense(false),
m_bPrereqResources(false),
m_bMechanized(false),
m_bSuicide(false),
m_bCaptureWhileEmbarked(false),
m_bRangeAttackOnlyInDomain(false),
m_pbUpgradeUnitClass(NULL),
m_pbUnitAIType(NULL),
m_pbNotUnitAIType(NULL),
m_pbBuilds(NULL),
m_pbGreatPeoples(NULL),
m_pbBuildings(NULL),
m_pbBuildingClassRequireds(NULL),
m_piPrereqAndTechs(NULL),
m_piResourceQuantityRequirements(NULL),
m_piProductionTraits(NULL),
m_piFlavorValue(NULL),
m_piUnitGroupRequired(NULL),
m_pbFreePromotions(NULL),
m_paszEarlyArtDefineTags(NULL),
m_paszLateArtDefineTags(NULL),
m_paszMiddleArtDefineTags(NULL),
m_paszUnitNames(NULL),
m_bUnitArtInfoEraVariation(false),
m_bUnitArtInfoCulturalVariation(false),
m_iUnitFlagIconOffset(0),
m_iUnitPortraitOffset(0)
{
}

/// Destructor
CvUnitEntry::~CvUnitEntry(void)
{
	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	SAFE_DELETE_ARRAY(m_pbBuildings);
	SAFE_DELETE_ARRAY(m_pbBuildingClassRequireds);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piResourceQuantityRequirements);
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
}

bool CvUnitEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iProductionCost = kResults.GetInt("Cost");
	m_iHurryCostModifier = kResults.GetInt("HurryCostModifier");
	m_iAdvancedStartCost = kResults.GetInt("AdvancedStartCost");
	m_iMinAreaSize = kResults.GetInt("MinAreaSize");
	m_iMoves = kResults.GetInt("Moves");
	m_bMoves = kResults.GetInt("Immobile");
	m_iBaseSightRange = kResults.GetInt("BaseSightRange");
	m_iRange = kResults.GetInt("Range");
	m_iAirUnitCap = kResults.GetInt("AirUnitCap");
	m_iNukeDamageLevel = kResults.GetInt("NukeDamageLevel");
	m_iWorkRate = kResults.GetInt("WorkRate");
	m_iNumFreeTechs = kResults.GetInt("NumFreeTechs");
	m_iBaseHurry = kResults.GetInt("BaseHurry");
	m_iHurryMultiplier = kResults.GetInt("HurryMultiplier");
	m_bRushBuilding= kResults.GetInt("RushBuilding");
	m_iBaseGold = kResults.GetInt("BaseGold");
	m_iNumGoldPerEra = kResults.GetInt("NumGoldPerEra");
	m_bSpreadReligion = kResults.GetInt("SpreadReligion");
	m_bIsReligious = kResults.GetInt("IsReligious");
	m_iCombat = kResults.GetInt("Combat");
	m_iCombatLimit = kResults.GetInt("CombatLimit");
	m_iRangedCombat = kResults.GetInt("RangedCombat");
	m_iRangedCombatLimit = kResults.GetInt("RangedCombatLimit");
	m_iXPValueAttack = kResults.GetInt("XPValueAttack");
	m_iXPValueDefense = kResults.GetInt("XPValueDefense");
	m_iConscriptionValue = kResults.GetInt("Conscription");
	m_iExtraMaintenanceCost = kResults.GetInt("ExtraMaintenanceCost");
	m_bNoMaintenance = kResults.GetBool("NoMaintenance");
	m_iUnhappiness = kResults.GetInt("Unhappiness");
	m_iUnitFlagIconOffset = kResults.GetInt("UnitFlagIconOffset");
	m_iUnitPortraitOffset = kResults.GetInt("PortraitIndex");
	m_iLeaderExperience = kResults.GetInt("LeaderExperience");
	m_bFoodProduction = kResults.GetBool("Food");
	m_bNoBadGoodies = kResults.GetBool("NoBadGoodies");
	m_bRivalTerritory = kResults.GetBool("RivalTerritory");
	m_bMilitarySupport = kResults.GetBool("MilitarySupport");
	m_bMilitaryProduction = kResults.GetBool("MilitaryProduction");
	m_bPillage = kResults.GetBool("Pillage");
	m_bFound = kResults.GetBool("Found");
	m_bFoundAbroad = kResults.GetBool("FoundAbroad");
	m_iCultureBombRadius = kResults.GetInt("CultureBombRadius");
	m_iGoldenAgeTurns = kResults.GetInt("GoldenAgeTurns");
	m_bIgnoreBuildingDefense = kResults.GetBool("IgnoreBuildingDefense");
	m_bPrereqResources = kResults.GetBool("PrereqResources");
	m_bMechanized = kResults.GetBool("Mechanized");
	m_bSuicide = kResults.GetBool("Suicide");
	m_bCaptureWhileEmbarked = kResults.GetBool("CaptureWhileEmbarked");
	m_bRangeAttackOnlyInDomain = kResults.GetBool("RangeAttackOnlyInDomain");

	m_strUnitArtInfoTag = kResults.GetText("UnitArtInfo");
	m_bUnitArtInfoCulturalVariation = kResults.GetBool("UnitArtInfoCulturalVariation");
	m_bUnitArtInfoEraVariation = kResults.GetBool("UnitArtInfoEraVariation");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("Class");
	m_iUnitClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Special");
	m_iSpecialUnitType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Capture");
	m_iUnitCaptureClassType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CombatClass");
	m_iUnitCombatType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("Domain");
	m_iDomainType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("CivilianAttackPriority");
	m_iCivilianAttackPriority = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DefaultUnitAI");
	m_iDefaultUnitAIType = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("PrereqTech");
	m_iPrereqAndTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ObsoleteTech");
	m_iObsoleteTech = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("GoodyHutUpgradeUnitClass");
	m_iGoodyHutUpgradeUnitClass = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpecialCargo");
	m_iSpecialCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("DomainCargo");
	m_iDomainCargo = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("ProjectPrereq");
	m_iProjectPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("SpaceshipProject");
	m_iSpaceshipProject = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("LeaderPromotion");
	m_iLeaderPromotion = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szUnitType = GetType();

	kUtility.SetFlavors(m_piFlavorValue, "Unit_Flavors", "UnitType", szUnitType);

	kUtility.PopulateArrayByValue(m_piProductionTraits, "Traits", "Unit_ProductionTraits", "TraitType", "UnitType", szUnitType, "Trait");

	kUtility.PopulateArrayByValue(m_piResourceQuantityRequirements, "Resources", "Unit_ResourceQuantityRequirements", "ResourceType", "UnitType", szUnitType, "Cost");
	kUtility.PopulateArrayByValue(m_piProductionModifierBuildings, "Buildings", "Unit_ProductionModifierBuildings", "BuildingType", "UnitType", szUnitType, "ProductionModifier");
	kUtility.PopulateArrayByExistence(m_pbFreePromotions, "UnitPromotions", "Unit_FreePromotions", "PromotionType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUpgradeUnitClass, "UnitClasses", "Unit_ClassUpgrades", "UnitClassType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbUnitAIType, "UnitAIInfos", "Unit_AITypes", "UnitAIType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbNotUnitAIType, "UnitAIInfos", "Unit_NotAITypes", "UnitAIType", "UnitType", szUnitType);

	kUtility.PopulateArrayByExistence(m_pbBuilds, "Builds", "Unit_Builds", "BuildType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbGreatPeoples, "Specialists", "Unit_GreatPersons", "GreatPersonType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildings, "Buildings", "Unit_Buildings", "BuildingType", "UnitType", szUnitType);
	kUtility.PopulateArrayByExistence(m_pbBuildingClassRequireds, "BuildingClasses", "Unit_BuildingClassRequireds", "BuildingClassType", "UnitType", szUnitType);

	//TechTypes
	{
		//Initialize array to NO_TECH
		kUtility.InitializeArray(m_piPrereqAndTechs, "Technologies", (int)NO_TECH);

		std::string strKey = "Units - TechTypes";
		Database::Results* pResults = kUtility.GetResults(strKey);
		if(pResults == NULL)
		{
			pResults = kUtility.PrepareResults(strKey, "select Technologies.ID from Unit_TechTypes inner join Technologies on TechType = Technologies.Type where UnitType = ?");
		}

		pResults->Bind(1, szUnitType, -1, false);

		int i = 0;
		while(pResults->Step())
		{
			m_piPrereqAndTechs[i++] = pResults->GetInt(0);
		}

		pResults->Reset();
	}

	//Unit Unique Names Count
	{
		std::string strKey = "Units - UniqueNameCount";
		Database::Results* pUnitNameCount = kUtility.GetResults(strKey);
		if(pUnitNameCount == NULL)
		{
			pUnitNameCount = kUtility.PrepareResults(strKey, "select count(*) from Unit_UniqueNames where UnitType = ?");
		}

		pUnitNameCount->Bind(1, szUnitType, -1, false);

		pUnitNameCount->Step();
		m_iNumUnitNames = pUnitNameCount->GetInt(0);

		pUnitNameCount->Reset();
	}

	//Unit Unique Names
	{
		if(m_iNumUnitNames > 0)
		{
			m_paszUnitNames = FNEW(CvString[m_iNumUnitNames], c_eCiv5GameplayDLL, 0);

			std::string strKey = "Units - UniqueNames";
			Database::Results* pResults = kUtility.GetResults(strKey);
			if(pResults == NULL)
			{
				pResults = kUtility.PrepareResults(strKey, "select UniqueName from Unit_UniqueNames where UnitType = ? ORDER BY rowid");
			}

			pResults->Bind(1, szUnitType, -1, false);

			int i = 0;
			while(pResults->Step())
			{
				m_paszUnitNames[i++] = pResults->GetText(0);
			}

			pResults->Reset();
		}
	}

	//MovementRates
	{
		std::string strKey = "Units - MovementRates";
		Database::Results* pMovementRates = kUtility.GetResults(strKey);
		if(pMovementRates == NULL)
		{
			pMovementRates = kUtility.PrepareResults(strKey, "SELECT * FROM MovementRates where Type = ? ORDER BY NumHexes");
		}

		const char* szMovementRate = kResults.GetText("MoveRate");
		pMovementRates->Bind(1, szMovementRate, -1, false);
		while (pMovementRates->Step())
		{
			int f0 = pMovementRates->GetInt("NumHexes") - 1;
			float f1 = pMovementRates->GetFloat("TotalTime");
			float f2 = pMovementRates->GetFloat("EaseIn");
			float f3 = pMovementRates->GetFloat("EaseOut");
			float f4 = pMovementRates->GetFloat("IndividualOffset");
			float f5 = pMovementRates->GetFloat("RowOffset");
			float f6 = pMovementRates->GetFloat("CurveRoll");
			int   iPathSubdivision = pMovementRates->GetInt("PathSubdivision");
			if (f0 >= 0 && f0 <= 11)
			{
				m_unitMoveRate[f0].m_fTotalMoveRate    = f1;
				m_unitMoveRate[f0].m_fEaseIn           = f2;
				m_unitMoveRate[f0].m_fEaseOut          = f3;
				m_unitMoveRate[f0].m_fIndividualOffset = f4;
				m_unitMoveRate[f0].m_fRowOffset        = f5;
				m_unitMoveRate[f0].m_fCurveRoll        = f6;
				m_unitMoveRate[f0].m_iPathSubdivision  = iPathSubdivision;
			}
		}
		pMovementRates->Reset();

	}

	// Calculate military Power and cache it
	DoUpdatePower();

	return true;
}

/// Shields to construct the unit
int CvUnitEntry::GetProductionCost() const
{
	return m_iProductionCost;
}

/// Does it cost extra to hurry this init?
int CvUnitEntry::GetHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

/// Cost if starting midway through game
int CvUnitEntry::GetAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

/// Required Plot count of the CvArea this City belongs to (Usually used for Water Units to prevent building them on tiny lakes and such)
int CvUnitEntry::GetMinAreaSize() const
{
	return m_iMinAreaSize;
}

/// Movement points
int CvUnitEntry::GetMoves() const
{
	return m_iMoves;
}

/// Can this Unit move on its own?
bool CvUnitEntry::IsImmobile() const
{
	return m_bMoves;
}

/// Sight range on flat terrain
int CvUnitEntry::GetBaseSightRange() const
{
	return m_iBaseSightRange;
}

/// Air mission range
int CvUnitEntry::GetRange() const
{
	return m_iRange;
}

/// How many air units does this count as?
int CvUnitEntry::GetAirUnitCap() const
{
	return m_iAirUnitCap;
}

/// If non-zero this is a nuclear weapon with the specified attack range
int CvUnitEntry::GetNukeDamageLevel() const
{
	return m_iNukeDamageLevel;
}

/// Amount of work performed in a turn
int CvUnitEntry::GetWorkRate() const
{
	return m_iWorkRate;
}

/// How many free Techs does this Unit give us?
int CvUnitEntry::GetNumFreeTechs() const
{
	return m_iNumFreeTechs;
}

/// What is the base amount of production provided by this unit?
int CvUnitEntry::GetBaseHurry() const
{
	return m_iBaseHurry;
}

/// Additional production * city population size
int CvUnitEntry::GetHurryMultiplier() const
{
	return m_iHurryMultiplier;
}

/// Ability to complete Building immediately (should replace the next 2 entries eventually)
bool CvUnitEntry::IsRushBuilding() const
{
	return m_bRushBuilding;
}

/// Base boost to gold (for great people)
int CvUnitEntry::GetBaseGold() const
{
	return m_iBaseGold;
}

/// Era boost to gold (for great people)
int CvUnitEntry::GetNumGoldPerEra() const
{
	return m_iNumGoldPerEra;
}

/// Can this Unit Spread Religion to a City?
bool CvUnitEntry::IsSpreadReligion() const
{
	return m_bSpreadReligion;
}

/// Is this a "Religous" Unit? (qualifies it for Production bonuses for Policies, etc.)
bool CvUnitEntry::IsReligious() const
{
	return m_bIsReligious;
}

/// Returns combat value
int CvUnitEntry::GetCombat() const
{
	return m_iCombat;
}

/// Sets combat value
void CvUnitEntry::SetCombat(int iNum)
{
	m_iCombat = iNum;
}

/// Maximum damage to enemy
int CvUnitEntry::GetCombatLimit() const
{
	return m_iCombatLimit;
}

/// Bombard combat value
int CvUnitEntry::GetRangedCombat() const
{
	return m_iRangedCombat;
}

/// Maximum damage to enemy in bombard
int CvUnitEntry::GetRangedCombatLimit() const
{
	return m_iRangedCombatLimit;
}

/// Experience point value when attacking
int CvUnitEntry::GetXPValueAttack() const
{
	return m_iXPValueAttack;
}

/// Experience point value when defending
int CvUnitEntry::GetXPValueDefense() const
{
	return m_iXPValueDefense;
}

/// Is there a special unit this unit carries (e.g. Nuclear Sub carries Nuclear missile)
int CvUnitEntry::GetSpecialCargo() const
{
	return m_iSpecialCargo;
}

/// Is there a class of units (domain) that this unit carries
int CvUnitEntry::GetDomainCargo() const
{
	return m_iDomainCargo;
}

/// Cost to conscript this unit
int CvUnitEntry::GetConscriptionValue() const
{
	return m_iConscriptionValue;
}

/// Extra cost for unit maintenance in Gold (deducted every turn)
int CvUnitEntry::GetExtraMaintenanceCost() const
{
	return m_iExtraMaintenanceCost;
}

/// No unit maintenance in Gold (deducted every turn)
bool CvUnitEntry::IsNoMaintenance() const
{
	return m_bNoMaintenance;
}

/// Amount of Happiness used up supporting this Unit
int CvUnitEntry::GetUnhappiness() const
{
	return m_iUnhappiness;
}

/// Class of unit
int CvUnitEntry::GetUnitClassType() const
{
	return m_iUnitClassType;
}

/// Special class of this unit (if any)
int CvUnitEntry::GetSpecialUnitType() const
{
	return m_iSpecialUnitType;
}

/// If captured, what unit does it become?
int CvUnitEntry::GetUnitCaptureClassType() const
{
	return m_iUnitCaptureClassType;
}

/// Combat type (melee, mounted, siege, air, etc.)
int CvUnitEntry::GetUnitCombatType() const
{
	return m_iUnitCombatType;
}

/// What domain does this unit operate in (land, air or sea)
int CvUnitEntry::GetDomainType() const
{
	return m_iDomainType;
}

/// If this is a civilian, what is our priority to attack it?
int CvUnitEntry::GetCivilianAttackPriority() const
{
	return m_iCivilianAttackPriority;
}

/// Default AI behavior
int CvUnitEntry::GetDefaultUnitAIType() const
{
	return m_iDefaultUnitAIType;
}

/// Prerequisite techs with AND
int CvUnitEntry::GetPrereqAndTech() const
{
	return m_iPrereqAndTech;
}

/// Tech that makes this unit obsolete
int CvUnitEntry::GetObsoleteTech() const
{
	return m_iObsoleteTech;
}

/// Unitclass that replaces this Unit if the appropriate Goody is received from a Hut
int CvUnitEntry::GetGoodyHutUpgradeUnitClass() const
{
	return m_iGoodyHutUpgradeUnitClass;
}

/// Initial number of individuals in the unit group
int CvUnitEntry::GetGroupSize() const
{
	return m_iGroupSize;
}

/// Number of UnitMeshGroups for this unit
int CvUnitEntry::GetGroupDefinitions() const
{
	return m_iGroupDefinitions;
}

/// Maximum number of attackers in one round (melee combat)
int CvUnitEntry::GetMeleeWaveSize() const
{
	return m_iUnitMeleeWaveSize;
}

/// Maximum number of attackers in one round (ranged combat)
int CvUnitEntry::GetRangedWaveSize() const
{
	return m_iUnitRangedWaveSize;
}

/// Individual names for this unit (for great people)
int CvUnitEntry::GetNumUnitNames() const
{
	return m_iNumUnitNames;
}

/// Is food used to produce this unit?
bool CvUnitEntry::IsFoodProduction() const
{
	return m_bFoodProduction;
}

/// Does this unit never encounter a bad goodie hut?
bool CvUnitEntry::IsNoBadGoodies() const
{
	return m_bNoBadGoodies;
}

/// Prohibited from enemy territory
bool CvUnitEntry::IsRivalTerritory() const
{
	return m_bRivalTerritory;
}

/// Does it require military supplies?
bool CvUnitEntry::IsMilitarySupport() const
{
	return m_bMilitarySupport;
}

/// Is this considered a military unit?
bool CvUnitEntry::IsMilitaryProduction() const
{
	return m_bMilitaryProduction;
}

/// Can this unit pillage?
bool CvUnitEntry::IsPillage() const
{
	return m_bPillage;
}

/// Can it start a city?
bool CvUnitEntry::IsFound() const
{
	return m_bFound;
}

/// Can it start a city on a continent other than one with capital?
bool CvUnitEntry::IsFoundAbroad() const
{
	return m_bFoundAbroad;
}

/// Distance this unit steals
int CvUnitEntry::GetCultureBombRadius() const
{
	return m_iCultureBombRadius;
}

/// Number of GA turns this Unit can give us
int CvUnitEntry::GetGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}

/// Not affected by walls?
bool CvUnitEntry::IsIgnoreBuildingDefense() const
{
	return m_bIgnoreBuildingDefense;
}

/// Does it open up resources (e.g. workboat)
bool CvUnitEntry::IsPrereqResources() const
{
	return m_bPrereqResources;
}

/// Mechanized unit?
bool CvUnitEntry::IsMechUnit() const
{
	return m_bMechanized;
}

/// Suicide attack unit?
bool CvUnitEntry::IsSuicide() const
{
	return m_bSuicide;
}

/// Capture this unit even if he's embarked?
bool CvUnitEntry::IsCaptureWhileEmbarked() const
{
	return m_bCaptureWhileEmbarked;
}

/// Only have a ranged attack in their own domain?
bool CvUnitEntry::IsRangeAttackOnlyInDomain() const
{
	return m_bRangeAttackOnlyInDomain;
}

/// Return unit's current command
int CvUnitEntry::GetCommandType() const
{
	return m_iCommandType;
}

/// Set unit's current command
void CvUnitEntry::SetCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

// ARRAYS

/// Prerequisite techs with AND
int CvUnitEntry::GetPrereqAndTechs(int i) const
{
	CvAssertMsg(i < GC.getNUM_UNIT_AND_TECH_PREREQS(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}

/// Resources consumed to construct
int CvUnitEntry::GetResourceQuantityRequirement(int i) const
{
	CvAssertMsg(i < GC.getNumResourceInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piResourceQuantityRequirements ? m_piResourceQuantityRequirements[i] : -1;
}

/// Production boost for having a specific building in city
int CvUnitEntry::GetBuildingProductionModifier(BuildingTypes eBuilding) const
{
	CvAssertMsg((int)eBuilding < GC.getNumBuildingInfos(), "Building type out of bounds");
	CvAssertMsg((int)eBuilding > -1, "Index out of bounds");
	return m_piProductionModifierBuildings[(int)eBuilding];
}

/// Boost in production for leader with this trait
int CvUnitEntry::GetProductionTraits(int i) const
{
	CvAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piProductionTraits ? m_piProductionTraits[i] : -1;
}

/// Find value of flavors associated with this building
int CvUnitEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvUnitEntry::GetUnitGroupRequired(int i) const
{
	CvAssertMsg(i < GetGroupDefinitions(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitGroupRequired ? m_piUnitGroupRequired[i] : NULL;
}

/// What can this unit upgrade into?
bool CvUnitEntry::GetUpgradeUnitClass(int i) const
{
	CvAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUpgradeUnitClass ? m_pbUpgradeUnitClass[i] : false;
}

/// What AIs strategies can this unit adopt
bool CvUnitEntry::GetUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbUnitAIType ? m_pbUnitAIType[i] : false;
}

/// AI strategies this unit can NOT adopt
bool CvUnitEntry::GetNotUnitAIType(int i) const
{
	CvAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbNotUnitAIType ? m_pbNotUnitAIType[i] : false;
}

/// What improvements can this unit build?
bool CvUnitEntry::GetBuilds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuilds ? m_pbBuilds[i] : false;
}

/// Type(s) of great people represented by this unit
bool CvUnitEntry::GetGreatPeoples(int i) const
{
	CvAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbGreatPeoples ? m_pbGreatPeoples[i] : false;
}

/// Is this unit required to construct a certain building?
bool CvUnitEntry::GetBuildings(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildings ? m_pbBuildings[i] : false;
}

/// Does this Unit need a certain BuildingClass in this City to train?
bool CvUnitEntry::GetBuildingClassRequireds(int i) const
{
	CvAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildingClassRequireds ? m_pbBuildingClassRequireds[i] : false;
}

/// Initial set of promotions for this unit
bool CvUnitEntry::GetFreePromotions(int i) const
{
	CvAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_pbFreePromotions ? m_pbFreePromotions[i] : false;
}

/// Project required to train this unit?
int CvUnitEntry::GetProjectPrereq() const
{
	return m_iProjectPrereq;
}

/// Does this unit build a spaceship project?
int CvUnitEntry::GetSpaceshipProject() const
{
	return m_iSpaceshipProject;
}

/// Is this a great leader (general) promotion unit?
int CvUnitEntry::GetLeaderPromotion() const
{
	return m_iLeaderPromotion;
}

/// What is the experience bonus provided by this great leader?
int CvUnitEntry::GetLeaderExperience() const
{
	return m_iLeaderExperience;
}

/// Return art tag
const char* CvUnitEntry::GetUnitArtInfoTag() const
{
	return m_strUnitArtInfoTag;
}

/// Return whether we should try to find a culture specific variant art tag
const bool CvUnitEntry::GetUnitArtInfoCulturalVariation() const
{
	return m_bUnitArtInfoCulturalVariation;
}


/// Return whether we should try to find an era specific variant art tag
const bool CvUnitEntry::GetUnitArtInfoEraVariation() const
{
	return m_bUnitArtInfoEraVariation;
}


/// Unique names for individual units (for great people)
const char* CvUnitEntry::GetUnitNames(int i) const
{
	CvAssertMsg(i < GetNumUnitNames(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
}


/// What flag icon to use
int CvUnitEntry::GetUnitFlagIconOffset() const
{
	return m_iUnitFlagIconOffset;
}


/// What portrait to use
int CvUnitEntry::GetUnitPortraitOffset() const
{
	return m_iUnitPortraitOffset;
}


// DERIVED FIELDS

/// Total cargo space from all free promotions
int CvUnitEntry::GetCargoSpace() const
{
	int rtnValue = 0;

	for (int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
	{
		const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
		if(pkPromotionInfo)
		{
			if (GetFreePromotions(iLoop))
			{
				const int iVal = pkPromotionInfo->GetCargoChange();
				rtnValue += iVal;
			}
		}
	}

	return rtnValue;
}

/// Military might or "power" - returns cache
int CvUnitEntry::GetPower() const
{
	return m_iCachedPower;
}

/// Update military Power
void CvUnitEntry::DoUpdatePower()
{
	int iPower;

// ***************
// Main Factors - Strength & Moves
// ***************

	// We want a Unit that has twice the strength to be roughly worth 3x as much with regards to Power
	iPower = int(pow((double) GetCombat(), 1.5));

	// Ranged Strength
	int iRangedStrength = int(pow((double) GetRangedCombat(), 1.45));

	// Naval ranged attacks are less useful
	if (GetDomainType() == DOMAIN_SEA)
	{
		iRangedStrength /= 2;
	}

	if (iRangedStrength > 0)
	{
		iPower = iRangedStrength;
	}

	// We want Movement rate to be important, but not a dominating factor; a Unit with double the moves of a similarly-strengthed Unit should be ~1.5x as Powerful
	iPower = int((float) iPower * pow((double) GetMoves(), 0.3));

// ***************
// Other modifiers
// ***************

	// Suicide Units are obviously less useful
	if (IsSuicide())
	{
		iPower /= 2;
	}

	// Nukes are cool
	if (GetNukeDamageLevel() > 0)
	{
		iPower += 4000;
	}

// ***************
// Promotion modifiers
// ***************

	int iTemp;
	int iLoop;

	for (int iPromotionLoop = 0; iPromotionLoop < GC.getNumPromotionInfos(); iPromotionLoop++)
	{
		CvPromotionEntry* kPromotion = GC.getPromotionInfo((PromotionTypes)iPromotionLoop);
		if(kPromotion == NULL)
			continue;

		if (GetFreePromotions(iPromotionLoop))
		{
			// City Attack - add half of the bonus
			if (kPromotion->GetCityAttackPercent() > 0)
			{
				iTemp = (iPower * kPromotion->GetCityAttackPercent() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Attack - add half of the bonus
			if (kPromotion->GetAttackMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetAttackMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Defense - add half of the bonus
			if (kPromotion->GetDefenseMod() > 0)
			{
				iTemp = (iPower * kPromotion->GetDefenseMod() / 2);
				iTemp /= 100;
				iPower += iTemp;
			}

			// Paradrop - add 25%
			if (kPromotion->GetDropRange() > 0)
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower += iTemp;
			}

			// Blitz - add 20%
			if (kPromotion->IsBlitz())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower += iTemp;
			}

			// Set Up For Ranged Attack - reduce by 20%
			if (kPromotion->IsMustSetUpToRangedAttack())
			{
				iTemp = iPower;
				iTemp /= 5;
				iPower -= iTemp;
			}

			// Only Defensive - reduce  by 25%, but only if the Unit has no ranged capability
			if (kPromotion->IsOnlyDefensive() && GetRangedCombat() == 0 )
			{
				iTemp = iPower;
				iTemp /= 4;
				iPower -= iTemp;
			}

			for (iLoop = 0; iLoop < GC.getNumTerrainInfos(); iLoop++)
			{
				// Terrain Attack - add one quarter of the bonus
				if (kPromotion->GetTerrainAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Terrain Defense - add one quarter of the bonus
				if (kPromotion->GetTerrainDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetTerrainDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for (iLoop = 0; iLoop < GC.getNumFeatureInfos(); iLoop++)
			{
				// Feature Attack - add one quarter of the bonus
				if (kPromotion->GetFeatureAttackPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureAttackPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Feature Defense - add one quarter of the bonus
				if (kPromotion->GetFeatureDefensePercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetFeatureDefensePercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for (iLoop = 0; iLoop < GC.getNumUnitCombatClassInfos(); iLoop++)
			{
				// Unit Combat Class (e.g. Pikemen) - add one quarter of the bonus
				if (kPromotion->GetUnitCombatModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitCombatModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for (iLoop = 0; iLoop < GC.getNumUnitClassInfos(); iLoop++)
			{
				// Unit Class (e.g. bonus ONLY against Galleys) - add one eighth of the bonus
				// We're assuming here that the bonus against the other Unit is at least going to be somewhat useful - trust the XML! :o
				if (kPromotion->GetUnitClassModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassModifierPercent(iLoop) / 8);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Unit Class Attack - one tenth of the bonus
				if (kPromotion->GetUnitClassAttackModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassAttackModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}
				// Unit Class Defense - one tenth of the bonus
				if (kPromotion->GetUnitClassDefenseModifier(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetUnitClassDefenseModifier(iLoop) / 10);
					iTemp /= 100;
					iPower += iTemp;
				}
			}

			for (iLoop = 0; iLoop < NUM_DOMAIN_TYPES; iLoop++)
			{
				// Domain - add one quarter of the bonus
				if (kPromotion->GetDomainModifierPercent(iLoop) > 0)
				{
					iTemp = (iPower * kPromotion->GetDomainModifierPercent(iLoop) / 4);
					iTemp /= 100;
					iPower += iTemp;
				}
			}
		}
	}

	// Debug output
	//char temp[256];
	//sprintf(temp, "%s: %i\n", GetDescription(), iPower);
	//OutputDebugString(temp);

	m_iCachedPower = iPower;
}

UnitMoveRate CvUnitEntry::GetMoveRate(int numHexes) const
{
	if (numHexes < 0)
		numHexes = 0;
	if (numHexes > 11)
		numHexes = 11;
	return m_unitMoveRate[numHexes];
}

//=====================================
// CvUnitXMLEntries
//=====================================
/// Constructor
CvUnitXMLEntries::CvUnitXMLEntries(void)
{

}

/// Destructor
CvUnitXMLEntries::~CvUnitXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of policy entries
std::vector<CvUnitEntry*>& CvUnitXMLEntries::GetUnitEntries()
{
	return m_paUnitEntries;
}

/// Number of defined policies
int CvUnitXMLEntries::GetNumUnits()
{
	return m_paUnitEntries.size();
}


/// Clear policy entries
void CvUnitXMLEntries::DeleteArray()
{
	for (std::vector<CvUnitEntry*>::iterator it = m_paUnitEntries.begin(); it != m_paUnitEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paUnitEntries.clear();
}

/// Get a specific entry
CvUnitEntry *CvUnitXMLEntries::GetEntry(int index)
{
	return m_paUnitEntries[index];
}

/// Helper function to read in an integer array of data sized according to number of unit types
void UnitArrayHelpers::Read(FDataStream &kStream, int *paiUnitArray)
{
	int iNumEntries;
	FStringFixedBuffer(sTemp, 64);
	int iType;

	kStream >> iNumEntries;

	for (int iI = 0; iI < iNumEntries; iI++)
	{
		kStream >> sTemp;
		if (sTemp != "NO_UNIT")
		{
			iType = GC.getInfoTypeForString(sTemp);
			if (iType != -1)
			{
				kStream >> paiUnitArray[iType];
			}
			else
			{
				CvString szError;
				szError.Format("LOAD ERROR: Unit Type not found: %s", sTemp);
				GC.LogMessage(szError.GetCString());
				CvAssertMsg(false, szError);
				int iDummy;
				kStream >> iDummy;	// Skip it.
			}
		}
	}
}

/// Helper function to write out an integer array of data sized according to number of unit types
void UnitArrayHelpers::Write(FDataStream &kStream, int *paiUnitArray, int iArraySize)
{
	FStringFixedBuffer(sTemp, 64);

	kStream << iArraySize;

	for (int iI = 0; iI < iArraySize; iI++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if(pkUnitInfo)
		{
			sTemp = pkUnitInfo->GetType();
			kStream << sTemp;
			kStream << paiUnitArray[iI];
		}
		else
		{
			sTemp = "NO_UNIT";
			kStream << sTemp;
		}
	}
}
