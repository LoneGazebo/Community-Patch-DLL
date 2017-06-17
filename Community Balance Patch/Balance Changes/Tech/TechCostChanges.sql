
-- Change Tech Costs (from CEP):

UPDATE Technologies SET Cost =     60 WHERE GridX =  1; --35
UPDATE Technologies SET Cost =     80 WHERE GridX =  2; --55
UPDATE Technologies SET Cost =    125 WHERE GridX =  3; --105
UPDATE Technologies SET Cost =    225 WHERE GridX =  4; --175
UPDATE Technologies SET Cost =    350 WHERE GridX =  5; --275
UPDATE Technologies SET Cost =    550 WHERE GridX =  6; --485
UPDATE Technologies SET Cost =   1000 WHERE GridX =  7; --780
UPDATE Technologies SET Cost =   1500 WHERE GridX =  8; --1150
UPDATE Technologies SET Cost =   2500 WHERE GridX =  9; --1600
UPDATE Technologies SET Cost =   3500 WHERE GridX = 10; --2350
UPDATE Technologies SET Cost =   5500 WHERE GridX = 11; --3100
UPDATE Technologies SET Cost =   6000 WHERE GridX = 12; --4100
UPDATE Technologies SET Cost =   6500 WHERE GridX = 13; --5100
UPDATE Technologies SET Cost =   7500 WHERE GridX = 14; --6400
UPDATE Technologies SET Cost =   8500 WHERE GridX = 15; --7700
UPDATE Technologies SET Cost =   9500 WHERE GridX = 16; --8800
UPDATE Technologies SET Cost =  10000 WHERE GridX = 17; --9500
UPDATE Technologies SET Cost =  11000 WHERE GridX = 18; --NEW!


-- Speed Change
UPDATE GameSpeeds SET ResearchPercent = 300 WHERE Type = 'GAMESPEED_MARATHON';
UPDATE GameSpeeds SET ResearchPercent = 200 WHERE Type = 'GAMESPEED_EPIC';