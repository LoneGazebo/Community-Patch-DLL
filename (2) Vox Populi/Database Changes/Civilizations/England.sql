----------------------------------------------------------
-- Unique Ability: Perfidious Albion
----------------------------------------------------------
UPDATE Traits
SET
	ExtraSpies = 0,
	ExtraEmbarkMoves = 1,
	StartingSpies = 1,
	NavalUnitMaintenanceModifier = -25,
	SpySecurityModifier = 15,
	SpyMoveRateModifier = 3,
	SpyOffensiveStrengthModifier = 25
WHERE Type = 'TRAIT_OCEAN_MOVEMENT';

----------------------------------------------------------
-- Unique Unit: Ship of the Line (Frigate)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_FRIGATE'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_FRIGATE') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_FRIGATE') + 3,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_FRIGATE') + 1
WHERE Type = 'UNIT_ENGLISH_SHIPOFTHELINE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ENGLISH_SHIPOFTHELINE', 'PROMOTION_SPLASH_1'),
	('UNIT_ENGLISH_SHIPOFTHELINE', 'PROMOTION_SPLASH_2');

----------------------------------------------------------
-- Unique Building: White Tower (Ironworks)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ENGLAND', 'BUILDINGCLASS_IRONWORKS', 'BUILDING_WHITE_TOWER');

UPDATE Buildings
SET
	Defense = 300,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 1,
	FreeGreatWork = 'GREAT_WORK_THE_CROWN_JEWELS'
WHERE Type = 'BUILDING_WHITE_TOWER';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 2),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 2),
	('BUILDING_WHITE_TOWER', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldFromConstruction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 25);

INSERT INTO Building_YieldFromSpyDefenseOrID
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 40),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 40);

INSERT INTO Building_YieldFromSpyAttack
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 40),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 40);

INSERT INTO Building_YieldFromSpyRigElection
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 25),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 25);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_ARMORY', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CASTLE', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CONSTABLE', 'YIELD_CULTURE', 1);
