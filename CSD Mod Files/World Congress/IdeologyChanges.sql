--Resistance: Unhappiness from World Ideology

UPDATE Resolutions
SET OtherIdeologyRebellionMod = 2
WHERE Type = 'RESOLUTION_WORLD_IDEOLOGY' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

--World's Fair Tech Change

UPDATE Resolutions
SET TechPrereqAnyMember = 'TECH_INDUSTRIALIZATION'
WHERE Type = 'RESOLUTION_WORLD_FAIR' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

--Tourism Early Boosts

UPDATE Technologies
SET InfluenceSpreadModifier = 15
WHERE Type = 'TECH_RADIO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Technologies
SET InfluenceSpreadModifier = 15
WHERE Type = 'TECH_COMPUTERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Broadcast Tower[ENDCOLOR], a building which greatly increases the [ICON_CULTURE] Cultural output of a city, and increases [ICON_TOURISM] Tourism output of all cities by 15%.'
WHERE Tag = 'TXT_KEY_TECH_RADIO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Helicopter Gunship[ENDCOLOR], a unit specialized in fighting enemy tanks, and increases [ICON_TOURISM] Tourism output of all cities by 15%.'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Greatly increases empire-wide [ICON_TOURISM] Tourism output.'
WHERE Tag = 'TXT_KEY_DOUBLE_TOURISM' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

--Resolution Changes


-- Sanctions 

UPDATE Language_en_US
SET Text = 'City-State Sanctions'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sanctions placed on City-States'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_CITY_STATES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sanctions placed on: {1_CivsList}'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes can be established with City-States. Warmonger penalty greatly reduced when declaring war on City-States or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sanction'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes can be established between the chosen Civilization and any other Civilization. Warmonger penalty greatly reduced when declaring war on the sanctioned Civilization or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Peace

UPDATE Language_en_US
SET Text = 'Global Peace Accords'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unit maintenance costs are raised by 25% [ICON_GOLD] Gold.[ICON_VICTORY_DOMINATION] Warmonger penalties for capturing cities and declaring war are greatly increased, and [ICON_VICTORY_DOMINATION] Warmonger scores decay much more slowly than normal.'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+{1_UnitMaintenancePercent}% [ICON_GOLD] Gold cost for Unit Maintenance. [ICON_VICTORY_DOMINATION] Warmonger penalties greatly increased'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Religion

UPDATE Language_en_US
SET Text = 'Designate a religion as the official World Religion.  Civilizations who have a majority of the cities in their empire following the religion get an additional Delegate. The founder of the religion and/or the controller of its Holy City receive 1 additional vote for every other Civilization following the religion. The religion spreads 25% faster, and its Holy City receives +50% [ICON_TOURISM] Tourism.  There can only be one World Religion at a time.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_RELIGION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Ideology

UPDATE Language_en_US
SET Text = 'Designate an Ideology as the official World Ideology.  Civilizations following the Ideology get an additional Delegate, plus 1 Delegate for every other Civilization that follows the Ideology.  Public opinion in favor of that Ideology is increased for all Civilizations. Enables the [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] if the United Nations has been constructed. [NEWLINE][NEWLINE]Available once any Civilization discovers Radio.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_IDEOLOGY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Scholars in Residence

UPDATE Language_en_US
SET Text = 'Civilizations research a technology 20% more quickly if it has been discovered by at least one other Civilization. This value is increased by an additional 5% for every City-State ally the Civilization currently maintains, up to -50%.'
WHERE Tag = 'TXT_KEY_RESOLUTION_MEMBER_DISCOVERED_TECH_DISCOUNT_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '-{1_ResearchCostPercent}% [ICON_RESEARCH] Research cost for Technologies already discovered by another Civilization. Additional -5% for every City-State ally, up to -50%'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_DISCOVERED_TECH_MODIFIER' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

--Wonder Culture

