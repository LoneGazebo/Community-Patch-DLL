-- Number of melee units per AA unit before the AI considers that they have enough, default value is 4
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_MELEE_PER_AA', 2);
-- Number of water tiles per ship for (small?) bodies of water, the default is 5, but 7 or 8 would be sufficient
INSERT INTO Defines(Name, Value) VALUES('AI_CONFIG_MILITARY_TILES_PER_SHIP', 6);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('CONFIG_AI_IN_XML', 1);