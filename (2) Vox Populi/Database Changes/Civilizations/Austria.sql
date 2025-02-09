----------------------------------------------------------
-- Unique Ability: Habsburg Diplomacy
----------------------------------------------------------
UPDATE Traits
SET
	AbleToAnnexCityStates = 0,
	MinorQuestYieldModifier = 50,
	DiplomaticMarriage = 1
WHERE Type = 'TRAIT_ANNEX_CITY_STATE';

----------------------------------------------------------
-- Unique Unit: Hussar (Cuirassier)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_AUSTRIAN_HUSSAR';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CUIRASSIER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 1
WHERE Type = 'UNIT_AUSTRIAN_HUSSAR';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_AUSTRIAN_HUSSAR', 'PROMOTION_LIGHTNING_WARFARE');
