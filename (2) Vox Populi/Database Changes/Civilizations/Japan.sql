----------------------------------------------------------
-- Unique Ability: Shogunate
----------------------------------------------------------
UPDATE Traits
SET FightWellDamaged = 0
WHERE Type = 'TRAIT_FIGHT_WELL_DAMAGED';

DELETE FROM Trait_ImprovementYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';
DELETE FROM Trait_UnimprovedFeatureYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_MELEE', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_GUN', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_MOUNTED', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_ARMOR', 'PROMOTION_BUSHIDO');

INSERT INTO Trait_GreatPersonBirthGWAM
	(TraitType, GreatPersonType, Value)
VALUES
	('TRAIT_FIGHT_WELL_DAMAGED', 'GREATPERSON_GENERAL', 50),
	('TRAIT_FIGHT_WELL_DAMAGED', 'GREATPERSON_ADMIRAL', 50);

----------------------------------------------------------
-- Unique Unit: Samurai (Longswordsman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN') + 2
WHERE Type = 'UNIT_JAPANESE_SAMURAI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_JAPANESE_SAMURAI', 'PROMOTION_SPAWN_GENERALS_II'),
	('UNIT_JAPANESE_SAMURAI', 'PROMOTION_GAIN_EXPERIENCE');

----------------------------------------------------------
-- Unique Unit: Mikasa (Dreadnought)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_JAPAN', 'UNITCLASS_DREADNOUGHT', 'UNIT_MIKASA');

UPDATE Units
SET
	PrereqTech = 'TECH_ELECTRICITY',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_DREADNOUGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_DREADNOUGHT') + 5,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_DREADNOUGHT') + 5
WHERE Type = 'UNIT_MIKASA';

INSERT INTO Unit_YieldOnCompletion
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_MIKASA', 'YIELD_GOLDEN_AGE_POINTS', 500);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MIKASA', 'PROMOTION_HOMELAND_GUARDIAN'),
	('UNIT_MIKASA', 'PROMOTION_KANTAI_KESSEN');

----------------------------------------------------------
-- Unique Building: Tatara (Forge)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_JAPAN', 'BUILDINGCLASS_FORGE', 'BUILDING_TATARA');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'YIELD_PRODUCTION', 2);

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_FORGE' AND YieldType = 'YIELD_SCIENCE') + 1
WHERE BuildingType = 'BUILDING_TATARA' AND YieldType = 'YIELD_SCIENCE';

UPDATE Buildings
SET SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_FORGE') + 1
WHERE Type = 'BUILDING_TATARA'; 

INSERT INTO Building_ResourcePlotsToPlace
	(BuildingType, ResourceType, NumPlots, ResourceQuantityToPlace)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 1, 1);

INSERT INTO Building_ResourceClaim
	(BuildingType, ResourceType, IncludeOwnedByOtherPlayer)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 0);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromCombatExperienceTimes100
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'YIELD_CULTURE', 140),
	('BUILDING_TATARA', 'YIELD_SCIENCE', 140);

----------------------------------------------------------
-- Unique Improvement: Torii
----------------------------------------------------------
INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WRITER', 'BUILD_TORII'),
	('UNIT_ARTIST', 'BUILD_TORII'),
	('UNIT_MUSICIAN', 'BUILD_TORII');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Remove)
VALUES
	('BUILD_TORII', 'FEATURE_FOREST', 1),
	('BUILD_TORII', 'FEATURE_JUNGLE', 1),
	('BUILD_TORII', 'FEATURE_MARSH', 1);

UPDATE Improvements
SET
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	HappinessOnConstruction = 1
WHERE Type = 'IMPROVEMENT_TORII';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_TORII', 'TERRAIN_GRASS'),
	('IMPROVEMENT_TORII', 'TERRAIN_PLAINS'),
	('IMPROVEMENT_TORII', 'TERRAIN_DESERT'),
	('IMPROVEMENT_TORII', 'TERRAIN_TUNDRA'),
	('IMPROVEMENT_TORII', 'TERRAIN_SNOW');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TORII', 'YIELD_CULTURE', 5),
	('IMPROVEMENT_TORII', 'YIELD_FAITH', 4),
	('IMPROVEMENT_TORII', 'YIELD_TOURISM', 3);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TORII', 'TECH_DRAMA', 'YIELD_CULTURE', 4),
	('IMPROVEMENT_TORII', 'TECH_DRAMA', 'YIELD_TOURISM', 3),
	('IMPROVEMENT_TORII', 'TECH_GUILDS', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TORII', 'TECH_GUILDS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_TORII', 'TECH_GUILDS', 'YIELD_GOLD', 1),
	('IMPROVEMENT_TORII', 'TECH_GUILDS', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_TORII', 'TECH_GUILDS', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_TORII', 'TECH_ACOUSTICS', 'YIELD_FAITH', 4),
	('IMPROVEMENT_TORII', 'TECH_ACOUSTICS', 'YIELD_TOURISM', 2),
	('IMPROVEMENT_TORII', 'TECH_ARCHAEOLOGY', 'YIELD_CULTURE', 7),
	('IMPROVEMENT_TORII', 'TECH_ARCHAEOLOGY', 'YIELD_TOURISM', 3),
	('IMPROVEMENT_TORII', 'TECH_RADIO', 'YIELD_GOLD', 5),
	('IMPROVEMENT_TORII', 'TECH_RADIO', 'YIELD_CULTURE', 4),
	('IMPROVEMENT_TORII', 'TECH_RADIO', 'YIELD_TOURISM', 12);

INSERT INTO Policy_ImprovementYieldChanges
	(PolicyType, ImprovementType, YieldType, Yield)
VALUES
	('POLICY_NEW_DEAL', 'IMPROVEMENT_TORII', 'YIELD_CULTURE', 6);

CREATE TEMP TABLE Helper (
	UnitClassType TEXT,
	Amount INTEGER
);

INSERT INTO Helper
VALUES
	('UNITCLASS_WRITER', 3), -- +3% culture per torii
	('UNITCLASS_ARTIST', 10), -- +10% golden age points per torii
	('UNITCLASS_MUSICIAN', 34); -- +0.34 (1/3) turns per torii

INSERT INTO Unit_ScalingFromOwnedImprovements
	(UnitType, ImprovementType, Amount)
SELECT
	a.Type, 'IMPROVEMENT_TORII', b.Amount
FROM Units a, Helper b
WHERE a.Class = b.UnitClassType;

DROP TABLE Helper;
