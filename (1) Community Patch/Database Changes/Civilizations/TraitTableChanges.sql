-- TODO: Documentation

-- Bonus XP to Gold-purchased military units, scaling with era.
ALTER TABLE Traits ADD PurchasedUnitsBonusXP integer DEFAULT 0;

-- Trade Route yields no longer scale from distance.
ALTER TABLE Traits ADD IgnoreTradeDistanceScaling boolean DEFAULT 0;

-- Trade Routes can be plundered without being at war
ALTER TABLE Traits ADD CanPlunderWithoutWar boolean DEFAULT 0;

-- No unhappiness from Isolation.
ALTER TABLE Traits ADD NoConnectionUnhappiness boolean DEFAULT 0;

-- can buy boats with faith (need faith cost on units to do so).
ALTER TABLE Traits ADD CanPurchaseNavalUnitsFaith boolean DEFAULT 0;

-- Imports count towards monopolies.
ALTER TABLE Traits ADD ImportsCountTowardsMonopolies boolean DEFAULT 0;

-- No unhappiness from religious strife.
ALTER TABLE Traits ADD IsNoReligiousStrife boolean DEFAULT 0;

-- Extra Wonder Production during Golden Ages.
ALTER TABLE Traits ADD WonderProductionModGA integer DEFAULT 0;

-- TRAIT: Changes the food (times 100) consumed by each non-specialist citizen.
ALTER TABLE Traits ADD NonSpecialistFoodChange integer DEFAULT 0;

-- TRAIT: Annexed City States continue to give yields. --
ALTER TABLE Traits ADD AnnexedCityStatesGiveYields boolean DEFAULT 0;

-- Abnormal scaler. Works for:
---- Trait_SpecialistYieldChanges (specialist yield change x2/x3/x4 in medieval/industrial/atomic eras)
---- FreeSocialPoliciesPerEra in Traits
---- Trait_YieldChangesPerImprovementBuilt (x2/x3/x4 of the bonus in Medieval/Industrial/Atomic)
ALTER TABLE Traits ADD IsOddEraScaler boolean DEFAULT 0;

-- Trait affects capital only. Works for:
---- Trait_YieldChangesPerImprovementBuilt (yield only shows up in capital)
ALTER TABLE Traits ADD IsCapitalOnly boolean DEFAULT 0;

-- No natural religion spread to/from other players' cities
ALTER TABLE Traits ADD NoNaturalReligionSpread boolean DEFAULT 0;

-- No trade routes to player if not already trading w/ you, tourism penalty if not trading (-50%)
ALTER TABLE Traits ADD NoOpenTrade boolean DEFAULT 0;

-- Earn a free building only in your capital as your trait. No tech requirement.
ALTER TABLE Traits ADD FreeCapitalBuilding text REFERENCES Buildings (Type);

-- Combat Bonus vs Higher Pop Civilization.
ALTER TABLE Traits ADD CombatBonusVsHigherPop integer DEFAULT 0;

-- Earn a set number of free buildings. Uses standard 'FreeBuilding' trait (i.e. Carthage). No tech requirement.
ALTER TABLE Traits ADD NumFreeBuildings integer DEFAULT 0;

-- Reduce national pop requirements for national wonders.
ALTER TABLE Traits ADD NationalPopReqModifier integer DEFAULT 0;

-- Adds a tech requirement to the free buildings.
ALTER TABLE Traits ADD FreeBuildingPrereqTech text REFERENCES Technologies (Type);

-- Adds a tech requirement to the free capital building.
ALTER TABLE Traits ADD CapitalFreeBuildingPrereqTech text REFERENCES Technologies (Type);

-- Adds a trait that lets a player conquer a city-state if they can bully it.
ALTER TABLE Traits ADD BullyAnnex boolean DEFAULT 0;

-- Adds a trait that lets a player ignore friends/PtP/allies in bully calculation
ALTER TABLE Traits ADD IgnoreBullyPenalties boolean DEFAULT 0;

