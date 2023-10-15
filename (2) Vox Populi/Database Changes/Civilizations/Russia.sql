----------------------------------------------------------
-- Unique Ability: Siberian Riches
----------------------------------------------------------
UPDATE Traits SET PlotCultureCostModifier = -33 WHERE Type = 'TRAIT_STRATEGIC_RICHES';

DELETE FROM Trait_YieldChangesStrategicResources WHERE TraitType = 'TRAIT_STRATEGIC_RICHES';

INSERT INTO Trait_ResourceQuantityModifiers
	(TraitType, ResourceType, ResourceQuantityModifier)
VALUES
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_HORSE', 100),
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_IRON', 100),
--	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_URANIUM', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_COAL', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_OIL', 100),
	('TRAIT_STRATEGIC_RICHES', 'RESOURCE_ALUMINUM', 100);

INSERT INTO Trait_YieldFromTileEarn
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_STRATEGIC_RICHES', 'YIELD_SCIENCE', 20);

----------------------------------------------------------
-- Unique Unit: Cossack (Cavalry)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CAVALRY'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY') + 4,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CAVALRY') + 6
WHERE Type = 'UNIT_RUSSIAN_COSSACK';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_RUSSIAN_COSSACK', 'PROMOTION_STRONGER_VS_DAMAGED'),
	('UNIT_RUSSIAN_COSSACK', 'PROMOTION_ESPRIT_DE_CORPS');

----------------------------------------------------------
-- Unique Building: Ostrog (Bastion Fort)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_FORTRESS'
WHERE BuildingType = 'BUILDING_KREPOST';

UPDATE Buildings
SET
	Defense = (SELECT Defense FROM Buildings WHERE Type = 'BUILDING_FORTRESS') + 200,
	BorderObstacleCity = 1
WHERE Type = 'BUILDING_KREPOST';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KREPOST', 'YIELD_PRODUCTION', 3);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_KREPOST', 'IMPROVEMENT_CAMP', 'YIELD_PRODUCTION', 1),
	('BUILDING_KREPOST', 'IMPROVEMENT_CAMP', 'YIELD_GOLD', 1),
	('BUILDING_KREPOST', 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 1),
	('BUILDING_KREPOST', 'IMPROVEMENT_MINE', 'YIELD_GOLD', 1),
	('BUILDING_KREPOST', 'IMPROVEMENT_LUMBERMILL', 'YIELD_PRODUCTION', 1),
	('BUILDING_KREPOST', 'IMPROVEMENT_LUMBERMILL', 'YIELD_GOLD', 1);

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD');

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
SELECT
	'BUILDING_KREPOST', a.Type, b.YieldType, 1
FROM Resources a, Helper b
WHERE a.ResourceUsage = 1 AND a.type <> 'RESOURCE_PAPER';

DROP TABLE Helper;
