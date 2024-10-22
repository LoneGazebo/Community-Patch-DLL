UPDATE Language_en_US
SET Text = 'Your counterspy {1_SpyRank} {2_SpyName} killed {TXT_KEY_GRAMMAR_A_AN << {3_CivAdj}} spy in {4_CityName} after they completed a Spy Mission. {5_MissionText}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_KILLED_A_SPY';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} was [COLOR_NEGATIVE_TEXT]killed[ENDCOLOR] while conducting their mission! Diplomatic relations with {3_CivName} have worsened. It will take some time until a replacement spy is recruited for {1_SpyRank} {2_SpyName}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_WAS_KILLED';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} killed!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_S';

UPDATE Language_en_US
SET Text = 'The people of {3_MinorCivName:textkey} look to worldly affairs for religious guidance. Whichever global religion can garner the most [ICON_PEACE] Followers in a period of {2_TurnsDuration} turns will gain [ICON_INFLUENCE] Influence with them. [COLOR_POSITIVE_TEXT]{1_TurnsRemaining} turns remaining.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = '{1_MinorCivName:textkey} calls for conversions!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = 'Another religion has impressed {1_MinorCivName:textkey} with its faith. Your faith growth was not enough, and your [ICON_INFLUENCE] Influence remains the same as before. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = 'You have killed a group of Barbarians near {1_CivName:textkey}! They are grateful, and your [ICON_INFLUENCE] Influence with them has increased by 15!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_BARB_KILLED';

UPDATE Language_en_US
SET Text = 'You have discovered Ancient Ruins! Sending {TXT_KEY_UNITCOMBAT_RECON} into the Ruins may uncover hidden secrets!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_FOUND_GOODY_HUT';

UPDATE Language_en_US
SET Text = 'If you have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and your population is Content, you only need to become Influential with 1 more civilization to be eligible to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_ACTIVE_PLAYER_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, {1_CivName} only needs their culture to become Influential with 1 more civilization to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_TT';

UPDATE Language_en_US
SET Text = 'If they have an Ideology, and their population is Content, an unmet civilization only needs their culture to become Influential with 1 more civilization to construct the Citizen Earth Protocol and win a Culture Victory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_ONE_UNMET_TT';

UPDATE Language_en_US
SET Text = 'If you have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and your population is Content, you only need to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_ACTIVE_PLAYER_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, {1_CivName} only needs their culture to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_TT';

UPDATE Language_en_US
SET Text = 'If they have [COLOR_POSITIVE_TEXT]two[ENDCOLOR] Tier 3 Tenets of an Ideology, and their population is Content, an unmet civilization only needs their culture to become Influential with 2 more civilizations to construct the Citizen Earth Protocol and win a Culture Victory.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CULTURE_VICTORY_WITHIN_TWO_UNMET_TT';

UPDATE Language_en_US
SET Text = 'You are currently using more {1_Resource:textkey} than you have! All Units which require it are [COLOR_NEGATIVE_TEXT]unable to heal[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT';

UPDATE Language_en_US
SET Text = 'Because {1_Resource:textkey} is connected to your trade network, the City of {2_CityName:textkey} enters "We Love the King Day", giving it a [ICON_FOOD] Growth bonus!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CITY_WLTKD';

UPDATE Language_en_US
SET Text = 'You have moved beyond the Modern Era. The ideas of modernity now permeate your society. Your people clamor for you to pick an Ideology for your civilization.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_ERA';

UPDATE Language_en_US
SET Text = 'Your people now consider themselves part of the Atomic Era, and the ideas of modernization permeate your society. Your people clamor for you to pick an Ideology for your civilization.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES';

UPDATE Language_en_US
SET Text = 'As a [COLOR_POSITIVE_TEXT]Mercantile[ENDCOLOR] City-State, their markets offer exotic goods to your people! (+{1_NumHappiness} [ICON_HAPPINESS_1] Happiness, +{2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE';

UPDATE Language_en_US
SET Text = 'As a [COLOR_POSITIVE_TEXT]Mercantile[ENDCOLOR] City-State, their markets spread trade of exotic goods across your empire! (+{1_NumHappiness} [ICON_HAPPINESS_1] Happiness, +{2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE';

UPDATE Language_en_US
SET Text = '({1_NumHappiness} [ICON_HAPPINESS_1] Happiness, {2_Gold} [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE';

UPDATE Language_en_US
SET Text = 'As a [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State, they will give you military Units and Science. (+{1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC';

UPDATE Language_en_US
SET Text = 'As a [COLOR_POSITIVE_TEXT]Militaristic[ENDCOLOR] City-State, they will regularly give you military Units and Science. (+{1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC';

UPDATE Language_en_US
SET Text = 'They will contribute fewer Units to your military, and less Science! ({1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC';

