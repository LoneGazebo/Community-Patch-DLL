-- Number of melee units per AA unit before the AI considers that they have enough, default value is 4
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_MELEE_PER_AA', 4);
-- Number of aircraft per "carrier slot" before the AI considers that they have enough, default value is 1,
--  ie the AI will build carriers until it can embark EVERY aircraft, 2 or 3 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_AIRCRAFT_PER_CARRIER_SPACE', 1);
-- Number of water tiles per ship for (small?) bodies of water, the default is 5, but 7 or 8 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_TILES_PER_SHIP', 5);
-- Warmonger weight for capturing a minor civ's city
INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_MINOR_CITY_WEIGHT', 1000);
-- Warmonger weight for capturing a major civ's city
INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_MAJOR_CITY_WEIGHT', 1000);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('CONFIG_AI_IN_XML', 1);

