----------------------------------------------------------
-- Unique Ability: River Warlord
----------------------------------------------------------

----------------------------------------------------------
-- Unique Unit: Mandekalu Cavalry (Knight)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 1
WHERE Type = 'UNIT_SONGHAI_MUSLIMCAVALRY';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_SONGHAI_MUSLIMCAVALRY' AND PromotionType = 'PROMOTION_CITY_PENALTY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SONGHAI_MUSLIMCAVALRY', 'PROMOTION_RAIDER');

----------------------------------------------------------
-- Unique Building: Tabya (Stone Works)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_STONE_WORKS'
WHERE BuildingType = 'BUILDING_MUD_PYRAMID_MOSQUE';

UPDATE Buildings
SET BuildingProductionModifier = 10
WHERE Type = 'BUILDING_MUD_PYRAMID_MOSQUE';

DELETE FROM Building_LocalResourceOrs
WHERE BuildingType = 'BUILDING_MUD_PYRAMID_MOSQUE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUD_PYRAMID_MOSQUE', 'YIELD_CULTURE', 1);

INSERT INTO Building_RiverPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUD_PYRAMID_MOSQUE', 'YIELD_PRODUCTION', 1);
