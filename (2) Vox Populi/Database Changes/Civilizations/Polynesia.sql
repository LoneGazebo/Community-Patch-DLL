----------------------------------------------------------
-- Unique Ability: Wayfinding
----------------------------------------------------------
UPDATE Traits
SET
	NearbyImprovementBonusRange = 3,
	NearbyImprovementCombatBonus = 20,
	NoConnectionUnhappiness = 1
WHERE Type = 'TRAIT_WAYFINDING';

INSERT INTO Trait_ImprovementYieldChanges
	(TraitType, ImprovementType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_FOOD', 2);

INSERT INTO Trait_FeatureYieldChanges
	(TraitType, FeatureType, YieldType, Yield)
VALUES
	('TRAIT_WAYFINDING', 'FEATURE_ATOLL', 'YIELD_FOOD', 2);

INSERT INTO Trait_BuildsUnitClasses
	(TraitType, UnitClassType, BuildType)
SELECT DISTINCT
	'TRAIT_WAYFINDING', Class, 'BUILD_POLYNESIAN_BOATS'
FROM Units
WHERE CombatClass = 'UNITCOMBAT_MELEE' OR CombatClass = 'UNITCOMBAT_GUN';

----------------------------------------------------------
-- Unique Unit: Maori Warrior (Longswordsman)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_LONGSWORDSMAN' WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

UPDATE Units
SET
	PrereqTech = 'TECH_CHIVALRY',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	)
WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_POLYNESIAN_MAORI_WARRIOR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_POLYNESIAN_MAORI_WARRIOR', 'PROMOTION_HAKA_WAR_DANCE');

----------------------------------------------------------
-- Unique Improvement: Moai
----------------------------------------------------------
UPDATE Builds
SET PrereqTech = 'TECH_MASONRY' -- Construction
WHERE Type = 'BUILD_MOAI';

-- Use Improvement_YieldAdjacentSameType instead
UPDATE Improvements
SET CultureAdjacentSameType = 0
WHERE Type = 'IMPROVEMENT_MOAI';

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

INSERT INTO Improvement_YieldAdjacentSameType
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_MOAI', 'TECH_ENGINEERING', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_MOAI', 'TECH_ASTRONOMY', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MOAI', 'TECH_ASTRONOMY', 'YIELD_CULTURE_LOCAL', -1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_MOAI', 'TECH_ARCHITECTURE', 'YIELD_CULTURE_LOCAL', -1),
	('IMPROVEMENT_MOAI', 'TECH_FLIGHT', 'YIELD_GOLD', 1);
