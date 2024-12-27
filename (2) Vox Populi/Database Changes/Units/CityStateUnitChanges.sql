DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType IN (
	SELECT Type FROM Units WHERE MinorCivGift = 1
);

-- Set obsolete tech to be same as its upgrade's
UPDATE Units
SET ObsoleteTech = (
	SELECT ObsoleteTech FROM Units a WHERE a.Type = (
		SELECT DefaultUnit FROM UnitClasses WHERE Type = (
			SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = (
				SELECT DefaultUnit FROM UnitClasses WHERE Type = Units.Class
			)
		)
	)
)
WHERE MinorCivGift = 1;

-------------------------------------
-- Battering Ram
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_BATTERING_RAM',
	ObsoleteTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO'),
	Combat = 17,
	BaseSightRange = 1,
	DefaultUnitAI = 'UNITAI_CITY_BOMBARD'
WHERE Type = 'UNIT_HUN_BATTERING_RAM';

UPDATE Unit_ClassUpgrades SET UnitClassType = 'UNITCLASS_TERCIO' WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';

-- Doesn't matter what Swordsman has. Replace them all.
DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';
DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_HUN_BATTERING_RAM';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_HUN_BATTERING_RAM', 'PROMOTION_COVER_1'),
	('UNIT_HUN_BATTERING_RAM', 'PROMOTION_SIEGE'),
	('UNIT_HUN_BATTERING_RAM', 'PROMOTION_NO_DEFENSIVE_BONUSES'),
	('UNIT_HUN_BATTERING_RAM', 'PROMOTION_CITY_ASSAULT'),
	('UNIT_HUN_BATTERING_RAM', 'PROMOTION_ONLY_ATTACKS_CITIES');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_HUN_BATTERING_RAM', 'UNITAI_CITY_BOMBARD');

-------------------------------------
-- Mehal Sefari
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_MEHAL_SEFARI',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 4
WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_COVER_1'),
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_DEFEND_NEAR_CAPITAL'),
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_HOMELAND_GUARDIAN');

-------------------------------------
-- Norwegian Ski Infantry
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_NORWEGIAN_SKI_INFANTRY',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 2
WHERE Type = 'UNIT_DANISH_SKI_INFANTRY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_DANISH_SKI_INFANTRY', 'PROMOTION_SKI_INFANTRY');

-------------------------------------
-- Pracinha
-------------------------------------
UPDATE Units
SET Class = 'UNITCLASS_PRACINHA'
WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BRAZILIAN_PRACINHA', 'PROMOTION_SURVIVALISM_1'),
	('UNIT_BRAZILIAN_PRACINHA', 'PROMOTION_PRIDE_OF_THE_NATION');

-------------------------------------
-- Longbowman
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_LONGBOWMAN',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CROSSBOWMAN') + 1
WHERE Type = 'UNIT_ENGLISH_LONGBOWMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ENGLISH_LONGBOWMAN', 'PROMOTION_AGINCOURT'),
	('UNIT_ENGLISH_LONGBOWMAN', 'PROMOTION_ASSIZE_OF_ARMS');

-------------------------------------
-- Companion Cavalry
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_COMPANION_CAVALRY',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_HORSEMAN') + 3,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_HORSEMAN') + 1
WHERE Type = 'UNIT_GREEK_COMPANIONCAVALRY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GREEK_COMPANIONCAVALRY', 'PROMOTION_SPAWN_GENERALS_I'),
	('UNIT_GREEK_COMPANIONCAVALRY', 'PROMOTION_MOVEMENT_TO_GENERAL'),
	('UNIT_GREEK_COMPANIONCAVALRY', 'PROMOTION_GENERAL_STACKING');

-------------------------------------
-- Sipahi
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_SIPAHI',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LANCER') + 2
WHERE Type = 'UNIT_OTTOMAN_SIPAHI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_OTTOMAN_SIPAHI', 'PROMOTION_OVERRUN'),
	('UNIT_OTTOMAN_SIPAHI', 'PROMOTION_HEAVY_FLANKING');
