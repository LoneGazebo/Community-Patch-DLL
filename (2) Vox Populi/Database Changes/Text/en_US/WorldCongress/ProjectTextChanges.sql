-- International Games
UPDATE Language_en_US
SET Text = '[ICON_TOURISM] Tourism increases by 50% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_1_HELP';

UPDATE Language_en_US
SET Text = ' Receive a free Great Person of your choice.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_2_HELP';

UPDATE Language_en_US
SET Text = 'Olympic Village wonder appears in [ICON_CAPITAL] Capital. (Receive +3 [ICON_HAPPINESS_1] Happiness and +3 [ICON_CULTURE] Culture in the City in which it is built. 25% of the [ICON_CULTURE] Culture from World Wonders and Tiles is added to the [ICON_TOURISM] Tourism output of the City. All Stadiums receive +2 [ICON_GOLD] Gold and [ICON_TOURISM] Tourism.)'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP';

-- World's Fair
UPDATE Language_en_US
SET Text = 'Free Social Policy.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP';

UPDATE Language_en_US
SET Text = '[ICON_CULTURE] Culture increases by 33% for 20 Turns.'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP';

UPDATE Language_en_US
SET Text = 'Crystal Palace wonder appears in [ICON_CAPITAL] Capital. (Receive +2 [ICON_PRODUCTION] Production and +2 [ICON_RESEARCH] Science for all Factories in owned Cities. The Empire enters a [ICON_GOLDEN_AGE] Golden Age.)'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP';

-- International Space Station
UPDATE Language_en_US
SET Text = 'International Space Station wonder appears in [ICON_CAPITAL] Capital. (+1 [ICON_PRODUCTION] Production from Scientists, and +1 [ICON_RESEARCH] Science from Engineers. [ICON_GREAT_SCIENTIST] Great Scientists provide 33% more [ICON_RESEARCH] Science when used to discover new Technology. +100 [ICON_PRODUCTION] in City where it is built when you unlock a new Technology, scaling with Era.)'
WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_ISS_3_HELP';
