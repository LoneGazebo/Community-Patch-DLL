-- Hero of the People
UPDATE Policies
SET
	-- GreatPeopleRateModifier = 25,
	NumFreeGreatPeople = 1,
	IncludesOneShotFreeUnits = 1
WHERE Type = 'POLICY_HERO_OF_THE_PEOPLE';

-- Socialist Realism (now People's Army)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_SOCIALIST_REALISM';

DELETE FROM Policy_BuildingClassProductionModifiers
WHERE PolicyType = 'POLICY_SOCIALIST_REALISM';

UPDATE Policies
SET OneShot = 1
WHERE Type = 'POLICY_SOCIALIST_REALISM';

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_MILITARY_ACADEMY', 100);

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_MILITARY_ACADEMY', 5);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_CULTURE', 5);

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_SOCIALIST_REALISM', 'BUILDINGCLASS_PUBLIC_SCHOOL', 2);

-- Skyscrapers (now Communism)
UPDATE Policies
SET
	BuildingPurchaseCostModifier = -25,
	WonderProductionModifier = 20
WHERE Type = 'POLICY_SKYSCRAPERS';

-- Patriotic War
UPDATE Policies
SET CityCaptureHealLocal = 100
WHERE Type = 'POLICY_PATRIOTIC_WAR';

INSERT INTO Policy_UnitClassReplacements
	(PolicyType, ReplacedUnitClassType, ReplacementUnitClassType)
VALUES
	('POLICY_PATRIOTIC_WAR', 'UNITCLASS_TANK', 'UNITCLASS_PANZER');

-- Double Agents
UPDATE Policies
SET
	CatchSpiesModifier = 0,
	FreeSpy = 2
WHERE Type = 'POLICY_DOUBLE_AGENTS';

INSERT INTO Policy_YieldForSpyID
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_DOUBLE_AGENTS', 'YIELD_SCIENCE', 125);

-- Worker's Faculties
UPDATE Policies
SET
	Level = 1,
	OneShot = 1
WHERE Type = 'POLICY_WORKERS_FACULTIES';

UPDATE Policy_BuildingClassYieldModifiers
SET YieldMod = 10
WHERE PolicyType = 'POLICY_WORKERS_FACULTIES';

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_WORKERS_FACULTIES', 'BUILDINGCLASS_FACTORY', 5);

-- Universal Healthcare (now Peace, Land, Bread)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_UNIVERSAL_HEALTHCARE_O';

UPDATE Policies
SET
	Description = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_O',
	Help = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_O_HELP',
	Civilopedia = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_TEXT_O',
	PovertyFlatReductionGlobal = 2,
	CityGrowthMod = 20,
	BuildingGoldMaintenanceMod = -20
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_O';

-- Young Pioneers (now Great Leap Forward)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_YOUNG_PIONEERS';

UPDATE Policies
SET
	Level = 2,
	NumFreeTechs = 1,
	OneShot = 1,
	PassiveEspionageBonusModifier = 100
WHERE Type = 'POLICY_YOUNG_PIONEERS';

INSERT INTO Policy_YieldFromNonSpecialistCitizens
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_YOUNG_PIONEERS', 'YIELD_SCIENCE', 34);

-- Academy of Sciences
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_ACADEMY_SCIENCES';

UPDATE Policies
SET
	IlliteracyFlatReductionGlobal = 2,
	OneShot = 1
WHERE Type = 'POLICY_ACADEMY_SCIENCES';

INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 100);

INSERT INTO Policy_FreeBuilding
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 5);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_ACADEMY_SCIENCES', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 2);

-- Party Leadership (now Dictatorship of the Proletariat)
DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_PARTY_LEADERSHIP' AND YieldType = 'YIELD_PRODUCTION';

UPDATE Policy_CityYieldChanges
SET Yield = 7
WHERE PolicyType = 'POLICY_PARTY_LEADERSHIP';

-- Resettlement
UPDATE Policies
SET
	-- NewCityExtraPopulation = 3,
	FreePopulation = 2,
	NoPartisans = 1,
	OneShot = 1
WHERE Type = 'POLICY_RESETTLEMENT';

-- Five Year Plan
DELETE FROM Policy_CityYieldChanges
WHERE PolicyType = 'POLICY_FIVE_YEAR_PLAN';

DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_FIVE_YEAR_PLAN';

UPDATE Policies
SET BuildingProductionModifier = 20
WHERE Type = 'POLICY_FIVE_YEAR_PLAN';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
SELECT
	'POLICY_FIVE_YEAR_PLAN', Type, 'YIELD_PRODUCTION', 3
FROM Improvements
WHERE SpecificCivRequired = 1 OR Type IN (
	'IMPROVEMENT_MINE',
	'IMPROVEMENT_QUARRY',
	'IMPROVEMENT_LUMBERMILL',
	'IMPROVEMENT_WELL',
	-- hidden unique improvements
	'IMPROVEMENT_POLDER_WATER',
	'IMPROVEMENT_MONGOLIA_ORDO'
);

-- Dictatorship of the Proletariat (now Cultural Revolution)
UPDATE Policies
SET
	Level = 2,
	LessHappyTourismModifier = 25
WHERE Type = 'POLICY_DICTATORSHIP_PROLETARIAT';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_DICTATORSHIP_PROLETARIAT', 'BUILDINGCLASS_FACTORY', 1);

-- Nationalization
INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	'POLICY_NATIONALIZATION', OfficeBuildingClass, 'YIELD_SCIENCE', 2
FROM Corporations;

-- Cultural Revolution (now Socialist Realism)
UPDATE Policies
SET
	SharedIdeologyTourismModifier = 25,
	Level = 3
WHERE Type = 'POLICY_CULTURAL_REVOLUTION';

INSERT INTO Policy_GreatWorkYieldChanges
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CULTURAL_REVOLUTION', 'YIELD_PRODUCTION', 4);

-- Iron Curtain
UPDATE Policies
SET InternalTradeRouteYieldModifier = 200
WHERE Type = 'POLICY_IRON_CURTAIN';

INSERT INTO Policy_YieldChangeTradeRoute
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_IRON_CURTAIN', 'YIELD_PRODUCTION', 5),
	('POLICY_IRON_CURTAIN', 'YIELD_GOLD', 5);

-- Spaceflight Pioneers
UPDATE Policies
SET
	-- EnablesSSPartHurry = 1,
	GreatEngineerHurryModifier = 50
WHERE Type = 'POLICY_SPACEFLIGHT_PIONEERS';

INSERT INTO Policy_YieldGPExpend
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_SPACEFLIGHT_PIONEERS', 'YIELD_SCIENCE', 100);
