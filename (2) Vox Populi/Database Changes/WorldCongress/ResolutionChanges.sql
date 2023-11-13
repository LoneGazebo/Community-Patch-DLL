-- Cultural Heritage Sites
UPDATE Resolutions
SET CulturePerWonder = 2
WHERE Type = 'RESOLUTION_CULTURAL_HERITAGE_SITES';

-- Natural Heritage Sites
UPDATE Resolutions
SET CulturePerNaturalWonder = 2
WHERE Type = 'RESOLUTION_NATURAL_HERITAGE_SITES';

-- Historical Landmarks
UPDATE Resolutions
SET
	GreatPersonTileImprovementCulture = 1,
	LandmarkCulture = 1
WHERE Type = 'RESOLUTION_HISTORICAL_LANDMARKS';

-- World's Fair
UPDATE Resolutions
SET TechPrereqAnyMember = 'TECH_INDUSTRIALIZATION'
WHERE Type = 'RESOLUTION_WORLD_FAIR';

-- World Ideology
UPDATE Resolutions
SET
	TechPrereqAnyMember = 'TECH_NUCLEAR_FISSION',
	OtherIdeologyRebellionMod = 2
WHERE Type = 'RESOLUTION_WORLD_IDEOLOGY';

-- International Space Station
UPDATE Resolutions
SET TechPrereqAnyMember = 'TECH_ROCKETRY'
WHERE Type = 'RESOLUTION_INTERNATIONAL_SPACE_STATION';

-- Global Hegemony
UPDATE Resolutions
SET LeadersVoteBonusOnFail = 5
WHERE Type = 'RESOLUTION_DIPLOMATIC_VICTORY';
