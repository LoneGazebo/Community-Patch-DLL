-- Unitinfos --

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_MESSENGER', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_ENVOY', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_DIPLOMAT', '1','UnFormed';

INSERT INTO ArtDefine_UnitInfos (
	Type, DamageStates, Formation)
SELECT 'ART_DEF_UNIT_AMBASSADOR', '1','UnFormed';

-- Unit Members --

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_MESSENGER','ART_DEF_UNIT_MEMBER_MISSIONARY_01', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_ENVOY','ART_DEF_UNIT_MEMBER_EUROFEMALE18', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_DIPLOMAT','ART_DEF_UNIT_MEMBER_SETTLERS_ASIAN_F2', '1';

INSERT INTO ArtDefine_UnitInfoMemberInfos (
	UnitInfoType, UnitMemberInfoType, NumMembers)
SELECT 'ART_DEF_UNIT_AMBASSADOR','ART_DEF_UNIT_MEMBER_ARCHAEOLOGIST', '1';

-- Vanilla - National Wonder Building Requirements
INSERT INTO Building_PrereqBuildingClasses (
  BuildingType, BuildingClassType, NumBuildingNeeded)
SELECT 'BUILDING_GUTENBERG_PRESS', 'BUILDINGCLASS_SCRIBE', '-1'
WHERE NOT EXISTS (SELECT * FROM sqlite_master WHERE name='Building_PrereqBuildingClassesPercentage');

INSERT INTO Building_PrereqBuildingClasses (
  BuildingType, BuildingClassType, NumBuildingNeeded)
SELECT 'BUILDING_FOREIGN_OFFICE', 'BUILDINGCLASS_POSTOFFICE', '-1'
WHERE NOT EXISTS (SELECT * FROM sqlite_master WHERE name='Building_PrereqBuildingClassesPercentage');

-- TBC Compatibility - National Wonder Building Requirements
CREATE TABLE IF NOT EXISTS Building_PrereqBuildingClassesPercentage (
  BuildingType text,
  BuildingClassType text,
  PercentBuildingNeeded integer default 0
);

INSERT INTO Building_PrereqBuildingClassesPercentage (
  BuildingType, BuildingClassType, PercentBuildingNeeded)
SELECT 'BUILDING_GUTENBERG_PRESS', 'BUILDINGCLASS_SCRIBE', '75'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value = 2 );

INSERT INTO Building_PrereqBuildingClassesPercentage (
  BuildingType, BuildingClassType, PercentBuildingNeeded)
SELECT 'BUILDING_FOREIGN_OFFICE', 'BUILDINGCLASS_POSTOFFICE', '75'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='USING_VEM' AND Value = 2 );

--Diplomatic Mission (Easy)
UPDATE Defines
SET Value = 60
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION' AND Value= 3 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Diplomatic Mission (Normal)
UPDATE Defines
SET Value = 50
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION' AND Value= 2 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Diplomatic Mission (Hard)
UPDATE Defines
SET Value = 40
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION' AND Value= 1 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Diplomatic Mission Vanilla (Easy)
UPDATE Defines
SET Value = 60
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION_VANILLA' AND Value= 3 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 0);

--Diplomatic Mission Vanilla (Normal)
UPDATE Defines
SET Value = 50
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION_VANILLA' AND Value= 2 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 0);

--Diplomatic Mission (Hard)
UPDATE Defines
SET Value = 40
WHERE Name = 'MINOR_FRIENDSHIP_FROM_TRADE_MISSION'
AND EXISTS (SELECT * FROM CSD WHERE Type='TRADE_MISSION_VANILLA' AND Value= 1 )
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 0);

