----------------------------------------------------------
-- Unique Ability: Ingenuity
----------------------------------------------------------
UPDATE Traits
SET InvestmentModifier = -15
WHERE Type = 'TRAIT_INGENIOUS';

----------------------------------------------------------
-- Unique Unit: Bowman (Archer)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_ARCHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_ARCHER') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_ARCHER') + 1
WHERE Type = 'UNIT_BABYLONIAN_BOWMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BABYLONIAN_BOWMAN', 'PROMOTION_STRONGER_VS_DAMAGED');

----------------------------------------------------------
-- Unique Unit: Sabum Kibitum (Spearman)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_BABYLON', 'UNITCLASS_SPEARMAN', 'UNIT_SABUM_KIBITUM');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPEARMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 1
WHERE Type = 'UNIT_SABUM_KIBITUM';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SABUM_KIBITUM', 'PROMOTION_GAIN_EXPERIENCE'),
	('UNIT_SABUM_KIBITUM', 'PROMOTION_LEGACY');

----------------------------------------------------------
-- Unique Building: Walls of Babylon (Walls)
----------------------------------------------------------
UPDATE Buildings
SET
	Defense = (SELECT Defense FROM Buildings WHERE Type = 'BUILDING_WALLS') + 200,
	ExtraCityHitPoints = (SELECT ExtraCityHitPoints FROM Buildings WHERE Type = 'BUILDING_WALLS') + 25,
	SpecialistType = 'SPECIALIST_SCIENTIST',
	GreatPeopleRateChange = 3,
	GreatScientistBeakerModifier = 5
WHERE Type = 'BUILDING_WALLS_OF_BABYLON';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WALLS_OF_BABYLON', 'YIELD_SCIENCE', 3);

----------------------------------------------------------
-- Unique Building: Etemenanki (National Monument)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_BABYLON', 'BUILDINGCLASS_NATIONAL_EPIC', 'BUILDING_ETEMENANKI');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_NATIONAL_EPIC' AND YieldType = 'YIELD_CULTURE') + 3
WHERE BuildingType = 'BUILDING_ETEMENANKI' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ETEMENANKI', 'YIELD_FOOD', 4),
	('BUILDING_ETEMENANKI', 'YIELD_FAITH', 2);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_ETEMENANKI', 'BUILDINGCLASS_MONUMENT', 'YIELD_SCIENCE', 1),
	('BUILDING_ETEMENANKI', 'BUILDINGCLASS_SHRINE', 'YIELD_FOOD', 2);

UPDATE Building_YieldFromBirth
SET Yield = (SELECT Yield FROM Building_YieldFromBirth WHERE BuildingType = 'BUILDING_NATIONAL_EPIC' AND YieldType = 'YIELD_CULTURE') * 5 / 3
WHERE BuildingType = 'BUILDING_ETEMENANKI' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldFromPurchaseGlobal
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_ETEMENANKI', 'YIELD_FOOD', 10),
	('BUILDING_ETEMENANKI', 'YIELD_SCIENCE', 10);
