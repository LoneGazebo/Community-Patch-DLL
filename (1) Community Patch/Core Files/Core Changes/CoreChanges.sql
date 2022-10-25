------------------------------
-- GameOptions
------------------------------
INSERT INTO GameOptions
			(Type,								Description,								Help,											"Default",	"SupportsSinglePlayer",	"SupportsMultiplayer")
VALUES		('GAMEOPTION_BARB_GG_GA_POINTS',	'TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS',	'TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS_HELP',	0,	1,	1),
			('GAMEOPTION_EVENTS',	'TXT_KEY_GAME_OPTION_EVENTS',	'TXT_KEY_GAME_OPTION_EVENTS_HELP',	1,	1,	1),
			('GAMEOPTION_GOOD_EVENTS_OFF',	'TXT_KEY_GAME_OPTION_EVENTS_GOOD',	'TXT_KEY_GAME_OPTION_EVENTS_GOOD_HELP',	0,	1,	1),
			('GAMEOPTION_NEUTRAL_EVENTS_OFF',	'TXT_KEY_GAME_OPTION_EVENTS_NEUTRAL',	'TXT_KEY_GAME_OPTION_EVENTS_NEUTRAL_HELP',	0,	1,	1),
			('GAMEOPTION_BAD_EVENTS_OFF',	'TXT_KEY_GAME_OPTION_EVENTS_BAD',	'TXT_KEY_GAME_OPTION_EVENTS_BAD_HELP',	0,	1,	1),
			('GAMEOPTION_TRADE_EVENTS_OFF',	'TXT_KEY_GAME_OPTION_EVENTS_TRADE',	'TXT_KEY_GAME_OPTION_EVENTS_TRADE_HELP',	0,	1,	1),
			('GAMEOPTION_CIV_SPECIFIC_EVENTS_OFF',	'TXT_KEY_GAMEOPTION_CIV_SPECIFIC_EVENTS',	'TXT_KEY_GAMEOPTION_CIV_SPECIFIC_EVENTS_HELP',	0,	1,	1),
			('GAMEOPTION_CHILL_BARBARIANS',	'TXT_KEY_GAMEOPTION_CHILL_BARBARIANS',	'TXT_KEY_GAMEOPTION_CHILL_BARBARIANS_HELP',	0,	1,	1),
			('GAMEOPTION_RANDOM_VICTORY',	'TXT_KEY_GAMEOPTION_RANDOM_VICTORY',	'TXT_KEY_GAMEOPTION_RANDOM_VICTORY_HELP',	0,	1,	1),
			('GAMEOPTION_KEEP_UNMET_PLAYERS_UNKNOWN',	'TXT_KEY_GAMEOPTION_KEEP_UNMET_PLAYERS_UNKNOWN',	'TXT_KEY_GAMEOPTION_KEEP_UNMET_PLAYERS_UNKNOWN_HELP',	0,	0,	1);

------------------------------
-- End GameOptions
------------------------------

-- No pillage and repairing in foreign lands
UPDATE CustomModOptions SET Value = 1 WHERE Name = 'NO_REPAIR_FOREIGN_LANDS';

-- No yield from Ice Features
UPDATE CustomModOptions SET Value = 1 WHERE Name = 'NO_YIELD_ICE';

-- No major civ gifting exploit fix
UPDATE CustomModOptions SET Value = 1 WHERE Name = 'NO_MAJORCIV_GIFTING';

-- No healing on mountains if not city plot
UPDATE CustomModOptions SET Value = 1 WHERE Name = 'NO_HEALING_ON_MOUNTAINS';

-- If player is using Alternate Assyria Trait...choose a free tech
UPDATE CustomModOptions SET Value = 0 WHERE Name = 'ALTERNATE_ASSYRIA_TRAIT';

-- Activates Active Diplomacy in DLL for Multiplayer trade deals between Human and AI
UPDATE CustomModOptions SET Value = 1 WHERE Name = 'ACTIVE_DIPLOMACY';

