-- Ancestor Worship
INSERT INTO Belief_YieldPerPop
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_ANCESTOR_WORSHIP', 'YIELD_FAITH', 4);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_ANCESTOR_WORSHIP', 'BUILDINGCLASS_GROVE', 'YIELD_CULTURE', 1),
	('BELIEF_ANCESTOR_WORSHIP', 'BUILDINGCLASS_GROVE', 'YIELD_FAITH', 2);

-- Tears of the Gods (now God of All Creation)
INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_TEARS_OF_GODS', 'BUILDINGCLASS_PALACE', 'YIELD_CULTURE', 1),
	('BELIEF_TEARS_OF_GODS', 'BUILDINGCLASS_MONUMENT', 'YIELD_FAITH', 1);

INSERT INTO Belief_YieldFromKnownPantheons
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_TEARS_OF_GODS', 'YIELD_FOOD', 50),
	('BELIEF_TEARS_OF_GODS', 'YIELD_PRODUCTION', 50),
	('BELIEF_TEARS_OF_GODS', 'YIELD_GOLD', 50),
	('BELIEF_TEARS_OF_GODS', 'YIELD_SCIENCE', 50);

-- Messenger of the Gods (now God of Commerce)
INSERT INTO Belief_YieldChangeTradeRoute
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MESSENGER_GODS', 'YIELD_GOLD', 2),
	('BELIEF_MESSENGER_GODS', 'YIELD_FAITH', 2);

INSERT INTO Belief_YieldPerActiveTR
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MESSENGER_GODS', 'YIELD_GOLD', 2),
	('BELIEF_MESSENGER_GODS', 'YIELD_FAITH', 2);

INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
VALUES
	('BELIEF_MESSENGER_GODS', 'GREATPERSON_MERCHANT', 2);

-- Stone Circles (now God of Craftsmen)
INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_STONE_CIRCLES', 'IMPROVEMENT_QUARRY', 'YIELD_PRODUCTION', 1),
	('BELIEF_STONE_CIRCLES', 'IMPROVEMENT_QUARRY', 'YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_STONE_CIRCLES', 'BUILDINGCLASS_PALACE', 'YIELD_SCIENCE', 2),
	('BELIEF_STONE_CIRCLES', 'BUILDINGCLASS_PALACE', 'YIELD_FAITH', 2),
	('BELIEF_STONE_CIRCLES', 'BUILDINGCLASS_STONE_WORKS', 'YIELD_CULTURE', 1),
	('BELIEF_STONE_CIRCLES', 'BUILDINGCLASS_STONE_WORKS', 'YIELD_FAITH', 2);

-- Religious Settlements (now God of the Expanse)
UPDATE Beliefs
SET
	PlotCultureCostModifier = 0,
	BorderGrowthRateIncreaseGlobal = 25
WHERE Type = 'BELIEF_RELIGIOUS_SETTLEMENTS';

INSERT INTO Belief_YieldPerBorderGrowth
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RELIGIOUS_SETTLEMENTS', 'YIELD_PRODUCTION', 15),
	('BELIEF_RELIGIOUS_SETTLEMENTS', 'YIELD_FAITH', 25);

-- God of Fire
UPDATE Beliefs SET RequiresResource = 1 WHERE Type = 'BELIEF_EARTH_MOTHER';

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_EARTH_MOTHER', 'IMPROVEMENT_MINE', 'YIELD_PRODUCTION', 1),
	('BELIEF_EARTH_MOTHER', 'IMPROVEMENT_MINE', 'YIELD_CULTURE', 1),
	('BELIEF_EARTH_MOTHER', 'IMPROVEMENT_MINE', 'YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_EARTH_MOTHER', 'BUILDINGCLASS_FORGE', 'YIELD_FAITH', 2);

-- God of the Open Sky
UPDATE Beliefs SET RequiresNoImprovementFeature = 1 WHERE Type = 'BELIEF_OPEN_SKY';

