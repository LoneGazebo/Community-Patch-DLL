----------------------------------------------------------
-- Unique Ability: Great Expanse
----------------------------------------------------------
UPDATE Traits
SET
	ExtraFoundedCityTerritoryClaimRange = 4,
	ExtraConqueredCityTerritoryClaimRange = 4,
	CombatBonusImprovement = 'IMPROVEMENT_VP_ENCAMPMENT',
	NearbyImprovementBonusRange = 2,
	NearbyImprovementCombatBonus = 20
WHERE Type = 'TRAIT_GREAT_EXPANSE';

DELETE FROM Trait_FreePromotionUnitCombats WHERE TraitType = 'TRAIT_GREAT_EXPANSE' AND PromotionType = 'PROMOTION_HOMELAND_GUARDIAN_BOOGALOO';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_GREAT_EXPANSE', 'UNITCOMBAT_RECON', 'PROMOTION_GOODY_HUT_PICKER');

INSERT INTO Trait_TerrainClaimBoost
	(TraitType, TerrainType)
VALUES
	('TRAIT_GREAT_EXPANSE', 'TERRAIN_GRASS'),
	('TRAIT_GREAT_EXPANSE', 'TERRAIN_PLAINS'),
	('TRAIT_GREAT_EXPANSE', 'TERRAIN_DESERT'),
	('TRAIT_GREAT_EXPANSE', 'TERRAIN_TUNDRA'),
	('TRAIT_GREAT_EXPANSE', 'TERRAIN_SNOW');

----------------------------------------------------------
-- Unique Unit: Comanche Rider (Cavalry)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CAVALRY'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CAVALRY') + 4
WHERE Type = 'UNIT_SHOSHONE_COMANCHE_RIDERS';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SHOSHONE_COMANCHE_RIDERS', 'PROMOTION_FREE_PILLAGE_MOVES'),
	('UNIT_SHOSHONE_COMANCHE_RIDERS', 'PROMOTION_WITHDRAW_BEFORE_MELEE'),
	('UNIT_SHOSHONE_COMANCHE_RIDERS', 'PROMOTION_MOON_STRIKER');

----------------------------------------------------------
-- Unique Unit: Yellow Brow (Tercio)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SHOSHONE', 'UNITCLASS_TERCIO', 'UNIT_YELLOW_BROW');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 2
WHERE Type = 'UNIT_YELLOW_BROW';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_YELLOW_BROW', 'PROMOTION_BIG_HORSE_DANCE');

----------------------------------------------------------
-- Unique Improvement: Encampment
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_HORSEBACK_RIDING', -- Trade
	Time = 600
WHERE Type = 'BUILD_ENCAMPMENT';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_ENCAMPMENT');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_ENCAMPMENT', 'FEATURE_FOREST', 400, 1),
	('BUILD_ENCAMPMENT', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_ENCAMPMENT', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	RequiresFlatlands = 1,
	NoTwoAdjacent = 1,
	DefenseModifier = 15,
	NearbyEnemyDamage = 5
WHERE Type = 'IMPROVEMENT_VP_ENCAMPMENT';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_VP_ENCAMPMENT', 'TERRAIN_GRASS'),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TERRAIN_DESERT'),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_VP_ENCAMPMENT', 'YIELD_FOOD', 2),
	('IMPROVEMENT_VP_ENCAMPMENT', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_DRAMA', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_GUNPOWDER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_GUNPOWDER', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_RIFLING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_RIFLING', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_ADVANCED_BALLISTICS', 'YIELD_FOOD', 1),
	('IMPROVEMENT_VP_ENCAMPMENT', 'TECH_ADVANCED_BALLISTICS', 'YIELD_SCIENCE', 1);

----------------------------------------------------------
-- Unique Building: Buffalo Pound (Well)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SHOSHONE', 'BUILDINGCLASS_WELL', 'BUILDING_BUFFALO_POUND'),
	('CIVILIZATION_SHOSHONE', 'BUILDINGCLASS_WATERMILL', NULL);

UPDATE Buildings
SET
	PrereqTech = 'TECH_TRAPPING',
	IsNoRiver = 0
WHERE Type = 'BUILDING_BUFFALO_POUND';

INSERT INTO Building_YieldChangesPerXTiles
	(BuildingType, YieldType, Yield, NumRequired)
VALUES
	('BUILDING_BUFFALO_POUND', 'YIELD_FOOD', 1, 6),
	('BUILDING_BUFFALO_POUND', 'YIELD_PRODUCTION', 1, 6);
