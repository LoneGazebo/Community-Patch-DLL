-- Table for Growth Extra Yield Buildings
CREATE TABLE IF NOT EXISTS Building_GrowthExtraYield (
BuildingType TEXT, YieldType TEXT, Yield INTEGER
);


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

-- Create buildings that must be in the Capital

ALTER TABLE Buildings ADD COLUMN 'CapitalOnly' BOOLEAN DEFAULT 0;

-- Requirement that allows you to bypass prereq techs to get something.
ALTER TABLE Buildings ADD COLUMN 'NumPoliciesNeeded' BOOLEAN DEFAULT 0;

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


-- Policy - reduces policy cost of Wonders by 1 for every x CS allies
ALTER TABLE Policies ADD COLUMN 'XCSAlliesLowersPolicyNeedWonders' INTEGER DEFAULT 0;

-- Policy - reduce speed of people stealing Great Works from you.
ALTER TABLE Policies ADD COLUMN 'StealGWSlowerModifier' INTEGER DEFAULT 0;
-- Policy - increase speed of your Great Work theft rate.
ALTER TABLE Policies ADD COLUMN 'StealGWFasterModifier' INTEGER DEFAULT 0;

-- Policy Branch - number of unlocked policies (finishers excluded) before branch is unlocked.
ALTER TABLE PolicyBranchTypes ADD COLUMN 'NumPolicyRequirement' INTEGER DEFAULT 100;

-- Adds ability to turn production into defense/healing in a city for a process
ALTER TABLE Processes ADD COLUMN 'DefenseValue' INTEGER DEFAULT 0;

-- Belief - increases pressure from trade routes

ALTER TABLE Beliefs ADD COLUMN 'PressureChangeTradeRoute' INTEGER DEFAULT 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE Eras ADD COLUMN 'StartingMinorDefenseUnits' INTEGER DEFAULT 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE HandicapInfos ADD COLUMN 'StartingMinorDefenseUnits' INTEGER DEFAULT 0;

-- CBP Difficulty Bonus
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusBase' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusA' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusB' INTEGER DEFAULT 0;
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonusC' INTEGER DEFAULT 0;

-- Grants additional starting happiness based on gamespeed.

ALTER TABLE GameSpeeds ADD COLUMN 'StartingHappiness' INTEGER DEFAULT 0;

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

-- Abnormal scaler for Specialist yields in cities UA. +specialist yield in medieval/industrial/atomic eras.

ALTER TABLE Traits ADD COLUMN 'IsOddEraScaler' BOOLEAN DEFAULT 0;

-- No natural religion spread to/from unowned cities

ALTER TABLE Traits ADD COLUMN 'NoNaturalReligionSpread' BOOLEAN DEFAULT 0;

-- No trade routes to player if not already trading w/ you, tourism penalty if not trading (-50%)

ALTER TABLE Traits ADD COLUMN 'NoOpenTrade' BOOLEAN DEFAULT 0;

-- Earn a free building only in your capital as your trait. No tech requirement.

ALTER TABLE Traits ADD COLUMN 'FreeCapitalBuilding' TEXT DEFAULT NULL;

-- Combat Bonus vs Higher Pop Civilization.

ALTER TABLE Traits ADD COLUMN 'CombatBonusVsHigherPop' INTEGER DEFAULT 0;

-- Earn a set number of free buildings. Uses standard 'FreeBuilding' trait (i.e. Carthage). No tech requirement.

ALTER TABLE Traits ADD COLUMN 'NumFreeBuildings' INTEGER DEFAULT 0;

-- Adds a tech requirement to the free buildings.

ALTER TABLE Traits ADD COLUMN 'FreeBuildingPrereqTech' TEXT DEFAULT NULL;

-- Adds a tech requirement to the free capital building.

ALTER TABLE Traits ADD COLUMN 'CapitalFreeBuildingPrereqTech' TEXT DEFAULT NULL;

-- Adds a trait that lets a player conquer a city-state if they can bully it.

ALTER TABLE Traits ADD COLUMN 'BullyAnnex' BOOLEAN DEFAULT 0;

-- Adds a trait that makes CS alliances boost the defense of CS cities.

