----------------------------------------------------------
-- Unique Ability: Lion of the North
----------------------------------------------------------
UPDATE Traits
SET
	GreatPersonGiftInfluence = 0,
	DOFGreatPersonModifier = 0,
	GreatGeneralExtraBonus = 15,
	XPBonusFromGreatPersonBirth = 2,
	UnitHealFromGreatPersonBirth = 10
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
SELECT
	'TRAIT_DIPLOMACY_GREAT_PEOPLE', Type, 'PROMOTION_ATTACK_BONUS_SWEDEN'
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsRanged = 0 AND IsNaval = 0 AND IsAerial = 0;

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_SIEGE', 'PROMOTION_MOBILITY');

----------------------------------------------------------
-- Unique Unit: Carolean (Fusilier)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_RIFLEMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_RIFLEMAN') + 3,
	DefaultUnitAI = 'UNITAI_ATTACK'
WHERE Type = 'UNIT_SWEDISH_CAROLEAN';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SWEDISH_CAROLEAN', 'PROMOTION_MARCH'),
	('UNIT_SWEDISH_CAROLEAN', 'PROMOTION_GRENADIER');

----------------------------------------------------------
-- Unique Unit: Hakkapeliitta (Cuirassier)
----------------------------------------------------------
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_CUIRASSIER' WHERE UnitType = 'UNIT_SWEDISH_HAKKAPELIITTA';

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
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CUIRASSIER') + 3
WHERE Type = 'UNIT_SWEDISH_HAKKAPELIITTA';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_SWEDISH_HAKKAPELIITTA', 'PROMOTION_SENTRY'),
	('UNIT_SWEDISH_HAKKAPELIITTA', 'PROMOTION_HAKKAA_PAALLE');

----------------------------------------------------------
-- Unique Building: Nobel Committee (Foreign Bureau)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SWEDEN', 'BUILDINGCLASS_FOREIGN_BUREAU', 'BUILDING_NOBEL_COMMITTEE');

UPDATE Buildings
SET
	PrereqTech = 'TECH_DYNAMITE',
	GPExpendInfluence = 10
WHERE Type = 'BUILDING_NOBEL_COMMITTEE';

DELETE FROM Building_ClassesNeededInCity WHERE BuildingType = 'BUILDING_NOBEL_COMMITTEE';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_FOREIGN_BUREAU' AND YieldType = 'YIELD_CULTURE') + 4
WHERE BuildingType = 'BUILDING_NOBEL_COMMITTEE' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NOBEL_COMMITTEE', 'YIELD_SCIENCE', 5);

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_NOBEL_COMMITTEE', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_SCIENCE', 2),
	('BUILDING_NOBEL_COMMITTEE', 'BUILDINGCLASS_PUBLIC_SCHOOL', 'YIELD_CULTURE', 3);

INSERT INTO Building_GreatWorkYieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NOBEL_COMMITTEE', 'YIELD_SCIENCE', 2);

INSERT INTO Building_ImprovementYieldChangesGlobal
	(BuildingType, ImprovementType, YieldType, Yield)
VALUES
	('BUILDING_NOBEL_COMMITTEE', 'IMPROVEMENT_ACADEMY', 'YIELD_CULTURE', 3);

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_NOBEL_COMMITTEE', 'YIELD_GOLD', 50);

----------------------------------------------------------
-- Unique Building: Bastu (Baths)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_SWEDEN', 'BUILDINGCLASS_BATH', 'BUILDING_BASTU');

UPDATE Buildings
SET FreshWater = 0
WHERE Type = 'BUILDING_BASTU';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_BATH' AND YieldType = 'YIELD_CULTURE') + 1
WHERE BuildingType = 'BUILDING_BASTU' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BASTU', 'YIELD_FOOD', 3);

INSERT INTO Building_YieldPerXTerrainTimes100
	(BuildingType, TerrainType, YieldType, Yield)
VALUES
	('BUILDING_BASTU', 'TERRAIN_TUNDRA', 'YIELD_SCIENCE', 50);

INSERT INTO Building_YieldFromGPExpend
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BASTU', 'YIELD_CULTURE', 8);