-- Adds a trait that makes CS alliances boost the defense of CS cities.
ALTER TABLE Traits ADD AllianceCSDefense integer DEFAULT 0;

-- Adds a trait that makes CS alliances boost the strength of CS units.
ALTER TABLE Traits ADD AllianceCSStrength integer DEFAULT 0;

-- Adds a trait that converts x% of tourism from cities to GAP, where x is the integer below.
ALTER TABLE Traits ADD TourismToGAP integer DEFAULT 0;

-- Adds a trait that converts x% of GROSS GPT to GAP, where x is the interger below.
ALTER TABLE Traits ADD GoldToGAP integer DEFAULT 0;

-- Adds a trait that boosts the value of historic event tourism. 1 = 10%, 2 = 20%, etc.
ALTER TABLE Traits ADD EventTourismBoost integer DEFAULT 0;

-- Adds x# of GP Points to Capital (scaling with era) when you complete a Historic Event.
ALTER TABLE Traits ADD EventGP integer DEFAULT 0;

-- Picks a random Great Engineer, Great Scietist, or Great Merchant Points to Player Cities (scaling with era) when you adopt a new social policy, ideology, or branch.
-- Also grants production, science, or gold corresponding to the selected specialist type (not a type of instant yields).
ALTER TABLE Traits ADD PolicyGEorGM integer DEFAULT 0;

-- Changes City Ranged Strike during Golden Ages
ALTER TABLE Traits ADD GAGarrisonCityRangeStrikeModifier integer DEFAULT 0;

-- Player enters a golden age on a declaration of war, either as attacking or defending
ALTER TABLE Traits ADD GoldenAgeOnWar boolean DEFAULT 0;

-- Is this Civ unable to cancel razing?
ALTER TABLE Traits ADD UnableToCancelRazing boolean DEFAULT 0;

-- Puppet negative modifiers reduced
ALTER TABLE Traits ADD ReducePuppetPenalties integer DEFAULT 0;

-- Player gains a free policy after unlocking x number of technologies from the tech tree.
ALTER TABLE Traits ADD FreePolicyPerXTechs integer DEFAULT 0;

-- Does this Civ get extra damage from multiple attacks on same target?
ALTER TABLE Traits ADD MultipleAttackBonus integer DEFAULT 0;

-- Does this Civ get extra influence from meeting a CS?
ALTER TABLE Traits ADD InfluenceMeetCS integer DEFAULT 0;

-- Civ gets bonuses to monopoly yields
ALTER TABLE Traits ADD MonopolyModFlat integer DEFAULT 0;
ALTER TABLE Traits ADD MonopolyModPercent integer DEFAULT 0;

-- Changes the global median for a Need in all cities by x% - values should be negative to be good!
ALTER TABLE Traits ADD BasicNeedsMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Traits ADD GoldMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Traits ADD ScienceMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Traits ADD CultureMedianModifierGlobal integer DEFAULT 0;
ALTER TABLE Traits ADD ReligiousUnrestModifierGlobal integer DEFAULT 0;

-- Get more tenets when you adopt for first time
ALTER TABLE Traits ADD ExtraTenetsFirstAdoption integer DEFAULT 0;

-- New Traits -- Can Only Select from beliefs unique to the civ
ALTER TABLE Traits ADD UniqueBeliefsOnly boolean DEFAULT 0;

-- New Traits -- Combat Boost from nearby owned NWs. Requires distance value from improvementdistancerange XML
ALTER TABLE Traits ADD CombatBoostNearNaturalWonder boolean DEFAULT 0;

-- New Traits - Free Great Work(s) on Conquest (or Culture Boost if no great works/slots)
ALTER TABLE Traits ADD FreeGreatWorkOnConquest boolean DEFAULT 0;

-- New Traits - Religious Pressure modified based on population
ALTER TABLE Traits ADD PopulationBoostReligion boolean DEFAULT 0;

-- New Traits - Starts with a Pantheon
ALTER TABLE Traits ADD StartsWithPantheon boolean DEFAULT 0;

