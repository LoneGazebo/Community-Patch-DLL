-- Gameplay Text

UPDATE Language_en_US
SET Text = 'How do Diplomatic Missions effect our relationship?'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_ADV_QUEST' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The most effective method of raising your Influence with a city-state is to create a Diplomatic Unit and send it on a Diplomatic Mission. A Diplomatic Mission allows diplomatic units to directly raise your Influence with city-states. To conduct a Diplomatic Mission, move your Diplomatic Unit into city-state territory and press the Diplomatic Mission button. The base amount of Influence gained from a Diplomatic Mission increases as a player unlocks new promotions for Diplomatic Units through new unit types, wonders and buildings. [ENDLINE][ENDLINE]Players may only have a limited number of Diplomatic Units active at any given time. The maximum number of active Diplomatic Units increases based on the amount of the [ICON_RES_PAPER] Paper a player controls. Once used to generate influence, Diplomatic Units are expended and must be rebuilt to use again.'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Diplomatic Missions and You'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Diplomatic Missions'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You have done something to anger {1_CityStateName:textkey}. Perhaps walking through their borders, or going to war recently.[NEWLINE][NEWLINE]Your [ICON_INFLUENCE] Influence with them recovers {2_Num} per turn. You should send a Diplomatic Unit to hasten the recovery of your [ICON_INFLUENCE] Influence.'
WHERE Tag = 'TXT_KEY_ANGRY_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '{1_CityStateName:textkey} neither likes nor dislikes you. You should send a Diplomatic Unit to increase your [ICON_INFLUENCE] Influence.'
WHERE Tag = 'TXT_KEY_NEUTRAL_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Conduct Diplomatic Mission'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will expend the unit.[NEWLINE][NEWLINE]If a Great Diplomat, you will gain a large amount of [ICON_INFLUENCE] Influence with the City-State and the [ICON_INFLUENCE] of all other major civilizations known to the City-State will be decreased with this City-State by the same amount.[NEWLINE][NEWLINE] If this unit is a Great Merchant, you will receive a large amount of [ICON_GOLD] Gold and an instant ''We Love the King Day'' in all owned cities. This action will consume the unit.'
WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You have gained {1_Num} [ICON_GOLD] Gold and {2_Num} [ICON_INFLUENCE] Influence from the Diplomatic Mission!'
WHERE Tag = 'TXT_KEY_MERCHANT_RESULT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You may initiate one of the following Diplomatic Missions.'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You may not gift [ICON_GOLD] Gold to City-States. Send a Diplomatic Unit instead.'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT_CANT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'You may initiate a Diplomatic Mission that can improve your [ICON_INFLUENCE] Influence with this City-State.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Once the United Nations World Congress Project is completed, the World Congress becomes the United Nations. Diplomatic Victory becomes possible once this is achieved, and a World Ideology is active. To win Diplomatic Victory, a civilization must receive enough delegate support on a Global Hegemony resolution.[NEWLINE][NEWLINE]Unlike other resolutions, the World Leader resolution cannot be proposed by a civilization. Instead, every other session of the Congress is dedicated to deciding it. During these sessions, which alternate with sessions of regular proposals, the World Leader resolution is automatically proposed to the Congress and no other proposals are made.[NEWLINE][NEWLINE]The amount of delegate support required to win depends on the number of civilizations and city-states in the game, and can be seen on the World Congress and Victory Progress screens. If no civilization receives enough support to win a World Leader resolution, the two civilizations that received the most support will permanently gain additional delegates.'
WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Once the United Nations World Congress Project is completed, the World Congress becomes the United Nations. Diplomatic Victory becomes possible once this is achieved, and a World Ideology is active. These prerequisites unlocks the Elect World Leader resolution, the winner of which will achieve a Diplomatic Victory.'
WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );