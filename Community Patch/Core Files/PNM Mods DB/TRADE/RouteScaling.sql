-- Scales the trade route distance based on map size
-- On a Standard map, the base caravan length is 10
-- On a Tiny map it will be 8, and on a Huge map 16
ALTER TABLE Worlds
  ADD TradeRouteDistanceMod INTEGER DEFAULT 100;

-- Calculated as the percentage of map diagonal vs standard maps
UPDATE Worlds SET TradeRouteDistanceMod=80 WHERE Type='WORLDSIZE_DUEL';
UPDATE Worlds SET TradeRouteDistanceMod=80 WHERE Type='WORLDSIZE_TINY';
UPDATE Worlds SET TradeRouteDistanceMod=90 WHERE Type='WORLDSIZE_SMALL';
UPDATE Worlds SET TradeRouteDistanceMod=100 WHERE Type='WORLDSIZE_STANDARD';
UPDATE Worlds SET TradeRouteDistanceMod=130 WHERE Type='WORLDSIZE_LARGE';
UPDATE Worlds SET TradeRouteDistanceMod=160 WHERE Type='WORLDSIZE_HUGE';


-- Scales the trade route turns between choosing a new destination based on game speed
-- On Standard speed the trade route target turns is 30 (rounded based on actual route length)
-- On Quick speed the target turns will be 20, and on Marathon speed 90
-- The number of turns the AI remembers a plundered trade route is also scaled using this value
ALTER TABLE GameSpeeds
  ADD TradeRouteSpeedMod INTEGER DEFAULT 100;

UPDATE GameSpeeds SET TradeRouteSpeedMod=67  WHERE Type='GAMESPEED_QUICK';
UPDATE GameSpeeds SET TradeRouteSpeedMod=100 WHERE Type='GAMESPEED_STANDARD';
UPDATE GameSpeeds SET TradeRouteSpeedMod=150 WHERE Type='GAMESPEED_EPIC';
UPDATE GameSpeeds SET TradeRouteSpeedMod=300 WHERE Type='GAMESPEED_MARATHON';


-- Number of tiles moved is now determined by the movement rate of the unit, (not hard-coded to 2 and 4)
UPDATE Units SET Moves=2 WHERE Type='UNIT_CARAVAN';
UPDATE Units SET Moves=4 WHERE Type='UNIT_CARGO_SHIP';


-- Trade route related constants (moved from hard-coded values int the <Defines> table)
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_TARGET_TURNS', 30);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_LAND_DISTANCE', 10);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_LAND_MODIFIER', 0);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_SEA_DISTANCE', 20);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_SEA_MODIFIER', 100);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_FOOD_VALUE', 300);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_PRODUCTION_VALUE', 300);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_SCIENCE_DIVISOR_TIMES100', 200);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_DIFFERENT_RESOURCE_VALUE', 50);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_RIVER_CITY_MODIFIER', 25);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_BASE_PLUNDER_GOLD', 100);
INSERT INTO Defines(Name, Value) VALUES('TRADE_ROUTE_PLUNDER_TURNS_COUNTER', 30);


INSERT INTO CustomModDbUpdates(Name, Value) VALUES('TRADE_ROUTE_SCALING', 1);