-- TR fix for religion spread
UPDATE Gamespeeds
SET ReligiousPressureAdjacentCity = 25 WHERE Type = 'GAMESPEED_MARATHON';

UPDATE Gamespeeds
SET ReligiousPressureAdjacentCity = 45 WHERE Type = 'GAMESPEED_EPIC';

UPDATE Gamespeeds
SET ReligiousPressureAdjacentCity = 65 WHERE Type = 'GAMESPEED_STANDARD';

UPDATE Gamespeeds
SET ReligiousPressureAdjacentCity = 95 WHERE Type = 'GAMESPEED_QUICK';

-- Art Stuff for Artifacts
UPDATE GreatWorks SET Image = 'artifact.dds' WHERE GreatWorkClassType = 'GREAT_WORK_ARTIFACT' AND NOT ArtifactClassType = 'ARTIFACT_SARCOPHAGUS';

-- Terrain Fixes
UPDATE Features SET NoImprovement = 0 WHERE Type = 'FEATURE_ICE';
UPDATE Features SET SeeThrough = 0 WHERE Type = 'FEATURE_SOLOMONS_MINES';
UPDATE Features SET SeeThrough = 1 WHERE Type = 'FEATURE_GEYSER';
UPDATE Features SET SeeThrough = 1 WHERE Type = 'FEATURE_FOUNTAIN_YOUTH';
UPDATE Features SET SeeThrough = 0 WHERE Type = 'FEATURE_SOLOMONS_MINES';

-- Shrink Citadel
UPDATE ArtDefine_Landmarks
SET Scale = 0.8
WHERE ImprovementType = 'ART_DEF_IMPROVEMENT_CITADEL';

