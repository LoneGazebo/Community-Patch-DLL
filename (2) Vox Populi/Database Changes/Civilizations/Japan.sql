----------------------------------------------------------
-- Unique Ability: Shogunate
----------------------------------------------------------
UPDATE Traits
SET FightWellDamaged = 0
WHERE Type = 'TRAIT_FIGHT_WELL_DAMAGED';

DELETE FROM Trait_ImprovementYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';
DELETE FROM Trait_UnimprovedFeatureYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';

INSERT INTO Trait_BuildingClassYieldChanges
	(TraitType, BuildingClassType, YieldType, YieldChange)
VALUES
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_WALLS', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_WALLS', 'YIELD_FAITH', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_CASTLE', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_CASTLE', 'YIELD_FAITH', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_FORTRESS', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_FORTRESS', 'YIELD_FAITH', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_ARSENAL', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_ARSENAL', 'YIELD_FAITH', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_MILITARY_BASE', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_MILITARY_BASE', 'YIELD_FAITH', 1);

INSERT INTO Trait_GreatPersonBirthGWAM
	(TraitType, GreatPersonType, Value)
VALUES
	('TRAIT_FIGHT_WELL_DAMAGED', 'GREATPERSON_GENERAL', 50),
	('TRAIT_FIGHT_WELL_DAMAGED', 'GREATPERSON_ADMIRAL', 50);

----------------------------------------------------------
-- Unique Unit: Samurai (Longswordsman)
----------------------------------------------------------
UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_LONGSWORDSMAN'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_LONGSWORDSMAN') + 3
WHERE Type = 'UNIT_JAPANESE_SAMURAI';

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_JAPANESE_SAMURAI', 'PROMOTION_SPAWN_GENERALS_II'),
	('UNIT_JAPANESE_SAMURAI', 'PROMOTION_GAIN_EXPERIENCE');

----------------------------------------------------------
-- Unique Building: Dojo (Armory)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_JAPAN', 'BUILDINGCLASS_ARMORY', 'BUILDING_DOJO');

UPDATE Buildings
SET
	CitySupplyFlat = (SELECT CitySupplyFlat FROM Buildings WHERE Type = 'BUILDING_ARMORY') + 1,
	TrainedFreePromotion = 'PROMOTION_BUSHIDO'
WHERE Type = 'BUILDING_DOJO';

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_ARMORY' AND YieldType = 'YIELD_SCIENCE') + 3
WHERE BuildingType = 'BUILDING_DOJO' AND YieldType = 'YIELD_SCIENCE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_DOJO', 'YIELD_CULTURE', 3);

UPDATE Building_DomainFreeExperiences
SET Experience = (SELECT Experience FROM Building_DomainFreeExperiences WHERE BuildingType = 'BUILDING_ARMORY') + 5
WHERE BuildingType = 'BUILDING_DOJO';

INSERT INTO Building_YieldFromCombatExperienceTimes100
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_DOJO', 'YIELD_CULTURE', 140),
	('BUILDING_DOJO', 'YIELD_SCIENCE', 140);
