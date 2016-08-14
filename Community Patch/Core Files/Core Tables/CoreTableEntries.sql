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

-- Policy - reduces policy cost of Wonders by 1 for every x CS allies
ALTER TABLE Policies ADD COLUMN 'XCSAlliesLowersPolicyNeedWonders' INTEGER DEFAULT 0;

-- Policy Branch - number of unlocked policies (finishers excluded) before branch is unlocked.
ALTER TABLE PolicyBranchTypes ADD COLUMN 'NumPolicyRequirement' INTEGER DEFAULT 0;

-- Belief - increases pressure from trade routes

ALTER TABLE Beliefs ADD COLUMN 'PressureChangeTradeRoute' INTEGER DEFAULT 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE Eras ADD COLUMN 'StartingMinorDefenseUnits' INTEGER DEFAULT 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE HandicapInfos ADD COLUMN 'StartingMinorDefenseUnits' INTEGER DEFAULT 0;

-- CBP Difficulty Bonus
ALTER TABLE HandicapInfos ADD COLUMN 'DifficultyBonus' INTEGER DEFAULT 0;

-- Grants additional starting happiness based on gamespeed.

ALTER TABLE GameSpeeds ADD COLUMN 'StartingHappiness' INTEGER DEFAULT 0;

-- No unhappiness from Isolation.

ALTER TABLE Traits ADD COLUMN 'NoConnectionUnhappiness' BOOLEAN DEFAULT 0;

-- No unhappiness from religious strife.

ALTER TABLE Traits ADD COLUMN 'IsNoReligiousStrife' BOOLEAN DEFAULT 0;

-- Extra Wonder Production during Golden Ages.

ALTER TABLE Traits ADD COLUMN 'WonderProductionModGA' INTEGER DEFAULT 0;

-- Abnormal scaler for Specialist yields in cities UA. +specialist yield in medieval/industrial/atomic eras.

ALTER TABLE Traits ADD COLUMN 'IsOddEraScaler' BOOLEAN DEFAULT 0;

-- No natural religion spread to/from unowned cities

ALTER TABLE Traits ADD COLUMN 'NoNaturalReligionSpread' BOOLEAN DEFAULT 0;

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

-- Adds a trait that boosts the value of historic event tourism. 1 = 10%, 2 = 20%, etc.

ALTER TABLE Traits ADD COLUMN 'EventTourismBoost' INTEGER DEFAULT 0;

-- Adds x# of GP Points to Capital (scaling with era) when you complete a Historic Event.

ALTER TABLE Traits ADD COLUMN 'EventGP' INTEGER DEFAULT 0;

-- Grants a free valid promotion to a unit when it is on a type of improvement (farm, mine, etc.).

ALTER TABLE Improvements ADD COLUMN 'UnitFreePromotion' TEXT DEFAULT NULL;

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

-- Adds minimum national population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'NationalPopRequired' INTEGER DEFAULT 0;

-- Adds minimum local population requirement for a building.
ALTER TABLE Buildings ADD COLUMN 'LocalPopRequired' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for land plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleCity' INTEGER DEFAULT 0;

-- Movement speed penalty (like Great Wall) for water plots worked by a City.
ALTER TABLE Buildings ADD COLUMN 'BorderObstacleWater' INTEGER DEFAULT 0;

-- Adds abiility for units to upgrade in allied CS lands.
ALTER TABLE Policies ADD COLUMN 'UpgradeCSTerritory' BOOLEAN DEFAULT 0;

-- Adds event tourism from digging up sites.
ALTER TABLE Policies ADD COLUMN 'ArchaeologicalDigTourism' INTEGER DEFAULT 0;
-- Adds event tourism from golden ages starting.
ALTER TABLE Policies ADD COLUMN 'GoldenAgeTourism' INTEGER DEFAULT 0;

-- Reduces specialist unhappiness in cities by a set amount, either in capital or in all cities.

ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialists' INTEGER DEFAULT 0;
ALTER TABLE Policies ADD COLUMN 'NoUnhappfromXSpecialistsCapital' INTEGER DEFAULT 0;

