-- Elite Forces
UPDATE Policies
SET
	WoundedUnitDamageMod = 0,
	FreeExperience = 15,
	ExpModifier = 50
WHERE Type = 'POLICY_ELITE_FORCES';

-- Mobilization (now Military-Industrial Complex)
INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_MOBILIZATION', 'BUILDINGCLASS_WALLS', 'YIELD_SCIENCE', 3),
	('POLICY_MOBILIZATION', 'BUILDINGCLASS_CASTLE', 'YIELD_SCIENCE', 3),
	('POLICY_MOBILIZATION', 'BUILDINGCLASS_FORTRESS', 'YIELD_SCIENCE', 3),
	('POLICY_MOBILIZATION', 'BUILDINGCLASS_ARSENAL', 'YIELD_SCIENCE', 3),
	('POLICY_MOBILIZATION', 'BUILDINGCLASS_MILITARY_BASE', 'YIELD_SCIENCE', 3);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
SELECT
	'POLICY_MOBILIZATION', Type, 'YIELD_SCIENCE', 3
FROM Improvements
WHERE SpecificCivRequired = 1 OR Type IN (
	'IMPROVEMENT_CITADEL',
	'IMPROVEMENT_FORT',
	-- hidden unique improvements
	'IMPROVEMENT_POLDER_WATER',
	'IMPROVEMENT_MONGOLIA_ORDO'
);

-- United Front
UPDATE Policies
SET
	ExtraSupplyPerPopulation = 25,
	MinimumAllyInfluenceIncreaseAtWar = 100,
	CityStateUnitFrequencyModifier = 300
WHERE Type = 'POLICY_UNITED_FRONT';

-- Futurism
DELETE FROM Policy_TourismOnUnitCreation
WHERE PolicyType = 'POLICY_FUTURISM';

UPDATE Policies
SET EventTourism = 3
WHERE Type = 'POLICY_FUTURISM';

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FUTURISM', 'YIELD_CULTURE', 2);

INSERT INTO Policy_ConquerorYield
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_FUTURISM', 'YIELD_TOURISM', 50);

-- Industrial Espionage (now Lebensraum)
UPDATE Policies
SET
	StealTechFasterModifier = 0,
	CultureBombBoost = 1
WHERE Type = 'POLICY_INDUSTRIAL_ESPIONAGE';

INSERT INTO Policy_YieldFromBorderGrowth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_INDUSTRIAL_ESPIONAGE', 'YIELD_CULTURE', 10),
	('POLICY_INDUSTRIAL_ESPIONAGE', 'YIELD_GOLDEN_AGE_POINTS', 10);

-- Fortified Borders (now New World Order)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_FORTIFIED_BORDERS';

UPDATE Policies
SET DistressFlatReductionGlobal = 2
WHERE Type = 'POLICY_FORTIFIED_BORDERS';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_CONSTABLE', 'YIELD_PRODUCTION', 5),
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_CONSTABLE', 'YIELD_CULTURE', 3),
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_POLICE_STATION', 'YIELD_PRODUCTION', 5),
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_POLICE_STATION', 'YIELD_CULTURE', 3);

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_CONSTABLE', 100),
	('POLICY_FORTIFIED_BORDERS', 'BUILDINGCLASS_POLICE_STATION', 100);

-- Universal Healthcare (now Iron Fist)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_UNIVERSAL_HEALTHCARE_A';

UPDATE Policies
SET
	Description = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_A',
	Help = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_A_HELP',
	Civilopedia = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_TEXT_A',
	VassalsNoRebel = 1,
	CSYieldBonusModifier = 25,
	VassalYieldBonusModifier = 25,
	WorkerSpeedModifier = 50
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_A';

-- Lightning Warfare
DELETE FROM Policy_FreePromotions
WHERE PolicyType = 'POLICY_LIGHTNING_WARFARE';

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_LIGHTNING_WARFARE', 'PROMOTION_FAST_GENERAL'),
	('POLICY_LIGHTNING_WARFARE', 'PROMOTION_LIGHTNING_WARFARE_GUN'),
	('POLICY_LIGHTNING_WARFARE', 'PROMOTION_LIGHTNING_WARFARE_ARMOR');

-- Police State
INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_POLICE_STATE', 'BUILDINGCLASS_POLICE_STATION', 100);