INSERT INTO Belief_CityYieldPerXTerrainTimes100
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_OPEN_SKY', 'TERRAIN_GRASS', 'YIELD_CULTURE', 50),
	('BELIEF_OPEN_SKY', 'TERRAIN_GRASS', 'YIELD_FAITH', 50),
	('BELIEF_OPEN_SKY', 'TERRAIN_PLAINS', 'YIELD_CULTURE', 50),
	('BELIEF_OPEN_SKY', 'TERRAIN_PLAINS', 'YIELD_FAITH', 50);

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_OPEN_SKY', 'IMPROVEMENT_PASTURE', 'YIELD_GOLD', 3),
	('BELIEF_OPEN_SKY', 'IMPROVEMENT_PASTURE', 'YIELD_FAITH', 1);

-- God of the Sea
INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_GOD_SEA', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_PRODUCTION', 1),
	('BELIEF_GOD_SEA', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_FAITH', 1);

INSERT INTO Belief_CoastalCityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_GOD_SEA', 'YIELD_FOOD', 2),
	('BELIEF_GOD_SEA', 'YIELD_FAITH', 1);

INSERT INTO Belief_FeatureYieldChanges
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_GOD_SEA', 'FEATURE_ATOLL', 'YIELD_PRODUCTION', 1),
	('BELIEF_GOD_SEA', 'FEATURE_ATOLL', 'YIELD_FAITH', 1);

-- Dance of the Aurora (now God of the Stars and Sky)
UPDATE Beliefs
SET RequiresResource = 1
WHERE Type = 'BELIEF_DANCE_AURORA';

INSERT INTO Belief_TerrainYieldChanges
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_DANCE_AURORA', 'TERRAIN_TUNDRA', 'YIELD_FOOD', 1),
	('BELIEF_DANCE_AURORA', 'TERRAIN_TUNDRA', 'YIELD_CULTURE', 1),
	('BELIEF_DANCE_AURORA', 'TERRAIN_TUNDRA', 'YIELD_FAITH', 1),
	('BELIEF_DANCE_AURORA', 'TERRAIN_SNOW', 'YIELD_FOOD', 1),
	('BELIEF_DANCE_AURORA', 'TERRAIN_SNOW', 'YIELD_CULTURE', 1),
	('BELIEF_DANCE_AURORA', 'TERRAIN_SNOW', 'YIELD_FAITH', 1);

-- Sun God (now God of the Sun)
UPDATE Beliefs SET RequiresResource = 1 WHERE Type = 'BELIEF_SUN_GOD';

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_SUN_GOD', 'IMPROVEMENT_FARM', 'YIELD_FOOD', 1),
	('BELIEF_SUN_GOD', 'IMPROVEMENT_FARM', 'YIELD_SCIENCE', 1),
	('BELIEF_SUN_GOD', 'IMPROVEMENT_FARM', 'YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_SUN_GOD', 'BUILDINGCLASS_GRANARY', 'YIELD_GOLD', 2),
	('BELIEF_SUN_GOD', 'BUILDINGCLASS_GRANARY', 'YIELD_FAITH', 2);

-- God of War
UPDATE Beliefs
SET
	MaxDistance = 0,
	FaithFromKills = 175
WHERE Type = 'BELIEF_GOD_WAR';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_GOD_WAR', 'BUILDINGCLASS_BARRACKS', 'YIELD_PRODUCTION', 1),
	('BELIEF_GOD_WAR', 'BUILDINGCLASS_BARRACKS', 'YIELD_FAITH', 2);

-- God-King
INSERT INTO Belief_YieldPerXFollowers
	(BeliefType, YieldType, PerXFollowers)
VALUES
	('BELIEF_GOD_KING', 'YIELD_GOLD', 5),
	('BELIEF_GOD_KING', 'YIELD_SCIENCE', 5),
	('BELIEF_GOD_KING', 'YIELD_CULTURE', 5),
	('BELIEF_GOD_KING', 'YIELD_FAITH', 5),
	('BELIEF_GOD_KING', 'YIELD_GOLDEN_AGE_POINTS', 5);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_GOD_KING', 'BUILDINGCLASS_PALACE', 'YIELD_PRODUCTION', 2),
	('BELIEF_GOD_KING', 'BUILDINGCLASS_PALACE', 'YIELD_FAITH', 2);

-- Monuments to the Gods (now Goddess of Beauty)
UPDATE Beliefs
SET
	WonderProductionModifier = 0,
	ObsoleteEra = NULL
WHERE Type = 'BELIEF_MONUMENT_GODS';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_MONUMENT_GODS', 'BUILDINGCLASS_PALACE', 'YIELD_FAITH', 2);

INSERT INTO Belief_YieldChangeWorldWonder
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MONUMENT_GODS', 'YIELD_FAITH', 2);

INSERT INTO Belief_GreatWorkYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MONUMENT_GODS', 'YIELD_FAITH', 1);

INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
VALUES
	('BELIEF_MONUMENT_GODS', 'GREATPERSON_ARTIST', 2),
	('BELIEF_MONUMENT_GODS', 'GREATPERSON_ENGINEER', 2);

-- Goddess of Festivals
INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_GOD_FESTIVALS', 'BUILDINGCLASS_PALACE', 'YIELD_FAITH', 1);

INSERT INTO Belief_YieldPerLux
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_GOD_FESTIVALS', 'YIELD_GOLD', 3),
	('BELIEF_GOD_FESTIVALS', 'YIELD_CULTURE', 1),
	('BELIEF_GOD_FESTIVALS', 'YIELD_FAITH', 1);

-- One With Nature (now Goddess of Nature)
INSERT INTO Belief_YieldChangeNaturalWonder
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_ONE_WITH_NATURE', 'YIELD_CULTURE', 2),
	('BELIEF_ONE_WITH_NATURE', 'YIELD_FAITH', 3);

INSERT INTO Belief_CityYieldPerXTerrainTimes100 -- Special: mountains don't need to be worked, but yields are capped at population
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_ONE_WITH_NATURE', 'TERRAIN_MOUNTAIN', 'YIELD_FOOD', 50),
	('BELIEF_ONE_WITH_NATURE', 'TERRAIN_MOUNTAIN', 'YIELD_GOLD', 50),
	('BELIEF_ONE_WITH_NATURE', 'TERRAIN_MOUNTAIN', 'YIELD_FAITH', 50);

-- Goddess of Protection
UPDATE Beliefs
SET
	CityRangeStrikeModifier = 0,
	FriendlyHealChange = 10
WHERE Type = 'BELIEF_GODDESS_STRATEGY';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_GODDESS_STRATEGY', 'BUILDINGCLASS_PALACE', 'YIELD_FAITH', 3),
	('BELIEF_GODDESS_STRATEGY', 'BUILDINGCLASS_WALLS', 'YIELD_CULTURE', 2),
	('BELIEF_GODDESS_STRATEGY', 'BUILDINGCLASS_WALLS', 'YIELD_FAITH', 2);

-- Sacred Waters (now Goddess of Purity)
INSERT INTO Belief_FeatureYieldChanges
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_SACRED_WATERS', 'FEATURE_MARSH', 'YIELD_FOOD', 1),
	('BELIEF_SACRED_WATERS', 'FEATURE_MARSH', 'YIELD_PRODUCTION', 1),
	('BELIEF_SACRED_WATERS', 'FEATURE_MARSH', 'YIELD_FAITH', 1);

INSERT INTO Belief_LakePlotYield
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_SACRED_WATERS', 'YIELD_FOOD', 1),
	('BELIEF_SACRED_WATERS', 'YIELD_FAITH', 1);

-- Sacred Path (now Goddess of Renewal)
INSERT INTO Belief_CityYieldPerXFeatureTimes100
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_SACRED_PATH', 'FEATURE_FOREST', 'YIELD_SCIENCE', 50),
	('BELIEF_SACRED_PATH', 'FEATURE_FOREST', 'YIELD_CULTURE', 50),
	('BELIEF_SACRED_PATH', 'FEATURE_FOREST', 'YIELD_FAITH', 50),
	('BELIEF_SACRED_PATH', 'FEATURE_JUNGLE', 'YIELD_SCIENCE', 50),
	('BELIEF_SACRED_PATH', 'FEATURE_JUNGLE', 'YIELD_CULTURE', 50),
	('BELIEF_SACRED_PATH', 'FEATURE_JUNGLE', 'YIELD_FAITH', 50);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_SACRED_PATH', 'BUILDINGCLASS_MARKET', 'YIELD_FOOD', 2),
	('BELIEF_SACRED_PATH', 'BUILDINGCLASS_MARKET', 'YIELD_SCIENCE', 2);

-- Oral Tradition (now Goddess of Springtime)
INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_ORAL_TRADITION', 'IMPROVEMENT_PLANTATION', 'YIELD_FOOD', 1),
	('BELIEF_ORAL_TRADITION', 'IMPROVEMENT_PLANTATION', 'YIELD_GOLD', 1),
	('BELIEF_ORAL_TRADITION', 'IMPROVEMENT_PLANTATION', 'YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_ORAL_TRADITION', 'BUILDINGCLASS_HERBALIST', 'YIELD_SCIENCE', 1),
	('BELIEF_ORAL_TRADITION', 'BUILDINGCLASS_HERBALIST', 'YIELD_FAITH', 2);

-- Fertility Rites (now Goddess of the Home)
UPDATE Beliefs
SET CityGrowthModifier = 25
WHERE Type = 'BELIEF_FERTILITY_RITES';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_FERTILITY_RITES', 'BUILDINGCLASS_SHRINE', 'YIELD_FOOD', 1),
	('BELIEF_FERTILITY_RITES', 'BUILDINGCLASS_SHRINE', 'YIELD_FAITH', 1);

INSERT INTO Belief_YieldPerConstruction
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_FERTILITY_RITES', 'YIELD_FOOD', 8),
	('BELIEF_FERTILITY_RITES', 'YIELD_FAITH', 8);

-- Goddess of the Hunt
INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_GODDESS_HUNT', 'IMPROVEMENT_CAMP', 'YIELD_GOLD', 1),
	('BELIEF_GODDESS_HUNT', 'IMPROVEMENT_CAMP', 'YIELD_CULTURE', 1),
	('BELIEF_GODDESS_HUNT', 'IMPROVEMENT_CAMP', 'YIELD_FAITH', 1);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_GODDESS_HUNT', 'BUILDINGCLASS_LODGE', 'YIELD_FOOD', 2);

-- Religious Idols (now Goddess of Wisdom)
INSERT INTO Belief_CityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_FORMAL_LITURGY', 'YIELD_SCIENCE', 1),
	('BELIEF_FORMAL_LITURGY', 'YIELD_FAITH', 1);

INSERT INTO Belief_YieldChangeAnySpecialist
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_FORMAL_LITURGY', 'YIELD_SCIENCE', 2),
	('BELIEF_FORMAL_LITURGY', 'YIELD_FAITH', 2);

INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
VALUES
	('BELIEF_FORMAL_LITURGY', 'GREATPERSON_SCIENTIST', 2);

-- Desert Folklore (now Spirit of the Desert)
UPDATE Beliefs
SET RequiresResource = 1
WHERE Type = 'BELIEF_DESERT_FOLKLORE';

INSERT INTO Belief_TerrainYieldChanges
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_DESERT_FOLKLORE', 'TERRAIN_DESERT', 'YIELD_PRODUCTION', 1),
	('BELIEF_DESERT_FOLKLORE', 'TERRAIN_DESERT', 'YIELD_GOLD', 1),
	('BELIEF_DESERT_FOLKLORE', 'TERRAIN_DESERT', 'YIELD_FAITH', 1);

INSERT INTO Belief_FeatureYieldChanges
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_DESERT_FOLKLORE', 'FEATURE_OASIS', 'YIELD_FOOD', 3);

-- Goddess of Love (now Tutelary Gods)
UPDATE Beliefs
SET
	MinPopulation = 3,
	HappinessPerCity = 0
WHERE Type = 'BELIEF_GODDESS_LOVE';

INSERT INTO Belief_CityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_GODDESS_LOVE', 'YIELD_PRODUCTION', 3),
	('BELIEF_GODDESS_LOVE', 'YIELD_GOLD', 2),
	('BELIEF_GODDESS_LOVE', 'YIELD_FAITH', 1);

INSERT INTO Belief_SpecialistYieldChanges
	(BeliefType, SpecialistType, YieldType, Yield)
VALUES
	('BELIEF_GODDESS_LOVE', 'SPECIALIST_ENGINEER', 'YIELD_PRODUCTION', 1),
	('BELIEF_GODDESS_LOVE', 'SPECIALIST_ENGINEER', 'YIELD_FAITH', 1);

-- Celtic unique pantheons (common Ceilidh Hall boosts)
INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_MORRIGAN', 'BUILDINGCLASS_CIRCUS', 'YIELD_GREAT_GENERAL_POINTS', 2),
	('BELIEF_EPONA', 'BUILDINGCLASS_CIRCUS', 'YIELD_CULTURE_LOCAL', 5),
	('BELIEF_NUADA', 'BUILDINGCLASS_CIRCUS', 'YIELD_GOLD', 5),
	('BELIEF_CERNUNNOS', 'BUILDINGCLASS_CIRCUS', 'YIELD_CULTURE', 2),
	('BELIEF_LUGH', 'BUILDINGCLASS_CIRCUS', 'YIELD_PRODUCTION', 3),
	('BELIEF_RHIANNON', 'BUILDINGCLASS_CIRCUS', 'YIELD_GOLDEN_AGE_POINTS', 5),
	('BELIEF_MANANNAN', 'BUILDINGCLASS_CIRCUS', 'YIELD_GREAT_ADMIRAL_POINTS', 2),
	('BELIEF_OGMA', 'BUILDINGCLASS_CIRCUS', 'YIELD_SCIENCE', 5),
	('BELIEF_DAGDA', 'BUILDINGCLASS_CIRCUS', 'YIELD_FOOD', 5),
	('BELIEF_CAILLEACH', 'BUILDINGCLASS_CIRCUS', 'YIELD_TOURISM', 5);

INSERT INTO Belief_BuildingClassHappiness
	(BeliefType, BuildingClassType, Happiness)
VALUES
	('BELIEF_BRAN', 'BUILDINGCLASS_CIRCUS', 2);

-- Morrigan, the Harbinger
INSERT INTO Belief_YieldFromKills
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MORRIGAN', 'YIELD_GOLD', 200),
	('BELIEF_MORRIGAN', 'YIELD_CULTURE', 200),
	('BELIEF_MORRIGAN', 'YIELD_GOLDEN_AGE_POINTS', 200);

INSERT INTO Belief_YieldFromPillageGlobal
	(BeliefType, YieldType, Yield, IsEraScaling)
VALUES
	('BELIEF_MORRIGAN', 'YIELD_GOLD', 20, 1),
	('BELIEF_MORRIGAN', 'YIELD_CULTURE', 20, 1),
	('BELIEF_MORRIGAN', 'YIELD_GOLDEN_AGE_POINTS', 20, 1);

-- Epona, the Great Mare
INSERT INTO Belief_CityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_EPONA', 'YIELD_CULTURE_LOCAL', 3);

INSERT INTO Belief_YieldPerBorderGrowth
	(BeliefType, YieldType, Yield, IsEraScaling)
VALUES
	('BELIEF_EPONA', 'YIELD_FOOD', 8, 1),
	('BELIEF_EPONA', 'YIELD_PRODUCTION', 8, 1),
	('BELIEF_EPONA', 'YIELD_SCIENCE', 8, 1),
	('BELIEF_EPONA', 'YIELD_CULTURE', 8, 1);

-- Nuada, the Silver-Handed
INSERT INTO Belief_YieldFromWLTKD
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 15),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 15);