UPDATE Language_en_US
SET Text = 'They will no longer give you military Units or Science! ({1_Science} [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC';

UPDATE Language_en_US
SET Text = '{@1_CivName} achieves Global Hegemony'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS';

UPDATE Language_en_US
SET Text = 'Garnering the required support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, {@2_CivName} has achieved Global Hegemony.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS_DETAILS';

UPDATE Language_en_US
SET Text = 'Global Hegemony not achieved'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL';

UPDATE Language_en_US
SET Text = 'Without a clear winner garnering the support of at least {1_NumDelegates} {1_NumDelegates: plural 1?Delegate; other?Delegates;}, the Global Hegemony proposal fails. The top {2_NumCivilizations} {2_NumCivilizations: plural 1?Civilization has; other?Civilizations have;} permanently gained an additional Delegate.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL_DETAILS';

UPDATE Language_en_US
SET Text = 'You may now add a Reformation belief to your religion.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_ADD_REFORMATION_BELIEF';

-- World Wonder consolation yields
UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_CULTURE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value = 2);

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_GOLDEN_AGE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value = 3);

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_RESEARCH]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value = 4);

UPDATE Language_en_US
SET Text = '{1_CityName} can no longer work on {2_BldgName}. The lost [ICON_PRODUCTION] is converted into {3_NumGold}[ICON_PEACE]!'
WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT 1 FROM Community WHERE Type = 'COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value = 5);

-- Cultural influence level changes
UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Exotic[ENDCOLOR] to {1_Num}. We will no longer get any bonuses from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_1_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Exotic[ENDCOLOR] to {1_Num}! See Culture Overview for the bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_1';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Familiar[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_2_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Familiar[ENDCOLOR] to {1_Num}!  See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_2';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Popular[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_3_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Popular[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_3';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Influential[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_4_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Influential[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_4';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Dominant[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_5_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Dominant[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_5';

-- City-State quests
UPDATE Language_en_US
SET Text = 'War breaks out between {2_CivName:textkey} and {1_TargetName:textkey}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{3_TargetName:textkey} has declared war on {1_CivName:textkey}, citing diplomatic concerns. Receive the [COLOR_POSITIVE_TEXT]full[ENDCOLOR] Quest Reward from {1_TargetName:textkey} by [COLOR_NEGATIVE_TEXT]destroying[ENDCOLOR] {3_CivName:textkey}, or a [COLOR_POSITIVE_TEXT]partial[ENDCOLOR] Quest Reward through [COLOR_POSITIVE_TEXT]allying[ENDCOLOR] both City-States. International pressure will force peace in [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] {2_Num: plural 1?Turn; other?Turns;}, so act fast if you intend to intervene!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = '{1_TargetName:textkey} defeated by {2_CivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_en_US
SET Text = 'The war between {2_CivName:textkey} and {1_TargetName:textkey} has ended, with {2_CivName:textkey} emerging as the clear victor. Your advisors worry that this outcome may lead to future conflict.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE';

-- City Revolt Brewing
UPDATE Language_en_US
SET Text = 'Because your Empire''s approval rating is below 35%, a City will revolt in [COLOR_NEGATIVE_TEXT]{1_Turns}[ENDCOLOR] turns. Based on current Unhappiness levels and Cultural/Ideological pressure, the City most likely to revolt is {2_CityName} and it will join {3_CivName}. Try to get your Empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT';

-- Revolutionary Unrest Brewing
UPDATE Language_en_US
SET Text = 'Because your Empire''s approval rating is below 35%, a City will revolt in [COLOR_NEGATIVE_TEXT]{1_Turns}[ENDCOLOR] {1_Turns: plural 1?Turn; other?Turns;}. Based on current Unhappiness levels, the City most likely to revolt is {2_CityName}. If this City revolts, it will restore the sovereignty of {3_CivName}. Try to get your Empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLUTION_CP';

UPDATE Language_en_US
SET Text = 'Because your Empire''s approval rating is below 35%, a City will revolt in [COLOR_NEGATIVE_TEXT]{1_Turns}[ENDCOLOR] {1_Turns: plural 1?Turn; other?Turns;}. Based on current Unhappiness levels, the City most likely to revolt is {2_CityName}. If this City revolts, it will become an independent City-State. Try to get your Empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLUTION_CP_FREE_CITY';

-- A City Revolts!
UPDATE Language_en_US
SET Text = 'Because your Empire''s approval rating is below 35%, the City of {1_CityName} has revolted and joined {2_CivName}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CITY_REVOLT';

-- Rebels Appear!
UPDATE Language_en_US
SET Text = 'Because your Empire''s approval rating is below 35%, an uprising has occurred in your territory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_REBELS';
