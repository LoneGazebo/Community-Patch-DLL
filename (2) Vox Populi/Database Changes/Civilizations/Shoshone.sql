----------------------------------------------------------
-- Unique Ability: Great Expanse
----------------------------------------------------------
UPDATE Traits
SET
	ExtraFoundedCityTerritoryClaimRange = 4,
	ExtraConqueredCityTerritoryClaimRange = 4,
	CombatBonusImprovement = 'IMPROVEMENT_ENCAMPMENT_SHOSHONE',
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
-- Unique Unit: Comanche Riders (Cavalry)
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
-- Unique Improvement: Encampment
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_HORSEBACK_RIDING', -- Trade
	Time = 600
WHERE Type = 'BUILD_ENCAMPMENT_SHOSHONE';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_ENCAMPMENT_SHOSHONE');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_FOREST', 400, 1),
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_ENCAMPMENT_SHOSHONE', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	RequiresFlatlands = 1,
	NoTwoAdjacent = 1,
	DefenseModifier = 15,
	NearbyEnemyDamage = 5
WHERE Type = 'IMPROVEMENT_ENCAMPMENT_SHOSHONE';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TERRAIN_GRASS'),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TERRAIN_DESERT'),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_FOOD', 2),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_DRAMA', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_GUNPOWDER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_GUNPOWDER', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_RIFLING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_RIFLING', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_ADVANCED_BALLISTICS', 'YIELD_FOOD', 1),
	('IMPROVEMENT_ENCAMPMENT_SHOSHONE', 'TECH_ADVANCED_BALLISTICS', 'YIELD_SCIENCE', 1);
