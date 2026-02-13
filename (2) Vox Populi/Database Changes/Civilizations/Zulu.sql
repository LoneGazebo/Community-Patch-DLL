----------------------------------------------------------
-- Unique Ability: Iklwa
----------------------------------------------------------
UPDATE Traits
SET
	IgnoreBullyPenalties = 1,
	CSBullyMilitaryStrengthModifier = 50
WHERE Type = 'TRAIT_BUFFALO_HORNS';

INSERT INTO Trait_MaintenanceModifierUnitCombats
	(TraitType, UnitCombatType, MaintenanceModifier)
VALUES
--	('TRAIT_BUFFALO_HORNS', 'UNITCOMBAT_MELEE', -50),
	('TRAIT_BUFFALO_HORNS', 'UNITCOMBAT_GUN', -50);

----------------------------------------------------------
-- Unique Unit: Impi (Tercio)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_ZULU_IMPI';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 3
WHERE Type = 'UNIT_ZULU_IMPI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ZULU_IMPI', 'PROMOTION_FASIMBA'),
	('UNIT_ZULU_IMPI', 'PROMOTION_RANGED_SUPPORT_FIRE');

----------------------------------------------------------
-- Unique Unit: Induna (Great General)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ZULU', 'UNITCLASS_GREAT_GENERAL', 'UNIT_INDUNA');

UPDATE Units
SET
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_GREAT_GENERAL') + 1,
	TileXPOnExpend = 30
WHERE Type = 'UNIT_INDUNA';

INSERT INTO Unit_UniqueNames
	(UnitType, UniqueName)
VALUES
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_01'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_02'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_03'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_04'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_05'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_06'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_07'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_08'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_09'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_10'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_11'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_12'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_13'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_14'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_15'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_16'),
	('UNIT_INDUNA', 'TXT_KEY_UNIT_INDUNA_17');

----------------------------------------------------------
-- Unique Improvement: Isibaya
----------------------------------------------------------
DELETE FROM Unit_Builds WHERE UnitType = 'UNIT_INDUNA' AND BuildType = 'BUILD_CITADEL';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_INDUNA', 'BUILD_ISIBAYA');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Remove)
VALUES
	('BUILD_ISIBAYA', 'FEATURE_FOREST', 1),
	('BUILD_ISIBAYA', 'FEATURE_JUNGLE', 1),
	('BUILD_ISIBAYA', 'FEATURE_MARSH', 1);

UPDATE Improvements
SET
	InAdjacentFriendly = 1,
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	NoTwoAdjacent = 1,
	CultureBombRadius = 1,
	MakesPassable = 1,
	DefenseModifier = 100,
	NoFollowUp = 1,
	NearbyEnemyDamage = 30,
	SpawnsAdjacentResource = 'RESOURCE_COW'
WHERE Type = 'IMPROVEMENT_ISIBAYA';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_ISIBAYA', 'TERRAIN_GRASS'),
	('IMPROVEMENT_ISIBAYA', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_ISIBAYA', 'TERRAIN_DESERT'),
	('IMPROVEMENT_ISIBAYA', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_ISIBAYA', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ISIBAYA', 'YIELD_FOOD', 1),
	('IMPROVEMENT_ISIBAYA', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_ISIBAYA', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_ISIBAYA', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_PASTURE', 'IMPROVEMENT_ISIBAYA', 'YIELD_FOOD', 1, 1),
	('IMPROVEMENT_PASTURE', 'IMPROVEMENT_ISIBAYA', 'YIELD_PRODUCTION', 1, 1),
	('IMPROVEMENT_PASTURE', 'IMPROVEMENT_ISIBAYA', 'YIELD_CULTURE', 1, 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ISIBAYA', 'TECH_CHEMISTRY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_ISIBAYA', 'TECH_MILITARY_SCIENCE', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_ISIBAYA', 'TECH_STEALTH', 'YIELD_SCIENCE', 4),
	('IMPROVEMENT_ISIBAYA', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 4);

----------------------------------------------------------
-- Unique Building: Ikanda (Barracks)
----------------------------------------------------------
UPDATE Buildings
SET
	PrereqTech = 'TECH_BRONZE_WORKING',
	TrainedFreePromotion = 'PROMOTION_IKLWA',
	CitySupplyFlat = (SELECT CitySupplyFlat FROM Buildings WHERE Type = 'BUILDING_BARRACKS') + 1
WHERE Type = 'BUILDING_IKANDA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_IKANDA', 'YIELD_CULTURE', 1);

----------------------------------------------------------
-- Unique Building: Iziko (Theater)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ZULU', 'BUILDINGCLASS_AMPHITHEATER', 'BUILDING_IZIKO');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_AMPHITHEATER' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_IZIKO' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_IZIKO', 'YIELD_FAITH', 2);

INSERT INTO Building_YieldFromUnitLevelUpGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_IZIKO', 'YIELD_CULTURE', 1);
