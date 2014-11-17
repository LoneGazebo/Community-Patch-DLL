-- Table for Growth Extra Yield Buildings
CREATE TABLE IF NOT EXISTS Building_GrowthExtraYield (
BuildingType text, YieldType text, Yield integer
);

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

-- % boosts to city yield for happiness sources (traits) - Values should be positive to be good!
ALTER TABLE Traits ADD COLUMN 'PovertyHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'DefenseHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'IlliteracyHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'UnculturedHappinessTraitMod' integer default 0;
ALTER TABLE Traits ADD COLUMN 'MinorityHappinessTraitMod' integer default 0;

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

-- Citizen movement Rate
ALTER TABLE Policies ADD COLUMN 'ExtraMoves' integer default 0;

-- Religious Pressure Mod Trade Route
ALTER TABLE Policies ADD COLUMN 'TradeReligionModifier' integer default 0;

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

-- New Goody Hut Additions
ALTER TABLE GoodyHuts ADD COLUMN 'Production' integer default 0;
ALTER TABLE GoodyHuts ADD COLUMN 'GoldenAge' integer default 0;