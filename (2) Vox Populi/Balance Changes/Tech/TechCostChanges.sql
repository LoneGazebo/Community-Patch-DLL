-- VP Tech Costs
-- Ancient Era
UPDATE Technologies SET Cost =      60 WHERE GridX =  1; --35
UPDATE Technologies SET Cost =     100 WHERE GridX =  2; --55
-- Classical Era
UPDATE Technologies SET Cost =     130 WHERE GridX =  3; --105
UPDATE Technologies SET Cost =     275 WHERE GridX =  4; --175
-- Medieval Era
UPDATE Technologies SET Cost =     500 WHERE GridX =  5; --275
UPDATE Technologies SET Cost =     700 WHERE GridX =  6; --485
-- Renaissance Era
UPDATE Technologies SET Cost =    1750 WHERE GridX =  7; --780
UPDATE Technologies SET Cost =    2400 WHERE GridX =  8; --1150
-- Industrial Era
UPDATE Technologies SET Cost =    3600 WHERE GridX =  9; --1600
UPDATE Technologies SET Cost =    5150 WHERE GridX = 10; --2350
-- Modern Era
UPDATE Technologies SET Cost =    8100 WHERE GridX = 11; --3100
UPDATE Technologies SET Cost =   11000 WHERE GridX = 12; --4100
-- Atomic Era
UPDATE Technologies SET Cost =   13000 WHERE GridX = 13; --5100
UPDATE Technologies SET Cost =   16000 WHERE GridX = 14; --6400
-- Information Era
UPDATE Technologies SET Cost =   19100 WHERE GridX = 15; --7700
UPDATE Technologies SET Cost =   23400 WHERE GridX = 16; --8800
UPDATE Technologies SET Cost =   24450 WHERE GridX = 17; --9500
-- Future Tech
UPDATE Technologies SET Cost =   28550 WHERE GridX = 18; --NEW!

-- Speed Change
UPDATE GameSpeeds SET ResearchPercent = 300 WHERE Type = 'GAMESPEED_MARATHON';
UPDATE GameSpeeds SET ResearchPercent = 150 WHERE Type = 'GAMESPEED_EPIC';