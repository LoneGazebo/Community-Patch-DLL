

/*
_______________City-State Diplomacy Mod (CSD) for Brave New World ________________

Thank you for downloading the City-State Diplomacy Mod! This mod is designed to completely overhaul the diplomatic portion of the game, particularly with regards to city-states.
More information on the mod can be found in the game's Civilopedia.

_______________OPTIONS FOR CSD____________________

All options below are preset to the default values of this mod.

Thanks:
- Thalassicus for help with SQL
- Seek for artwork and consulation
- The denizens of Civfanatics for supporting CSD and offering sage advice
- Whoward for invaluable dll assistance

Enjoy,
Gazebo
*/

--IMPORTANT!! PLEASE READ THE FOLLOWING:
 
--To edit the Options.SQL file, do the following steps:
--Scroll down until you see the following text:

/*
Gold Gifting and Gold-Related Quests with City-States On/Off
1 = Gold Gifts Enabled
2 = Gold Gifts Disabled (Default)
*/

--INSERT INTO CSD (Type, Value)
--VALUES ('GIFT_OPTION', 2);

--To enable gold gifts, change the integer after GIFT_OPTION to '1'
--The final code should look like this: 

--Code:
/*
Gold Gifting and Gold-Related Quests with City-States On/Off
1 = Gold Gifts Enabled
2 = Gold Gifts Disabled (Default)
*/

--INSERT INTO CSD (Type, Value)
--VALUES ('GIFT_OPTION', 1);
--Save the file and launch the game!




-- OPTIONS --
-- If you have not read the instructions above, please do so now!

/* CSD Difficulty and AI Competitiveness for City-States. Higher values than '2' make the AI more interested in City-States, and more sensitive to your actions with City-States.
1 = Less Competitive (Easy)
2 = Standard (Default)
3 = More Competitive (Hard)
4 = Extremely Competitive (Very Hard)
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_DIFFICULTY', 2);

/*
National Wonder Tweaks (CSD) - National Wonders now require a certain amount of national population instead of buildings in every city.
Will not work without enabling COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS (in the Community Balance Patch) as well!
1 = Enabled (Default)
0 = Disabled 
*/

INSERT INTO CSD (Type, Value)
VALUES ('COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD', 1);

/*
Gold Gifting and Gold-Related Quests with City-States On/Off
1 = Gold Gifts Enabled
2 = Gold Gifts Disabled (Default)
*/

INSERT INTO CSD (Type, Value)
VALUES ('GIFT_OPTION', 2);

/*
Changes to ideologies and World Congress
Also Enables Extra Vote Sources from Buildings
0 = Disabled (Careful with this!)
1 = Normal Difficulty (Default)
*/

INSERT INTO CSD (Type, Value)
VALUES ('IDEOLOGY_CSD', 1);

/*
CSD Unit Gold Hurrying Cost
1 = +40% of Production Value (Default)
2 = +60% of Production Value
3 = +80% of Production Value
4 = Gold Hurrying Disabled
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_GOLDHURRY', 1);

/*
CSD Patronage Policy Modifications
1 = Modified Patronage Policies Enabled (Default)
0 = Modified Patronage Policies Disabled
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_POLICIES', 1);

/*
CSD Beliefs and Traits
1 = Beliefs and Traits Enabled (Default)
2 = Beliefs and Traits Disabled
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_BONUSES', 2);

/*
Fix for Mausoleum of Halicarnassus (Wonders of the Ancient World DLC)
1 = Fix Enabled (Default)
2 = Fix Disabled 
*/

INSERT INTO CSD (Type, Value)
VALUES ('ANCIENT_WONDERS', 2);

/*
Sweden Trait Modification (G&K + Brave New World)
To compensate, Swedish Diplomatic Units have the Nobel Laureate Promotion which grants 10% more Influence from Diplomatic Missions.
3 = 40 Influence When Gifting Great People (Very Easy)
2 = 20 Influence When Gifting Great People (Easy)
1 = 0 Influence When Gifting Great People (Normal, Default)
0 = 90 Influence Per Mission (Vanilla- Not Recommended)
*/

INSERT INTO CSD (Type, Value)
VALUES ('SWEDEN_TRAIT', 0);

/*
Diplomatic Unit Influence - Base Gain (Brave New World)
5 = 60 Influence Per Mission (Super Duper Hard)
4 = 70 Influence Per Mission (Really Hard)
3 = 80 Influence Per Great Merchant Mission (Harder)
2 = 90 Influence Per Great Merchant Mission (Hard)
1 = 100 Influence Per Great Merchant Mission (Normal, Default)
*/

INSERT INTO CSD (Type, Value)
VALUES ('TRADE_MISSION', 1);

/*
City-State Gold Gifting Gold:Influence Ratio
Note: The higher the number, the less influence you will earn per gold gift as the game progresses. 
3 = 24 (Hard)
2 = 18 (Normal, Default)
1 = 12 Influence Per Mission (Easy)
0 = 6.3 Influence Per Mission (Very Easy, Vanilla)
*/

INSERT INTO CSD (Type, Value)
VALUES ('INFLUENCE_RATIO', 2);

/*
Small Gold Gift Cost
3 = 800 Gold (Higher)
2 = 600 Gold (Normal, Default)
1 = 400 Gold (Lower)
0 = 250 Gold (Vanilla)
*/

INSERT INTO CSD (Type, Value)
VALUES ('GIFT_SMALL', 2);

/*
Medium Gold Gift Cost
3 = 1300 Gold (Higher)
2 = 1000 Gold (Normal, Default)
1 = 700 Gold (Lower)
0 = 500 Gold (Vanilla)
*/

INSERT INTO CSD (Type, Value)
VALUES ('GIFT_MEDIUM', 2);

/*
Large Gold Gift Cost
3 = 2500 Gold (Higher)
2 = 2000 Gold (Normal, Default)
1 = 1500 Gold (Lower)
0 = 1000 Gold (Vanilla)
*/

INSERT INTO CSD (Type, Value)
VALUES ('GIFT_LARGE', 2);














---------------------- DO NOT MODIFY THESE UNLESS YOU KNOW WHAT YOU ARE DOING ----------------------------



/*
Gods and Kings and Brave New World Expansion Compatibility - DO NOT MODIFY THIS VALUE
1 = Using Gods and Kings Expansion (default)
0 = DO NOT USE
*/

INSERT INTO CSD (Type, Value)
VALUES ('USING_GAK', 1);

/*
Diplomatic Unit Influence Gain (Vanilla Civ V - DO NOT MODIFY)
3 = 60 Influence Per Mission (Easy)
2 = 50 Influence Per Mission (Normal, Default)
1 = 40 Influence Per Mission (Hard)
0 = 30 Influence Per Mission (Vanilla)
*/

INSERT INTO CSD (Type, Value)
VALUES ('TRADE_MISSION_VANILLA', 2);

/*
CSD AI (not the DLL!)
1 = Core AI Tweaks Enabled (Default)
2 = Core AI Tweaks Disabled
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_AI', 1);

/*
CSD Gameplay Text
1 = Core Gameplay Text Enabled (Default)
2 = Core Gameplay Text Disabled
*/

INSERT INTO CSD (Type, Value)
VALUES ('CSD_TEXT', 1);