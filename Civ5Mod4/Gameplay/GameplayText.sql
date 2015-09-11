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
SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will have the unit conduct a Diplomatic Mission, giving you a large amount of [ICON_INFLUENCE] Influence with the City-State. If you use a Great Merchant in this way, you will also receive a large amount of [ICON_GOLD] Gold. This action will consume the unit.'
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