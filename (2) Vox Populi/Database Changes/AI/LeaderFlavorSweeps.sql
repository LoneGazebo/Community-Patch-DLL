DELETE FROM Leader_Flavors;

-- Personality Types
CREATE TEMP TABLE ConquerorFlavors (
	FlavorType TEXT,
	Flavor INTEGER
);

INSERT INTO ConquerorFlavors
VALUES
	('FLAVOR_MILITARY_TRAINING', 8),
	('FLAVOR_OFFENSE', 8),
	('FLAVOR_DEFENSE', 5),
	('FLAVOR_CITY_DEFENSE', 4),
	('FLAVOR_MOBILE', 7),
	('FLAVOR_RECON', 7),
	('FLAVOR_RANGED', 8),
	('FLAVOR_NAVAL', 5),
	('FLAVOR_NAVAL_RECON', 7),
	('FLAVOR_NAVAL_GROWTH', 8),
	('FLAVOR_NAVAL_TILE_IMPROVEMENT', 6),
	('FLAVOR_AIR', 8),
	('FLAVOR_ANTIAIR', 4),
	('FLAVOR_AIRLIFT', 6),
	('FLAVOR_AIR_CARRIER', 5),
	('FLAVOR_USE_NUKE', 9),
	('FLAVOR_NUKE', 9),
	('FLAVOR_GROWTH', 7),
	('FLAVOR_PRODUCTION', 8),
	('FLAVOR_GOLD', 8),
	('FLAVOR_SCIENCE', 5),
	('FLAVOR_SPACESHIP', 4),
	('FLAVOR_ESPIONAGE', 4),
	('FLAVOR_CULTURE', 4),
	('FLAVOR_ARCHAEOLOGY', 3),
	('FLAVOR_RELIGION', 3),
	('FLAVOR_GREAT_PEOPLE', 4),
	('FLAVOR_HAPPINESS', 8),
	('FLAVOR_EXPANSION', 8),
	('FLAVOR_TILE_IMPROVEMENT', 3),
	('FLAVOR_INFRASTRUCTURE', 4),
	('FLAVOR_WATER_CONNECTION', 5),
	('FLAVOR_I_LAND_TRADE_ROUTE', 5),
	('FLAVOR_I_SEA_TRADE_ROUTE', 5),
	('FLAVOR_I_TRADE_ORIGIN', 5),
	('FLAVOR_I_TRADE_DESTINATION', 5),
	('FLAVOR_WONDER', 3),
	('FLAVOR_DIPLOMACY', 2);

INSERT INTO Leader_Flavors
	(LeaderType, FlavorType, Flavor)
SELECT
	a.Type, b.FlavorType, b.Flavor
FROM Leaders a, ConquerorFlavors b
WHERE a.Personality = 'PERSONALITY_CONQUEROR';

DROP TABLE ConquerorFlavors;

CREATE TEMP TABLE CoalitionFlavors (
	FlavorType TEXT,
	Flavor INTEGER
);

INSERT INTO CoalitionFlavors
VALUES
	('FLAVOR_MILITARY_TRAINING', 6),
	('FLAVOR_OFFENSE', 7),
	('FLAVOR_DEFENSE', 7),
	('FLAVOR_CITY_DEFENSE', 9),
	('FLAVOR_MOBILE', 5),
	('FLAVOR_RECON', 5),
	('FLAVOR_RANGED', 7),
	('FLAVOR_NAVAL', 7),
	('FLAVOR_NAVAL_RECON', 5),
	('FLAVOR_NAVAL_GROWTH', 9),
	('FLAVOR_NAVAL_TILE_IMPROVEMENT', 7),
	('FLAVOR_AIR', 5),
	('FLAVOR_ANTIAIR', 7),
	('FLAVOR_AIRLIFT', 5),
	('FLAVOR_AIR_CARRIER', 5),
	('FLAVOR_USE_NUKE', 3),
	('FLAVOR_NUKE', 3),
	('FLAVOR_GROWTH', 9),
	('FLAVOR_PRODUCTION', 6),
	('FLAVOR_GOLD', 7),
	('FLAVOR_SCIENCE', 8),
	('FLAVOR_SPACESHIP', 6),
	('FLAVOR_ESPIONAGE', 7),
	('FLAVOR_CULTURE', 6),
	('FLAVOR_ARCHAEOLOGY', 7),
	('FLAVOR_RELIGION', 8),
	('FLAVOR_GREAT_PEOPLE', 8),
	('FLAVOR_HAPPINESS', 5),
	('FLAVOR_EXPANSION', 7),
	('FLAVOR_TILE_IMPROVEMENT', 6),
	('FLAVOR_INFRASTRUCTURE', 7),
	('FLAVOR_WATER_CONNECTION', 6),
	('FLAVOR_I_LAND_TRADE_ROUTE', 7),
	('FLAVOR_I_SEA_TRADE_ROUTE', 7),
	('FLAVOR_I_TRADE_ORIGIN', 6),
	('FLAVOR_I_TRADE_DESTINATION', 6),
	('FLAVOR_WONDER', 5),
	('FLAVOR_DIPLOMACY', 8);

