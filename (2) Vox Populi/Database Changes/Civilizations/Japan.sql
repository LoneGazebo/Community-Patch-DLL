----------------------------------------------------------
-- Unique Ability: Shogunate
----------------------------------------------------------
UPDATE Traits
SET FightWellDamaged = 0
WHERE Type = 'TRAIT_FIGHT_WELL_DAMAGED';

DELETE FROM Trait_ImprovementYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';
DELETE FROM Trait_UnimprovedFeatureYieldChanges WHERE TraitType = 'TRAIT_FIGHT_WELL_DAMAGED';

INSERT INTO Trait_FreePromotionUnitCombats
	(TraitType, UnitCombatType, PromotionType)
VALUES
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_MELEE', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_GUN', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_MOUNTED', 'PROMOTION_BUSHIDO'),
	('TRAIT_FIGHT_WELL_DAMAGED', 'UNITCOMBAT_ARMOR', 'PROMOTION_BUSHIDO');

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
-- Unique Unit: Mikasa (Dreadnought)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_JAPAN', 'UNITCLASS_DREADNOUGHT', 'UNIT_MIKASA');

UPDATE Units
SET
	PrereqTech = 'TECH_ELECTRICITY',
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_DREADNOUGHT'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_DREADNOUGHT') + 5,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_DREADNOUGHT') + 5
WHERE Type = 'UNIT_MIKASA';

INSERT INTO Unit_YieldOnCompletion
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_MIKASA', 'YIELD_GOLDEN_AGE_POINTS', 500);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_MIKASA', 'PROMOTION_HOMELAND_GUARDIAN'),
	('UNIT_MIKASA', 'PROMOTION_KANTAI_KESSEN');

----------------------------------------------------------
-- Unique Building: Tatara (Forge)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_JAPAN', 'BUILDINGCLASS_FORGE', 'BUILDING_TATARA');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_FORGE' AND YieldType = 'YIELD_SCIENCE') + 1
WHERE BuildingType = 'BUILDING_TATARA' AND YieldType = 'YIELD_SCIENCE';

INSERT INTO Building_YieldChanges
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'YIELD_PRODUCTION', 2);

UPDATE Buildings
SET SpecialistCount = (SELECT SpecialistCount FROM Buildings WHERE Type = 'BUILDING_FORGE') + 1
WHERE Type = 'BUILDING_TATARA'; 

INSERT INTO Building_ResourcePlotsToPlace
	(BuildingType, ResourceType, NumPlots, ResourceQuantityToPlace)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 1, 1);

INSERT INTO Building_ResourceClaim
	(BuildingType, ResourceType, IncludeOwnedByOtherPlayer)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 0);

INSERT INTO Building_ResourceYieldChanges
	(BuildingType, ResourceType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'RESOURCE_IRON', 'YIELD_CULTURE', 1);

INSERT INTO Building_YieldFromCombatExperienceTimes100
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_TATARA', 'YIELD_CULTURE', 140),
	('BUILDING_TATARA', 'YIELD_SCIENCE', 140);

----------------------------------------------------------
-- Unique Building: Kabuki Theater (Opera House)
----------------------------------------------------------
INSERT INTO Civilization_BuildingClassOverrides
	(CivilizationType, BuildingClassType, BuildingType)
VALUES
	('CIVILIZATION_JAPAN', 'BUILDINGCLASS_OPERA_HOUSE', 'BUILDING_KABUKI_THEATER');

UPDATE Building_YieldChanges
SET Yield = (SELECT Yield FROM Building_YieldChanges WHERE BuildingType = 'BUILDING_OPERA_HOUSE' AND YieldType = 'YIELD_CULTURE') + 2
WHERE BuildingType = 'BUILDING_KABUKI_THEATER' AND YieldType = 'YIELD_CULTURE';

INSERT INTO Building_BuildingClassYieldChanges
	(BuildingType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BUILDING_KABUKI_THEATER', 'BUILDINGCLASS_WRITERS_GUILD', 'YIELD_GOLD', 1),
	('BUILDING_KABUKI_THEATER', 'BUILDINGCLASS_ARTISTS_GUILD', 'YIELD_GOLD', 1),
	('BUILDING_KABUKI_THEATER', 'BUILDINGCLASS_MUSICIANS_GUILD', 'YIELD_GOLD', 1);

INSERT INTO Building_YieldFromInternalTREnd
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KABUKI_THEATER', 'YIELD_CULTURE', 50);

INSERT INTO Building_YieldFromGPBirthScaledWithWriterBulb
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KABUKI_THEATER', 'YIELD_GOLD', 5);

INSERT INTO Building_YieldFromGPBirthScaledWithArtistBulb
	(BuildingType, YieldType, Yield)
VALUES
	('BUILDING_KABUKI_THEATER', 'YIELD_SCIENCE', 2);

INSERT INTO Building_YieldFromGPBirthScaledWithPerTurnYield
	(BuildingType, GreatPersonType, YieldIn, YieldOut, Value)
VALUES
	('BUILDING_KABUKI_THEATER', 'GREATPERSON_MUSICIAN', 'YIELD_TOURISM', 'YIELD_FAITH', 100);
