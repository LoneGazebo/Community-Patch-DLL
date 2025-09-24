----------------------------------------------------------
-- Unique Ability: Great Ancestor
----------------------------------------------------------
UPDATE Traits
SET WonderProductionModGA = 20
WHERE Type = 'TRAIT_WONDER_BUILDER';

INSERT INTO Trait_AlternateResourceTechs
	(TraitType, ResourceType, TechReveal)
VALUES
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 'TECH_AGRICULTURE');

INSERT INTO Trait_FreeResourceFirstXCities
	(TraitType, ResourceType, ResourceQuantity, NumCities)
VALUES
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 1, 9999);

INSERT INTO Trait_FreeResourceOnWorldWonderCompletion
	(TraitType, ResourceType, ResourceQuantity)
VALUES
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 1);

INSERT INTO Trait_ResourceYieldChangesFromGoldenAge
	(TraitType, ResourceType, YieldType, Yield, YieldCap)
VALUES
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 'YIELD_SCIENCE', 1, 5),
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 'YIELD_CULTURE', 1, 5),
	('TRAIT_WONDER_BUILDER', 'RESOURCE_ARTIFACTS', 'YIELD_FAITH', 1, 5);

----------------------------------------------------------
-- Unique Unit: War Chariot (Chariot Archer)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_THE_WHEEL',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CHARIOT_ARCHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1
WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_EGYPTIAN_WARCHARIOT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_EGYPTIAN_WARCHARIOT', 'PROMOTION_GIFT_OF_THE_PHARAOH');

----------------------------------------------------------
-- Unique Unit: Mamluk (Lancer)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_EGYPT', 'UNITCLASS_LANCER', 'UNIT_MAMLUK');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LANCER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LANCER') + 1,
	ProhibitsSpread = 1
WHERE Type = 'UNIT_MAMLUK';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MAMLUK', 'PROMOTION_VUR_HA');

----------------------------------------------------------
-- Unique Improvement: Obelisk
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_MINING',
	Time = 600
WHERE Type = 'BUILD_OBELISK';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_OBELISK');

INSERT INTO BuildFeatures
	(BuildType, FeatureType, Time, Remove)
VALUES
	('BUILD_OBELISK', 'FEATURE_FOREST', 400, 1),
	('BUILD_OBELISK', 'FEATURE_JUNGLE', 500, 1),
	('BUILD_OBELISK', 'FEATURE_MARSH', 500, 1);

INSERT INTO Improvement_ResourceTypes
	(ImprovementType, ResourceType)
VALUES
	('IMPROVEMENT_OBELISK', 'RESOURCE_ARTIFACTS'),
	('IMPROVEMENT_OBELISK', 'RESOURCE_HIDDEN_ARTIFACTS');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_OBELISK', 'YIELD_SCIENCE', 1),
	('IMPROVEMENT_OBELISK', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_OBELISK', 'YIELD_FAITH', 1),
	('IMPROVEMENT_OBELISK', 'YIELD_GOLDEN_AGE_POINTS', 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_OBELISK', 'TECH_ENGINEERING', 'YIELD_GOLDEN_AGE_POINTS', 3),
	('IMPROVEMENT_OBELISK', 'TECH_CIVIL_SERVICE', 'YIELD_FAITH', 2),
	('IMPROVEMENT_OBELISK', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_OBELISK', 'TECH_ARCHAEOLOGY', 'YIELD_SCIENCE', 2);

----------------------------------------------------------
-- Unique Building: Nilometer (Water Mill)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_EGYPT', 'BUILDINGCLASS_WATERMILL', 'BUILDING_NILOMETER'),
	('CIVILIZATION_EGYPT', 'BUILDINGCLASS_WELL', NULL);

UPDATE Buildings
SET River = 0
WHERE Type = 'BUILDING_NILOMETER';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_WATERMILL' AND YieldType = 'YIELD_FOOD') + 1
WHERE BuildingType = 'BUILDING_NILOMETER' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_NILOMETER', 'YIELD_PRODUCTION', 3),
	('BUILDING_NILOMETER', 'YIELD_CULTURE', 2);

UPDATE Building_YieldChangesPerPop
SET Yield = (SELECT Yield FROM Building_YieldChangesPerPop WHERE BuildingType = 'BUILDING_WATERMILL' AND YieldType = 'YIELD_PRODUCTION') * 8 / 5
WHERE BuildingType = 'BUILDING_NILOMETER';

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NILOMETER', 'YIELD_FOOD', 10);
