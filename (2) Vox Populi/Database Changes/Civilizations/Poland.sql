----------------------------------------------------------
-- Unique Ability: Solidarity
----------------------------------------------------------
UPDATE Traits
SET
	IsOddEraScaler = 1,
	ExtraTenetsFirstAdoption = 2
WHERE Type = 'TRAIT_SOLIDARITY';

----------------------------------------------------------
-- Unique Unit: Winged Hussar (Lancer)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LANCER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LANCER') + 3,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_LANCER') + 1
WHERE Type = 'UNIT_POLISH_WINGED_HUSSAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_POLISH_WINGED_HUSSAR', 'PROMOTION_FORMATION_1'),
	('UNIT_POLISH_WINGED_HUSSAR', 'PROMOTION_HEAVY_CHARGE');

----------------------------------------------------------
-- Unique Building: Ducal Stable (Stable)
----------------------------------------------------------
DELETE FROM Building_LocalResourceOrs
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

UPDATE Building_UnitCombatProductionModifiers
SET Modifier = 50 -- 33
WHERE BuildingType = 'BUILDING_DUCAL_STABLE';

INSERT INTO Building_UnitCombatFreeExperiences
	(BuildingType, UnitCombatType, Experience)
VALUES
	('BUILDING_DUCAL_STABLE', 'UNITCOMBAT_MOUNTED', 15);

UPDATE Building_ResourceYieldChanges
SET Yield = (SELECT Yield FROM Building_ResourceYieldChanges WHERE BuildingType = 'BUILDING_STABLE' AND YieldType = 'YIELD_PRODUCTION') + 1
WHERE BuildingType = 'BUILDING_DUCAL_STABLE' AND YieldType = 'YIELD_PRODUCTION';

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_DUCAL_STABLE', 'RESOURCE_COW', 'YIELD_GOLD', 3),
	('BUILDING_DUCAL_STABLE', 'RESOURCE_SHEEP', 'YIELD_GOLD', 3),
	('BUILDING_DUCAL_STABLE', 'RESOURCE_HORSE', 'YIELD_GOLD', 3);

INSERT INTO Building_ResourceQuantity
	(BuildingType, ResourceType, Quantity)
VALUES
	('BUILDING_DUCAL_STABLE', 'RESOURCE_HORSE', 1);
