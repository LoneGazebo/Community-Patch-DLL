-- TODO: Documentation

ALTER TABLE Improvements ADD AddsFreshWater integer DEFAULT 0;

-- Grants a free valid promotion to a unit when it is on a type of improvement (farm, mine, etc.).
ALTER TABLE Improvements ADD UnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Grants Free Experience when Unit is On Improvement plot (must be owned) on Do Turn.
ALTER TABLE Improvements ADD UnitPlotExperience integer DEFAULT 0;

-- Grants Free Experience when Unit is On Improvement plot during Golden Ages (must be owned) on Do Turn.
ALTER TABLE Improvements ADD GAUnitPlotExperience integer DEFAULT 0;

-- Improvement grants extra moves when unit is on this plot
ALTER TABLE Improvements ADD MovesChange integer DEFAULT 0;

-- Improvement restores all moves when unit is on this plot
ALTER TABLE Improvements ADD RestoreMoves boolean DEFAULT 0;

-- Improvement requires fresh water, coast, or river adjacency to make valid.
ALTER TABLE Improvements ADD WaterAdjacencyMakesValid boolean DEFAULT 0;

-- Improvement connects all resources
ALTER TABLE Improvements ADD ConnectsAllResources boolean DEFAULT 0;

-- Improvement grants a Great Person Point generation percentage bonus.
ALTER TABLE Improvements ADD GreatPersonRateModifier integer DEFAULT 0;

-- Grants resource to improvement
ALTER TABLE Improvements ADD ImprovementResource text REFERENCES Resources (Type);
ALTER TABLE Improvements ADD ImprovementResourceQuantity integer DEFAULT 0;

-- Improvement creates happiness for builder when finished.
ALTER TABLE Improvements ADD HappinessOnConstruction integer DEFAULT 0;

-- Grants obsoletion tech to improvement (tie to build above for AI)
ALTER TABLE Improvements ADD ObsoleteTech text REFERENCES Technologies (Type);

-- Improvement can not be built adjacent to a city
ALTER TABLE Improvements ADD NoAdjacentCity boolean DEFAULT 0;

-- Improvements can be made valid by being adjacent to a city
ALTER TABLE Improvements ADD Cityside boolean DEFAULT 0;

-- Improvements can be made valid by being adjacent to a lake
ALTER TABLE Improvements ADD Lakeside boolean DEFAULT 0;

-- Improvements can be made valid by being adjacent to X of the same improvement
ALTER TABLE Improvements ADD XSameAdjacentMakesValid integer DEFAULT 0;

-- Improvements can be made valid by being on coast terrain
ALTER TABLE Improvements ADD CoastMakesValid boolean DEFAULT 0;

-- Improvements can generate vision for builder x tiles away (radially)
ALTER TABLE Improvements ADD GrantsVisionXTiles integer DEFAULT 0;

-- Improvement spawns a resource in an adjacent tile on completion
ALTER TABLE Improvements ADD SpawnsAdjacentResource text REFERENCES Resources (Type);

--Grants Wonder Production Modifier based on number of City Improvements. Value is % gained.
ALTER TABLE Improvements ADD WonderProductionModifier integer DEFAULT 0;

-- Improvement creates a feature. Will remove the improvement when complete, leaving only the feature.
ALTER TABLE Improvements ADD CreatesFeature text REFERENCES Features (Type);

-- What is the chance to get a random resource? Resource spawned is random and selected based on the various resource boolean tables.
ALTER TABLE Improvements ADD RandResourceChance integer DEFAULT 0;

-- Removes the improvement when built. Useful in combination with CreatesFeature.
ALTER TABLE Improvements ADD RemoveWhenComplete boolean DEFAULT 0;

-- Units that stand on this improvement don't leave it when they attack (like from a city)
ALTER TABLE Improvements ADD NoFollowUp boolean DEFAULT 0;

-- Improvement grants new Ownership if plot is not owned.
ALTER TABLE Improvements ADD NewOwner boolean DEFAULT 0;

-- Improvement grants promotion if plot is owned by the player.
ALTER TABLE Improvements ADD OwnerOnly boolean DEFAULT 1;

ALTER TABLE Improvements ADD ImprovementLeagueVotes integer DEFAULT 0;

ALTER TABLE Improvements ADD IsEmbassy boolean DEFAULT 0;

-- The tile cannot be stolen by any means. Disabled when pillaged.
ALTER TABLE Improvements ADD BlockTileSteal boolean DEFAULT 0;

-- GLOBAL_ALPINE_PASSES
ALTER TABLE Improvements ADD MountainsMakesValid integer DEFAULT 0;

-- GLOBAL_PASSABLE_FORTS
ALTER TABLE Improvements ADD MakesPassable integer DEFAULT 0;

ALTER TABLE Improvements ADD AllowsRebaseTo integer DEFAULT 0;
ALTER TABLE Improvements ADD AllowsAirliftFrom integer DEFAULT 0;
ALTER TABLE Improvements ADD AllowsAirliftTo integer DEFAULT 0;

-- Improvement requires a resource in the tile to be buildable
ALTER TABLE Improvements ADD RequiresResource boolean DEFAULT 0;