ALTER TABLE Traits ADD COLUMN 'AllianceCSDefense' INTEGER DEFAULT 0;

-- Adds a trait that makes CS alliances boost the strength of CS units.

ALTER TABLE Traits ADD COLUMN 'AllianceCSStrength' INTEGER DEFAULT 0;

-- Adds a trait that converts x% of tourism to GAP, where x is the integer below.

ALTER TABLE Traits ADD COLUMN 'TourismToGAP' INTEGER DEFAULT 0;
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

-- Puppet negative modifiers no longer apply
ALTER TABLE Traits ADD COLUMN 'IgnorePuppetPenalties' BOOLEAN DEFAULT 0;

-- Player gains a free policy after unlocking x number of technologies from the tech tree.
ALTER TABLE Traits ADD COLUMN 'FreePolicyPerXTechs' INTEGER default 0;

-- Does this Civ get extra damage from multiple attacks on same target?
ALTER TABLE Traits ADD COLUMN 'MultipleAttackBonus' INTEGER DEFAULT 0;

-- Does this Civ get extra influence from meeting a CS?
ALTER TABLE Traits ADD COLUMN 'InfluenceMeetCS' INTEGER DEFAULT 0;


-- Grants a free valid promotion to a unit when it is on a type of improvement (farm, mine, etc.).

ALTER TABLE Improvements ADD COLUMN 'UnitFreePromotion' TEXT DEFAULT NULL;

-- Grants Free Experience when Unit is On Improvement plot (must be owned) on Do Turn.
ALTER TABLE Improvements ADD COLUMN 'UnitPlotExperience' INTEGER DEFAULT 0;

-- Grants Free Experience when Unit is On Improvement plot during Golden Ages (must be owned) on Do Turn.
ALTER TABLE Improvements ADD COLUMN 'GAUnitPlotExperience' INTEGER DEFAULT 0;

-- Activates the above two, UnitPlotExperience and GAUnitPlotExperience.
ALTER TABLE Improvements ADD COLUMN 'IsExperience' BOOLEAN DEFAULT 0;

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

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundMid' BOOLEAN DEFAULT 0;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundLate' BOOLEAN DEFAULT 0;

-- Adds marker for city attack only units (for AI)
ALTER TABLE Units ADD COLUMN 'CityAttackOnly' BOOLEAN DEFAULT 0;

-- Adds Culture from experience to owner of unit when disbanded or upgraded
ALTER TABLE Units ADD COLUMN 'CulExpOnDisbandUpgrade' BOOLEAN DEFAULT 0;

-- Adds minimum national population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'NationalPopRequired' INTEGER DEFAULT 0;

-- Adds minimum local population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'LocalPopRequired' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for land plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleCity' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for water plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleWater' INTEGER DEFAULT 0;

-- One building gives all cities this ability
ALTER TABLE Buildings ADD COLUMN 'AllowsFoodTradeRoutesGlobal' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'AllowsProductionTradeRoutesGlobal' BOOLEAN DEFAULT 0;

-- Local city connection strong
ALTER TABLE Buildings ADD COLUMN 'CityConnectionGoldModifier' INTEGER DEFAULT 0;

-- Adds abiility for units to upgrade in allied CS lands.
ALTER TABLE Policies ADD COLUMN 'UpgradeCSTerritory' BOOLEAN DEFAULT 0;

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

-- CS resources count towards monopolies
ALTER TABLE Policies ADD COLUMN 'CSResourcesCountForMonopolies' BOOLEAN DEFAULT 0;

-- % modifiers to city unhappiness sources - values should be negative to be good!
ALTER TABLE Buildings ADD COLUMN 'PovertyHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'DefenseHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'UnculturedHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'MinorityHappinessChange' INTEGER DEFAULT 0;

-- % global modifiers to city unhappiness sources - values should be negative to be good!
ALTER TABLE Buildings ADD COLUMN 'PovertyHappinessChangeGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'DefenseHappinessChangeGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyHappinessChangeGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'UnculturedHappinessChangeGlobal' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'MinorityHappinessChangeGlobal' INTEGER DEFAULT 0;

-- % reduction to city threshold for happiness sources (policies) - Values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'PovertyHappinessMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'DefenseHappinessMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyHappinessMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'UnculturedHappinessMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'MinorityHappinessMod' INTEGER DEFAULT 0;

