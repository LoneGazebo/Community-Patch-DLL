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
	Class = 'UNITCLASS_HUNNIC_BATTERING_RAM',
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
-- Norwegian Ski Infantry
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_SKI_INFANTRY',
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
SET Class = 'UNITCLASS_BRAZILIAN_PRACINHA'
WHERE Type = 'UNIT_BRAZILIAN_PRACINHA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BRAZILIAN_PRACINHA', 'PROMOTION_SURVIVALISM_1'),
	('UNIT_BRAZILIAN_PRACINHA', 'PROMOTION_MANY_GOLDEN_AGE_POINTS');

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
	('UNIT_ENGLISH_LONGBOWMAN', 'PROMOTION_RANGE'),
	('UNIT_ENGLISH_LONGBOWMAN', 'PROMOTION_ASSIZE_OF_ARMS');

-------------------------------------
-- Companion Cavalry
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_COMPANIONCAVALRY',
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
	('UNIT_OTTOMAN_SIPAHI', 'PROMOTION_SHOCK_4'),
	('UNIT_OTTOMAN_SIPAHI', 'PROMOTION_FLANK_ATTACK_BONUS_STRONG');

-------------------------------------
-- Hakkapeliitta
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_SWEDISH_HAKKAPELIITTA',
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_LANCER') + 1
WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SWEDISH_HAKKAPELIITTA', 'PROMOTION_SCOUT_CAVALRY'),
	('UNIT_SWEDISH_HAKKAPELIITTA', 'PROMOTION_HAKKAA_PAALLE');

-------------------------------------
-- Ballista
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_BALLISTA',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CATAPULT') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CATAPULT') + 1,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CATAPULT') + 1
WHERE Type = 'UNIT_ROMAN_BALLISTA';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_ROMAN_BALLISTA' AND PromotionType = 'PROMOTION_SIEGE_INACCURACY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ROMAN_BALLISTA', 'PROMOTION_FIELD_1');

-------------------------------------
-- Turtle Ship
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_KOREAN_TURTLE_SHIP',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CARAVEL') + 7
WHERE Type = 'UNIT_KOREAN_TURTLE_SHIP';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KOREAN_TURTLE_SHIP', 'PROMOTION_TARGETING_4'),
	('UNIT_KOREAN_TURTLE_SHIP', 'PROMOTION_OCEAN_HALF_MOVES');

-- Not suitable for exploration
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_KOREAN_TURTLE_SHIP' AND UnitAIType = 'UNITAI_EXPLORE_SEA';

-------------------------------------
-- Dromon
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_BYZANTINE_DROMON',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LIBURNA') + 3,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_LIBURNA') + 2
WHERE Type = 'UNIT_BYZANTINE_DROMON';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BYZANTINE_DROMON', 'PROMOTION_TARGETING_1'),
	('UNIT_BYZANTINE_DROMON', 'PROMOTION_SPLASH_1');

-------------------------------------
-- Great Galleass
-------------------------------------
UPDATE Units
SET
	Class = 'UNITCLASS_VENETIAN_GALLEASS',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_GALLEASS') + 6,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_GALLEASS') + 3
WHERE Type = 'UNIT_VENETIAN_GALLEASS';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_VENETIAN_GALLEASS', 'PROMOTION_BOMBARDMENT_1');