-- Half specialist food in just capital
ALTER TABLE Policies ADD COLUMN 'HalfSpecialistFoodCapital' BOOLEAN DEFAULT 0;

-- Flat boosts to city yield for happiness sources (buildings) - values should be positive to be good!
ALTER TABLE Buildings ADD COLUMN 'PovertyHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'DefenseHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'IlliteracyHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'UnculturedHappinessChange' INTEGER DEFAULT 0;
ALTER TABLE Buildings ADD COLUMN 'MinorityHappinessChange' INTEGER DEFAULT 0;

-- Flat global boosts to city yield for happiness sources (buildings) - values should be positive to be good!
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

-- Global Happiness Based on # of Citizens in Empire

ALTER TABLE Policies ADD COLUMN 'HappinessPerXPopulationGlobal' INTEGER DEFAULT 0;

-- War Weariness Mod - Positive values make it harder to accumulate war weariness.
ALTER TABLE Policies ADD COLUMN 'WarWearinessModifier' INTEGER DEFAULT 0;

-- % boosts to city yield for happiness sources (traits) - Values should be positive to be good!
ALTER TABLE Traits ADD COLUMN 'PovertyHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'DefenseHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'IlliteracyHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'UnculturedHappinessTraitMod' INTEGER DEFAULT 0;
ALTER TABLE Traits ADD COLUMN 'MinorityHappinessTraitMod' INTEGER DEFAULT 0;

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

-- New Traits - GG bonus when spawned
ALTER TABLE Traits ADD COLUMN 'InspirationalLeader' BOOLEAN DEFAULT 0;

-- New Traits - Diplomatic Marriage bonus (Austria UA - CBP)
ALTER TABLE Traits ADD COLUMN 'DiplomaticMarriage' BOOLEAN DEFAULT 0;

-- New Traits - Adoption of Policies/Beliefs/Ideology = free tech
ALTER TABLE Traits ADD COLUMN 'IsAdoptionFreeTech' BOOLEAN DEFAULT 0;

-- New Traits - Extra Growth from GA/WLTKD
ALTER TABLE Traits ADD COLUMN 'GrowthBoon' INTEGER DEFAULT 0;

-- New Traits - WLTKD from GP birth
ALTER TABLE Traits ADD COLUMN 'GPWLTKD' BOOLEAN DEFAULT 0;

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

-- Extra Votes from Belief
ALTER TABLE Beliefs ADD COLUMN 'HalvedFollowers' BOOLEAN DEFAULT 0;

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

-- Allows for Unit to be purchased in puppet city
ALTER TABLE Units ADD COLUMN 'PuppetPurchaseOverride' BOOLEAN DEFAULT 0;


-- Grants resource to improvement
ALTER TABLE Improvements ADD COLUMN 'ImprovementResource' TEXT DEFAULT NULL;

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

ALTER TABLE UnitPromotions ADD 'ReconChange' INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions ADD 'GainsXPFromScouting' BOOLEAN DEFAULT 0;

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

-- Double Heal in Feature/Terrain
ALTER TABLE UnitPromotions_Features ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;
ALTER TABLE UnitPromotions_Terrains ADD COLUMN 'DoubleHeal' BOOLEAN DEFAULT 0;

-- Unit stuff for minor civs
ALTER TABLE Units ADD COLUMN 'MinorCivGift' BOOLEAN DEFAULT 0;

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

-- Limits the amount that can be built of a Unit class per city
ALTER TABLE UnitClasses ADD COLUMN 'UnitInstancePerCity' INTEGER DEFAULT -1;

-- Trade Route Internal Capital Bonus -- policy -- Internal TR from Capital stronger!
ALTER TABLE Policies ADD COLUMN 'InternalTradeRouteYieldModifierCapital' INTEGER DEFAULT 0;

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


-- Resources
ALTER TABLE Resources ADD COLUMN 'StrategicHelp' TEXT DEFAULT NULL;

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