-- % reduction to city threshold for happiness sources in Capital (policies) - Values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'PovertyHappinessModCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'DefenseHappinessModCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyHappinessModCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'UnculturedHappinessModCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'MinorityHappinessModCapital' INTEGER DEFAULT 0;

-- Reduce the Unhappiness Threshold for Puppeted Cities. Value should be negative to be good.

ALTER TABLE Policies ADD COLUMN 'PuppetUnhappinessModPolicy' INTEGER DEFAULT 0;

-- Puppets and/or Occupied cities receive a % production modifier. Values should be positive to be good.
ALTER TABLE Policies ADD COLUMN 'PuppetProdMod' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'OccupiedProdMod' INTEGER DEFAULT 0;

-- Increases City Defense - needs to be in 100s value
ALTER TABLE Policies ADD COLUMN 'DefenseBoostAllCities' INTEGER DEFAULT 0;

-- Global Happiness Based on # of Citizens in Empire

ALTER TABLE Policies ADD COLUMN 'HappinessPerXPopulationGlobal' INTEGER DEFAULT 0;

-- War Weariness Mod - Positive values make it harder to accumulate war weariness.
ALTER TABLE Policies ADD COLUMN 'WarWearinessModifier' INTEGER DEFAULT 0;

-- GG Mod - boosts strength modifier of GGs
ALTER TABLE Policies ADD COLUMN 'GreatGeneralExtraBonus' INTEGER DEFAULT 0;

-- Shift Influence with CS for all by % +/- when you bully
ALTER TABLE Policies ADD COLUMN 'BullyGlobalCSInfluenceShift' INTEGER DEFAULT 0;

-- More yields from Vassals and CSs
ALTER TABLE Policies ADD COLUMN 'VassalCSBonusModifier' INTEGER DEFAULT 0;

-- Can bully friendly CSs (no penalty)
ALTER TABLE Policies ADD COLUMN 'CanBullyFriendlyCS' BOOLEAN DEFAULT 0;

-- CS influence does not decline at war
ALTER TABLE Policies ADD COLUMN 'NoAlliedCSInfluenceDecayAtWar' BOOLEAN DEFAULT 0;

-- Vassals don't rebel and can't be forced out by deals or WC
ALTER TABLE Policies ADD COLUMN 'VassalsNoRebel' BOOLEAN DEFAULT 0;

-- % boosts to city yield for happiness sources (traits) - Values should be negative to be good!
ALTER TABLE Traits ADD COLUMN 'PovertyHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'DefenseHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'IlliteracyHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'UnculturedHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'MinorityHappinessTraitMod' INTEGER DEFAULT 0;

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
ALTER TABLE Policies ADD COLUMN 'GarrisonsOccupiedUnhapppinessMod' INTEGER DEFAULT 0;

-- Spawns a free ranged unit with a new city.
ALTER TABLE Policies ADD COLUMN 'BestRangedUnitSpawnSettle' INTEGER DEFAULT 0;

-- No Unhappiness from Expansion
ALTER TABLE Policies ADD COLUMN 'NoUnhappinessExpansion' BOOLEAN DEFAULT 0;

-- No Unhapppiness from Isolation
ALTER TABLE Policies ADD COLUMN 'NoUnhappyIsolation' BOOLEAN DEFAULT 0;

-- Double City Border Growth During GA
ALTER TABLE Policies ADD COLUMN 'DoubleBorderGA' BOOLEAN DEFAULT 0;

-- Free Population
ALTER TABLE Policies ADD COLUMN 'FreePopulation' INTEGER DEFAULT 0;

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
ALTER TABLE Policies ADD COLUMN 'CitadelBoost' INTEGER DEFAULT 0;

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

-- Influence with CS from missionary spread
ALTER TABLE Beliefs ADD COLUMN 'MissionaryInfluenceCS' INTEGER DEFAULT 0;

-- Happiness per known civ with a Pantheon
ALTER TABLE Beliefs ADD COLUMN 'HappinessPerPantheon' INTEGER DEFAULT 0;

