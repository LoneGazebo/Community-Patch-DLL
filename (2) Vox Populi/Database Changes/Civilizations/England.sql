----------------------------------------------------------
-- Unique Ability: Sun Never Sets
----------------------------------------------------------
UPDATE Traits
SET
	ExtraSpies = 0,
	ExtraEmbarkMoves = 0,
	NavalUnitMaintenanceModifier = -25
WHERE Type = 'TRAIT_OCEAN_MOVEMENT';

UPDATE Trait_FreePromotionUnitCombats SET PromotionType = 'PROMOTION_SUN_NEVER_SETS' WHERE TraitType = 'TRAIT_OCEAN_MOVEMENT';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_OCEAN_MOVEMENT', 'UNITCOMBAT_DIPLOMACY', 'PROMOTION_RIVAL_TERRITORY');

INSERT INTO Trait_FreePromotionUnitClass
	(TraitType, UnitClassType, PromotionType)
VALUES
	('TRAIT_OCEAN_MOVEMENT', 'UNITCLASS_ARCHAEOLOGIST', 'PROMOTION_RIVAL_TERRITORY');

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
-- Unique Unit: Redcoat (Fusilier)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ENGLAND', 'UNITCLASS_RIFLEMAN', 'UNIT_REDCOAT');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_RIFLEMAN'
			)
		)
	)
WHERE Type = 'UNIT_REDCOAT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_REDCOAT', 'PROMOTION_SUN_NEVER_SETS'),
	('UNIT_REDCOAT', 'PROMOTION_RULE_BRITANNIA');

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
	ExtraSpies = 1,
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
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 40),
	('BUILDING_WHITE_TOWER', 'YIELD_GREAT_ADMIRAL_POINTS', 40);

INSERT INTO Building_YieldFromSpyAttack
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 40),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 40),
	('BUILDING_WHITE_TOWER', 'YIELD_GREAT_ADMIRAL_POINTS', 40);

INSERT INTO Building_YieldFromSpyRigElection
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WHITE_TOWER', 'YIELD_GOLD', 25),
	('BUILDING_WHITE_TOWER', 'YIELD_SCIENCE', 25),
	('BUILDING_WHITE_TOWER', 'YIELD_GREAT_ADMIRAL_POINTS', 25);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_ARMORY', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CASTLE', 'YIELD_CULTURE', 1),
	('BUILDING_WHITE_TOWER', 'BUILDINGCLASS_CONSTABLE', 'YIELD_CULTURE', 1);

----------------------------------------------------------
-- Unique Building: Steam Mill (Factory)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ENGLAND', 'BUILDINGCLASS_FACTORY', 'BUILDING_STEAM_MILL');

UPDATE Buildings
SET
	PrereqTech = 'TECH_STEAM_POWER',
	SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_FACTORY') + 1,
	NoUnhappfromXSpecialists = (SELECT NoUnhappfromXSpecialists FROM Buildings WHERE Type = 'BUILDING_FACTORY') + 1
WHERE Type = 'BUILDING_STEAM_MILL';

DELETE FROM Building_ResourceQuantityRequirements WHERE BuildingType = 'BUILDING_STEAM_MILL';

UPDATE Building_YieldChangesPerPop
SET Yield = (SELECT Yield FROM Building_YieldChangesPerPop WHERE BuildingType = 'BUILDING_FACTORY') * 2
WHERE BuildingType = 'BUILDING_STEAM_MILL';

UPDATE Building_ImprovementYieldChanges
SET Yield = (SELECT Yield FROM Building_ImprovementYieldChanges WHERE BuildingType = 'BUILDING_FACTORY') + 2
WHERE BuildingType = 'BUILDING_STEAM_MILL';

UPDATE Building_ResourceYieldChanges
SET Yield = (SELECT Yield FROM Building_ResourceYieldChanges a WHERE a.BuildingType = 'BUILDING_FACTORY' AND a.YieldType = Building_ResourceYieldChanges.YieldType) + 1
WHERE BuildingType = 'BUILDING_STEAM_MILL';

INSERT INTO Building_GreatPersonPointFromConstruction
	(BuildingType, GreatPersonType, EraType, Value)
VALUES
	('BUILDING_STEAM_MILL', 'GREATPERSON_ENGINEER', 'ERA_INDUSTRIAL', 10);
