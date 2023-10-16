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
-- Unique Building: Runestone (Lighthouse)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_DENMARK', 'BUILDINGCLASS_LIGHTHOUSE', 'BUILDING_JELLING_STONES');

UPDATE Buildings
SET Water = 0
WHERE Type = 'BUILDING_JELLING_STONES';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_JELLING_STONES', 'YIELD_CULTURE', 2);

UPDATE Building_SeaPlotYieldChanges
SET Yield = (SELECT Yield FROM Building_SeaPlotYieldChanges WHERE BuildingType = 'BUILDING_LIGHTHOUSE' AND YieldType = 'YIELD_FOOD') + 1
WHERE BuildingType = 'BUILDING_JELLING_STONES' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldFromPillage
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_JELLING_STONES', 'YIELD_CULTURE', 30);

INSERT INTO Building_UnitCombatProductionModifiers
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_JELLING_STONES', 'UNITCOMBAT_MELEE', 25),
	('BUILDING_JELLING_STONES', 'UNITCOMBAT_GUN', 25),
	('BUILDING_JELLING_STONES', 'UNITCOMBAT_NAVALMELEE', 25);