-- Extra Votes from Belief
ALTER TABLE Beliefs ADD COLUMN 'ExtraVotes' INTEGER DEFAULT 0;

-- Ignore Policy Requirements (number) for wonders up to a set era
ALTER TABLE Beliefs ADD COLUMN 'IgnorePolicyRequirementsUpToEra' BOOLEAN DEFAULT 0;

-- Increase yields from friendship/alliance for CS sharing your religion by x%
ALTER TABLE Beliefs ADD COLUMN 'CSYieldBonusFromSharedReligion' INTEGER DEFAULT 0;


-- New Buildings

-- Building Modifies amount of unhappiness provided by an individual city
ALTER TABLE Buildings ADD COLUMN 'LocalUnhappinessModifier' INTEGER DEFAULT 0;

-- Building Modifies amount of maintenance buildings cost in your empire.
ALTER TABLE Buildings ADD COLUMN 'GlobalBuildingGoldMaintenanceMod' INTEGER DEFAULT 0;

-- National Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'NationalFollowerPopRequired' INTEGER DEFAULT 0;

-- Global Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'GlobalFollowerPopRequired' INTEGER DEFAULT 0;

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

-- Quantity of Resources to place in your Territory. Must be used with 'Building_ResourcePlotsToPlace'. Works like "GrantsRandomResourceTerritory", but any resource in table Resources can be used.
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

-- Allows you to set a % of warscore that is added to a tourism bonus against a civ
ALTER TABLE Policies ADD COLUMN 'PositiveWarScoreTourismMod' INTEGER DEFAULT 0;

-- Allows for Unit to be purchased in puppet city
ALTER TABLE Units ADD COLUMN 'PuppetPurchaseOverride' BOOLEAN DEFAULT 0;

-- Allows for Unit to gain more yields and experience from a ruin
ALTER TABLE Units ADD COLUMN 'GoodyModifier' INTEGER DEFAULT 0;
-- Allows for Unit to increase your supply cap.
ALTER TABLE Units ADD COLUMN 'SupplyCapBoost' INTEGER DEFAULT 0;

-- Grants resource to improvement
ALTER TABLE Improvements ADD COLUMN 'ImprovementResource' TEXT DEFAULT NULL;
ALTER TABLE Improvements ADD COLUMN 'ImprovementResourceQuantity' INTEGER DEFAULT 0;

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

-- Improvements can generate vision for builder x tiles away (radially)
ALTER TABLE Improvements ADD COLUMN 'GrantsVisionXTiles' INTEGER DEFAULT 0;

-- New Goody Hut Additions
ALTER TABLE GoodyHuts ADD COLUMN 'Production' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'GoldenAge' INTEGER DEFAULT 0;
ALTER TABLE GoodyHuts ADD COLUMN 'FreeTiles' INTEGER DEFAULT 0;

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

ALTER TABLE UnitPromotions ADD 'AttackFullyHealedMod' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'AttackAbove50HealthMod' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'AttackBelowEqual50HealthMod' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'SplashDamage' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'AOEDamageOnKill' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD 'AoEWhileFortified' INTEGER DEFAULT 0;

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

-- A unit gains a combat bonus VS barbarians
ALTER TABLE UnitPromotions ADD COLUMN 'BarbarianCombatBonus' INTEGER DEFAULT 0;

-- Ranged units have a % chance to force another unit to retreat, taking additional damage if they cannot do so.
ALTER TABLE UnitPromotions ADD COLUMN 'MoraleBreakChance' INTEGER DEFAULT 0;

-- Promotion bonuses restricted to Barbarians.

ALTER TABLE UnitPromotions ADD COLUMN 'BarbarianOnly' BOOLEAN DEFAULT 0;

-- Promotion bonuses restricted to City States.

ALTER TABLE UnitPromotions ADD COLUMN 'CityStateOnly' BOOLEAN DEFAULT 0;

-- Promotion grants the same bonus as the Japan UA
ALTER TABLE UnitPromotions ADD COLUMN 'StrongerDamaged' BOOLEAN DEFAULT 0;

-- Double Movement on Mountains
ALTER TABLE UnitPromotions ADD COLUMN 'MountainsDoubleMove' BOOLEAN DEFAULT 0;

