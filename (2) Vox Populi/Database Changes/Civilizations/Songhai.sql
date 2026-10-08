----------------------------------------------------------
-- Unique Ability: River Warlord
----------------------------------------------------------
DELETE FROM Trait_FreePromotionUnitCombats WHERE TraitType = 'TRAIT_AMPHIB_WARLORD';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, PromotionType, UnitCombatType)
SELECT
	'TRAIT_AMPHIB_WARLORD', 'PROMOTION_SEWN_CANOES', Type
FROM UnitCombatInfos WHERE IsNaval = 0 AND IsAerial = 0;

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
-- Unique Unit: Sofa (Crossbowman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_SONGHAI', 'UNITCLASS_CROSSBOWMAN', 'UNIT_SOFA');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CROSSBOWMAN'
			)
		)
	),
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN') + 1
WHERE Type = 'UNIT_SOFA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SOFA', 'PROMOTION_MEDIC'),
	('UNIT_SOFA', 'PROMOTION_FATHER_OF_THE_HORSE');

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

----------------------------------------------------------
-- Unique Building: Gumey (Caravansary)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SONGHAI', 'BUILDINGCLASS_CARAVANSARY', 'BUILDING_GUMEY');

UPDATE Buildings
SET
	TradeRouteLandDistanceModifier = (SELECT TradeRouteLandDistanceModifier FROM Buildings WHERE Type = 'BUILDING_CARAVANSARY') + 25,
	TradeRouteRecipientBonus = 2,
	TradeRouteTargetBonus = 2
WHERE Type = 'BUILDING_GUMEY';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_CARAVANSARY' AND YieldType = 'YIELD_GOLD') + 1
WHERE BuildingType = 'BUILDING_GUMEY' AND YieldType = 'YIELD_GOLD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GUMEY', 'YIELD_PRODUCTION', 2),
	('BUILDING_GUMEY', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesFromPassingTR
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GUMEY', 'YIELD_PRODUCTION', 2),
	('BUILDING_GUMEY', 'YIELD_GOLD', 2),
	('BUILDING_GUMEY', 'YIELD_CULTURE', 2);

INSERT INTO Building_LuxuryYieldChanges
	(BuildingType, YIeldType, Yield)
VALUES
	('BUILDING_GUMEY', 'YIELD_GOLD', 1);
