--DO NOT MODIFY THIS--
CREATE TABLE IF NOT EXISTS COMMUNITY (
Type text,
Value variant default 0
);

CREATE TABLE IF NOT EXISTS Cep(Type text NOT NULL UNIQUE, Value);

CREATE TABLE IF NOT EXISTS
	Resource_TerrainWeights (
	ResourceType	text REFERENCES Resources(Type),
	TerrainType		text REFERENCES Terrains(Type),
	FeatureType		text default NULL,
	PlotType		text default NULL,
	Freshwater		boolean default false,
	NotLake			boolean default false,
	Weight			variant default 1
);
--DO NOT MODIFY ABOVE--