UPDATE Resolutions
SET CulturePerWonder = '2'
WHERE Type = 'RESOLUTION_CULTURAL_HERITAGE_SITES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = '+{1_CulturePerWorldWonder} [ICON_CULTURE] Culture from World Wonders, +1 [ICON_RESEARCH] Science from Great Works'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Each World Wonder provides +2 [ICON_CULTURE] Culture to its city, and each Great Work provides +1 [ICON_RESEARCH] Science to its city.'
WHERE Tag = 'TXT_KEY_RESOLUTION_CULTURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Natural Wonder

UPDATE Resolutions
SET CulturePerNaturalWonder = '2'
WHERE Type = 'RESOLUTION_NATURAL_HERITAGE_SITES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = 'Each Natural Wonder provides +2 to all Yields when worked.'
WHERE Tag = 'TXT_KEY_RESOLUTION_NATURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+{1_CulturePerWorldWonder} to yields from Natural Wonders'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_NATURAL_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Improvement/Landmarks

UPDATE Resolutions
SET GreatPersonTileImprovementCulture = '1'
WHERE Type = 'RESOLUTION_HISTORICAL_LANDMARKS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Resolutions
SET LandmarkCulture = '1'
WHERE Type = 'RESOLUTION_HISTORICAL_LANDMARKS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = 'Great Person tile improvements provide +1 to [ICON_PRODUCTION] Production, [ICON_FOOD] Food, and [ICON_GOLD] Gold when worked, and Landmarks built by Archaeologists provide +1 [ICON_CULTURE] Culture, [ICON_RESEARCH] Science, and [ICON_PEACE] Faith when worked.[NEWLINE][NEWLINE]Available once any Civilization discovers Archaeology.'
WHERE Tag = 'TXT_KEY_RESOLUTION_HISTORICAL_LANDMARKS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+{1_NumCulture} [ICON_PRODUCTION]/[ICON_FOOD]/[ICON_GOLD] from Great Person improvements'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_TILE_IMPROVEMENT_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+{1_NumCulture} [ICON_CULTURE]/[ICON_RESEARCH]/[ICON_PEACE] from Landmarks'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_LANDMARK_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );


-- World's Fair

