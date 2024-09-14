-- Scales the trade route turns between choosing a new destination based on game speed
-- On Standard speed the trade route target turns is 30 (TRADE_ROUTE_BASE_TARGET_TURNS)
-- On Quick speed the target turns will be 20, and on Marathon speed 90
-- The number of turns the AI remembers a plundered trade route is also scaled using this value
ALTER TABLE GameSpeeds ADD TradeRouteSpeedMod integer DEFAULT 100;
