----------------------------------------------------------
-- Unique Ability: Sumpah Palapa
----------------------------------------------------------
UPDATE Traits
SET
	UniqueLuxuryRequiresNewArea = 0,
	UniqueLuxuryCities = 9999,
	UniqueLuxuryQuantity = 1,
	MonopolyModPercent = 5,
	MonopolyModFlat = 2
WHERE Type = 'TRAIT_SPICE';

----------------------------------------------------------
-- Unique Unit: Kris Warrior (Swordsman)
----------------------------------------------------------
UPDATE Units
SET ObsoleteTech = (
	SELECT ObsoleteTech FROM Units WHERE Type = (
		SELECT DefaultUnit FROM UnitClasses WHERE Type = (
			SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_SWORDSMAN'
		)
	)
)
WHERE Type = 'UNIT_KRIS_SWORDSMAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_KRIS_SWORDSMAN', 'PROMOTION_MYSTIC_BLADE');

----------------------------------------------------------
-- Unique Unit: Djong (Galleass)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_INDONESIA', 'UNITCLASS_GALLEASS', 'UNIT_DJONG');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_GALLEASS'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_GALLEASS') + 4,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_GALLEASS') + 1,
	"Range" = 2
WHERE Type = 'UNIT_DJONG';

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_DJONG' AND PromotionType IN (
	'PROMOTION_CAN_MOVE_AFTER_ATTACKING',
	'PROMOTION_SHALLOW_DRAFT'
);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_DJONG', 'PROMOTION_CETBANG');

----------------------------------------------------------
-- Unique Improvement: Kampong
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_OPTICS', -- Sailing
	Water = 1,
	CanBeEmbarked = 1,
	Time = 600
WHERE Type = 'BUILD_KAMPONG';

INSERT INTO Unit_Builds
	(UnitType, BuildType)
VALUES
	('UNIT_WORKER', 'BUILD_KAMPONG');

UPDATE Improvements
SET
	Water = 1,
	NoTwoAdjacent = 1
WHERE Type = 'IMPROVEMENT_KAMPONG';

INSERT INTO Improvement_ValidTerrains
	(ImprovementType, TerrainType)
VALUES
	('IMPROVEMENT_KAMPONG', 'TERRAIN_COAST');

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KAMPONG', 'YIELD_FOOD', 1),
	('IMPROVEMENT_KAMPONG', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_KAMPONG', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KAMPONG', 'TECH_COMPASS', 'YIELD_PRODUCTION', 1),
	('IMPROVEMENT_KAMPONG', 'TECH_NAVIGATION', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_KAMPONG', 'TECH_DYNAMITE', 'YIELD_FOOD', 2),
	('IMPROVEMENT_KAMPONG', 'TECH_ECOLOGY', 'YIELD_SCIENCE', 2);

----------------------------------------------------------
-- Unique Building: Candi (Garden)
----------------------------------------------------------
UPDATE Buildings
SET GrantsRandomResourceTerritory = 1
WHERE Type = 'BUILDING_CANDI';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'YIELD_CULTURE', 3),
	('BUILDING_CANDI', 'YIELD_FAITH', 2);

INSERT INTO Building_WLTKDYieldMod
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_CANDI', 'YIELD_CULTURE', 15),
	('BUILDING_CANDI', 'YIELD_FAITH', 15);
