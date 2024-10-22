----------------------------------------------------------
-- Below 2 columns only function when:
-- 1. EVENTS_CITY_BOMBARD option is on
-- 2. CORE_NO_RANGED_ATTACK_FROM_CITIES and BALANCE_CORE_BOMBARD_RANGE_BUILDINGS options are off
-- It's written in Lua, and only counts the highest range tech.
-- e.g. if tech A has BombardRange = 2, BombardIndirect = 1 and tech B has BombardRange = 3, BombardIndirect = 0,
-- having both techs will give cities range 3 without indirect fire.
--
-- TODO: integrate GlobalCityBombardRange.lua into DLL so both techs and buildings can grant range and indirect fire,
-- and make range stack additively.
----------------------------------------------------------

-- Set range of city ranged strike to this
ALTER TABLE Technologies ADD BombardRange integer DEFAULT 0;

-- Actually a boolean - set to 1 to grant the indirect fire ability to cities
ALTER TABLE Technologies ADD BombardIndirect integer DEFAULT 0;

----------------------------------------------------------
-- 1 move cost to embark from/disembark into cities or canal tiles
ALTER TABLE Technologies ADD CityLessEmbarkCost boolean DEFAULT 0;

-- 0.1 move cost to embark from/disembark into cities or canal tiles. Overrides CityLessEmbarkCost.
ALTER TABLE Technologies ADD CityNoEmbarkCost boolean DEFAULT 0;

-- Happiness to every city
ALTER TABLE Technologies ADD Happiness integer DEFAULT 0;

-- Enables founding corporations
ALTER TABLE Technologies ADD CorporationsEnabled boolean DEFAULT 0;

-- Enables gaining vassals
ALTER TABLE Technologies ADD VassalageTradingAllowed boolean DEFAULT 0;

-- Extra automatons for every city
-- Automaton works the same as citizen, but doesn't count towards population
ALTER TABLE Technologies ADD CityAutomatonWorkersChange integer DEFAULT 0;

-- Extra working range for every city
-- Note that working range caps at MAX_CITY_RADIUS
ALTER TABLE Technologies ADD CityWorkingChange integer DEFAULT 0;
