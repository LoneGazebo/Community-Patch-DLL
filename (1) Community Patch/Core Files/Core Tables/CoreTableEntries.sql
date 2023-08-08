-- HistoricEventTypes table: Defines the different types of historic events. The order used here matches with the enum used in the DLL. Do not change without changing the DLL as well!
-- Not all of these historic events actually generate a bonus in the VP mod - some are only used for the Difficulty Bonus.
CREATE TABLE IF NOT EXISTS HistoricEventTypes(
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE
);

INSERT INTO HistoricEventTypes(ID, Type) VALUES ('0','HISTORIC_EVENT_ERA_CHANGE');
INSERT INTO HistoricEventTypes(Type) VALUES 
('HISTORIC_EVENT_WORLD_WONDER'), -- 1
('HISTORIC_EVENT_GREAT_PERSON'), -- 2
('HISTORIC_EVENT_WON_WAR'), -- 3
('HISTORIC_EVENT_GOLDEN_AGE'), -- 4
('HISTORIC_EVENT_DIG'), -- 5
('HISTORIC_EVENT_TRADE_LAND'), -- 6
('HISTORIC_EVENT_TRADE_SEA'), -- 7
('HISTORIC_EVENT_TRADE_CS'), -- 8
('HISTORIC_EVENT_CITY_FOUND_CAPITAL'), -- 9
('HISTORIC_EVENT_CITY_FOUND'), -- 10
('HISTORIC_EVENT_CITY_CONQUEST'), -- 11
('HISTORIC_EVENT_PLAYER_TURNS_PASSED'), -- 12
('HISTORIC_EVENT_AI_TURNS_PASSED'); -- 13


-- VictoryPursuitTypes table: Used by the Leaders Table. Allows modders to give a hint to the AI about which victory conditions a civ's UA is best suited for.
CREATE TABLE IF NOT EXISTS VictoryPursuitTypes(
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE
);

INSERT INTO VictoryPursuitTypes(ID, Type) VALUES ('0','VICTORY_PURSUIT_DOMINATION');
INSERT INTO VictoryPursuitTypes(Type) VALUES ('VICTORY_PURSUIT_DIPLOMACY'), ('VICTORY_PURSUIT_CULTURE'), ('VICTORY_PURSUIT_SCIENCE');

