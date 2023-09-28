----------------------------------------------------------
-- Unique Ability: Achaemenid Legacy
----------------------------------------------------------
UPDATE Traits
SET
	AlwaysReligion = 1,
	AnyBelief = 1,
	FaithCostModifier = -15,
	GPFaithPurchaseEra = 'ERA_CLASSICAL'
WHERE Type = 'TRAIT_EXTRA_BELIEF';

----------------------------------------------------------
-- Unique Unit: Cataphract (Knight)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_KNIGHT' WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_KNIGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_KNIGHT') + 4,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_KNIGHT') - 1
WHERE Type = 'UNIT_BYZANTINE_CATAPHRACT';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_BYZANTINE_CATAPHRACT' AND PromotionType IN (
	'PROMOTION_NO_DEFENSIVE_BONUSES',
	'PROMOTION_CITY_PENALTY'
);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_COVER_1'),
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_SMALL_CITY_PENALTY'),
	('UNIT_BYZANTINE_CATAPHRACT', 'PROMOTION_OPEN_TERRAIN');

----------------------------------------------------------
-- Unique Building: Tetraconch (Temple)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_BYZANTIUM', 'BUILDINGCLASS_TEMPLE', 'BUILDING_BASILICA');

UPDATE Buildings
SET
	MaxStartEra = NULL,
	TradeReligionModifier = 100
WHERE Type = 'BUILDING_BASILICA';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
--	('BUILDING_BASILICA', 'YIELD_FAITH', 3),
	('BUILDING_BASILICA', 'YIELD_CULTURE', 2);

INSERT INTO Building_YieldChangesPerPop
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BASILICA', 'YIELD_FAITH', 34);
