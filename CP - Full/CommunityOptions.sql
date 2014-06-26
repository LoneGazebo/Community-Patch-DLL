--DO NOT MODIFY THIS--
CREATE TABLE IF NOT EXISTS COMMUNITY (
Type text,
Value variant default 0
);
--DO NOT MODIFY ABOVE--

/*
_______________Community Patch for Brave New World ________________

Thank you for downloading the Community Patch!

_______________OPTIONS FOR Community Patch____________________

All options below are preset to the default values of this mod.

-- OPTIONS --
-- If you have not read the instructions above, please do so now!

/*
Core Balance Tweaks - Changes from files in CoreBalance folder
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE', 1);


/*
National Happiness Tweaks - Allows for variable bonuses/penalties from happiness/unhappiness
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS', 1);

/*
City Happiness Tweaks - Allows for variable bonuses/penalties from happiness/unhappiness
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_CITY_HAPPINESS', 1);

/*
Luxury Happiness Tweaks - Allows for scaling happiness bonuses for luxury bonuses based on population, era and number of cities.
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_LUXURY_HAPPINESS', 1);

/*
Changes to Settler Behavior
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_SETTLER_TWEAKS', 1);

/*
Changes to Military Behavior and Flavors
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_MILITARY_TWEAKS', 1);

/*
Changes to Policy Flavors
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_POLICY_TWEAKS', 1);

/*
Changes to City Strategy Flavors
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_CITYSTRATEGY_TWEAKS', 1);

/*
Changes to Civilization Traits
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_CIVS', 0);

/*
Changes to Promotions
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_PROMOTIONS', 1);


/*
COMMUNITY Global Difficulty Option
1 = Global Difficulty Enabled (Default)
2 = Global Difficulty Disabled
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('BALANCE_GLOBAL_DIFFICULTY_OPTION', 1);

/*
COMMUNITY Global Difficulty Level
0 = Global Difficulty - Disabled
1 = Global Difficulty - King
2 = Global Difficulty - Emperor (Default)
3 = Global Difficulty - Immortal
4 = Global Difficulty - Deity
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('BALANCE_GLOBAL_DIFFICULTY_LEVEL', 2);

/*
Changes to Settlers
0 = Boring Ol' Vanilla (Careful with this!)
1 = Pioneers and Colonists! (Default)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('SETTLER_COMMUNITY', 1);

/*
Barbarians Heal!
0 = No Healing
1 = 6HP out of camps, 12 in camps. (Default)
2 = 10HP out of camps, 20 in camps. (Hard)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('BARBARIAN_HEAL', 1);