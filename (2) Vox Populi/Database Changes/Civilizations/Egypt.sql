----------------------------------------------------------
-- Unique Ability: Monument Builders
----------------------------------------------------------
UPDATE Traits
SET WonderProductionModGA = 20
WHERE Type = 'TRAIT_WONDER_BUILDER';

INSERT INTO Trait_ArtifactYieldChanges
	(TraitType, YieldType, Yield)
VALUES
	('TRAIT_WONDER_BUILDER', 'YIELD_SCIENCE', 5),
	('TRAIT_WONDER_BUILDER', 'YIELD_CULTURE', 5);

INSERT INTO Trait_ImprovementYieldChanges
	(TraitType, ImprovementType, YieldType, Yield)
VALUES
	('TRAIT_WONDER_BUILDER', 'IMPROVEMENT_LANDMARK', 'YIELD_GOLD', 5),
	('TRAIT_WONDER_BUILDER', 'IMPROVEMENT_LANDMARK', 'YIELD_TOURISM', 5);

----------------------------------------------------------
-- Unique Unit: War Chariot (Chariot Archer)
----------------------------------------------------------
UPDATE Units
SET
	PrereqTech = 'TECH_THE_WHEEL',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_CHARIOT_ARCHER'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_CHARIOT_ARCHER') + 1
WHERE Type = 'UNIT_EGYPTIAN_WARCHARIOT';

DELETE FROM Unit_ResourceQuantityRequirements WHERE UnitType = 'UNIT_EGYPTIAN_WARCHARIOT';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_EGYPTIAN_WARCHARIOT', 'PROMOTION_ENSLAVEMENT');

----------------------------------------------------------
-- Unique Building: Burial Tomb (Caravansary)
----------------------------------------------------------
UPDATE Civilization_BuildingClassOverrides
SET BuildingClassType = 'BUILDINGCLASS_CARAVANSARY'
WHERE BuildingType = 'BUILDING_BURIAL_TOMB';

UPDATE Buildings
SET
	GreatWorkSlotType = 'GREAT_WORK_SLOT_ART_ARTIFACT',
	GreatWorkCount = 1,
	FreeArtifacts = 1
WHERE Type = 'BUILDING_BURIAL_TOMB';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_BURIAL_TOMB', 'YIELD_FAITH', 1);

UPDATE Building_YieldPerXTerrainTimes100
SET Yield = 67
WHERE BuildingType = 'BUILDING_BURIAL_TOMB';
