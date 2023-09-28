----------------------------------------------------------
-- Unique Ability: Solomonic Wisdom
----------------------------------------------------------
UPDATE Traits
SET
	CombatBonusVsLargerCiv = 0,
	IsAdoptionFreeTech = 1
WHERE Type = 'TRAIT_BONUS_AGAINST_TECH';

INSERT INTO Trait_YieldChangesStrategicResources
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_BONUS_AGAINST_TECH', 'YIELD_FAITH', 1);

----------------------------------------------------------
-- Unique Unit: Mehal Sefari (Fusilier)
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
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 4
WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_COVER_1'),
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_DEFEND_NEAR_CAPITAL'),
	('UNIT_ETHIOPIAN_MEHAL_SEFARI', 'PROMOTION_HOMELAND_GUARDIAN');

----------------------------------------------------------
-- Unique Building: Stele (Monument)
----------------------------------------------------------
INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STELE', 'YIELD_FAITH', 2);

UPDATE Buildings
SET Help = 'TXT_KEY_BUILDING_STELE_HELP'
WHERE Type = 'BUILDING_STELE';

INSERT INTO Building_GoldenAgeYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_STELE', 'YIELD_FAITH', 25);
