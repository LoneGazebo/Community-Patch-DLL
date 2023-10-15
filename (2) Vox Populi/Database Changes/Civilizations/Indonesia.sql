----------------------------------------------------------
-- Unique Ability: Sumpah Palapa
----------------------------------------------------------
UPDATE Traits
SET
	UniqueLuxuryRequiresNewArea = 0,
	UniqueLuxuryCities = 9999,
	UniqueLuxuryQuantity = 1,
	MonopolyModPercent = 5,
	MonopolyModFlat = 2
WHERE Type = 'TRAIT_SPICE';

----------------------------------------------------------
-- Unique Unit: Kris Warrior (Swordsman)
----------------------------------------------------------
UPDATE Units
SET ObsoleteTech = (
	SELECT ObsoleteTech FROM Units WHERE Type = (
		SELECT DefaultUnit FROM UnitClasses WHERE Type = (
			SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SWORDSMAN'
		)
	)
)
WHERE Type = 'UNIT_KRIS_SWORDSMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KRIS_SWORDSMAN', 'PROMOTION_MYSTIC_BLADE');

----------------------------------------------------------
-- Unique Building: Candi (Garden)
----------------------------------------------------------
UPDATE Buildings
SET GrantsRandomResourceTerritory = 1
WHERE Type = 'BUILDING_CANDI';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'YIELD_CULTURE', 3),
	('BUILDING_CANDI', 'YIELD_FAITH', 2);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'YIELD_CULTURE', 15),
	('BUILDING_CANDI', 'YIELD_FAITH', 15);