-- New Traits - May spend first Great Prophet spread action (Found/Enhance/Build Holy Site) without consuming it
ALTER TABLE Traits ADD ProphetFervor boolean DEFAULT 0;

-- New Traits - % of capital culture converted to tourism during a GA
ALTER TABLE Traits ADD TourismGABonus integer DEFAULT 0;

-- New Traits - Additioanl WC vote for every x CS Alliances
ALTER TABLE Traits ADD VotePerXCSAlliance integer DEFAULT 0;

-- New Traits - Golden Age received from favorable peace treaty
ALTER TABLE Traits ADD GoldenAgeFromVictory integer DEFAULT 0;

-- New Traits - Can buy owned plots with gold
ALTER TABLE Traits ADD BuyOwnedTiles boolean DEFAULT 0;

-- New Traits - New Cities start with the religion of the capital
ALTER TABLE Traits ADD NewCitiesStartWithCapitalReligion boolean DEFAULT 0;

-- New Traits - No Foreign Religious Spread in cities or allied CSs
ALTER TABLE Traits ADD NoSpread boolean DEFAULT 0;

-- OBSOLETE: to be removed in VP5.0, replaced by Trait_BonusFromGPBirth
ALTER TABLE Traits ADD XPBonusFromGGBirth integer DEFAULT 0;

-- New Traits - Military x% better at intimidating CSs
ALTER TABLE Traits ADD CSBullyMilitaryStrengthModifier integer DEFAULT 0;

-- New Traits - Gain %x more yields from bullying CSs
ALTER TABLE Traits ADD CSBullyValueModifier integer DEFAULT 0;

-- New Traits - Diplomatic Marriage bonus (Austria UA - CBP)
ALTER TABLE Traits ADD DiplomaticMarriage boolean DEFAULT 0;

-- New Traits - Adoption of Policies/Beliefs/Ideology = free tech
ALTER TABLE Traits ADD IsAdoptionFreeTech boolean DEFAULT 0;

-- New Traits - Extra Growth from WTLKD
ALTER TABLE Traits ADD GrowthBoon integer DEFAULT 0;

-- New Traits - Extra GI yields from WTLKD
ALTER TABLE Traits ADD WLTKDGPImprovementModifier integer DEFAULT 0;

-- New Traits - WLTKD from GP birth
ALTER TABLE Traits ADD GPWLTKD boolean DEFAULT 0;

-- New Traits - WLTKD from GP expansion - global
ALTER TABLE Traits ADD ExpansionWLTKD boolean DEFAULT 0;

-- New Traits - WLTKD from GWs - global
ALTER TABLE Traits ADD GreatWorkWLTKD boolean DEFAULT 0;

-- New Traits - permanent yield decrease at new era, scaling with empire size. Ties to permanent yields XML table.
ALTER TABLE Traits ADD PermanentYieldsDecreaseEveryEra boolean DEFAULT 0;

-- New Traits - Extra Terrain When Conquering a City
ALTER TABLE Traits ADD ExtraConqueredCityTerritoryClaimRange integer DEFAULT 0;

-- New Traits - Extra Terrain When Conquering a City
ALTER TABLE Traits ADD KeepConqueredBuildings boolean DEFAULT 0;

-- New Traits - Extra Terrain Yields IF you have a trade route or city connection crossing tile - this scales with era (making this false causes terrain yields not to be scaled)
ALTER TABLE Traits ADD TradeRouteOnly boolean DEFAULT 0;

-- New Traits - Mountain Movement and City Connections
ALTER TABLE Traits ADD MountainPass boolean DEFAULT 0;

-- % of Total Wonder Production Modifier from traits, policies, beliefs, units, improvements, etc. is added to building production modifier when building non-wonderclass buildings, value of 100 is all, 50 is 50%, etc.
ALTER TABLE Traits ADD WonderProductionModifierToBuilding integer DEFAULT 0;

-- Starting era for GP faith purchases
ALTER TABLE Traits ADD GPFaithPurchaseEra text REFERENCES Eras (Type);

