-- Number of melee units per AA unit before the AI considers that they have enough, default value is 4
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_MELEE_PER_AA', 2);
-- Number of water tiles per ship for (small?) bodies of water, the default is 5, but 7 or 8 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_TILES_PER_SHIP', 6);


-- Priority caller for homeland AI secondary settler
INSERT INTO Defines(Name, Value) VALUES('AI_HOMELAND_MOVE_PRIORITY_SECONDARY_SETTLER', 1);
INSERT INTO Defines(Name, Value) VALUES('AI_HOMELAND_MOVE_PRIORITY_SECONDARY_WORKER', 3);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('CONFIG_AI_IN_XML', 1);