----------------------------------------------------------
-- Unique Ability: Gateway to Africa
----------------------------------------------------------
UPDATE Traits
SET
	IgnoreTradeDistanceScaling = 1,
	CanPlunderWithoutWar = 1
WHERE Type = 'TRAIT_GATEWAY_AFRICA';

DELETE FROM Trait_YieldChangesPerTradePartner WHERE TraitType = 'TRAIT_GATEWAY_AFRICA';
DELETE FROM Trait_YieldChangesIncomingTradeRoute WHERE TraitType = 'TRAIT_GATEWAY_AFRICA';

INSERT INTO Trait_YieldChangesPerTradePartner
	(TraitType, YieldType, Yield)
SELECT
	'TRAIT_GATEWAY_AFRICA', Type, 1
FROM Yields
WHERE ID < 6; -- "All" yields

----------------------------------------------------------
-- Unique Unit: Berber Cavalry (Cavalry)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CAVALRY'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CAVALRY') + 2,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CAVALRY') + 3
WHERE Type = 'UNIT_BERBER_CAVALRY';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_BERBER_CAVALRY', 'PROMOTION_HOMELAND_GUARDIAN'),
	('UNIT_BERBER_CAVALRY', 'PROMOTION_IGNORE_TERRAIN_COST');

-- Go explore! And plunder more trade routes while we're at it
INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_BERBER_CAVALRY', 'UNITAI_EXPLORE');

----------------------------------------------------------
-- Unique Improvement: Kasbah
----------------------------------------------------------
UPDATE Builds
SET
	PrereqTech = 'TECH_CHIVALRY',
	Time = 900
WHERE Type = 'BUILD_KASBAH';

UPDATE Improvements
SET
	Cityside = 1, -- Makes valid
	BuildableOnResources = 1,
	ConnectsAllResources = 1,
	DefenseModifier = 30,
	NearbyEnemyDamage = 5
WHERE Type = 'IMPROVEMENT_KASBAH';

DELETE FROM Improvement_ValidTerrains WHERE ImprovementType = 'IMPROVEMENT_KASBAH';

DELETE FROM Improvement_Yields WHERE ImprovementType = 'IMPROVEMENT_KASBAH';

INSERT INTO Improvement_Yields
	(ImprovementType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KASBAH', 'YIELD_FOOD', 1),
	('IMPROVEMENT_KASBAH', 'YIELD_PRODUCTION', 2),
	('IMPROVEMENT_KASBAH', 'YIELD_GOLD', 2),
	('IMPROVEMENT_KASBAH', 'YIELD_CULTURE', 2);

INSERT INTO Improvement_TechYieldChanges
	(ImprovementType, TechType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KASBAH', 'TECH_ARCHITECTURE', 'YIELD_GOLD', 1),
	('IMPROVEMENT_KASBAH', 'TECH_ARCHITECTURE', 'YIELD_CULTURE', 1),
	('IMPROVEMENT_KASBAH', 'TECH_RADIO', 'YIELD_CULTURE', 1);

INSERT INTO Improvement_YieldPerXAdjacentImprovement
	(ImprovementType, OtherImprovementType, YieldType, Yield, NumRequired)
VALUES
	('IMPROVEMENT_FISHING_BOATS', 'IMPROVEMENT_KASBAH', 'YIELD_GOLD', 2, 1);

INSERT INTO Improvement_AdjacentTerrainYieldChanges
	(ImprovementType, TerrainType, YieldType, Yield)
VALUES
	('IMPROVEMENT_KASBAH', 'TERRAIN_COAST', 'YIELD_CULTURE', 1);
