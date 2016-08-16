-- Number of melee units per AA unit before the AI considers that they have enough, default value is 4
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_MELEE_PER_AA', 2);
-- Number of aircraft per "carrier slot" before the AI considers that they have enough, default value is 1,
--  ie the AI will build carriers until it can embark EVERY aircraft, 2 or 3 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_AIRCRAFT_PER_CARRIER_SPACE', 3);
-- Number of water tiles per ship for (small?) bodies of water, the default is 5, but 7 or 8 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_TILES_PER_SHIP', 6);


-- Priority caller for homeland AI secondary settler
INSERT INTO Defines(Name, Value) VALUES('AI_HOMELAND_MOVE_PRIORITY_SECONDARY_SETTLER', 1);
INSERT INTO Defines(Name, Value) VALUES('AI_HOMELAND_MOVE_PRIORITY_SECONDARY_WORKER', 3);

-- Warmonger weight for capturing a major civ's city
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_MAJOR_CITY_WEIGHT', 1000);
-- Warmonger weight for capturing a minor civ's city
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_MINOR_CITY_WEIGHT', 400);
-- Warmonger modifier for the capital
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_CAPITAL_CITY_PERCENT', 150);
-- Warmonger modifiers for knowing the attacker/defender
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_KNOWS_ATTACKER_PERCENT', 33);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_KNOWS_DEFENDER_PERCENT', 100);
-- Warmonger modifier for being the aggrieved
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_AGGRIEVED_PERCENT', 67);
-- Warmonger modifier for joint wars
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_COOP_WAR_PERCENT', 0);
-- Warmonger enabled for def pact consideration
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_DEF_PACT_ENABLED', 1);
-- Warmonger enabled for city size consideration
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_CITY_SIZE_ENABLED', 0);
-- Warmonger approach modifiers
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_PERCENT_HOSTILE', 30);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_PERCENT_AFRAID', 20);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_PERCENT_GUARDED', 15);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_PERCENT_NEUTRAL', 10);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_PERCENT_FRIENDLY', 0);
-- Warmonger decay modifiers
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_DECAY_PERCENT_HOSTILE', 70);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_DECAY_PERCENT_AFRAID', 90);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_DECAY_PERCENT_GUARDED', 100);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_DECAY_PERCENT_NEUTRAL', 110);
--INSERT INTO Defines(Name, Value) VALUES('WARMONGER_THREAT_APPROACH_DECAY_PERCENT_FRIENDLY', 130);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('CONFIG_AI_IN_XML', 1);

