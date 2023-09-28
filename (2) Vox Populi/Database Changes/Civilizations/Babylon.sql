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
