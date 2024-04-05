UPDATE LeagueSpecialSessions
SET TurnsBetweenSessions = 20
WHERE Type = 'LEAGUE_SPECIAL_SESSION_START_WORLD_CONGRESS';

UPDATE LeagueSpecialSessions
SET TurnsBetweenSessions = 15
WHERE Type = 'LEAGUE_SPECIAL_SESSION_WELCOME_CITY_STATES';

UPDATE LeagueSpecialSessions
SET TurnsBetweenSessions = 10
WHERE Type = 'LEAGUE_SPECIAL_SESSION_LEADERSHIP_COUNCIL';

-- Requires both building United Nations and passing World Ideology to trigger Hegemony vote
UPDATE LeagueSpecialSessions
SET
	BuildingTrigger = 'BUILDING_UN',
	TriggerResolution = 'RESOLUTION_WORLD_IDEOLOGY',
	EraTrigger = 'ERA_MODERN'
WHERE Type = 'LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS';