INSERT INTO Belief_YieldPerLux
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 3),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 3),
	('BELIEF_NUADA', 'YIELD_GOLDEN_AGE_POINTS', 3);

INSERT INTO Belief_YieldPerActiveTR
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_NUADA', 'YIELD_GOLD', 3),
	('BELIEF_NUADA', 'YIELD_SCIENCE', 3),
	('BELIEF_NUADA', 'YIELD_GOLDEN_AGE_POINTS', 3);

-- Cernunnos, the Horned Stag
INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_PLANTATION', 'YIELD_SCIENCE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_PLANTATION', 'YIELD_CULTURE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_CAMP', 'YIELD_SCIENCE', 1),
	('BELIEF_CERNUNNOS', 'IMPROVEMENT_CAMP', 'YIELD_CULTURE', 1);

INSERT INTO Belief_FeatureYieldChanges
	(BeliefType, FeatureType, YieldType, Yield)
VALUES
	('BELIEF_CERNUNNOS', 'FEATURE_FOREST', 'YIELD_FOOD', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_FOREST', 'YIELD_GOLD', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_JUNGLE', 'YIELD_FOOD', 1),
	('BELIEF_CERNUNNOS', 'FEATURE_JUNGLE', 'YIELD_PRODUCTION', 1);

-- Lugh, the Skilled One
UPDATE Beliefs SET WonderProductionModifier = 10 WHERE Type = 'BELIEF_LUGH';

