----------------------------------------------------------
-- Unique Ability: One Thousand and One Nights
----------------------------------------------------------
UPDATE Traits
SET
	LandTradeRouteRangeBonus = 0,
	TradeReligionModifier = 0,
	EventGP = 15
WHERE Type = 'TRAIT_LAND_TRADE_GOLD';

DELETE FROM Trait_ResourceQuantityModifiers
WHERE TraitType = 'TRAIT_LAND_TRADE_GOLD';

INSERT INTO Trait_YieldFromHistoricEvent
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_LAND_TRADE_GOLD', 'YIELD_SCIENCE', 1),
	('TRAIT_LAND_TRADE_GOLD', 'YIELD_CULTURE', 1);

----------------------------------------------------------
-- Unique Unit: Camel Archer (Heavy Skirmisher)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_HEAVY_SKIRMISHER' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_HEAVY_SKIRMISHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_HEAVY_SKIRMISHER') + 2
WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ARABIAN_CAMELARCHER', 'PROMOTION_WITHDRAW_BEFORE_MELEE'),
	('UNIT_ARABIAN_CAMELARCHER', 'PROMOTION_SPLASH_1');

----------------------------------------------------------
-- Unique Unit: Hashemite Raider (Light Tank)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_ARABIA', 'UNITCLASS_ANTI_TANK_GUN', 'UNIT_HASHEMITE_RAIDER');

UPDATE Units
SET
	PrereqTech = 'TECH_COMBUSTION',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_ANTI_TANK_GUN'
			)
		)
	)
WHERE Type = 'UNIT_HASHEMITE_RAIDER';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_HASHEMITE_RAIDER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_HASHEMITE_RAIDER', 'PROMOTION_GARLAND_MINE'),
	('UNIT_HASHEMITE_RAIDER', 'PROMOTION_DESERT_RAIDER');

----------------------------------------------------------
-- Unique Building: Bazaar (Market)
----------------------------------------------------------
UPDATE Buildings
SET
	FinishLandTRTourism = 6,
	FinishSeaTRTourism = 6,
	TradeRouteLandDistanceModifier = 50
WHERE Type = 'BUILDING_BAZAAR';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BAZAAR', 'YIELD_SCIENCE', 2),
	('BUILDING_BAZAAR', 'YIELD_FAITH', 2);

----------------------------------------------------------
-- Unique Building: Bimaristan (University)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_ARABIA', 'BUILDINGCLASS_UNIVERSITY', 'BUILDING_BIMARISTAN');

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BIMARISTAN', 'YIELD_FOOD', 3);

INSERT INTO Building_SpecialistYieldChangesLocal
	(BuildingType, SpecialistType, YieldType, Yield)
SELECT
	'BUILDING_BIMARISTAN', Type, 'YIELD_FOOD', 1
FROM Specialists
WHERE GreatPeopleUnitClass IS NOT NULL;

INSERT INTO Building_YieldFromFaithPurchase
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BIMARISTAN', 'YIELD_FOOD', 15),
	('BUILDING_BIMARISTAN', 'YIELD_SCIENCE', 15);
