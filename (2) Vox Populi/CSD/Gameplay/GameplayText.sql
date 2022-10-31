-- Gameplay Text

UPDATE Language_en_US
SET Text = 'How do Diplomatic Missions effect our relationship?'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_ADV_QUEST' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The most effective method of raising your Influence with a city-state is to create a Diplomatic Unit and send it on a Diplomatic Mission. A Diplomatic Mission allows diplomatic units to directly raise your Influence with city-states. To conduct a Diplomatic Mission, move your Diplomatic Unit into city-state territory and press the Diplomatic Mission button. The base amount of Influence gained from a Diplomatic Mission increases as a player unlocks new promotions for Diplomatic Units through new unit types, wonders and buildings. [ENDLINE][ENDLINE]Players may only have a limited number of Diplomatic Units active at any given time. The maximum number of active Diplomatic Units increases based on the amount of the [ICON_RES_PAPER] Paper a player controls. Once used to generate influence, Diplomatic Units are expended and must be rebuilt to use again.'
WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_GREEN]Diplomatic Missions and You[ENDCOLOR]'
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
SET Text = 'If the unit is inside City-State territory that you are not at war with, this order will expend the unit.[NEWLINE][NEWLINE]If a Great Diplomat, you will gain a large amount of [ICON_INFLUENCE] Influence with the City-State and the [ICON_INFLUENCE] of all other major civilizations known to the City-State will be decreased with this City-State by the same amount.[NEWLINE][NEWLINE] If this unit is a Great Merchant, you will receive a large amount of [ICON_GOLD] Gold and an instant "We Love the King Day" in all owned cities. This action will consume the unit.'
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

-- Civilopedia Updates

UPDATE Language_en_US
SET Text = 'A resolution is a change to the game world that is proposed to and decided by the civilizations of the World Congress. For example, a resolution can embargo trade routes with a certain civilization, increase the culture from Wonders, prevent the construction of nuclear weapons, or award someone Diplomatic Victory. A resolution may also repeal a previously passed resolution, reversing its effects. [NEWLINE][NEWLINE][COLOR_YELLOW]Only three civilizations get to propose resolutions: the current host of the Congress, and the other two players who have the most delegates.[ENDCOLOR][NEWLINE][NEWLINE]All civilizations get to help decide the outcome of proposed resolutions once the Congress is in session. They do so by allocating their delegates towards the outcome they desire: "Yea" to help it pass or "Nay" to help it fail. Some resolutions require choosing a civilization rather than simply "Yea" or "Nay".[NEWLINE][NEWLINE]Once all civilizations have used their delegates, the resolution''s outcome is decided by whichever option received the most delegate support. If there is a tie, the resolution fails.'
WHERE Tag = 'TXT_KEY_CONGRESS_RESOLUTIONS_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Resolutions[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_RESOLUTIONS_HEADING2_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Proposed resolutions to the Congress are decided through the use of delegates. Once a session of the Congress begins, civilizations divide their delegates as they like between proposed resolutions to support the outcomes they desire. Delegates support "Yea" in order to help pass the resolution, or "Nay" in order to help prevent it from passing. Some resolutions require choosing a civilization rather than simply "Yea" or "Nay".[NEWLINE][NEWLINE]Delegates can be obtained in the following ways:[NEWLINE][NEWLINE]  [COLOR_GREEN]Base Value[ENDCOLOR]: Each Civilization has a basic amount of Delegates depending on the current Era of the World Congress (starting with 1 Delegate when the World Congress is founded, then 1 additional Delegate per Era).[NEWLINE][NEWLINE] [COLOR_GREEN]Host Status[ENDCOLOR]: The Host of the World Congress gets 1-2 additional Delegates, depending on Era.[NEWLINE][NEWLINE]  [COLOR_GREEN]City State Alliances[ENDCOLOR]: Each City State Alliace grants 1 additional Delegate. Once the United Nations are founded, this number increases to 2 Delegates per Alliance.[NEWLINE][NEWLINE] [COLOR_GREEN]City State Embassies[ENDCOLOR]: [COLOR_YELLOW]Each Embassy built in a City State grants 1 additional Delegate.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]World Religions[ENDCOLOR]: If a Religion has been designated as the official World Religion by the World Congress, all Civilizations following it will get an additional Delegate. [COLOR_YELLOW]The founder of the World Religions also gets an additional Delegates for each foreign Civilization following the World Religion.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]Religious Authority[ENDCOLOR]: [COLOR_YELLOW]A Civilization that has founded a Religion and built the corresponding National Wonder gets 1 additional Delegate for every 10 Cities following the Religion.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]World Ideology[ENDCOLOR]: If an Ideology has been designated as the official World Ideology by the World Congress, all Civilizations following the ideology will get 1 additional Delegate [COLOR_YELLOW]and 1 additional Delegate for each foreign Civilization following the World Ideology.[ENDCOLOR][NEWLINE][NEWLINE]  [COLOR_GREEN]Policies and Wonders[ENDCOLOR]: Some Social Policies, National Wonders and World Wonders grant additional Delegates in different ways.[NEWLINE][NEWLINE]  [COLOR_GREEN]Globalization[ENDCOLOR]: When the Technology "Globalization" has been researched, each Diplomat in a foreign Capital grants 1 additional Delegate.'
WHERE Tag = 'TXT_KEY_CONGRESS_DELEGATES_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Delegates[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_DELEGATES_HEADING2_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Diplomatic Victory is only possible after the World Congress has become the United Nations. The United Nations are founded once half of the civilizations in the game reach the Atomic Era (or one civilization reaches the Information era). [COLOR_YELLOW]As a further requirement, the World Congress Resolution "United Nations" must have been passed and the United Nations World Wonder must have been built by a civilization. It is possible to achieve a Diplomatic Victory even if you have not built the United Nations yourself.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]The United Nations[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Once the World Congress has become the United Nations, Diplomatic Victory becomes possible if a World Ideology is active.[ENDCOLOR] To win Diplomatic Victory, a civilization must receive enough delegate support on a Global Hegemony resolution.[NEWLINE][NEWLINE]Unlike other resolutions, the Global Hegemony resolution cannot be proposed by a civilization. Instead, every other session of the United Nations is dedicated to deciding it if a World Ideology is active. During these sessions, which alternate with sessions of regular proposals, the World Leader resolution is automatically proposed to the Congress and no other proposals are made.[NEWLINE][NEWLINE]The amount of delegate support required to win depends on the number of civilizations and city-states in the game, and can be seen on the World Congress and Victory Progress screens. If no civilization receives enough support to win a World Leader resolution, the two civilizations that received the most support will permanently gain additional delegates.'
WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Diplomatic Victory[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'They will reward the player(s) with the largest number of new Followers.'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL} So far, the leader has {1_LeaderScore} new Followers and you have [COLOR_POSITIVE_TEXT]{2_PlayerScore}[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_LOSING_FORMAL' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

UPDATE Language_en_US
SET Text = '{TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_FORMAL} So far, you have the lead with [COLOR_POSITIVE_TEXT]{1_PlayerScore} new Followers'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_CONTEST_FAITH_WINNING_FORMAL' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );



