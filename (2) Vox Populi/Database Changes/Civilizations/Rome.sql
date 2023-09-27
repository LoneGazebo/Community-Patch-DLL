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
	('UNIT_ROMAN_LEGION', 'PROMOTION_AOE_STRIKE_FORTIFY');

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_ROMAN_LEGION', 'BUILD_ROAD'),
	('UNIT_ROMAN_LEGION', 'BUILD_FORT');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_ROMAN_LEGION', 'UNITAI_WORKER');

----------------------------------------------------------
-- Unique Building: Colosseum
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ROME', 'BUILDINGCLASS_COLOSSEUM', 'BUILDING_FLAVIAN_COLOSSEUM');

UPDATE Buildings
SET CityConnectionTradeRouteModifier = 2
WHERE Type = 'BUILDING_FLAVIAN_COLOSSEUM';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_COLOSSEUM' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_FLAVIAN_COLOSSEUM' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLAVIAN_COLOSSEUM', 'YIELD_PRODUCTION', 3);

UPDATE Building_YieldChangesPerPop
SET Yield = 34 -- 25
WHERE BuildingType = 'BUILDING_FLAVIAN_COLOSSEUM' AND YieldType = 'YIELD_TOURISM';

INSERT INTO Building_YieldFromVictoryGlobal
	(BuildingType, YieldType, Yield, IsEraScaling)
VALUES
	('BUILDING_FLAVIAN_COLOSSEUM', 'YIELD_GOLDEN_AGE_POINTS', 5, 1);

INSERT INTO Building_YieldFromVictory
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_FLAVIAN_COLOSSEUM', 'YIELD_GREAT_GENERAL_POINTS', 10),
	('BUILDING_FLAVIAN_COLOSSEUM', 'YIELD_GREAT_ADMIRAL_POINTS', 10);
