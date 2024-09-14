-- Blocks a specified City-State from appearing ingame if the associated Civilization is present at game start.
CREATE TABLE MajorBlocksMinor (
	MajorCiv text NOT NULL,
	MinorCiv text NOT NULL,
	FOREIGN KEY (MajorCiv) REFERENCES Civilizations(Type),
	FOREIGN KEY (MinorCiv) REFERENCES MinorCivilizations(Type)
);

-- Allows modders to specify a fixed personality type for a City-State. This overrides the MOD_BALANCE_CITY_STATE_PERSONALITIES CustomModOption.
CREATE TABLE MinorCivPersonalityTypes (
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE
);

INSERT INTO MinorCivPersonalityTypes(ID, Type) VALUES (0, 'MINOR_CIV_PERSONALITY_FRIENDLY');
INSERT INTO MinorCivPersonalityTypes(Type) VALUES
('MINOR_CIV_PERSONALITY_NEUTRAL'), -- 1
('MINOR_CIV_PERSONALITY_HOSTILE'), -- 2
('MINOR_CIV_PERSONALITY_IRRATIONAL'); -- 3

ALTER TABLE MinorCivilizations ADD COLUMN FixedPersonality text DEFAULT NULL REFERENCES MinorCivPersonalityTypes(Type);

-- HistoricEventTypes table: Defines the different types of historic events. The order used here matches with the enum used in the DLL. Do not change without changing the DLL as well!
-- Not all of these historic events actually generate a bonus in the VP mod - some are only used for the Difficulty Bonus. These are prefixed with DIFFICULTY_BONUS.
CREATE TABLE HistoricEventTypes (
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE
);

INSERT INTO HistoricEventTypes(ID, Type) VALUES (0,'HISTORIC_EVENT_ERA_CHANGE');
INSERT INTO HistoricEventTypes(Type) VALUES 
('HISTORIC_EVENT_WORLD_WONDER'), -- 1
('HISTORIC_EVENT_GREAT_PERSON'), -- 2
('HISTORIC_EVENT_WON_WAR'), -- 3
('HISTORIC_EVENT_GOLDEN_AGE'), -- 4
('HISTORIC_EVENT_DIG'), -- 5
('HISTORIC_EVENT_TRADE_LAND'), -- 6
('HISTORIC_EVENT_TRADE_SEA'), -- 7
('HISTORIC_EVENT_TRADE_CS'), -- 8
('DIFFICULTY_BONUS_CITY_FOUND_CAPITAL'), -- 9
('DIFFICULTY_BONUS_CITY_FOUND'), -- 10
('DIFFICULTY_BONUS_CITY_CONQUEST'), -- 11
('DIFFICULTY_BONUS_RESEARCHED_TECH'), -- 12
('DIFFICULTY_BONUS_ADOPTED_POLICY'), -- 13
('DIFFICULTY_BONUS_COMPLETED_POLICY_TREE'), -- 14
('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT'), -- 15
('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT'), -- 16
('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT'), -- 17
('DIFFICULTY_BONUS_PLAYER_TURNS_PASSED'), -- 18
('DIFFICULTY_BONUS_AI_TURNS_PASSED'); -- 19


-- VictoryPursuitTypes table: Used by the Leaders Table. Allows modders to give a hint to the AI about which victory conditions a civ's UA is best suited for.
CREATE TABLE VictoryPursuitTypes (
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE
);

INSERT INTO VictoryPursuitTypes(ID, Type) VALUES (0,'VICTORY_PURSUIT_DOMINATION');
INSERT INTO VictoryPursuitTypes(Type) VALUES ('VICTORY_PURSUIT_DIPLOMACY'), ('VICTORY_PURSUIT_CULTURE'), ('VICTORY_PURSUIT_SCIENCE');

-- Recreate Leader Tables
-- Default diplomacy flavors now 5, adds three columns: Personality, PrimaryVictoryPursuit, SecondaryVictoryPursuit
CREATE TABLE Leaders_NEW (
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE,
	Description text,
	Civilopedia text,
	CivilopediaTag text,
	ArtDefineTag text,
	Personality text DEFAULT NULL,
	PrimaryVictoryPursuit text DEFAULT NULL,
	SecondaryVictoryPursuit text DEFAULT NULL,
	VictoryCompetitiveness INTEGER DEFAULT 5,
	WonderCompetitiveness INTEGER DEFAULT 5,
	MinorCivCompetitiveness INTEGER DEFAULT 5,
	Boldness INTEGER DEFAULT 5,
	DiploBalance INTEGER DEFAULT 5,
	WarmongerHate INTEGER DEFAULT 5,
	DoFWillingness INTEGER DEFAULT 5,
	DenounceWillingness INTEGER DEFAULT 5,
	Loyalty INTEGER DEFAULT 5,
	Forgiveness INTEGER DEFAULT 5,
	Neediness INTEGER DEFAULT 5,
	Meanness INTEGER DEFAULT 5,
	Chattiness INTEGER DEFAULT 5,
	-- Unused, but kept for backwards compatibility --
	WorkWithWillingness INTEGER DEFAULT 0,
	WorkAgainstWillingness INTEGER DEFAULT 0,
	-- End Unused --
	PortraitIndex INTEGER DEFAULT -1,
	IconAtlas text DEFAULT NULL REFERENCES IconTextureAtlases(Atlas),
	PackageID text DEFAULT NULL,
	FOREIGN KEY (PrimaryVictoryPursuit) REFERENCES VictoryPursuitTypes(Type),
	FOREIGN KEY (SecondaryVictoryPursuit) REFERENCES VictoryPursuitTypes(Type)
);

