/*
_______________Community Balance Patch for Brave New World ________________

Thank you for downloading the Community Balance Patch!

_______________OPTIONS FOR Community Balance Patch____________________

All options below are preset to the default values of this mod.

-- OPTIONS --
-- If you have not read the instructions above, please do so now!

/*
Enables Support for City-State Diplomacy Mod
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CSD_COMPATIBILITY', 1);

/*
Enables Support for Civ 4 Diplomacy Features Mod
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CIVIVDF_COMPATIBILITY', 1);

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
VALUES ('COMMUNITY_CORE_BALANCE_PROMOTIONS', 0);

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
0 = Boring Ol' Vanilla (Careful)
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
National Wonder Tweaks - National Wonders now require a certain amount of national population instead of buildings in every city.
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS', 1);

/*
Wonder Consolation Tweaks - Wonders grant different yields if you are beaten to them by another player Choose one value below.
1 = Gold
2 = Culture (Default)
3 = Golden Age Points
4 = Science
5 = Faith
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK', 2);

/*
Wonder Consolation Tweaks Enabler - Wonders grant different yields if you are beaten to them by another player Choose one value below.
0 = Disabled
1 = Enabled (Default)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK_ENABLER', 1);

/*
Religion Balance
0 = Disabled
1 = Enabled (Default)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_RELIGION', 1);

/*
Tourism Changes
0 = Disabled
1 = Enabled (Default)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_TOURISM_CHANGES', 1);


