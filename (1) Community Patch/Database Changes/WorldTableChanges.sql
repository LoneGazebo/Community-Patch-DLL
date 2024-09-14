-- Scales the trade route distance (caravan/cargo ship) based on map size
-- On a Standard map, the base caravan length is 10 (TRADE_ROUTE_BASE_LAND_DISTANCE)
-- On a Tiny map it will be 8, and on a Huge map 16
ALTER TABLE Worlds ADD TradeRouteDistanceMod integer DEFAULT 100;