INSERT INTO Leaders_NEW(Type, Description, Civilopedia, CivilopediaTag, ArtDefineTag, VictoryCompetitiveness, WonderCompetitiveness, MinorCivCompetitiveness, Boldness, DiploBalance, WarmongerHate, DoFWillingness, DenounceWillingness, Loyalty, Forgiveness, Neediness, Meanness, Chattiness, PortraitIndex, IconAtlas, PackageID)
SELECT Type, Description, Civilopedia, CivilopediaTag, ArtDefineTag, VictoryCompetitiveness, WonderCompetitiveness, MinorCivCompetitiveness, Boldness, DiploBalance, WarmongerHate, DoFWillingness, DenounceWillingness, Loyalty, Forgiveness, Neediness, Meanness, Chattiness, PortraitIndex, IconAtlas, PackageID
FROM Leaders;

DROP TABLE Leaders;
ALTER TABLE Leaders_NEW RENAME TO Leaders;

CREATE TABLE Leader_MajorCivApproachBiases_NEW (
	LeaderType text REFERENCES Leaders(Type),
	MajorCivApproachType text REFERENCES MajorCivApproachTypes(Type),
	Bias INTEGER DEFAULT 5
);

INSERT INTO Leader_MajorCivApproachBiases_NEW(LeaderType, MajorCivApproachType) SELECT LeaderType, MajorCivApproachType FROM Leader_MajorCivApproachBiases;
DROP TABLE Leader_MajorCivApproachBiases;
ALTER TABLE Leader_MajorCivApproachBiases_NEW RENAME TO Leader_MajorCivApproachBiases;

-- A default of -1 (which will always return 1) is intentionally used here because the Firaxis Friendly/Protective approaches are merged into a single approach in the modded DLL, using the highest value of the two
-- This prevents situations where one approach is set below 5 and the other is not set, but the default of 5 overrides the modder's flavor because it's greater in value
CREATE TABLE Leader_MinorCivApproachBiases_NEW (
	LeaderType text REFERENCES Leaders(Type),
	MinorCivApproachType text REFERENCES MinorCivApproachTypes(Type),
	Bias INTEGER DEFAULT -1
);

INSERT INTO Leader_MinorCivApproachBiases_NEW(LeaderType, MinorCivApproachType) SELECT LeaderType, MinorCivApproachType FROM Leader_MinorCivApproachBiases;
DROP TABLE Leader_MinorCivApproachBiases;
ALTER TABLE Leader_MinorCivApproachBiases_NEW RENAME TO Leader_MinorCivApproachBiases;


-- Bombard Ranges for cities increase/decrease over default
ALTER TABLE Technologies ADD BombardRange INTEGER DEFAULT 0;
ALTER TABLE Technologies ADD BombardIndirect INTEGER DEFAULT 0;
ALTER TABLE Beliefs ADD BorderGrowthRateIncreaseGlobal INTEGER DEFAULT 0;

-- Belief requires an improvement on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresImprovement' BOOLEAN DEFAULT 0;

-- Belief Tooltip for LUA
ALTER TABLE Beliefs ADD COLUMN 'Tooltip' TEXT DEFAULT "";

-- Belief requires a resource (improved or not) on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresResource' BOOLEAN DEFAULT 0;

-- NOTE: THESE TWO (RequiresImprovement and RequiresResource) interact, and can be used to refine belief yields.

-- Belief requires no improvement to grant its yield on a tile.

ALTER TABLE Beliefs ADD COLUMN 'RequiresNoImprovement' BOOLEAN DEFAULT 0;

-- Belief requires no feature to grant its yield on a tile. Ignore the 'improvement' part.

ALTER TABLE Beliefs ADD COLUMN 'RequiresNoImprovementFeature' BOOLEAN DEFAULT 0;

-- NOTE: THESE TWO (RequiresNoImprovement and RequiresNoImprovementFeature) interact, and can be used to refine belief yields.

-- Belief - reduces policy cost of Wonders by 1 for every x cities following religion

ALTER TABLE Beliefs ADD COLUMN 'PolicyReductionWonderXFollowerCities' INTEGER DEFAULT 0;

-- To help the AI choose a starting pantheon for civs with the trait "StartsWithPantheon": If there are pantheons with AI_GoodStartingPantheon = true, the AI will make a random choice between them
ALTER TABLE Beliefs ADD COLUMN 'AI_GoodStartingPantheon' BOOLEAN DEFAULT 0;

