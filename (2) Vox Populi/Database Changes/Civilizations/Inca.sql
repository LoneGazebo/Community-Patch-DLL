----------------------------------------------------------
-- Unique Ability: Great Andean Road
----------------------------------------------------------
UPDATE Traits
SET
	ImprovementMaintenanceModifier = 0,
	NoHillsImprovementMaintenance = 0,
	MountainPass = 1
WHERE Type = 'TRAIT_GREAT_ANDEAN_ROAD';

INSERT INTO Trait_MountainRangeYield
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_FOOD', '1'),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_GOLD', '1'),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_SCIENCE', '1');

----------------------------------------------------------
-- Unique Unit: Waraq'Ak (Slinger)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_VP_SLINGER' WHERE UnitType = 'UNIT_INCAN_SLINGER';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_VP_SLINGER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_VP_SLINGER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_VP_SLINGER') + 1,
	"Range" = 2
WHERE Type = 'UNIT_INCAN_SLINGER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_INCAN_SLINGER', 'PROMOTION_SLINGER');

----------------------------------------------------------
-- Unique Improvement: Pata-Pata
----------------------------------------------------------
UPDATE Builds
SET
	Help = 'TXT_KEY_BUILD_TERRACE_FARM_HELP',
	Recommendation = 'TXT_KEY_BUILD_TERRACE_FARM_REC'
WHERE Type = 'BUILD_TERRACE_FARM';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
--	('IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'YIELD_PRODUCTION', 2);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_FARM', 'IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1, 1),
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1, 1),
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_MANUFACTORY', 'YIELD_FOOD', 1, 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'TECH_CIVIL_SERVICE', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_ROBOTICS', 'YIELD_FOOD', 3);
