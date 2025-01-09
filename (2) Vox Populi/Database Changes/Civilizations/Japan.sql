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
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_BASTION_FORT', 'YIELD_CULTURE', 1),
	('TRAIT_FIGHT_WELL_DAMAGED', 'BUILDINGCLASS_BASTION_FORT', 'YIELD_FAITH', 1),
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
-- Unique Unit: Yamato (Battleship)
----------------------------------------------------------
INSERT INTO Civilization_UnitClassOverrides
	(CivilizationType, UnitClassType, UnitType)
VALUES
	('CIVILIZATION_JAPAN', 'UNITCLASS_BATTLESHIP', 'UNIT_YAMATO');

UPDATE Units
SET
	ObsoleteTech = (
		SELECT ObsoleteTech FROM Units WHERE Type = (
			SELECT DefaultUnit FROM UnitClasses WHERE Type = (
				SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = 'UNIT_BATTLESHIP'
			)
		)
	),
	Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_BATTLESHIP') + 5,
	RangedCombat = (SELECT RangedCombat FROM Units WHERE Type = 'UNIT_BATTLESHIP') + 10,
	"Range" = (SELECT "Range" FROM Units WHERE Type = 'UNIT_BATTLESHIP') + 1
WHERE Type = 'UNIT_YAMATO';

INSERT INTO Unit_YieldOnCompletion
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_YAMATO', 'YIELD_GOLDEN_AGE_POINTS', 600);

INSERT INTO Unit_Bounties
	(UnitType, YieldType, Yield)
VALUES
	('UNIT_YAMATO', 'YIELD_CULTURE', 400),
	('UNIT_YAMATO', 'YIELD_GOLDEN_AGE_POINTS', 400);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_YAMATO', 'PROMOTION_ARMOR_PLATING_1'),
	('UNIT_YAMATO', 'PROMOTION_TAIKAN_KYOHO');

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