-- Policy - increases happiness in every city for which city strength is at least the define value 'CITY_STRENGTH_THRESHOLD_FOR_BONUSES'
ALTER TABLE Policies ADD COLUMN 'HappinessPerCityOverStrengthThreshold' INTEGER DEFAULT 0;

-- Policy - increases potency of beakers for GSs
ALTER TABLE Policies ADD COLUMN 'GreatScientistBeakerModifier' INTEGER DEFAULT 0;
-- and GE hurry
ALTER TABLE Policies ADD COLUMN 'GreatEngineerHurryModifier' INTEGER DEFAULT 0;

-- Reduce tech costs from cities:
ALTER TABLE Policies ADD COLUMN 'TechCostXCitiesMod' INTEGER DEFAULT 0;
-- And reduce tourism cost from cities
ALTER TABLE Policies ADD COLUMN 'TourismCostXCitiesMod' INTEGER DEFAULT 0;

-- MaxAirUnitsChangeGlobal - every city gets that many air unit slots
ALTER TABLE Policies ADD COLUMN 'MaxAirUnitsChangeGlobal' INTEGER DEFAULT 0;

-- Policy - reduces policy cost of Wonders by 1 for every x CS allies
ALTER TABLE Policies ADD COLUMN 'XCSAlliesLowersPolicyNeedWonders' INTEGER DEFAULT 0;

-- Policy - reduce speed of people stealing Great Works from you.
ALTER TABLE Policies ADD COLUMN 'StealGWSlowerModifier' INTEGER DEFAULT 0;
-- Policy - increase speed of your Great Work theft rate.
ALTER TABLE Policies ADD COLUMN 'StealGWFasterModifier' INTEGER DEFAULT 0;
-- Policy - conquered cities keep all their buildings (except for those with NeverCapture==true).
ALTER TABLE Policies ADD COLUMN 'KeepConqueredBuildings' BOOLEAN DEFAULT 0;

-- Policy - extra GAP generation from excess happiness.
ALTER TABLE Policies ADD COLUMN 'GAPFromHappinessModifier' INTEGER DEFAULT 0;

-- Policy Branch - number of unlocked policies (finishers excluded) before branch is unlocked.
ALTER TABLE PolicyBranchTypes ADD COLUMN 'NumPolicyRequirement' INTEGER DEFAULT 100;

-- A font icon for an ideology
ALTER TABLE PolicyBranchTypes ADD COLUMN 'FontIcon' TEXT DEFAULT NULL;

-- Belief - increases pressure from trade routes

ALTER TABLE Beliefs ADD COLUMN 'PressureChangeTradeRoute' INTEGER DEFAULT 0;

-- Give CSs defensive units at the beginning of the game.
ALTER TABLE Eras ADD COLUMN 'StartingCityStateDefenseUnits' INTEGER DEFAULT 0;

-- Increase base unit supply if the game is started later.
ALTER TABLE Eras ADD COLUMN 'UnitSupplyBase' INTEGER DEFAULT 0;

-- Grants additional starting happiness based on gamespeed.
ALTER TABLE GameSpeeds ADD COLUMN 'StartingHappiness' INTEGER DEFAULT 0;

-- Value by which all instant yields are modified for different game speeds
ALTER TABLE GameSpeeds ADD COLUMN 'InstantYieldPercent' INTEGER DEFAULT 100;

-- Value by which instant yields from difficulty bonuses are modified for different game speeds
ALTER TABLE GameSpeeds ADD COLUMN 'DifficultyBonusPercent' INTEGER DEFAULT 100;
UPDATE GameSpeeds SET DifficultyBonusPercent = 67 WHERE Type = 'GAMESPEED_QUICK';
UPDATE GameSpeeds SET DifficultyBonusPercent = 100 WHERE Type = 'GAMESPEED_STANDARD';
UPDATE GameSpeeds SET DifficultyBonusPercent = 160 WHERE Type = 'GAMESPEED_EPIC';
UPDATE GameSpeeds SET DifficultyBonusPercent = 300 WHERE Type = 'GAMESPEED_MARATHON';

-- Percentage by which military rating decays each turn for different game speeds (affects AI strength perception behavior)
-- (10 = 1%)
ALTER TABLE GameSpeeds ADD COLUMN 'MilitaryRatingDecayPercent' INTEGER DEFAULT 0;

-- Table for Civilopedia Game Concepts to add more in boxes...
ALTER TABLE Concepts ADD COLUMN 'Extended' TEXT DEFAULT NULL;
ALTER TABLE Concepts ADD COLUMN 'DesignNotes' TEXT DEFAULT NULL;

-- Adds abiility for units to upgrade in allied CS or vassal lands.
ALTER TABLE Policies ADD COLUMN 'UpgradeCSVassalTerritory' BOOLEAN DEFAULT 0;

