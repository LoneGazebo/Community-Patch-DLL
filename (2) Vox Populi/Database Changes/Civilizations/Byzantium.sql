----------------------------------------------------------
-- Unique Ability: Achaemenid Legacy
----------------------------------------------------------
UPDATE Traits
SET
	AlwaysReligion = 1,
	AnyBelief = 1,
	FaithCostModifier = -15,
	GPFaithPurchaseEra = 'ERA_CLASSICAL'
WHERE Type = 'TRAIT_EXTRA_BELIEF';

-- Has a "unique" Fort
INSERT INTO Trait_NoBuilds
	(TraitType, BuildType)
VALUES
	('TRAIT_EXTRA_BELIEF', 'BUILD_FORT');

----------------------------------------------------------
-- Unique Unit: Cataphract (Knight)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 4,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_KNIGHT') - 1
WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT' AND PromotionType IN (
	'PROMOTION_NO_DEFENSIVE_BONUSES',
	'PROMOTION_CITY_PENALTY'
);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_COVER_1'),
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_SMALL_CITY_PENALTY'),
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_OPEN_TERRAIN');

----------------------------------------------------------
-- Unique Unit: Dromon (Liburna)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_LIBURNA' WHERE UnitType = 'UNIT_BYZANTINE_DROMON';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LIBURNA'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LIBURNA') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_LIBURNA') + 3
WHERE Type = 'UNIT_BYZANTINE_DROMON';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BYZANTINE_DROMON', 'PROMOTION_GREEK_FIRE');

----------------------------------------------------------
-- Unique Improvement: Aplekton
-- Considered a unique Fort in all in-game text, but NOT in code
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_ENGINEERING',
	Time = 500
WHERE Type = 'BUILD_APLEKTON';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_APLEKTON');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_APLEKTON', 'FEATURE_FOREST', 400, 1),
	('BUILD_APLEKTON', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_APLEKTON', 'FEATURE_MARSH', 500, 1);

UPDATE Improvements
SET
	NoTwoAdjacent = 1,
	BuildableOnResources = 1,
	DefenseModifier = 50,
	NoFollowUp = 1,
	MakesPassable = 1,
	BlockTileSteal = 1
WHERE Type = 'IMPROVEMENT_APLEKTON';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_APLEKTON', 'TERRAIN_GRASS'),
	('IMPROVEMENT_APLEKTON', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_APLEKTON', 'TERRAIN_DESERT'),
	('IMPROVEMENT_APLEKTON', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_APLEKTON', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_APLEKTON', 'YIELD_FOOD', 2),
	('IMPROVEMENT_APLEKTON', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_APLEKTON', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_APLEKTON', 'YIELD_CULTURE_LOCAL', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_APLEKTON', 'TECH_CHEMISTRY', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_APLEKTON', 'TECH_MILITARY_SCIENCE', 'YIELD_FOOD', 1),
	('IMPROVEMENT_APLEKTON', 'TECH_MILITARY_SCIENCE', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_APLEKTON', 'TECH_MILITARY_SCIENCE', 'YIELD_CULTURE_LOCAL', 2),
	('IMPROVEMENT_APLEKTON', 'TECH_STEALTH', 'YIELD_SCIENCE', 4),
	('IMPROVEMENT_APLEKTON', 'TECH_ELECTRONICS', 'YIELD_FOOD', 2),
	('IMPROVEMENT_APLEKTON', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_APLEKTON', 'TECH_ELECTRONICS', 'YIELD_CULTURE_LOCAL', 4);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_APLEKTON', 'YIELD_SCIENCE', 2),
	('POLICY_NAVAL_TRADITION', 'IMPROVEMENT_APLEKTON', 'YIELD_CULTURE', 1);
-- Other policy boosts are handled in the policy files

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_APLEKTON', 'YIELD_CULTURE', 2),
	('BUILDING_MENIN_GATE', 'IMPROVEMENT_APLEKTON', 'YIELD_GOLDEN_AGE_POINTS', 2);

----------------------------------------------------------
-- Unique Building: Tetraconch (Temple)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_BYZANTIUM', 'BUILDINGCLASS_TEMPLE', 'BUILDING_TETRACONCH');

UPDATE Buildings
SET
	MaxStartEra = NULL,
	TradeReligionModifier = 100
WHERE Type = 'BUILDING_TETRACONCH';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_TETRACONCH', 'YIELD_FAITH', 3),
	('BUILDING_TETRACONCH', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TETRACONCH', 'YIELD_FAITH', 34);
