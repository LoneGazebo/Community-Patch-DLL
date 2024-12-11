----------------------------------------------------------
-- Unique Ability: Treasures of Nineveh
----------------------------------------------------------
INSERT INTO Trait_GreatWorkYieldChanges
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_SLAYER_OF_TIAMAT', 'YIELD_SCIENCE', 3);

----------------------------------------------------------
-- Unique Unit: Siege Tower
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides
SET UnitClassType = 'UNITCLASS_SIEGE_TOWER'
WHERE UnitType = 'UNIT_ASSYRIAN_SIEGE_TOWER';

UPDATE Units
SET
	CombatClass = NULL,
	DefaultUnitAI = 'UNITAI_CITY_SPECIAL',
	PrereqTech = 'TECH_ARCHERY', -- Military Strategy
	ObsoleteTech = NULL
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'PROMOTION_SAPPER'),
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'PROMOTION_MEDIC'),
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'PROMOTION_MEDIC_II');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_ASSYRIAN_SIEGE_TOWER', 'UNITAI_CITY_SPECIAL');

----------------------------------------------------------
-- Unique Unit: Iron Chariot
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ASSYRIA', 'UNITCLASS_IRON_CHARIOT', 'UNIT_IRON_CHARIOT');

UPDATE Units SET Combat = 17 WHERE Type = 'UNIT_IRON_CHARIOT';

INSERT INTO Unit_ResourceQuantityRequirements
	(UnitType, ResourceType, Cost)
VALUES
	('UNIT_IRON_CHARIOT', 'RESOURCE_IRON', 1);

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_IRON_CHARIOT', 'UNITCLASS_KNIGHT');

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_IRON_CHARIOT', 'PROMOTION_BEAM_AXLE'),
	('UNIT_IRON_CHARIOT', 'PROMOTION_SHOCK_CAVALRY');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_IRON_CHARIOT', 'UNITAI_DEFENSE'),
	('UNIT_IRON_CHARIOT', 'UNITAI_FAST_ATTACK');

----------------------------------------------------------
-- Unique Building: Royal Library (School of Philosophy)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_NATIONAL_COLLEGE'
WHERE CivilizationType = 'CIVILIZATION_ASSYRIA';

DELETE FROM Building_ClassesNeededInCity
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY';

UPDATE Buildings
SET
	PrereqTech = 'TECH_WRITING',
	ThemingBonusHelp = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_THEMING_BONUS_HELP',
	FreeBuildingThisCity = 'BUILDINGCLASS_LIBRARY',
	FreeGreatWork = 'GREAT_WORK_FLOOD_TABLET',
	GreatWorkCount = 3 -- 1
WHERE Type = 'BUILDING_ROYAL_LIBRARY';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_NATIONAL_COLLEGE' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'YIELD_SCIENCE', 50);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'BUILDINGCLASS_LIBRARY', 'YIELD_SCIENCE', 3);

INSERT INTO Building_DomainFreeExperiencePerGreatWorkGlobal
	(BuildingType, DomainType, Experience)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_LAND', 5),
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_SEA', 5),
	('BUILDING_ROYAL_LIBRARY', 'DOMAIN_AIR', 5);

INSERT INTO Building_ThemingBonuses
	(BuildingType, Description, RequiresAnyButOwner)
VALUES
	('BUILDING_ROYAL_LIBRARY', 'TXT_KEY_THEMING_BONUS_ROYAL_LIBRARY', 1);

----------------------------------------------------------
-- Unique Building: Lamassu Gate (Walls)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ASSYRIA', 'BUILDINGCLASS_WALLS', 'BUILDING_LAMASSU_GATE');

UPDATE Buildings
SET
	RangedStrikeModifier = 10,
	CitySupplyModifier = (SELECT CitySupplyModifier FROM Buildings WHERE Type = 'BUILDING_WALLS') + 5
WHERE Type = 'BUILDING_LAMASSU_GATE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LAMASSU_GATE', 'YIELD_FAITH', 1);

INSERT INTO Building_YieldFromUnitProduction
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_LAMASSU_GATE', 'YIELD_FAITH', 15);