-- Spies gain additional Network Points per Turn
ALTER TABLE Policies ADD COLUMN 'EspionageNetworkPoints' INTEGER DEFAULT 0;

-- Modifies passive bonuses for Spies
ALTER TABLE Policies ADD COLUMN 'PassiveEspionageBonusModifier' INTEGER DEFAULT 0;

-- Modifies influence gained/lost when rigging elections
ALTER TABLE Policies ADD COLUMN 'RigElectionInfluenceModifier' INTEGER DEFAULT 0;

-- Adds event tourism from digging up sites.
ALTER TABLE Policies ADD COLUMN 'ArchaeologicalDigTourism' INTEGER DEFAULT 0;
-- Adds event tourism from golden ages starting.
ALTER TABLE Policies ADD COLUMN 'GoldenAgeTourism' INTEGER DEFAULT 0;

-- Gives event tourism when completing internal trade routes as if they are international.
ALTER TABLE Policies ADD COLUMN 'InternalTRTourism' boolean DEFAULT 0;

-- Reduces specialist unhappiness in cities by a set amount, either in capital or in all cities.

ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialists' INTEGER DEFAULT 0;
-- adds it back in as happiness!
ALTER TABLE Policies ADD COLUMN 'HappfromXSpecialists' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialistsCapital' INTEGER DEFAULT 0;

-- Warscore mod
ALTER TABLE Policies ADD COLUMN 'WarScoreModifier' INTEGER DEFAULT 0;

-- Can only trade with same ideology
ALTER TABLE Policies ADD COLUMN 'IsOnlyTradeSameIdeology' BOOLEAN DEFAULT 0;

-- Half specialist food in just capital
ALTER TABLE Policies ADD COLUMN 'HalfSpecialistFoodCapital' BOOLEAN DEFAULT 0;

-- Increases influence from trade mission
ALTER TABLE Policies ADD COLUMN 'MissionInfluenceModifier' INTEGER DEFAULT 0;

-- Happiness per trade route active
ALTER TABLE Policies ADD COLUMN 'HappinessPerActiveTradeRoute' INTEGER DEFAULT 0;

-- air units reduce unhappiness from needs by x% per unit.
ALTER TABLE Policies ADD COLUMN 'NeedsModifierFromAirUnits' INTEGER DEFAULT 0;

-- air units increase defense by +x flat per unit
ALTER TABLE Policies ADD COLUMN 'FlatDefenseFromAirUnits' INTEGER DEFAULT 0;

-- puppet penalties reduced by positive amount
ALTER TABLE Policies ADD COLUMN 'PuppetYieldPenaltyMod' INTEGER DEFAULT 0;

-- Puppet/Occupied cities gain x+x*era difference for buildings.
ALTER TABLE Policies ADD COLUMN 'ConquestPerEraBuildingProductionMod' INTEGER DEFAULT 0;

-- Increased free luxuries created by freelux routine for units (used by admiral in VP).
ALTER TABLE Policies ADD COLUMN 'AdmiralLuxuryBonus' INTEGER DEFAULT 0;


-- CS resources count towards monopolies
ALTER TABLE Policies ADD COLUMN 'CSResourcesCountForMonopolies' BOOLEAN DEFAULT 0;

-- Liberating a city gives influence to all CS
ALTER TABLE Policies ADD COLUMN 'InfluenceAllCSFromLiberation' INTEGER DEFAULT 0;

-- Liberating a city gives that city some units 
ALTER TABLE Policies ADD COLUMN 'NumUnitsInLiberatedCities' INTEGER DEFAULT 0;

-- Liberating a city gives XP to all units
ALTER TABLE Policies ADD COLUMN 'ExperienceAllUnitsFromLiberation' INTEGER DEFAULT 0;

-- Conquering a city heals all units (by X%)
ALTER TABLE Policies ADD COLUMN 'CityCaptureHealGlobal' INTEGER DEFAULT 0;

-- Conquering a city heals all units near the city (by X%)
ALTER TABLE Policies ADD COLUMN 'CityCaptureHealLocal' INTEGER DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in all Cities
ALTER TABLE Policies ADD COLUMN 'DistressFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'PovertyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BoredomFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestFlatReductionGlobal' INTEGER DEFAULT 0;

-- Changes the global median for a Need in the capital by x% - values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'BasicNeedsMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'GoldMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ScienceMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CultureMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestModifierCapital' INTEGER DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'BasicNeedsMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'GoldMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ScienceMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CultureMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestModifierGlobal' INTEGER DEFAULT 0;

-- Puppets and/or Occupied cities receive a % production modifier. Values should be positive to be good.
ALTER TABLE Policies ADD COLUMN 'PuppetProdMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'OccupiedProdMod' INTEGER DEFAULT 0;

-- Increases City Defense - needs to be in 100s value
ALTER TABLE Policies ADD COLUMN 'DefenseBoostAllCities' INTEGER DEFAULT 0;

