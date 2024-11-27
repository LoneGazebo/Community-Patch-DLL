----------------------------------------------------------
-- Unique Ability: The Glory of Rome
----------------------------------------------------------
UPDATE Traits
SET
	CapitalBuildingModifier = 15,
	CityStateCombatModifier = 30,
	AnnexedCityStatesGiveYields = 1
WHERE Type = 'TRAIT_CAPITAL_BUILDINGS_CHEAPER';

----------------------------------------------------------
-- Unique Unit: Legion (Swordsman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN') + 2,
	WorkRate = 100
WHERE Type = 'UNIT_ROMAN_LEGION';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ROMAN_LEGION', 'PROMOTION_COVER_1'),
	('UNIT_ROMAN_LEGION', 'PROMOTION_PILUM');

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_ROMAN_LEGION', 'BUILD_ROAD'),
	('UNIT_ROMAN_LEGION', 'BUILD_FORT');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_ROMAN_LEGION', 'UNITAI_WORKER');
