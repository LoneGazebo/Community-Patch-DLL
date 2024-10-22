-- Scales the trade route distance (caravan/cargo ship) based on map size
-- On a Standard map, the base caravan length is 10 (TRADE_ROUTE_BASE_LAND_DISTANCE)
-- On a Tiny map it will be 8, and on a Huge map 16
-- Need TRADE_ROUTE_SCALING mod option to function
ALTER TABLE Worlds ADD TradeRouteDistanceMod integer DEFAULT 100;

-- Plot distance from each existing city that cannot be settled by major civilizations
ALTER TABLE Worlds ADD MinDistanceCities integer DEFAULT 0;

-- Plot distance from each existing city that cannot be settled by city states
ALTER TABLE Worlds ADD MinDistanceCityStates integer DEFAULT 0;

-- % Multiplier to follower% required for reformation
ALTER TABLE Worlds ADD ReformationPercentRequired integer DEFAULT 100;

-- Modifier to tourism from player A to player B for each non-puppet city player A has more than player B
ALTER TABLE Worlds ADD NumCitiesTourismCostMod integer DEFAULT 0;

-- Modifier to flat supply and supply from population for each non-puppet city
ALTER TABLE Worlds ADD NumCitiesUnitSupplyMod integer DEFAULT 0;
