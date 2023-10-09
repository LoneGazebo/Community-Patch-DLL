----------------------------------------------------------
-- Unique Ability: Mare Clausum
----------------------------------------------------------
UPDATE Traits
SET TradeRouteResourceModifier = 0
WHERE Type = 'TRAIT_EXTRA_TRADE';

INSERT INTO Trait_YieldFromRouteMovement
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_EXTRA_TRADE', 'YIELD_GOLD', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_SCIENCE', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_GENERAL_POINTS', 4),
	('TRAIT_EXTRA_TRADE', 'YIELD_GREAT_ADMIRAL_POINTS', 4);

----------------------------------------------------------
-- Unique Unit: Nau (Caravel)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CARAVEL'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CARAVEL') + 5,
	BaseSightRange = (SELECT BaseSightRange FROM Units WHERE Type = 'UNIT_CARAVEL') + 1,
	NumExoticGoods = 2
WHERE Type = 'UNIT_PORTUGUESE_NAU';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PORTUGUESE_NAU', 'PROMOTION_SELL_EXOTIC_GOODS'),
	('UNIT_PORTUGUESE_NAU', 'PROMOTION_WITHDRAW_BEFORE_MELEE');

----------------------------------------------------------
-- Unique Improvement: Feitoria
----------------------------------------------------------
UPDATE Builds
SET PrereqTech = 'TECH_COMPASS'
WHERE Type = 'BUILD_FEITORIA';

UPDATE Improvements
SET
	OnlyCityStateTerritory = 0,
	NoTwoAdjacent = 1,
	DefenseModifier = 25,
	GrantsVisionXTiles = 2,
	MakesPassable = 1
WHERE Type = 'IMPROVEMENT_FEITORIA';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_FEITORIA', 'YIELD_PRODUCTION', 3),
	('IMPROVEMENT_FEITORIA', 'YIELD_GOLD', 4);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_FEITORIA', 'TECH_PRINTING_PRESS', 'YIELD_SCIENCE', 2),
	('IMPROVEMENT_FEITORIA', 'TECH_ASTRONOMY', 'YIELD_GOLD', 2),
	('IMPROVEMENT_FEITORIA', 'TECH_INDUSTRIALIZATION', 'YIELD_PRODUCTION', 2);
