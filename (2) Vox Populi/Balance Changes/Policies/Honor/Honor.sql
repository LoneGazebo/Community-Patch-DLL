-- Shift Policies Around

UPDATE Policies
SET
	GridX = 4, GridY = 1
WHERE Type = 'POLICY_MILITARY_TRADITION';

UPDATE Policies
SET
	GridX = 2, GridY = 1
WHERE Type = 'POLICY_DISCIPLINE';

UPDATE Policies
SET
	GridX = 2, GridY = 2
WHERE Type = 'POLICY_WARRIOR_CODE';

DELETE FROM Policy_PrereqPolicies
WHERE PrereqPolicy IN 
('POLICY_MILITARY_TRADITION',
 'POLICY_WARRIOR_CODE',
 'POLICY_DISCIPLINE');

INSERT INTO Policy_PrereqPolicies
	(PolicyType, PrereqPolicy)
VALUES
	('POLICY_WARRIOR_CODE', 'POLICY_DISCIPLINE'),
	('POLICY_MILITARY_CASTE', 'POLICY_MILITARY_TRADITION');


-- Opener (now Authority)

UPDATE Policies
SET
	CultureFromBarbarianKills = 0,
	CultureFromKills = 100,
	BarbarianCombatBonus = 25
WHERE Type = 'POLICY_HONOR';


-- Warrior Code (Now Tribute)

DELETE FROM Policy_UnitCombatProductionModifiers
WHERE PolicyType = 'POLICY_WARRIOR_CODE';

DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_WARRIOR_CODE';

UPDATE Policies
SET
	GreatGeneralRateModifier = 0,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_WARRIOR_CODE';

INSERT INTO Policy_FreeUnitClasses
	(PolicyType, UnitClassType, Count)
VALUES
	('POLICY_WARRIOR_CODE', 'UNITCLASS_SETTLER', 1);


-- Discipline (Now Imperium)
DELETE FROM Policy_FreePromotions
WHERE PolicyType = 'POLICY_DISCIPLINE';

UPDATE Policies
SET
	PortraitIndex = 23
WHERE Type = 'POLICY_DISCIPLINE';

INSERT INTO Policy_ConquerorYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_CULTURE', 40),
	('POLICY_DISCIPLINE', 'YIELD_SCIENCE', 40);

INSERT INTO Policy_FounderYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_CULTURE', 40),
	('POLICY_DISCIPLINE', 'YIELD_SCIENCE', 40);

-- Military Tradition (Now Dominance)

UPDATE Policies
SET
	ExpModifier = 0,
	ExtraSupplyPerPopulation = 10
WHERE Type = 'POLICY_MILITARY_TRADITION';

INSERT INTO Policy_YieldFromKills
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_MILITARY_TRADITION', 'YIELD_SCIENCE', 100);


-- Military Caste (Now Militarism)

UPDATE Policies
SET
	CulturePerGarrisonedUnit = 0,
	HappinessPerGarrisonedUnit = 0,
	UnitGoldMaintenanceMod = -15,
	RouteGoldMaintenanceMod = -50,
	PortraitIndex = 22
WHERE Type = 'POLICY_MILITARY_CASTE';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_MILITARY_CASTE', 'BUILDINGCLASS_BARRACKS', 1);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_MILITARY_CASTE', 'BUILDINGCLASS_BARRACKS', 'YIELD_CULTURE', 2);


-- Professional Army (Now Honor)

DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_PROFESSIONAL_ARMY';

UPDATE Policies
SET
	UnitUpgradeCostMod = 0,
	WarWearinessModifier = 25,
	XPopulationConscription = 10,
	PortraitIndex = 20
WHERE Type = 'POLICY_PROFESSIONAL_ARMY';

UPDATE Policy_BuildingClassProductionModifiers
SET
	ProductionModifier = 0
WHERE PolicyType = 'POLICY_PROFESSIONAL_ARMY';


-- Finisher

UPDATE Policies
SET
	GoldFromKills = 0
WHERE Type = 'POLICY_HONOR_FINISHER';

-- Mercenary Army 
-- Free Company and Mercenaries are added in NewUnits.sql (strength, cost, promotions, etc)
-- Foreign Legion: Cost and Combat are balanced in UnitCosts.sql and UnitCombat.sql; Unit-Class Promotions added in MeleeCombat.sql)
UPDATE Units SET
	Class							= 'UNITCLASS_FOREIGNLEGION',
	PrereqTech						= 'TECH_RIFLING',
	ObsoleteTech					= 'TECH_PENICILIN',
	DefaultUnitAI					= 'UNITAI_ATTACK',
	PolicyType 					 	= 'POLICY_HONOR_FINISHER',
	PurchaseOnly 					= 1,
	RequiresFaithPurchaseEnabled	= 0,
	FaithCost 						= 0,
	MoveAfterPurchase 				= 1,
	GoodyHutUpgradeUnitClass = 'UNITCLASS_GREAT_WAR_INFANTRY'
WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION';

UPDATE Unit_ClassUpgrades
SET
	UnitClassType = 'UNITCLASS_GREAT_WAR_INFANTRY'
WHERE UnitType = 'UNIT_FRENCH_FOREIGNLEGION';



----------------------
-- Combined Insertions
----------------------

-- Scaler

INSERT INTO Policy_CityYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_HONOR', 'YIELD_PRODUCTION', 1),
	('POLICY_WARRIOR_CODE', 'YIELD_PRODUCTION', 1),
	('POLICY_MILITARY_TRADITION', 'YIELD_PRODUCTION', 1),
	('POLICY_PROFESSIONAL_ARMY', 'YIELD_PRODUCTION', 1),
	('POLICY_DISCIPLINE', 'YIELD_PRODUCTION', 1),
	('POLICY_MILITARY_CASTE', 'YIELD_PRODUCTION', 1);



INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_MILITARY_TRADITION', 'PROMOTION_KILL_HEAL'),
	('POLICY_PROFESSIONAL_ARMY', 'PROMOTION_HONOR_BONUS');

INSERT INTO Policy_YieldFromBorderGrowth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_GOLD', 20),
	('POLICY_DISCIPLINE', 'YIELD_PRODUCTION', 20),
	('POLICY_HONOR_FINISHER', 'YIELD_GOLD', 20),
	('POLICY_HONOR_FINISHER', 'YIELD_PRODUCTION', 20);

INSERT INTO Policy_YieldFromMinorDemand
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DISCIPLINE', 'YIELD_CULTURE', 25),
	('POLICY_HONOR_FINISHER', 'YIELD_CULTURE', 25);

-- Promotions

INSERT INTO UnitPromotions
	(Type, Description, Help, Sound, CannotBeChosen, CombatPercent, PortraitIndex, IconAtlas, PediaType, PediaEntry, HPHealedIfDestroyEnemy, HealIfDestroyExcludesBarbarians)
VALUES
	('PROMOTION_HONOR_BONUS', 'TXT_KEY_PROMOTION_HONOR_BONUS', 'TXT_KEY_PROMOTION_HONOR_BONUS_HELP', 'AS2D_IF_LEVELUP', 1, 10, 59, 'ABILITY_ATLAS', 'PEDIA_ATTRIBUTES', 'TXT_KEY_PROMOTION_HONOR_BONUS', 0, 0),
	('PROMOTION_KILL_HEAL', 'TXT_KEY_PROMOTION_KILL_HEAL', 'TXT_KEY_PROMOTION_KILL_HEAL_HELP', 'AS2D_IF_LEVELUP', 1, 0, 59, 'ABILITY_ATLAS', 'PEDIA_HEAL', 'TXT_KEY_PROMOTION_KILL_HEAL', 15, 0);

INSERT INTO UnitPromotions_UnitCombats
	(PromotionType, UnitCombatType)
VALUES
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_RECON'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_MOUNTED'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_ARCHER'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_MELEE'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_GUN'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_ARMOR'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_HELICOPTER'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_NAVALMELEE'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_NAVALRANGED'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_CARRIER'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_FIGHTER'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_BOMBER'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_SIEGE'),
	('PROMOTION_HONOR_BONUS', 'UNITCOMBAT_SUBMARINE'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_RECON'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_MOUNTED'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_MELEE'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_GUN'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_ARMOR'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_HELICOPTER'),
	('PROMOTION_KILL_HEAL', 'UNITCOMBAT_NAVALMELEE');