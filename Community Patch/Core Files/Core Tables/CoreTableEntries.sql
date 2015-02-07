-- Table for Growth Extra Yield Buildings
CREATE TABLE IF NOT EXISTS Building_GrowthExtraYield (
BuildingType text, YieldType text, Yield integer
);

-- Create Era Value for Great Works creators that makes them spawn in the appropriate era.
ALTER TABLE Unit_UniqueNames ADD COLUMN 'EraType' text default NULL;

-- Create buildings that cannot have access to fresh water.

ALTER TABLE Buildings ADD COLUMN 'IsNoWater' boolean default false;

-- Create buildings that must be in the Capital

ALTER TABLE Buildings ADD COLUMN 'CapitalOnly' boolean default false;

-- Belief requires an improvement on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresImprovement' boolean default false;

-- Belief requires a resource (improved or not) on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresResource' boolean default false;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE Eras ADD COLUMN 'StartingMinorDefenseUnits' integer default 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE HandicapInfos ADD COLUMN 'StartingMinorDefenseUnits' integer default 0;

-- Grants additional starting happiness based on gamespeed.

ALTER TABLE GameSpeeds ADD COLUMN 'StartingHappiness' integer default 0;

-- No unhappiness from Isolation.

ALTER TABLE Traits ADD COLUMN 'NoConnectionUnhappiness' boolean default false;

-- No unhappiness from religious strife.

ALTER TABLE Traits ADD COLUMN 'IsNoReligiousStrife' boolean default false;

-- Earn a free building only in your capital as your trait. No tech requirement.

ALTER TABLE Traits ADD COLUMN 'FreeCapitalBuilding' text default NULL;

-- Combat Bonus vs Higher Pop Civilization.

ALTER TABLE Traits ADD COLUMN 'CombatBonusVsHigherPop' integer default 0;

-- Earn a set number of free buildings. Uses standard 'FreeBuilding' trait (i.e. Carthage). No tech requirement.

ALTER TABLE Traits ADD COLUMN 'NumFreeBuildings' integer default 0;

-- Adds a tech requirement to the free buildings.

ALTER TABLE Traits ADD COLUMN 'FreeBuildingPrereqTech' text default NULL;

-- Adds a tech requirement to the free capital building.

ALTER TABLE Traits ADD COLUMN 'CapitalFreeBuildingPrereqTech' text default NULL;

-- Adds a trait that lets a player conquer a city-state if they can bully it.

ALTER TABLE Traits ADD COLUMN 'BullyAnnex' boolean default false;

-- Grants a free valid promotion to a unit when it enters a type of feature (forest, marsh, etc.).

ALTER TABLE Features ADD COLUMN 'LocationUnitFreePromotion' text default NULL;

-- Grants a free valid promotion to a unit when it spawns on type of feature (forest, marsh, etc.). Generally used for barbarians.

ALTER TABLE Features ADD COLUMN 'SpawnLocationUnitFreePromotion' text default NULL;

-- Feature-promotion bonuses restricted to Barbarians. Can be used with CityStateOnly if desired.

ALTER TABLE Features ADD COLUMN 'IsBarbarianOnly' boolean default false;

-- Feature-promotion bonuses restricted to City States. Can be used with IsBarbarianOnly if desired.

ALTER TABLE Features ADD COLUMN 'IsCityStateOnly' boolean default false;

-- Grants a free valid promotion to a unit when it enters a type of terrain (grassland, plains, coast, etc.).

ALTER TABLE Terrains ADD COLUMN 'LocationUnitFreePromotion' text default NULL;

-- Grants a free valid promotion to a unit when it spawns on type of terrain (grassland, plains, coast, etc.). Generally used for barbarians.

ALTER TABLE Terrains ADD COLUMN 'SpawnLocationUnitFreePromotion' text default NULL;

-- Grants a free valid promotion to a unit when it is adjacent to a type of terrain (grassland, plains, coast, etc.). Generally used for barbarians.

ALTER TABLE Terrains ADD COLUMN 'AdjacentUnitFreePromotion' text default NULL;

-- Terrain-promotion bonuses restricted to Barbarians. Can be used with CityStateOnly if desired.

ALTER TABLE Terrains ADD COLUMN 'IsBarbarianOnly' boolean default false;

-- Terrain-promotion bonuses restricted to City States. Can be used with IsBarbarianOnly if desired.

ALTER TABLE Terrains ADD COLUMN 'IsCityStateOnly' boolean default false;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundMid' boolean default false;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD COLUMN 'FoundLate' boolean default false;

-- Adds minimum national population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'NationalPopRequired' integer default 0;

-- Adds minimum local population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'LocalPopRequired' integer default 0;

-- Reduces specialist unhappiness in cities by a set amount, either in capital or in all cities.
ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialists' integer default 0;
ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialistsCapital' integer default 0;

