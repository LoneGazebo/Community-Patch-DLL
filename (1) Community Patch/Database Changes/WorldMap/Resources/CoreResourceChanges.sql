-- Default value for new column. Will be changed in VP.
UPDATE Resources SET TechImproveable = TechCityTrade;

-- Iron can appear in any forest
INSERT INTO Resource_FeatureBooleans
	(ResourceType, FeatureType)
VALUES
	('RESOURCE_IRON', 'FEATURE_FOREST');

INSERT INTO Resource_FeatureTerrainBooleans
	(ResourceType, TerrainType)
VALUES
	('RESOURCE_IRON', 'TERRAIN_GRASS'),
	('RESOURCE_IRON', 'TERRAIN_PLAINS'),
	('RESOURCE_IRON', 'TERRAIN_TUNDRA');

--------------------------------------------------------------------------
-- Resource classifications (used in modmods)
-- Resources that don't have graphics are not assigned
--------------------------------------------------------------------------

-- Land resources
UPDATE Resources SET LandResource = 1
WHERE Type IN (
	'RESOURCE_ALUMINUM',
	'RESOURCE_BANANA',
	'RESOURCE_BISON',
	'RESOURCE_CITRUS',
	'RESOURCE_CLOVES',
	'RESOURCE_COAL',
	'RESOURCE_COCOA',
	'RESOURCE_COPPER',
	'RESOURCE_COTTON',
	'RESOURCE_COW',
	'RESOURCE_DEER',
	'RESOURCE_DYE',
	'RESOURCE_FUR',
	'RESOURCE_GEMS',
	'RESOURCE_GOLD',
	'RESOURCE_HORSE',
	'RESOURCE_INCENSE',
	'RESOURCE_IRON',
	'RESOURCE_IVORY',
	'RESOURCE_MARBLE',
	'RESOURCE_NUTMEG',
	'RESOURCE_OIL', -- both land and sea
	'RESOURCE_PEPPER',
	'RESOURCE_SALT',
	'RESOURCE_SHEEP',
	'RESOURCE_SILK',
	'RESOURCE_SILVER',
	'RESOURCE_SPICES',
	'RESOURCE_STONE',
	'RESOURCE_SUGAR',
	'RESOURCE_TRUFFLES',
	'RESOURCE_URANIUM',
	'RESOURCE_WHEAT',
	'RESOURCE_WINE'
);

-- Sea resources
UPDATE Resources SET SeaResource = 1
WHERE Type IN (
	'RESOURCE_CRAB',
	'RESOURCE_FISH',
	'RESOURCE_OIL', -- both sea and land
	'RESOURCE_PEARLS',
	'RESOURCE_WHALE'
);

-- Animal resources
UPDATE Resources SET AnimalResource = 1
WHERE Type IN (
	'RESOURCE_BISON',
	'RESOURCE_COW',
	'RESOURCE_CRAB',
	'RESOURCE_DEER',
	'RESOURCE_FISH',
	'RESOURCE_FUR',
	'RESOURCE_HORSE',
	'RESOURCE_IVORY',
	'RESOURCE_PEARLS',
	'RESOURCE_SHEEP',
	'RESOURCE_SILK',
	'RESOURCE_WHALE'
);

-- Plant resources
UPDATE Resources SET PlantResource = 1
WHERE Type IN (
	'RESOURCE_BANANA',
	'RESOURCE_CITRUS',
	'RESOURCE_CLOVES',
	'RESOURCE_COCOA',
	'RESOURCE_COTTON',
	'RESOURCE_DYE',
	'RESOURCE_INCENSE',
	'RESOURCE_NUTMEG',
	'RESOURCE_PEPPER',
	'RESOURCE_SPICES',
	'RESOURCE_SUGAR',
	'RESOURCE_TRUFFLES',
	'RESOURCE_WHEAT',
	'RESOURCE_WINE'
);

-- Rock resources
UPDATE Resources SET RockResource = 1
WHERE Type IN (
	'RESOURCE_ALUMINUM',
	'RESOURCE_COAL',
	'RESOURCE_COPPER',
	'RESOURCE_GEMS',
	'RESOURCE_GOLD',
	'RESOURCE_IRON',
	'RESOURCE_MARBLE',
	'RESOURCE_SALT',
	'RESOURCE_SILVER',
	'RESOURCE_STONE',
	'RESOURCE_URANIUM'
);
