-- Create buildings that cannot have access to fresh water.

ALTER TABLE Buildings ADD COLUMN 'IsNoWater' boolean default false;

-- Belief requires an improvement on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresImprovement' boolean default false;

-- Belief requires a resource (improved or not) on a terrain type to grant its yield.

ALTER TABLE Beliefs ADD COLUMN 'RequiresResource' boolean default false;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE Eras ADD COLUMN 'StartingMinorDefenseUnits' integer default 0;

-- Give CSs defensive units at the beginning of the game.

ALTER TABLE HandicapInfos ADD COLUMN 'StartingMinorDefenseUnits' integer default 0;

-- Earn a free building only in your capital as your trait. No tech requirement.

ALTER TABLE Traits ADD COLUMN 'FreeCapitalBuilding' text default NULL;

-- Earn a set number of free buildings. Uses standard 'FreeBuilding' trait (i.e. Carthage). No tech requirement.

ALTER TABLE Traits ADD COLUMN 'NumFreeBuildings' integer default 0;

-- Adds a tech requirement to the free buildings.

ALTER TABLE Traits ADD COLUMN 'FreeBuildingPrereqTech' text default NULL;

-- Adds a tech requirement to the free capital building.

ALTER TABLE Traits ADD COLUMN 'CapitalFreeBuildingPrereqTech' text default NULL;

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