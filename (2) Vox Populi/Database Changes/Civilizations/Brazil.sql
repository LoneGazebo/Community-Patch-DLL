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
-- Unique Unit: Bandeirantes (Explorer)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_BRAZIL', 'UNITCLASS_EXPLORER', 'UNIT_BANDEIRANTES');

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
WHERE Type = 'UNIT_BANDEIRANTES';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BANDEIRANTES', 'PROMOTION_SURVIVALISM_3'),
	('UNIT_BANDEIRANTES', 'PROMOTION_RECON_BANDEIRANTES');

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_BANDEIRANTES', 'BUILD_MINE'),
	('UNIT_BANDEIRANTES', 'BUILD_QUARRY'),
	('UNIT_BANDEIRANTES', 'BUILD_FORT'),
	('UNIT_BANDEIRANTES', 'BUILD_BRAZILWOOD_CAMP');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_BANDEIRANTES', 'UNITAI_WORKER');

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