INSERT INTO Belief_YieldChangeAnySpecialist
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_LUGH', 'YIELD_FOOD', 2),
	('BELIEF_LUGH', 'YIELD_GOLD', 2),
	('BELIEF_LUGH', 'YIELD_SCIENCE', 2),
	('BELIEF_LUGH', 'YIELD_CULTURE', 2);

INSERT INTO Belief_YieldChangeWorldWonder
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_LUGH', 'YIELD_CULTURE', 2),
	('BELIEF_LUGH', 'YIELD_TOURISM', 2);

-- Rhiannon, the Sovereign
INSERT INTO Belief_YieldPerFollowingCity
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_RHIANNON', 'YIELD_CULTURE', 2);

UPDATE Beliefs
SET RequiresResource = 1
WHERE Type = 'BELIEF_RHIANNON';

UPDATE Beliefs
SET RequiresImprovement = 1
WHERE Type = 'BELIEF_RHIANNON';

CREATE TEMP TABLE Helper (
	YieldType TEXT
);

INSERT INTO Helper
VALUES
	('YIELD_PRODUCTION'),
	('YIELD_GOLD');

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
SELECT DISTINCT
	'BELIEF_RHIANNON', a.ImprovementType, b.YieldType, 1
FROM Improvement_ResourceTypes a, Helper b;