-- Recreate Leader Tables
-- Default diplomacy flavors now 5, adds three columns: Personality, PrimaryVictoryPursuit, SecondaryVictoryPursuit
CREATE TABLE IF NOT EXISTS Leaders_NEW(
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

CREATE TABLE IF NOT EXISTS Leader_MajorCivApproachBiases_NEW(
	LeaderType text REFERENCES Leaders(Type),
	MajorCivApproachType text REFERENCES MajorCivApproachTypes(Type),
	Bias INTEGER DEFAULT 5
);

INSERT INTO Leader_MajorCivApproachBiases_NEW(LeaderType, MajorCivApproachType) SELECT LeaderType, MajorCivApproachType FROM Leader_MajorCivApproachBiases;
DROP TABLE Leader_MajorCivApproachBiases;
ALTER TABLE Leader_MajorCivApproachBiases_NEW RENAME TO Leader_MajorCivApproachBiases;

-- A default of -1 (which will always return 1) is intentionally used here because the Firaxis Friendly/Protective approaches are merged into a single approach in the modded DLL, using the highest value of the two
-- This prevents situations where one approach is set below 5 and the other is not set, but the default of 5 overrides the modder's flavor because it's greater in value
CREATE TABLE IF NOT EXISTS Leader_MinorCivApproachBiases_NEW(
	LeaderType text REFERENCES Leaders(Type),
	MinorCivApproachType text REFERENCES MinorCivApproachTypes(Type),
	Bias INTEGER DEFAULT -1
);

INSERT INTO Leader_MinorCivApproachBiases_NEW(LeaderType, MinorCivApproachType) SELECT LeaderType, MinorCivApproachType FROM Leader_MinorCivApproachBiases;
DROP TABLE Leader_MinorCivApproachBiases;
ALTER TABLE Leader_MinorCivApproachBiases_NEW RENAME TO Leader_MinorCivApproachBiases;


-- Unified Yields Stuff
ALTER TABLE Yields ADD ColorString TEXT DEFAULT '[COLOR_WHITE]';

UPDATE Yields SET ColorString='[COLOR_MAGENTA]' WHERE Type='YIELD_CULTURE';
UPDATE Yields SET ColorString='[COLOR_WHITE]' WHERE Type='YIELD_FAITH';
UPDATE Yields SET ColorString='[COLOR_GREEN]' WHERE Type='YIELD_FOOD';
UPDATE Yields SET ColorString='[COLOR_YELLOW]' WHERE Type='YIELD_GOLD';
UPDATE Yields SET ColorString='[COLOR_YELLOW]' WHERE Type='YIELD_PRODUCTION';
UPDATE Yields SET ColorString='[COLOR_BLUE]' WHERE Type='YIELD_SCIENCE';

ALTER TABLE Yields ADD ImageTexture TEXT DEFAULT 'YieldAtlas.dds';
ALTER TABLE Yields ADD ImageOffset INTEGER DEFAULT 0;

UPDATE Yields SET ImageOffset=128 WHERE Type='YIELD_PRODUCTION';
UPDATE Yields SET ImageOffset=256 WHERE Type='YIELD_GOLD';
UPDATE Yields SET ImageOffset=384 WHERE Type='YIELD_SCIENCE';
UPDATE Yields SET ImageTexture='YieldAtlas_128_Culture.dds' WHERE Type='YIELD_CULTURE';
UPDATE Yields SET ImageTexture='YieldAtlas_128_Faith.dds' WHERE Type='YIELD_FAITH';


-- Smallaward Info table used for CS quests - does not allow you to add new quests - simply makes it so that you can more easily tweak/define the quest rewards/timers
ALTER TABLE SmallAwards ADD 'Influence' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'GlobalGPPoints' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'CapitalGPPoints' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'GlobalExperience' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'QuestDuration' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Food' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Gold' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Culture' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Faith' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Science' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Production' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'GoldenAgePoints' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Happiness' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'Tourism' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'GeneralPoints' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'AdmiralPoints' INTEGER DEFAULT 0;
ALTER TABLE SmallAwards ADD 'RandomMod' INTEGER DEFAULT 0;

-- Bombard Ranges for cities increase/decrease over default
ALTER TABLE Technologies ADD BombardRange INTEGER DEFAULT 0;
ALTER TABLE Technologies ADD BombardIndirect INTEGER DEFAULT 0;

-- Create Era Value for Great Works creators that makes them spawn in the appropriate era.
ALTER TABLE Unit_UniqueNames ADD COLUMN 'EraType' TEXT DEFAULT NULL;

-- Create buildings that cannot have access to fresh water.

ALTER TABLE Buildings ADD COLUMN 'IsNoWater' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IsNoRiver' BOOLEAN DEFAULT 0;

-- Create buildings that cannot have access to fresh water.
ALTER TABLE Buildings ADD COLUMN 'IsNoCoast' BOOLEAN DEFAULT 0;

-- Create buildings that must be in the Capital

ALTER TABLE Buildings ADD COLUMN 'CapitalOnly' BOOLEAN DEFAULT 0;

-- Requirement that allows you to bypass prereq techs to get something.
ALTER TABLE Buildings ADD COLUMN 'NumPoliciesNeeded' INTEGER DEFAULT 0;

-- Reduce specialist unhappiness from urbanization (CBO)
ALTER TABLE Buildings ADD COLUMN 'NoUnhappfromXSpecialists' INTEGER DEFAULT 0;
-- Reduce specialist unhappiness from urbanization (CBO)
ALTER TABLE Buildings ADD COLUMN 'NoUnhappfromXSpecialistsGlobal' INTEGER DEFAULT 0;

-- BUILDING: PlayerBorderGainlessPillage & CityGainlessPillage
-- If such a building's effect applies, other teams get neither gold nor heal from pillaging the appropriate tiles.
-- CityGainlessPillage affects the constructing city's worked tiles, PlayerBorderGainlessPillage proofs every tile of the player
ALTER TABLE Buildings ADD PlayerBorderGainlessPillage BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD CityGainlessPillage BOOLEAN DEFAULT 0;


-- Building, Belief, UA
-- Increase to border growth expansion rate
ALTER TABLE Buildings ADD BorderGrowthRateIncrease INTEGER DEFAULT 0; -- For the city
ALTER TABLE Buildings ADD BorderGrowthRateIncreaseGlobal INTEGER DEFAULT 0; -- For the empire
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
-- Policy - extra non-gold yields from heavy tribute (in percent).
ALTER TABLE Policies ADD COLUMN 'ExtraYieldsFromHeavyTribute' INTEGER DEFAULT 0;
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

-- Percentage by which military rating decays each turn for different game speeds (affects AI strength perception behavior)
-- (10 = 1%)
ALTER TABLE GameSpeeds ADD COLUMN 'MilitaryRatingDecayPercent' INTEGER DEFAULT 0;

-- Trade Route yields no longer scale from distance.
ALTER TABLE Traits ADD COLUMN 'IgnoreTradeDistanceScaling' BOOLEAN DEFAULT 0;

-- Trade Routes can be plundered without being at war
ALTER TABLE Traits ADD COLUMN 'CanPlunderWithoutWar' BOOLEAN DEFAULT 0;

-- No unhappiness from Isolation.
ALTER TABLE Traits ADD COLUMN 'NoConnectionUnhappiness' BOOLEAN DEFAULT 0;

-- can buy boats with faith (need faith cost on units to do so).
ALTER TABLE Traits ADD COLUMN 'CanPurchaseNavalUnitsFaith' BOOLEAN DEFAULT 0;

-- Imports count towards monopolies.
ALTER TABLE Traits ADD COLUMN 'ImportsCountTowardsMonopolies' BOOLEAN DEFAULT 0;

-- No unhappiness from religious strife.
ALTER TABLE Traits ADD COLUMN 'IsNoReligiousStrife' BOOLEAN DEFAULT 0;

-- Extra Wonder Production during Golden Ages.
ALTER TABLE Traits ADD COLUMN 'WonderProductionModGA' INTEGER DEFAULT 0;

-- TRAIT: Changes the food (times 100) consumed by each non-specialist citizen. --
ALTER TABLE Traits ADD COLUMN 'NonSpecialistFoodChange' INTEGER DEFAULT 0;

-- TRAIT: Annexed City States continue to give yields. --
ALTER TABLE Traits ADD COLUMN 'AnnexedCityStatesGiveYields' BOOLEAN DEFAULT 0;

-- Abnormal scaler. Works for:
---- Trait_SpecialistYieldChanges (specialist yield change x2/x3/x4 in medieval/industrial/atomic eras)
---- FreeSocialPoliciesPerEra column in Traits
---- Trait_YieldChangesPerImprovementBuilt (x2/x3/x4 of the bonus in Medieval/Industrial/Atomic)

ALTER TABLE Traits ADD COLUMN 'IsOddEraScaler' BOOLEAN DEFAULT 0;

-- Trait affects capital only. Works for:
---- Trait_YieldChangesPerImprovementBuilt (yield only shows up in capital)

ALTER TABLE Traits ADD COLUMN 'IsCapitalOnly' BOOLEAN DEFAULT 0;

-- No natural religion spread to/from other players' cities

ALTER TABLE Traits ADD COLUMN 'NoNaturalReligionSpread' BOOLEAN DEFAULT 0;

-- No trade routes to player if not already trading w/ you, tourism penalty if not trading (-50%)

ALTER TABLE Traits ADD COLUMN 'NoOpenTrade' BOOLEAN DEFAULT 0;

-- Earn a free building only in your capital as your trait. No tech requirement.

ALTER TABLE Traits ADD COLUMN 'FreeCapitalBuilding' TEXT DEFAULT NULL;

-- Combat Bonus vs Higher Pop Civilization.

ALTER TABLE Traits ADD COLUMN 'CombatBonusVsHigherPop' INTEGER DEFAULT 0;

-- Earn a set number of free buildings. Uses standard 'FreeBuilding' trait (i.e. Carthage). No tech requirement.

ALTER TABLE Traits ADD COLUMN 'NumFreeBuildings' INTEGER DEFAULT 0;

-- Reduce national pop requirements for national wonders.

ALTER TABLE Traits ADD COLUMN 'NationalPopReqModifier' INTEGER DEFAULT 0;

-- Adds a tech requirement to the free buildings.

ALTER TABLE Traits ADD COLUMN 'FreeBuildingPrereqTech' TEXT DEFAULT NULL;

-- Adds a tech requirement to the free capital building.

ALTER TABLE Traits ADD COLUMN 'CapitalFreeBuildingPrereqTech' TEXT DEFAULT NULL;

-- Adds a trait that lets a player conquer a city-state if they can bully it.

ALTER TABLE Traits ADD COLUMN 'BullyAnnex' BOOLEAN DEFAULT 0;

-- Adds a trait that lets a player ignore friends/PtP/allies in bully calculation
ALTER TABLE Traits ADD COLUMN 'IgnoreBullyPenalties' BOOLEAN DEFAULT 0;

-- Adds a trait that makes CS alliances boost the defense of CS cities.

ALTER TABLE Traits ADD COLUMN 'AllianceCSDefense' INTEGER DEFAULT 0;

-- Adds a trait that makes CS alliances boost the strength of CS units.

ALTER TABLE Traits ADD COLUMN 'AllianceCSStrength' INTEGER DEFAULT 0;

-- Adds a trait that converts x% of tourism from cities to GAP, where x is the integer below.

ALTER TABLE Traits ADD COLUMN 'TourismToGAP' INTEGER DEFAULT 0;

-- Adds a trait that converts x% of GROSS GPT to GAP, where x is the interger below.

ALTER TABLE Traits ADD COLUMN 'GoldToGAP' INTEGER DEFAULT 0;

-- Adds a trait that boosts the value of historic event tourism. 1 = 10%, 2 = 20%, etc.

ALTER TABLE Traits ADD COLUMN 'EventTourismBoost' INTEGER DEFAULT 0;

-- Adds x# of GP Points to Capital (scaling with era) when you complete a Historic Event.

ALTER TABLE Traits ADD COLUMN 'EventGP' INTEGER DEFAULT 0;

-- Picks a random Great Engineer, Great Scietist, or Great Merchant Points to Player Cities (scaling with era) when you adopt a new social policy, ideology, or branch.
ALTER TABLE Traits ADD COLUMN 'PolicyGEorGM' INTEGER DEFAULT 0;

-- Changes City Ranged Strike during Golden Ages
ALTER TABLE Traits ADD COLUMN 'GAGarrisonCityRangeStrikeModifier' INTEGER DEFAULT 0;

-- Player enters a golden age on a declaration of war, either as attacking or defending
ALTER TABLE Traits ADD COLUMN 'GoldenAgeOnWar' BOOLEAN DEFAULT 0;

-- Is this Civ unable to cancel razing?
ALTER TABLE Traits ADD COLUMN 'UnableToCancelRazing' BOOLEAN DEFAULT 0;

-- Puppet negative modifiers reduced
ALTER TABLE Traits ADD COLUMN 'ReducePuppetPenalties' INTEGER DEFAULT 0;

-- Player gains a free policy after unlocking x number of technologies from the tech tree.
ALTER TABLE Traits ADD COLUMN 'FreePolicyPerXTechs' INTEGER default 0;

-- Does this Civ get extra damage from multiple attacks on same target?
ALTER TABLE Traits ADD COLUMN 'MultipleAttackBonus' INTEGER DEFAULT 0;

-- Does this Civ get extra influence from meeting a CS?
ALTER TABLE Traits ADD COLUMN 'InfluenceMeetCS' INTEGER DEFAULT 0;

-- Civ gets bonuses to monopoly yields
ALTER TABLE Traits ADD COLUMN 'MonopolyModFlat' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'MonopolyModPercent' INTEGER DEFAULT 0;


-- Grants a free valid promotion to a unit when it is on a type of improvement (farm, mine, etc.).

ALTER TABLE Improvements ADD COLUMN 'UnitFreePromotion' TEXT DEFAULT NULL;

-- Grants Free Experience when Unit is On Improvement plot (must be owned) on Do Turn.
ALTER TABLE Improvements ADD COLUMN 'UnitPlotExperience' INTEGER DEFAULT 0;

-- Grants Free Experience when Unit is On Improvement plot during Golden Ages (must be owned) on Do Turn.
ALTER TABLE Improvements ADD COLUMN 'GAUnitPlotExperience' INTEGER DEFAULT 0;

-- Improvement grants extra moves when unit is on this plot
ALTER TABLE Improvements ADD COLUMN 'MovesChange' INTEGER DEFAULT 0;

-- Improvement requires fresh water, coast, or river adjacency to make valid.
ALTER TABLE Improvements ADD COLUMN 'WaterAdjacencyMakesValid' BOOLEAN DEFAULT 0;

-- Table for Lua elements that we don't want shown in Civ selection screen or in Civilopedia
ALTER TABLE Improvements ADD 'ShowInPedia' BOOLEAN DEFAULT 1;

-- Allows you to set a tech that makes an impassable terrain/feature element passable.
ALTER TABLE Features ADD COLUMN 'PassableTechFeature' TEXT DEFAULT NULL;

-- Allows you to set a promotion gained for owning a feature. Applies to new units.
ALTER TABLE Features ADD COLUMN 'FreePromotionIfOwned' TEXT DEFAULT NULL;

-- Allows you to set a tech that makes an impassable terrain/feature element passable.
ALTER TABLE Terrains ADD COLUMN 'PassableTechTerrain' TEXT DEFAULT NULL;

-- Grants a free valid promotion to a unit when it enters a type of feature (forest, marsh, etc.).

ALTER TABLE Features ADD COLUMN 'LocationUnitFreePromotion' TEXT DEFAULT NULL;

-- Grants a free valid promotion to a unit when it spawns on type of feature (forest, marsh, etc.). Generally used for barbarians.

ALTER TABLE Features ADD COLUMN 'SpawnLocationUnitFreePromotion' TEXT DEFAULT NULL;

-- Grants a free valid promotion to a unit when it spawns adjacent to a type of feature (forest, marsh, etc.). Generally used for barbarians.

ALTER TABLE Features ADD COLUMN 'AdjacentSpawnLocationUnitFreePromotion' TEXT DEFAULT NULL;

--Ability to add additional Natural Wonders with Graphics
ALTER TABLE Features ADD PseudoNaturalWonder INTEGER DEFAULT 0;

-- Grants a free valid promotion to a unit when it enters a type of terrain (grassland, plains, coast, etc.).

ALTER TABLE Terrains ADD COLUMN 'LocationUnitFreePromotion' TEXT DEFAULT NULL;

-- Grants a free valid promotion to a unit when it spawns on type of terrain (grassland, plains, coast, etc.). Generally used for barbarians.

ALTER TABLE Terrains ADD COLUMN 'SpawnLocationUnitFreePromotion' TEXT DEFAULT NULL;

-- Same as above, but for spawning adjacent to a terrain type.
ALTER TABLE Terrains ADD COLUMN 'AdjacentSpawnLocationUnitFreePromotion' TEXT DEFAULT NULL;

-- Grants a free valid promotion to a unit when it is adjacent to a type of terrain (grassland, plains, coast, etc.). Generally used for barbarians.

ALTER TABLE Terrains ADD COLUMN 'AdjacentUnitFreePromotion' TEXT DEFAULT NULL;

-- Adds ability for units to have max HP values other than 100 (whoward)
ALTER TABLE Units ADD MaxHitPoints INTEGER DEFAULT 100;
ALTER TABLE UnitPromotions ADD MaxHitPointsChange INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD MaxHitPointsModifier INTEGER DEFAULT 0;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundMid' BOOLEAN DEFAULT 0;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundLate' BOOLEAN DEFAULT 0;

-- Adds ability for Venetian merchants to create puppets.
ALTER TABLE Units ADD COLUMN 'FoundColony' INTEGER DEFAULT 0;

-- Adds marker for city attack only units (for AI)
ALTER TABLE Units ADD COLUMN 'CityAttackOnly' BOOLEAN DEFAULT 0;

-- Adds Culture from experience to owner of unit when disbanded or upgraded
ALTER TABLE Units ADD COLUMN 'CulExpOnDisbandUpgrade' BOOLEAN DEFAULT 0;

-- Increases the cost by this amount every time you build the unit
ALTER TABLE Units ADD 'CostScalerNumRepeats' INTEGER DEFAULT 0;

-- Tourism Bomb can now alternative give 50% of tourism to all civs, and 100% of tourism output to target civ for x turns
ALTER TABLE Units ADD COLUMN 'TourismBonusTurns' INTEGER DEFAULT 0;

-- Adds minimum national population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'NationalPopRequired' INTEGER DEFAULT 0;

-- Adds minimum local population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'LocalPopRequired' INTEGER DEFAULT 0;

-- Adds population instantly to city.
ALTER TABLE Buildings ADD COLUMN 'PopulationChange' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for land plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleCity' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for water plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleWater' INTEGER DEFAULT 0;

-- x damage to units that end turn on a deep water tile owned by a city
ALTER TABLE Buildings ADD COLUMN 'DeepWaterTileDamage' INTEGER DEFAULT 0;

-- One building gives all cities this ability
ALTER TABLE Buildings ADD COLUMN 'AllowsFoodTradeRoutesGlobal' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'AllowsProductionTradeRoutesGlobal' BOOLEAN DEFAULT 0;

-- Local city connection strong
ALTER TABLE Buildings ADD COLUMN 'CityConnectionGoldModifier' INTEGER DEFAULT 0;

-- Adds abiility for units to upgrade in allied CS or vassal lands.
ALTER TABLE Policies ADD COLUMN 'UpgradeCSVassalTerritory' BOOLEAN DEFAULT 0;

-- Adds ability for spies to steal GW/tech through Spy Actions
ALTER TABLE Policies ADD COLUMN 'EnablesTechSteal' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'EnablesGWSteal' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'EnablesTechSteal' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'EnablesGWSteal' BOOLEAN DEFAULT 0;


-- Adds event tourism from digging up sites.
ALTER TABLE Policies ADD COLUMN 'ArchaeologicalDigTourism' INTEGER DEFAULT 0;
-- Adds event tourism from golden ages starting.
ALTER TABLE Policies ADD COLUMN 'GoldenAgeTourism' INTEGER DEFAULT 0;

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

-- % modifiers to empire size modifier - negative = good!
ALTER TABLE Buildings ADD COLUMN 'EmpireSizeModifierReduction' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'EmpireSizeModifierReductionGlobal' INTEGER DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in this city
ALTER TABLE Buildings ADD COLUMN 'DistressFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'PovertyFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BoredomFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ReligiousUnrestFlatReduction' INTEGER DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in all Cities
ALTER TABLE Buildings ADD COLUMN 'DistressFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'PovertyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BoredomFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ReligiousUnrestFlatReductionGlobal' INTEGER DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in all Cities
ALTER TABLE Policies ADD COLUMN 'DistressFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'PovertyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BoredomFlatReductionGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestFlatReductionGlobal' INTEGER DEFAULT 0;

-- Changes the global median for a Need in this city by x% - values should be negative to be good!
ALTER TABLE Buildings ADD COLUMN 'BasicNeedsMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'GoldMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ScienceMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'CultureMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ReligiousUnrestModifier' INTEGER DEFAULT 0;

-- Changes the global median for a Need in the capital by x% - values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'BasicNeedsMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'GoldMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ScienceMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CultureMedianModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestModifierCapital' INTEGER DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Buildings ADD COLUMN 'BasicNeedsMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'GoldMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ScienceMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'CultureMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'ReligiousUnrestModifierGlobal' INTEGER DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'BasicNeedsMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'GoldMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ScienceMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CultureMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'ReligiousUnrestModifierGlobal' INTEGER DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Traits ADD COLUMN 'BasicNeedsMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'GoldMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'ScienceMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'CultureMedianModifierGlobal' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'ReligiousUnrestModifierGlobal' INTEGER DEFAULT 0;

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

-- Can bully friendly CSs (no penalty)
ALTER TABLE Policies ADD COLUMN 'CanBullyFriendlyCS' BOOLEAN DEFAULT 0;

-- Allied CS influence does not decline at war
ALTER TABLE Policies ADD COLUMN 'NoAlliedCSInfluenceDecayAtWar' BOOLEAN DEFAULT 0;

-- Allied CS influence resting point increases while at war
ALTER TABLE Policies ADD COLUMN 'MinimumAllyInfluenceIncreaseAtWar' INTEGER DEFAULT 0;

-- Vassals don't rebel and can't be forced out by deals or WC
ALTER TABLE Policies ADD COLUMN 'VassalsNoRebel' BOOLEAN DEFAULT 0;

-- Get more tenets when you adopt for first time
ALTER TABLE Traits ADD COLUMN 'ExtraTenetsFirstAdoption' INTEGER DEFAULT 0;

-- New Traits -- Can Only Select from beliefs unique to the civ
ALTER TABLE Traits ADD COLUMN 'UniqueBeliefsOnly' BOOLEAN DEFAULT 0;

-- New Traits -- Combat Boost from nearby owned NWs. Requires distance value from improvementdistancerange XML
ALTER TABLE Traits ADD COLUMN 'CombatBoostNearNaturalWonder' BOOLEAN DEFAULT 0;

-- New Belief Element -- Assign Belief to Specific Civ only
ALTER TABLE Beliefs ADD COLUMN 'CivilizationType' TEXT DEFAULT NULL;

-- New Traits - Free Great Work(s) on Conquest (or Culture Boost if no great works/slots)

ALTER TABLE Traits ADD COLUMN 'FreeGreatWorkOnConquest' BOOLEAN DEFAULT 0;

-- New Traits - Religious Pressure modified based on population

ALTER TABLE Traits ADD COLUMN 'PopulationBoostReligion' BOOLEAN DEFAULT 0;

-- New Traits - % of capital culture converted to tourism during a GA
ALTER TABLE Traits ADD COLUMN 'TourismGABonus' INTEGER DEFAULT 0;

-- New Traits - Additioanl WC vote for every x CS Alliances
ALTER TABLE Traits ADD COLUMN 'VotePerXCSAlliance' INTEGER DEFAULT 0;

-- New Traits - Golden Age received from favorable peace treaty
ALTER TABLE Traits ADD COLUMN 'GoldenAgeFromVictory' INTEGER DEFAULT 0;

-- New Traits - Can buy owned plots with gold
ALTER TABLE Traits ADD COLUMN 'BuyOwnedTiles' BOOLEAN DEFAULT 0;

-- New Traits - Converts citizens on conquest
ALTER TABLE Traits ADD COLUMN 'Reconquista' BOOLEAN DEFAULT 0;

-- New Traits - No Foreign Religious Spread in cities or allied CSs
ALTER TABLE Traits ADD COLUMN 'NoSpread' BOOLEAN DEFAULT 0;

-- New Traits - GG XP bonus when spawned and removes damage from all owned units
ALTER TABLE Traits ADD COLUMN 'XPBonusFromGGBirth' INTEGER DEFAULT 0;

-- New Traits - Military x% better at intimidating CSs
ALTER TABLE Traits ADD COLUMN 'CSBullyMilitaryStrengthModifier' INTEGER DEFAULT 0;

-- New Traits - Gain %x more yields from bullying CSs
ALTER TABLE Traits ADD COLUMN 'CSBullyValueModifier' INTEGER DEFAULT 0;

-- New Traits - Diplomatic Marriage bonus (Austria UA - CBP)
ALTER TABLE Traits ADD COLUMN 'DiplomaticMarriage' BOOLEAN DEFAULT 0;

-- New Traits - Adoption of Policies/Beliefs/Ideology = free tech
ALTER TABLE Traits ADD COLUMN 'IsAdoptionFreeTech' BOOLEAN DEFAULT 0;

-- New Traits - Extra Growth from WTLKD
ALTER TABLE Traits ADD COLUMN 'GrowthBoon' INTEGER DEFAULT 0;

-- New Traits - Extra GI yields from WTLKD
ALTER TABLE Traits ADD COLUMN 'WLTKDGPImprovementModifier' INTEGER DEFAULT 0;

-- New Traits - WLTKD from GP birth
ALTER TABLE Traits ADD COLUMN 'GPWLTKD' BOOLEAN DEFAULT 0;

-- New Traits - WLTKD from GP expansion - global
ALTER TABLE Traits ADD COLUMN 'ExpansionWLTKD' BOOLEAN DEFAULT 0;

-- New Traits - WLTKD from GWs - global
ALTER TABLE Traits ADD COLUMN 'GreatWorkWLTKD' BOOLEAN DEFAULT 0;

-- New Traits - permanent yield decrease at new era, scaling with empire size. Ties to permanent yields XML table.
ALTER TABLE Traits ADD COLUMN 'PermanentYieldsDecreaseEveryEra' BOOLEAN DEFAULT 0;


-- New Traits - Extra Terrain When Conquering a City
ALTER TABLE Traits ADD COLUMN 'ExtraConqueredCityTerritoryClaimRange' INTEGER DEFAULT 0;

-- New Traits - Extra Terrain When Conquering a City
ALTER TABLE Traits ADD COLUMN 'KeepConqueredBuildings' BOOLEAN DEFAULT 0;

-- New Traits - Extra Terrain Yields IF you have a trade route or city connection crossing tile - this scales with era (making this false causes terrain yields not to be scaled)
ALTER TABLE Traits ADD COLUMN 'TradeRouteOnly' BOOLEAN DEFAULT 0;

-- New Traits - Extra Terrain when you gain a tile naturally - must be the ID # of the TERRAIN_TYPE (i.e. TERRAIN_DESERT is 2)
ALTER TABLE Traits ADD COLUMN 'TerrainClaimBoost' TEXT DEFAULT NULL;

-- New Traits - Mountain Movement and City Connections
ALTER TABLE Traits ADD COLUMN 'MountainPass' BOOLEAN DEFAULT 0;

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

-- Free Votes in WC
ALTER TABLE Policies ADD COLUMN 'FreeWCVotes' INTEGER DEFAULT 0;

-- GP Expend Influence Boost
ALTER TABLE Policies ADD COLUMN 'InfluenceGPExpend' INTEGER DEFAULT 0;

-- Free Trade Route
ALTER TABLE Policies ADD COLUMN 'FreeTradeRoute' INTEGER DEFAULT 0;

-- Free Spy
ALTER TABLE Policies ADD COLUMN 'FreeSpy' INTEGER DEFAULT 0;

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

-- Religious Pressure Modifier (affects cities without a majority religion)
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


-- New Buildings

-- Building Modifies amount of unhappiness provided by an individual city
ALTER TABLE Buildings ADD COLUMN 'LocalUnhappinessModifier' INTEGER DEFAULT 0;

-- Building Modifies amount of maintenance buildings cost in your empire.
ALTER TABLE Buildings ADD COLUMN 'GlobalBuildingGoldMaintenanceMod' INTEGER DEFAULT 0;

-- National Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'NationalFollowerPopRequired' INTEGER DEFAULT 0;

-- Global Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'GlobalFollowerPopRequired' INTEGER DEFAULT 0;

-- Reduces the value above by x value (a % reduction)
ALTER TABLE Buildings ADD COLUMN 'ReformationFollowerReduction' INTEGER DEFAULT 0;

-- Gives all current and future missionaries an extra x spread(s)
ALTER TABLE Buildings ADD COLUMN 'ExtraMissionarySpreadsGlobal' INTEGER DEFAULT 0;

-- Allows for Reformation Policy
ALTER TABLE Buildings ADD COLUMN 'IsReformation' BOOLEAN DEFAULT 0;

-- Allows for Building to be unlocked by a specific policy (not a branch)
ALTER TABLE Buildings ADD COLUMN 'PolicyType' TEXT DEFAULT NULL;

-- Allows for Building to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Buildings ADD COLUMN 'ResourceType' TEXT DEFAULT NULL;

-- Allows for Unit to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Units ADD COLUMN 'ResourceType' TEXT DEFAULT NULL;

-- Assigns a unit to a belief type. If you lack this belief in the city where you wish to buy a unit, you won't be able to buy it with faith.
ALTER TABLE Units ADD COLUMN 'BeliefRequired' TEXT DEFAULT NULL;

-- Allows for Building to be purchased in puppet city
ALTER TABLE Buildings ADD COLUMN 'PuppetPurchaseOverride' BOOLEAN DEFAULT 0;

-- Allows for Building to grant a single WC vote (or any value) - not scaled by CS
ALTER TABLE Buildings ADD COLUMN 'SingleLeagueVotes' INTEGER DEFAULT 0;

-- Allows for All Units/Buildings to be purchased in puppet city
ALTER TABLE Buildings ADD COLUMN 'AllowsPuppetPurchase' BOOLEAN DEFAULT 0;

-- Creates a resource unique to this civ (i.e. Indonesian Candi) in the territory around the city. To make this work with a civ, you'll need to create a new resource modelled on the Indonesian resources and assign them to the civ. Value is indicative of the number of resources that will be granted.
ALTER TABLE Buildings ADD COLUMN 'GrantsRandomResourceTerritory' INTEGER DEFAULT 0;

-- Deprecated. Now inside the Building_ResourcePlotsToPlace table itself. DLL no longer reads/saves this value.
-- Left in for mod compatibility.
-- OLD: Quantity of Resources to place in your Territory. Must be used with 'Building_ResourcePlotsToPlace'. Works like "GrantsRandomResourceTerritory", but any resource in table Resources can be used.
ALTER TABLE Buildings ADD COLUMN 'ResourceQuantityToPlace' INTEGER DEFAULT 0;

-- Boosts trade route religious pressure on buildings
ALTER TABLE Buildings ADD COLUMN 'TradeReligionModifier' INTEGER DEFAULT 0;

-- Allows you to define a building needed by this building (similar to BuildingClassNeeded) -->
ALTER TABLE Buildings ADD COLUMN 'NeedBuildingThisCity' TEXT DEFAULT NULL;

-- Allows you to define a number of WLTKD turns for the City -- 
ALTER TABLE Buildings ADD COLUMN 'WLTKDTurns' INTEGER DEFAULT 0;

-- Allows you to define an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed. 
ALTER TABLE Buildings ADD COLUMN 'EventTourism' INTEGER DEFAULT 0;

-- Allows you to define an amount that a unit will heal in a city whether or not it took an action this turn.
ALTER TABLE Buildings ADD COLUMN 'AlwaysHeal' INTEGER DEFAULT 0;

-- Allows you to define an amount that a city will increase your unit supply cap by x% per pop.
ALTER TABLE Buildings ADD COLUMN 'CitySupplyModifier' INTEGER DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by x% per pop globally.
ALTER TABLE Buildings ADD COLUMN 'CitySupplyModifierGlobal' INTEGER DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by a flat value.
ALTER TABLE Buildings ADD COLUMN 'CitySupplyFlat' INTEGER DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by a flat value globally.
ALTER TABLE Buildings ADD COLUMN 'CitySupplyFlatGlobal' INTEGER DEFAULT 0;

-- Allows buildings to modify a city's ranged strike parameters.
ALTER TABLE Buildings ADD COLUMN 'CityRangedStrikeRange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'CityIndirectFire' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'RangedStrikeModifier' INTEGER DEFAULT 0;

-- Missionaries built by a city gain % more strength
ALTER TABLE Buildings ADD COLUMN 'ExtraMissionaryStrengthGlobal' INTEGER DEFAULT 0;

-- Resource Diversity Provided by a City increased by x% (or reduced by x% if negative
ALTER TABLE Buildings ADD COLUMN 'ResourceDiversityModifier' INTEGER DEFAULT 0;

-- Tourism Mod, global, from WC
ALTER TABLE Resolutions ADD COLUMN 'TourismMod' integer default 0;

-- When a City with this building finishes a land ITR, gain tourism with the target civ.
ALTER TABLE Buildings ADD COLUMN 'FinishLandTRTourism' INTEGER DEFAULT 0;

-- When a City with this building finishes a sea ITR, gain tourism with the target civ.
ALTER TABLE Buildings ADD COLUMN 'FinishSeaTRTourism' INTEGER DEFAULT 0;

-- WC vote per GPT - GPT / x (value below) = votes
ALTER TABLE Buildings ADD COLUMN 'VotesPerGPT' INTEGER DEFAULT 0;

-- Requires Rail Connection to be built
ALTER TABLE Buildings ADD COLUMN 'RequiresRail' BOOLEAN DEFAULT 0;

-- Civ-specific building (but not necessarily a UB!)
ALTER TABLE Buildings ADD COLUMN 'CivilizationRequired' TEXT DEFAULT NULL;

-- reduces purchase cooldown on units
ALTER TABLE Buildings ADD COLUMN 'PurchaseCooldownReduction' INTEGER DEFAULT 0;

-- reduces purchase cooldown on civilian units
ALTER TABLE Buildings ADD COLUMN 'PurchaseCooldownReductionCivilian' INTEGER DEFAULT 0;

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

-- Allows for Unit to be purchased in puppet city
ALTER TABLE Units ADD COLUMN 'PuppetPurchaseOverride' BOOLEAN DEFAULT 0;

-- Allows for Unit to gain more yields and experience from a ruin
ALTER TABLE Units ADD COLUMN 'GoodyModifier' INTEGER DEFAULT 0;
-- Allows for Unit to increase your supply cap.
ALTER TABLE Units ADD COLUMN 'SupplyCapBoost' INTEGER DEFAULT 0;

-- Gain Production from best X cities from previous turns - Replacement for Population-based Hurry Production on Great Engineers, etc.
ALTER TABLE Units ADD COLUMN 'BaseProductionTurnsToCount' INTEGER DEFAULT 0;
-- Gain Gold from previous turns - Replacement for Era-based Gold Trade Missions on Great Merchants, etc.
ALTER TABLE Units ADD COLUMN 'BaseGoldTurnsToCount' INTEGER DEFAULT 0;
-- Gain GAP from previous turns - Replacement for free GA on Great Artists, etc.
ALTER TABLE Units ADD COLUMN 'BaseTurnsForGAPToCount' INTEGER DEFAULT 0;
-- Gain turns of WLTKD - Replacement for Influence from Trade Missions on Great Merchants, etc.
ALTER TABLE Units ADD Column 'BaseWLTKDTurns' INTEGER DEFAULT 0;

-- Grants resource to improvement
ALTER TABLE Improvements ADD COLUMN 'ImprovementResource' TEXT DEFAULT NULL;
ALTER TABLE Improvements ADD COLUMN 'ImprovementResourceQuantity' INTEGER DEFAULT 0;

-- Improvement creates happiness for builder when finished.
ALTER TABLE Improvements ADD COLUMN 'HappinessOnConstruction' INTEGER DEFAULT 0;

-- Grants obsoletion tech to build (tie to improvement below for function)
ALTER TABLE Builds ADD COLUMN 'ObsoleteTech' TEXT DEFAULT NULL;

-- Builds only kill civilian units (bugfix for design woes re: double improvement logic)
ALTER TABLE Builds ADD COLUMN 'KillOnlyCivilian' BOOLEAN DEFAULT 0;

-- Grants obsoletion tech to improvement (tie to build above for AI)
ALTER TABLE Improvements ADD COLUMN 'ObsoleteTech' TEXT DEFAULT NULL;

-- Improvements can be made valid by being adjacent to a lake
ALTER TABLE Improvements ADD COLUMN 'Lakeside' BOOLEAN DEFAULT 0;

-- Improvements can be made valid by being adjacent to a city
ALTER TABLE Improvements ADD COLUMN 'Cityside' BOOLEAN DEFAULT 0;

-- Improvements can be made valid by being adjacent to X of the same improvement
ALTER TABLE Improvements ADD COLUMN 'XSameAdjacentMakesValid' INTEGER DEFAULT 0;

-- Improvements can be made valid by being on coast terrain
ALTER TABLE Improvements ADD COLUMN 'CoastMakesValid' BOOLEAN DEFAULT 0;

-- Improvements can generate vision for builder x tiles away (radially)
ALTER TABLE Improvements ADD COLUMN 'GrantsVisionXTiles' INTEGER DEFAULT 0;

-- New Goody Hut Additions
ALTER TABLE GoodyHuts ADD COLUMN 'Production' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'GoldenAge' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'FreeTiles' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'Science' INTEGER DEFAULT 0;

-- Additional Goody Hut options, requires NEW_GOODIES CustomModOptions to set True
ALTER TABLE GoodyHuts ADD COLUMN 'Food' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'BorderGrowth' INTEGER DEFAULT 0;

-- Tech Additions
ALTER TABLE Technologies ADD COLUMN 'CityLessEmbarkCost' BOOLEAN;
ALTER TABLE Technologies ADD COLUMN 'CityNoEmbarkCost' BOOLEAN;

-- Tech adds raw happiness to capital
ALTER TABLE Technologies ADD COLUMN 'Happiness' INTEGER DEFAULT 0;

--Units
ALTER TABLE Units ADD 'NumFreeLux' INTEGER DEFAULT 0;

-- GP tabling - use this to define up to 5 GP types with indpendent meters (will not affect rates of other GPs)
-- GPs that you assign the same number to will affect each other, so make sure no other mods are using these!
ALTER TABLE Units ADD 'GPExtra' INTEGER DEFAULT 0;
-- Example: <GPExtra>1</GPExtra> in a unit's XML table will put all of its GPP into a unique meter and a faith buy track.

-- Promotions

ALTER TABLE UnitPromotions ADD 'MinimumRangeRequired' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'MaximumRangeRequired' INTEGER DEFAULT 0;

-- Disallows units w/o 'mounted' flag at unit type level from taking a promotion.
ALTER TABLE UnitPromotions ADD 'MountedOnly' BOOLEAN DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'AttackFullyHealedMod' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'AttackAbove50HealthMod' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'AttackBelowEqual50HealthMod' INTEGER DEFAULT 0;

-- ranged attacks benefit from other units flanking the target
ALTER TABLE UnitPromotions ADD 'RangedFlankAttack' INTEGER DEFAULT 0;
-- counts as extra units when supporting a flank
ALTER TABLE UnitPromotions ADD 'ExtraFlankPower' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'SplashDamage' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'AOEDamageOnKill' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'AoEWhileFortified' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'AoEDamageOnMove' INTEGER DEFAULT 0; -- JJ: Do AoE damage when the unit moves

ALTER TABLE UnitPromotions ADD 'WorkRateMod' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'ReconChange' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'PromotionDuration' INTEGER DEFAULT 0;

-- Promotion restricted to tile. Lost if unit leaves it. Only used in conjuction with free promotions from features and terrains (see above)
ALTER TABLE UnitPromotions ADD COLUMN 'IsLostOnMove' BOOLEAN DEFAULT 0;

-- Put the name of the Promotion in this row in order to negate it with another promotion - currently only works with the terrain/feature related promotion functions above!
ALTER TABLE UnitPromotions ADD 'NegatesPromotion' TEXT DEFAULT NULL;

-- Forced sets the damage the unit will receive from the combat REGARDLESS of how much damage was calculated to be dealt to it.
ALTER TABLE UnitPromotions ADD 'ForcedDamageValue' INTEGER DEFAULT 0;

-- Change reduces damage the unit will receive BASED on how much damage was calculated to be dealt to it. Negative numbers to reduce!
ALTER TABLE UnitPromotions ADD 'ChangeDamageValue' INTEGER DEFAULT 0;

-- A unit cannot be captured by another player (i.e. from prize ships, etc.). Does not apply to civilians.
ALTER TABLE UnitPromotions ADD 'CannotBeCaptured' BOOLEAN DEFAULT 0;

-- Units captured by a unit with this promotion gain the conscript promotion (don't count for military supply). Only used together with CaptureDefeatedEnemy
ALTER TABLE UnitPromotions ADD COLUMN 'CapturedUnitsConscripted' BOOLEAN DEFAULT 0;

-- A unit gains a combat bonus VS barbarians
ALTER TABLE UnitPromotions ADD COLUMN 'BarbarianCombatBonus' INTEGER DEFAULT 0;

-- Ranged units have a % chance to force another unit to retreat, taking additional damage if they cannot do so.
ALTER TABLE UnitPromotions ADD COLUMN 'MoraleBreakChance' INTEGER DEFAULT 0;

-- Promotion bonuses restricted to Barbarians.

ALTER TABLE UnitPromotions ADD COLUMN 'BarbarianOnly' BOOLEAN DEFAULT 0;

-- Promotion bonuses restricted to City States.

ALTER TABLE UnitPromotions ADD COLUMN 'CityStateOnly' BOOLEAN DEFAULT 0;

-- Promotion grants the same bonus as the Japan UA
ALTER TABLE UnitPromotions ADD COLUMN 'StrongerDamaged' BOOLEAN DEFAULT 0;  -- No wounded penalty, instead gains a combat bonus the more damage the unit has
ALTER TABLE UnitPromotions ADD COLUMN 'FightWellDamaged' BOOLEAN DEFAULT 0; -- No wounded penalty, no additional bonus

-- Great General gives extra XP% during a golden age (Persia)
ALTER TABLE UnitPromotions ADD COLUMN 'GeneralGoldenAgeExpPercent' INTEGER DEFAULT 0;

-- The Following Promotions Require IsNearbyPromotion and NearbyRange to be set. IsNearbyPromotion is an "m_unitsAreaEffectPromotion" Unit
ALTER TABLE UnitPromotions ADD IsNearbyPromotion BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyRange INTEGER DEFAULT 0;
-- Set the Domain that Gets the Bonus
ALTER TABLE UnitPromotions ADD COLUMN 'GiveDomain' TEXT DEFAULT NULL REFERENCES Domains(Type);

-- Unit gives additional combat strength to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveCombatMod' INTEGER DEFAULT 0;

-- Unit Gives HP to additional units if they kill an enemy units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveHPHealedIfEnemyKilled' INTEGER DEFAULT 0;

-- Unit Gives additional XP in combat to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveExperiencePercent' INTEGER DEFAULT 0;

-- Unit Gives a bonus to outside friendly lands unis? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveOutsideFriendlyLandsModifier' INTEGER DEFAULT 0;

-- Unit Gives extra attacks to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveExtraAttacks' INTEGER DEFAULT 0;

-- Unit Gives extra defense to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveDefenseMod' INTEGER DEFAULT 0;

-- Unit gives Invisibility to another Unit? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'GiveInvisibility' BOOLEAN DEFAULT 0;

-- Unit only gives these effects at the start of the turn (works for GiveExperiencePercent, GiveCombatMod, GiveDefenseMod, GiveInvisibility, GiveOutsideFriendlyLandsModifier, GiveHPHealedIfEnemyKilled, GiveExtraAttacks)
ALTER TABLE UnitPromotions ADD COLUMN 'GiveOnlyOnStartingTurn' BOOLEAN DEFAULT 0;

-- Unit gains Combat modifier when near cities. Requires IsNearbyPromotion and NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyCityCombatMod INTEGER DEFAULT 0;

-- Unit gains Combat modifier when near friendly cities. Requires IsNearbyPromotion and NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyFriendlyCityCombatMod INTEGER DEFAULT 0;

-- Unit gains Combat modifier when near enemy cities. Requires IsNearbyPromotion and NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyEnemyCityCombatMod INTEGER DEFAULT 0;

-- Unit Gives extra healing to nearby units? Must have set IsNearbyPromotion, NearbyRange, and GiveDomain for this promotion.
ALTER TABLE UnitPromotions ADD COLUMN 'NearbyHealEnemyTerritory' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'NearbyHealNeutralTerritory' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'NearbyHealFriendlyTerritory' INTEGER DEFAULT 0;
-- End

-- Double Movement on Mountains
ALTER TABLE UnitPromotions ADD COLUMN 'MountainsDoubleMove' BOOLEAN DEFAULT 0;

-- Embarking Costs One Movement Point
ALTER TABLE UnitPromotions ADD COLUMN 'EmbarkFlatCost' BOOLEAN DEFAULT 0;

-- Disembarking Costs One Movement Point
ALTER TABLE UnitPromotions ADD COLUMN 'DisembarkFlatCost' BOOLEAN DEFAULT 0;

-- Admirals can use their repair fleet action multiple times before expending.
ALTER TABLE UnitPromotions ADD COLUMN 'NumRepairCharges' INTEGER DEFAULT 0;

-- Base damage dealth to adjacent air units after an unintercepted air sweep
ALTER TABLE UnitPromotions ADD COLUMN 'GetGroundAttackDamage' INTEGER DEFAULT 0;

-- Allows for Unit to increase your supply cap when expended.
ALTER TABLE UnitPromotions ADD COLUMN 'MilitaryCapChange' INTEGER DEFAULT 0;

-- Double Heal in Feature/Terrain
ALTER TABLE UnitPromotions_Features ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions_Terrains ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;

-- Allow Attack and Defense specific modifiers VS domain
ALTER TABLE UnitPromotions_Domains ADD COLUMN 'Attack' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions_Domains ADD COLUMN 'Defense' INTEGER DEFAULT 0;

-- Combat Modifier for determined range near a defined UnitClass
ALTER TABLE UnitPromotions ADD CombatBonusFromNearbyUnitClass INTEGER DEFAULT -1;
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonusRange INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonus INTEGER DEFAULT 0;

-- Unit gains this promotion in Friendly Lands.
ALTER TABLE Units ADD COLUMN 'FriendlyLandsPromotion' TEXT DEFAULT NULL REFERENCES UnitPromotions(Type);

-- Changes the intercept range against air units (NEW)
ALTER TABLE UnitPromotions ADD AirInterceptRangeChange INTEGER DEFAULT 0;

-- Allows you to Convert a unit when it X plot is a different domain, e.g. A great general becomes a great admiral, etc.
ALTER TABLE UnitPromotions ADD ConvertDomainUnit TEXT DEFAULT NULL REFERENCES Units(Type);
ALTER TABLE UnitPromotions ADD ConvertDomain TEXT DEFAULT NULL REFERENCES Domains(Type);
ALTER TABLE Units ADD IsConvertUnit BOOLEAN DEFAULT 0;

-- relates to Great Artist and Great Writer scaling bonuses
ALTER TABLE Units ADD 'ScaleFromNumGWs' INTEGER DEFAULT 0;
ALTER TABLE Units ADD 'ScaleFromNumThemes' INTEGER DEFAULT 0;

-- How many culture bombs can this unit do, must have set CultureBombRadius to a number.
ALTER TABLE Units ADD 'NumberOfCultureBombs' INTEGER DEFAULT 0;

-- City Gains Wonder Production Modifier while this Unit is stationed in this City
ALTER TABLE UnitPromotions ADD WonderProductionModifier INTEGER DEFAULT 0;

-- % of Total Wonder Production Modifier from traits, policies, beliefs, units, improvements, etc. is added to building production modifier when building non-wonderclass buildings, value of 100 is all, 50 is 50%, etc.
ALTER TABLE Traits ADD WonderProductionModifierToBuilding INTEGER DEFAULT 0;

--Grants Wonder Production Modifier based on number of City Improvements. Value is % gained.
ALTER TABLE Improvements ADD WonderProductionModifier INTEGER DEFAULT 0;

-- Unit stuff for minor civs
ALTER TABLE Units ADD COLUMN 'MinorCivGift' BOOLEAN DEFAULT 0;

-- Forbids unit from being a CS UU
ALTER TABLE Units ADD COLUMN 'NoMinorCivGift' BOOLEAN DEFAULT 0;

-- Alters monopoly values below (except for strategic elements, like attack/defense/heal/etc.)
ALTER TABLE Policies ADD COLUMN 'MonopolyModFlat' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'MonopolyModPercent' INTEGER DEFAULT 0;

-- Resource Changes for Monopolies
ALTER TABLE Resources ADD COLUMN 'MonopolyHappiness' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyGALength' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyAttackBonus' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyDefenseBonus' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyMovementBonus' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyHealBonus' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'MonopolyXPBonus' INTEGER DEFAULT 0;
ALTER TABLE Resources ADD COLUMN 'IsMonopoly' BOOLEAN DEFAULT 0;

-- Cooldowns for Units/Buildings
ALTER TABLE Units ADD COLUMN 'PurchaseCooldown' INTEGER DEFAULT 0;
-- Affects only the city the unit is purchased from (works like PurchaseCooldown for faith purchases)
ALTER TABLE Units ADD COLUMN 'LocalFaithPurchaseCooldown' INTEGER DEFAULT 0;
-- Affects Faith purchases for all faith buys in all cities.
ALTER TABLE Units ADD COLUMN 'GlobalFaithPurchaseCooldown' INTEGER DEFAULT 0;

ALTER TABLE Buildings ADD COLUMN 'PurchaseCooldown' INTEGER DEFAULT 0;

-- Base air defense when defending against an airstrike.
ALTER TABLE Units ADD COLUMN 'BaseLandAirDefense' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'LandAirDefenseBonus' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'CityAirStrikeDefense' INTEGER DEFAULT 0;

-- Allows Courthouses to be built in any city
ALTER TABLE Buildings ADD COLUMN 'BuildAnywhere' BOOLEAN DEFAULT 0;

-- Builds 1+ artifact in the building, up to the number of artifact slots in the building.
ALTER TABLE Buildings ADD COLUMN 'FreeArtifacts' INTEGER DEFAULT 0;

-- Adds Mounted trait to a unit that isn't in the unitcombat_mounted combat class
ALTER TABLE Units ADD COLUMN 'IsMounted' BOOLEAN DEFAULT 0;

-- Investment reduction costs -- policy -- negative makes it stronger!
ALTER TABLE Policies ADD COLUMN 'InvestmentModifier' INTEGER DEFAULT 0;

-- Starting era for GP faith purchases
ALTER TABLE Traits ADD COLUMN 'GPFaithPurchaseEra' TEXT DEFAULT NULL;

-- Faith purchase % reduction. Negative reduces.
ALTER TABLE Traits ADD COLUMN 'FaithCostModifier' INTEGER DEFAULT 0;

-- Investment reduction costs -- trait -- negative makes it stronger!
ALTER TABLE Traits ADD COLUMN 'InvestmentModifier' INTEGER DEFAULT 0;

-- Cities produce % culture instead of growth during WLTKD.
ALTER TABLE Traits ADD COLUMN 'WLTKDCultureBoost' INTEGER DEFAULT 0;

-- Cities gain x turns of WLTKD because of a GA trigger!
ALTER TABLE Traits ADD COLUMN 'WLTKDFromGATurns' INTEGER DEFAULT 0;

-- Unhappiness modifier during WLTKD - more negative the better!
ALTER TABLE Traits ADD COLUMN 'WLTKDUnhappinessNeedsMod' INTEGER DEFAULT 0;

-- Start game with x number of spies
ALTER TABLE Traits ADD COLUMN 'StartingSpies' INTEGER DEFAULT 0;

-- Spies begin at x rank
ALTER TABLE Traits ADD COLUMN 'StartingSpyRank' INTEGER DEFAULT 0;

-- Boost CS Quest Value
ALTER TABLE Traits ADD COLUMN 'MinorQuestYieldModifier' INTEGER DEFAULT 0;

-- % culture bonus from conquering cities
ALTER TABLE Traits ADD COLUMN 'CultureBonusModifierConquest' INTEGER DEFAULT 0;

-- % production bonus in all cities from conquering cities
ALTER TABLE Traits ADD COLUMN 'ProductionBonusModifierConquest' INTEGER DEFAULT 0;

-- Global Espionage Modifier
ALTER TABLE Traits ADD COLUMN 'EspionageRateModifier' INTEGER DEFAULT 0;

-- Increase Spy Rank power without changing rank enum value
ALTER TABLE Traits ADD COLUMN 'SpyExtraRankBonus' INTEGER DEFAULT 0;

-- Spy travel/intermediate action rate modifier
ALTER TABLE Traits ADD COLUMN 'SpyMoveRateModifier' INTEGER DEFAULT 0;

-- GWAM from conquest
ALTER TABLE Traits ADD COLUMN 'CityConquestGWAM' INTEGER DEFAULT 0;

-- Shared religion tourism modifier, same as the one for policies
ALTER TABLE Traits ADD COLUMN 'SharedReligionTourismModifier' INTEGER DEFAULT 0;

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

--Espionage Modifier for Policies - should be negative for player benefit!
ALTER TABLE Policies ADD COLUMN 'EspionageModifier' INTEGER DEFAULT 0;

--Espionage Modifier for Policies - changes durations of spy missions by a fixed amount of turns for friendly/enemy spies
ALTER TABLE Policies ADD COLUMN 'EspionageTurnsModifierFriendly' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'EspionageTurnsModifierEnemy' INTEGER DEFAULT 0;

-- C4DF Function

ALTER TABLE Buildings ADD COLUMN 'VassalLevyEra' BOOLEAN DEFAULT 0;

-- Processes

-- Adds ability to turn production into defense/healing in a city for a process
ALTER TABLE Processes ADD COLUMN 'DefenseValue' INTEGER DEFAULT 0;

-- Unique processes, requires CIVILIZATIONS_UNIQUE_PROCESSES in CustomModOptions
ALTER TABLE Processes ADD COLUMN 'CivilizationType' TEXT DEFAULT NULL;


-- Projects
ALTER TABLE Projects ADD COLUMN 'FreeBuildingClassIfFirst' TEXT DEFAULT NULL;
ALTER TABLE Projects ADD COLUMN 'FreePolicyIfFirst' TEXT DEFAULT NULL;

ALTER TABLE Projects ADD COLUMN 'CostScalerEra' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'CostScalerNumRepeats' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'IsRepeatable' BOOLEAN DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'Happiness' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'EmpireSizeModifierReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'DistressFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'PovertyFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'IlliteracyFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'BoredomFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'ReligiousUnrestFlatReduction' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'BasicNeedsMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'GoldMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'ScienceMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'CultureMedianModifier' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'ReligiousUnrestModifier' INTEGER DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'EspionageMod' INTEGER DEFAULT 0;

ALTER TABLE Projects ADD COLUMN 'InfluenceAllRequired' BOOLEAN DEFAULT 0;
ALTER TABLE Projects ADD COLUMN 'IdeologyRequired' BOOLEAN DEFAULT 0;

-- require x tier 3 tenets prior to construction
ALTER TABLE Projects ADD COLUMN 'NumRequiredTier3Tenets' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'NumRequiredTier3Tenets' INTEGER DEFAULT 0;

--Increase Air Unit Support Per City Global
ALTER TABLE Buildings ADD COLUMN 'AirModifierGlobal' INTEGER DEFAULT 0;

ALTER TABLE Buildings ADD COLUMN 'EventChoiceRequiredActive' TEXT DEFAULT NULL;
ALTER TABLE Buildings ADD COLUMN 'CityEventChoiceRequiredActive' TEXT DEFAULT NULL;


-- Resources
ALTER TABLE Resources ADD COLUMN 'StrategicHelp' TEXT DEFAULT NULL;

ALTER TABLE Policies ADD COLUMN 'TRSpeedBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TRVisionBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXPolicies' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXGreatWorks' INTEGER DEFAULT 0;

-- Alters food consumption of specialists - use integer (is raised to 100s later)
ALTER TABLE Policies ADD COLUMN 'SpecialistFoodChange' INTEGER DEFAULT 0;

-- Trade Routes
ALTER TABLE Policies ADD COLUMN 'ExtraCultureandScienceTradeRoutes' INTEGER DEFAULT 0;

-- CORPORATIONS
ALTER TABLE Technologies ADD COLUMN 'CorporationsEnabled' BOOLEAN;

ALTER TABLE Buildings ADD COLUMN 'IsCorporation' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'GPRateModifierPerXFranchises' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'TRSpeedBoost' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'TRVisionBoost' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'TRTurnModGlobal' INTEGER DEFAULT 0; --modifies the turns a TR takes to complete, an int between 100 and -100, like a percent
ALTER TABLE Buildings ADD COLUMN 'TRTurnModLocal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'OfficeBenefitHelper' TEXT DEFAULT NULL;

-- Corporation Policies
ALTER TABLE Policies ADD COLUMN 'CorporationOfficesAsNumFranchises' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationNumFreeFranchiseAbovePopular' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationRandomForeignFranchiseMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'AdditionalNumFranchisesMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'AdditionalNumFranchises' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoForeignCorpsInCities' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoFranchisesInForeignCities' BOOLEAN DEFAULT 0;


-- 20 = 20% additional, NOT 1/5 of existing value. this stacks, so 120%, 140%, 160%, etc...

-- Minor Civs
ALTER TABLE MinorCivilizations ADD COLUMN 'BullyUnitClass' TEXT DEFAULT NULL;

-- CSD
ALTER TABLE Buildings ADD COLUMN 'DPToVotes' integer default 0;
ALTER TABLE UnitPromotions ADD COLUMN 'DiploMissionInfluence' INTEGER DEFAULT 0;

-- GameSpeed for JFD
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMin' INTEGER DEFAULT 0;
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMax' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'SecondaryPantheon' BOOLEAN DEFAULT 0;

-- Plague Stuff
ALTER TABLE UnitPromotions ADD COLUMN 'PlagueChance' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'PlaguePromotion' TEXT DEFAULT NULL REFERENCES UnitPromotions(Type);
ALTER TABLE UnitPromotions ADD COLUMN 'IsPlague' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'PlagueID' INTEGER DEFAULT -1;
ALTER TABLE UnitPromotions ADD COLUMN 'PlaguePriority' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'PlagueIDImmunity' INTEGER DEFAULT 0;

-- Modify CS/RCS based on terrain roughness
ALTER TABLE UnitPromotions ADD COLUMN 'OpenFromMod' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'RoughFromMod' INTEGER DEFAULT 0;

ALTER TABLE Buildings ADD COLUMN 'IsDummy' BOOLEAN DEFAULT 0;

-- HH Mod
ALTER TABLE Policies ADD COLUMN 'ExtraNaturalWonderHappiness' INTEGER DEFAULT 0;
--ALTER TABLE Policies ADD CONSTRAINT ck_ExtraNaturalWonderHappiness CHECK (ExtraNaturalWonderHappiness >= 0);

-- Worlds
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCities' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCityStates' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'ReformationPercentRequired' INTEGER DEFAULT 100;
ALTER TABLE Worlds ADD COLUMN 'NumCitiesTourismCostMod' INTEGER DEFAULT 0;

-- New City Plot Yields Method
ALTER TABLE Yields ADD COLUMN 'MinCityFlatFreshWater' INTEGER DEFAULT 0;
ALTER TABLE Yields ADD COLUMN 'MinCityFlatNoFreshWater' INTEGER DEFAULT 0;
ALTER TABLE Yields ADD COLUMN 'MinCityHillFreshWater' INTEGER DEFAULT 0;
ALTER TABLE Yields ADD COLUMN 'MinCityHillNoFreshWater' INTEGER DEFAULT 0;
ALTER TABLE Yields ADD COLUMN 'MinCityMountainFreshWater' INTEGER DEFAULT 0;
ALTER TABLE Yields ADD COLUMN 'MinCityMountainNoFreshWater' INTEGER DEFAULT 0;

-- Policy Initialize Number of best units in captial. Requries 'IncludesOneShotFreeUnits' Set to true.
ALTER TABLE Policies ADD COLUMN 'BestNumberLandCombatUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberLandRangedUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberSeaCombatUnitClass' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'BestNumberSeaRangedUnitClass' INTEGER DEFAULT 0;

-- Extra Military Supply from Population
ALTER TABLE Policies ADD COLUMN 'ExtraSupplyPerPopulation' INTEGER DEFAULT 0;

ALTER TABLE Units ADD COLUMN 'NoSupply' INTEGER DEFAULT 0;
  
ALTER TABLE UnitPromotions ADD COLUMN 'NoSupply' INTEGER DEFAULT 0;

-- Spawn Best Melee Unit on an Improvement during a DOW
ALTER TABLE Traits ADD COLUMN 'BestUnitSpawnOnImpDOW' BOOLEAN DEFAULT 0;
ALTER TABLE Traits ADD BestUnitImprovement TEXT DEFAULT NULL REFERENCES Improvements(Type);

-- Spawn best Melee Type Unit on finishing a Build (accounts for Domain of Build)
ALTER TABLE Builds ADD COLUMN 'IsFreeBestDomainUnit' BOOLEAN DEFAULT 0;

-- Unit Upgrades for free when reaching it's UnitClass Upgrade Tech
ALTER TABLE Units ADD COLUMN 'FreeUpgrade' BOOLEAN DEFAULT 0;

-- Naval units can only attack coastal tiles regardless of range.
ALTER TABLE Units ADD COLUMN 'CoastalFireOnly' BOOLEAN DEFAULT 0;

-- Unit gets a new Combat Strength in specific Era, must be set to 'true' or 1, if using Unit_EraCombatStrength, and Unit_EraCombatStrength must be filled in.
ALTER TABLE Units ADD COLUMN 'UnitEraUpgrade' BOOLEAN DEFAULT 0;

-- Grants a free building to a city when founded
ALTER TABLE Policies ADD COLUMN 'NewCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free building to all existing and future cities
ALTER TABLE Policies ADD COLUMN 'AllCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free building to newly founded cities
ALTER TABLE Policies ADD COLUMN 'NewFoundCityFreeBuilding' TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

-- Grants a free unit to newly founded cities
ALTER TABLE Policies ADD COLUMN 'NewFoundCityFreeUnit' TEXT DEFAULT NULL REFERENCES UnitClasses(Type);

-- Promotion grants a unit with XP if stacked with a Great General (or great admiral if a boat)
ALTER TABLE UnitPromotions ADD COLUMN 'StackedGreatGeneralXP' INTEGER DEFAULT 0;

-- Promotions 
ALTER TABLE UnitPromotions ADD COLUMN 'GoodyHutYieldBonus' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'GainsXPFromScouting' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'GainsXPFromPillaging' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'GainsXPFromSpotting' BOOLEAN DEFAULT 0;

ALTER TABLE UnitPromotions ADD COLUMN 'MultiAttackBonus' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'DamageReductionCityAssault' INTEGER DEFAULT 0;

-- Note: The below entries are used, e.g. mounting or dismounting a unit, say a Lancer gets below 50 HP "DamageThreshold", and can "dismount" and fortify as an Infantry type unit.

-- Unit will convert to another UnitType. Must define a "DamageThreshold" and the "ConvertDamageOrFullHPUnit" Type
ALTER TABLE UnitPromotions ADD COLUMN 'IsConvertOnDamage' BOOLEAN DEFAULT 0;

-- Unit will convert to the original UnitType when Max Hit Points are restored. Must define "ConvertDamageOrFullHPUnit" Type to the original Unit.
ALTER TABLE UnitPromotions ADD COLUMN 'IsConvertOnFullHP' BOOLEAN DEFAULT 0;

-- Unit will convert to another UnitType if "IsConvertOnDamage" and "DamageThreshold" are defined. If used to convert back to the original unit when full HP is restored, "IsConvertOnFullHp" must be defined.
ALTER TABLE UnitPromotions ADD COLUMN 'ConvertDamageOrFullHPUnit' TEXT DEFAULT NULL REFERENCES Units(Type);

-- Unit will convert to another UnitType. Must define a "IsConvertOnDamage" and the "ConvertDamageOrFullHPUnit" Type. Or Can be set with IsConvertEnemyUnitToBarbarian
ALTER TABLE UnitPromotions ADD COLUMN 'DamageThreshold' INTEGER DEFAULT 0;

-- Can this unit convert an enemy unit into a barbarian? Must set DamageThreshold to a value you want enemy to convert.
ALTER TABLE UnitPromotions ADD COLUMN 'IsConvertEnemyUnitToBarbarian' BOOLEAN DEFAULT 0;

-- Special Units that have a different Special rating can be modified here to load on to ships (e.g. Great People).
ALTER TABLE Units ADD COLUMN 'SpecialUnitCargoLoad' TEXT DEFAULT NULL REFERENCES SpecialUnits(Type);

-- Does this Civ get a GG/GA Rate Modifier bonus from denunciations and wars?
ALTER TABLE Traits ADD COLUMN 'GGGARateFromDenunciationsAndWars' INTEGER DEFAULT 0;

-- War Weariness Modifier
ALTER TABLE Traits ADD COLUMN 'WarWearinessModifier' INTEGER DEFAULT 0;

-- War Weariness Modifier
ALTER TABLE Traits ADD COLUMN 'EnemyWarWearinessModifier' INTEGER DEFAULT 0;

-- Bully Annex % Modifier for Yields for Mongolia UA
ALTER TABLE Traits ADD COLUMN 'BullyYieldMultiplierAnnex' INTEGER DEFAULT 0;

-- Does this civ get a free Unit.Type on Conquest? Must be able to train it first....
ALTER TABLE Traits ADD COLUMN 'FreeUnitOnConquest' TEXT DEFAULT NULL REFERENCES Units(Type);

-- Can this unit only be trained during War?
ALTER TABLE Units ADD COLUMN 'WarOnly' BOOLEAN DEFAULT 0;

-- Civ gets an influence boost and Great Admiral Points when sending a Trade Route to a minor Civ.
ALTER TABLE Traits ADD COLUMN 'TradeRouteMinorInfluenceAP' BOOLEAN DEFAULT 0;

-- Can this building be built next to any body of water?
ALTER TABLE Buildings ADD COLUMN 'AnyWater' BOOLEAN DEFAULT 0;

--Espionage Modifier for Policies - changes durations of spy missions by a fixed amount of turns for friendly/enemy spies
ALTER TABLE Buildings ADD COLUMN 'EspionageTurnsModifierFriendly' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'EspionageTurnsModifierEnemyCity' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'EspionageTurnsModifierEnemyGlobal' INTEGER DEFAULT 0;

-- Promotion grants additional combat strength if on a pillaged improvement
ALTER TABLE UnitPromotions ADD COLUMN 'PillageBonusStrength' INTEGER DEFAULT 0;

-- Improvement creates a feature. Will remove the improvement when complete, leaving only the feature.
ALTER TABLE Improvements ADD COLUMN 'CreatesFeature' TEXT DEFAULT NULL REFERENCES Features(Type);

-- What is the chance to get a random resource? Resource spawned is random and selected based on the various resource boolean tables.
ALTER TABLE Improvements ADD COLUMN 'RandResourceChance' INTEGER DEFAULT 0;

-- Removes the improvement when built. Useful in combination with CreatesFeature.
ALTER TABLE Improvements ADD COLUMN 'RemoveWhenComplete' BOOLEAN DEFAULT 0;

-- Units that stand on this improvement don't leave it when they attack (like from a city)
ALTER TABLE Improvements ADD COLUMN 'NoFollowUp' BOOLEAN DEFAULT 0;

-- Start a WLTKD when this unit is born or gained. GP's only.
ALTER TABLE Units ADD COLUMN 'WLTKDFromBirth' BOOLEAN DEFAULT 0;

-- Civ cities gets a production modifier (from 1%) for every specialist that they have.
ALTER TABLE Traits ADD COLUMN 'ProdModFromNumSpecialists' BOOLEAN DEFAULT 0;

-- Player during a Golden Age receives a City Attack modifier definable.
ALTER TABLE Traits ADD COLUMN 'ConquestOfTheWorldCityAttack' INTEGER DEFAULT 0;

-- Internal trade route yields doubled during Golden Ages, starts a Golden Age when conquering a city and extends if already in Golden Age.
ALTER TABLE Traits ADD COLUMN 'ConquestOfTheWorld' BOOLEAN DEFAULT 0;

-- Start a GoldenAge when this unit is born or gained. GP's only.
ALTER TABLE Units ADD 'GoldenAgeFromBirth' BOOLEAN DEFAULT 0;

-- Define a defense modifier to a building, like GlobalDefenseModifier (but only local).
ALTER TABLE Buildings ADD 'BuildingDefenseModifier' INTEGER DEFAULT 0;

-- Damage to the city with this building is reduced by this flat amount.
ALTER TABLE Buildings ADD 'DamageReductionFlat' INTEGER DEFAULT 0;

-- Define a modifier for all tile/building based tourism in all cities.
ALTER TABLE Buildings ADD 'GlobalLandmarksTourismPercent' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD 'LandmarksTourismPercent' INTEGER DEFAULT 0;

-- Define a modifier for all great work tourism in all cities.
ALTER TABLE Buildings ADD 'GlobalGreatWorksTourismModifier' INTEGER DEFAULT 0;

-- Define a modifier for cities to be able to intercept nukes.
-- Atomic Bombs are completely destroyed, Nuclear Missiles are converted to Atomic Bombs before detonation.
ALTER TABLE Buildings ADD 'NukeInterceptionChance' INTEGER DEFAULT 0;

-- Table for Lua elements that we don't want shown in Civ selection screen or in Civilopedia
ALTER TABLE Buildings ADD 'ShowInPedia' BOOLEAN DEFAULT 1;

-- Gives a boost of pressure in the city when built (based on what the religion is in your capital)
ALTER TABLE Buildings ADD 'InstantReligiousPressure' INTEGER DEFAULT 0;

-- Promotion grants additional religious pressure when this unit is garrisoned in the city (if the player has a religion).
ALTER TABLE UnitPromotions ADD COLUMN 'ReligiousPressureModifier' INTEGER DEFAULT 0;

-- Trait allows player to have every unit upgraded once tech is reached.
ALTER TABLE Traits ADD COLUMN 'FreeUpgrade' BOOLEAN DEFAULT 0;

-- Gain a culture boost equal to 4X the culture per turn when this unit is born. GP's only
ALTER TABLE Units ADD 'CultureBoost' BOOLEAN DEFAULT 0;

-- Unit gets extra attacks and partial health is restored upon killing an enemy unit.
ALTER TABLE Units ADD 'ExtraAttackHealthOnKill' BOOLEAN DEFAULT 0;

-- Trait allows player to have every unit upgraded once tech is reached.
ALTER TABLE Traits ADD COLUMN 'VotePerXCSFollowingYourReligion' INTEGER DEFAULT 0;

-- Trait allows player a % chance to convert religious units when they enter player's territory.
ALTER TABLE Traits ADD COLUMN 'ChanceToConvertReligiousUnits' INTEGER DEFAULT 0;

-- When a unit with this promotion is adjacent to a city, city strength value is boosted by X %.
ALTER TABLE UnitPromotions ADD COLUMN 'AdjacentCityDefenseMod' INTEGER DEFAULT 0;

-- Traveling Citadel.
ALTER TABLE UnitPromotions ADD COLUMN 'NearbyEnemyDamage' INTEGER DEFAULT 0;

-- Reduce Enemy Movement at start of turn.
ALTER TABLE UnitPromotions ADD COLUMN 'AdjacentEnemySapMovement' INTEGER DEFAULT 0;

-- Enemy Units gain the "EnemyWarSawPactPromotion" promotio when in your territory or Friendly City States or Player's that follow the same Ideology. Must define "EnemyWarSawPactPromotion" promotion type for this to work (see below).
ALTER TABLE Traits ADD COLUMN 'WarsawPact' BOOLEAN DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'EnemyWarSawPactPromotion' TEXT DEFAULT NULL REFERENCES UnitPromotions(Type);

-- Build adds an instant yield of culture to Player's culture pool.
ALTER TABLE Builds ADD COLUMN 'CultureBoost' BOOLEAN DEFAULT 0;

-- When a unit (civilian or combat) with this promotion is stationed in a City, City gains X% modifier towards building military units.
ALTER TABLE UnitPromotions ADD COLUMN 'MilitaryProductionModifier' INTEGER DEFAULT 0;

-- Unit gets the "HighSeaRaider" Promotion Entry (if defined) when it plunders a Trade Route.
ALTER TABLE Units ADD COLUMN 'HighSeaRaider' BOOLEAN DEFAULT 0;

-- Units gains this promotion when its Unit Entry is a HighSeaRaider
ALTER TABLE UnitPromotions ADD COLUMN 'HighSeaRaider' BOOLEAN DEFAULT 0;

-- Unit gets a CS modifier if not adjacent to any Friendly Unit
ALTER TABLE UnitPromotions ADD COLUMN 'NoAdjacentUnitMod' INTEGER DEFAULT 0;

-- Zulu Pikeman Class Upgrade immediately to Impi's upon discovering Impi tech, assuming Impi is not a Pikeman, but a Renaissance Unit.
ALTER TABLE Traits ADD COLUMN 'FreeZuluPikemanToImpi' BOOLEAN DEFAULT 0;

-- Improvement grants new Ownership if plot is not owned.
ALTER TABLE Improvements ADD COLUMN 'NewOwner' BOOLEAN DEFAULT 0;

-- Improvement grants promotion if plot is owned by the player.
ALTER TABLE Improvements ADD COLUMN 'OwnerOnly' BOOLEAN DEFAULT 1;

-- Missionaries gain % more strength
ALTER TABLE Traits ADD COLUMN 'ExtraMissionaryStrength' INTEGER DEFAULT 0;

-- Can send gold internal trade routes; gold yield calculated as if international, culture and science calculated as if allied city state. Only sender city gets the yields.
-- Note: Requires BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES in CustomModOptions to be set to 1 (0 by default for performance).
ALTER TABLE Traits ADD COLUMN 'CanGoldInternalTradeRoutes' BOOLEAN DEFAULT 0;

-- Additional trade routes based on the number of owned cities
ALTER TABLE Traits ADD COLUMN 'TradeRoutesPerXOwnedCities' INTEGER DEFAULT 0;

-- Additional trade routes based on the number of owned vassals
ALTER TABLE Traits ADD COLUMN 'TradeRoutesPerXOwnedVassals' INTEGER DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD COLUMN 'ExtraMissionaryStrength' INTEGER DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD COLUMN 'ExtraMissionarySpreads' INTEGER DEFAULT 0;

-- If the player has this building, they ignore the global Defensive Pact limit
ALTER TABLE Buildings ADD COLUMN 'IgnoreDefensivePactLimit' BOOLEAN DEFAULT 0;

-- Gifting units to City-States awards Influence per turn while the units remain alive
ALTER TABLE Traits ADD COLUMN 'MinorInfluencePerGiftedUnit' INTEGER DEFAULT 0;

-- Unit can't attack while in ocean tiles
ALTER TABLE UnitPromotions ADD COLUMN 'NoAttackInOcean' BOOLEAN DEFAULT 0;

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

ALTER TABLE Buildings ADD COLUMN 'FaithToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'CapitalsToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'DoFToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'RAToVotes' integer default 0;

ALTER TABLE Buildings ADD COLUMN 'GPExpendInfluence' integer default 0;

ALTER TABLE Units ADD COLUMN 'NumInfPerEra' integer default 0;
ALTER TABLE Units ADD COLUMN 'RestingPointChange' integer default 0;
ALTER TABLE Units ADD COLUMN 'ProductionCostAddedPerEra' integer default 0;

ALTER TABLE Policies ADD COLUMN 'GreatDiplomatRateModifier' integer default 0;

ALTER TABLE Improvements ADD COLUMN 'ImprovementLeagueVotes' integer default 0;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'BuildingTrigger' text default NULL;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'TriggerResolution' text default NULL;

ALTER TABLE Improvements ADD COLUMN 'IsEmbassy' boolean default 0;

-- C4DF

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

-- Whoward Tables
ALTER TABLE Traits ADD GGFromBarbarians INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD AuraRangeChange INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD AuraEffectChange INTEGER DEFAULT 0;

ALTER TABLE Traits ADD ExtraSupply INTEGER DEFAULT 0;
ALTER TABLE Traits ADD ExtraSupplyPerCity INTEGER DEFAULT 0;
ALTER TABLE Traits ADD ExtraSupplyPerPopulation INTEGER DEFAULT 0;

-- De-hardcode New Era Popup splash image with a new column and improved LUA file
-- Add the new column for new era popup splash image	
ALTER TABLE Eras ADD EraSplashImage TEXT DEFAULT 'ERA_Medievel.dds';

-- Update the base eras with the correct values
-- Yes, Medieval and Renaissance are correct values, blame the typo to Firaxis
UPDATE Eras SET EraSplashImage = 'ERA_Classical.dds'  		WHERE Type = 'ERA_CLASSICAL';
UPDATE Eras SET EraSplashImage = 'ERA_Medievel.dds'   		WHERE Type = 'ERA_MEDIEVAL';
UPDATE Eras SET EraSplashImage = 'ERA_Renissance.dds' 		WHERE Type = 'ERA_RENAISSANCE';
UPDATE Eras SET EraSplashImage = 'ERA_Industrial.dds' 		WHERE Type = 'ERA_INDUSTRIAL';
UPDATE Eras SET EraSplashImage = 'ERA_Modern.dds'     		WHERE Type = 'ERA_MODERN';
UPDATE Eras SET EraSplashImage = 'ERA_Atomic.dds'     		WHERE Type = 'ERA_POSTMODERN';
UPDATE Eras SET EraSplashImage = 'ERA_Future.dds'     		WHERE Type = 'ERA_FUTURE';
