----------------------------------------------------------
-- Unique Ability: Hellenic League
----------------------------------------------------------
UPDATE Traits
SET AllianceCSStrength = 5
WHERE Type = 'TRAIT_CITY_STATE_FRIENDSHIP';

----------------------------------------------------------
-- Unique Unit: Hoplite (Spearman)
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
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 2
WHERE Type = 'UNIT_GREEK_HOPLITE';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GREEK_HOPLITE', 'PROMOTION_ADJACENT_BONUS'),
	('UNIT_GREEK_HOPLITE', 'PROMOTION_SPAWN_GENERALS_II');

----------------------------------------------------------
-- Unique Unit: Klepht (Commando)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_GREECE', 'UNITCLASS_COMMANDO', 'UNIT_KLEPHT');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_COMMANDO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_COMMANDO') + 2,
	DefaultUnitAI = 'UNITAI_FAST_ATTACK'
WHERE Type = 'UNIT_KLEPHT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KLEPHT', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'),
	('UNIT_KLEPHT', 'PROMOTION_PHILHELLENISM');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_KLEPHT', 'UNITAI_FAST_ATTACK');

----------------------------------------------------------
-- Unique Building: Gymnasion (Theater)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_GREECE', 'BUILDINGCLASS_AMPHITHEATER', 'BUILDING_GYMNASION');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_AMPHITHEATER' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_GYMNASION' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GYMNASION', 'YIELD_SCIENCE', 2);

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling)
VALUES
	('BUILDING_GYMNASION', 'YIELD_CULTURE_LOCAL', 5, 1);

INSERT INTO Building_YieldFromVictory
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_GYMNASION', 'YIELD_CULTURE', 25);

----------------------------------------------------------
-- Unique Building: Parthenon (Scrivener's Office)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_GREECE', 'BUILDINGCLASS_SCRIVENERS_OFFICE', 'BUILDING_PARTHENON');

UPDATE Buildings
SET
	Defense = 300,
	TrainedFreePromotion = 'PROMOTION_PROXENOS'
WHERE Type = 'BUILDING_PARTHENON';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_SCRIVENERS_OFFICE' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_PARTHENON' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_PARTHENON', 'YIELD_GOLD', 2),
	('BUILDING_PARTHENON', 'YIELD_FAITH', 1);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_PARTHENON', 'BUILDINGCLASS_MARKET', 'YIELD_CULTURE', 3);

UPDATE Building_FreeUnits
SET NumUnits = (SELECT NumUnits FROM Building_FreeUnits WHERE BuildingType = 'BUILDING_SCRIVENERS_OFFICE') + 1
WHERE BuildingType = 'BUILDING_PARTHENON';

UPDATE Building_ResourceQuantity
SET Quantity = (SELECT Quantity FROM Building_ResourceQuantity WHERE BuildingType = 'BUILDING_SCRIVENERS_OFFICE') + 1
WHERE BuildingType = 'BUILDING_PARTHENON';

DELETE FROM Building_UnitCombatProductionModifiers WHERE BuildingType = 'BUILDING_PARTHENON';

INSERT INTO Building_UnitCombatProductionModifiersGlobal
	(BuildingType, UnitCombatType, Modifier)
VALUES
	('BUILDING_PARTHENON', 'UNITCOMBAT_DIPLOMACY', 25);

INSERT INTO Building_YieldChangesFromAccomplishments
	(BuildingType, AccomplishmentType, YieldType, Yield)
VALUES
	('BUILDING_PARTHENON', 'ACCOMPLISHMENT_DIPLOMATIC_MISSION_BOOST', 'YIELD_GOLD', 2);