DROP TABLE Helper;

-- Manannan, Son of the Sea
INSERT INTO Belief_CoastalCityYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_MANANNAN', 'YIELD_FOOD', 3),
	('BELIEF_MANANNAN', 'YIELD_PRODUCTION', 3),
	('BELIEF_MANANNAN', 'YIELD_GOLD', 3);

INSERT INTO Belief_TerrainYieldChanges
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_MANANNAN', 'TERRAIN_COAST', 'YIELD_PRODUCTION', 1),
	('BELIEF_MANANNAN', 'TERRAIN_OCEAN', 'YIELD_PRODUCTION', 1);

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_MANANNAN', 'IMPROVEMENT_FISHING_BOATS', 'YIELD_GOLD', 1);

-- Ogma, the Learned
INSERT INTO Belief_YieldPerPop
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_OGMA', 'YIELD_SCIENCE', 4);

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_OGMA', 'BUILDINGCLASS_PALACE', 'YIELD_SCIENCE', 3),
	('BELIEF_OGMA', 'BUILDINGCLASS_PALACE', 'YIELD_CULTURE', 3);

INSERT INTO Belief_GreatWorkYieldChanges
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_OGMA', 'YIELD_SCIENCE', 1),
	('BELIEF_OGMA', 'YIELD_CULTURE', 1);

