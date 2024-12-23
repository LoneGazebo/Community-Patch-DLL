----------------------------------------------------------
-- Unique Ability: Druidic Lore
----------------------------------------------------------
UPDATE Traits
SET
	FaithFromUnimprovedForest = 0,
	UniqueBeliefsOnly = 1,
	NoNaturalReligionSpread = 1
WHERE Type = 'TRAIT_FAITH_FROM_NATURE';

INSERT INTO Trait_YieldFromOwnPantheon
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_FAITH_FROM_NATURE', 'YIELD_FAITH', 3);

----------------------------------------------------------
-- Unique Unit: Pictish Warrior (Spearman)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_MINING',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPEARMAN'
			)
		)
	),
	DefaultUnitAI = 'UNITAI_ATTACK',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 1
WHERE Type = 'UNIT_CELT_PICTISH_WARRIOR';

DELETE FROM Unit_AITypes
WHERE UnitType = 'UNIT_CELT_PICTISH_WARRIOR' AND UnitAIType = 'UNITAI_COUNTER';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_CELT_PICTISH_WARRIOR' AND PromotionType = 'PROMOTION_FORMATION_1';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CELT_PICTISH_WARRIOR', 'PROMOTION_SKI_INFANTRY'),
	('UNIT_CELT_PICTISH_WARRIOR', 'PROMOTION_FREE_PILLAGE_MOVES');

INSERT INTO Unit_YieldFromKills
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_CELT_PICTISH_WARRIOR', 'YIELD_FAITH', 200);

----------------------------------------------------------
-- Unique Unit: Scythed Chariot (Skirmisher)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_CELTS', 'UNITCLASS_SKIRMISHER', 'UNIT_SCYTHED_CHARIOT');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MONGOLIAN_KESHIK'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MONGOLIAN_KESHIK') + 4,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_MONGOLIAN_KESHIK') - 1
WHERE Type = 'UNIT_SCYTHED_CHARIOT';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_SCYTHED_CHARIOT' AND PromotionType = 'PROMOTION_SKIRMISHER_DOCTRINE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SCYTHED_CHARIOT', 'PROMOTION_BEAM_AXLE'),
	('UNIT_SCYTHED_CHARIOT', 'PROMOTION_REND'),
	('UNIT_SCYTHED_CHARIOT', 'PROMOTION_CARNYX');

----------------------------------------------------------
-- Unique Improvement: Oppidum
----------------------------------------------------------
INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_SCIENTIST', 'BUILD_OPPIDUM'),
	('UNIT_MERCHANT', 'BUILD_OPPIDUM'),
	('UNIT_ENGINEER', 'BUILD_OPPIDUM'),
	('UNIT_GREAT_GENERAL', 'BUILD_OPPIDUM');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Remove)
VALUES
	('BUILD_OPPIDUM', 'FEATURE_FOREST', 1),
	('BUILD_OPPIDUM', 'FEATURE_JUNGLE', 1),
	('BUILD_OPPIDUM', 'FEATURE_MARSH', 1);

UPDATE Improvements
SET
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	NoTwoAdjacent = 1,
	CreatedByGreatPerson = 1,
	DefenseModifier = 100,
	NoFollowUp = 1
WHERE Type = 'IMPROVEMENT_OPPIDUM';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_GRASS'),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_DESERT'),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_OPPIDUM', 'YIELD_FOOD', 2),
	('IMPROVEMENT_OPPIDUM', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'YIELD_GOLD', 1),
	('IMPROVEMENT_OPPIDUM', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_OPPIDUM', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OPPIDUM', 'YIELD_FAITH', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_OPPIDUM', 'TECH_METAL_CASTING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_METAL_CASTING', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_PHYSICS', 'YIELD_GOLD', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_PHYSICS', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_CHEMISTRY', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_CHEMISTRY', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_ARCHITECTURE', 'YIELD_FOOD', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_ARCHITECTURE', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_RAILROAD', 'YIELD_GOLD', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_RAILROAD', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_FERTILIZER', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_FLIGHT', 'YIELD_GOLD', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_FLIGHT', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_ELECTRONICS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TECH_ELECTRONICS', 'YIELD_SCIENCE', 1);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NEW_DEAL', 'IMPROVEMENT_OPPIDUM', 'YIELD_PRODUCTION', 2),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_OPPIDUM', 'YIELD_GOLD', 2),
	('POLICY_NEW_DEAL', 'IMPROVEMENT_OPPIDUM', 'YIELD_SCIENCE', 2);

INSERT INTO Improvement_AdjacentTerrainYieldChanges
	(ImprovementType, TerrainType, YieldType, Yield)
VALUES
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_GRASS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_GRASS', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_PLAINS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_PLAINS', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_DESERT', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_DESERT', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_TUNDRA', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_TUNDRA', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_SNOW', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_SNOW', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_COAST', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_OPPIDUM', 'TERRAIN_COAST', 'YIELD_FAITH', 1);

CREATE TEMP TABLE Helper (
	UnitClassType TEXT,
	Amount INTEGER
);

INSERT INTO Helper
VALUES
	('UNITCLASS_SCIENTIST', 5),
	('UNITCLASS_MERCHANT', 10),
	('UNITCLASS_ENGINEER', 5);

INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
SELECT
	a.Type, 'IMPROVEMENT_OPPIDUM', b.Amount
FROM Units a, Helper b
WHERE a.Class = b.UnitClassType;

DROP TABLE Helper;

----------------------------------------------------------
-- Unique Building: Ceilidh Hall (Circus)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_CIRCUS'
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_CEILIDH_HALL_HELP',
	WLTKDTurns = (SELECT WLTKDTurns FROM Buildings WHERE Type = 'BUILDING_CIRCUS') * 150 / 100,
	SpecialistType = 'SPECIALIST_MUSICIAN',
	SpecialistCount = 1,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC',
	GreatWorkCount = 1
WHERE Type = 'BUILDING_CEILIDH_HALL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CEILIDH_HALL', 'YIELD_CULTURE', 4),
	('BUILDING_CEILIDH_HALL', 'YIELD_FAITH', 2);

UPDATE Building_InstantYield
SET Yield = (SELECT Yield FROM Building_InstantYield WHERE BuildingType = 'BUILDING_CIRCUS') * 150 / 100
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';
