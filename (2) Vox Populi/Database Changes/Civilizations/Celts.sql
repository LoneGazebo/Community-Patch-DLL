----------------------------------------------------------
-- Unique Ability: Druidic Lore
----------------------------------------------------------
UPDATE Traits
SET
	FaithFromUnimprovedForest = 0,
	UniqueBeliefsOnly = 1,
	NoNaturalReligionSpread = 1
WHERE Type = 'TRAIT_FAITH_FROM_NATURE';

INSERT INTO Trait_YieldFromOwnPantheon
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_FAITH_FROM_NATURE', 'YIELD_FAITH', 3);

----------------------------------------------------------
-- Unique Unit: Pictish Warrior (Spearman)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_MINING',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPEARMAN'
			)
		)
	),
	DefaultUnitAI = 'UNITAI_ATTACK',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPEARMAN') + 1
WHERE Type = 'UNIT_CELT_PICTISH_WARRIOR';

DELETE FROM Unit_AITypes
WHERE UnitType = 'UNIT_CELT_PICTISH_WARRIOR' AND UnitAIType = 'UNITAI_COUNTER';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_CELT_PICTISH_WARRIOR' AND PromotionType = 'PROMOTION_FORMATION_1';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CELT_PICTISH_WARRIOR', 'PROMOTION_SKI_INFANTRY'),
	('UNIT_CELT_PICTISH_WARRIOR', 'PROMOTION_FREE_PILLAGE_MOVES');

INSERT INTO Unit_YieldFromKills
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_CELT_PICTISH_WARRIOR', 'YIELD_FAITH', 200);

----------------------------------------------------------
-- Unique Building: Ceilidh Hall (Circus)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_CIRCUS'
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';

UPDATE Buildings
SET
	Help = 'TXT_KEY_BUILDING_CEILIDH_HALL_HELP',
	WLTKDTurns = (SELECT WLTKDTurns FROM Buildings WHERE Type = 'BUILDING_CIRCUS') * 150 / 100,
	SpecialistType = 'SPECIALIST_MUSICIAN',
	SpecialistCount = 1,
	GreatWorkSlotType = 'GREAT_WORK_SLOT_MUSIC',
	GreatWorkCount = 1
WHERE Type = 'BUILDING_CEILIDH_HALL';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CEILIDH_HALL', 'YIELD_CULTURE', 4),
	('BUILDING_CEILIDH_HALL', 'YIELD_FAITH', 2);

UPDATE Building_InstantYield
SET Yield = (SELECT Yield FROM Building_InstantYield WHERE BuildingType = 'BUILDING_CIRCUS') * 150 / 100
WHERE BuildingType = 'BUILDING_CEILIDH_HALL';