-- Community Patch: Global Happiness Based on # of Citizens in Empire
-- Vox Populi: Local Happiness for each X Citizens in this city
ALTER TABLE Policies ADD COLUMN 'HappinessPerXPopulationGlobal' INTEGER DEFAULT 0;

-- War Weariness Mod - Positive values make it harder to accumulate war weariness.
ALTER TABLE Policies ADD COLUMN 'WarWearinessModifier' INTEGER DEFAULT 0;

-- GG Mod - boosts strength modifier of GGs
ALTER TABLE Policies ADD COLUMN 'GreatGeneralExtraBonus' INTEGER DEFAULT 0;

-- Shift Influence with CS for all by % +/- when you bully
ALTER TABLE Policies ADD COLUMN 'BullyGlobalCSInfluenceShift' INTEGER DEFAULT 0;

-- More yields from Vassals and CSs
ALTER TABLE Policies ADD COLUMN 'VassalYieldBonusModifier' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CSYieldBonusModifier' INTEGER DEFAULT 0;

-- Removes penalties for bullying City-States (no Influence loss or PTP/quest revocation)
ALTER TABLE Policies ADD COLUMN 'CanBullyFriendlyCS' BOOLEAN DEFAULT 0;

-- Allied CS influence does not decline at war
ALTER TABLE Policies ADD COLUMN 'NoAlliedCSInfluenceDecayAtWar' BOOLEAN DEFAULT 0;

-- Allied CS influence resting point increases while at war
ALTER TABLE Policies ADD COLUMN 'MinimumAllyInfluenceIncreaseAtWar' INTEGER DEFAULT 0;

-- Vassals don't rebel and can't be forced out by deals or WC
ALTER TABLE Policies ADD COLUMN 'VassalsNoRebel' BOOLEAN DEFAULT 0;

-- New Belief Element -- Assign Belief to Specific Civ only
ALTER TABLE Beliefs ADD COLUMN 'CivilizationType' TEXT DEFAULT NULL;

-- New Policies

-- Reduces unhappiness in occupied cities w/ Garrison. Negative = reduction.
ALTER TABLE Policies ADD COLUMN 'GarrisonsOccupiedUnhappinessMod' INTEGER DEFAULT 0;

-- Spawns a free unit at x population for a city.
ALTER TABLE Policies ADD COLUMN 'XPopulationConscription' INTEGER DEFAULT 0;

-- No Unhappiness from Expansion
ALTER TABLE Policies ADD COLUMN 'NoUnhappinessExpansion' BOOLEAN DEFAULT 0;

-- No Unhappiness from Isolation
ALTER TABLE Policies ADD COLUMN 'NoUnhappyIsolation' BOOLEAN DEFAULT 0;

-- Double City Border Growth During GA (does not stack with WLTKD)
ALTER TABLE Policies ADD COLUMN 'DoubleBorderGrowthGA' BOOLEAN DEFAULT 0;

-- Double City Border Growth During WLTKD (does not stack with GA)
ALTER TABLE Policies ADD COLUMN 'DoubleBorderGrowthWLTKD' BOOLEAN DEFAULT 0;

-- Free Population
ALTER TABLE Policies ADD COLUMN 'FreePopulation' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'FreePopulationCapital' INTEGER DEFAULT 0;

-- Extra Moves for Civilian Units
ALTER TABLE Policies ADD COLUMN 'ExtraMoves' INTEGER DEFAULT 0;

-- Religious Pressure Mod Trade Route
ALTER TABLE Policies ADD COLUMN 'TradeReligionModifier' INTEGER DEFAULT 0;

-- Increased Quest Influence
ALTER TABLE Policies ADD COLUMN 'IncreasedQuestRewards' INTEGER DEFAULT 0;

-- Extra vote in WC for every X City-States originally in the world
ALTER TABLE Policies ADD COLUMN 'FreeWCVotes' INTEGER DEFAULT 0;

-- Gives votes per city following your state religion
ALTER TABLE Policies ADD COLUMN 'VotesPerFollowingCityTimes100' integer DEFAULT 0;

-- GP Expend Influence Boost
ALTER TABLE Policies ADD COLUMN 'InfluenceGPExpend' INTEGER DEFAULT 0;

-- Free Trade Route
ALTER TABLE Policies ADD COLUMN 'FreeTradeRoute' INTEGER DEFAULT 0;

-- Free Spy
ALTER TABLE Policies ADD COLUMN 'FreeSpy' INTEGER DEFAULT 0;

-- City Security Modifier
ALTER TABLE Policies ADD COLUMN 'SpySecurityModifier' INTEGER DEFAULT 0;

-- Gold from Internal Trade Routes
ALTER TABLE Policies ADD COLUMN 'InternalTradeGold' INTEGER DEFAULT 0;

-- Boost Culture Bomb from Citadel
ALTER TABLE Policies ADD COLUMN 'CultureBombBoost' INTEGER DEFAULT 0;

-- Unlock Era for Policy (Unlocks later eras earlier than normal)
ALTER TABLE Policies ADD COLUMN 'UnlocksPolicyBranchEra' TEXT DEFAULT NULL;

