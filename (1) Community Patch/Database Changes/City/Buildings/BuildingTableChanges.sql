-- TODO: Documentation
-- Unless otherwise stated, effects are applied to the city with the building.

ALTER TABLE Buildings ADD AddsFreshWater boolean DEFAULT 0;
ALTER TABLE Buildings ADD PurchaseOnly boolean DEFAULT 0;

-- Create buildings that cannot have access to fresh water.
ALTER TABLE Buildings ADD IsNoWater boolean DEFAULT 0;
ALTER TABLE Buildings ADD IsNoRiver boolean DEFAULT 0;

-- Create buildings that cannot have access to fresh water.
ALTER TABLE Buildings ADD IsNoCoast boolean DEFAULT 0;

-- Create buildings that must be in the Capital
ALTER TABLE Buildings ADD CapitalOnly boolean DEFAULT 0;

-- Requirement that allows you to bypass prereq techs to get something.
ALTER TABLE Buildings ADD NumPoliciesNeeded integer DEFAULT 0;

-- Reduce specialist unhappiness from urbanization (VP)
ALTER TABLE Buildings ADD NoUnhappfromXSpecialists integer DEFAULT 0;
-- Reduce specialist unhappiness from urbanization (VP)
ALTER TABLE Buildings ADD NoUnhappfromXSpecialistsGlobal integer DEFAULT 0;

-- +x% Food for each follower of the city's majority religion
ALTER TABLE Buildings ADD FoodBonusPerCityMajorityFollower integer DEFAULT 0;

-- BUILDING: PlayerBorderGainlessPillage & CityGainlessPillage
-- If such a building's effect applies, other teams get neither gold nor heal from pillaging the appropriate tiles.
-- CityGainlessPillage affects the constructing city's worked tiles, PlayerBorderGainlessPillage proofs every tile of the player
ALTER TABLE Buildings ADD PlayerBorderGainlessPillage boolean DEFAULT 0;
ALTER TABLE Buildings ADD CityGainlessPillage boolean DEFAULT 0;

-- Allows industrial city connections via water
ALTER TABLE Buildings ADD AllowsIndustrialWaterRoutes boolean DEFAULT 0;

-- Allows city connections via the air
ALTER TABLE Buildings ADD AllowsAirRoutes boolean DEFAULT 0;

-- Increase to border growth expansion rate
ALTER TABLE Buildings ADD BorderGrowthRateIncrease integer DEFAULT 0; -- For the city
ALTER TABLE Buildings ADD BorderGrowthRateIncreaseGlobal integer DEFAULT 0; -- For the empire

-- +X Influence from Diplomatic Missions for units that originate from this city
ALTER TABLE Buildings ADD DiplomatInfluenceBoost integer DEFAULT 0;

-- Adds minimum national population requirement for a building.
ALTER TABLE Buildings ADD NationalPopRequired integer DEFAULT 0;

-- Adds minimum local population requirement for a building.
ALTER TABLE Buildings ADD LocalPopRequired integer DEFAULT 0;

-- Adds population instantly to city.
ALTER TABLE Buildings ADD PopulationChange integer DEFAULT 0;

-- Newly trained units in this city gain additional experience for every golden age started after contruction of the building
ALTER TABLE Buildings ADD ExperiencePerGoldenAge integer DEFAULT 0;
ALTER TABLE Buildings ADD ExperiencePerGoldenAgeCap integer DEFAULT 0;

-- Movement speed penalty (like Great Wall) for land plots worked by a City.
ALTER TABLE Buildings ADD BorderObstacleCity boolean DEFAULT 0;

-- Movement speed penalty (like Great Wall) for water plots worked by a City.
ALTER TABLE Buildings ADD BorderObstacleWater boolean DEFAULT 0;

-- x damage to units that end turn on a deep water tile owned by a city
ALTER TABLE Buildings ADD DeepWaterTileDamage integer DEFAULT 0;

-- One building gives all cities this ability
ALTER TABLE Buildings ADD AllowsFoodTradeRoutesGlobal boolean DEFAULT 0;
ALTER TABLE Buildings ADD AllowsProductionTradeRoutesGlobal boolean DEFAULT 0;

-- Local city connection strong
ALTER TABLE Buildings ADD CityConnectionGoldModifier integer DEFAULT 0;

