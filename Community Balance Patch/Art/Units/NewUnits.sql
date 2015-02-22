-- Cruiser 

INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_CRUISER', 'TXT_KEY_DESC_CRUISER', 'UNIT_CRUISER'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_CRUISER'), ('TXT_KEY_DESC_CRUISER'), ('TXT_KEY_CIV5_CRUISER_PEDIA') , ('TXT_KEY_CIV5_CRUISER_STRATEGY'), ('TXT_KEY_CIV5_CRUISER_HELP'), Requirements, ('35'), ('40'), ('375'), ('4'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_CRUISER'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_ASSAULT_SEA'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_DYNAMITE'), ('TECH_ELECTRONICS'), ('UNITCLASS_BATTLESHIP'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_CRUISER'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('39'), ('COMMUNITY_ATLAS'), ('CRUSIER_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_BATTLESHIP');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_CRUISER', 'UNITAI_ASSAULT_SEA'
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
	SELECT 'UNIT_CRUISER', 'RESOURCE_COAL', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_DESC_CRUISER', 'Cruiser'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_CRUISER_PEDIA', 'A cruiser is a type of warship. The term has been in use for several hundred years, and has had different meanings throughout this period. During the Age of Sail, the term cruising referred to certain kinds of missions – independent scouting, raiding or commerce protection – fulfilled by a frigate or sloop, which were the cruising warships of a fleet. In the middle of the 19th century, cruiser came to be a classification for the ships intended for this kind of role, though cruisers came in a wide variety of sizes, from the small protected cruiser to armored cruisers which were as large (though not as powerful) as a battleship. By the early 20th century, cruisers could be placed on a consistent scale of warship size, smaller than a battleship but larger than a destroyer. In 1922, the Washington Naval Treaty placed a formal limit on cruisers, which were defined as warships of up to 10,000 tons displacement carrying guns no larger than 8 inches in calibre. These limits shaped cruisers up until the end of World War II. The very large battlecruisers of the World War I era were now classified, along with battleships, as capital ships.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_CRUISER_STRATEGY', 'Build Cruisers to augment your Ironclads, and to take control of the seas! Make sure you have plenty of coal, however.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_CRUISER_HELP', 'Industrial-Era ranged naval unit designed to support naval invasions, and to wreak havoc on the wooden ships of previous eras.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- VOLLEY_GUN 

INSERT INTO UnitClasses (Type, Description, DefaultUnit )
	SELECT 'UNITCLASS_VOLLEY_GUN', 'TXT_KEY_DESC_VOLLEY_GUN', 'UNIT_VOLLEY_GUN'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Units (Type, Description, Civilopedia, Strategy, Help, Requirements, Combat, RangedCombat, Cost, FaithCost, RequiresFaithPurchaseEnabled, Moves, Immobile, Range, BaseSightRange, Class, Special, Capture, CombatClass, Domain, CivilianAttackPriority, DefaultUnitAI, Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, PrereqTech, ObsoleteTech, GoodyHutUpgradeUnitClass, HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness, UnitArtInfo, UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, UnitFlagIconOffset, PortraitIndex, IconAtlas, UnitFlagAtlas)
	SELECT	('UNIT_VOLLEY_GUN'), ('TXT_KEY_DESC_VOLLEY_GUN'), ('TXT_KEY_CIV5_VOLLEY_GUN_PEDIA') , ('TXT_KEY_CIV5_VOLLEY_GUN_STRATEGY'), ('TXT_KEY_CIV5_VOLLEY_GUN_HELP'), Requirements, ('15'), ('25'), ('225'), ('450'), ('true'), ('2'), Immobile, ('2'), BaseSightRange, ('UNITCLASS_VOLLEY_GUN'), Special, Capture, CombatClass, Domain, CivilianAttackPriority, ('UNITAI_RANGED'), Food, NoBadGoodies, RivalTerritory, MilitarySupport, MilitaryProduction, Pillage, Found, FoundAbroad, CultureBombRadius, GoldenAgeTurns, IgnoreBuildingDefense, PrereqResources, Mechanized, Suicide, CaptureWhileEmbarked, ('TECH_METALLURGY'), ('TECH_DYNAMITE'), ('UNITCLASS_GATLINGGUN'), HurryCostModifier, AdvancedStartCost, MinAreaSize, AirUnitCap, NukeDamageLevel, WorkRate, NumFreeTechs, RushBuilding, BaseHurry, HurryMultiplier, BaseGold, NumGoldPerEra, SpreadReligion, CombatLimit, RangeAttackOnlyInDomain, RangeAttackIgnoreLOS, RangedCombatLimit, XPValueAttack, XPValueDefense, SpecialCargo, DomainCargo, Conscription, ExtraMaintenanceCost, NoMaintenance, Unhappiness,
			('ART_DEF_UNIT_VOLLEY_GUN'), UnitArtInfoCulturalVariation, UnitArtInfoEraVariation, ProjectPrereq, SpaceshipProject, LeaderPromotion, LeaderExperience, DontShowYields, ShowInPedia, MoveRate, ('0'), ('59'), ('COMMUNITY_ATLAS'), ('VOLLEY_FLAG_ATLAS')
	FROM Units WHERE (Type = 'UNIT_GATLINGGUN');

INSERT INTO Unit_AITypes (UnitType, UnitAIType)
	SELECT 'UNIT_VOLLEY_GUN', 'UNITAI_RANGED'
	FROM Unit_AITypes WHERE (UnitType = 'UNIT_GATLINGGUN');

INSERT INTO Unit_ClassUpgrades (UnitType, UnitClassType)
	SELECT 'UNIT_VOLLEY_GUN', 'UNITCLASS_GATLINGGUN'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_FreePromotions (UnitType, PromotionType)
	SELECT 'UNIT_VOLLEY_GUN', 'PROMOTION_ONLY_DEFENSIVE'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Unit_Flavors (UnitType, FlavorType, Flavor)
	SELECT ('UNIT_VOLLEY_GUN'), FlavorType, Flavor
	FROM Unit_Flavors WHERE (UnitType = 'UNIT_GATLINGGUN');

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_DESC_VOLLEY_GUN', 'Volley Gun'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_PEDIA', 'A volley gun is a gun with several barrels for firing a number of shots, either simultaneously or in sequence. They differ from modern machine guns in that they lack automatic loading and automatic fire and are limited by the number of barrels bundled together. '
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_STRATEGY', 'Volley Guns are a powerful Renaissance ranged weapon, though they are quite vulnerable to Melee. Keep them away from your front lines.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_HELP', 'Renaissance-Era ranged land unit designed to support your Musketmen.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