INSERT INTO Leader_Flavors
	(LeaderType, FlavorType, Flavor)
SELECT
	a.Type, b.FlavorType, b.Flavor
FROM Leaders a, CoalitionFlavors b
WHERE a.Personality = 'PERSONALITY_COALITION';

DROP TABLE CoalitionFlavors;

CREATE TEMP TABLE DiplomatFlavors (
	FlavorType TEXT,
	Flavor INTEGER
);

INSERT INTO DiplomatFlavors
VALUES
	('FLAVOR_MILITARY_TRAINING', 3),
	('FLAVOR_OFFENSE', 5),
	('FLAVOR_DEFENSE', 7),
	('FLAVOR_CITY_DEFENSE', 8),
	('FLAVOR_MOBILE', 5),
	('FLAVOR_RECON', 8),
	('FLAVOR_RANGED', 8),
	('FLAVOR_NAVAL', 5),
	('FLAVOR_NAVAL_RECON', 8),
	('FLAVOR_NAVAL_GROWTH', 8),
	('FLAVOR_NAVAL_TILE_IMPROVEMENT', 8),
	('FLAVOR_AIR', 5),
	('FLAVOR_ANTIAIR', 6),
	('FLAVOR_AIRLIFT', 4),
	('FLAVOR_AIR_CARRIER', 4),
	('FLAVOR_USE_NUKE', 4),
	('FLAVOR_NUKE', 4),
	('FLAVOR_GROWTH', 9),
	('FLAVOR_PRODUCTION', 8),
	('FLAVOR_GOLD', 8),
	('FLAVOR_SCIENCE', 8),
	('FLAVOR_SPACESHIP', 8),
	('FLAVOR_ESPIONAGE', 8),
	('FLAVOR_CULTURE', 8),
	('FLAVOR_ARCHAEOLOGY', 8),
	('FLAVOR_RELIGION', 7),
	('FLAVOR_GREAT_PEOPLE', 7),
	('FLAVOR_HAPPINESS', 7),
	('FLAVOR_EXPANSION', 7),
	('FLAVOR_TILE_IMPROVEMENT', 7),
	('FLAVOR_INFRASTRUCTURE', 5),
	('FLAVOR_WATER_CONNECTION', 6),
	('FLAVOR_I_LAND_TRADE_ROUTE', 8),
	('FLAVOR_I_SEA_TRADE_ROUTE', 8),
	('FLAVOR_I_TRADE_ORIGIN', 8),
	('FLAVOR_I_TRADE_DESTINATION', 8),
	('FLAVOR_WONDER', 6),
	('FLAVOR_DIPLOMACY', 8);

INSERT INTO Leader_Flavors
	(LeaderType, FlavorType, Flavor)
SELECT
	a.Type, b.FlavorType, b.Flavor
FROM Leaders a, DiplomatFlavors b
WHERE a.Personality = 'PERSONALITY_DIPLOMAT';

DROP TABLE DiplomatFlavors;

CREATE TEMP TABLE ExpansionistFlavors (
	FlavorType TEXT,
	Flavor INTEGER
);

