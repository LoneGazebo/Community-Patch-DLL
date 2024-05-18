UPDATE Language_en_US
SET Text = 'City-State Sanctions'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO';

UPDATE Language_en_US
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes can be established with City-States, and all Corporation Franchises are removed from City-States. ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_ATTACKED_SANCTIONED_PLAYER') / 100.0 AS NUMERIC) || 'x Warmonger penalty when declaring war on City-States or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO_HELP';

UPDATE Language_en_US
SET Text = 'Sanction'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO';

UPDATE Language_en_US
SET Text = 'No [ICON_INTERNATIONAL_TRADE] Trade Routes, Deals, or Corporation Franchises can be established with the chosen Civilization (excluding Vassals). Their Corporation Franchises are removed from foreign cities. ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_ATTACKED_SANCTIONED_PLAYER') / 100.0 AS NUMERIC) || 'x Warmonger penalty when declaring war on the sanctioned Civilization or conquering cities owned by them.'
WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO_HELP';

UPDATE Language_en_US
SET Text = 'Global Peace Accords'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX';

UPDATE Language_en_US
SET Text = 'Unit maintenance costs are raised by 25% [ICON_GOLD] Gold. ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_ATTACKED_WEIGHT_WORLD_PEACE') / 100.0 AS NUMERIC) || 'x [ICON_VICTORY_DOMINATION] Warmonger penalties for capturing cities and declaring war, and ' || CAST((SELECT Value FROM Defines WHERE Name = 'WARMONGER_THREAT_PER_TURN_DECAY_DECREASED') / 100.0 AS NUMERIC) || 'x [ICON_VICTORY_DOMINATION] Warmonger scores decay.'
WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX_HELP';

UPDATE Language_en_US
SET Text = 'Endowment for the Arts'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING';

UPDATE Language_en_US
SET Text = 'Increases up to 33% generation of Great Writers, Artists, and Musicians. Decreases up to 33% generation of Great Scientists, Engineers, and Merchants.[NEWLINE][NEWLINE]Civilizations with Scores from Policies and Great Works [COLOR_POSITIVE_TEXT]below the global median[ENDCOLOR] will receive a boost to [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture up to 30%.[NEWLINE][NEWLINE]Details: The boosts are proportional to the difference between the score of the Civilization and the leader.'
WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING_HELP';

UPDATE Language_en_US
SET Text = 'World Science Initiative'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING';

UPDATE Language_en_US
SET Text = 'Increases up to 33% generation of Great Scientists, Engineers, and Merchants. Decreases up to 33% generation of Great Writers, Artists, and Musicians.[NEWLINE][NEWLINE]Civilizations with Scores from Technologies and Future Tech [COLOR_POSITIVE_TEXT]below the global median[ENDCOLOR] will receive a boost to [ICON_FOOD] Food and [ICON_RESEARCH] Science up to 30%.[NEWLINE][NEWLINE]Details: The boosts are proportional to the difference between the score of the Civilization and the leader.'
WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING_HELP';

UPDATE Language_en_US
SET Text = 'Global Hegemony'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY';

UPDATE Language_en_US
SET Text = 'Designate a player that could achieve Global Hegemony. If a Civilization receives support from enough Delegates in the Congress, that leader will attain [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR]. The number of Delegates needed depends on the number of Civilizations and City-States in the game, and can be seen on the World Congress screen.[NEWLINE][NEWLINE]If no Civilization receives enough support to win, the two Civilizations who received the most support permanently gain [COLOR_POSITIVE_TEXT]5 additional Delegates[ENDCOLOR] to use in future sessions.[NEWLINE][NEWLINE]May not be proposed by a Civilization. Automatically proposed at regular intervals once a [COLOR_POSITIVE_TEXT]World Ideology Resolution[ENDCOLOR] is active.'
WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY_HELP';

-- World Religion
UPDATE Language_en_US
SET Text = 'Designate a religion as the official World Religion. Civilizations who have a majority of the cities in their empire following the religion get an additional Delegate. The founder of the religion and/or the controller of its Holy City receives additional votes based on the number of Civilizations following the religion. The religion spreads 25% faster, and its Holy City receives +50% [ICON_TOURISM] Tourism. There can only be one World Religion at a time.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_RELIGION_HELP';

