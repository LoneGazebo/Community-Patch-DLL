/*
_______________Community Balance Overhaul for Brave New World ________________

Thank you for downloading the Community Balance Overhaul!

_______________OPTIONS FOR Community Balance Overhaul____________________

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
Barbarians Heal!
0 = No Healing
1 = 10. (Default)
2 = 15. (Hard)
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('BARBARIAN_HEAL', 1);

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