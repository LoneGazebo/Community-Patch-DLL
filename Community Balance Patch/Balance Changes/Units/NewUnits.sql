-- Tercio Class, used later
INSERT INTO UnitClasses (Type, Description, DefaultUnit) 
SELECT 'UNITCLASS_TERCIO', 'TXT_KEY_UNIT_SPANISH_TERCIO', 'UNIT_SPANISH_TERCIO' 
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Cruiser 

INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_CRUISER', 'TXT_KEY_DESC_CRUISER', 'UNIT_CRUISER'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_CRUISER'), ('TXT_KEY_DESC_CRUISER'), ('TXT_KEY_CIV5_CRUISER_PEDIA') , ('TXT_KEY_CIV5_CRUISER_STRATEGY'), ('TXT_KEY_CIV5_CRUISER_HELP'), Requirements, ('35'), ('40'), ('600'), ('4'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_CRUISER'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_ASSAULT_SEA'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_DYNAMITE'), ('TECH_ELECTRONICS'), ('UNITCLASS_BATTLESHIP'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_CRUISER'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('39'), ('COMMUNITY_ATLAS'), ('CRUSIER_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_BATTLESHIP');

INSERT INTO UnitGameplay2DScripts (UnitType, SelectionSound, FirstSelectionSound)
	SELECT 'UNIT_CRUISER', SelectionSound, FirstSelectionSound
	FROM UnitGameplay2DScripts WHERE (UnitType = 'UNIT_BATTLESHIP');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_CRUISER', UnitAIType
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_BATTLESHIP');

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
	SELECT 'UNIT_CRUISER', 'UNITCLASS_BATTLESHIP'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_CRUISER'), FlavorType, Flavor
	FROM Unit_Flavors WHERE (UnitType = 'UNIT_BATTLESHIP');

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CRUISER', 'PROMOTION_ONLY_DEFENSIVE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_CRUISER', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- VOLLEY_GUN 

--INSERT INTO UnitClasses (Type, Description, DefaultUnit )
--	SELECT 'UNITCLASS_VOLLEY_GUN', 'TXT_KEY_DESC_VOLLEY_GUN', 'UNIT_VOLLEY_GUN'
--	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

--INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
--	SELECT	('UNIT_VOLLEY_GUN'), ('TXT_KEY_DESC_VOLLEY_GUN'), ('TXT_KEY_CIV5_VOLLEY_GUN_PEDIA') , ('TXT_KEY_CIV5_VOLLEY_GUN_STRATEGY'), ('TXT_KEY_CIV5_VOLLEY_GUN_HELP'), Requirements, ('15'), ('25'), ('225'), ('225'), ('1'), ('2'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_VOLLEY_GUN'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_RANGED'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, ('1'), PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_METALLURGY'), ('TECH_DYNAMITE'), ('UNITCLASS_GATLINGGUN'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
	--		('ART_DEF_UNIT_VOLLEY_GUN'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('59'), ('COMMUNITY_ATLAS'), ('VOLLEY_FLAG_ATLAS')
--	FROM Units WHERE (Type = 'UNIT_GATLINGGUN');

--INSERT INTO UnitGameplay2DScripts (UnitType, SelectionSound, FirstSelectionSound)
--	SELECT 'UNIT_VOLLEY_GUN', SelectionSound, FirstSelectionSound
--	FROM UnitGameplay2DScripts WHERE (UnitType = 'UNIT_CANNON');

--INSERT INTO Unit_AITypes (UnitType, UnitAIType)
--	SELECT 'UNIT_VOLLEY_GUN', 'UNITAI_RANGED'
--	FROM Unit_AITypes WHERE (UnitType = 'UNIT_GATLINGGUN');

--INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
--	SELECT 'UNIT_VOLLEY_GUN', 'UNITCLASS_GATLINGGUN'
--	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

--INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	--SELECT 'UNIT_VOLLEY_GUN', 'PROMOTION_ONLY_DEFENSIVE'
	--WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

--INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
--	SELECT ('UNIT_VOLLEY_GUN'), FlavorType, Flavor
--	FROM Unit_Flavors WHERE (UnitType = 'UNIT_GATLINGGUN');


-- Mounted XBOW
INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_MOUNTED_BOWMAN', 'TXT_KEY_UNIT_MOUNTED_BOWMAN', 'UNIT_MOUNTED_BOWMAN'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_MOUNTED_BOWMAN'), ('TXT_KEY_DESC_MOUNTED_BOWMAN'), ('TXT_KEY_CIV5_MOUNTED_BOWMAN_PEDIA') , ('TXT_KEY_CIV5_MOUNTED_BOWMAN_STRATEGY'), ('TXT_KEY_CIV5_MOUNTED_BOWMAN_HELP'), Requirements, ('8'), ('17'), ('175'), ('200'), ('1'), ('4'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_MOUNTED_BOWMAN'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_RANGED'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, ('1'), PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_PHYSICS'), ('TECH_METALLURGY'), ('UNITCLASS_CUIRASSIER'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_HEAVY_SKIRMISH'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('30'), ('COMMUNITY_ATLAS'), ('GENGHIS_UNIT_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO UnitGameplay2DScripts (UnitType, SelectionSound, FirstSelectionSound)
	SELECT 'UNIT_MOUNTED_BOWMAN', SelectionSound, FirstSelectionSound
	FROM UnitGameplay2DScripts WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'UNITAI_RANGED'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'UNITAI_COUNTER'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'UNITCLASS_CUIRASSIER'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'RESOURCE_HORSE', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'PROMOTION_NO_DEFENSIVE_BONUSES'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'PROMOTION_ONLY_DEFENSIVE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_MOUNTED_BOWMAN', 'PROMOTION_CITY_PENALTY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_MOUNTED_BOWMAN'), 'FLAVOR_OFFENSE', '8'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_MOUNTED_BOWMAN'), 'FLAVOR_MOBILE', '8'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_MOUNTED_BOWMAN'), 'FLAVOR_RANGED', '5'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Cuirassier
INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_CUIRASSIER', 'TXT_KEY_DESC_CUIRASSIER', 'UNIT_CUIRASSIER'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_CUIRASSIER'), ('TXT_KEY_DESC_CUIRASSIER'), ('TXT_KEY_CIV5_CUIRASSIER_PEDIA') , ('TXT_KEY_CIV5_CUIRASSIER_STRATEGY'), ('TXT_KEY_CIV5_CUIRASSIER_HELP'), Requirements, ('18'), ('23'), ('250'), ('250'), ('1'), ('4'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_CUIRASSIER'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_RANGED'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, ('1'), PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_METALLURGY'), ('TECH_MILITARY_SCIENCE'), ('UNITCLASS_CAVALRY'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_CUIRASSIER'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('29'), ('COMMUNITY_ATLAS'), ('CUIRASSIER_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO UnitGameplay2DScripts (UnitType, SelectionSound, FirstSelectionSound)
	SELECT 'UNIT_CUIRASSIER', SelectionSound, FirstSelectionSound
	FROM UnitGameplay2DScripts WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_CUIRASSIER', 'UNITAI_RANGED'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_CUIRASSIER', 'UNITAI_COUNTER'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_HUN_HORSE_ARCHER');

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
	SELECT 'UNIT_CUIRASSIER', 'UNITCLASS_CAVALRY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_CUIRASSIER', 'RESOURCE_HORSE', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CUIRASSIER', 'PROMOTION_NO_DEFENSIVE_BONUSES'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CUIRASSIER', 'PROMOTION_ONLY_DEFENSIVE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CUIRASSIER', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_CUIRASSIER', 'PROMOTION_CITY_PENALTY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_CUIRASSIER'), 'FLAVOR_OFFENSE', '10'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_CUIRASSIER'), 'FLAVOR_MOBILE', '11'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_CUIRASSIER'), 'FLAVOR_RANGED', '9'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Field Gun

-- Cuirassier
INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_FIELD_GUN', 'TXT_KEY_UNIT_FIELD_GUN', 'UNIT_FIELD_GUN'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_FIELD_GUN'), ('TXT_KEY_DESC_FIELD_GUN'), ('TXT_KEY_CIV5_FIELD_GUN_PEDIA') , ('TXT_KEY_CIV5_FIELD_GUN_STRATEGY'), ('TXT_KEY_CIV5_FIELD_GUN_HELP'), Requirements, ('16'), ('40'), ('250'), ('250'), ('1'), Moves, Immobile, Range, BaseSightRange, ('UNITCLASS_FIELD_GUN'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_RIFLING'), ('TECH_BALLISTICS'), ('UNITCLASS_ARTILLERY'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_FIELD_GUN'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('7'), ('COMMUNITY_2_ATLAS'), ('FIELD_GUN_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_ARTILLERY');

INSERT INTO UnitGameplay2DScripts (UnitType, SelectionSound, FirstSelectionSound)
	SELECT 'UNIT_FIELD_GUN', SelectionSound, FirstSelectionSound
	FROM UnitGameplay2DScripts WHERE (UnitType = 'UNIT_ARTILLERY');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_FIELD_GUN', 'UNITAI_CITY_BOMBARD'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_ARTILLERY');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_FIELD_GUN', 'UNITAI_RANGED'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_ARTILLERY');

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
	SELECT 'UNIT_FIELD_GUN', 'UNITCLASS_ARTILLERY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_ResourceQuantityRequirements (UnitType, ResourceType, Cost)
	SELECT 'UNIT_FIELD_GUN', 'RESOURCE_IRON', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_NO_DEFENSIVE_BONUSES'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_ONLY_DEFENSIVE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_CITY_SIEGE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_MUST_SET_UP'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_SIGHT_PENALTY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_SIEGE_INACCURACY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_FIELD_GUN', 'PROMOTION_COVER_1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Mercenaries

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit)
VALUES
	('UNITCLASS_GUERILLA', 'TXT_KEY_UNIT_GUERILLA', 'UNIT_GUERILLA');

