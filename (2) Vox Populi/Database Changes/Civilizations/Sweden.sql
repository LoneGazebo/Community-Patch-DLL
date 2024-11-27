----------------------------------------------------------
-- Unique Ability: Lion of the North
----------------------------------------------------------
UPDATE Traits
SET
	GreatPersonGiftInfluence = 0,
	DOFGreatPersonModifier = 0,
	XPBonusFromGGBirth = 15,
	GreatGeneralExtraBonus = 15
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_DIPLOMACY_GREAT_PEOPLE', Type, 'PROMOTION_ATTACK_BONUS_SWEDEN'
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsRanged = 0 AND IsNaval = 0 AND IsAerial = 0;

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_SIEGE', 'PROMOTION_MOBILITY');

----------------------------------------------------------
-- Unique Unit: Carolean (Fusilier)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_RIFLEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 3,
	DefaultUnitAI = 'UNITAI_ATTACK'
WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SWEDISH_CAROLEAN', 'PROMOTION_MARCH'),
	('UNIT_SWEDISH_CAROLEAN', 'PROMOTION_GRENADIER');