-- World Ideology
UPDATE Language_en_US
SET Text = 'Designate an Ideology as the official World Ideology. Civilizations following the Ideology get an additional Delegate, plus 1 Delegate for every other Civilization that follows the Ideology. Public opinion in favor of that Ideology is increased for all Civilizations. Enables the [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR] if the United Nations has been constructed.[NEWLINE][NEWLINE]Available once any Civilization discovers Nuclear Fission.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_IDEOLOGY_HELP';

-- Ban Luxury
UPDATE Language_en_US
SET Text = 'A Luxury resource is designated as immoral, endangered, or otherwise inappropriate for use by the general population. As long as the ban is in place, Civilizations receive no [ICON_HAPPINESS_1] Happiness or Monopoly benefits from this Luxury resource.'
WHERE Tag = 'TXT_KEY_RESOLUTION_BAN_LUXURY_HAPPINESS_HELP';

-- Scholars In Residence
UPDATE Language_en_US
SET Text = 'Civilizations have -20% [ICON_RESEARCH] Research cost for Technologies already discovered by another Civilization. Additional -2% for every City-State Ally, up to -50%.'
WHERE Tag = 'TXT_KEY_RESOLUTION_MEMBER_DISCOVERED_TECH_DISCOUNT_HELP';

-- Cultural Heritage Sites
UPDATE Language_en_US
SET Text = 'Each World Wonder provides +2 [ICON_CULTURE] Culture to its city, and each Great Work provides +1 [ICON_RESEARCH] Science to its city.'
WHERE Tag = 'TXT_KEY_RESOLUTION_CULTURAL_HERITAGE_SITES_HELP';

-- Natural Heritage Sites
UPDATE Language_en_US
SET Text = 'Each Natural Wonder provides +2 to all Yields when worked.'
WHERE Tag = 'TXT_KEY_RESOLUTION_NATURAL_HERITAGE_SITES_HELP';

-- Historical Landmarks
UPDATE Language_en_US
SET Text = 'Great Person tile improvements provide +1 to [ICON_FOOD] Food, [ICON_PRODUCTION] Production, and [ICON_GOLD] Gold when worked, and Landmarks built by Archaeologists provide +1 [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith when worked.[NEWLINE][NEWLINE]Available once any Civilization discovers Archaeology.'
WHERE Tag = 'TXT_KEY_RESOLUTION_HISTORICAL_LANDMARKS_HELP';

-- World's Fair
UPDATE Language_en_US
SET Text = 'Begins the World`s Fair project. Once underway, Civilizations can contribute [ICON_PRODUCTION] Production towards its completion by selecting it in the city production list. When complete, Civilizations receive bonuses based on how much they contributed.[NEWLINE][NEWLINE][ICON_TROPHY_GOLD]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP}[NEWLINE][ICON_TROPHY_SILVER]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP}[NEWLINE][ICON_TROPHY_BRONZE]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP}[NEWLINE][NEWLINE]Available once any Civilization discovers Industrialization.'
WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_FAIR_HELP';

-- International Space Station
UPDATE Language_en_US
SET Text = 'Begins the International Space Station project. Once underway, Civilizations can contribute [ICON_PRODUCTION] Production towards its completion by selecting it in the city production list. When complete, Civilizations receive bonuses based on how much they contributed.[NEWLINE][NEWLINE][ICON_TROPHY_GOLD]: {TXT_KEY_LEAGUE_PROJECT_REWARD_ISS_3_HELP}[NEWLINE][ICON_TROPHY_SILVER]: {TXT_KEY_LEAGUE_PROJECT_REWARD_ISS_2_HELP}[NEWLINE][ICON_TROPHY_BRONZE]: {TXT_KEY_LEAGUE_PROJECT_REWARD_ISS_1_HELP}[NEWLINE][NEWLINE]Available once any Civilization discovers Rocketry.'
WHERE Tag = 'TXT_KEY_RESOLUTION_INTERNATIONAL_SPACE_STATION_HELP';
