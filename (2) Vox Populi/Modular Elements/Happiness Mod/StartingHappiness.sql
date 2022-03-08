-- FREE HAPPINESS FROM GAME SPEED

UPDATE GameSpeeds
SET StartingHappiness = '2', GoldenAgePercent = '300'
WHERE Type = 'GAMESPEED_MARATHON';

UPDATE GameSpeeds
SET StartingHappiness = '1', GoldenAgePercent = '150'
WHERE Type = 'GAMESPEED_EPIC';

UPDATE GameSpeeds
SET StartingHappiness = '0', GoldenAgePercent = '100'
WHERE Type = 'GAMESPEED_STANDARD';

UPDATE GameSpeeds
SET GoldenAgePercent = '67'
WHERE Type = 'GAMESPEED_QUICK';