INSERT INTO Policy_BuildingClassSecurityChanges
	(PolicyType, BuildingClassType, SecurityChange)
VALUES
	('POLICY_POLICE_STATE', 'BUILDINGCLASS_POLICE_STATION', 10);

-- Nationalism (now Commerce Raiders)
UPDATE Policies
SET UnitGoldMaintenanceMod = 0
WHERE Type = 'POLICY_NATIONALISM';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NATIONALISM', 'IMPROVEMENT_OFFSHORE_PLATFORM', 'YIELD_PRODUCTION', 2),
	('POLICY_NATIONALISM', 'IMPROVEMENT_OFFSHORE_PLATFORM', 'YIELD_CULTURE', 2),
	('POLICY_NATIONALISM', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_PRODUCTION', 2),
	('POLICY_NATIONALISM', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_CULTURE', 2);

INSERT INTO Policy_FeatureYieldChanges
	(PolicyType, FeatureType, YieldType, Yield)
VALUES
	('POLICY_NATIONALISM', 'FEATURE_ATOLL', 'YIELD_PRODUCTION', 2),
	('POLICY_NATIONALISM', 'FEATURE_ATOLL', 'YIELD_CULTURE', 2);

INSERT INTO Policy_ResourcefromCSAlly
	(PolicyType, ResourceType, Number)
VALUES
	('POLICY_NATIONALISM', 'RESOURCE_OIL', 100),
	('POLICY_NATIONALISM', 'RESOURCE_COAL', 100);

INSERT INTO Policy_FreePromotions
	(PolicyType, PromotionType)
VALUES
	('POLICY_NATIONALISM', 'PROMOTION_MARE_NOSTRUM');

-- Third Alternative
DELETE FROM Policy_CapitalYieldChanges
WHERE PolicyType = 'POLICY_THIRD_ALTERNATIVE';

UPDATE Policies
SET UnitGoldMaintenanceMod = -25
WHERE Type = 'POLICY_THIRD_ALTERNATIVE';

-- Total War
UPDATE Policies
SET
	FreeExperience = 0,
	MilitaryProductionModifier = 0,
	WarScoreModifier = 25,
	MinorBullyScoreModifier = 25
WHERE Type = 'POLICY_TOTAL_WAR';

INSERT INTO Policy_UnitCombatProductionModifiers
	(PolicyType, UnitCombatType, ProductionModifier)
SELECT
	'POLICY_TOTAL_WAR', Type, 25
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsNaval = 0 AND IsAerial = 0;

-- Clausewitz's Legacy (now Martial Spirit)
UPDATE Policies
SET
	Level = 2,
	WarWearinessModifier = 25,
	RazingSpeedBonus = 100
WHERE Type = 'POLICY_NEW_ORDER';

-- Syndicalism
INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	'POLICY_INFILTRATION', OfficeBuildingClass, 'YIELD_PRODUCTION', 2
FROM Corporations;

-- Militarism (now Air Supremacy)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_MILITARISM';

UPDATE Policies
SET
	Level = 3,
	AllCityFreeBuilding = 'BUILDINGCLASS_AIRPORT'
WHERE Type = 'POLICY_MILITARISM';

INSERT INTO Policy_UnitCombatProductionModifiers
	(PolicyType, UnitCombatType, ProductionModifier)
SELECT
	'POLICY_MILITARISM', Type, 25
FROM UnitCombatInfos
WHERE IsAerial = 1;

INSERT INTO Policy_UnitClassReplacements
	(PolicyType, ReplacedUnitClassType, ReplacementUnitClassType)
VALUES
	('POLICY_MILITARISM', 'UNITCLASS_FIGHTER', 'UNITCLASS_ZERO');

-- Cult of Personality
UPDATE Policies
SET
	PositiveWarScoreTourismMod = 50,
	NumFreeGreatPeople = 1,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_CULT_PERSONALITY';

-- Gunboat Diplomacy (now Tyranny)
UPDATE Policies
SET
	MinorBullyScoreModifier = 0,
	AfraidMinorPerTurnInfluence = 0,
	CanBullyFriendlyCS = 1,
	BullyGlobalCSInfluenceShift = 10
WHERE Type = 'POLICY_GUNBOAT_DIPLOMACY';