INSERT INTO Units
	(Class, Type, PrereqTech, Combat, Cost, FaithCost, PurchaseOnly, RequiresFaithPurchaseEnabled, Moves, CombatClass, Domain, MoveAfterPurchase, DefaultUnitAI, Description, Civilopedia, Strategy, Help, MilitarySupport, MilitaryProduction, Pillage, PolicyType, IgnoreBuildingDefense, AdvancedStartCost, XPValueAttack, XPValueDefense, Conscription, UnitArtInfo, UnitFlagAtlas, UnitFlagIconOffset, IconAtlas, PortraitIndex)
VALUES
	('UNITCLASS_GUERILLA', 'UNIT_GUERILLA', 'TECH_PENICILIN', 65, 350, 0, 1, 'false', 2, 'UNITCOMBAT_GUN', 'DOMAIN_LAND', 1, 'UNITAI_FAST_ATTACK', 'TXT_KEY_UNIT_GUERILLA', 'TXT_KEY_CIV5_GUERILLA_TEXT', 'TXT_KEY_UNIT_GUERILLA_STRATEGY', 'TXT_KEY_UNIT_HELP_GUERILLA', 1, 1, 1, 'POLICY_HONOR_FINISHER', 1, 40, 3, 3, 3, 'ART_DEF_UNIT_MERC', 'MERC_FLAG_ATLAS', 0, 'COMMUNITY_ATLAS', 52);

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_GUERILLA', 'UNITCLASS_MECHANIZED_INFANTRY'),
	('UNIT_MARINE', 'UNITCLASS_MECHANIZED_INFANTRY');

