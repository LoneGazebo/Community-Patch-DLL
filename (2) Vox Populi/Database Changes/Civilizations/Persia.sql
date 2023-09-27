----------------------------------------------------------
-- Unique Ability: Achaemenid Legacy
----------------------------------------------------------
UPDATE Traits
SET
	GoldToGAP = 15,
	GoldenAgeCombatModifier = 15
WHERE Type = 'TRAIT_ENHANCED_GOLDEN_AGES';

----------------------------------------------------------
-- Unique Unit: Immortal (Spearman)
----------------------------------------------------------
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
WHERE Type = 'UNIT_PERSIAN_IMMORTAL';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PERSIAN_IMMORTAL', 'PROMOTION_FASTER_HEAL'),
	('UNIT_PERSIAN_IMMORTAL', 'PROMOTION_ARMOR_PLATING_1');

----------------------------------------------------------
-- Unique Building: Satrap's Court (Courthouse)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_COURTHOUSE'
WHERE BuildingType = 'BUILDING_SATRAPS_COURT';

UPDATE Buildings
SET
	BuildAnywhere = 1,
	Happiness = 1,
	SpecialistType = 'SPECIALIST_MERCHANT',
	SpecialistCount = 1
WHERE Type = 'BUILDING_SATRAPS_COURT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'YIELD_GOLD', 34);

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
VALUES
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_SCIENTIST', 'YIELD_GOLD', 1),
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_MERCHANT', 'YIELD_GOLD', 1),
	('BUILDING_SATRAPS_COURT', 'SPECIALIST_ENGINEER', 'YIELD_GOLD', 1);