-- Faith purchase % reduction. Negative reduces.
ALTER TABLE Traits ADD FaithCostModifier integer DEFAULT 0;

-- Investment reduction costs -- trait -- negative makes it stronger!
ALTER TABLE Traits ADD InvestmentModifier integer DEFAULT 0;

-- Cities produce % culture instead of growth during WLTKD.
ALTER TABLE Traits ADD WLTKDCultureBoost integer DEFAULT 0;

-- Cities gain x turns of WLTKD because of a GA trigger!
ALTER TABLE Traits ADD WLTKDFromGATurns integer DEFAULT 0;

-- Unhappiness modifier during WLTKD - more negative the better!
ALTER TABLE Traits ADD WLTKDUnhappinessNeedsMod integer DEFAULT 0;

-- Start game with x number of spies
ALTER TABLE Traits ADD StartingSpies integer DEFAULT 0;

-- Modifier to offensive strength of spies
ALTER TABLE Traits ADD SpyOffensiveStrengthModifier integer DEFAULT 0;

-- Boost CS Quest Value
ALTER TABLE Traits ADD MinorQuestYieldModifier integer DEFAULT 0;

-- % culture bonus from conquering cities
ALTER TABLE Traits ADD CultureBonusModifierConquest integer DEFAULT 0;

-- % production bonus in all cities from conquering cities
ALTER TABLE Traits ADD ProductionBonusModifierConquest integer DEFAULT 0;

-- Global City Security Modifier
ALTER TABLE Traits ADD SpySecurityModifier integer DEFAULT 0;

-- Increase Spy Rank power without changing rank enum value
ALTER TABLE Traits ADD SpyExtraRankBonus integer DEFAULT 0;

-- Spy travel/intermediate action rate modifier
ALTER TABLE Traits ADD SpyMoveRateModifier integer DEFAULT 0;

-- GWAM from conquest
ALTER TABLE Traits ADD CityConquestGWAM integer DEFAULT 0;

-- Shared religion tourism modifier, same as the one for policies
ALTER TABLE Traits ADD SharedReligionTourismModifier integer DEFAULT 0;

-- Spawn Best Melee Unit on an Improvement during a DOW
ALTER TABLE Traits ADD BestUnitSpawnOnImpDOW boolean DEFAULT 0;
ALTER TABLE Traits ADD BestUnitImprovement text REFERENCES Improvements (Type);

-- Does this Civ get a GG/GA Rate Modifier bonus from denunciations and wars?
ALTER TABLE Traits ADD GGGARateFromDenunciationsAndWars integer DEFAULT 0;

-- War Weariness Modifier
ALTER TABLE Traits ADD WarWearinessModifier integer DEFAULT 0;

-- War Weariness Modifier
ALTER TABLE Traits ADD EnemyWarWearinessModifier integer DEFAULT 0;

-- Bully Annex % Modifier for Yields for Mongolia UA
ALTER TABLE Traits ADD BullyYieldMultiplierAnnex integer DEFAULT 0;

-- Does this civ get a free Unit.Type on Conquest? Must be able to train it first....
ALTER TABLE Traits ADD FreeUnitOnConquest text REFERENCES Units (Type);

-- Civ gets an influence boost and Great Admiral Points when sending a Trade Route to a minor Civ.
ALTER TABLE Traits ADD TradeRouteMinorInfluenceAP boolean DEFAULT 0;

-- Civ cities gets a production modifier (from 1%) for every specialist that they have.
ALTER TABLE Traits ADD ProdModFromNumSpecialists boolean DEFAULT 0;

-- Player during a Golden Age receives a City Attack modifier definable.
ALTER TABLE Traits ADD ConquestOfTheWorldCityAttack integer DEFAULT 0;

-- Internal trade route yields doubled during Golden Ages, starts a Golden Age when conquering a city and extends if already in Golden Age.
ALTER TABLE Traits ADD ConquestOfTheWorld boolean DEFAULT 0;

-- Trait allows player to have every unit upgraded once tech is reached.
ALTER TABLE Traits ADD FreeUpgrade boolean DEFAULT 0;