INSERT INTO UnitGameplay2DScripts
	(UnitType, SelectionSound, FirstSelectionSound)
VALUES
	('UNIT_GUERILLA', 'AS2D_SELECT_INFANTRY', 'AS2D_BIRTH_INFANTRY');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_GUERILLA', 'UNITAI_ATTACK'),
	('UNIT_GUERILLA', 'UNITAI_DEFENSE'),
	('UNIT_GUERILLA', 'UNITAI_FAST_ATTACK');

INSERT INTO Unit_Flavors
	(UnitType, FlavorType, Flavor)
VALUES
	('UNIT_GUERILLA', 'FLAVOR_OFFENSE', 20),
	('UNIT_GUERILLA', 'FLAVOR_DEFENSE', 15);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_GUERILLA', 'PROMOTION_AMPHIBIOUS'),
	('UNIT_GUERILLA', 'PROMOTION_DEFENSIVE_EMBARKATION'),
	('UNIT_GUERILLA', 'PROMOTION_FLANK_ATTACK_BONUS');

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit)
VALUES
	('UNITCLASS_EXPLORER', 'TXT_KEY_UNIT_EXPLORER', 'UNIT_EXPLORER'),
	('UNITCLASS_ZEPPELIN', 'TXT_KEY_UNIT_ZEPPELIN', 'UNIT_ZEPPELIN');

INSERT INTO Units
	(Class, Type, Combat, RangedCombat, Range, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, PrereqTech, ObsoleteTech, CombatClass, Domain, NoBadGoodies, DefaultUnitAI, Description, Civilopedia, Strategy, Help, Pillage, MilitarySupport, GoodyHutUpgradeUnitClass, MilitaryProduction, AdvancedStartCost, XPValueAttack, XPValueDefense, UnitArtInfo, UnitFlagAtlas, UnitFlagIconOffset, PortraitIndex, IconAtlas, MoveRate)
VALUES
	('UNITCLASS_EXPLORER', 'UNIT_EXPLORER', 16, 0, 0, 180, 200, 1, 3, 'TECH_COMPASS', 'TECH_STEAM_POWER', 'UNITCOMBAT_RECON', 'DOMAIN_LAND', 1, 'UNITAI_EXPLORE', 'TXT_KEY_UNIT_EXPLORER_CBP', 'TXT_KEY_CIV5_EXPLORER_TEXT_CBP', 'TXT_KEY_UNIT_EXPLORER_STRATEGY_CBP', 'TXT_KEY_UNIT_HELP_EXPLORER_CBP', 1, 1, 'UNITCLASS_ZEPPELIN', 1, 30, 3, 3, 'ART_DEF_UNIT_EXPLORER_CBP', 'EXPANSION2_UNIT_FLAG_ATLAS', 5, 58, 'COMMUNITY_ATLAS', 'BIPED'),
	('UNITCLASS_ZEPPELIN', 'UNIT_ZEPPELIN', 18, 25, 1, 300, 300, 1, 3, 'TECH_STEAM_POWER', 'TECH_ROCKETRY', 'UNITCOMBAT_RECON', 'DOMAIN_LAND', 1, 'UNITAI_EXPLORE', 'TXT_KEY_UNIT_ZEPPELIN_CBP', 'TXT_KEY_CIV5_ZEPPELIN_TEXT_CBP', 'TXT_KEY_UNIT_ZEPPELIN_STRATEGY_CBP', 'TXT_KEY_UNIT_HELP_ZEPPELIN_CBP', 1, 1, 'UNITCLASS_PARATROOPER', 1, 30, 3, 3, 'ART_DEF_UNIT_SMOKEY_STEAM_AIRSHIP_LEVEL_1', 'EXPANSION_SCEN_UNIT_FLAG_ATLAS', 7, 7, 'EXPANSION_SCEN_UNIT_ATLAS', 'WHEELED');

INSERT INTO ArtDefine_UnitInfos(Type, DamageStates, Formation)
  VALUES ('ART_DEF_UNIT_EXPLORER_CBP',1,'Scout'),
	('ART_DEF_UNIT_PIONEER',1,'LooseCivilian'),
	('ART_DEF_UNIT_COLONIST',1,'Vehicle');

INSERT INTO ArtDefine_UnitInfoMemberInfos(UnitInfoType, UnitMemberInfoType, NumMembers)
  VALUES ('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_1', 1),
	('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_2', 1),
	('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_3', 1),
	('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_1', 1),
	('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_2', 1),
	('ART_DEF_UNIT_EXPLORER_CBP','ART_DEF_UNIT_MEMBER_BANDEIRANTE_3', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_EUROFEMALE18', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_EUROMALE20', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_EUROFEMALE18', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_EURODONKEY', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_GREATMERCHANT_EARLY_CAMEL_V1', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_CARAVAN_F1', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_CARAVAN_F2', 1),
	('ART_DEF_UNIT_PIONEER','ART_DEF_UNIT_MEMBER_GREATMERCHANT_EARLY_CAMEL_V2', 1),
	('ART_DEF_UNIT_COLONIST','ART_DEF_UNIT_MEMBER_COLONIST2', 1),
	('ART_DEF_UNIT_COLONIST','ART_DEF_UNIT_MEMBER_COLONIST3', 1),
	('ART_DEF_UNIT_COLONIST','ART_DEF_UNIT_MEMBER_COLONIST4', 1);

INSERT INTO ArtDefine_UnitMemberInfos(Type, Scale, Model, MaterialTypeTag, MaterialTypeSoundOverrideTag)
  VALUES ('ART_DEF_UNIT_MEMBER_COLONIST2', 0.07, 'GreatMerchant_Late.fxsxml', 'CLOTH', 'FLESH'),
	('ART_DEF_UNIT_MEMBER_COLONIST3', 0.05, 'Caravan_Late_F1.fxsxml', 'CLOTH', 'FLESH'),
	('ART_DEF_UNIT_MEMBER_COLONIST4', 0.05, 'Caravan_Late_F2.fxsxml', 'CLOTH', 'FLESH');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_EXPLORER', 'UNITAI_EXPLORE'),
	('UNIT_ZEPPELIN', 'UNITAI_EXPLORE');

INSERT INTO Unit_Flavors
	(UnitType, FlavorType, Flavor)
VALUES
	('UNIT_EXPLORER', 'FLAVOR_RECON', 10),
	('UNIT_ZEPPELIN', 'FLAVOR_RECON', 15);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_EXPLORER', 'PROMOTION_DEFENSIVE_EMBARKATION'),
	('UNIT_EXPLORER', 'PROMOTION_EMBARKED_SIGHT'),
	('UNIT_EXPLORER', 'PROMOTION_IGNORE_TERRAIN_COST'),
	('UNIT_EXPLORER', 'PROMOTION_RECON_EXPERIENCE'),
	('UNIT_ZEPPELIN', 'PROMOTION_HOVERING_UNIT'),
	('UNIT_ZEPPELIN', 'PROMOTION_IGNORE_TERRAIN_COST'),
	('UNIT_ZEPPELIN', 'PROMOTION_RECON_EXPERIENCE'),
	('UNIT_ZEPPELIN', 'PROMOTION_EXTRA_SIGHT_I'),
	('UNIT_ZEPPELIN', 'PROMOTION_HOVER_UNIT');


INSERT INTO UnitGameplay2DScripts
	(UnitType, SelectionSound, FirstSelectionSound)
VALUES
	('UNIT_EXPLORER', 'AS2D_SELECT_SCOUT', 'AS2D_BIRTH_SCOUT'),
	('UNIT_ZEPPELIN', 'AS2D_SELECT_SCOUT', 'AS2D_BIRTH_SCOUT');

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_SCOUT', 'UNITCLASS_EXPLORER'),
	('UNIT_SHOSHONE_PATHFINDER', 'UNITCLASS_EXPLORER'),
	('UNIT_EXPLORER', 'UNITCLASS_ZEPPELIN'),
	('UNIT_ZEPPELIN', 'UNITCLASS_PARATROOPER');


	INSERT INTO Units
	(Class, Type, Moves, Capture, CivilianAttackPriority, HurryCostModifier, Domain, DefaultUnitAI, Description, Civilopedia, Strategy, Help, PrereqTech, ObsoleteTech, Food, Found, FoundMid, FoundLate, CombatLimit, UnitArtInfo, UnitArtInfoCulturalVariation, PortraitIndex, IconAtlas)
VALUES
	('UNITCLASS_PIONEER', 'UNIT_PIONEER', 3, 'UNITCLASS_WORKER', 'CIVILIAN_ATTACK_PRIORITY_HIGH_EARLY_GAME_ONLY', -1, 'DOMAIN_LAND', 'UNITAI_SETTLE', 'TXT_KEY_UNIT_PIONEER', 'TXT_KEY_CIV5_PIONEER_TEXT', 'TXT_KEY_UNIT_PIONEER_STRATEGY', 'TXT_KEY_UNIT_HELP_PIONEER', 'TECH_BANKING', 'TECH_BIOLOGY', 1, 1, 1, 0, 0, 'ART_DEF_UNIT_PIONEER', 1, 2, 'COMMUNITY_ATLAS'),
	('UNITCLASS_COLONIST', 'UNIT_COLONIST', 4, 'UNITCLASS_WORKER', 'CIVILIAN_ATTACK_PRIORITY_HIGH_EARLY_GAME_ONLY', -1, 'DOMAIN_LAND', 'UNITAI_SETTLE', 'TXT_KEY_UNIT_COLONIST', 'TXT_KEY_CIV5_COLONIST_TEXT', 'TXT_KEY_UNIT_COLONIST_STRATEGY', 'TXT_KEY_UNIT_HELP_COLONIST', 'TECH_BIOLOGY', NULL, 1, 1, 0, 1, 0, 'ART_DEF_UNIT_COLONIST', 1, 5, 'EXPANSION2_UNIT_ATLAS');

INSERT INTO UnitGameplay2DScripts
	(UnitType, SelectionSound, FirstSelectionSound)
VALUES
	('UNIT_PIONEER', 'AS2D_SELECT_SETTLER', 'AS2D_BIRTH_SETTLER'),
	('UNIT_COLONIST', 'AS2D_SELECT_SETTLER', 'AS2D_BIRTH_SETTLER');

INSERT INTO Unit_AITypes
	(UnitType, UnitAIType)
VALUES
	('UNIT_PIONEER', 'UNITAI_SETTLE'),
	('UNIT_COLONIST', 'UNITAI_SETTLE');

INSERT INTO Unit_Flavors
	(UnitType, FlavorType, Flavor)
VALUES
	('UNIT_PIONEER', 'FLAVOR_EXPANSION', 25),
	('UNIT_COLONIST', 'FLAVOR_EXPANSION', 25);

INSERT INTO Unit_FreePromotions
	(UnitType, PromotionType)
VALUES
	('UNIT_PIONEER', 'PROMOTION_EXTRA_SIGHT_I'),
	('UNIT_PIONEER', 'PROMOTION_DEFENSIVE_EMBARKATION'),
	('UNIT_COLONIST', 'PROMOTION_EXTRA_SIGHT_I'),
	('UNIT_COLONIST', 'PROMOTION_DEFENSIVE_EMBARKATION'),
	('UNIT_COLONIST', 'PROMOTION_OCEAN_MOVEMENT');

INSERT INTO UnitClasses
	(Type, Description, DefaultUnit)
VALUES
	('UNITCLASS_PIONEER', 'TXT_KEY_UNIT_PIONEER', 'UNIT_PIONEER'),
	('UNITCLASS_COLONIST', 'TXT_KEY_UNIT_COLONIST', 'UNIT_COLONIST');

INSERT INTO Unit_ClassUpgrades
	(UnitType, UnitClassType)
VALUES
	('UNIT_SETTLER', 'UNITCLASS_PIONEER'),
	('UNIT_PIONEER', 'UNITCLASS_COLONIST');

INSERT INTO Trait_NoTrain
	(TraitType, UnitClassType)
VALUES
	('TRAIT_SUPER_CITY_STATE', 'UNITCLASS_PIONEER'),
	('TRAIT_SUPER_CITY_STATE', 'UNITCLASS_COLONIST');

-- Pioneer/Colonist Change

INSERT INTO Defines (
Name, Value)
SELECT 'PIONEER_POPULATION_CHANGE', '3';

INSERT INTO Defines (
Name, Value)
SELECT 'PIONEER_EXTRA_PLOTS', '3';

INSERT INTO Defines (
Name, Value)
SELECT 'PIONEER_FOOD_PERCENT', '25';

INSERT INTO Defines (
Name, Value)
SELECT 'COLONIST_POPULATION_CHANGE', '5';

INSERT INTO Defines (
Name, Value)
SELECT 'COLONIST_EXTRA_PLOTS', '5';

INSERT INTO Defines (
Name, Value)
SELECT 'COLONIST_FOOD_PERCENT', '50';

UPDATE Units
SET ObsoleteTech = 'TECH_BANKING'
WHERE Class = 'UNITCLASS_SETTLER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='SETTLER_COMMUNITY' AND Value= 1);

INSERT INTO Unit_BuildOnFound
	(UnitType, BuildingClassType)
VALUES
	('UNIT_PIONEER', 'BUILDINGCLASS_GROVE'),
	('UNIT_PIONEER', 'BUILDINGCLASS_GRANARY'),
	('UNIT_PIONEER', 'BUILDINGCLASS_MARKET'),
	('UNIT_PIONEER', 'BUILDINGCLASS_MONUMENT'),
	('UNIT_PIONEER', 'BUILDINGCLASS_SHRINE'),
	('UNIT_PIONEER', 'BUILDINGCLASS_BARRACKS'),
	('UNIT_PIONEER', 'BUILDINGCLASS_WATERMILL'),
	('UNIT_PIONEER', 'BUILDINGCLASS_LIBRARY'),
	('UNIT_PIONEER', 'BUILDINGCLASS_FORGE'),
	('UNIT_PIONEER', 'BUILDINGCLASS_WELL'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_GROVE'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_GRANARY'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_ARMORY'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_MARKET'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_MONUMENT'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_SHRINE'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_BARRACKS'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_WATERMILL'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_LIBRARY'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_FORGE'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_WELL'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_HARBOR'),
	('UNIT_SPANISH_CONQUISTADOR', 'BUILDINGCLASS_CASTLE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_GROVE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_GRANARY'),
	('UNIT_COLONIST', 'BUILDINGCLASS_MARKET'),
	('UNIT_COLONIST', 'BUILDINGCLASS_MONUMENT'),
	('UNIT_COLONIST', 'BUILDINGCLASS_BARRACKS'),
	('UNIT_COLONIST', 'BUILDINGCLASS_SHRINE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_AQUEDUCT'),
	('UNIT_COLONIST', 'BUILDINGCLASS_WATERMILL'),
	('UNIT_COLONIST', 'BUILDINGCLASS_LIBRARY'),
	('UNIT_COLONIST', 'BUILDINGCLASS_FORGE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_WINDMILL'),
	('UNIT_COLONIST', 'BUILDINGCLASS_HARBOR'),
	('UNIT_COLONIST', 'BUILDINGCLASS_LIGHTHOUSE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_AMPHITHEATER'),
	('UNIT_COLONIST', 'BUILDINGCLASS_TEMPLE'),
	('UNIT_COLONIST', 'BUILDINGCLASS_WORKSHOP'),
	('UNIT_COLONIST', 'BUILDINGCLASS_WELL');