-- Points towards ideologies
ALTER TABLE Policies ADD COLUMN 'IdeologyPoint' INTEGER DEFAULT 0;

-- Religious Distance Modifier (affects spread to owned cities based on distance)
ALTER TABLE Policies ADD COLUMN 'ReligionDistance' INTEGER DEFAULT 0;

-- Religious Pressure Modifier (only from cities following state religion)
ALTER TABLE Policies ADD COLUMN 'PressureMod' INTEGER DEFAULT 0;

-- New Beliefs

-- Combat bonus v. other religions in our lands
ALTER TABLE Beliefs ADD COLUMN 'CombatVersusOtherReligionOwnLands' INTEGER DEFAULT 0;

-- Combat bonus v. other religions in their lands
ALTER TABLE Beliefs ADD COLUMN 'CombatVersusOtherReligionTheirLands' INTEGER DEFAULT 0;

-- Production modifier for units
ALTER TABLE Beliefs ADD COLUMN 'UnitProductionModifier' INTEGER DEFAULT 0;

-- Influence with CS from missionary spread
ALTER TABLE Beliefs ADD COLUMN 'MissionaryInfluenceCS' INTEGER DEFAULT 0;

-- Happiness per known civ with a Pantheon
ALTER TABLE Beliefs ADD COLUMN 'HappinessPerPantheon' INTEGER DEFAULT 0;

-- Extra Votes from Belief
ALTER TABLE Beliefs ADD COLUMN 'ExtraVotes' INTEGER DEFAULT 0;

ALTER TABLE Beliefs ADD COLUMN 'CityScalerLimiter' INTEGER DEFAULT 0;
ALTER TABLE Beliefs ADD COLUMN 'FollowerScalerLimiter' INTEGER DEFAULT 0;

-- Ignore Policy Requirements (number) for wonders up to a set era
ALTER TABLE Beliefs ADD COLUMN 'ReducePolicyRequirements' INTEGER DEFAULT 0;

-- Increase yields from friendship/alliance for CS sharing your religion by x%
ALTER TABLE Beliefs ADD COLUMN 'CSYieldBonusFromSharedReligion' INTEGER DEFAULT 0;

-- Increase happiness from owned foreign spies active in other cities
ALTER TABLE Beliefs ADD COLUMN 'HappinessFromForeignSpies' INTEGER DEFAULT 0;

-- Decrease inquisitor cost. What it says on the tin ya git.
ALTER TABLE Beliefs ADD COLUMN 'InquisitorCostModifier' INTEGER DEFAULT 0;

-- Tourism Mod, global, from WC
ALTER TABLE Resolutions ADD COLUMN 'TourismMod' integer default 0;

-- Allows you to define a an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed. -- 
ALTER TABLE Policies ADD COLUMN 'EventTourismCS' INTEGER DEFAULT 0;

-- Allows you to define a an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed. -- 
ALTER TABLE Policies ADD COLUMN 'EventTourism' INTEGER DEFAULT 0;

-- Define a policy as a dummy policy - won't count towards policy total -- 
ALTER TABLE Policies ADD COLUMN 'IsDummy' BOOLEAN DEFAULT 0;

-- Define Opener/Closer Policies - useful for counting elements
ALTER TABLE Policies ADD COLUMN 'IsOpener' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'IsFinisher' BOOLEAN DEFAULT 0;

-- Allows you to increase razing speed from policies
ALTER TABLE Policies ADD COLUMN 'RazingSpeedBonus' INTEGER DEFAULT 0;

-- Allows you to set whether or not partisans spawn from razing cities (via policy)
ALTER TABLE Policies ADD COLUMN 'NoPartisans' BOOLEAN DEFAULT 0;

-- Allows you to set whether or not units gain full XP when purchased
ALTER TABLE Policies ADD COLUMN 'NoXPLossUnitPurchase' BOOLEAN DEFAULT 0;

-- Allows you to set a % of warscore that is added to a tourism bonus against a civ
ALTER TABLE Policies ADD COLUMN 'PositiveWarScoreTourismMod' INTEGER DEFAULT 0;

-- Allows you to set a policy requirement to unlock a process
ALTER TABLE Processes ADD COLUMN 'RequiredPolicy' TEXT DEFAULT NULL REFERENCES Policies(Type);

-- Grants obsoletion tech to build (tie to improvement below for function)
ALTER TABLE Builds ADD COLUMN 'ObsoleteTech' TEXT DEFAULT NULL;

-- Builds only kill civilian units (bugfix for design woes re: double improvement logic)
ALTER TABLE Builds ADD COLUMN 'KillOnlyCivilian' BOOLEAN DEFAULT 0;

-- New Goody Hut Additions
ALTER TABLE GoodyHuts ADD COLUMN 'Production' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'GoldenAge' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'FreeTiles' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'Science' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'PantheonPercent' INTEGER DEFAULT 0;

