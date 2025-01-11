-- TODO: Documentation

-- Policy - increases happiness in every city for which city strength is at least the define value 'CITY_STRENGTH_THRESHOLD_FOR_BONUSES'
ALTER TABLE Policies ADD HappinessPerCityOverStrengthThreshold integer DEFAULT 0;

-- Policy - increases potency of beakers for GSs
ALTER TABLE Policies ADD GreatScientistBeakerModifier integer DEFAULT 0;
-- and GE hurry
ALTER TABLE Policies ADD GreatEngineerHurryModifier integer DEFAULT 0;

-- Reduce tech costs from cities:
ALTER TABLE Policies ADD TechCostXCitiesMod integer DEFAULT 0;
-- And reduce tourism cost from cities
ALTER TABLE Policies ADD TourismCostXCitiesMod integer DEFAULT 0;

-- MaxAirUnitsChangeGlobal - every city gets that many air unit slots
ALTER TABLE Policies ADD MaxAirUnitsChangeGlobal integer DEFAULT 0;

-- Policy - reduces policy cost of Wonders by 1 for every x CS allies
ALTER TABLE Policies ADD XCSAlliesLowersPolicyNeedWonders integer DEFAULT 0;

-- Policy - reduce speed of people stealing Great Works from you.
ALTER TABLE Policies ADD StealGWSlowerModifier integer DEFAULT 0;
-- Policy - increase speed of your Great Work theft rate.
ALTER TABLE Policies ADD StealGWFasterModifier integer DEFAULT 0;
-- Policy - conquered cities keep all their buildings (except for those with NeverCapture==true).
ALTER TABLE Policies ADD KeepConqueredBuildings boolean DEFAULT 0;

-- Policy - extra GAP generation from excess happiness.
ALTER TABLE Policies ADD GAPFromHappinessModifier integer DEFAULT 0;

-- Adds abiility for units to upgrade in allied CS or vassal lands.
ALTER TABLE Policies ADD UpgradeCSVassalTerritory boolean DEFAULT 0;

-- Spies gain additional Network Points per Turn
ALTER TABLE Policies ADD EspionageNetworkPoints integer DEFAULT 0;

-- Modifies passive bonuses for Spies
ALTER TABLE Policies ADD PassiveEspionageBonusModifier integer DEFAULT 0;

-- Modifies influence gained/lost when rigging elections
ALTER TABLE Policies ADD RigElectionInfluenceModifier integer DEFAULT 0;

-- Adds event tourism from digging up sites.
ALTER TABLE Policies ADD ArchaeologicalDigTourism integer DEFAULT 0;
-- Adds event tourism from golden ages starting.
ALTER TABLE Policies ADD GoldenAgeTourism integer DEFAULT 0;

-- Gives event tourism when completing internal trade routes as if they are international.
ALTER TABLE Policies ADD InternalTRTourism boolean DEFAULT 0;

-- Reduces specialist unhappiness in cities by a set amount, either in capital or in all cities.
ALTER TABLE Policies ADD NoUnhappfromXSpecialists integer DEFAULT 0;
-- adds it back in as happiness!
ALTER TABLE Policies ADD HappfromXSpecialists integer DEFAULT 0;
ALTER TABLE Policies ADD NoUnhappfromXSpecialistsCapital integer DEFAULT 0;

-- Warscore mod
ALTER TABLE Policies ADD WarScoreModifier integer DEFAULT 0;

-- Can only trade with same ideology
ALTER TABLE Policies ADD IsOnlyTradeSameIdeology boolean DEFAULT 0;

-- Half specialist food in just capital
ALTER TABLE Policies ADD HalfSpecialistFoodCapital boolean DEFAULT 0;

-- Increases influence from trade mission
ALTER TABLE Policies ADD MissionInfluenceModifier integer DEFAULT 0;

-- Happiness per trade route active
ALTER TABLE Policies ADD HappinessPerActiveTradeRoute integer DEFAULT 0;

-- air units reduce unhappiness from needs by x% per unit.
ALTER TABLE Policies ADD NeedsModifierFromAirUnits integer DEFAULT 0;

-- air units increase defense by +x flat per unit
ALTER TABLE Policies ADD FlatDefenseFromAirUnits integer DEFAULT 0;

-- puppet penalties reduced by positive amount
ALTER TABLE Policies ADD PuppetYieldPenaltyMod integer DEFAULT 0;

-- Puppet/Occupied cities gain x+x*era difference for buildings.
ALTER TABLE Policies ADD ConquestPerEraBuildingProductionMod integer DEFAULT 0;

-- Increased free luxuries created by freelux routine for units (used by admiral in VP).
ALTER TABLE Policies ADD AdmiralLuxuryBonus integer DEFAULT 0;

-- CS resources count towards monopolies
ALTER TABLE Policies ADD CSResourcesCountForMonopolies boolean DEFAULT 0;

-- Liberating a city gives influence to all CS
ALTER TABLE Policies ADD InfluenceAllCSFromLiberation integer DEFAULT 0;

-- Liberating a city gives that city some units 
ALTER TABLE Policies ADD NumUnitsInLiberatedCities integer DEFAULT 0;

-- Liberating a city gives XP to all units
ALTER TABLE Policies ADD ExperienceAllUnitsFromLiberation integer DEFAULT 0;

-- Conquering a city heals all units (by X%)
ALTER TABLE Policies ADD CityCaptureHealGlobal integer DEFAULT 0;

-- Conquering a city heals all units near the city (by X%)
ALTER TABLE Policies ADD CityCaptureHealLocal integer DEFAULT 0;

-- Flat reductions to Unhappiness from Needs in all Cities
ALTER TABLE Policies ADD DistressFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD PovertyFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD IlliteracyFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD BoredomFlatReductionGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD ReligiousUnrestFlatReductionGlobal integer DEFAULT 0;

-- Changes the global median for a Need in the capital by x% - values should be negative to be good!
ALTER TABLE Policies ADD BasicNeedsMedianModifierCapital integer DEFAULT 0;
ALTER TABLE Policies ADD GoldMedianModifierCapital integer DEFAULT 0;
ALTER TABLE Policies ADD ScienceMedianModifierCapital integer DEFAULT 0;
ALTER TABLE Policies ADD CultureMedianModifierCapital integer DEFAULT 0;
ALTER TABLE Policies ADD ReligiousUnrestModifierCapital integer DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Policies ADD BasicNeedsMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD GoldMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD ScienceMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD CultureMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Policies ADD ReligiousUnrestModifierGlobal integer DEFAULT 0;

-- Puppets and/or Occupied cities receive a % production modifier. Values should be positive to be good.
ALTER TABLE Policies ADD PuppetProdMod integer DEFAULT 0;
ALTER TABLE Policies ADD OccupiedProdMod integer DEFAULT 0;

-- Increases City Defense - needs to be in 100s value
ALTER TABLE Policies ADD DefenseBoostAllCities integer DEFAULT 0;

-- Community Patch: Global Happiness Based on # of Citizens in Empire
-- Vox Populi: Local Happiness for each X Citizens in this city
ALTER TABLE Policies ADD HappinessPerXPopulationGlobal integer DEFAULT 0;

-- War Weariness Mod - Positive values make it harder to accumulate war weariness.
ALTER TABLE Policies ADD WarWearinessModifier integer DEFAULT 0;

-- GG Mod - boosts strength modifier of GGs
ALTER TABLE Policies ADD GreatGeneralExtraBonus integer DEFAULT 0;

-- Shift Influence with CS for all by % +/- when you bully
ALTER TABLE Policies ADD BullyGlobalCSInfluenceShift integer DEFAULT 0;

-- More yields from Vassals and CSs
ALTER TABLE Policies ADD VassalYieldBonusModifier integer DEFAULT 0;
ALTER TABLE Policies ADD CSYieldBonusModifier integer DEFAULT 0;

-- Removes penalties for bullying City-States (no Influence loss or PTP/quest revocation)
ALTER TABLE Policies ADD CanBullyFriendlyCS boolean DEFAULT 0;

-- Allied CS influence does not decline at war
ALTER TABLE Policies ADD NoAlliedCSInfluenceDecayAtWar boolean DEFAULT 0;

-- Allied CS influence resting point increases while at war
ALTER TABLE Policies ADD MinimumAllyInfluenceIncreaseAtWar integer DEFAULT 0;

-- Vassals don't rebel and can't be forced out by deals or WC
ALTER TABLE Policies ADD VassalsNoRebel boolean DEFAULT 0;

-- Reduces unhappiness in occupied cities w/ Garrison. Negative = reduction.
ALTER TABLE Policies ADD GarrisonsOccupiedUnhappinessMod integer DEFAULT 0;

-- Spawns a free unit at x population for a city.
ALTER TABLE Policies ADD XPopulationConscription integer DEFAULT 0;

-- No Unhappiness from Expansion
ALTER TABLE Policies ADD NoUnhappinessExpansion boolean DEFAULT 0;

