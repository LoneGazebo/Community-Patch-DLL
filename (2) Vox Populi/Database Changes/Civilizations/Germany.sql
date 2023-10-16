----------------------------------------------------------
-- Unique Ability: Blood and Iron
----------------------------------------------------------
UPDATE Traits
SET
	LandBarbarianConversionPercent = 0,
	LandUnitMaintenanceModifier = 0,
	MinorInfluencePerGiftedUnit = 1
WHERE Type = 'TRAIT_CONVERTS_LAND_BARBARIANS';

INSERT INTO Trait_YieldFromCSFriend
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CONVERTS_LAND_BARBARIANS', 'YIELD_SCIENCE', 2);

INSERT INTO Trait_YieldFromCSAlly
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_CONVERTS_LAND_BARBARIANS', 'YIELD_SCIENCE', 2),
	('TRAIT_CONVERTS_LAND_BARBARIANS', 'YIELD_CULTURE', 2);

----------------------------------------------------------
-- Unique Unit: Landsknecht (Tercio)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_GERMANY', 'UNITCLASS_TERCIO', 'UNIT_GERMAN_LANDSKNECHT');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SPANISH_TERCIO'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SPANISH_TERCIO') + 2,
	MoveAfterPurchase = 1
WHERE Type = 'UNIT_GERMAN_LANDSKNECHT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GERMAN_LANDSKNECHT', 'PROMOTION_FORMATION_2'),
	('UNIT_GERMAN_LANDSKNECHT', 'PROMOTION_DOPPELSOLDNER');

----------------------------------------------------------
-- Unique Building: Hanse (Customs House)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_MINT'
WHERE BuildingType = 'BUILDING_HANSE';

UPDATE Buildings
SET
	TradeRouteRecipientBonus = (SELECT TradeRouteRecipientBonus FROM Buildings WHERE Type = 'BUILDING_MINT') + 1,
	TradeRouteTargetBonus = (SELECT TradeRouteTargetBonus FROM Buildings WHERE Type = 'BUILDING_MINT') + 1,
	CityStateTradeRouteProductionModifier = 5
WHERE Type = 'BUILDING_HANSE';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_MINT' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_HANSE' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_HANSE', 'YIELD_GOLD', 5);

INSERT INTO Building_YieldFromYieldPercent
	(BuildingType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_HANSE', 'YIELD_GOLD', 'YIELD_SCIENCE', 10);
