----------------------------------------------------------
-- Unique Ability: Sacrificial Captives
----------------------------------------------------------
UPDATE Traits
SET
	CultureFromKills = 0,
	GoldenAgeFromVictory = 10
WHERE Type = 'TRAIT_CULTURE_FROM_KILLS';

INSERT INTO Trait_YieldFromKills
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_GOLD', 150),
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_FAITH', 150);

----------------------------------------------------------
-- Unique Unit: Jaguar (Longswordsman)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_LONGSWORDSMAN' WHERE UnitType = 'UNIT_AZTEC_JAGUAR';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN') + 1
WHERE Type = 'UNIT_AZTEC_JAGUAR';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_AZTEC_JAGUAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY'),
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_COUP_DE_GRACE');

----------------------------------------------------------
-- Unique Unit: Otomi (Warrior)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_AZTEC', 'UNITCLASS_WARRIOR', 'UNIT_OTOMI');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_WARRIOR'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_WARRIOR') + 2
WHERE Type = 'UNIT_OTOMI';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_OTOMI' AND PromotionType = 'PROMOTION_BRUTE_FORCE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_OTOMI', 'PROMOTION_WOODSMAN'),
	('UNIT_OTOMI', 'PROMOTION_SURVIVALISM_1'),
	('UNIT_OTOMI', 'PROMOTION_BRUTE_STRENGTH');

----------------------------------------------------------
-- Unique Building: Telpochcalli (Library)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AZTEC', 'BUILDINGCLASS_LIBRARY', 'BUILDING_TELPOCHCALLI');

UPDATE Buildings
SET
	ExperiencePerGoldenAge = 2,
	ExperiencePerGoldenAgeCap = 20
WHERE Type = 'BUILDING_TELPOCHCALLI';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TELPOCHCALLI', 'YIELD_SCIENCE', 25);

INSERT INTO Building_YieldChangesPerGoldenAge
	(BuildingType, YieldType, Yield, YieldCap)
VALUES
	('BUILDING_TELPOCHCALLI', 'YIELD_FOOD', 1, 10),
	('BUILDING_TELPOCHCALLI', 'YIELD_PRODUCTION', 1, 10);

----------------------------------------------------------
-- Unique Building: Huey Teocalli (Grand Temple)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AZTEC', 'BUILDINGCLASS_GRAND_TEMPLE', 'BUILDING_HUEY_TEOCALLI');

UPDATE Buildings SET GoldenAge = 1 WHERE Type = 'BUILDING_HUEY_TEOCALLI';

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_HUEY_TEOCALLI';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HUEY_TEOCALLI', 'YIELD_FOOD', 3);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_HUEY_TEOCALLI', 'BUILDINGCLASS_TEMPLE', 'YIELD_FOOD', 2),
	('BUILDING_HUEY_TEOCALLI', 'BUILDINGCLASS_BARRACKS', 'YIELD_FOOD', 2),
	('BUILDING_HUEY_TEOCALLI', 'BUILDINGCLASS_BARRACKS', 'YIELD_GOLD', 1),
	('BUILDING_HUEY_TEOCALLI', 'BUILDINGCLASS_BARRACKS', 'YIELD_FAITH', 2);

INSERT INTO Building_LakePlotYieldChangesGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HUEY_TEOCALLI', 'YIELD_FOOD', 2);

INSERT INTO Building_YieldFromGoldenAgeStart
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HUEY_TEOCALLI', 'YIELD_POPULATION', 1);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling, GoldenAgeOnly)
VALUES
	('BUILDING_HUEY_TEOCALLI', 'YIELD_GOLD', 10, 1, 1),
	('BUILDING_HUEY_TEOCALLI', 'YIELD_FAITH', 10, 1, 1);