-- Double Heal in Feature/Terrain
ALTER TABLE UnitPromotions_Features ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions_Terrains ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;

-- Combat Modifier for determined range near a defined UnitClass
ALTER TABLE UnitPromotions ADD CombatBonusFromNearbyUnitClass INTEGER DEFAULT -1;
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonusRange INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonus INTEGER DEFAULT 0;

-- Requres some Explanation: Unit A has X promotion, Unit B gains promotion by adding these Values to a Promotion that it wishes to gain when near some distance
-- AddedFromNearbyPromotion = 'Unit A's promotion' IsNearbyPromotion must be set to 1 or true, and NearbyRange is distance in which the promotion triggers.
ALTER TABLE UnitPromotions ADD AddedFromNearbyPromotion INTEGER DEFAULT -1;
ALTER TABLE UnitPromotions ADD IsNearbyPromotion BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyRange INTEGER DEFAULT 0;

-- A unit gains a promotion if "NearbyRange" is set to a distance from City, RequiredUnit must be set to the unit that you wish to give the promotion to.
ALTER TABLE UnitPromotions ADD IsNearbyCityPromotion BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD IsNearbyFriendlyCityPromotion BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD IsNearbyEnemyCityPromotion BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD IsFriendlyLands BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions ADD RequiredUnit TEXT DEFAULT NULL REFERENCES Units(Type);

-- Changes the intercept range against air units (NEW)
ALTER TABLE UnitPromotions ADD AirInterceptRangeChange INTEGER DEFAULT 0;

-- Allows you to Convert a unit when it X plot is a different domain, e.g. A great general becomes a great admiral, etc.
ALTER TABLE UnitPromotions ADD ConvertDomainUnit TEXT DEFAULT NULL REFERENCES Units(Type);
ALTER TABLE UnitPromotions ADD ConvertDomain TEXT DEFAULT NULL REFERENCES Domains(Type);
ALTER TABLE Units ADD IsConvertUnit BOOLEAN DEFAULT 0;

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

-- Investment reduction costs -- trait -- negative makes it stronger!
ALTER TABLE Traits ADD COLUMN 'InvestmentModifier' INTEGER DEFAULT 0;

-- Cities produce % culture instead of growth during WLTKD.
ALTER TABLE Traits ADD COLUMN 'WLTKDCultureBoost' INTEGER DEFAULT 0;

-- Cities gain x turns of WLTKD because of a GA trigger!
ALTER TABLE Traits ADD COLUMN 'WLTKDFromGATurns' INTEGER DEFAULT 0;

-- Unhappienss modifier during GAs - more negative the better!
ALTER TABLE Traits ADD COLUMN 'GAUnhappinesNeedMod' INTEGER DEFAULT 0;

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

-- GWAM from conquest
ALTER TABLE Traits ADD COLUMN 'CityConquestGWAM' INTEGER DEFAULT 0;

-- Limits the amount that can be built of a Unit class per city
ALTER TABLE UnitClasses ADD COLUMN 'UnitInstancePerCity' INTEGER DEFAULT -1;

-- Trade Route Internal Capital Bonus -- policy -- Internal/INTL TR from Capital and Holy City stronger!
ALTER TABLE Policies ADD COLUMN 'InternalTradeRouteYieldModifierCapital' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TradeRouteYieldModifierCapital' INTEGER DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD COLUMN 'GreatEngineerRateModifier' INTEGER DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD COLUMN 'CityStateCombatModifier' INTEGER DEFAULT 0;

-- Trade Route Modifiers for Policies
ALTER TABLE Policies ADD COLUMN 'TradeRouteLandDistanceModifier' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TradeRouteSeaDistanceModifier' INTEGER DEFAULT 0;

--Espionage Modifier for Policies - should be negative for player benefit!
ALTER TABLE Policies ADD COLUMN 'EspionageModifier' INTEGER DEFAULT 0;

-- C4DF Function

ALTER TABLE Buildings ADD COLUMN 'VassalLevyEra' BOOLEAN DEFAULT 0;

-- Projects
ALTER TABLE Projects ADD COLUMN 'FreeBuildingClassIfFirst' TEXT DEFAULT NULL;
ALTER TABLE Projects ADD COLUMN 'FreePolicyIfFirst' TEXT DEFAULT NULL;

