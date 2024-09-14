UPDATE Worlds SET MinDistanceCities = 3, MinDistanceCityStates = 3;

UPDATE Worlds SET TradeRouteDistanceMod = 80 WHERE Type = 'WORLDSIZE_DUEL';
UPDATE Worlds SET TradeRouteDistanceMod = 80 WHERE Type = 'WORLDSIZE_TINY';
UPDATE Worlds SET TradeRouteDistanceMod = 90 WHERE Type = 'WORLDSIZE_SMALL';
UPDATE Worlds SET TradeRouteDistanceMod = 100 WHERE Type = 'WORLDSIZE_STANDARD';
UPDATE Worlds SET TradeRouteDistanceMod = 130 WHERE Type = 'WORLDSIZE_LARGE';
UPDATE Worlds SET TradeRouteDistanceMod = 160 WHERE Type = 'WORLDSIZE_HUGE';