-- Additional Goody Hut options, requires NEW_GOODIES CustomModOptions to set True
ALTER TABLE GoodyHuts ADD COLUMN 'Food' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'BorderGrowth' INTEGER DEFAULT 0;

-- Tech Additions
ALTER TABLE Technologies ADD COLUMN 'CityLessEmbarkCost' boolean;
ALTER TABLE Technologies ADD COLUMN 'CityNoEmbarkCost' boolean;

-- Tech adds raw happiness to capital
ALTER TABLE Technologies ADD COLUMN 'Happiness' INTEGER DEFAULT 0;

-- Alters monopoly values below (except for strategic elements, like attack/defense/heal/etc.)
ALTER TABLE Policies ADD COLUMN 'MonopolyModFlat' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'MonopolyModPercent' INTEGER DEFAULT 0;

-- Investment reduction costs -- policy -- negative makes it stronger!
ALTER TABLE Policies ADD COLUMN 'InvestmentModifier' INTEGER DEFAULT 0;

-- Limits the amount that can be built of a Unit class per city
ALTER TABLE UnitClasses ADD COLUMN 'UnitInstancePerCity' INTEGER DEFAULT -1;

-- Trade Route Internal Capital Bonus -- policy -- Internal/INTL TR from Capital and Holy City stronger!
ALTER TABLE Policies ADD COLUMN 'InternalTradeRouteYieldModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TradeRouteYieldModifierCapital' INTEGER DEFAULT 0;
-- Or all routes.
ALTER TABLE Policies ADD COLUMN 'TradeRouteYieldModifier' INTEGER DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD COLUMN 'GreatEngineerRateModifier' INTEGER DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD COLUMN 'CityStateCombatModifier' INTEGER DEFAULT 0;

-- Trade Route Modifiers for Policies
ALTER TABLE Policies ADD COLUMN 'TradeRouteLandDistanceModifier' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TradeRouteSeaDistanceModifier' INTEGER DEFAULT 0;


-- Processes

-- Adds ability to turn production into defense/healing in a city for a process
ALTER TABLE Processes ADD COLUMN 'DefenseValue' INTEGER DEFAULT 0;

-- Unique processes, requires CIVILIZATIONS_UNIQUE_PROCESSES in CustomModOptions
ALTER TABLE Processes ADD COLUMN 'CivilizationType' TEXT DEFAULT NULL;

ALTER TABLE Policies ADD COLUMN 'TRSpeedBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TRVisionBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXPolicies' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXGreatWorks' INTEGER DEFAULT 0;

-- Alters food consumption of specialists - use integer (is raised to 100s later)
ALTER TABLE Policies ADD COLUMN 'SpecialistFoodChange' INTEGER DEFAULT 0;

-- Trade Routes
ALTER TABLE Policies ADD COLUMN 'ExtraCultureandScienceTradeRoutes' INTEGER DEFAULT 0;

-- CORPORATIONS
ALTER TABLE Technologies ADD COLUMN 'CorporationsEnabled' boolean;

-- Corporation Policies
ALTER TABLE Policies ADD COLUMN 'CorporationOfficesAsNumFranchises' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationNumFreeFranchiseAbovePopular' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationRandomForeignFranchiseMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'AdditionalNumFranchisesMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'AdditionalNumFranchises' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoForeignCorpsInCities' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoFranchisesInForeignCities' BOOLEAN DEFAULT 0;

-- 20 = 20% additional, NOT 1/5 of existing value. this stacks, so 120%, 140%, 160%, etc...
UPDATE Policies SET StrategicResourceMod = 100 WHERE StrategicResourceMod = 200;

-- Minor Civs
ALTER TABLE MinorCivilizations ADD COLUMN 'BullyUnitClass' TEXT DEFAULT NULL;

-- GameSpeed for JFD
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMin' INTEGER DEFAULT 0;
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMax' INTEGER DEFAULT 0;

-- HH Mod
ALTER TABLE Policies ADD COLUMN 'ExtraNaturalWonderHappiness' INTEGER DEFAULT 0;
--ALTER TABLE Policies ADD CONSTRAINT ck_ExtraNaturalWonderHappiness CHECK (ExtraNaturalWonderHappiness >= 0);

-- Worlds
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCities' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCityStates' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'ReformationPercentRequired' INTEGER DEFAULT 100;
ALTER TABLE Worlds ADD COLUMN 'NumCitiesTourismCostMod' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'NumCitiesUnitSupplyMod' INTEGER DEFAULT 0;

-- Policy which initializes N of the best units in a category in capital. Requires 'IncludesOneShotFreeUnits' set to true.
ALTER TABLE Policies ADD COLUMN 'BestNumberLandCombatUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberLandRangedUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberSeaCombatUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberSeaRangedUnitClass' INTEGER DEFAULT 0;

-- Extra Military Supply from Population
ALTER TABLE Policies ADD COLUMN 'ExtraSupplyPerPopulation' INTEGER DEFAULT 0;

