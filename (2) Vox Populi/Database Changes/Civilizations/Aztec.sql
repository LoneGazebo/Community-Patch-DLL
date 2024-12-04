----------------------------------------------------------
-- Unique Ability: Sacrificial Captives
----------------------------------------------------------
UPDATE Traits
SET
	CultureFromKills = 0,
	GoldenAgeFromVictory = 10
WHERE Type = 'TRAIT_CULTURE_FROM_KILLS';

INSERT INTO Trait_YieldFromKills
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_GOLD', 150),
	('TRAIT_CULTURE_FROM_KILLS', 'YIELD_FAITH', 150);

----------------------------------------------------------
-- Unique Unit: Jaguar (Warrior)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_WARRIOR'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_WARRIOR') + 2
WHERE Type = 'UNIT_AZTEC_JAGUAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_WOODSMAN'),
	('UNIT_AZTEC_JAGUAR', 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY');
