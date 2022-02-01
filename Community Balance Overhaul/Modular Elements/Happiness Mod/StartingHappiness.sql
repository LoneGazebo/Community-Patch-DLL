-- FREE HAPPINESS FROM GAME SPEED

UPDATE GameSpeeds
SET StartingHappiness = '2'
WHERE Type = 'GAMESPEED_MARATHON';

UPDATE GameSpeeds
SET StartingHappiness = '1'
WHERE Type = 'GAMESPEED_EPIC';

UPDATE GameSpeeds
SET StartingHappiness = '0'
WHERE Type = 'GAMESPEED_STANDARD';

UPDATE GameSpeeds
SET GoldenAgePercent = '300'
WHERE Type = 'GAMESPEED_MARATHON';

UPDATE GameSpeeds
SET GoldenAgePercent = '150'
WHERE Type = 'GAMESPEED_EPIC';

UPDATE GameSpeeds
SET GoldenAgePercent = '100'
WHERE Type = 'GAMESPEED_STANDARD';

UPDATE GameSpeeds
SET GoldenAgePercent = '67'
WHERE Type = 'GAMESPEED_QUICK';