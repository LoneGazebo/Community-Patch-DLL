------------------
-- Level 1
------------------

-- Avant Garde
UPDATE Policies
SET
	GreatPeopleRateModifier = 33,
	BoredomFlatReductionGlobal = 2
WHERE Type = 'POLICY_OPEN_SOCIETY';

-- Creative Expression
UPDATE Policy_GreatWorkYieldChanges
SET YieldType = 'YIELD_TOURISM', Yield = 2
WHERE PolicyType = 'POLICY_CREATIVE_EXPRESSION';

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
VALUES
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_MUSEUM', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_MUSEUM', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_BROADCAST_TOWER', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_BROADCAST_TOWER', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_OPERA_HOUSE', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_CULTURE', 3),
	('POLICY_CREATIVE_EXPRESSION', 'BUILDINGCLASS_AMPHITHEATER', 'YIELD_GOLDEN_AGE_POINTS', 3);

-- Civil Society
UPDATE Policies
SET
	HalfSpecialistFood = 0,
	SpecialistFoodChange = -2
WHERE Type = 'POLICY_CIVIL_SOCIETY';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
SELECT
	'POLICY_CIVIL_SOCIETY', Type, 'YIELD_FOOD', 4
FROM Improvements
WHERE SpecificCivRequired = 1 OR Type IN (
	'IMPROVEMENT_FARM',
	'IMPROVEMENT_PLANTATION',
	'IMPROVEMENT_CAMP',
	-- hidden unique improvements
	'IMPROVEMENT_POLDER_WATER',
	'IMPROVEMENT_MONGOLIA_ORDO'
);

-- Volunteer Army (now Draft Registration)
DELETE FROM Policy_FreeUnitClasses
WHERE PolicyType = 'POLICY_VOLUNTEER_ARMY';

UPDATE Policies
SET
	Level = 1,
	BaseFreeUnits = 0,
	IncludesOneShotFreeUnits = 0,
	NoXPLossUnitPurchase = 1
WHERE Type = 'POLICY_VOLUNTEER_ARMY';

-- Covert Action
UPDATE Policies
SET
	RiggingElectionModifier = 0,
	FreeSpy = 1,
	RigElectionInfluenceModifier = 50,
	EspionageNetworkPoints = 10
WHERE Type = 'POLICY_COVERT_ACTION';

-- Economic Union
UPDATE Policies
SET
	FreeTradeRoute = 2,
	SharedIdeologyTradeGoldChange = 6
WHERE Type = 'POLICY_ECONOMIC_UNION';

-- Universal Suffrage 
UPDATE Policies
SET
	Level = 1,
	HalfSpecialistUnhappiness = 0,
	GoldenAgeTurns = 10,
	-- GoldenAgeDurationMod = 50,
	ExtraHappinessPerCity = 1,
	OneShot = 1
WHERE Type = 'POLICY_UNIVERSAL_SUFFRAGE';

------------------
-- Level 2
------------------

-- Urbanization (now Self Determination)
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_URBANIZATION';

UPDATE Policies
SET
	ExperienceAllUnitsFromLiberation = 15,
	InfluenceAllCSFromLiberation = 50,
	NumUnitsInLiberatedCities = 6
WHERE Type = 'POLICY_URBANIZATION';

INSERT INTO Policy_BuildingClassInLiberatedCities
	(PolicyType, BuildingClassType, Count)
VALUES
	('POLICY_URBANIZATION', 'BUILDINGCLASS_ARSENAL', 1);

INSERT INTO Policy_YieldForLiberation
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_URBANIZATION', 'YIELD_SCIENCE', 40);

-- Capitalism
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_CAPITALISM';

UPDATE Policies
SET
	Level = 2,
	NoUnhappfromXSpecialists = 2,
	HappfromXSpecialists = 2
WHERE Type = 'POLICY_CAPITALISM';

INSERT INTO Policy_SpecialistExtraYields
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_CAPITALISM', 'YIELD_GOLD', 1),
	('POLICY_CAPITALISM', 'YIELD_SCIENCE', 1);

-- Their Finest Hour
UPDATE Policies
SET
	CityStrengthMod = 0,
	MaxAirUnitsChangeGlobal = 2,
	FlatDefenseFromAirUnits = 3
WHERE Type = 'POLICY_THEIR_FINEST_HOUR';