UPDATE LeagueProjectRewards
SET FreeSocialPolicies = '1'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_1' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET GoldenAgePoints = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_1' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET FreeSocialPolicies = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_2' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET CultureBonusTurns = '20'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_2' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET Building = 'BUILDING_CRYSTAL_PALACE'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_3' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET CultureBonusTurns = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_FAIR_3' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = 'Free Social Policy.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture increases by 33% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Crystal Palace wonder appears in [ICON_CAPITAL] Capital. ({TXT_KEY_BUILDING_CRYSTAL_PALACE_HELP})'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Begins the World`s Fair project.  Once underway, Civilizations can contribute [ICON_PRODUCTION] Production towards its completion by selecting it in the city production list.  When complete, Civilizations receive bonuses based on how much they contributed.[NEWLINE][NEWLINE][ICON_TROPHY_GOLD]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP}[NEWLINE][ICON_TROPHY_SILVER]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP}[NEWLINE][ICON_TROPHY_BRONZE]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP} [NEWLINE][NEWLINE]Available once any Civilization discovers Industrialization.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_FAIR_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Games

UPDATE LeagueProjectRewards
SET Happiness = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_1' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET TourismBonusTurns = '20'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_1' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET Happiness = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_2' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET GetNumFreeGreatPeople = '1'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_2' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET CityStateInfluenceBoost = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_2' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET FreeSocialPolicies = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_3' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET TourismBonusTurns = '0'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_3' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE LeagueProjectRewards
SET Building = 'BUILDING_OLYMPIC_VILLAGE'
WHERE Type = 'LEAGUE_PROJECT_REWARD_WORLD_GAMES_3' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1);

UPDATE Language_en_US
SET Text = '[ICON_TOURISM] Tourism increases by 50% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = ' Receive a free Great Person of your choice.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Olympic Village wonder appears in [ICON_CAPITAL] Capital. ({TXT_KEY_BUILDING_OLYMPIC_VILLAGE_HELP})'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- Arts/Sciences Funding

UPDATE Language_en_US
SET Text = 'Increases generation of Great Writers, Artists, and Musicians by 33%.  Decreases generation of Great Scientists, Engineers, and Merchants by 33%. [NEWLINE][NEWLINE]Civilizations with Scores [COLOR_POSITIVE_TEXT]below the global average[ENDCOLOR] will receive a boost to [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture anywhere from 10-30%. [NEWLINE][NEWLINE]Details: The boost is the difference between the score of the Civilization and the leader, divided by 50.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Endowment for the Arts'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Increases generation of Great Scientists, Engineers, and Merchants by 33%.  Decreases generation of Great Writers, Artists, and Musicians by 33%. [NEWLINE][NEWLINE]Civilizations with Scores [COLOR_POSITIVE_TEXT]below the global average[ENDCOLOR] will receive a boost to [ICON_FOOD] Food and [ICON_RESEARCH] Science anywhere from 10-30%. [NEWLINE][NEWLINE]Details: The boost is the difference between the score of the Civilization and the leader, divided by 50.'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'World Science Initiative'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

-- World Leader

UPDATE Language_en_US
SET Text = 'Global Hegemony'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '{1_CivName} has attained Global Hegemony.'
WHERE Tag = 'TXT_KEY_VP_DIPLO_SOMEONE_WON' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Designate a player that could achieve Global Hegemony.  If a Civilization receives support from enough Delegates in the Congress, that leader will attain [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR].  The number of Delegates needed depends on the number of Civilizations and City-States in the game, and can be seen on the World Congress screen.[NEWLINE][NEWLINE]If no Civilization receives enough support to win, the two Civilizations who received the most support permanently gain an [COLOR_POSITIVE_TEXT]additional Delegate[ENDCOLOR] to use in future sessions.[NEWLINE][NEWLINE]May not be proposed by a Civilization.  Automatically proposed at regular intervals once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Turns until attempt at Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Delegates needed for Global Hegemony: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Since the World Congress has moved to the newly-constructed United Nations, Diplomatic Victory is now possible.  To enable the Diplomatic Victory special sessions, a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] must be active. Once active, a Civilization must receive the support of {1_DelegatesForWin} or more Delegates on a Global Hegemony proposal.  This number is based on the number of Civilizations and City-States in the game.[NEWLINE][NEWLINE]The Global Hegemony proposal is made to the Congress every other session once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active, alternating with regular sessions of regular proposals.'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[NEWLINE][ICON_BULLET]{1_NumVotes} from previous attmepts at Global Hegemony'
WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_EXTRA_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal on alternating sessions if a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active'
WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = '{@1_CivName} achieves Global Hegemony'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Global Hegemony not achieved'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Garnering the required support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, {@2_CivName} has achieved Global Hegemony.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Without a clear winner garnering the support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, the Global Hegemony proposal fails. The top {2_NumCivilizations} {2_NumCivilizations: plural 1?Civilization has; other?Civilizations have;} permanently gained an additional Delegate.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The World Congress is reborn as the United Nations.  A special session to is called to choose the host of this powerful new diplomatic force.  The United Nations will continue as before, but regular sessions will alternate with sessions allowing civilizations to try and claim Global Hegemony, making [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] possible. To enable these sessions, a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] must be active.'
WHERE Tag = 'TXT_KEY_LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Diplomacy[NEWLINE][NEWLINE]Turns until the United Nations convenes: {1_TurnsUntilSession}[NEWLINE][NEWLINE]Turns until the next [COLOR_POSITIVE_TEXT]Global Hegemony[ENDCOLOR] proposal: {2_TurnsUntilVictorySession}[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] is now possible, if a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_EO_DIPLOMACY_AND_VICTORY_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The world has entered the {@1_EraName}, either because of technological advances or completed resolutions.'
WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_GAME_ERA' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

--United Nations Addition

UPDATE LeagueSpecialSessions
SET BuildingTrigger = 'BUILDING_UN'
WHERE Type = 'LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

UPDATE LeagueSpecialSessions
SET TriggerResolution = 'RESOLUTION_WORLD_IDEOLOGY'
WHERE Type = 'LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );