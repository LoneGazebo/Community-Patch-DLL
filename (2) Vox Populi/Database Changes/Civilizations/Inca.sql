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
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_PRODUCTION', 1),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_GOLD', 1),
	('TRAIT_GREAT_ANDEAN_ROAD', 'YIELD_SCIENCE', 1);

----------------------------------------------------------
-- Unique Unit: Warak'aq (Slinger)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_INCA', 'UNITCLASS_SLINGER', 'UNIT_WARAKAQ');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_INCAN_SLINGER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_INCAN_SLINGER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_INCAN_SLINGER') + 1,
	"Range" = 2
WHERE Type = 'UNIT_WARAKAQ';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_WARAKAQ', 'PROMOTION_CONCUSSIVE_HIT');

----------------------------------------------------------
-- Unique Unit: Inti Maceman (Longswordsman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_INCA', 'UNITCLASS_LONGSWORDSMAN', 'UNIT_INTI_MACEMAN');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN') + 1
WHERE Type = 'UNIT_INTI_MACEMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_INTI_MACEMAN', 'PROMOTION_SENTINEL_OF_THE_SUN');

----------------------------------------------------------
-- Unique Improvement: Pata-Pata
----------------------------------------------------------
UPDATE Builds
SET
	Help = 'TXT_KEY_BUILD_TERRACE_FARM_HELP',
	Recommendation = 'TXT_KEY_BUILD_TERRACE_FARM_REC'
WHERE Type = 'BUILD_TERRACE_FARM';

UPDATE Improvement_Yields SET Yield = 2 WHERE ImprovementType = 'IMPROVEMENT_TERRACE_FARM' AND YieldType = 'YIELD_FOOD';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'YIELD_PRODUCTION', 2);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_FARM', 'IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1, 2),
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 1, 2),
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_TERRACE_FARM', 'YIELD_FOOD', 1, 2),
	('IMPROVEMENT_TERRACE_FARM', 'IMPROVEMENT_ACADEMY', 'YIELD_FOOD', 1, 2);

DELETE FROM Improvement_AdjacentMountainYieldChanges WHERE ImprovementType = 'IMPROVEMENT_TERRACE_FARM';

INSERT INTO Improvement_YieldPerXAdjacentTerrain
	(ImprovementType, TerrainType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'TERRAIN_MOUNTAIN', 'YIELD_FOOD', 1, 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_TERRACE_FARM', 'TECH_CIVIL_SERVICE', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_CIVIL_SERVICE', 'YIELD_GOLD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_CHEMISTRY', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_CHEMISTRY', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_FERTILIZER', 'YIELD_FOOD', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_FERTILIZER', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_BALLISTICS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_TERRACE_FARM', 'TECH_BALLISTICS', 'YIELD_SCIENCE', 1);

----------------------------------------------------------
-- Unique Building: Qullqa (Granary)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_INCA', 'BUILDINGCLASS_GRANARY', 'BUILDING_QULLQA');

UPDATE Buildings
SET
	NoStarvationNonSpecialist = 1,
	CityConnectionTradeRouteModifier = 5
WHERE Type = 'BUILDING_QULLQA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_QULLQA', 'YIELD_FOOD', 1),
	('BUILDING_QULLQA', 'YIELD_PRODUCTION', 2);
