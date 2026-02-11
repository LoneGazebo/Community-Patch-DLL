----------------------------------------------------------
-- Unique Ability: Phoenician Heritage
----------------------------------------------------------
UPDATE Traits
SET
	CrossesMountainsAfterGreatGeneral = 0,
	FreeBuilding = NULL,
	PurchasedUnitsBonusXP = 5,
	TradeRouteResourceModifier = 100
WHERE Type = 'TRAIT_PHOENICIAN_HERITAGE';

INSERT INTO Trait_YieldFromLuxuryResourceGain
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_PHOENICIAN_HERITAGE', 'YIELD_GOLD', 100),
	('TRAIT_PHOENICIAN_HERITAGE', 'YIELD_SCIENCE', 25);

UPDATE Civilization_FreeUnits
SET UnitClassType = 'UNITCLASS_ASAMU'
WHERE CivilizationType = 'CIVILIZATION_CARTHAGE' AND UnitClassType = 'UNITCLASS_SETTLER';

----------------------------------------------------------
-- Unique Unit: Quinquereme (Trireme)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_TRIREME'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_TRIREME') + 2
WHERE Type = 'UNIT_CARTHAGINIAN_QUINQUEREME';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CARTHAGINIAN_QUINQUEREME', 'PROMOTION_PINCER'),
	('UNIT_CARTHAGINIAN_QUINQUEREME', 'PROMOTION_HEAVY_ASSAULT');

----------------------------------------------------------
-- Unique Unit: Atlas Elephant (Horseman)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_HORSEBACK_RIDING', -- Trade
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_HORSEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_HORSEMAN') + 3,
	Moves = (SELECT Moves FROM Units WHERE Type = 'UNIT_HORSEMAN') - 1
WHERE Type = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT';

DELETE FROM Unit_FreePromotions WHERE UnitType = 'UNIT_CARTHAGINIAN_FOREST_ELEPHANT' AND PromotionType = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_CARTHAGINIAN_FOREST_ELEPHANT', 'PROMOTION_FEARED_ELEPHANT'),
	('UNIT_CARTHAGINIAN_FOREST_ELEPHANT', 'PROMOTION_AT_THE_GATES');

----------------------------------------------------------
-- Unique Building: Tophet (Shrine)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_CARTHAGE', 'BUILDINGCLASS_SHRINE', 'BUILDING_TOPHET');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TOPHET', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldFromPurchase
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TOPHET', 'YIELD_CULTURE', 5),
	('BUILDING_TOPHET', 'YIELD_FOOD', 5);

INSERT INTO Building_YieldFromFaithPurchase
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TOPHET', 'YIELD_CULTURE', 5),
	('BUILDING_TOPHET', 'YIELD_FOOD', 5);

----------------------------------------------------------
-- Unique Building: Great Cothon (East India Company)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_CARTHAGE', 'BUILDINGCLASS_NATIONAL_TREASURY', 'BUILDING_GREAT_COTHON');

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_GREAT_COTHON';

UPDATE Buildings
SET
	PrereqTech = 'TECH_CURRENCY',
	TradeRouteRecipientBonus = (SELECT TradeRouteRecipientBonus FROM Buildings WHERE Type = 'BUILDING_NATIONAL_TREASURY') + 1,
	TradeRouteTargetBonus = (SELECT TradeRouteTargetBonus FROM Buildings WHERE Type = 'BUILDING_NATIONAL_TREASURY') + 1,
	NumTradeRouteBonus = 2,
	PovertyFlatReduction = 1,
	Water = 1,
	FreeBuilding = 'BUILDINGCLASS_HARBOR'
WHERE Type = 'BUILDING_GREAT_COTHON';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_GREAT_COTHON', 'BUILDINGCLASS_LIGHTHOUSE', 'YIELD_CULTURE', 2),
	('BUILDING_GREAT_COTHON', 'BUILDINGCLASS_HARBOR', 'YIELD_PRODUCTION', 3);