-- No Unhappiness from Isolation
ALTER TABLE Policies ADD NoUnhappyIsolation boolean DEFAULT 0;

-- Double City Border Growth During GA (does not stack with WLTKD)
ALTER TABLE Policies ADD DoubleBorderGrowthGA boolean DEFAULT 0;

-- Double City Border Growth During WLTKD (does not stack with GA)
ALTER TABLE Policies ADD DoubleBorderGrowthWLTKD boolean DEFAULT 0;

-- Free Population
ALTER TABLE Policies ADD FreePopulation integer DEFAULT 0;
ALTER TABLE Policies ADD FreePopulationCapital integer DEFAULT 0;

-- Extra Moves for Civilian Units
ALTER TABLE Policies ADD ExtraMoves integer DEFAULT 0;

-- Religious Pressure Mod Trade Route
ALTER TABLE Policies ADD TradeReligionModifier integer DEFAULT 0;

-- Increased Quest Influence
ALTER TABLE Policies ADD IncreasedQuestRewards integer DEFAULT 0;

-- Extra vote in WC for every X City-States originally in the world
ALTER TABLE Policies ADD FreeWCVotes integer DEFAULT 0;

-- Gives votes per city following your state religion
ALTER TABLE Policies ADD VotesPerFollowingCityTimes100 integer DEFAULT 0;

-- GP Expend Influence Boost
ALTER TABLE Policies ADD InfluenceGPExpend integer DEFAULT 0;

-- Free Trade Route
ALTER TABLE Policies ADD FreeTradeRoute integer DEFAULT 0;

-- Free Spy
ALTER TABLE Policies ADD FreeSpy integer DEFAULT 0;

-- City Security Modifier
ALTER TABLE Policies ADD SpySecurityModifier integer DEFAULT 0;

-- Gold from Internal Trade Routes
ALTER TABLE Policies ADD InternalTradeGold integer DEFAULT 0;

-- Boost Culture Bomb from Citadel
ALTER TABLE Policies ADD CultureBombBoost integer DEFAULT 0;

-- Unlock Era for Policy (Unlocks later eras earlier than normal)
ALTER TABLE Policies ADD UnlocksPolicyBranchEra text REFERENCES Eras (Type);

-- Points towards ideologies
ALTER TABLE Policies ADD IdeologyPoint integer DEFAULT 0;

-- Religious Distance Modifier (affects spread to owned cities based on distance)
ALTER TABLE Policies ADD ReligionDistance integer DEFAULT 0;

-- Religious Pressure Modifier (only from cities following state religion)
ALTER TABLE Policies ADD PressureMod integer DEFAULT 0;

-- Allows you to define a an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed.
ALTER TABLE Policies ADD EventTourismCS integer DEFAULT 0;

-- Allows you to define a an amount of Tourism gained from GP birth, WW creation, and CS quest completion. Scales with gamespeed.
ALTER TABLE Policies ADD EventTourism integer DEFAULT 0;

-- Define a policy as a dummy policy - won't count towards policy total
ALTER TABLE Policies ADD IsDummy boolean DEFAULT 0;

-- This policy is given for free when a player completes the tree; useful for counting elements
ALTER TABLE Policies ADD IsFinisher boolean DEFAULT 0;

-- Allows you to increase razing speed from policies
ALTER TABLE Policies ADD RazingSpeedBonus integer DEFAULT 0;

-- Allows you to set whether or not partisans spawn from razing cities (via policy)
ALTER TABLE Policies ADD NoPartisans boolean DEFAULT 0;

-- Allows you to set whether or not units gain full XP when purchased
ALTER TABLE Policies ADD NoXPLossUnitPurchase boolean DEFAULT 0;

-- Allows you to set a % of warscore that is added to a tourism bonus against a civ
ALTER TABLE Policies ADD PositiveWarScoreTourismMod integer DEFAULT 0;

-- Alters monopoly values below (except for strategic elements, like attack/defense/heal/etc.)
ALTER TABLE Policies ADD MonopolyModFlat integer DEFAULT 0;
ALTER TABLE Policies ADD MonopolyModPercent integer DEFAULT 0;

-- Investment reduction costs -- policy -- negative makes it stronger!
ALTER TABLE Policies ADD InvestmentModifier integer DEFAULT 0;

