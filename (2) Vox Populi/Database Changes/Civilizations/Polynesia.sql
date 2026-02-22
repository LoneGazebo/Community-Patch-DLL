----------------------------------------------------------
-- Unique Ability: Wayfinding
----------------------------------------------------------
UPDATE Traits
SET
	CombatBonusImprovement = NULL,
	NearbyImprovementBonusRange = 0,
	NearbyImprovementCombatBonus = 0,
	NoConnectionUnhappiness = 1
WHERE Type = 'TRAIT_WAYFINDING';

INSERT INTO Trait_ImprovementYieldChanges
	(TraitType, ImprovementType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_PRODUCTION', 2);

INSERT INTO Trait_FeatureYieldChanges
	(TraitType, FeatureType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'FEATURE_ATOLL', 'YIELD_PRODUCTION', 2);

INSERT INTO Trait_BuildsUnitClasses
	(TraitType, UnitClassType, BuildType)
SELECT DISTINCT
	'TRAIT_WAYFINDING', Class, 'BUILD_FISHING_BOATS_EMBARKED'
FROM Units
WHERE CombatClass = 'UNITCOMBAT_MELEE' OR CombatClass = 'UNITCOMBAT_GUN';

----------------------------------------------------------
-- Unique Unit: Maori Warrior (Pikeman)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_PIKEMAN' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_PIKEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_PIKEMAN') + 5
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_POLYNESIAN_MAORI_WARRIOR', 'PROMOTION_HAKA_WAR_DANCE');

----------------------------------------------------------
-- Unique Unit: Vaka Nui (Galley)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_POLYNESIA', 'UNITCLASS_GALLEY', 'UNIT_VAKA_NUI');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_VP_GALLEY'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_VP_GALLEY') + 2,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_VP_GALLEY') + 1
WHERE Type = 'UNIT_VAKA_NUI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_VAKA_NUI', 'PROMOTION_SUPPLY');

----------------------------------------------------------
-- Unique Improvement: Moai
----------------------------------------------------------
UPDATE Builds
SET PrereqTech = 'TECH_MASONRY' -- Construction
WHERE Type = 'BUILD_MOAI';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
--	('IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MOAI', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MOAI', 'YIELD_CULTURE_LOCAL', 2);

INSERT INTO Improvement_AdjacentCityYields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
--	('IMPROVEMENT_MOAI', 'IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1, 1),
	('IMPROVEMENT_MOAI', 'IMPROVEMENT_ACADEMY', 'YIELD_CULTURE', 1, 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'TECH_ENGINEERING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MOAI', 'TECH_ASTRONOMY', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MOAI', 'TECH_ASTRONOMY', 'YIELD_CULTURE_LOCAL', -1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHITECTURE', 'YIELD_CULTURE_LOCAL', -1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHAEOLOGY', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHAEOLOGY', 'YIELD_TOURISM', 1),
	('IMPROVEMENT_MOAI', 'TECH_REFRIGERATION', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MOAI', 'TECH_REFRIGERATION', 'YIELD_TOURISM', 1);

----------------------------------------------------------
-- Unique Building: Marae (Council)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_POLYNESIA', 'BUILDINGCLASS_COUNCIL', 'BUILDING_MARAE');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_MARAE', 'YIELD_SCIENCE', 1),
	('BUILDING_MARAE', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MARAE', 'YIELD_FOOD', 3),
	('BUILDING_MARAE', 'YIELD_GOLD', 3),
	('BUILDING_MARAE', 'YIELD_CULTURE', 3);