-- Advanced Action Spy Stuff (for CBP)

-- Spies cannot fail advanced actions -- they won't trigger unless they can do damage
ALTER TABLE Buildings ADD COLUMN 'CannotFailSpies' INTEGER DEFAULT 0;

-- Each point in these columns increases the chances of the event happening by 1
-- All possible events are pooled and then selected from at random - each point in these adds that value to the pool again
-- Are empire-wide values - affect all spies
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionGold' INTEGER DEFAULT 0; -- starts at 3 
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionScience' INTEGER DEFAULT 0; -- starts at 3 
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionUnrest' INTEGER DEFAULT 0; -- starts at 1 
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionRebellion' INTEGER DEFAULT 0; -- starts at 1
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionGP' INTEGER DEFAULT 0; -- starts at 1
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionUnit' INTEGER DEFAULT 0; -- starts at 2
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionWonder' INTEGER DEFAULT 0; -- starts at 1
ALTER TABLE Buildings ADD COLUMN 'AdvancedActionBuilding' INTEGER DEFAULT 0; -- starts at 2

-- Blocks advanced actions entirely in a city -- 1 enables
ALTER TABLE Buildings ADD COLUMN 'BlockBuildingDestructionSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockWWDestructionSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockUDestructionSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockGPDestructionSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockRebellionSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockUnrestSpies' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockScienceTheft' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'BlockGoldTheft' INTEGER DEFAULT 0;


ALTER TABLE Buildings ADD COLUMN 'EventChoiceRequiredActive' TEXT DEFAULT NULL;
ALTER TABLE Buildings ADD COLUMN 'CityEventChoiceRequiredActive' TEXT DEFAULT NULL;


-- Resources
ALTER TABLE Resources ADD COLUMN 'StrategicHelp' TEXT DEFAULT NULL;

ALTER TABLE Policies ADD COLUMN 'TRSpeedBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'TRVisionBoost' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXPolicies' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'HappinessPerXGreatWorks' INTEGER DEFAULT 0;

-- Trade Routes
ALTER TABLE Policies ADD COLUMN 'ExtraCultureandScienceTradeRoutes' INTEGER DEFAULT 0;

-- CORPORATIONS
ALTER TABLE Technologies ADD COLUMN 'CorporationsEnabled' BOOLEAN;

ALTER TABLE Buildings ADD COLUMN 'IsCorporation' BOOLEAN DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'GPRateModifierPerXFranchises' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'TRSpeedBoost' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'TRVisionBoost' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'OfficeBenefitHelper' TEXT DEFAULT NULL;
-- Corporation Policies
ALTER TABLE Policies ADD COLUMN 'CorporationOfficesAsFranchises' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationFreeFranchiseAbovePopular' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'CorporationRandomForeignFranchise' BOOLEAN DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'AdditionalNumFranchisesMod' INTEGER DEFAULT 0;				-- 20 = 20% additional, NOT 1/5 of existing value. this stacks, so 120%, 140%, 160%, etc...

-- Minor Civs
ALTER TABLE MinorCivilizations ADD COLUMN 'BullyUnitClass' TEXT DEFAULT NULL;

-- CSD
ALTER TABLE Buildings ADD COLUMN 'DPToVotes' integer default 0;

-- GameSpeed for JFD
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMin' INTEGER DEFAULT 0;
ALTER TABLE GameSpeeds ADD COLUMN 'PietyMax' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'SecondaryPantheon' BOOLEAN DEFAULT 0;

-- Plague Stuff for JFD
ALTER TABLE UnitPromotions ADD COLUMN 'PlagueChance' INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions ADD COLUMN 'IsPlague' BOOLEAN DEFAULT 0;

ALTER TABLE Buildings ADD COLUMN 'IsDummy' BOOLEAN DEFAULT 0;

-- HH Mod
ALTER TABLE Policies ADD COLUMN 'ExtraNaturalWonderHappiness' INTEGER DEFAULT 0;
--ALTER TABLE Policies ADD CONSTRAINT ck_ExtraNaturalWonderHappiness CHECK (ExtraNaturalWonderHappiness >= 0);

