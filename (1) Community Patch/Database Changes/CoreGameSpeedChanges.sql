UPDATE GameSpeeds SET TradeRouteSpeedMod = 67 WHERE Type = 'GAMESPEED_QUICK';
UPDATE GameSpeeds SET TradeRouteSpeedMod = 100 WHERE Type = 'GAMESPEED_STANDARD';
UPDATE GameSpeeds SET TradeRouteSpeedMod = 150 WHERE Type = 'GAMESPEED_EPIC';
UPDATE GameSpeeds SET TradeRouteSpeedMod = 300 WHERE Type = 'GAMESPEED_MARATHON';

UPDATE GameSpeeds SET DifficultyBonusPercent = 67 WHERE Type = 'GAMESPEED_QUICK';
UPDATE GameSpeeds SET DifficultyBonusPercent = 100 WHERE Type = 'GAMESPEED_STANDARD';
UPDATE GameSpeeds SET DifficultyBonusPercent = 160 WHERE Type = 'GAMESPEED_EPIC';
UPDATE GameSpeeds SET DifficultyBonusPercent = 300 WHERE Type = 'GAMESPEED_MARATHON';

UPDATE Gamespeeds SET ReligiousPressureAdjacentCity = 95 WHERE Type = 'GAMESPEED_QUICK';
UPDATE Gamespeeds SET ReligiousPressureAdjacentCity = 65 WHERE Type = 'GAMESPEED_STANDARD';
UPDATE Gamespeeds SET ReligiousPressureAdjacentCity = 45 WHERE Type = 'GAMESPEED_EPIC';
UPDATE Gamespeeds SET ReligiousPressureAdjacentCity = 25 WHERE Type = 'GAMESPEED_MARATHON';

UPDATE Gamespeeds SET MilitaryRatingDecayPercent = 30 WHERE Type = 'GAMESPEED_QUICK';
UPDATE Gamespeeds SET MilitaryRatingDecayPercent = 20 WHERE Type = 'GAMESPEED_STANDARD';
UPDATE Gamespeeds SET MilitaryRatingDecayPercent = 13 WHERE Type = 'GAMESPEED_EPIC';
UPDATE Gamespeeds SET MilitaryRatingDecayPercent = 7 WHERE Type = 'GAMESPEED_MARATHON';
