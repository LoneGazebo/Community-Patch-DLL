----------------------------------------------------------
-- Unique Ability: Father Governs Children
----------------------------------------------------------
UPDATE Traits SET CityStateBonusModifier = 100 WHERE Type = 'TRAIT_CITY_STATE_BONUSES';

----------------------------------------------------------
-- Unique Unit: Naresuan's Elephant (Knight)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 2,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_KNIGHT') - 1
WHERE Type = 'UNIT_SIAMESE_WARELEPHANT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_SIAMESE_WARELEPHANT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_GENERAL_STACKING'),
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_ANTI_MOUNTED_I'),
	('UNIT_SIAMESE_WARELEPHANT', 'PROMOTION_FEARED_ELEPHANT');

----------------------------------------------------------
-- Unique Building: Wat (Constabulary)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_CONSTABLE'
WHERE BuildingType = 'BUILDING_WAT';

UPDATE Buildings
SET
	SpecialistType = 'SPECIALIST_SCIENTIST',
	SpecialistCount = 1,
	SpySecurityModifier = 25, -- 20
	DiplomatInfluenceBoost = 10,
	PrereqTech = 'TECH_THEOLOGY'
WHERE Type = 'BUILDING_WAT';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_WAT', 'YIELD_SCIENCE', 2),
	('BUILDING_WAT', 'YIELD_CULTURE', 1),
	('BUILDING_WAT', 'YIELD_FAITH', 1);

INSERT INTO Building_BuildingClassLocalYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_WAT', 'BUILDINGCLASS_SHRINE', 'YIELD_SCIENCE', 3),
	('BUILDING_WAT', 'BUILDINGCLASS_TEMPLE', 'YIELD_SCIENCE', 3);