-- % modifiers to empire size modifier - negative = good!
ALTER TABLE Buildings ADD EmpireSizeModifierReduction integer DEFAULT 0;
ALTER TABLE Buildings ADD EmpireSizeModifierReductionGlobal integer DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in this city
ALTER TABLE Buildings ADD DistressFlatReduction integer DEFAULT 0;
ALTER TABLE Buildings ADD PovertyFlatReduction integer DEFAULT 0;
ALTER TABLE Buildings ADD IlliteracyFlatReduction integer DEFAULT 0;
ALTER TABLE Buildings ADD BoredomFlatReduction integer DEFAULT 0;
ALTER TABLE Buildings ADD ReligiousUnrestFlatReduction integer DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in all Cities
ALTER TABLE Buildings ADD DistressFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD PovertyFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD IlliteracyFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD BoredomFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD ReligiousUnrestFlatReductionGlobal integer DEFAULT 0;

-- Changes the global median for a Need in this city by x% - values should be negative to be good!
ALTER TABLE Buildings ADD BasicNeedsMedianModifier integer DEFAULT 0;
ALTER TABLE Buildings ADD GoldMedianModifier integer DEFAULT 0;
ALTER TABLE Buildings ADD ScienceMedianModifier integer DEFAULT 0;
ALTER TABLE Buildings ADD CultureMedianModifier integer DEFAULT 0;
ALTER TABLE Buildings ADD ReligiousUnrestModifier integer DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Buildings ADD BasicNeedsMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD GoldMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD ScienceMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD CultureMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Buildings ADD ReligiousUnrestModifierGlobal integer DEFAULT 0;

-- Building Modifies amount of unhappiness provided by an individual city
ALTER TABLE Buildings ADD LocalUnhappinessModifier integer DEFAULT 0;

-- Building Modifies amount of maintenance buildings cost in your empire.
ALTER TABLE Buildings ADD GlobalBuildingGoldMaintenanceMod integer DEFAULT 0;

-- National Religious Followers Needed for a Building
ALTER TABLE Buildings ADD NationalFollowerPopRequired integer DEFAULT 0;

-- Global Religious Followers Needed for a Building
ALTER TABLE Buildings ADD GlobalFollowerPopRequired integer DEFAULT 0;

-- Reduces the value above by x value (a % reduction)
ALTER TABLE Buildings ADD ReformationFollowerReduction integer DEFAULT 0;

-- Gives all current and future missionaries an extra x spread(s)
ALTER TABLE Buildings ADD ExtraMissionarySpreadsGlobal integer DEFAULT 0;

-- Allows for Reformation Policy
ALTER TABLE Buildings ADD IsReformation boolean DEFAULT 0;

-- Allows for Building to be unlocked by a specific policy (not a branch)
ALTER TABLE Buildings ADD PolicyType text REFERENCES Policies (Type);

-- Allows for Building to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Buildings ADD ResourceType text REFERENCES Resources (Type);

-- Allows for Building to be purchased in puppet city
ALTER TABLE Buildings ADD PuppetPurchaseOverride boolean DEFAULT 0;

-- Allows for Building to grant a single WC vote (or any value) - not scaled by CS
ALTER TABLE Buildings ADD SingleLeagueVotes integer DEFAULT 0;

-- Allows for All Units/Buildings to be purchased in puppet city
ALTER TABLE Buildings ADD AllowsPuppetPurchase boolean DEFAULT 0;

-- Creates a resource unique to this civ (i.e. Indonesian Candi) in the territory around the city. To make this work with a civ, you'll need to create a new resource modelled on the Indonesian resources and assign them to the civ. Value is indicative of the number of resources that will be granted.
ALTER TABLE Buildings ADD GrantsRandomResourceTerritory integer DEFAULT 0;

-- Boosts trade route religious pressure on buildings
ALTER TABLE Buildings ADD TradeReligionModifier integer DEFAULT 0;

-- Allows you to define a building needed by this building (similar to BuildingClassNeeded) -->
ALTER TABLE Buildings ADD NeedBuildingThisCity text REFERENCES Buildings (Type);

-- Allows you to define a number of WLTKD turns for the City -- 
ALTER TABLE Buildings ADD WLTKDTurns integer DEFAULT 0;

-- Allows you to define an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed. 
ALTER TABLE Buildings ADD EventTourism integer DEFAULT 0;

-- Allows you to define an amount that a unit will heal in a city whether or not it took an action this turn.
ALTER TABLE Buildings ADD AlwaysHeal integer DEFAULT 0;

-- Allows you to define an amount that a city will increase your unit supply cap by x% per pop.
ALTER TABLE Buildings ADD CitySupplyModifier integer DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by x% per pop globally.
ALTER TABLE Buildings ADD CitySupplyModifierGlobal integer DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by a flat value.
ALTER TABLE Buildings ADD CitySupplyFlat integer DEFAULT 0;
-- Allows you to define an amount that a city will increase your unit supply cap by a flat value globally.
ALTER TABLE Buildings ADD CitySupplyFlatGlobal integer DEFAULT 0;