INSERT INTO ExpansionistFlavors
VALUES
	('FLAVOR_MILITARY_TRAINING', 5),
	('FLAVOR_OFFENSE', 5),
	('FLAVOR_DEFENSE', 8),
	('FLAVOR_CITY_DEFENSE', 6),
	('FLAVOR_MOBILE', 7),
	('FLAVOR_RECON', 8),
	('FLAVOR_RANGED', 8),
	('FLAVOR_NAVAL', 5),
	('FLAVOR_NAVAL_RECON', 8),
	('FLAVOR_NAVAL_GROWTH', 9),
	('FLAVOR_NAVAL_TILE_IMPROVEMENT', 7),
	('FLAVOR_AIR', 7),
	('FLAVOR_ANTIAIR', 7),
	('FLAVOR_AIRLIFT', 7),
	('FLAVOR_AIR_CARRIER', 4),
	('FLAVOR_USE_NUKE', 4),
	('FLAVOR_NUKE', 4),
	('FLAVOR_GROWTH', 7),
	('FLAVOR_PRODUCTION', 7),
	('FLAVOR_GOLD', 7),
	('FLAVOR_SCIENCE', 7),
	('FLAVOR_SPACESHIP', 5),
	('FLAVOR_ESPIONAGE', 5),
	('FLAVOR_CULTURE', 6),
	('FLAVOR_ARCHAEOLOGY', 6),
	('FLAVOR_RELIGION', 6),
	('FLAVOR_GREAT_PEOPLE', 5),
	('FLAVOR_HAPPINESS', 8),
	('FLAVOR_EXPANSION', 9),
	('FLAVOR_TILE_IMPROVEMENT', 7),
	('FLAVOR_INFRASTRUCTURE', 9),
	('FLAVOR_WATER_CONNECTION', 9),
	('FLAVOR_I_LAND_TRADE_ROUTE', 5),
	('FLAVOR_I_SEA_TRADE_ROUTE', 5),
	('FLAVOR_I_TRADE_ORIGIN', 5),
	('FLAVOR_I_TRADE_DESTINATION', 5),
	('FLAVOR_WONDER', 5),
	('FLAVOR_DIPLOMACY', 6);

INSERT INTO Leader_Flavors
	(LeaderType, FlavorType, Flavor)
SELECT
	a.Type, b.FlavorType, b.Flavor
FROM Leaders a, ExpansionistFlavors b
WHERE a.Personality = 'PERSONALITY_EXPANSIONIST';

DROP TABLE ExpansionistFlavors;

-- barbs don't have leader personality anymore, so don't get inserted with the function above
INSERT INTO Leader_Flavors
	(LeaderType, FlavorType, Flavor)
VALUES
	('LEADER_BARBARIAN', 'FLAVOR_OFFENSE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_DEFENSE', 8),
	('LEADER_BARBARIAN', 'FLAVOR_CITY_DEFENSE', 8),
	('LEADER_BARBARIAN', 'FLAVOR_MILITARY_TRAINING', 4),
	('LEADER_BARBARIAN', 'FLAVOR_RECON', 2),
	('LEADER_BARBARIAN', 'FLAVOR_RANGED', 4),
	('LEADER_BARBARIAN', 'FLAVOR_MOBILE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_NAVAL', 4),
	('LEADER_BARBARIAN', 'FLAVOR_NAVAL_RECON', 4),
	('LEADER_BARBARIAN', 'FLAVOR_NAVAL_GROWTH', 4),
	('LEADER_BARBARIAN', 'FLAVOR_NAVAL_TILE_IMPROVEMENT', 4),
	('LEADER_BARBARIAN', 'FLAVOR_AIR', 0),
	('LEADER_BARBARIAN', 'FLAVOR_ANTIAIR', 0),
	('LEADER_BARBARIAN', 'FLAVOR_AIRLIFT', 0),
	('LEADER_BARBARIAN', 'FLAVOR_AIR_CARRIER', 0),
	('LEADER_BARBARIAN', 'FLAVOR_EXPANSION', 0),
	('LEADER_BARBARIAN', 'FLAVOR_GROWTH', 4),
	('LEADER_BARBARIAN', 'FLAVOR_HAPPINESS', 4),
	('LEADER_BARBARIAN', 'FLAVOR_TILE_IMPROVEMENT', 4),
	('LEADER_BARBARIAN', 'FLAVOR_INFRASTRUCTURE', 0),
	('LEADER_BARBARIAN', 'FLAVOR_PRODUCTION', 0),
	('LEADER_BARBARIAN', 'FLAVOR_GOLD', 4),
	('LEADER_BARBARIAN', 'FLAVOR_SCIENCE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_CULTURE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_GREAT_PEOPLE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_WONDER', 0),
	('LEADER_BARBARIAN', 'FLAVOR_RELIGION', 0),
	('LEADER_BARBARIAN', 'FLAVOR_DIPLOMACY', 0),
	('LEADER_BARBARIAN', 'FLAVOR_SPACESHIP', 0),
	('LEADER_BARBARIAN', 'FLAVOR_WATER_CONNECTION', 0),
	('LEADER_BARBARIAN', 'FLAVOR_USE_NUKE', 4),
	('LEADER_BARBARIAN', 'FLAVOR_NUKE', 0),
	('LEADER_BARBARIAN', 'FLAVOR_I_LAND_TRADE_ROUTE', 0),
	('LEADER_BARBARIAN', 'FLAVOR_I_SEA_TRADE_ROUTE', 0),
	('LEADER_BARBARIAN', 'FLAVOR_I_TRADE_ORIGIN', 0),
	('LEADER_BARBARIAN', 'FLAVOR_I_TRADE_DESTINATION', 0),
	('LEADER_BARBARIAN', 'FLAVOR_ARCHAEOLOGY', 0),
	('LEADER_BARBARIAN', 'FLAVOR_ESPIONAGE', 0);