INSERT INTO Belief_GreatPersonPoints
	(BeliefType, GreatPersonType, Value)
VALUES
	('BELIEF_OGMA', 'GREATPERSON_ARTIST', 3),
	('BELIEF_OGMA', 'GREATPERSON_SCIENTIST', 3);

-- Dagda, the All-Father
UPDATE Beliefs
SET
	CityGrowthModifier = 25,
	HappinessPerCity = 1
WHERE Type = 'BELIEF_DAGDA';

INSERT INTO Belief_YieldPerBirth
	(BeliefType, YieldType, Yield)
VALUES
	('BELIEF_DAGDA', 'YIELD_PRODUCTION', 12),
	('BELIEF_DAGDA', 'YIELD_GOLD', 12),
	('BELIEF_DAGDA', 'YIELD_CULTURE', 12);

-- Bran, the Sleeping Guardian
UPDATE Beliefs
SET
	FriendlyHealChange = 10,
	CityRangeStrikeModifier = 25
WHERE Type = 'BELIEF_BRAN';

INSERT INTO Belief_BuildingClassYieldChanges
	(BeliefType, BuildingClassType, YieldType, YieldChange)
VALUES
	('BELIEF_BRAN', 'BUILDINGCLASS_PALACE', 'YIELD_PRODUCTION', 1),
	('BELIEF_BRAN', 'BUILDINGCLASS_PALACE', 'YIELD_GOLD', 1),
	('BELIEF_BRAN', 'BUILDINGCLASS_PALACE', 'YIELD_SCIENCE', 1),
	('BELIEF_BRAN', 'BUILDINGCLASS_PALACE', 'YIELD_CULTURE', 1);

