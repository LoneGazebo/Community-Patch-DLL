----------------------------------------------------------
-- Unique Ability: River Warlord
----------------------------------------------------------
UPDATE Traits
SET
	FasterAlongRiver = 1,
	RiverTradeRoad = 1
WHERE Type = 'TRAIT_AMPHIB_WARLORD';

DELETE FROM Trait_FreePromotionUnitCombats WHERE TraitType = 'TRAIT_AMPHIB_WARLORD' AND PromotionType = 'PROMOTION_WAR_CANOES';

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

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUD_PYRAMID_MOSQUE', 'YIELD_CULTURE', 1);

INSERT INTO Building_RiverPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_MUD_PYRAMID_MOSQUE', 'YIELD_PRODUCTION', 1);