-- Extra flat supply
ALTER TABLE Policies ADD COLUMN 'ExtraSupplyFlat' integer DEFAULT 0;

-- Spawn best Melee Type Unit on finishing a Build (accounts for Domain of Build)
ALTER TABLE Builds ADD COLUMN 'IsFreeBestDomainUnit' BOOLEAN DEFAULT 0;

-- Grants a free building to a city when founded
ALTER TABLE Policies ADD COLUMN 'NewCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free building to all existing and future cities
ALTER TABLE Policies ADD COLUMN 'AllCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free building to newly founded cities
ALTER TABLE Policies ADD COLUMN 'NewFoundCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free unit to newly founded cities
ALTER TABLE Policies ADD COLUMN 'NewFoundCityFreeUnit' TEXT DEFAULT NULL REFERENCES UnitClasses(Type);
ALTER TABLE Policies ADD 'LandmarksTourismPercent' INTEGER DEFAULT 0;

-- Build adds an instant yield of culture to Player's culture pool.
ALTER TABLE Builds ADD COLUMN 'CultureBoost' BOOLEAN DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD COLUMN 'ExtraMissionaryStrength' INTEGER DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD COLUMN 'ExtraMissionarySpreads' INTEGER DEFAULT 0;

-- CSD

-- Insert SQL Rules Here 

ALTER TABLE Resolutions ADD COLUMN 'OpenDoor' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'SphereOfInfluence' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'Decolonization' boolean default 0;

ALTER TABLE Resolutions ADD COLUMN 'SpaceshipProductionMod' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'SpaceshipPurchaseMod' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'IsWorldWar' integer default 0;

ALTER TABLE Resolutions ADD COLUMN 'EmbargoIdeology' boolean default 0;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'AttackBonusTurns' integer default 0;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'BaseFreeUnits' integer default 0;

ALTER TABLE LeagueProjectRewards ADD COLUMN 'GetNumFreeGreatPeople' integer default 0;

ALTER TABLE Policies ADD COLUMN 'GreatDiplomatRateModifier' integer default 0;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'BuildingTrigger' text default NULL;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'TriggerResolution' text default NULL;

-- Insert SQL Rules Here 

ALTER TABLE GameSpeeds		ADD		TechCostPerTurnMultiplier		float;										-- How much does each turn of research add to tech cost?
ALTER TABLE GameSpeeds		ADD		MinimumVassalLiberateTurns		integer;									-- Minimum turns of vassalage (before master can liberate them)
ALTER TABLE GameSpeeds		ADD		MinimumVassalTurns				integer;									-- Minimum turns of vassalage (before vassal can break it off)
ALTER TABLE GameSpeeds		ADD		MinimumVassalTaxTurns			integer;									-- Minimum number of turns before we can change vassal taxes
ALTER TABLE GameSpeeds		ADD		MinimumVoluntaryVassalTurns		integer;
ALTER TABLE GameSpeeds		ADD		NumTurnsBetweenVassals			integer;									-- How many turns we must wait (after we've revoked vassalage) before they'll sign a new agreement)
ALTER TABLE Technologies	ADD		VassalageTradingAllowed			boolean;									-- Enables Vassalage (tech)
ALTER TABLE Eras			ADD		VassalageEnabled				boolean;									-- Enables Vassalage (era)
ALTER TABLE Resolutions		ADD		VassalMaintenanceGoldPercent	integer	DEFAULT	0;
ALTER TABLE Resolutions		ADD		EndAllCurrentVassals			boolean	DEFAULT	0;

-- De-hardcode New Era Popup splash image with a new column and improved LUA file
-- Add the new column for new era popup splash image
ALTER TABLE Eras ADD EraSplashImage TEXT DEFAULT 'ERA_Medievel.dds';

-- Add useful properties to UnitCombatInfos for easy SQL reference
-- Please don't use this to determine properties of individual unit classes/units
ALTER TABLE UnitCombatInfos ADD IsMilitary BOOLEAN DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsRanged BOOLEAN DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsNaval BOOLEAN DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsAerial BOOLEAN DEFAULT 0;

UPDATE UnitCombatInfos SET IsMilitary = 1
WHERE ID <= 13;

UPDATE UnitCombatInfos SET IsRanged = 1
WHERE Type IN (
	'UNITCOMBAT_ARCHER',
	'UNITCOMBAT_SIEGE',
	'UNITCOMBAT_NAVALRANGED',
	'UNITCOMBAT_SUBMARINE',
	'UNITCOMBAT_CARRIER'
);

UPDATE UnitCombatInfos SET IsNaval = 1
WHERE Type IN (
	'UNITCOMBAT_NAVALRANGED',
	'UNITCOMBAT_NAVALMELEE',
	'UNITCOMBAT_SUBMARINE',
	'UNITCOMBAT_CARRIER'
);

UPDATE UnitCombatInfos SET IsAerial = 1
WHERE Type IN (
	'UNITCOMBAT_FIGHTER',
	'UNITCOMBAT_BOMBER'
);
