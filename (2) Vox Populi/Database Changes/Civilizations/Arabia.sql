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
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_MOUNTED_BOWMAN' WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_MOUNTED_BOWMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_MOUNTED_BOWMAN') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_MOUNTED_BOWMAN') + 2
WHERE Type = 'UNIT_ARABIAN_CAMELARCHER';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_ARABIAN_CAMELARCHER';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_ARABIAN_CAMELARCHER', 'PROMOTION_WITHDRAW_BEFORE_MELEE'),
	('UNIT_ARABIAN_CAMELARCHER', 'PROMOTION_SPLASH_1');

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