-- Flat boosts to city yield for happiness sources (buildings) - values should be positive to be good!
ALTER TABLE Buildings ADD COLUMN 'PovertyHappinessChange' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'DefenseHappinessChange' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyHappinessChange' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'UnculturedHappinessChange' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'MinorityHappinessChange' integer default 0;

-- Flat global boosts to city yield for happiness sources (buildings) - values should be positive to be good!
ALTER TABLE Buildings ADD COLUMN 'PovertyHappinessChangeGlobal' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'DefenseHappinessChangeGlobal' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyHappinessChangeGlobal' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'UnculturedHappinessChangeGlobal' integer default 0;
ALTER TABLE Buildings ADD COLUMN 'MinorityHappinessChangeGlobal' integer default 0;

-- % reduction to city threshold for happiness sources (policies) - Values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'PovertyHappinessMod' integer default 0;
ALTER TABLE Policies ADD COLUMN 'DefenseHappinessMod' integer default 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyHappinessMod' integer default 0;
ALTER TABLE Policies ADD COLUMN 'UnculturedHappinessMod' integer default 0;
ALTER TABLE Policies ADD COLUMN 'MinorityHappinessMod' integer default 0;

-- % reduction to city threshold for happiness sources in Capital (policies) - Values should be negative to be good!
ALTER TABLE Policies ADD COLUMN 'PovertyHappinessModCapital' integer default 0;
ALTER TABLE Policies ADD COLUMN 'DefenseHappinessModCapital' integer default 0;
ALTER TABLE Policies ADD COLUMN 'IlliteracyHappinessModCapital' integer default 0;
ALTER TABLE Policies ADD COLUMN 'UnculturedHappinessModCapital' integer default 0;
ALTER TABLE Policies ADD COLUMN 'MinorityHappinessModCapital' integer default 0;

-- Reduce the Unhappiness Threshold for Puppeted Cities. Value should be negative to be good.

ALTER TABLE Policies ADD COLUMN 'PuppetUnhappinessModPolicy' integer default 0;

-- Puppets and/or Occupied cities receive a % production modifier. Values should be positive to be good.
ALTER TABLE Policies ADD COLUMN 'PuppetProdMod' integer default 0;
ALTER TABLE Policies ADD COLUMN 'OccupiedProdMod' integer default 0;

-- Global Happiness Based on # of Citizens in Empire

ALTER TABLE Policies ADD COLUMN 'HappinessPerXPopulationGlobal' integer default 0;

-- % boosts to city yield for happiness sources (traits) - Values should be positive to be good!
ALTER TABLE Traits ADD COLUMN 'PovertyHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'DefenseHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'IlliteracyHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'UnculturedHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'MinorityHappinessTraitMod' integer default 0;

-- New Traits - Free Great Work(s) on Conquest (or Culture Boost if no great works/slots)

ALTER TABLE Traits ADD COLUMN 'FreeGreatWorkOnConquest' boolean default false;

-- New Traits - Religious Pressure modified based on population

ALTER TABLE Traits ADD COLUMN 'PopulationBoostReligion' boolean default false;

-- New Traits - Additioanl WC vote for every x CS Alliances
ALTER TABLE Traits ADD COLUMN 'VotePerXCSAlliance' integer default 0;

-- New Traits - Golden Age received from favorable peace treaty
ALTER TABLE Traits ADD COLUMN 'GoldenAgeFromVictory' integer default 0;

-- New Traits - Can buy owned plots with gold
ALTER TABLE Traits ADD COLUMN 'BuyOwnedTiles' boolean default false;

-- New Policies

-- Reduces unhappiness in occupied cities w/ Garrison. Negative = reduction.
ALTER TABLE Policies ADD COLUMN 'GarrisonsOccupiedUnhapppinessMod' integer default 0;

-- Spawns a free ranged unit with a new city.
ALTER TABLE Policies ADD COLUMN 'BestRangedUnitSpawnSettle' integer default 0;

-- No Unhappiness from Expansion
ALTER TABLE Policies ADD COLUMN 'NoUnhappinessExpansion' boolean default false;

-- No Unhapppiness from Isolation
ALTER TABLE Policies ADD COLUMN 'NoUnhappyIsolation' boolean default false;

-- Double City Border Growth During GA
ALTER TABLE Policies ADD COLUMN 'DoubleBorderGA' boolean default false;

-- Free Population
ALTER TABLE Policies ADD COLUMN 'FreePopulation' integer default 0;

-- Extra Votes
ALTER TABLE Policies ADD COLUMN 'ExtraMoves' integer default 0;

-- Religious Pressure Mod Trade Route
ALTER TABLE Policies ADD COLUMN 'TradeReligionModifier' integer default 0;

-- Increased Quest Influence
ALTER TABLE Policies ADD COLUMN 'IncreasedQuestInfluence' boolean default false;

