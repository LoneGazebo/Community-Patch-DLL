--------------------------------------
-- Authority
--------------------------------------

-- Order matters! Do NOT swap the following update statements
UPDATE Units
SET
	Class = 'UNITCLASS_GUERILLA',
	PrereqTech = 'TECH_PENICILIN',
	ObsoleteTech = NULL,
	PolicyType = 'POLICY_HONOR_FINISHER',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_INFANTRY') + 4,
	DefaultUnitAI = 'UNITAI_ATTACK',
	PurchaseOnly = 1,
	MoveAfterPurchase = 1
WHERE Type = 'UNIT_GUERILLA';

UPDATE Units
SET
	Class = 'UNITCLASS_FOREIGNLEGION',
	ObsoleteTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_GUERILLA'),
	PolicyType = 'POLICY_HONOR_FINISHER',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 2,
	DefaultUnitAI = 'UNITAI_ATTACK',
	PurchaseOnly = 1,
	MoveAfterPurchase = 1
WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION';

UPDATE Units
SET
	Class = 'UNITCLASS_FCOMPANY',
	ObsoleteTech = (SELECT PrereqTech FROM Units WHERE Type = 'UNIT_FRENCH_FOREIGNLEGION'),
	PolicyType = 'POLICY_HONOR_FINISHER',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_PIKEMAN') + 2,
	DefaultUnitAI = 'UNITAI_ATTACK',
	PurchaseOnly = 1,
	MoveAfterPurchase = 1
WHERE Type = 'UNIT_FCOMPANY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_FCOMPANY', 'PROMOTION_DOUBLE_PLUNDER'),
	('UNIT_FCOMPANY', 'PROMOTION_FREE_PILLAGE_MOVES'),
	('UNIT_FRENCH_FOREIGNLEGION', 'PROMOTION_FOREIGN_LANDS'),
	('UNIT_GUERILLA', 'PROMOTION_AMPHIBIOUS'),
	('UNIT_GUERILLA', 'PROMOTION_DEFENSIVE_EMBARKATION'),
	('UNIT_GUERILLA', 'PROMOTION_FLANK_ATTACK_BONUS');

-- Remove building requirement in city
DELETE FROM Unit_BuildingClassPurchaseRequireds WHERE UnitType IN ('UNIT_GUERILLA', 'UNIT_FRENCH_FOREIGNLEGION', 'UNIT_FCOMPANY');

--------------------------------------
-- Freedom
--------------------------------------
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_AMERICAN_B17';

UPDATE Units
SET
	Class = 'UNITCLASS_B17',
	PolicyType = 'POLICY_THEIR_FINEST_HOUR',
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_BOMBER') + 5,
	"Range" = (SELECT "Range" FROM Units WHERE Type = 'UNIT_BOMBER') + 2,
	DefaultUnitAI = 'UNITAI_ATTACK_AIR'
WHERE Type = 'UNIT_AMERICAN_B17';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AMERICAN_B17', 'PROMOTION_AIR_SIEGE_1'),
	('UNIT_AMERICAN_B17', 'PROMOTION_EVASION');

--------------------------------------
-- Order
--------------------------------------
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_GERMAN_PANZER';

UPDATE Units
SET
	Class = 'UNITCLASS_PANZER',
	ObsoleteTech = NULL,
	PolicyType = 'POLICY_PATRIOTIC_WAR',
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_TANK') + 7,
	UnitArtInfo = 'ART_DEF_UNIT_TANK',
	UnitFlagIconOffset = 57,
	PortraitIndex = 13
WHERE Type = 'UNIT_GERMAN_PANZER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GERMAN_PANZER', 'PROMOTION_MOBILITY');

--------------------------------------
-- Autocracy
--------------------------------------
DELETE FROM Civilization_UnitClassOverrides WHERE UnitType = 'UNIT_JAPANESE_ZERO';

UPDATE Units
SET
	Class = 'UNITCLASS_ZERO',
	PolicyType = 'POLICY_MILITARISM'
WHERE Type = 'UNIT_JAPANESE_ZERO';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_JAPANESE_ZERO';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_JAPANESE_ZERO', 'PROMOTION_DOGFIGHTING_1'),
	('UNIT_JAPANESE_ZERO', 'PROMOTION_ANTI_FIGHTER');