-- Allows buildings to modify a city's ranged strike parameters.
ALTER TABLE Buildings ADD CityRangedStrikeRange integer DEFAULT 0;
ALTER TABLE Buildings ADD CityIndirectFire boolean DEFAULT 0;
ALTER TABLE Buildings ADD RangedStrikeModifier integer DEFAULT 0;

-- Missionaries built by a city gain % more strength
ALTER TABLE Buildings ADD ExtraMissionaryStrengthGlobal integer DEFAULT 0;

-- Resource Diversity Provided by a City increased by x% (or reduced by x% if negative
ALTER TABLE Buildings ADD ResourceDiversityModifier integer DEFAULT 0;

-- When a City with this building finishes a land ITR, gain tourism with the target civ.
ALTER TABLE Buildings ADD FinishLandTRTourism integer DEFAULT 0;

-- When a City with this building finishes a sea ITR, gain tourism with the target civ.
ALTER TABLE Buildings ADD FinishSeaTRTourism integer DEFAULT 0;

-- WC vote per GPT - GPT / x (value below) = votes
ALTER TABLE Buildings ADD VotesPerGPT integer DEFAULT 0;

-- Requires an Industrial City Connection to be built
ALTER TABLE Buildings ADD RequiresIndustrialCityConnection boolean DEFAULT 0;

-- Civ-specific building (but not necessarily a UB!)
ALTER TABLE Buildings ADD CivilizationRequired text REFERENCES Civilizations (Type);

-- reduces purchase cooldown on units
ALTER TABLE Buildings ADD PurchaseCooldownReduction integer DEFAULT 0;

-- reduces purchase cooldown on civilian units
ALTER TABLE Buildings ADD PurchaseCooldownReductionCivilian integer DEFAULT 0;

ALTER TABLE Buildings ADD PurchaseCooldown integer DEFAULT 0;

-- Base air defense when defending against an airstrike.
ALTER TABLE Buildings ADD CityAirStrikeDefense integer DEFAULT 0;

-- Allows Courthouses to be built in any city
ALTER TABLE Buildings ADD BuildAnywhere boolean DEFAULT 0;

-- Builds 1+ artifact in the building, up to the number of artifact slots in the building.
ALTER TABLE Buildings ADD FreeArtifacts integer DEFAULT 0;

-- City Security against Spies
ALTER TABLE Buildings ADD SpySecurityModifier integer DEFAULT 0;
-- City Security against Spies per X, where X = (total of this value in the city) * (city population) / ESPIONAGE_SECURITY_PER_POPULATION_BUILDING_SCALER
ALTER TABLE Buildings ADD SpySecurityModifierPerXPop integer DEFAULT 0;
-- City Security against Spies in all Cities
ALTER TABLE Buildings ADD GlobalSpySecurityModifier integer DEFAULT 0;

ALTER TABLE Buildings ADD VassalLevyEra integer DEFAULT 0;

ALTER TABLE Buildings ADD NumRequiredTier3Tenets integer DEFAULT 0;

--Increase Air Unit Support Per City Global
ALTER TABLE Buildings ADD AirModifierGlobal integer DEFAULT 0;

ALTER TABLE Buildings ADD EventChoiceRequiredActive text REFERENCES EventChoices (Type);
ALTER TABLE Buildings ADD CityEventChoiceRequiredActive text REFERENCES CityEventChoices (Type);

ALTER TABLE Buildings ADD IsCorporation boolean DEFAULT 0;
ALTER TABLE Buildings ADD GPRateModifierPerXFranchises integer DEFAULT 0;
ALTER TABLE Buildings ADD TRSpeedBoost integer DEFAULT 0;
ALTER TABLE Buildings ADD TRVisionBoost integer DEFAULT 0;
ALTER TABLE Buildings ADD TRTurnModGlobal integer DEFAULT 0; -- modifies the turns a TR takes to complete, an int between 100 and -100, like a percent
ALTER TABLE Buildings ADD TRTurnModLocal integer DEFAULT 0;

-- CSD
ALTER TABLE Buildings ADD DPToVotes integer DEFAULT 0;

ALTER TABLE Buildings ADD SecondaryPantheon boolean DEFAULT 0;

ALTER TABLE Buildings ADD IsDummy boolean DEFAULT 0;

-- Can this building be built next to any body of water?
ALTER TABLE Buildings ADD AnyWater boolean DEFAULT 0;

-- Define a defense modifier to a building, like GlobalDefenseModifier (but only local).
ALTER TABLE Buildings ADD BuildingDefenseModifier integer DEFAULT 0;

-- Damage to the city with this building is reduced by this flat amount.
ALTER TABLE Buildings ADD DamageReductionFlat integer DEFAULT 0;

-- Define a modifier for all tile/building based tourism in all cities.
ALTER TABLE Buildings ADD GlobalLandmarksTourismPercent integer DEFAULT 0;

-- Define a modifier for all great work tourism in all cities.
ALTER TABLE Buildings ADD GlobalGreatWorksTourismModifier integer DEFAULT 0;

-- Define a modifier for cities to be able to intercept nukes.
-- Atomic Bombs are completely destroyed, Nuclear Missiles are converted to Atomic Bombs before detonation.
ALTER TABLE Buildings ADD NukeInterceptionChance integer DEFAULT 0;

-- Table for Lua elements that we don't want shown in Civ selection screen or in Civilopedia
ALTER TABLE Buildings ADD ShowInPedia boolean DEFAULT 1;

-- Gives a boost of pressure in the city when built (based on what the religion is in your capital)
ALTER TABLE Buildings ADD InstantReligiousPressure integer DEFAULT 0;

-- All owned cities following the state religion emanates X% more pressure
ALTER TABLE Buildings ADD BasePressureModifierGlobal integer DEFAULT 0;

-- If the player has this building, they ignore the global Defensive Pact limit
ALTER TABLE Buildings ADD IgnoreDefensivePactLimit boolean DEFAULT 0;

ALTER TABLE Buildings ADD FaithToVotes integer DEFAULT 0;

ALTER TABLE Buildings ADD CapitalsToVotes integer DEFAULT 0;

ALTER TABLE Buildings ADD DoFToVotes integer DEFAULT 0;

ALTER TABLE Buildings ADD RAToVotes integer DEFAULT 0;

ALTER TABLE Buildings ADD GPExpendInfluence integer DEFAULT 0;

ALTER TABLE Buildings ADD CityAutomatonWorkersChange integer DEFAULT 0;
ALTER TABLE Buildings ADD GlobalCityAutomatonWorkersChange integer DEFAULT 0;

ALTER TABLE Buildings ADD CityWorkingChange integer DEFAULT 0;
ALTER TABLE Buildings ADD GlobalCityWorkingChange integer DEFAULT 0;

-- Modifier for GP rate per active marriage with city state (not at war)
ALTER TABLE Buildings ADD GPRateModifierPerMarriage integer DEFAULT 0;

-- Modifier for GP rate per completed great work theme
ALTER TABLE Buildings ADD GPRateModifierPerLocalTheme integer DEFAULT 0;

-- Instant GPP for the great person with the most points when a citizen is born, scaling with era
ALTER TABLE Buildings ADD GPPOnCitizenBirth integer DEFAULT 0;

-- Global happiness per major civ currently at war with the player
ALTER TABLE Buildings ADD GlobalHappinessPerMajorWar integer DEFAULT 0;

-- Global modifier for military unit production per major civ currently at war with the player
ALTER TABLE Buildings ADD GlobalMilitaryProductionModPerMajorWar integer DEFAULT 0;

-- Non-specialists cannot consume more than the city's food per turn
ALTER TABLE Buildings ADD NoStarvationNonSpecialist boolean DEFAULT 0;

-- 1 city strength per X national/world wonder
ALTER TABLE Buildings ADD DefensePerXWonder integer DEFAULT 0;

-- Modifier for ranged combat strength of garrison when attacking
ALTER TABLE Buildings ADD GarrisonRangedAttackModifier integer DEFAULT 0;

-- GLOBAL_GREATWORK_YIELDTYPES
ALTER TABLE Buildings ADD GreatWorkYieldType text REFERENCES Yields (Type) DEFAULT 'YIELD_CULTURE';

-- RELIGION_CONVERSION_MODIFIERS
ALTER TABLE Buildings ADD GlobalConversionModifier integer DEFAULT 0;
ALTER TABLE Buildings ADD ConversionModifier integer DEFAULT 0;

ALTER TABLE Building_ThemingBonuses ADD ConsecutiveEras integer DEFAULT 0;

-- The tech required to boost the building
-- If NULL, will fall back to using Buildings.EnhancedYieldTech
-- Does nothing if both columns are NULL
ALTER TABLE Building_TechEnhancedYieldChanges ADD TechType text REFERENCES Technologies (Type);
