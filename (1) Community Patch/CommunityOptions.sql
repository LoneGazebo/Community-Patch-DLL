/*
_______________Community Patch for Brave New World ________________

		Thank you for downloading the Community Patch!

_______________OPTIONS FOR Community Patch____________________

All options below are preset to the default values of this mod.

-- OPTIONS --

/*
Core Balance Tweaks - Changes from files in CoreBalance folder
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE', 1);

/*
Changes to Settler Behavior
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_SETTLER_TWEAKS', 1);

/*
Era-Specific Great Writers/Artists/Musicians
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_ERA_GREAT_WORKS', 1);

/*
AI Settlers allowed to move on turn 0
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_SETTLER_MOVE_START', 1);

/*
Great Engineers instantly build their project
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO COMMUNITY (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_ENGINEER_INSTANT_BUILD', 1);

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

-- DO NOT EDIT

UPDATE CustomModOptions
SET Value = '1'
WHERE Name = 'BALANCE_CORE_ENGINEER_HURRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_ENGINEER_INSTANT_BUILD' AND Value= 1 );