INSERT INTO Policy_UnitClassReplacements
	(PolicyType, ReplacedUnitClassType, ReplacementUnitClassType)
VALUES
	('POLICY_THEIR_FINEST_HOUR', 'UNITCLASS_BOMBER', 'UNITCLASS_B17');

-- New Deal
DELETE FROM Policy_ImprovementYieldChanges
WHERE PolicyType = 'POLICY_NEW_DEAL';

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NEW_DEAL', 'IMPROVEMENT_LANDMARK', 'YIELD_TOURISM', 8),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_ACADEMY', 'YIELD_SCIENCE', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_CUSTOMS_HOUSE', 'YIELD_GOLD', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_MANUFACTORY', 'YIELD_PRODUCTION', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_CITADEL', 'YIELD_PRODUCTION', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_HOLY_SITE', 'YIELD_FAITH', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_EMBASSY', 'YIELD_CULTURE', 6),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_MONGOLIA_ORDO', 'YIELD_PRODUCTION', 6);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
SELECT
	'POLICY_NEW_DEAL', Type, 'YIELD_TOURISM', 2
FROM Improvements
WHERE CreatedByGreatPerson = 1;

-- Arsenal of Democracy
UPDATE Policies
SET
	-- MilitaryProductionModifier = 15,
	InfluenceGPExpend = 10,
	MilitaryUnitGiftExtraInfluence = 40
WHERE Type = 'POLICY_ARSENAL_DEMOCRACY';

-- Universal Healthcare
DELETE FROM Policy_BuildingClassHappiness
WHERE PolicyType = 'POLICY_UNIVERSAL_HEALTHCARE_F';

UPDATE Policies
SET
	Level = 2,
	Help = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_F_HELP',
	Description = 'TXT_KEY_POLICY_UNIVERSAL_HEALTHCARE_F',
	AllCityFreeBuilding = 'BUILDINGCLASS_HOSPITAL'
WHERE Type = 'POLICY_UNIVERSAL_HEALTHCARE_F';

INSERT INTO Policy_YieldFromBirth
	(PolicyType, YieldType, Yield)
VALUES
	('POLICY_UNIVERSAL_HEALTHCARE_F', 'YIELD_CULTURE', 50);

-- Transnationalism
INSERT INTO Policy_FranchisePerImprovement
	(PolicyType, ImprovementType, NumFranchise)
VALUES
	('POLICY_GLOBALIZATION', 'IMPROVEMENT_CUSTOMS_HOUSE', 1);

INSERT INTO Policy_BuildingClassYieldChanges
	(PolicyType, BuildingClassType, YieldType, YieldChange)
SELECT
	'POLICY_GLOBALIZATION', OfficeBuildingClass, 'YIELD_CULTURE', 2
FROM Corporations;

------------------
-- Level 3
------------------

-- Media Culture
UPDATE Policy_BuildingClassTourismModifiers
SET TourismModifier = 25
WHERE PolicyType = 'POLICY_MEDIA_CULTURE' AND BuildingClassType = 'BUILDINGCLASS_BROADCAST_TOWER';

INSERT INTO Policy_BuildingClassHappiness
	(PolicyType, BuildingClassType, Happiness)
VALUES
	('POLICY_MEDIA_CULTURE', 'BUILDINGCLASS_BROADCAST_TOWER', 1);

INSERT INTO Policy_BuildingClassYieldModifiers
	(PolicyType, BuildingClassType, YieldType, YieldMod)
VALUES
	('POLICY_MEDIA_CULTURE', 'BUILDINGCLASS_STADIUM', 'YIELD_CULTURE', 20);

-- Treaty Organization
UPDATE Policies
SET
	ProtectedMinorPerTurnInfluence = 200,
	FreeWCVotes = 2
WHERE Type = 'POLICY_TREATY_ORGANIZATION';

-- Space Procurements
INSERT INTO Policy_BuildingClassProductionModifiers
	(PolicyType, BuildingClassType, ProductionModifier)
VALUES
	('POLICY_SPACE_PROCUREMENTS', 'BUILDINGCLASS_SPACESHIP_FACTORY', 100);

INSERT INTO Policy_BuildingClassYieldModifiers
	(PolicyType, BuildingClassType, YieldType, YieldMod)
VALUES
	('POLICY_SPACE_PROCUREMENTS', 'BUILDINGCLASS_LABORATORY', 'YIELD_SCIENCE', 20);
