-- Scales the trade route turns between choosing a new destination based on game speed
-- On Standard speed the trade route target turns is 30 (TRADE_ROUTE_BASE_TARGET_TURNS)
-- On Quick speed the target turns will be 20, and on Marathon speed 90
-- The number of turns the AI remembers a plundered trade route is also scaled using this value
-- Need TRADE_ROUTE_SCALING mod option to function
ALTER TABLE GameSpeeds ADD TradeRouteSpeedMod integer DEFAULT 100;

-- Additional starting happiness for each player
ALTER TABLE GameSpeeds ADD StartingHappiness integer DEFAULT 0;

-- Modifier for all instant yields (and more)
ALTER TABLE GameSpeeds ADD InstantYieldPercent integer DEFAULT 100;

-- Modifier for all difficulty bonuses
ALTER TABLE GameSpeeds ADD DifficultyBonusPercent integer DEFAULT 100;

-- Percentage by which military rating decays each turn for different game speeds (affects AI strength perception behavior)
-- (10 = 1%)
ALTER TABLE GameSpeeds ADD MilitaryRatingDecayPercent integer DEFAULT 0;

-- How much does each turn of research add to tech cost?
ALTER TABLE GameSpeeds ADD TechCostPerTurnMultiplier float DEFAULT 30;

-- Number of turns before masters can liberate vassals
ALTER TABLE GameSpeeds ADD MinimumVassalLiberateTurns integer DEFAULT 50;

-- Number of turns before capitulated vassals can request independence
ALTER TABLE GameSpeeds ADD MinimumVassalTurns integer DEFAULT 50;

-- Number of turns before voluntary vassals can request independence
ALTER TABLE GameSpeeds ADD MinimumVoluntaryVassalTurns integer DEFAULT 10;

-- Number of turns before we can change vassal taxes
ALTER TABLE GameSpeeds ADD MinimumVassalTaxTurns integer DEFAULT 25;

-- Number of turns after vassalage ends before the former vassal can be vassalized by the same master again
ALTER TABLE GameSpeeds ADD NumTurnsBetweenVassals integer DEFAULT 30;

-- JFD stuff
ALTER TABLE GameSpeeds ADD PietyMin integer DEFAULT 0;
ALTER TABLE GameSpeeds ADD PietyMax integer DEFAULT 0;
