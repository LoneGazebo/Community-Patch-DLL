----------------------------------------------------------
-- Unique Ability: Dutch East India Company
----------------------------------------------------------
UPDATE Traits
SET
	LuxuryHappinessRetention = 0,
	ImportsCountTowardsMonopolies = 1
WHERE Type = 'TRAIT_LUXURY_RETENTION';

INSERT INTO Trait_YieldFromImport
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_LUXURY_RETENTION', 'YIELD_GOLD', 3),
	('TRAIT_LUXURY_RETENTION', 'YIELD_CULTURE', 3);

INSERT INTO Trait_YieldFromExport
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_LUXURY_RETENTION', 'YIELD_GOLD', 3),
	('TRAIT_LUXURY_RETENTION', 'YIELD_CULTURE', 3);

----------------------------------------------------------
-- Unique Unit: Sea Beggar (Corvette)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_PRIVATEER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_PRIVATEER') + 2
WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_DUTCH_SEA_BEGGAR', 'PROMOTION_COASTAL_TERROR'),
	('UNIT_DUTCH_SEA_BEGGAR', 'PROMOTION_SUPPLY'),
	('UNIT_DUTCH_SEA_BEGGAR', 'PROMOTION_PRIZE_SHIPS');

----------------------------------------------------------
-- Unique Improvement: Polder
----------------------------------------------------------

-- Give build via traits table so that it doesn't appear in the civ select screen
INSERT INTO Trait_BuildsUnitClasses
	(TraitType, UnitClassType, BuildType)
VALUES
	('TRAIT_LUXURY_RETENTION', 'UNITCLASS_WORKER', 'BUILD_POLDER_WATER');

UPDATE Builds
SET
	Help = 'TXT_KEY_BUILD_POLDER_HELP',
	Recommendation = 'TXT_KEY_BUILD_POLDER_REC'
WHERE Type = 'BUILD_POLDER';

UPDATE Builds
SET
	PrereqTech = (SELECT PrereqTech FROM Builds WHERE Type = 'BUILD_POLDER'),
	Time = (SELECT Time FROM Builds WHERE Type = 'BUILD_POLDER'),
	Water = 1,
	CanBeEmbarked = 1
WHERE Type = 'BUILD_POLDER_WATER';

UPDATE Improvements
SET
	Water = 1,
	RequiresXAdjacentLand = 3,
	AllowsWalkWater = 1
WHERE Type = 'IMPROVEMENT_POLDER_WATER';

-- Marsh only
DELETE FROM Improvement_ValidFeatures
WHERE ImprovementType = 'IMPROVEMENT_POLDER' AND FeatureType = 'FEATURE_FLOOD_PLAINS';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_POLDER_WATER', 'TERRAIN_COAST');

DELETE FROM Improvement_Yields WHERE ImprovementType = 'IMPROVEMENT_POLDER';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_POLDER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_POLDER', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_POLDER', 'YIELD_GOLD', 2),
	('IMPROVEMENT_POLDER_WATER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_POLDER_WATER', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_POLDER_WATER', 'YIELD_GOLD', 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_POLDER', 'TECH_CHEMISTRY', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_POLDER', 'TECH_ECONOMICS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_POLDER', 'TECH_ECONOMICS', 'YIELD_GOLD', 2),
	('IMPROVEMENT_POLDER_WATER', 'TECH_CHEMISTRY', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_POLDER_WATER', 'TECH_ECONOMICS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_POLDER_WATER', 'TECH_ECONOMICS', 'YIELD_GOLD', 2);
