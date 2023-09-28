----------------------------------------------------------
-- Unique Ability: Iklwa
----------------------------------------------------------
UPDATE Traits
SET
	IgnoreBullyPenalties = 1,
	CSBullyMilitaryStrengthModifier = 50
WHERE Type = 'TRAIT_BUFFALO_HORNS';

INSERT INTO Trait_MaintenanceModifierUnitCombats
	(TraitType, UnitCombatType, MaintenanceModifier)
VALUES
--	('TRAIT_BUFFALO_HORNS', 'UNITCOMBAT_MELEE', -50),
	('TRAIT_BUFFALO_HORNS', 'UNITCOMBAT_GUN', -50);

----------------------------------------------------------
-- Unique Unit: Impi (Tercio)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_ZULU_IMPI';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 3
WHERE Type = 'UNIT_ZULU_IMPI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ZULU_IMPI', 'PROMOTION_KNOCKOUT'),
	('UNIT_ZULU_IMPI', 'PROMOTION_RANGED_SUPPORT_FIRE');

----------------------------------------------------------
-- Unique Building: Ikanda (Barracks)
----------------------------------------------------------
UPDATE Buildings
SET
	PrereqTech = 'TECH_BRONZE_WORKING',
	TrainedFreePromotion = 'PROMOTION_IKLWA',
	CitySupplyFlat = (SELECT CitySupplyFlat FROM Buildings WHERE Type = 'BUILDING_BARRACKS') + 1
WHERE Type = 'BUILDING_IKANDA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_IKANDA', 'YIELD_CULTURE', 1);