INSERT INTO Belief_YieldPerXFollowers
	(BeliefType, YieldType, PerXFollowers)
VALUES
	('BELIEF_BRAN', 'YIELD_PRODUCTION', 4),
	('BELIEF_BRAN', 'YIELD_GOLD', 4),
	('BELIEF_BRAN', 'YIELD_SCIENCE', 4),
	('BELIEF_BRAN', 'YIELD_CULTURE', 4);

-- Cailleach, the Queen of Winter
UPDATE Beliefs SET RequiresResource = 1 WHERE Type = 'BELIEF_CAILLEACH';

-- Note: if you have the RequiresResources flag set, the Times100 table ignores it for tundra, but the YieldChanges table obeys it
INSERT INTO Belief_CityYieldPerXTerrainTimes100
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_CAILLEACH', 'TERRAIN_TUNDRA', 'YIELD_PRODUCTION', 50),
	('BELIEF_CAILLEACH', 'TERRAIN_TUNDRA', 'YIELD_FOOD', 50);

INSERT INTO Belief_TerrainYieldChanges
	(BeliefType, TerrainType, YieldType, Yield)
VALUES
	('BELIEF_CAILLEACH', 'TERRAIN_SNOW', 'YIELD_FOOD', 1),
	('BELIEF_CAILLEACH', 'TERRAIN_SNOW', 'YIELD_PRODUCTION', 1),
	('BELIEF_CAILLEACH', 'TERRAIN_SNOW', 'YIELD_SCIENCE', 1),
	('BELIEF_CAILLEACH', 'TERRAIN_SNOW', 'YIELD_CULTURE', 1);

INSERT INTO Belief_ImprovementYieldChanges
	(BeliefType, ImprovementType, YieldType, Yield)
VALUES
	('BELIEF_CAILLEACH', 'IMPROVEMENT_QUARRY', 'YIELD_GOLD', 1),
	('BELIEF_CAILLEACH', 'IMPROVEMENT_QUARRY', 'YIELD_CULTURE', 1),
	('BELIEF_CAILLEACH', 'IMPROVEMENT_MINE', 'YIELD_GOLD', 1),
	('BELIEF_CAILLEACH', 'IMPROVEMENT_MINE', 'YIELD_CULTURE', 1);

-- help the AI select a good pantheon belief for India
UPDATE Beliefs SET AI_GoodStartingPantheon = 1 WHERE Type IN ('BELIEF_ANCESTOR_WORSHIP', 'BELIEF_MESSENGER_GODS', 'BELIEF_GOD_KING', 'BELIEF_GODDESS_LOVE');