-- Worlds
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCities' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'MinDistanceCityStates' INTEGER DEFAULT 0;
ALTER TABLE Worlds ADD COLUMN 'ReformationPercentRequired' INTEGER DEFAULT 100;

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

-- Spawn Best Melee Unit on an Improvement during a DOW
ALTER TABLE Traits ADD COLUMN 'BestUnitSpawnOnImpDOW' BOOLEAN DEFAULT 0;
ALTER TABLE Traits ADD BestUnitImprovement TEXT DEFAULT NULL REFERENCES Improvements(Type);

-- Spawn best Melee Type Unit on finishing a Build (accounts for Domain of Build)
ALTER TABLE Builds ADD IsFreeBestDomainUnit BOOLEAN DEFAULT 0;

-- Unit Upgrades for free when reaching it's UnitClass Upgrade Tech
ALTER TABLE Units ADD FreeUpgrade BOOLEAN DEFAULT 0;

-- Unit gets a new Combat Strength in specific Era, must be set to 'true' or 1, if using Unit_EraCombatStrength, and Unit_EraCombatStrength must be filled in.
ALTER TABLE Units ADD UnitEraUpgrade BOOLEAN DEFAULT 0;

-- Grants a free building to a city when founded
ALTER TABLE Policies ADD NewCityFreeBuilding TEXT DEFAULT NULL REFERENCES BuildingClasses(Type);

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

-- Unit will convert to another UnitType. Must define a "DamageThreshold" and the "ConvertUnit" Type
ALTER TABLE Units ADD ConvertOnDamage BOOLEAN DEFAULT 0;

-- Unit will convert to another UnitType if "ConvertOnDamage" and "DamageThreshold" are defined.
ALTER TABLE Units ADD ConvertUnit TEXT DEFAULT NULL REFERENCES Units(Type);

-- Special Units that have a different Special rating can be modified here to load on to ships (e.g. Great People).
ALTER TABLE Units ADD SpecialUnitCargoLoad TEXT DEFAULT NULL REFERENCES SpecialUnits(Type);

-- Unit will convert to another UnitType. Must define a "ConvertOnDamage" and the "ConvertUnit" Type
ALTER TABLE Units ADD DamageThreshold INTEGER DEFAULT 0;

-- Unit will convert to the original UnitType when Max Hip Points are restored. Must define "ConvertUnit" Type to the original Unit.
ALTER TABLE Units ADD ConvertOnFullHP BOOLEAN DEFAULT 0;

-- Does this Civ get a GG/GA Rate Modifier bonus from denunciations and wars?
ALTER TABLE Traits ADD COLUMN 'GGGARateFromDenunciationsAndWars' INTEGER DEFAULT 0;

-- Does this civ get a free Unit.Type on Conquest? Must be able to train it first....
ALTER TABLE Traits ADD FreeUnitOnConquest TEXT DEFAULT NULL REFERENCES Units(Type);

-- Can this unit only be trained during War?
ALTER TABLE Units ADD WarOnly BOOLEAN DEFAULT 0;

-- Can this unit convert an enemy unit into a barbarian? Must set as well DamageThreshold to a value you want enemy to convert.
ALTER TABLE Units ADD ConvertEnemyUnitToBarbarian BOOLEAN DEFAULT 0;

-- Civ gets an influence boost and Great Admiral Points when sending a Trade Route to a minor Civ.
ALTER TABLE Traits ADD COLUMN 'TradeRouteMinorInfluenceAP' BOOLEAN DEFAULT 0;

-- Can this building be built next to any body of water?
ALTER TABLE Buildings ADD AnyWater BOOLEAN DEFAULT 0;

-- Promotion grants additional combat strength if on a pillaged improvement
ALTER TABLE UnitPromotions ADD COLUMN 'PillageBonusStrength' INTEGER DEFAULT 0;

-- Improvement can create a feature. Best implemented on Tiles that don't have features.
ALTER TABLE Improvements ADD COLUMN 'CreatesFeature' INTEGER DEFAULT 0;

-- Start a WLTKD when this unit is born or gained. GP's only.
ALTER TABLE Units ADD WLTKDFromBirth BOOLEAN DEFAULT 0;

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

