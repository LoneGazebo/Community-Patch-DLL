
-- Change Tech Costs (from CEP):

UPDATE Technologies SET Cost =     40 WHERE GridX =  1; --35
UPDATE Technologies SET Cost =     80 WHERE GridX =  2; --55
UPDATE Technologies SET Cost =    150 WHERE GridX =  3; --105
UPDATE Technologies SET Cost =    250 WHERE GridX =  4; --175
UPDATE Technologies SET Cost =    400 WHERE GridX =  5; --275
UPDATE Technologies SET Cost =    675 WHERE GridX =  6; --485
UPDATE Technologies SET Cost =   1050 WHERE GridX =  7; --780
UPDATE Technologies SET Cost =   1700 WHERE GridX =  8; --1150
UPDATE Technologies SET Cost =   2300 WHERE GridX =  9; --1600
UPDATE Technologies SET Cost =   3100 WHERE GridX = 10; --2350
UPDATE Technologies SET Cost =   4200 WHERE GridX = 11; --3100
UPDATE Technologies SET Cost =   4800 WHERE GridX = 12; --4100
UPDATE Technologies SET Cost =   5600 WHERE GridX = 13; --5100
UPDATE Technologies SET Cost =   6800 WHERE GridX = 14; --6400
UPDATE Technologies SET Cost =   8300 WHERE GridX = 15; --7700
UPDATE Technologies SET Cost =   9300 WHERE GridX = 16; --8800
UPDATE Technologies SET Cost =  10000 WHERE GridX = 17; --9500
UPDATE Technologies SET Cost =  10000 WHERE GridX = 18; --NEW!


-- Speed Change
UPDATE GameSpeeds SET ResearchPercent = 350 WHERE Type = 'GAMESPEED_MARATHON';
UPDATE GameSpeeds SET ResearchPercent = 200 WHERE Type = 'GAMESPEED_EPIC';