-- OVERRIDES FOR UAs
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_AHMAD_ALMANSUR'; -- 4, Skirmishers line has both ranged and mobile
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_AHMAD_ALMANSUR'; -- 7, has unique improvement

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_ALEXANDER'; -- 5, primary: domination
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_ALEXANDER'; -- 5, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_ALEXANDER'; -- 9, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE' AND LeaderType = 'LEADER_ALEXANDER'; -- 5, both UU and UB boost GP
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_ALEXANDER'; -- 6, cultural UB
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_ALEXANDER'; -- 7, primary: domination
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_ALEXANDER'; -- 6, secondary&UA: diplo

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_ASHURBANIPAL'; -- 4, primary: science
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE' AND LeaderType = 'LEADER_ASHURBANIPAL'; -- 4, UA boosts great works

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_ASKIA'; -- 8, only pursuit: domination

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_AUGUSTUS'; -- 8, only pursuit: domination
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE' AND LeaderType = 'LEADER_AUGUSTUS'; -- 4, UB boosts GP

UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_BISMARCK'; -- 8, DOM
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_GROWTH' AND LeaderType = 'LEADER_BISMARCK'; -- 9, should be focusing Gold/Production
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_BISMARCK'; -- 8, should be focusing Gold/Production
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_BISMARCK'; -- 8, should be focusing Gold/Production
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_BISMARCK'; -- 8, DOM
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_ESPIONAGE' AND LeaderType = 'LEADER_BISMARCK'; -- 8, DOM
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_BISMARCK'; -- 8, DOM
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_RELIGION' AND LeaderType = 'LEADER_BISMARCK'; -- 8, should be focusing Gold/Production
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_BISMARCK'; -- 3, Unit Gifter
UPDATE Leader_Flavors SET Flavor = 3 WHERE FlavorType = 'FLAVOR_RECON' AND LeaderType = 'LEADER_BISMARCK'; -- 8, Can't gift recon units

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_CASIMIR'; -- 4, UB & UU focus on mobile
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_CASIMIR'; -- 4, UB & UU focus on mobile
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_PRODUCTION' AND LeaderType = 'LEADER_CASIMIR'; -- 6, generalist
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_CASIMIR'; -- 10, generalist
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_CASIMIR'; -- 6, generalist
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_CASIMIR'; -- 5, generalist, literally noone has this low
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_CASIMIR'; -- 5, generalist

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_CATHERINE'; -- 5, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_CATHERINE'; -- 5, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_CATHERINE'; -- 9, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' AND LeaderType = 'LEADER_CATHERINE'; -- 5, UB
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_NUKE' AND LeaderType = 'LEADER_CATHERINE'; -- 4, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_USE_NUKE' AND LeaderType = 'LEADER_CATHERINE'; -- 4, primary: domination
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_CATHERINE'; -- 5, secondary: science
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_CATHERINE'; -- 8, secondary: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_CATHERINE'; -- 7, UB, UA

UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_GOLD' AND LeaderType = 'LEADER_DARIUS'; -- 8 UA

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_AIR_CARRIER' AND LeaderType = 'LEADER_DIDO'; -- 4, strong navy
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' AND LeaderType = 'LEADER_DIDO'; -- 6
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_DIDO'; -- 9
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_DIDO'; -- 4, lol no
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL_RECON' AND LeaderType = 'LEADER_DIDO'; -- 4
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_DIDO'; -- 7
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_I_TRADE_ORIGIN' AND LeaderType = 'LEADER_DIDO'; -- 4, UB
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_I_TRADE_DESTINATION' AND LeaderType = 'LEADER_DIDO'; -- 4 UB
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_DIDO'; -- 6, primary: diplomacy

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_ELIZABETH'; -- 6, primary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_AIR' AND LeaderType = 'LEADER_ELIZABETH'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_ELIZABETH'; -- 7, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_AIR_CARRIER' AND LeaderType = 'LEADER_ELIZABETH'; -- 4, primary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_ELIZABETH'; -- 7, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_PRODUCTION' AND LeaderType = 'LEADER_ELIZABETH'; -- 6, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_ELIZABETH'; -- 9, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_ELIZABETH'; -- 6, secondary: science
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_ESPIONAGE' AND LeaderType = 'LEADER_ELIZABETH'; -- 8, UA, UB
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_ELIZABETH'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_WATER_CONNECTION' AND LeaderType = 'LEADER_ELIZABETH'; -- 6, naval oriented
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' AND LeaderType = 'LEADER_ELIZABETH'; -- 9, DOM
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_ELIZABETH'; -- 8, DOM

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 8, has few, better protect them 
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RECON' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 9
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 5, has strong coast bias
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 5
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 9, primary: diplo
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_AIR_CARRIER' AND LeaderType = 'LEADER_ENRICO_DANDOLO'; -- 4, naval-ish

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_GAJAH_MADA'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_GAJAH_MADA'; -- 9, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_GAJAH_MADA'; -- 6, secondary: diplo

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_GANDHI'; -- 4, skirmisher
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_GANDHI'; -- 8, primary: science
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_GANDHI'; -- 8, primary: science
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_GANDHI'; -- 

UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_GENGHIS_KHAN'; -- 8, primary: dom
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_GENGHIS_KHAN'; -- 8, primary: dom
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_NUKE' AND LeaderType = 'LEADER_GENGHIS_KHAN'; -- 9, primary: dom
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_USE_NUKE' AND LeaderType = 'LEADER_GENGHIS_KHAN'; -- 9, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_GENGHIS_KHAN'; -- 3, UB

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS'; -- 8, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_GUSTAVUS_ADOLPHUS'; -- 5, UB

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_HARALD'; -- 8, dom
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_HARALD'; -- 8, dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_WATER_CONNECTION' AND LeaderType = 'LEADER_HARALD'; -- 5, UB
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_AIR_CARRIER' AND LeaderType = 'LEADER_HARALD'; -- 5, naval-ish

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 7, primary: culture
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 4, skirmisher
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 9, primary: culture
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 8, primary: culture
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 7, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 5, primary: culture
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_I_LAND_TRADE_ROUTE' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 7, UB
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_I_TRADE_ORIGIN' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 6, UA
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_I_TRADE_DESTINATION' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 6, UA
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 5, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_HARUN_AL_RASHID'; -- 8, primary: culture

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_HIAWATHA'; -- 5, secondary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_HIAWATHA'; -- 9, secondary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_HIAWATHA'; -- 6, primary: dom

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_ISABELLA'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_ISABELLA'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 4 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_ISABELLA'; -- 8, primary: dom
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_ISABELLA'; -- 7, UU is a pseudo mounted
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_ISABELLA'; -- 5, UA, loves overseas expansion
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_AIR_CARRIER' AND LeaderType = 'LEADER_ISABELLA'; -- 4, naval
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_USE_NUKE' AND LeaderType = 'LEADER_ISABELLA'; -- 4, primary: dom
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_NUKE' AND LeaderType = 'LEADER_ISABELLA'; -- 4, primary: dom
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_GROWTH' AND LeaderType = 'LEADER_ISABELLA'; -- 7, primary: dom
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_GOLD' AND LeaderType = 'LEADER_ISABELLA'; -- 7, UA
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_ISABELLA'; -- 7, primary: dom
UPDATE Leader_Flavors SET Flavor = 3 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_ISABELLA'; -- 5, primary: dom
UPDATE Leader_Flavors SET Flavor = 3 WHERE FlavorType = 'FLAVOR_ESPIONAGE' AND LeaderType = 'LEADER_ISABELLA'; -- 5, passive
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_RELIGION' AND LeaderType = 'LEADER_ISABELLA'; -- 6, UA
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_ISABELLA'; -- 8, naval
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_ISABELLA'; -- 7, UI
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_I_SEA_TRADE_ROUTE' AND LeaderType = 'LEADER_ISABELLA'; -- 5, naval
UPDATE Leader_Flavors SET Flavor = 4 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_ISABELLA'; -- 5, primary: dom

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 7, expansionist
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 8, ocean goer
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_NAVAL_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 7, UA
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 9, primary: culture
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 7, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 5, primary: culture
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 6, UI
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_WATER_CONNECTION' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 6, naval civ
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_KAMEHAMEHA'; -- 5, primary: culture

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_MARIA'; -- 4, skirmisher
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RECON' AND LeaderType = 'LEADER_MARIA'; -- 9
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_MARIA'; -- 5
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_MARIA'; -- 8, only: diplo
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_MARIA'; -- 9, only: diplo

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_MARIA_I'; -- has GA&GG bonuses
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RECON' AND LeaderType = 'LEADER_MARIA_I'; -- 9
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_MARIA_I'; -- 5
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_MARIA_I'; -- 8, only: diplo
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_MARIA_I'; -- 9, only: diplo
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_MARIA_I'; -- UI
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_I_TRADE_ORIGIN' AND LeaderType = 'LEADER_MARIA_I'; -- UA
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_I_TRADE_DESTINATION' AND LeaderType = 'LEADER_MARIA_I'; -- UA

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_MONTEZUMA'; -- 8, primary: dom

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_NAPOLEON'; -- 8, primary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_NAPOLEON'; -- 10, primary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_NAPOLEON'; -- UI
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_NAPOLEON'; -- 3, secondary: culture

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_NEBUCHADNEZZAR'; -- 7, turtle
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RANGED' AND LeaderType = 'LEADER_NEBUCHADNEZZAR'; -- 7, UU
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_NEBUCHADNEZZAR'; -- 6, SCIENCE CIV
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_NEBUCHADNEZZAR'; -- 8, SCIENCE CIV

UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_ODA_NOBUNAGA'; -- 9, UB
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_ODA_NOBUNAGA'; -- 3, secondary: culture
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE' AND LeaderType = 'LEADER_ODA_NOBUNAGA'; -- 4, UA
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_ODA_NOBUNAGA'; -- 4, secondary: culture

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RANGED' AND LeaderType = 'LEADER_PACAL'; -- 7, UU
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_PACAL'; -- 6, secondary: culture
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_PACAL'; -- 6, primary: science

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_PACHACUTI'; -- 7, primary: science
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_PACHACUTI'; -- 5, primary: science
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_PACHACUTI'; -- 7, UI

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_PEDRO'; -- 5
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_PEDRO'; -- 8, primary: culture
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_HAPPINESS' AND LeaderType = 'LEADER_PEDRO'; -- 7, UA
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_PEDRO'; -- 6, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_PEDRO'; -- 9
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_PEDRO'; -- 7, UI

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CITY_DEFENSE' AND LeaderType = 'LEADER_POCATELLO'; -- 6, needs defense
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_EXPANSION' AND LeaderType = 'LEADER_POCATELLO'; -- 9, this is what peak expansionism looks like, colossal
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_POCATELLO'; -- 6, generalist civ
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_POCATELLO'; -- 7, UI

UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_RAMESSES'; -- 6, primary: culture
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_RAMESSES'; -- 6, UA!
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_RAMESSES'; -- 9, 
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_GOLD' AND LeaderType = 'LEADER_RAMESSES'; -- 7, UB 

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_RAMKHAMHAENG'; -- 4, UU 
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_RAMKHAMHAENG'; -- 5
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_RAMKHAMHAENG'; -- 8, only: diplo
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_RAMKHAMHAENG'; -- 9

UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_SEJONG'; -- 4, UU 
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_SEJONG'; -- 5
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_SEJONG'; -- 8, only: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_SEJONG'; -- 9, only: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_SEJONG'; -- 9, only: science
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_SEJONG'; -- 9, only: science

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_SELASSIE'; -- 7 
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_SELASSIE'; -- 7
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_SELASSIE'; -- 8, strong secondary: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_SELASSIE'; -- 9, only: science
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_SELASSIE'; -- 9, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_SELASSIE'; -- 5, primary: culture
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_SELASSIE'; -- 8, has enough strong pursuits already

UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_SHAKA'; -- 9, UB
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_SHAKA'; -- lives for the blood of his enemies

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_SULEIMAN'; -- 5, secondary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_SULEIMAN'; -- 5, secondary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_SULEIMAN'; -- 9, secondary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_NUKE' AND LeaderType = 'LEADER_SULEIMAN'; -- 4, could nuke on a bad day
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_USE_NUKE' AND LeaderType = 'LEADER_SULEIMAN'; -- 4
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_PRODUCTION' AND LeaderType = 'LEADER_SULEIMAN'; -- 7, UB
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_SULEIMAN'; -- 7, primary: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_SULEIMAN'; -- 5, primary: science
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_I_TRADE_ORIGIN' AND LeaderType = 'LEADER_SULEIMAN'; -- 4, UA 
UPDATE Leader_Flavors SET Flavor = 10 WHERE FlavorType = 'FLAVOR_I_TRADE_DESTINATION' AND LeaderType = 'LEADER_SULEIMAN'; -- 4, UA 

UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_THEODORA'; -- 3, generalist
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_THEODORA'; -- 4, UU

UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_RANGED' AND LeaderType = 'LEADER_WASHINGTON'; -- 7, UU
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_WASHINGTON'; -- 6, primary: culture
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_WASHINGTON'; -- 7, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_WONDER' AND LeaderType = 'LEADER_WASHINGTON'; -- 5, primary: culture
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_WASHINGTON'; -- 9, primary: culture
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_EXPANSION' AND LeaderType = 'LEADER_WASHINGTON'; -- 7, UA
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_WASHINGTON'; -- 8, primary: culture

UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_WILLIAM'; -- 3, aggresive UU, can benefit
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_NAVAL' AND LeaderType = 'LEADER_WILLIAM'; -- 5, naval civ
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ANTIAIR' AND LeaderType = 'LEADER_WILLIAM'; -- 5
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_WILLIAM'; -- 8, only: diplo
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_WILLIAM'; -- 8, only: diplo
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_WILLIAM'; -- 9, only: diplo
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_WILLIAM'; -- 9, only: diplo
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_TILE_IMPROVEMENT' AND LeaderType = 'LEADER_WILLIAM'; -- 7, UI
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_DIPLOMACY' AND LeaderType = 'LEADER_WILLIAM'; -- 5, only: diplo

UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_MILITARY_TRAINING' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 5, secondary: dom
UPDATE Leader_Flavors SET Flavor = 6 WHERE FlavorType = 'FLAVOR_OFFENSE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 5, secondary: dom
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_DEFENSE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 9, secondary: dom
UPDATE Leader_Flavors SET Flavor = 5 WHERE FlavorType = 'FLAVOR_MOBILE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 7
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_NUKE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 4, secondary: dom
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_USE_NUKE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 4
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SCIENCE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 5, primary: science
UPDATE Leader_Flavors SET Flavor = 8 WHERE FlavorType = 'FLAVOR_SPACESHIP' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 5, primary: science
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_CULTURE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 10
UPDATE Leader_Flavors SET Flavor = 7 WHERE FlavorType = 'FLAVOR_ARCHAEOLOGY' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 8
UPDATE Leader_Flavors SET Flavor = 9 WHERE FlavorType = 'FLAVOR_GREAT_PEOPLE' AND LeaderType = 'LEADER_WU_ZETIAN'; -- 8, UA
