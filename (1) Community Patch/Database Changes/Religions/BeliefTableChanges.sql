-- TODO: Documentation and sorting

-- Bombard Ranges for cities increase/decrease over default
ALTER TABLE Beliefs ADD BorderGrowthRateIncreaseGlobal integer DEFAULT 0;

-- Belief Tooltip for LUA
ALTER TABLE Beliefs ADD Tooltip text REFERENCES Language_en_US (Tag);

-- Belief requires an improvement on a terrain type to grant its yield.
ALTER TABLE Beliefs ADD RequiresImprovement boolean DEFAULT 0;

-- Belief requires a resource (improved or not) on a terrain type to grant its yield.
ALTER TABLE Beliefs ADD RequiresResource boolean DEFAULT 0;

-- NOTE: THESE TWO (RequiresImprovement and RequiresResource) interact, and can be used to refine belief yields.

-- Belief requires no improvement to grant its yield on a tile.
ALTER TABLE Beliefs ADD RequiresNoImprovement boolean DEFAULT 0;

-- Belief requires no feature to grant its yield on a tile. Ignore the 'improvement' part.
ALTER TABLE Beliefs ADD RequiresNoImprovementFeature boolean DEFAULT 0;

-- NOTE: THESE TWO (RequiresNoImprovement and RequiresNoImprovementFeature) interact, and can be used to refine belief yields.

-- Belief - reduces policy cost of Wonders by 1 for every x cities following religion
ALTER TABLE Beliefs ADD PolicyReductionWonderXFollowerCities integer DEFAULT 0;

-- To help the AI choose a starting pantheon for civs with the trait "StartsWithPantheon": If there are pantheons with AI_GoodStartingPantheon = true, the AI will make a random choice between them
ALTER TABLE Beliefs ADD AI_GoodStartingPantheon boolean DEFAULT 0;

-- Belief - increases pressure from trade routes
ALTER TABLE Beliefs ADD PressureChangeTradeRoute integer DEFAULT 0;

-- New Belief Element -- Assign Belief to Specific Civ only
ALTER TABLE Beliefs ADD CivilizationType text REFERENCES Civilizations (Type);

-- Combat bonus v. other religions in our lands
ALTER TABLE Beliefs ADD CombatVersusOtherReligionOwnLands integer DEFAULT 0;

-- Combat bonus v. other religions in their lands
ALTER TABLE Beliefs ADD CombatVersusOtherReligionTheirLands integer DEFAULT 0;

-- Production modifier for units
ALTER TABLE Beliefs ADD UnitProductionModifier integer DEFAULT 0;

-- Influence with CS from missionary spread
ALTER TABLE Beliefs ADD MissionaryInfluenceCS integer DEFAULT 0;

-- Happiness per known civ with a Pantheon
ALTER TABLE Beliefs ADD HappinessPerPantheon integer DEFAULT 0;

-- Extra Votes from Belief
ALTER TABLE Beliefs ADD ExtraVotes integer DEFAULT 0;

ALTER TABLE Beliefs ADD CityScalerLimiter integer DEFAULT 0;
ALTER TABLE Beliefs ADD FollowerScalerLimiter integer DEFAULT 0;

-- Ignore Policy Requirements (number) for wonders up to a set era
ALTER TABLE Beliefs ADD ReducePolicyRequirements integer DEFAULT 0;

-- Increase yields from friendship/alliance for CS sharing your religion by x%
ALTER TABLE Beliefs ADD CSYieldBonusFromSharedReligion integer DEFAULT 0;

-- Increase happiness from owned foreign spies active in other cities
ALTER TABLE Beliefs ADD HappinessFromForeignSpies integer DEFAULT 0;

-- Decrease inquisitor cost. What it says on the tin ya git.
ALTER TABLE Beliefs ADD InquisitorCostModifier integer DEFAULT 0;