-- Define a modifier for all tile/building based tourism in all cities.
ALTER TABLE Buildings ADD 'GlobalLandmarksTourismPercent' INTEGER DEFAULT 0;

-- Define a modifier for all great work tourism in all cities.
ALTER TABLE Buildings ADD 'GlobalGreatWorksTourismModifier' INTEGER DEFAULT 0;

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

-- Enemy Units gain the "EnemyLands" promotio when in your territory or Friendly City States or Player's that follow the same Ideology. Must define "EnemyLands" promotion type for this to work (see below).
ALTER TABLE Traits ADD COLUMN 'WarsawPact' BOOLEAN DEFAULT 0;

-- Units gain this promotion when "WarsawPact" Player is set.
ALTER TABLE UnitPromotions ADD COLUMN 'EnemyLands' BOOLEAN DEFAULT 0;

-- Unit gains this promotion when adjacent when adjacent to a unit and both units have this Prereq Promotion defined as AdjacentSameType = 'PROMOTION_X'
ALTER TABLE UnitPromotions ADD AdjacentSameType TEXT DEFAULT NULL REFERENCES UnitPromotions(Type);

-- Build adds an instant yield of culture to Player's culture pool.
ALTER TABLE Builds ADD CultureBoost BOOLEAN DEFAULT 0;

-- When a unit (civilian or combat) with this promotion is stationed in a City, City gains X% modifier towards building military units.
ALTER TABLE UnitPromotions ADD COLUMN 'MilitaryProductionModifier' INTEGER DEFAULT 0;

-- Unit gets the "HighSeaRaider" Promotion Entry (if defined) when it plunders a Trade Route.
ALTER TABLE Units ADD HighSeaRaider BOOLEAN DEFAULT 0;

-- Units gains this promotion when its Unit Entry is a HighSeaRaider
ALTER TABLE UnitPromotions ADD COLUMN 'HighSeaRaider' BOOLEAN DEFAULT 0;

-- Zulu Pikeman Class Upgrade immediately to Impi's upon discovering Impi tech, assuming Impi is not a Pikeman, but a Renaissance Unit.
ALTER TABLE Traits ADD COLUMN 'FreeZuluPikemanToImpi' BOOLEAN DEFAULT 0;

-- Improvement grants new Ownership if plot is not owned.
ALTER TABLE Improvements ADD COLUMN 'NewOwner' BOOLEAN DEFAULT 0;

-- Improvement grants promotion if plot is owned by the player.
ALTER TABLE Improvements ADD COLUMN 'OwnerOnly' BOOLEAN DEFAULT 1;

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

ALTER TABLE Policies ADD COLUMN 'GreatDiplomatRateModifier' integer default 0;

ALTER TABLE Improvements ADD COLUMN 'ImprovementLeagueVotes' integer default 0;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'BuildingTrigger' text default NULL;

ALTER TABLE LeagueSpecialSessions ADD COLUMN 'TriggerResolution' text default NULL;

ALTER TABLE Improvements ADD COLUMN 'IsEmbassy' boolean default 0;

-- C4DF

-- Insert SQL Rules Here 

ALTER TABLE GameSpeeds		ADD		ShareOpinionDuration			integer;									-- How long do we have to wait after Share Opinion rejection?
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

ALTER TABLE Traits
  ADD GGFromBarbarians INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('TRAITS_GG_FROM_BARBARIANS', 1);


ALTER TABLE UnitPromotions
  ADD AuraRangeChange INTEGER DEFAULT 0;
  
ALTER TABLE UnitPromotions
  ADD AuraEffectChange INTEGER DEFAULT 0;
  
INSERT OR REPLACE INTO Defines(Name, Value)
  VALUES('GREAT_GENERAL_MAX_RANGE', 2);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('PROMOTIONS_AURA_CHANGE', 1);

ALTER TABLE Traits
  ADD ExtraSupply INTEGER DEFAULT 0;

ALTER TABLE Traits
  ADD ExtraSupplyPerCity INTEGER DEFAULT 0;

ALTER TABLE Traits
  ADD ExtraSupplyPerPopulation INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('TRAITS_EXTRA_SUPPLY', 1);

