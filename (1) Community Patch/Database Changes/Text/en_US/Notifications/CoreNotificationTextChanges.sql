UPDATE Language_en_US
SET Text = '[SPACE]You have received {1_Num} [ICON_GOLD] Gold!'
WHERE Tag = 'TXT_KEY_MISC_RECEIVED_GOLD';

UPDATE Language_en_US
SET Text = 'MOVE UNIT'
WHERE Tag = 'TXT_KEY_MOVE_STACKED_UNIT';

UPDATE Language_en_US
SET Text = 'Move a unit to a tile where it can end the turn.'
WHERE Tag = 'TXT_KEY_MOVE_STACKED_UNIT_TT';

UPDATE Language_en_US
SET Text = '{1_CivName:textkey} lost {1_CivName: plural 1?its; other?their;} [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_PLAYER_LOST_CAPITAL';

-- Rig elections
UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} successfully rigged the local elections in {3_CityName}. Your influence was increased by {4_Num}. The influence of other civilizations was reduced.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_SUCCESS';

UPDATE Language_en_US
SET Text = 'After recent elections, your influence in {1_CityName} was surprisingly reduced by {2_Num}. This probably reflects the involvement of enemy spies rigging the local elections.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_ALERT';

UPDATE Language_en_US
SET Text = '{1_SpyRank} {2_SpyName} failed to rig the local elections in {3_CityName}. {4_CivShortDesc} succeeded in rigging them and have gained influence there. Your influence was reduced by {5_Num}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SPY_RIG_ELECTION_FAILURE';

-- Fixed quest text - transitioned to new 'quest rewards' panel
UPDATE Language_en_US
SET Text = 'You have successfully destroyed the Barbarian Encampment as requested by {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully connected {1_ResourceName} to your trade network! Their merchants are quite pleased!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully constructed {1_WonderName}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER';

UPDATE Language_en_US
SET Text = 'As {2_MinorCivName:textkey} requested, you have successfully created a {1_UnitName}! Their scholars are in awe.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON';

UPDATE Language_en_US
SET Text = 'Your discovery of a new Natural Wonder has inspired the citizens of {1_MinorCivName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER';

UPDATE Language_en_US
SET Text = 'Your discovery of {1_TargetName:textkey} has greatly pleased the merchants of {2_MinorCivName:textkey}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER';

UPDATE Language_en_US
SET Text = 'Your gift of gold has helped {2_MinorCivName:textkey} recover from the tribute paid to {1_BullyName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} enjoyed seeing its rival {1_TargetName:textkey} squirm and give up hard-earned resources.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE';

UPDATE Language_en_US
SET Text = 'Your promise to protect {2_MinorCivName:textkey} has helped them recover from the tribute paid to {1_BullyName:textkey}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT';

UPDATE Language_en_US
SET Text = '{2_MinorCivName:textkey} is relieved that the injustice they suffered at the hands of {1_TargetName:textkey} is now known by major world powers.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR';

UPDATE Language_en_US
SET Text = 'Citizens of {@2_MinorCivName} are glad to see {@1_ReligionName} come to their city and become the dominant religion.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION';

UPDATE Language_en_US
SET Text = 'Citizens of {@1_MinorCivName} are glad to see a connected trade route.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE';

UPDATE Language_en_US
SET Text = 'You have impressed {1_MinorCivName:textkey} with your culture! They turn a blind eye to the culture of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its culture. Your cultural growth was not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE';

UPDATE Language_en_US
SET Text = 'The priesthood of {1_MinorCivName:textkey} are moved by your piety. They shun the inferior faith of other civilizations. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its faith. Your faith growth was not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH';

UPDATE Language_en_US
SET Text = '{1_MinorCivName:textkey} is amazed by your swift technological advancements! Their scientists and intellectuals dismiss other civilizations as stagnant. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS';

UPDATE Language_en_US
SET Text = 'Another civilization has impressed {1_MinorCivName:textkey} with its technological advancement. Your scientific discoveries were not enough. Civilizations that succeeded (ties are allowed):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS';

-- City Revolt Brewing
UPDATE Language_en_US
SET Text = 'Because your Empire has at least 20 [ICON_HAPPINESS_4] Unhappiness (or because our Public Opinion is not Content), a City will revolt in [COLOR_NEGATIVE_TEXT]{1_Turns}[ENDCOLOR] turns. Based on current Unhappiness levels and Cultural/Ideological pressure, the City most likely to revolt is {2_CityName} and it will join {3_CivName}. Try to get your Empire''s Happiness level out of this range as soon as possible!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_POSSIBLE_CITY_REVOLT';

-- A City Revolts!
UPDATE Language_en_US
SET Text = 'Because your Empire has at least 20 [ICON_HAPPINESS_4] Unhappiness, the City of {1_CityName} has revolted and joined {2_CivName}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CITY_REVOLT';

-- Rebels Appear!
UPDATE Language_en_US
SET Text = 'Because your Empire has at least 20 [ICON_HAPPINESS_4] Unhappiness, an uprising has occurred in your territory!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_REBELS';

-- Pantheon Founded
UPDATE Language_en_US
SET Text = 'Your people have started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{1_BeliefName}[ENDCOLOR] ({@2_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_ACTIVE_PLAYER';

UPDATE Language_en_US
SET Text = '{@1_CivName} {@1: plural 1?has; 2?have;} started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{2_BeliefName}[ENDCOLOR] ({@3_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED';

UPDATE Language_en_US
SET Text = 'An unknown civilization has started worshipping a pantheon of gods. They have chosen the belief: [COLOR_POSITIVE_TEXT]{1_BeliefName}[ENDCOLOR] ({@2_BeliefDesc})'
WHERE Tag = 'TXT_KEY_NOTIFICATION_PANTHEON_FOUNDED_UNKNOWN';
