----------------------------------------------------------
-- Unique Ability: Mycel Hæþen Here
----------------------------------------------------------
DELETE FROM Trait_FreePromotionUnitCombats WHERE TraitType = 'TRAIT_VIKING_FURY';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_VIKING_FURY', Type, 'PROMOTION_VIKING'
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsRanged = 0 AND IsNaval = 0 AND IsAerial = 0;

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_VIKING_FURY', Type, 'PROMOTION_LONGBOAT'
FROM UnitCombatInfos
WHERE IsRanged = 0 AND IsNaval = 1;

----------------------------------------------------------
-- Unique Unit: Berserker (Pikeman)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_PIKEMAN' WHERE UnitType = 'UNIT_DANISH_BERSERKER';

UPDATE Units
SET
	PrereqTech = 'TECH_METAL_CASTING',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_PIKEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_PIKEMAN') + 2,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_PIKEMAN') + 1
WHERE Type = 'UNIT_DANISH_BERSERKER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_DANISH_BERSERKER', 'PROMOTION_AMPHIBIOUS'),
	('UNIT_DANISH_BERSERKER', 'PROMOTION_CHARGE');

----------------------------------------------------------
-- Unique Unit: Longship (Caravel)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_DENMARK', 'UNITCLASS_CARAVEL', 'UNIT_LONGSHIP');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARAVEL'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CARAVEL') + 5
WHERE Type = 'UNIT_LONGSHIP';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_LONGSHIP', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'),
	('UNIT_LONGSHIP', 'PROMOTION_PIRACY');

----------------------------------------------------------
-- Unique Building: Runestone (Lighthouse)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_DENMARK', 'BUILDINGCLASS_LIGHTHOUSE', 'BUILDING_RUNESTONE');

UPDATE Buildings
SET Water = 0
WHERE Type = 'BUILDING_RUNESTONE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RUNESTONE', 'YIELD_CULTURE', 2);

UPDATE Building_SeaPlotYieldChanges
SET Yield = (SELECT Yield FROM Building_SeaPlotYieldChanges WHERE BuildingType = 'BUILDING_LIGHTHOUSE' AND YieldType = 'YIELD_FOOD') + 1
WHERE BuildingType = 'BUILDING_RUNESTONE' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldFromPillage
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_RUNESTONE', 'YIELD_CULTURE', 30);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_RUNESTONE', 'UNITCOMBAT_MELEE', 25),
	('BUILDING_RUNESTONE', 'UNITCOMBAT_GUN', 25),
	('BUILDING_RUNESTONE', 'UNITCOMBAT_NAVALMELEE', 25);

----------------------------------------------------------
-- Unique Building: Andelsbevægelse (Agribusiness)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_DENMARK', 'BUILDINGCLASS_AGRIBUSINESS', 'BUILDING_ANDELSBEVAEGELSE');

UPDATE Buildings
SET BuildingProductionModifier = 10
WHERE Type = 'BUILDING_ANDELSBEVAEGELSE';

DELETE FROM Building_ResourceQuantityRequirements WHERE BuildingType = 'BUILDING_ANDELSBEVAEGELSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_ANDELSBEVAEGELSE', 'YIELD_FOOD', 5),
	('BUILDING_ANDELSBEVAEGELSE', 'YIELD_PRODUCTION', 3),
	('BUILDING_ANDELSBEVAEGELSE', 'YIELD_GOLD', 3);

INSERT INTO Building_ImprovementYieldChanges
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
--	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_FARM', 'YIELD_PRODUCTION', 2),
--	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_FARM', 'YIELD_GOLD', 2),
	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 2),
--	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_PASTURE', 'YIELD_PRODUCTION', 2),
--	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 2),
	('BUILDING_ANDELSBEVAEGELSE', 'IMPROVEMENT_PASTURE', 'YIELD_FOOD', 2);

INSERT INTO Building_TerrainYieldChanges
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_ANDELSBEVAEGELSE', 'TERRAIN_GRASS', 'YIELD_FOOD', 1),
	('BUILDING_ANDELSBEVAEGELSE', 'TERRAIN_PLAINS', 'YIELD_FOOD', 1);