-- Trait allows player to have every unit upgraded once tech is reached.
ALTER TABLE Traits ADD VotePerXCSFollowingYourReligion integer DEFAULT 0;

-- Trait allows player a % chance to convert religious units when they enter player's territory.
ALTER TABLE Traits ADD ChanceToConvertReligiousUnits integer DEFAULT 0;

-- Enemy Units gain the "EnemyWarSawPactPromotion" promotio when in your territory or Friendly City States or Player's that follow the same Ideology. Must define "EnemyWarSawPactPromotion" promotion type for this to work (see below).
ALTER TABLE Traits ADD WarsawPact boolean DEFAULT 0;
ALTER TABLE Traits ADD EnemyWarSawPactPromotion text REFERENCES UnitPromotions (Type);

-- Zulu Pikeman Class Upgrade immediately to Impi's upon discovering Impi tech, assuming Impi is not a Pikeman, but a Renaissance Unit.
ALTER TABLE Traits ADD FreeZuluPikemanToImpi boolean DEFAULT 0;

-- Missionaries gain % more strength
ALTER TABLE Traits ADD ExtraMissionaryStrength integer DEFAULT 0;

-- Can send gold internal trade routes; gold yield calculated as if international, culture and science calculated as if allied city state. Only sender city gets the yields.
-- Note: Requires BALANCE_CORE_GOLD_INTERNAL_TRADE_ROUTES in CustomModOptions to be set to 1 (0 by default for performance).
ALTER TABLE Traits ADD CanGoldInternalTradeRoutes boolean DEFAULT 0;

-- Additional trade routes based on the number of owned cities
ALTER TABLE Traits ADD TradeRoutesPerXOwnedCities integer DEFAULT 0;

-- Additional trade routes based on the number of owned vassals
ALTER TABLE Traits ADD TradeRoutesPerXOwnedVassals integer DEFAULT 0;

-- Gifting units to City-States awards Influence per turn while the units remain alive
ALTER TABLE Traits ADD MinorInfluencePerGiftedUnit integer DEFAULT 0;

-- Whoward Tables
ALTER TABLE Traits ADD GGFromBarbarians integer DEFAULT 0;

ALTER TABLE Traits ADD ExtraSupply integer DEFAULT 0;
ALTER TABLE Traits ADD ExtraSupplyPerCity integer DEFAULT 0;
ALTER TABLE Traits ADD ExtraSupplyPerPopulation integer DEFAULT 0;

ALTER TABLE Traits ADD CityAutomatonWorkersChange integer DEFAULT 0;

ALTER TABLE Traits ADD CityWorkingChange integer DEFAULT 0;
ALTER TABLE Traits ADD AnyBelief boolean DEFAULT 0;
ALTER TABLE Traits ADD AlwaysReligion boolean DEFAULT 0;
ALTER TABLE Traits ADD CrossesIce integer DEFAULT 0;

ALTER TABLE Traits ADD ObsoleteBelief text REFERENCES Beliefs (Type);
ALTER TABLE Traits ADD PrereqBelief text REFERENCES Beliefs (Type);

ALTER TABLE Traits ADD ObsoletePolicy text REFERENCES Policies (Type);
ALTER TABLE Traits ADD PrereqPolicy text REFERENCES Policies (Type);

ALTER TABLE Traits ADD SeaTradeRouteRangeBonus integer DEFAULT 0;

-- All cities gain this production modifier for units when empire is unhappy
ALTER TABLE Traits ADD UnhappyUnitProductionMod integer DEFAULT 0;

-- Cities cannot avoid growth
ALTER TABLE Traits ADD NoAvoidGrowth boolean DEFAULT 0;

-- Modifier for city revolution countdown timer when empire is very unhappy
ALTER TABLE Traits ADD RevoltTimerMod integer DEFAULT 0;
-- Artists Open Golden Age to Receive Current Median One-Time Research Points in Researchable Technology
ALTER TABLE Traits ADD ArtistGoldenAgeTechBoost boolean DEFAULT 0;
