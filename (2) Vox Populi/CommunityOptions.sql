----------------------------------------------------------------------------
-- Thank you for downloading Vox Populi!
-- You can freely change the below option values.
-- All options below are preset to the default values of this mod.
----------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Resource visibility at Game Start
-- 0 = All Resources Visible at Game Start
-- 1 = Only Luxury Resources Visible at Game Start (Default)
-- 2 = Only Bonus Resources Visible at Game Start
-- 3 = No Resources Visible at Game Start
----------------------------------------------------------------------------

INSERT INTO Community (Type, Value) VALUES ('COMMUNITY_CORE_BALANCE_RESOURCE_REVEAL', 1);

----------------------------------------------------------------------------
-- Barbarian heal rate (only if they don't move)
-- 0 = No Healing
-- 1 = 10 HP per turn (Default)
-- 2 = 15 HP per turn
----------------------------------------------------------------------------

INSERT INTO Community (Type, Value) VALUES ('BARBARIAN_HEAL', 1);

----------------------------------------------------------------------------
-- World Wonders refund yields if you are beaten to them by another player
-- Individual conversion percentages can be configured in the Defines table
-- 0 = Disabled
-- 1 = 100% of Production is converted into Gold
-- 2 = 33% of Production is converted into Culture (Default)
-- 3 = 25% of Production is converted into Golden Age Points
-- 4 = 10% of Production is converted into Science
-- 5 = 10% of Production is converted into Faith
----------------------------------------------------------------------------

INSERT INTO Community (Type, Value) VALUES ('COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK', 2);
