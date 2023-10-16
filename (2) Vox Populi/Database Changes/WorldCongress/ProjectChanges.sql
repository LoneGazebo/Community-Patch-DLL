-- Treasure Fleet
UPDATE LeagueProjects
SET CostPerPlayer = 1500
WHERE Type = 'LEAGUE_PROJECT_TREASURE_FLEET';

UPDATE LeagueProjectRewards
SET
	FreeUnitClass = 'UNITCLASS_FRIGATE',
	GoldenAgePoints = 300
WHERE Type = 'LEAGUE_PROJECT_REWARD_TREASURE_FLEET_1';

UPDATE LeagueProjectRewards
SET
	FreeUnitClass = 'UNITCLASS_GREAT_ADMIRAL',
	BaseFreeUnits = 3
WHERE Type = 'LEAGUE_PROJECT_REWARD_TREASURE_FLEET_2';

UPDATE LeagueProjectRewards
SET Building = 'BUILDING_GRAND_CANAL'
WHERE Type = 'LEAGUE_PROJECT_REWARD_TREASURE_FLEET_3';

-- Global Wargames
UPDATE LeagueProjects
SET CostPerPlayer = 3300
WHERE Type = 'LEAGUE_PROJECT_WARGAMES';

UPDATE LeagueProjectRewards
SET
	FreeUnitClass = 'UNITCLASS_INFANTRY',
	BaseFreeUnits = 3
WHERE Type = 'LEAGUE_PROJECT_REWARD_WARGAMES_1';

UPDATE LeagueProjectRewards
SET
	BaseFreeUnits = 4,
	AttackBonusTurns = 30
WHERE Type = 'LEAGUE_PROJECT_REWARD_WARGAMES_2';

UPDATE LeagueProjectRewards
SET Building = 'BUILDING_MENIN_GATE'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WARGAMES_3';

-- World's Fair
UPDATE LeagueProjects
SET CostPerPlayer = 3750
WHERE Type = 'LEAGUE_PROJECT_WORLD_FAIR';

UPDATE LeagueProjectRewards
SET
	GoldenAgePoints = 0,
	FreeSocialPolicies = 1
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_1';

UPDATE LeagueProjectRewards
SET
	FreeSocialPolicies = 0,
	CultureBonusTurns = 20
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_2';

UPDATE LeagueProjectRewards
SET
	CultureBonusTurns = 0,
	Building = 'BUILDING_CRYSTAL_PALACE'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_3';

-- United Nations
UPDATE LeagueProjects
SET CostPerPlayer = 5100
WHERE Type = 'LEAGUE_PROJECT_UNITED_NATIONS';

UPDATE LeagueProjectRewards
SET FreeUnitClass = 'UNITCLASS_GREAT_DIPLOMAT'
WHERE Type = 'LEAGUE_PROJECT_REWARD_UNITED_NATIONS_1';

UPDATE LeagueProjectRewards
SET
	Building = 'BUILDING_CONSULATE',
	CityStateInfluenceBoost = 25
WHERE Type = 'LEAGUE_PROJECT_REWARD_UNITED_NATIONS_2';

UPDATE LeagueProjectRewards
SET Building = 'BUILDING_UN'
WHERE Type = 'LEAGUE_PROJECT_REWARD_UNITED_NATIONS_3';

-- International Games
UPDATE LeagueProjects
SET CostPerPlayer = 6750
WHERE Type = 'LEAGUE_PROJECT_WORLD_GAMES';

UPDATE LeagueProjectRewards
SET
	Happiness = 0,
	TourismBonusTurns = 20
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_1';

UPDATE LeagueProjectRewards
SET
	Happiness = 0,
	CityStateInfluenceBoost = 0,
	GetNumFreeGreatPeople = 1
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_2';

UPDATE LeagueProjectRewards
SET
	TourismBonusTurns = 0,
	FreeSocialPolicies = 0,
	Building = 'BUILDING_OLYMPIC_VILLAGE'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_3';

-- International Space Station
UPDATE LeagueProjects
SET CostPerPlayer = 8250
WHERE Type = 'LEAGUE_PROJECT_INTERNATIONAL_SPACE_STATION';