-- Free Votes in WC
ALTER TABLE Policies ADD COLUMN 'FreeWCVotes' integer default 0;

-- GP Expend Influence Boost
ALTER TABLE Policies ADD COLUMN 'InfluenceGPExpend' integer default 0;

-- Free Trade Route
ALTER TABLE Policies ADD COLUMN 'FreeTradeRoute' integer default 0;

-- Free Spy
ALTER TABLE Policies ADD COLUMN 'FreeSpy' integer default 0;

-- Gold from Internal Trade Routes
ALTER TABLE Policies ADD COLUMN 'InternalTradeGold' integer default 0;

-- Boost Culture Bomb from Citadel
ALTER TABLE Policies ADD COLUMN 'CitadelBoost' integer default 0;

-- Unlock Era for Policy (Unlocks later eras earlier than normal)
ALTER TABLE Policies ADD COLUMN 'UnlocksPolicyBranchEra' text default NULL;

-- Points towards ideologies
ALTER TABLE Policies ADD COLUMN 'IdeologyPoint' integer default 0;

-- New Beliefs

-- Combat bonus v. other religions in our lands
ALTER TABLE Beliefs ADD COLUMN 'CombatVersusOtherReligionOwnLands' integer default 0;

-- Combat bonus v. other religions in their lands
ALTER TABLE Beliefs ADD COLUMN 'CombatVersusOtherReligionTheirLands' integer default 0;

-- Influence with CS from missionary spread
ALTER TABLE Beliefs ADD COLUMN 'MissionaryInfluenceCS' integer default 0;

-- Happiness per known civ with a Pantheon
ALTER TABLE Beliefs ADD COLUMN 'HappinessPerPantheon' integer default 0;

-- Extra Votes from Belief
ALTER TABLE Beliefs ADD COLUMN 'ExtraVotes' integer default 0;

-- New Buildings

-- National Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'NationalFollowerPopRequired' integer default 0;

-- Global Religious Followers Needed for a Building
ALTER TABLE Buildings ADD COLUMN 'GlobalFollowerPopRequired' integer default 0;

-- Allows for Reformation Policy
ALTER TABLE Buildings ADD COLUMN 'IsReformation' boolean default false;

-- Allows for Building to be unlocked by a specific policy (not a branch)
ALTER TABLE Buildings ADD COLUMN 'PolicyType' text default NULL;

-- Allows for Building to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Buildings ADD COLUMN 'ResourceType' text default NULL;

-- Allows for Unit to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Units ADD COLUMN 'ResourceType' text default NULL;

-- Allows for Building to be purchased in puppet city
ALTER TABLE Buildings ADD COLUMN 'PuppetPurchaseOverride' boolean default false;

-- Allows for All Units/Buildings to be purchased in puppet city
ALTER TABLE Buildings ADD COLUMN 'AllowsPuppetPurchase' boolean default false;

-- Creates a resource unique to this civ (i.e. Indonesian Candi) in the territory around the city. To make this work with a civ, you'll need to create a new resource modelled on the Indonesian resources and assign them to the civ. Value is indicative of the number of resources that will be granted.
ALTER TABLE Buildings ADD COLUMN 'GrantsRandomResourceTerritory' integer default 0;

-- Allows you to define a building needed by this building (similar to BuildingClassNeeded) -->
ALTER TABLE Buildings ADD COLUMN 'NeedBuildingThisCity' text default NULL;

-- Allows for Unit to be purchased in puppet city
ALTER TABLE Units ADD COLUMN 'PuppetPurchaseOverride' boolean default false;

-- Grants resource to improvement
ALTER TABLE Improvements ADD COLUMN 'ImprovementResource' text default NULL;

-- Grants obsoletion tech to build (tie to improvement below for function)
ALTER TABLE Builds ADD COLUMN 'ObsoleteTech' text default NULL;

-- Grants obsoletion tech to improvement (tie to build above for AI)
ALTER TABLE Improvements ADD COLUMN 'ObsoleteTech' text default NULL;

-- Improvements can be made valid by being adjacent to a lake
ALTER TABLE Improvements ADD COLUMN 'Lakeside' boolean default false;

-- New Goody Hut Additions
ALTER TABLE GoodyHuts ADD COLUMN 'Production' integer default 0;
ALTER TABLE GoodyHuts ADD COLUMN 'GoldenAge' integer default 0;

-- Tech Additions
ALTER TABLE Technologies ADD COLUMN 'CityLessEmbarkCost' boolean;
ALTER TABLE Technologies ADD COLUMN 'CityNoEmbarkCost' boolean;

-- Promotions

ALTER TABLE UnitPromotions ADD 'ReconChange' integer default 0;

ALTER TABLE UnitPromotions ADD 'GainsXPFromScouting' boolean default false;

-- Unit stuff for minor civs
ALTER TABLE Units ADD COLUMN 'MinorCivGift' boolean default false;
