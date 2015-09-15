/*
_______________Community Balance Patch for Brave New World ________________

Thank you for downloading the Community Balance Patch!

_______________OPTIONS FOR Community Balance Patch____________________

All options below are preset to the default values of this mod.

-- OPTIONS --
-- If you have not read the instructions above, please do so now!
-- Disabling files below will only remove the SQL changes. If you also want to remove the XML changes, you will need to delete the associated XML files.
/*
Changes to Civilization Traits
0 = All Resources Visible at Game Start
1 = Only Luxury Resources Visible at Game Start
2 = Only Bonus Resources Visible at Game Start
3 = No Resources Visible at Game Start
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_RESOURCE_REVEAL', 1);


/*
Changes to Civilization Traits
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_CIVS', 1);

/*
Changes to Buildings
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_BUILDINGS', 1);

/*
Enable/Disable Building and Unit Gold purchase cooldown values
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_BUILDINGS_COOLDOWN', 1);

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_UNITS_COOLDOWN', 1);

/*
Changes to Units
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_UNITS', 1);

/*
Changes to Leaders
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_LEADERS', 1);

/*
Changes to Terrain
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_TERRAIN', 1);

/*
Changes to Promotions
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_PROMOTIONS', 1);

/*
Purchases in Cities scale with # of cities
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_PURCHASE_CITY_MOD', 1);

/*
Barbarians can Steal Gold
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BARBARIAN_THEFT', 1);

/*
Enables No More Civilian Traffic Jams (by Pazyryk)
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_NMCJT', 1);

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
1 = 10 in camps. (Default)
2 = 15 in camps. (Hard)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('BARBARIAN_HEAL', 1);

/*
Policy Tweaks - Changes to game policies and ideologies
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_POLICIES', 1);

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

/*
Changes to Minors
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_MINORS', 1);