-- Trade Route Internal Capital Bonus -- policy -- Internal/INTL TR from Capital and Holy City stronger!
ALTER TABLE Policies ADD InternalTradeRouteYieldModifierCapital integer DEFAULT 0;
ALTER TABLE Policies ADD TradeRouteYieldModifierCapital integer DEFAULT 0;
-- Or all routes.
ALTER TABLE Policies ADD TradeRouteYieldModifier integer DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD GreatEngineerRateModifier integer DEFAULT 0;

-- Great Engineer Policy bonus - rate modifier.
ALTER TABLE Policies ADD CityStateCombatModifier integer DEFAULT 0;

-- Trade Route Modifiers for Policies
ALTER TABLE Policies ADD TradeRouteLandDistanceModifier integer DEFAULT 0;
ALTER TABLE Policies ADD TradeRouteSeaDistanceModifier integer DEFAULT 0;

ALTER TABLE Policies ADD CityAutomatonWorkersChange integer DEFAULT 0;

ALTER TABLE Policies ADD CityWorkingChange integer DEFAULT 0;

-- RELIGION_CONVERSION_MODIFIERS
ALTER TABLE Policies ADD ConversionModifier integer DEFAULT 0;

ALTER TABLE Policies ADD TRSpeedBoost integer DEFAULT 0;
ALTER TABLE Policies ADD TRVisionBoost integer DEFAULT 0;
ALTER TABLE Policies ADD HappinessPerXPolicies integer DEFAULT 0;
ALTER TABLE Policies ADD HappinessPerXGreatWorks integer DEFAULT 0;

-- Alters food consumption of specialists - use integer (is raised to 100s later)
ALTER TABLE Policies ADD SpecialistFoodChange integer DEFAULT 0;

-- Trade Routes
ALTER TABLE Policies ADD ExtraCultureandScienceTradeRoutes integer DEFAULT 0;

-- Corporation Policies
ALTER TABLE Policies ADD CorporationOfficesAsNumFranchises integer DEFAULT 0;
ALTER TABLE Policies ADD CorporationNumFreeFranchiseAbovePopular integer DEFAULT 0;
ALTER TABLE Policies ADD CorporationRandomForeignFranchiseMod integer DEFAULT 0;
ALTER TABLE Policies ADD AdditionalNumFranchisesMod integer DEFAULT 0;
ALTER TABLE Policies ADD AdditionalNumFranchises integer DEFAULT 0;
ALTER TABLE Policies ADD NoForeignCorpsInCities boolean DEFAULT 0;
ALTER TABLE Policies ADD NoFranchisesInForeignCities boolean DEFAULT 0;

-- HH Mod
ALTER TABLE Policies ADD ExtraNaturalWonderHappiness integer DEFAULT 0;

-- Policy which initializes N of the best units in a category in capital. Requires 'IncludesOneShotFreeUnits' set to true.
ALTER TABLE Policies ADD BestNumberLandCombatUnitClass integer DEFAULT 0;
ALTER TABLE Policies ADD BestNumberLandRangedUnitClass integer DEFAULT 0;
ALTER TABLE Policies ADD BestNumberSeaCombatUnitClass integer DEFAULT 0;
ALTER TABLE Policies ADD BestNumberSeaRangedUnitClass integer DEFAULT 0;

-- Extra Military Supply from Population
ALTER TABLE Policies ADD ExtraSupplyPerPopulation integer DEFAULT 0;

-- Extra flat supply
ALTER TABLE Policies ADD ExtraSupplyFlat integer DEFAULT 0;

-- Grants a free building to a city when founded
ALTER TABLE Policies ADD NewCityFreeBuilding text REFERENCES BuildingClasses (Type);

-- Grants a free building to all existing and future cities
ALTER TABLE Policies ADD AllCityFreeBuilding text REFERENCES BuildingClasses (Type);

-- Grants a free building to newly founded cities
ALTER TABLE Policies ADD NewFoundCityFreeBuilding text REFERENCES BuildingClasses (Type);

-- Grants a free unit to newly founded cities
ALTER TABLE Policies ADD NewFoundCityFreeUnit text REFERENCES UnitClasses (Type);
ALTER TABLE Policies ADD LandmarksTourismPercent integer DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD ExtraMissionaryStrength integer DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Policies ADD ExtraMissionarySpreads integer DEFAULT 0;

ALTER TABLE Policies ADD GreatDiplomatRateModifier integer DEFAULT 0;

-- Policy Branch - number of unlocked policies (finishers excluded) before branch is unlocked.
ALTER TABLE PolicyBranchTypes ADD NumPolicyRequirement integer DEFAULT -1;

-- A font icon for an ideology
ALTER TABLE PolicyBranchTypes ADD FontIcon text;
