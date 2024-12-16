----------------------------------------------------------
-- Unique Ability: Carnival
----------------------------------------------------------
UPDATE Traits
SET
	GoldenAgeTourismModifier = 0,
	GoldenAgeGreatWriterRateModifier = 0,
	GoldenAgeGreatArtistRateModifier = 0,
	GoldenAgeGreatMusicianRateModifier = 0,
	WLTKDUnhappinessNeedsMod = -50,
	WLTKDFromGATurns = 10
WHERE Type = 'TRAIT_CARNIVAL';

INSERT INTO Trait_GAPToYield
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CARNIVAL', 'YIELD_GOLD', 30),
	('TRAIT_CARNIVAL', 'YIELD_TOURISM', 30);

----------------------------------------------------------
-- Unique Unit: Bandeirante (Explorer)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_BRAZIL', 'UNITCLASS_EXPLORER', 'UNIT_BANDEIRANTE');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_EXPLORER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_EXPLORER') + 3,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_EXPLORER') + 1,
	WorkRate = 100
WHERE Type = 'UNIT_BANDEIRANTE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BANDEIRANTE', 'PROMOTION_SURVIVALISM_3'),
	('UNIT_BANDEIRANTE', 'PROMOTION_FLAG_BEARER');

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_BANDEIRANTE', 'BUILD_MINE'),
	('UNIT_BANDEIRANTE', 'BUILD_QUARRY'),
	('UNIT_BANDEIRANTE', 'BUILD_FORT'),
	('UNIT_BANDEIRANTE', 'BUILD_BRAZILWOOD_CAMP');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_BANDEIRANTE', 'UNITAI_WORKER');

----------------------------------------------------------
-- Unique Unit: Amazonas (Ironclad)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_BRAZIL', 'UNITCLASS_IRONCLAD', 'UNIT_AMAZONAS');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_IRONCLAD'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_IRONCLAD') + 3
WHERE Type = 'UNIT_AMAZONAS';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AMAZONAS', 'PROMOTION_BLITZ'),
	('UNIT_AMAZONAS', 'PROMOTION_BREACHER'),
	('UNIT_AMAZONAS', 'PROMOTION_RIACHUELO');

----------------------------------------------------------
-- Unique Improvement: Brazilwood Camp
----------------------------------------------------------
UPDATE Builds
SET PrereqTech = 'TECH_CALENDAR'
WHERE Type = 'BUILD_BRAZILWOOD_CAMP';

UPDATE Improvements
SET
	NoTwoAdjacent = 1,
	NoFreshWater = 1,
	ImprovementResource = 'RESOURCE_BRAZILWOOD',
	ImprovementResourceQuantity = 1
WHERE Type = 'IMPROVEMENT_BRAZILWOOD_CAMP';

INSERT INTO Improvement_ValidFeatures
	(ImprovementType, FeatureType)
VALUES
--	('IMPROVEMENT_BRAZILWOOD_CAMP', 'FEATURE_JUNGLE'),
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'FEATURE_FOREST');

-- Allow Brazilwood Camp to be built on any Brazilwood resource, if something spawns them on the map
INSERT INTO Improvement_ResourceTypes
	(ImprovementType, ResourceType)
VALUES
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'RESOURCE_BRAZILWOOD');

UPDATE Improvement_Yields SET Yield = 1 WHERE ImprovementType = 'IMPROVEMENT_BRAZILWOOD_CAMP'; -- Gold

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'TECH_PHYSICS', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'TECH_ACOUSTICS', 'YIELD_CULTURE', 2),
	('IMPROVEMENT_BRAZILWOOD_CAMP', 'TECH_RADIO', 'YIELD_CULTURE', 2);

----------------------------------------------------------
-- Unique Building: Embrapa (Research Lab)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_BRAZIL', 'BUILDINGCLASS_LABORATORY', 'BUILDING_EMBRAPA');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_EMBRAPA', 'YIELD_GOLD', 2),
	('BUILDING_EMBRAPA', 'YIELD_TOURISM', 2);

CREATE TEMP TABLE Helper (
	Type TEXT
);

CREATE TEMP TABLE Helper2 (
	YieldType TEXT,
	Yield INTEGER
);

INSERT INTO Helper
VALUES
	('RESOURCE_BANANA'),
	('RESOURCE_WHEAT'),
	('RESOURCE_RICE'),
	('RESOURCE_MAIZE');

INSERT INTO Helper2
VALUES
	('YIELD_GOLD', 2),
	('YIELD_SCIENCE', 2),
	('YIELD_CULTURE', 2);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
SELECT
	'BUILDING_EMBRAPA', a.Type, b.YieldType, b.Yield
FROM Helper a, Helper2 b;

DELETE FROM Helper;

INSERT INTO Helper
VALUES
	('IMPROVEMENT_PASTURE'),
	('IMPROVEMENT_BRAZILWOOD_CAMP');

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
SELECT
	'BUILDING_EMBRAPA', a.Type, b.YieldType, b.Yield
FROM Helper a, Helper2 b;

DROP TABLE Helper;
DROP TABLE Helper2;

INSERT INTO Building_FeatureYieldChanges
	(BuildingType, FeatureType, YieldType, Yield)
VALUES
	('BUILDING_EMBRAPA', 'FEATURE_FOREST', 'YIELD_SCIENCE', 1),
	('BUILDING_EMBRAPA', 'FEATURE_FOREST', 'YIELD_TOURISM', 1),
	('BUILDING_EMBRAPA', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 1),
	('BUILDING_EMBRAPA', 'FEATURE_JUNGLE', 'YIELD_TOURISM', 1);

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_EMBRAPA', 'YIELD_FOOD', 'YIELD_TOURISM', 5);
