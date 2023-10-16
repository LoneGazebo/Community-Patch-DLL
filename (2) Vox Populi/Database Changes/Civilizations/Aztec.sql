----------------------------------------------------------
-- Unique Ability: Sacrificial Captives
----------------------------------------------------------
UPDATE Traits
SET
	CultureFromKills = 0,
	GoldenAgeFromVictory = 10
WHERE Type = 'TRAIT_CULTURE_FROM_KILLS';

INSERT INTO Trait_YieldFromKills
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_GOLD', 150),
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_FAITH', 150);

----------------------------------------------------------
-- Unique Unit: Jaguar (Warrior)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_WARRIOR'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_WARRIOR') + 2
WHERE Type = 'UNIT_AZTEC_JAGUAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_JAGUAR'),
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_WOODSMAN'),
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY');

----------------------------------------------------------
-- Unique Building: Floating Gardens (Water Mill)
----------------------------------------------------------

-- Can't build Well
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_AZTEC', 'BUILDINGCLASS_WELL', NULL);

UPDATE Buildings SET River = 0 WHERE Type = 'BUILDING_FLOATING_GARDENS';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_WATERMILL' AND YieldType = 'YIELD_FOOD') + 1
WHERE BuildingType = 'BUILDING_FLOATING_GARDENS' AND YieldType = 'YIELD_FOOD';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLOATING_GARDENS', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_FLOATING_GARDENS', 'YIELD_PRODUCTION', 25),
	('BUILDING_FLOATING_GARDENS', 'YIELD_FOOD', 25);

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLOATING_GARDENS', 'YIELD_FOOD', 10);

INSERT INTO Building_LakePlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLOATING_GARDENS', 'YIELD_FOOD', 2);

INSERT INTO Building_RiverPlotYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLOATING_GARDENS', 'YIELD_FOOD', 1);
