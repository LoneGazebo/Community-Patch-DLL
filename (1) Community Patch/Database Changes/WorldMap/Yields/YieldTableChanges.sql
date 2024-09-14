-- Art stuff
ALTER TABLE Yields ADD ColorString text DEFAULT '[COLOR_WHITE]';
ALTER TABLE Yields ADD ImageTexture text DEFAULT 'YieldAtlas.dds';
ALTER TABLE Yields ADD ImageOffset integer DEFAULT 0;

-- City tile yields
ALTER TABLE Yields ADD MinCityFlatFreshWater integer DEFAULT 0;
ALTER TABLE Yields ADD MinCityFlatNoFreshWater integer DEFAULT 0;
ALTER TABLE Yields ADD MinCityHillFreshWater integer DEFAULT 0;
ALTER TABLE Yields ADD MinCityHillNoFreshWater integer DEFAULT 0;
ALTER TABLE Yields ADD MinCityMountainFreshWater integer DEFAULT 0;
ALTER TABLE Yields ADD MinCityMountainNoFreshWater integer DEFAULT 0;
