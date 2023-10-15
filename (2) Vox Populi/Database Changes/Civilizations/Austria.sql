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

----------------------------------------------------------
-- Unique Building: Coffee House (Grocer)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_GROCER'
WHERE BuildingType = 'BUILDING_COFFEE_HOUSE';

UPDATE Buildings
SET
	GreatPeopleRateModifier = 33,
	SpecialistType = 'SPECIALIST_MERCHANT',
	SpecialistCount = 1
WHERE Type = 'BUILDING_COFFEE_HOUSE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_COFFEE_HOUSE', 'YIELD_FOOD', 3);

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_COFFEE_HOUSE', 'YIELD_CULTURE', 'YIELD_SCIENCE', 10);