--Sweden Trait (Bonus)
INSERT INTO Trait_FreePromotionUnitCombats 
(TraitType, UnitCombatType, PromotionType)
SELECT 'TRAIT_DIPLOMACY_GREAT_PEOPLE', 'UNITCOMBAT_DIPLOMACY', 'PROMOTION_NOBEL_LAUREATE'
WHERE EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Sweden Trait (Bonus Text)
UPDATE Language_en_US
SET Text = 'Diplomatic Units start with the Nobel Laureate promotion, which grants +20% [ICON_INFLUENCE] from Diplomatic Missions. When declaring friendship, Sweden and friend gain +10% Great Person generation.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE'
AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Sweden Trait (Very Easy)
UPDATE Traits
SET GreatPersonGiftInfluence = 40
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE'
AND EXISTS (SELECT * FROM CSD WHERE Type='SWEDEN_TRAIT' AND Value= 3 ) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Sweden Trait (Easy)
UPDATE Traits
SET GreatPersonGiftInfluence = 20
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE'
AND EXISTS (SELECT * FROM CSD WHERE Type='SWEDEN_TRAIT' AND Value= 2 ) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Sweden Trait (Normal, Default)
UPDATE Traits
SET GreatPersonGiftInfluence = 0
WHERE Type = 'TRAIT_DIPLOMACY_GREAT_PEOPLE'
AND EXISTS (SELECT * FROM CSD WHERE Type='SWEDEN_TRAIT' AND Value= 1 ) AND EXISTS (SELECT * FROM CSD WHERE Type='USING_GAK' AND Value= 1);

--Gold:Influence Ratio (Hard)
UPDATE Defines
SET Value = 24
WHERE Name = 'GOLD_GIFT_FRIENDSHIP_DIVISOR' AND EXISTS (SELECT * FROM CSD WHERE Type='INFLUENCE_RATIO' AND Value= 3 );

--Gold:Influence Ratio (Normal, Default)
UPDATE Defines
SET Value = 18
WHERE Name = 'GOLD_GIFT_FRIENDSHIP_DIVISOR' AND EXISTS (SELECT * FROM CSD WHERE Type='INFLUENCE_RATIO' AND Value= 2 );

--Gold:Influence Ratio (Easy)
UPDATE Defines
SET Value = 12
WHERE Name = 'GOLD_GIFT_FRIENDSHIP_DIVISOR' AND EXISTS (SELECT * FROM CSD WHERE Type='INFLUENCE_RATIO' AND Value= 1 );

--Gold Gift Amount (Small- Default)
UPDATE Defines
SET Value = 600
WHERE Name = 'MINOR_GOLD_GIFT_SMALL' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_SMALL' AND Value= 2 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Small- Higher)
UPDATE Defines
SET Value = 800
WHERE Name = 'MINOR_GOLD_GIFT_SMALL' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_SMALL' AND Value= 3 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Small- Lower)
UPDATE Defines
SET Value = 400
WHERE Name = 'MINOR_GOLD_GIFT_SMALL' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_SMALL' AND Value= 1 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Medium- Default)
UPDATE Defines
SET Value = 1000
WHERE Name = 'MINOR_GOLD_GIFT_MEDIUM' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_MEDIUM' AND Value= 2 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Medium- Higher)
UPDATE Defines
SET Value = 1300
WHERE Name = 'MINOR_GOLD_GIFT_MEDIUM' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_MEDIUM' AND Value= 3 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Medium- Lower)
UPDATE Defines
SET Value = 700
WHERE Name = 'MINOR_GOLD_GIFT_MEDIUM' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_MEDIUM' AND Value= 1 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Large- Default)
UPDATE Defines
SET Value = 2000
WHERE Name = 'MINOR_GOLD_GIFT_LARGE' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_LARGE' AND Value= 2 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Large- Higher)
UPDATE Defines
SET Value = 2500
WHERE Name = 'MINOR_GOLD_GIFT_LARGE' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_LARGE' AND Value= 3 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gift Amount (Large- Lower)
UPDATE Defines
SET Value = 1500
WHERE Name = 'MINOR_GOLD_GIFT_LARGE' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_LARGE' AND Value= 1 ) AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 1 );

--Gold Gifting Off
UPDATE Defines
SET Value = 0
WHERE Name = 'MINOR_GOLD_GIFT_LARGE' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

UPDATE Defines
SET Value = 0
WHERE Name = 'MINOR_GOLD_GIFT_MEDIUM' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

UPDATE Defines
SET Value = 0
WHERE Name = 'MINOR_GOLD_GIFT_SMALL' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

UPDATE Defines
SET Value = 0
WHERE Name = 'MINOR_CIV_GOLD_GIFT_MINIMUM_FRIENDSHIP_REWARD' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

--Investment and Give Gold Quests Off
UPDATE Defines
SET Value = 1
WHERE Name = 'QUEST_DISABLED_INVEST' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

UPDATE Defines
SET Value = 1
WHERE Name = 'QUEST_DISABLED_GIVE_GOLD' AND EXISTS (SELECT * FROM CSD WHERE Type='GIFT_OPTION' AND Value= 2 );