-- Set Forbidden Palace to 2 votes for CP (we'll reset in CBP)
UPDATE Buildings
SET SingleLeagueVotes = 2
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

UPDATE Buildings
SET ExtraLeagueVotes = 0
WHERE Type = 'BUILDING_FORBIDDEN_PALACE';

-- Unit Promotions
UPDATE UnitPromotions
SET FlankAttackModifier = 5
WHERE FlankAttackModifier = 50;

UPDATE UnitPromotions
SET FlankAttackModifier = 3
WHERE FlankAttackModifier = 25;

-- Resource Priority Changes

UPDATE Resources
SET AITradeModifier = 30
WHERE Type = 'RESOURCE_URANIUM';

UPDATE Resources
SET AITradeModifier = 30
WHERE Type = 'RESOURCE_ALUMINUM';

UPDATE Resources
SET AITradeModifier = 25
WHERE Type = 'RESOURCE_OIL';

UPDATE Resources
SET AITradeModifier = 25
WHERE Type = 'RESOURCE_COAL';

UPDATE Resources
SET AITradeModifier = 20
WHERE Type = 'RESOURCE_IRON';

UPDATE Resources
SET AITradeModifier = 20
WHERE Type = 'RESOURCE_HORSES';

-- Technologies
UPDATE Technology_Flavors
SET Flavor = 50
WHERE TechType = 'TECH_SAILING' AND FlavorType = 'FLAVOR_NAVAL';

-- Policy Helpers
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_LIBERTY';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_TRADITION';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_HONOR';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_PIETY';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_PATRONAGE';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_COMMERCE';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_RATIONALISM';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_AESTHETICS';
UPDATE Policies SET IsOpener = 1 WHERE Type = 'POLICY_EXPLORATION';

UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_TRADITION_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_LIBERTY_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_HONOR_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_PIETY_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_PATRONAGE_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_COMMERCE_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_RATIONALISM_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_AESTHETICS_FINISHER';
UPDATE Policies SET IsFinisher = 1 WHERE Type = 'POLICY_EXPLORATION_FINISHER';

-- Policy Decision Stuff

UPDATE Policy_Flavors
SET Flavor = 15
WHERE PolicyType = 'POLICY_PIETY';

UPDATE Policy_Flavors
SET Flavor = 15
WHERE PolicyType = 'POLICY_LIBERTY';

UPDATE Policy_Flavors
SET Flavor = 15
WHERE PolicyType = 'POLICY_TRADITION' AND FlavorType = 'FLAVOR_CULTURE';

UPDATE Policy_Flavors
SET Flavor = 15
WHERE PolicyType = 'POLICY_TRADITION' AND FlavorType = 'FLAVOR_GROWTH';

UPDATE Policy_Flavors
SET Flavor = 10
WHERE PolicyType = 'POLICY_TRADITION' AND FlavorType = 'FLAVOR_WONDER';

-- Grand Strategy
UPDATE AIGrandStrategy_Flavors SET Flavor = 20 WHERE FlavorType= 'FLAVOR_OFFENSE' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_CONQUEST';
UPDATE AIGrandStrategy_Flavors SET Flavor = 20 WHERE FlavorType= 'FLAVOR_CULTURE' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_CULTURE';
UPDATE AIGrandStrategy_Flavors SET Flavor = 20 WHERE FlavorType= 'FLAVOR_DIPLOMACY' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_UNITED_NATIONS';
UPDATE AIGrandStrategy_Flavors SET Flavor = 20 WHERE FlavorType= 'FLAVOR_SPACESHIP' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_SPACESHIP';

UPDATE AIGrandStrategy_Yields SET Yield = 200 WHERE YieldType= 'YIELD_GOLD' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_UNITED_NATIONS';
UPDATE AIGrandStrategy_Yields SET Yield = 200 WHERE YieldType= 'YIELD_SCIENCE' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_SPACESHIP';
UPDATE AIGrandStrategy_Yields SET Yield = 200 WHERE YieldType= 'YIELD_PRODUCTION' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_SPACESHIP';
UPDATE AIGrandStrategy_Yields SET Yield = 200 WHERE YieldType= 'YIELD_PRODUCTION' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_CONQUEST';
UPDATE AIGrandStrategy_Yields SET Yield = 150 WHERE YieldType= 'YIELD_GOLD' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_CULTURE';
UPDATE AIGrandStrategy_Yields SET YieldType = 'YIELD_CULTURE' WHERE YieldType= 'YIELD_SCIENCE' AND AIGrandStrategyType = 'AIGRANDSTRATEGY_CULTURE';

-- City Specialization Stuff

UPDATE CitySpecialization_TargetYields
SET Yield = 100
WHERE YieldType = 'YIELD_GOLD';

UPDATE CitySpecialization_TargetYields
SET Yield = 175
WHERE YieldType = 'YIELD_SCIENCE';

-- Anti Air AI fix
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_ANTI_AIRCRAFT_GUN' AND UnitAIType = 'UNITAI_CITY_SPECIAL';
DELETE FROM Unit_AITypes WHERE UnitType = 'UNIT_MOBILE_SAM' AND UnitAIType = 'UNITAI_CITY_SPECIAL';

-- Operational AI changes
UPDATE MultiUnitFormations
SET RequiresNavalUnitConsistency = 0
WHERE Type = 'MUFORMATION_NAVAL_INVASION';

UPDATE MultiUnitFormations
SET RequiresNavalUnitConsistency = 0
WHERE Type = 'MUFORMATION_CITY_STATE_INVASION';

-- Recruitment

UPDATE TacticalMoves
SET OperationsCanRecruit = 1
WHERE Type = 'TACTICAL_BASTION_ALREADY_THERE';

UPDATE TacticalMoves
SET OperationsCanRecruit = 1
WHERE Type = 'TACTICAL_GUARD_IMPROVEMENT_ALREADY_THERE';

UPDATE TacticalMoves
SET OperationsCanRecruit = 1
WHERE Type = 'TACTICAL_GARRISON_ALREADY_THERE';

UPDATE TacticalMoves
SET OperationsCanRecruit = 1
WHERE Type = 'TACTICAL_BARBARIAN_CAMP';

-- Tactical AI Stuff
UPDATE TacticalMoves
SET Priority = 75
WHERE Type = 'TACTICAL_ESCORT_EMBARKED_UNIT';

UPDATE TacticalMoves
SET Priority = 20
WHERE Type = 'TACTICAL_ATTACK_LOW_PRIORITY_CIVILIAN';

UPDATE TacticalMoves
SET Priority = 40
WHERE Type = 'TACTICAL_ATTACK_MEDIUM_PRIORITY_CIVILIAN';

UPDATE TacticalMoves
SET Priority = 90
WHERE Type = 'TACTICAL_ATTACK_HIGH_PRIORITY_CIVILIAN';

UPDATE TacticalMoves
SET Priority = 75
WHERE Type = 'TACTICAL_PILLAGE';

UPDATE TacticalMoves
SET Priority = 200
WHERE Type = 'TACTICAL_PILLAGE_CITADEL';

UPDATE TacticalMoves
SET Priority = 75
WHERE Type = 'TACTICAL_PILLAGE_CITADEL_NEXT_TURN';

UPDATE TacticalMoves
SET Priority = 50
WHERE Type = 'TACTICAL_PILLAGE_NEXT_TURN';

UPDATE TacticalMoves
SET Priority = 100
WHERE Type = 'TACTICAL_PILLAGE_RESOURCE';

UPDATE TacticalMoves
SET Priority = 45
WHERE Type = 'TACTICAL_PILLAGE_RESOURCE_NEXT_TURN';

UPDATE TacticalMoves
SET Priority = 5
WHERE Type = 'TACTICAL_REPOSITION';

-- higher than all offensive moves so that the damaged units retreat from the front first to make space
UPDATE TacticalMoves
SET Priority = 190
WHERE Type = 'TACTICAL_HEAL';

-- first
UPDATE TacticalMoves
SET Priority = 180
WHERE Type = 'TACTICAL_MOVE_OPERATIONS';

-- second
UPDATE TacticalMoves
SET Priority = 150
WHERE Type = 'TACTICAL_OFFENSIVE_POSTURE_MOVES';

-- after destroy_high_unit
UPDATE TacticalMoves
SET Priority = 125
WHERE Type = 'TACTICAL_CAPTURE_CITY';

UPDATE TacticalMoves
SET Priority = 90
WHERE Type = 'TACTICAL_SAFE_BOMBARDS';

-- after unit destroy/attrition but before hedgehog
UPDATE TacticalMoves
SET Priority = 30
WHERE Type = 'TACTICAL_CLOSE_ON_TARGET';

UPDATE TacticalMoves
SET Priority = 25
WHERE Type = 'TACTICAL_POSTURE_HEDGEHOG';

UPDATE TacticalMoves
SET Priority = 50
WHERE Type = 'TACTICAL_DAMAGE_CITY';

UPDATE TacticalMoves
SET Priority = 35
WHERE Type = 'TACTICAL_ATTRIT_MEDIUM_UNIT';

UPDATE TacticalMoves
SET Priority = 50
WHERE Type = 'TACTICAL_ATTRIT_HIGH_UNIT';

-- this is for remaining air units after attacks 
UPDATE TacticalMoves
SET Priority = 23
WHERE Type = 'TACTICAL_AIR_INTERCEPT';

-- this is only for remaining idle air units after attacks and interceptors
UPDATE TacticalMoves
SET Priority = 21
WHERE Type = 'TACTICAL_AIR_SWEEP';

UPDATE TacticalMoves
SET Priority = 12
WHERE Type = 'TACTICAL_BASTION_ALREADY_THERE';

UPDATE TacticalMoves
SET Priority = 6
WHERE Type = 'TACTICAL_BASTION_1_TURN';

UPDATE TacticalMoves
SET Priority = 2
WHERE Type = 'TACTICAL_BASTION_2_TURN';

-- Military Strategy Flavors

DELETE FROM Unit_Flavors
WHERE UnitType = 'UNIT_ANTI_AIRCRAFT_GUN' AND FlavorType = 'FLAVOR_DEFENSE';

DELETE FROM AIMilitaryStrategy_Player_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_AT_WAR';

DELETE FROM AIMilitaryStrategy_Player_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WINNING_WARS';

DELETE FROM AIMilitaryStrategy_Player_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_LOSING_WARS';

DELETE FROM AIMilitaryStrategy_City_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_AT_WAR';

DELETE FROM AIMilitaryStrategy_City_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WINNING_WARS';

DELETE FROM AIMilitaryStrategy_City_Flavors
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_LOSING_WARS';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 60
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WAR_MOBILIZATION' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 60
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WAR_MOBILIZATION' AND FlavorType = 'FLAVOR_DEFENSE';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 60
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WAR_MOBILIZATION' AND FlavorType = 'FLAVOR_RANGED';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 30
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WAR_MOBILIZATION' AND FlavorType = 'FLAVOR_NAVAL';

INSERT INTO AIMilitaryStrategy_Player_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_WAR_MOBILIZATION', 'FLAVOR_NAVAL_RECON' , 25;

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = -15
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_WAR_MOBILIZATION' AND FlavorType = 'FLAVOR_EXPANSION';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 50
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_NEED_ANTIAIR' AND FlavorType = 'FLAVOR_ANTIAIR';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 20
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_NEED_AIR_CARRIER' AND FlavorType = 'FLAVOR_AIR_CARRIER';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 50
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_NEED_AIR' AND FlavorType = 'FLAVOR_AIR';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 50
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 75
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE' AND FlavorType = 'FLAVOR_DEFENSE';

INSERT INTO AIMilitaryStrategy_Player_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE', 'FLAVOR_MOBILE' , 25;

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 30
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 100
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIMilitaryStrategy_Player_Flavors
SET Flavor = 150
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_DEFENSE';

INSERT INTO AIMilitaryStrategy_Player_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL', 'FLAVOR_MOBILE' , 60;

INSERT INTO AIMilitaryStrategy_Player_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL', 'FLAVOR_AIR' , 60;

-- Military City Strategy Flavors

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 50
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 80
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE' AND FlavorType = 'FLAVOR_DEFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 30
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE' AND FlavorType = 'FLAVOR_NAVAL';

INSERT INTO AIMilitaryStrategy_City_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL', 'FLAVOR_MOBILE' , 40;

INSERT INTO AIMilitaryStrategy_City_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL', 'FLAVOR_AIR' , 50;

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 40
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 100
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 150
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND FlavorType = 'FLAVOR_DEFENSE';

INSERT INTO AIMilitaryStrategy_City_Flavors (AIMilitaryStrategyType, FlavorType, Flavor)
SELECT 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL', 'FLAVOR_MOBILE' , 60
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL' AND Value= 1 );

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 40
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_NEED_NAVAL_UNITS' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 60
WHERE AIMilitaryStrategyType = 'MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AICityStrategy_Flavors
SET Flavor = 10 
WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND AICityStrategyType = 'AICITYSTRATEGY_IS_PUPPET';

-- Economic Strategies

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_REALLY_EXPAND_TO_OTHER_CONTINENTS' AND FlavorType = 'FLAVOR_EXPANSION';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 50
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS' AND FlavorType = 'FLAVOR_EXPANSION';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 15
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS' AND FlavorType = 'FLAVOR_RECON';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 40
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 50
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 15
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_RECON' AND FlavorType = 'FLAVOR_RECON';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 150
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NAVAL_MAP' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_Player_Flavors
SET Flavor = 75
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategies
SET MinimumNumTurnsExecuted = 10
WHERE Type = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS';

UPDATE AIEconomicStrategies
SET CheckTriggerTurnCount = 7
WHERE Type = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS';

UPDATE AIEconomicStrategies
SET MinimumNumTurnsExecuted = 10
WHERE Type = 'ECONOMICAISTRATEGY_FOUND_CITY';

UPDATE AIEconomicStrategies
SET CheckTriggerTurnCount = 3
WHERE Type = 'ECONOMICAISTRATEGY_FOUND_CITY';

-- Economic City Strategy Flavors

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 8
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_GS_CULTURE' AND FlavorType = 'FLAVOR_EXPANSION';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 40
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 50
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 20
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_RECON' AND FlavorType = 'FLAVOR_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_DEFENSE';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_RANGED';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_MOBILE';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_TOO_MANY_UNITS' AND FlavorType = 'FLAVOR_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -50
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_LOSING_MONEY' AND FlavorType = 'FLAVOR_OFFENSE';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = -50
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_LOSING_MONEY' AND FlavorType = 'FLAVOR_DEFENSE';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_LOSING_MONEY' AND FlavorType = 'FLAVOR_GOLD';

UPDATE AICityStrategy_Flavors
SET Flavor = 150
WHERE AICityStrategyType = 'AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT' AND FlavorType = 'FLAVOR_NAVAL_TILE_IMPROVEMENT';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 100
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 80
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NAVAL_MAP' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 80
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NAVAL_MAP' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 60
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_NAVAL_MAP' AND FlavorType = 'FLAVOR_WATER_CONNECTION';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 40
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP' AND FlavorType = 'FLAVOR_NAVAL';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 40
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP' AND FlavorType = 'FLAVOR_NAVAL_RECON';

UPDATE AIEconomicStrategy_City_Flavors
SET Flavor = 30
WHERE AIEconomicStrategyType = 'ECONOMICAISTRATEGY_OFFSHORE_EXPANSION_MAP' AND FlavorType = 'FLAVOR_WATER_CONNECTION';

-- AI Strategies - no CSs
UPDATE AIEconomicStrategies
SET NoMinorCivs = 1
WHERE Type = 'ECONOMICAISTRATEGY_LOSING_MONEY';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_NEED_RANGED';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_ENOUGH_RANGED';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_NEED_MOBILE';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_ENOUGH_MOBILE';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_NEED_ANTIAIR';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_ENOUGH_ANTIAIR';

UPDATE AIMilitaryStrategies
SET NoMinorCivs = 1
WHERE Type = 'MILITARYAISTRATEGY_NEED_AIR_CARRIER';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 40
WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 80
WHERE FlavorType = 'FLAVOR_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 80
WHERE FlavorType = 'FLAVOR_OFFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 15
WHERE FlavorType = 'FLAVOR_NAVAL' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 50
WHERE FlavorType = 'FLAVOR_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_ELEVATED';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 25
WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_ELEVATED';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = -10
WHERE FlavorType = 'FLAVOR_NAVAL' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_THREAT_ELEVATED';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 10
WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 30
WHERE FlavorType = 'FLAVOR_DEFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = 20
WHERE FlavorType = 'FLAVOR_OFFENSE' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE';

UPDATE AIMilitaryStrategy_City_Flavors
SET Flavor = -20
WHERE FlavorType = 'FLAVOR_NAVAL' and AIMilitaryStrategyType = 'MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE';

-- Percentage by which military rating decays each turn for different game speeds (affects AI strength perception behavior)
-- 10 = 1%
UPDATE Gamespeeds
SET MilitaryRatingDecayPercent = 7 WHERE Type = 'GAMESPEED_MARATHON';

UPDATE Gamespeeds
SET MilitaryRatingDecayPercent = 13 WHERE Type = 'GAMESPEED_EPIC';

UPDATE Gamespeeds
SET MilitaryRatingDecayPercent = 20 WHERE Type = 'GAMESPEED_STANDARD';

UPDATE Gamespeeds
SET MilitaryRatingDecayPercent = 30 WHERE Type = 'GAMESPEED_QUICK';

-- Compatibility fix for AI intelligence
UPDATE HandicapInfos SET CityProductionNumOptionsConsidered = 90;
UPDATE HandicapInfos SET TechNumOptionsConsidered = 90;
UPDATE HandicapInfos SET PolicyNumOptionsConsidered = 90;
UPDATE HandicapInfos SET BeliefNumOptionsConsidered = 90;