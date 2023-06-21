-- Diplo Opinion Weights
-- All opinion weights are listed here, even the unmodified ones, for easier modding.

-- Opinion Thresholds
-- Defines how much positive or negative Opinion is required for a specific opinion rating.
UPDATE Defines SET Value = '160' WHERE Name = 'OPINION_THRESHOLD_UNFORGIVABLE';
UPDATE Defines SET Value = '80' WHERE Name = 'OPINION_THRESHOLD_ENEMY';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_THRESHOLD_COMPETITOR';

UPDATE Defines SET Value = '-30' WHERE Name = 'OPINION_THRESHOLD_FAVORABLE';
UPDATE Defines SET Value = '-80' WHERE Name = 'OPINION_THRESHOLD_FRIEND';
UPDATE Defines SET Value = '-160' WHERE Name = 'OPINION_THRESHOLD_ALLY';

-- Major Modifier Thresholds
-- Opinion modifiers with a value higher than these thresholds will display in bright red/green instead of dark red/green
INSERT INTO Defines (Name, Value) SELECT 'OPINION_THRESHOLD_MAJOR_POSITIVE', '-15'; -- for positive modifiers
INSERT INTO Defines (Name, Value) SELECT 'OPINION_THRESHOLD_MAJOR_NEGATIVE', '15'; -- for negative modifiers


-- Opinion Modifiers

--	//////////////////////////////////////
--	// DISPUTE MODIFIERS
--	//////////////////////////////////////

-- Territorial disputes strain your relationship. / You have no contested borders.
-- NOTE: This modifier scales based on the AI's Boldness flavor and the player's difficulty level.
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_LAND_FIERCE';
UPDATE Defines SET Value = '25' WHERE Name = 'OPINION_WEIGHT_LAND_STRONG';
UPDATE Defines SET Value = '15' WHERE Name = 'OPINION_WEIGHT_LAND_WEAK';
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_LAND_NONE';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LAND_WARMONGER', '10'; -- penalty for Conqueror AIs
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LAND_NONE_WARMONGER', '-5'; -- bonus for Conqueror AIs (increases based on Neediness flavor)

-- You are (not) competing for World Wonders.
-- NOTE: This modifier scales based on the AI's WonderCompetitiveness flavor and the player's difficulty level.
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_WONDER_FIERCE';
UPDATE Defines SET Value = '35' WHERE Name = 'OPINION_WEIGHT_WONDER_STRONG';
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_WONDER_WEAK';
UPDATE Defines SET Value = '0' WHERE Name = 'OPINION_WEIGHT_WONDER_NONE';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_CULTURAL', '10'; -- penalty for Cultural AIs
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_NONE_CULTURAL', '-5'; -- bonus for Cultural AIs (increases based on Neediness flavor)

-- You are (not) competing for the favor of the same City-States!
-- NOTE: This modifier scales based on the AI's MinorCivCompetitiveness flavor and the player's difficulty level.
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_FIERCE';
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_STRONG';
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_WEAK';
UPDATE Defines SET Value = '0' WHERE Name = 'OPINION_WEIGHT_MINOR_CIV_NONE';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MINOR_CIV_DIPLOMAT', '10'; -- penalty for Diplomat AIs
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MINOR_CIV_NONE_DIPLOMAT', '-5'; -- bonus for Diplomat AIs (increases based on Neediness flavor)

-- You are (not) competing for technological advancement. (only for Scientific AIs)
-- NOTE: This modifier scales based on the AI's DiploBalance flavor and the player's difficulty level.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_TECH_FIERCE', '30';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_TECH_STRONG', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_TECH_WEAK', '10';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_TECH_NONE', '-5'; -- increases based on Neediness flavor

-- You are (not) competing for cultural advancement. (only for Cultural AIs)
-- NOTE: This modifier scales based on the AI's DiploBalance flavor and the player's difficulty level.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_POLICY_FIERCE', '30';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_POLICY_STRONG', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_POLICY_WEAK', '10';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_POLICY_NONE', '-5'; -- increases based on Neediness flavor

-- NOTE: This modifier scales based on the AI's VictoryCompetitiveness flavor and the player's difficulty level.
-- They fear/suspect you are competing with them. / They know you are competing with them, and they hate it!
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_VICTORY_FIERCE';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_VICTORY_STRONG';
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_VICTORY_WEAK';
UPDATE Defines SET Value = '0' WHERE Name = 'OPINION_WEIGHT_VICTORY_NONE';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_PER_ERA', '4'; -- increase per era if > 0

-- NOTE: This modifier scales based on the AI's VictoryCompetitiveness flavor and the player's difficulty level.
-- Your behavior worries/angers/infuriates them.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_FIERCE', '40';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_STRONG', '30';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_WEAK', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_NONE', '0';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VICTORY_BLOCK_PER_ERA', '4'; -- increase per era if > 0


-- Reckless Expander: Too many cities/land without enough military to back it up
-- AI only applies this if nearby, competing for victory, and you have more land/cities than they do
-- NOTE: This penalty scales based on the AI's Boldness flavor and the player's difficulty level.

INSERT INTO Defines (Name, Value) SELECT 'RECKLESS_EXPANDER_CITIES_THRESHOLD', '200'; -- must have at least this % city count compared to the median
INSERT INTO Defines (Name, Value) SELECT 'RECKLESS_EXPANDER_LAND_THRESHOLD', '250'; -- OR must have at least this % tile count compared to the median

UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_RECKLESS_EXPANDER'; -- base opinion penalty; the highest of the two penalties below is added to it
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_PER_CITY', '10'; -- penalty for each city above the median city count (unless your cities - their cities = a smaller value, in which case that one is used)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_PER_TILE', '1'; -- penalty for each tile above the median tile count (unless your tiles - their tiles = a smaller value, in which case that one is used)

INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RECKLESS_EXPANDER_STRATEGIC_MOD', '20'; -- extra penalty for Conqueror AIs


-- Wonder Spammer: Too many World Wonders compared to other civs
-- AI only applies this if nearby or Cultural, they're competing for victory, and you have more Wonders than they do
-- Captured Wonders, Corporations and World Congress Wonders do not count for this penalty!
-- NOTE: This penalty scales based on the AI's WonderCompetitiveness flavor and the player's difficulty level. At lower difficulties the AI is also slightly more tolerant of Wonder spamming.

INSERT INTO Defines (Name, Value) SELECT 'WONDER_SPAMMER_THRESHOLD', '3'; -- must have constructed this many more Wonders than the median (only counting Wonder-building civs); must also have at least 50% more than the global average

INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_SPAMMER', '20'; -- base opinion penalty
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_PER_WONDER', '5'; -- penalty for each wonder above WONDER_SPAMMER_THRESHOLD (unless your Wonders - their Wonders = a smaller value, in which case that one is used)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_CAP', '60'; -- max. opinion penalty from Wonders constructed count

INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WONDER_SPAMMER_STRATEGIC_MOD', '20'; -- extra penalty for Conqueror AIs


--	//////////////////////////////////////
--	// WAR STUFF
--	//////////////////////////////////////

-- You plundered their trade routes!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PLUNDERED_TRADE_ROUTE', '5'; -- x2 for origin civ

-- You killed or captured their civilians! / You killed or captured civilians during war!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_MAX', '50'; -- maximum opinion penalty
INSERT INTO Defines (Name, Value) SELECT 'CIVILIAN_KILLER_VALUE_PER_OPINION_WEIGHT', '100'; -- increase this to reach the maximum more quickly
INSERT INTO Defines (Name, Value) SELECT 'CIVILIAN_KILLER_VALUE_PER_TURN_DECAY', '100'; -- how fast this penalty decays per turn
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_WORLD_THRESHOLD', '25'; -- threshold for global penalty; penalty score must be at least this high with another civ that they don't hate
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CIVILIAN_KILLER_WORLD', '20'; -- global penalty if you're at max with any civ; higher of the two is used

-- You nuked them!
UPDATE Defines SET Value = '100' WHERE Name = 'OPINION_WEIGHT_NUKED_MAX';

-- You captured their original capital / Holy City!
UPDATE Defines SET Value = '160' WHERE Name = 'OPINION_WEIGHT_CAPTURED_CAPITAL';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPTURED_HOLY_CITY', '80';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPTURED_KEY_CITY_RETURNED_DIVISOR', '200'; -- penalty reduction for returning the captured city to the AI
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPTURED_KEY_CITY_CAPITULATION_DIVISOR', '200'; -- penalty reduction for capitulated vassals; x100 then divided by this; only applies if not treating them badly

-- Your military deployment is (extremely) threatening.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_LOW', '10';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_MEDIUM', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_HIGH', '40';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_INCREDIBLE', '60';


--	//////////////////////////////////////
--	// Player has done nice stuff
--	//////////////////////////////////////

-- We are trade partners.  (n.b. maximum recent trade value = OPINION_WEIGHT_TRADE_MAX * DEAL_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
UPDATE Defines SET Value = '-40' WHERE Name = 'OPINION_WEIGHT_TRADE_MAX'; -- maximum opinion bonus from recent trade value
UPDATE Defines SET Value = '5' WHERE Name = 'DEAL_VALUE_PER_OPINION_WEIGHT'; -- how much recent trade value equals -1 opinion weight
UPDATE Defines SET Value = '2' WHERE Name = 'DEAL_VALUE_PER_TURN_DECAY'; -- how fast recent trade value decays
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_STRATEGIC_TRADE_PARTNER_MULTIPLIER', '150'; -- increases current and maximum trade bonus if player is considered a strategic trade partner

-- Your recent diplomatic actions please/disappoint them. (n.b. maximum recent assist value = OPINION_WEIGHT_ASSIST_MAX * ASSIST_VALUE_PER_OPINION_WEIGHT; scales with game speed)
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_ASSIST_MAX'; -- maximum opinion bonus/penalty from recent assist value
INSERT INTO Defines (Name, Value) SELECT 'ASSIST_VALUE_PER_OPINION_WEIGHT', '5'; -- how much recent assist value equals +/- 1 opinion weight
INSERT INTO Defines (Name, Value) SELECT 'ASSIST_VALUE_PER_TURN_DECAY', '3'; -- how fast recent assist value decays

-- We fought together against a common foe. (n.b. maximum common foe value = OPINION_WEIGHT_COMMON_FOE_MAX * COMMON_FOE_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
UPDATE Defines SET Value = '-100' WHERE Name = 'OPINION_WEIGHT_COMMON_FOE_MAX';
UPDATE Defines SET Value = '50' WHERE Name = 'COMMON_FOE_VALUE_PER_OPINION_WEIGHT'; -- how much common foe value equals -1 opinion weight
UPDATE Defines SET Value = '25' WHERE Name = 'COMMON_FOE_VALUE_PER_TURN_DECAY'; -- how fast common foe value decays

-- You freed their captured citizens!
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_RETURNED_CIVILIAN';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RETURNED_CIVILIAN_SUBSEQUENT', '-5'; -- any after the first; must be a lesser bonus or equal to the initial value

-- You built a Landmark in their territory.
UPDATE Defines SET Value = '-20' WHERE Name = 'OPINION_WEIGHT_BUILT_LANDMARK';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_BUILT_LANDMARK_SUBSEQUENT', '-10'; -- any after the first; must be a lesser bonus or equal to the initial value

-- You restored their civilization after they were annihilated!
UPDATE Defines SET Value = '-200' WHERE Name = 'OPINION_WEIGHT_RESURRECTED';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATOR_CAPTURED_CAPITAL_DIVISOR', '200'; -- penalty to resurrection bonus if player captured AI's original capital
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATOR_CAPTURED_HOLY_CITY_DIVISOR', '200'; -- penalty to several liberation bonuses if player captured AI's Holy City

-- You liberated their original capital / Holy City.
UPDATE Defines SET Value = '-120' WHERE Name = 'OPINION_WEIGHT_LIBERATED_CAPITAL';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATED_CAPITAL_VASSAL_MULTIPLIER', '150';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATED_HOLY_CITY', '-80';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATED_HOLY_CITY_VASSAL_MULTIPLIER', '150';

-- You have liberated some of their people!
UPDATE Defines SET Value = '-30' WHERE Name = 'OPINION_WEIGHT_LIBERATED_CITY';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_LIBERATED_CITY_VASSAL_MULTIPLIER', '200';

-- You gave them back their original capital / Holy City.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RETURNED_CAPITAL', '-60';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RETURNED_CAPITAL_VASSAL_MULTIPLIER', '150';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RETURNED_HOLY_CITY', '-40';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RETURNED_HOLY_CITY_VASSAL_MULTIPLIER', '150';

-- We have shared embassies. / They have an embassy in our capital. / We have an embassy in their capital.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_EMBASSY_MUTUAL', '-3';
UPDATE Defines SET Value = '-2' WHERE Name = 'OPINION_WEIGHT_EMBASSY';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_EMBASSY_THEM', '-1';

-- We have a Diplomat in their capital. (no bonus for capitulated vassals unless Content, no bonus for voluntary vassals unless Content, Disagreeing or Resurrected)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DIPLOMAT', '-15';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DIPLOMAT_MOD', '-10'; -- bonus for Diplomat AIs

-- You forgave them for spying.
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_FORGAVE_FOR_SPYING';

-- You have shared intrigue with them.
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_INTRIGUE_SHARED_BY';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_INTRIGUE_SHARED_BY_SUBSEQUENT', '-5'; -- any after the first; must be a lesser bonus or equal to the initial value


--	//////////////////////////////////////
--	// Player has done mean things
--	//////////////////////////////////////

-- You stole their territory!
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_CULTURE_BOMBED'; -- multiplied by 1 to 6 for each theft depending on the value of the stolen tile(s)

-- Your spies were caught stealing from them.
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_ROBBED_BY';

-- You were caught plotting against them.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PLOTTED_AGAINST_US', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PLOTTED_AGAINST_US_SUBSEQUENT', '10'; -- any after the first; must be a lesser penalty or equal to the initial value

-- Your spies usurped their City-State Alliance in a coup d'état.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PERFORMED_COUP', '30';

-- You excavated their cultural artifacts!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_EXCAVATED_ARTIFACT', '30';


--	//////////////////////////////////////
--	// Player has asked us to do things we don't like
--	//////////////////////////////////////

-- You demanded they not settle near your lands! (if they refused the request)
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_ASKED_NO_SETTLE';

-- You asked them not to spy on you. (if they refused the request)
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_ASKED_STOP_SPYING';

-- You made a trade demand of them!
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_MADE_DEMAND_OF_US';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MADE_DEMAND_OF_US_SUBSEQUENT', '10'; -- any after the first; must be a lesser penalty or equal to the initial value


--	//////////////////////////////////////
--	// DENOUNCING
--	//////////////////////////////////////

-- We have denounced them/each other! / They have denounced us!
UPDATE Defines SET Value = '35' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ME'; -- player denounced AI
UPDATE Defines SET Value = '35' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_THEM'; -- AI denounced player
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MUTUAL_DENOUNCEMENT', '50'; -- both of these

-- You have Denounced a leader they made a Declaration of Friendship with!
-- NOTE: This penalty scales based on the AI's Loyalty flavor.
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_FRIEND';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_FRIEND_SUBSEQUENT', '8'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_MOST_VALUED_FRIEND', '20'; -- denounced the AI's most valued friend
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_MOST_VALUED_ALLY', '10'; -- denounced the AI's most valued DP (if they have a DoF with them)

-- We have Denounced the same leaders!
-- NOTE: This bonus scales based on the AI's DenounceWillingness flavor.
UPDATE Defines SET Value = '-20' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ENEMY';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_ENEMY_SUBSEQUENT', '-8'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_BIGGEST_COMPETITOR', '-20'; -- mutual denouncement against AI's biggest competitor
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_BIGGEST_LEAGUE_RIVAL', '-20'; -- mutual denouncement against AI's biggest World Congress rival

-- Other civs that they like more than you have denounced you!
-- NOTE: This penalty scales inversely (negatively) based on the AI's DiploBalance flavor.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND', '15';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND_SUBSEQUENT', '5'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DENOUNCED_BY_THEIR_KEY_FRIEND', '15'; -- additional penalty if the denouncement comes from a teammate or their most valued friend/ally

--	//////////////////////////////////////
--	// PROMISES
--	//////////////////////////////////////

-- BROKEN = player made a promise and then broke it
-- IGNORED = player ignored the AI's request to make a promise

-- AI asked player not to declare war on them
UPDATE Defines SET Value = '80' WHERE Name = 'OPINION_WEIGHT_BROKEN_MILITARY_PROMISE';
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD'; -- global penalty; applied if no personal penalty for breaking the promise
UPDATE Defines SET Value = '0' WHERE Name = 'OPINION_WEIGHT_IGNORED_MILITARY_PROMISE';

-- AI asked player to stop settling near them
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_EXPANSION_PROMISE';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_IGNORED_EXPANSION_PROMISE';

-- AI asked player to stop buying land near them
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_BORDER_PROMISE';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_IGNORED_BORDER_PROMISE';

-- AI asked player not to conquer their protected City-States
UPDATE Defines SET Value = '80' WHERE Name = 'OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE';
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD'; -- global penalty; applied if no personal penalty for breaking the promise
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE';

-- AI asked player not to bully their protected City-States
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_BROKEN_BULLY_CITY_STATE_PROMISE', '40';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_IGNORED_BULLY_CITY_STATE_PROMISE', '20';

-- AI asked player to stop converting their cities to another religion
-- These values are multiplied by the current game era's Diplo Emphasis for Religion.
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE';
UPDATE Defines SET Value = '5' WHERE Name = 'OPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE';

-- AI asked player to stop digging up their artifacts
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_NO_DIG_PROMISE';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_IGNORED_NO_DIG_PROMISE';

-- AI asked player to stop spying on them
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_SPY_PROMISE';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_IGNORED_SPY_PROMISE';

-- Player and AI made a coop war agreement against another civ
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE'; -- player broke the agreement somehow (befriended the target, ended DoF early, etc.)


--	//////////////////////////////////////
--	// RELIGION/IDEOLOGY
--	//////////////////////////////////////

-- We have similar / divergent Social Policies. (note: AI sometimes chooses to ignore policy differences)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_SIMILAR_POLICIES', '-10'; -- min. opinion bonus if similar
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DIVERGENT_POLICIES', '10'; -- min. opinion penalty if divergent
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PER_SIMILAR_POLICY', '-5'; -- bonus per same policy branch unlocked
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PER_DIVERGENT_POLICY', '5'; -- penalty per divergent policy branch unlocked
INSERT INTO Defines (Name, Value) SELECT 'POLICY_SCORE_NEEDY_THRESHOLD', '8'; -- Neediness flavor value needed to add Needy Bonus
INSERT INTO Defines (Name, Value) SELECT 'POLICY_SCORE_NEEDY_BONUS', '5'; -- extra bonus/penalty if Needy

-- [RELIGION]
-- NOTE 1: Values below are multiplied by the current game era's Diplo Emphasis for Religion. Civs may get an increase or decrease to the era value based on their UA and flavors.
-- NOTE 2: You don't get a bonus/penalty for being the founder of a religion if you have 0 cities following that religion.
-- NOTE 3: AI sometimes chooses to ignore religious differences.

-- You have adopted their religion in the majority of your cities.
UPDATE Defines SET Value = '-8' WHERE Name = 'OPINION_WEIGHT_ADOPTING_MY_RELIGION';

-- They have happily adopted your religion in the majority of their cities.
UPDATE Defines SET Value = '-4' WHERE Name = 'OPINION_WEIGHT_ADOPTING_HIS_RELIGION';

-- We have adopted the same religion in the majority of our cities.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_SAME_STATE_RELIGIONS', '-2';

-- Religious differences strain your relationship.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DIFFERENT_OWNED_RELIGIONS', '5'; -- we founded or control the Holy City of different religions
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DIFFERENT_STATE_RELIGIONS', '2'; -- only one/neither of us founded, but we have different majority religions

-- Increases +/- opinion weights for same/different state religions if one of them is the World Religion
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WORLD_RELIGION_MODIFIER', '150';

-- They are spreading their own religion, but you converted some of their cities to your religion.
UPDATE Defines SET Value = '1' WHERE Name = 'OPINION_WEIGHT_PER_NEGATIVE_CONVERSION'; -- also multiplied by # of religious conversion points; see below
UPDATE Defines SET Value = '1' WHERE Name = 'RELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY'; -- # of points added for converting a city not following their owned religion
UPDATE Defines SET Value = '3' WHERE Name = 'RELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY'; -- # of points added for converting a city following their owned religion
UPDATE Defines SET Value = '25' WHERE Name = 'RELIGION_DIPLO_HIT_CONVERT_HOLY_CITY'; -- # of points added for converting the Holy City
UPDATE Defines SET Value = '4' WHERE Name = 'RELIGION_DIPLO_HIT_THRESHOLD'; -- point total is reduced to this (if higher) when player makes a promise to stop converting AI's cities.

-- [IDEOLOGY]
-- NOTE 1: Values below are multiplied by the current game era's Diplo Emphasis for Ideology (Late Policies). Civs may get an increase or decrease to the era value based on their UA, flavors, and how close they are to a cultural victory.
-- NOTE 2: AI sometimes chooses to ignore ideological differences.
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_SAME_LATE_POLICIES';
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_DIFFERENT_LATE_POLICIES';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_WORLD_IDEOLOGY_MODIFIER', '150'; -- increases +/- opinion weights for the World Ideology
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_IDEOLOGY_VASSAL_DIVISOR', '200'; -- decreases +/- opinion weights if one player is a vassal (doesn't affect voluntary vassals' opinion towards their masters)


--	//////////////////////////////////////
--	// PROTECTED CITY-STATES (MINOR CIVS)
--	//////////////////////////////////////

-- We have Pledged to Protect the same City-States.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PTP_SAME_MINOR_MIN', '-12'; -- min. opinion bonus
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PTP_SAME_MINOR_EACH', '-3'; -- opinion bonus per mutual PtP
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_PTP_SAME_MINOR_DIPLOMAT_MULTIPLIER', '200'; -- diplomat AIs double the amount of mutual PtPs for this calculation if you aren't a "City-State troublemaker" to them

-- You have conquered City-States under their protection!
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_KILLED_PROTECTED_MINOR';
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS'; -- killed more than one

-- You have attacked City-States under their protection!
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY';
UPDATE Defines SET Value = '15' WHERE Name = 'OPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS'; -- attacked more than one

-- You have bullied City-States under their protection!
UPDATE Defines SET Value = '15' WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY';
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS'; -- bullied more than one

-- They mistreated your protected City-States, and you didn't look the other way!
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_AGGRESSIVE_MOD', '10'; -- penalty for aggressive AIs


--	//////////////////////////////////////
--	// DECLARATION OF FRIENDSHIP
--	//////////////////////////////////////

-- We have made a public Declaration of Friendship! / We were previously friends.
-- NOTE: This bonus scales inversely (negatively) based on the AI's DoFWillingness flavor.
UPDATE Defines SET Value = '-30' WHERE Name = 'OPINION_WEIGHT_DOF';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_MOST_VALUED_FRIEND', '-20'; -- additional bonus if player is AI's most valued friend
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_TYPE_FRIENDS', '-10'; -- bonus for 1 previous DoF that wasn't ended early, without a denouncement/DoW in the interim (if not currently friends)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_TYPE_ALLIES', '-20'; -- bonus for 2 consecutive DoFs ""
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_TYPE_BATTLE_BROTHERS', '-30'; -- bonus for 3+ consecutive DoFs ""

-- We have made Declarations of Friendship with the same leaders!
-- NOTE: This bonus scales based on the AI's DoFWillingness flavor.
UPDATE Defines SET Value = '-20' WHERE Name = 'OPINION_WEIGHT_DOF_WITH_FRIEND';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_FRIEND_SUBSEQUENT', '-8'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_MOST_VALUED_FRIEND', '-20'; -- made a DoF with AI's most valued friend
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_MOST_VALUED_ALLY', '-10'; -- made a DoF with AI's most valued DP (if they have a DoF with them)

-- You have made a Declaration of Friendship with one of their enemies!
-- NOTE: This penalty scales based on the AI's Neediness flavor.
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_DOF_WITH_ENEMY';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_ENEMY_SUBSEQUENT', '8'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_BIGGEST_COMPETITOR', '20'; -- made a DoF with AI's biggest competitor, and AI has denounced them or they're at war
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DOF_WITH_BIGGEST_LEAGUE_RIVAL', '20'; -- made a DoF with AI's biggest World Congress rival, and AI has denounced them or they're at war


--	//////////////////////////////////////
--	// TRADE AGREEMENTS
--	//////////////////////////////////////

-- We have made a Defensive Pact!
-- NOTE: This bonus scales based on the AI's WarmongerHate flavor.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP', '-20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_MOST_VALUED_ALLY', '-20'; -- additional bonus if player is AI's most valued DP
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_CONQUEROR_MULTIPLIER', '150'; -- multiplier if AI has conquered another player (replaces WarmongerHate scaling)

-- We have made a Defensive Pact with the same leaders!
-- NOTE: This bonus scales based on the AI's WarmongerHate flavor.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_FRIEND', '-15';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_FRIEND_SUBSEQUENT', '-10'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_MOST_VALUED_ALLY', '-10'; -- made a DP with AI's most valued ally
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_MOST_VALUED_FRIEND', '-10'; -- made a DP with AI's most valued friend (if they have a DP with them)

-- You have made a Defensive Pact with one of their enemies!
-- NOTE: This penalty scales based on the AI's Neediness flavor.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_ENEMY', '20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_ENEMY_SUBSEQUENT', '10'; -- any after the first
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_BIGGEST_COMPETITOR', '20'; -- made a DP with AI's biggest competitor, and AI has denounced them or they're at war
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DP_WITH_BIGGEST_LEAGUE_RIVAL', '20'; -- made a DoF with AI's biggest World Congress rival, and AI has denounced them or they're at war

-- We have opened our borders to each other.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_OPEN_BORDERS_MUTUAL', '-12';

-- We have opened our borders to them.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_OPEN_BORDERS_US', '-8';

-- They have opened our borders to us. (no bonus for capitulated vassals unless Content, no bonus for voluntary vassals unless Content, Disagreeing or Resurrected)
INSERT INTO Defines (Name, Value )SELECT 'OPINION_WEIGHT_OPEN_BORDERS_THEM', '-4';

-- We have made a Research Agreement!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RA', '-10';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RA_SCIENTIFIC_MOD', '-10'; -- bonus for Scientific AIs


--	//////////////////////////////////////
--	// BACKSTABBING PENALTIES
--	//////////////////////////////////////

-- NOTE 1: The highest of these penalties is applied.
-- NOTE 2: None of these penalties are applied if the player isn't considered "untrustworthy" yet, except for "Your friends found reason to denounce you!"

INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_BETRAYED_OUR_FRIEND_MULTIPLIER', '200'; -- multiplier to all traitor opinion penalties wherein the player betrayed or was betrayed by AI's friends or allies

-- Your friends found reason to denounce you!
-- NOTE: This penalty scales based on the AI's DenounceWillingness flavor.
UPDATE Defines SET Value = '20' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH'; -- if they like the denouncer more than you
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE'; -- if they like you more than the denouncer

-- You have denounced leaders you've made Declarations of Friendship with!
UPDATE Defines SET Value = '40' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_FRIEND_EACH';

-- We made a Declaration of Friendship and then denounced them!
UPDATE Defines SET Value = '80' WHERE Name = 'OPINION_WEIGHT_DENOUNCED_ME_FRIENDS';

-- You declared war on your own vassal!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_ATTACKED_OWN_VASSAL', '50';

-- You declared war on them while they were your vassal!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT', '100';

-- You have declared war on leaders you've made Declarations of Friendship with!
UPDATE Defines SET Value = '75' WHERE Name = 'OPINION_WEIGHT_WAR_FRIEND_EACH';

-- We made a Declaration of Friendship and then declared war on them!
UPDATE Defines SET Value = '150' WHERE Name = 'OPINION_WEIGHT_WAR_ME_FRIENDS';

-- You have declared war on civilizations that you've resurrected!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_ATTACKED_RESURRECTED_PLAYER', '75';

-- You restored their civilization and then declared war on them!
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_RESURRECTOR_ATTACKED_US', '200';


--	//////////////////////////////////////
--	// WORLD CONGRESS
--	//////////////////////////////////////

-- Your past World Congress votes have generally supported/undermined their civ's interests.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VOTING_HISTORY_MAX', '60';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VOTING_HISTORY_DIPLOMAT_MULTIPLIER', '200'; -- increase for Diplomat AIs
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_MAX', '2400'; -- max. voting history score value (internally)
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_PLAYER_VOTE_PERCENT_VALUE', '100'; -- the higher this value, the more score is gained/lost for each % of the player's total votes devoted to the AI's favored outcome
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_OUTCOME_VOTE_PERCENT_VALUE', '200'; -- the higher this value, the more score is gained/lost for each % of the total votes for the AI's favored outcome were devoted by this player (compared to the contribution from other players)
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_WEAK', '100'; -- if the outcome is WEAKLY (dis)liked by the AI, the score gain is increased by this multiplier.
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STANDARD', '200'; -- if the outcome is NORMALLY (dis)liked by the AI, the score gain is increased by this multiplier.
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STRONG', '300'; -- if the outcome is STRONGLY (dis)liked by the AI, the score gain is increased by this multiplier.
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_OVERWHELMING', '400'; -- if the outcome is OVERWHELMINGLY (dis)liked by the AI, the score gain is increased by this multiplier.
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_LEAGUE_ALIGNMENT_SCALER', '10'; -- the higher this value, the more the voting history score affects the AI's "League Alignment" for this player, which affects voting decisions and is used to select the "top World Congress rival".
INSERT INTO Defines (Name, Value) SELECT 'VOTING_HISTORY_SCORE_PRIME_COMPETITOR_THRESHOLD', '12'; -- civs that have a voting history score equal to (VOTING_HISTORY_SCORE_MAX) / this value * -1 or worse in order are given greater consideration for "top World Congress rival".

-- They liked our proposal to the World Congress.
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_WEAK', '-15';
UPDATE Defines SET Value = '-30' WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG', '-45';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING', '-60';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', '134'; -- increase for Diplomat AIs

-- They disliked our proposal to the World Congress.
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_WEAK', '15';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG', '45';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING', '60';
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', '134'; -- increase for Diplomat AIs

-- We passed their proposal in the World Congress.
UPDATE Defines SET Value = '-10' WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL'; -- min. opinion bonus
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_MAX', '-60'; -- max. opinion bonus
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', '167'; -- increase for Diplomat AIs

-- We defeated their proposal in the World Congress.
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL'; -- min. opinion penalty
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_MAX', '60'; -- max. opinion penalty
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', '167'; -- increase for Diplomat AIs

-- We helped them become/remain the host of the World Congress.
UPDATE Defines SET Value = '-20' WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING'; -- min. opinion bonus
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_MAX', '-70'; -- max. opinion bonus
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_DIPLOMAT_MULTIPLIER', '150'; -- increase for Diplomat AIs

-- This % of the difference between the max and min values is added for each % that you contributed to the triggering vote
INSERT INTO Defines(Name, Value) SELECT 'OPINION_WEIGHT_PER_VOTE_PERCENT', '2';

-- We [tried to place/placed] sanctions on them in the World Congress.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_SANCTIONED_US', '50';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_SANCTIONED_US_DIPLOMAT_MULTIPLIER', '200'; -- increase for Diplomat AIs
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_SANCTIONED_US_FAILURE_DIVISOR', '200'; -- decrease if you don't succeed in placing the sanctions

-- We [tried to free/freed] them from sanctions in the World Congress.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_UNSANCTIONED_US', '-50';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_UNSANCTIONED_US_DIPLOMAT_MULTIPLIER', '200'; -- increase for Diplomat AIs
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_THEY_UNSANCTIONED_US_FAILURE_DIVISOR', '200'; -- decrease if you don't succeed in removing the sanctions


--	//////////////////////////////////////
--	// CIV IV DIPLOMATIC FEATURES
--	//////////////////////////////////////

-- Opinion Weight for AI being Master
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER', '-40';

-- Opinion Weights for AI being Vassal
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL', '-20';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL', '0';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_SAME_MASTER', '-60';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_SAME_MASTER_DIFFERENT_SURRENDER_DIVISOR', '200'; -- reduces the same master bonus between voluntary and capitulated vassals


-- Vassalage Treatment Thresholds
-- Determines vassal treatment "level", based on the Opinion Score from Vassal Treatment
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_TREATMENT_THRESHOLD_DISAGREE', '1';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_TREATMENT_THRESHOLD_MISTREATED', '25';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_TREATMENT_THRESHOLD_UNHAPPY', '50';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_TREATMENT_THRESHOLD_ENSLAVED', '80';

-- Vassal Treatment Score Factors
-- Making Demands
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_DEMANDED_WHILE_VASSAL', '100'; -- if > 100: reduction to penalty for demanding from vassals

-- Stealing From Them
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPITULATED_VASSAL_PLUNDERED_DIVISOR', '100'; -- if > 100: reduction to penalty for plundering TRs from capitulated vassals
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPITULATED_VASSAL_SPYING_DIVISOR', '100'; -- if > 100: reduction to penalty for spy actions against capitulated vassals
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_CAPITULATED_VASSAL_CULTURE_BOMB_DIVISOR', '100'; -- if > 100: reduction to penalty for stealing territory from capitulated vassals

-- Denouncing Them
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_DENOUNCED_BY_MASTER', '25'; -- Master denounced vassal!

-- Taxation Policies
-- Penalty = (((Gold Taxed By Masters Since Vassalage Started * 100 / Gross Gold Collected Since Vassalage Started)^OPINION_WEIGHT_VASSAL_TAX_EXPONENT) / OPINION_WEIGHT_VASSAL_TAX_DIVISOR) + (Current Tax Rate / OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_TAX_EXPONENT', '1.5';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_TAX_DIVISOR', '4';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER', '50';

-- You protected their territory as their master! (n.b. maximum vassal protect value = OPINION_WEIGHT_VASSALAGE_PROTECT_MAX * VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_PROTECT_MAX', '-50';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT', '50'; -- how much vassal protection value equals -1 opinion weight
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_PROTECTED_PER_TURN_DECAY', '25'; -- how fast vassal protection value decays
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_PROTECTED_CITY_DISTANCE', '6'; -- How close to a vassal city must an enemy unit be (in tiles) for it to count for protect value.

-- You failed to protect their territory as their master! (n.b. maximum vassal failed protect value = OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX * VASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT; scales with game speed)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX', '50';
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT', '50'; -- how much vassal failed protection value equals +1 opinion weight
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_FAILED_PROTECT_PER_TURN_DECAY', '25'; -- how fast vassal failed protection value decays
INSERT INTO Defines (Name, Value) SELECT 'VASSALAGE_FAILED_PROTECT_CITY_DISTANCE', '0'; -- How close to a vassal city must a vassal unit be for it to count for failed protect value (IF NOT IN OWNED TERRITORY). 0 = disabled.

-- Trade Routes With Them
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_TRADE_ROUTE', '-15'; -- only applies for trade routes the master sends to the vassal
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_TRADE_ROUTE_SUBSEQUENT', '-10'; -- any after the first

-- Opened Our Borders
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_OPEN_BORDERS', '-5'; -- for giving them Open Borders

-- Shared Religion Interests
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_FOUNDER_MASTER_ADOPTED_RELIGION', '-40'; -- if master has adopted the vassal's religion in a majority of their cities
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_HAPPILY_ADOPTED_RELIGION', '-20'; -- if vassal has no state religion and adopted master's religion
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSAL_SAME_STATE_RELIGION', '-10'; -- master and vassal have same majority religion (not master's owned religion)

-- Multiplier to negative factors in Vassal Treatment Opinion Score when AI is a Voluntary Vassal
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD', '120';


-- Master liberated Vassal without being asked
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE', '-50';

-- Master liberated Vassal when the Vassal requested
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED', '-25';

-- Master refused to liberate Vassal when the Vassal requested (declared war!)
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED', '50';


-- Opinion Weight Change from Having 2+ Vassals
-- Each player on a team counts as one vassal. Your own vassals and players you have a DoF with don't apply this penalty.
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS', '20';


--	//////////////////////////////////////
--	// PROMISE DURATIONS
--	//////////////////////////////////////

-- Defines how many turns a specific promise lasts for (for making the promise).
-- Defines how many turns until the AI forgets that you ignored/broke a specific promise (for other values). Once the AI forgets that you ignored/broke a promise, they become able to ask you to make it again.
-- Most promise lengths scale with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x

-- Military/Move Troops Request
-- This promise does NOT scale with game speed!
INSERT INTO Defines (Name, Value) SELECT 'MOVE_TROOPS_MEMORY_TURN_EXPIRATION', '20';
INSERT INTO Defines (Name, Value) SELECT 'MILITARY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';
INSERT INTO Defines (Name, Value) SELECT 'MILITARY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '80';

-- Stop Settling Near Us
UPDATE Defines SET Value = '50' WHERE Name = 'EXPANSION_PROMISE_TURNS_EFFECTIVE';
INSERT INTO Defines (Name, Value) SELECT 'EXPANSION_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'EXPANSION_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Buying Land Near Us
UPDATE Defines SET Value = '50' WHERE Name = 'BORDER_PROMISE_TURNS_EFFECTIVE'; 
INSERT INTO Defines (Name, Value) SELECT 'BORDER_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'BORDER_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Bullying Our Protected City-States
INSERT INTO Defines (Name, Value) SELECT 'BULLY_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'BULLY_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Don't Conquer Our Protected City-States
INSERT INTO Defines (Name, Value) SELECT 'ATTACK_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '40';
INSERT INTO Defines (Name, Value) SELECT 'ATTACK_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';

-- Stop Spying On Us
INSERT INTO Defines (Name, Value) SELECT 'SPY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'SPY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Converting Our Cities
INSERT INTO Defines (Name, Value) SELECT 'CONVERT_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'CONVERT_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Stop Digging Up Our Artifacts
INSERT INTO Defines (Name, Value) SELECT 'DIGGING_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'DIGGING_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '50';

-- Broken Coop War Promise
-- This promise does NOT scale with game speed!
INSERT INTO Defines (Name, Value) SELECT 'COOP_WAR_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', '60';


--	//////////////////////////////////////
--	// BACKSTABBING PENALTY DURATIONS
--	//////////////////////////////////////

-- Defines how many turns until the AI forgets that you backstabbed them.
-- The amount of turns scales with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x
-- NOTE: Broken military / CS conquest promises count as backstabbing penalties as well, but they are handled in the Promises section just above.

INSERT INTO Defines (Name, Value) SELECT 'DOF_BROKEN_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'DOF_BROKEN_TURNS_UNTIL_FORGIVEN_FRIENDS', '10'; -- if Opinion has risen to FRIEND or higher since ending the DoF
INSERT INTO Defines (Name, Value) SELECT 'FRIEND_DENOUNCED_US_TURNS_UNTIL_FORGIVEN', '75';
INSERT INTO Defines (Name, Value) SELECT 'FRIEND_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', '100';
INSERT INTO Defines (Name, Value) SELECT 'MASTER_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', '100';

-- Number of turns after prematurely ending a DoF during which you will obtain backstabbing penalties for denouncing or declaring war.
-- It will always take AT LEAST this many turns to clear the penalty for prematurely ending a DoF.
-- This value does not scale with game speed!
INSERT INTO Defines (Name, Value) SELECT 'DOF_BROKEN_BACKSTAB_TIMER', '10';


--	//////////////////////////////////////
--	// OTHER OPINION MODIFIER DURATIONS
--	//////////////////////////////////////

-- Defines the base number of turns until the AI forgets about an opinion modifier.
-- When there are multiple "stacks" of a modifier, this instead is the time before the # of stacks is halved (rounded down).
-- The amount of turns scales with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x
-- The amount of turns is also increased or decreased based on leader flavors (and this increase/decrease also scales with game speed). Modifiers less than 50 turns are less affected by flavors; more than 50 turns are moreso.
-- Certain modifiers are not included here.

INSERT INTO Defines (Name, Value) SELECT 'PLUNDERED_TRADE_ROUTE_TURNS_UNTIL_FORGIVEN', '25';
INSERT INTO Defines (Name, Value) SELECT 'RETURNED_CIVILIAN_TURNS_UNTIL_FORGOTTEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'BUILT_LANDMARK_TURNS_UNTIL_FORGOTTEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'LIBERATED_CITY_TURNS_UNTIL_FORGOTTEN', '75';
INSERT INTO Defines (Name, Value) SELECT 'FORGAVE_FOR_SPYING_TURNS_UNTIL_FORGOTTEN', '30';
INSERT INTO Defines (Name, Value) SELECT 'SHARED_INTRIGUE_TURNS_UNTIL_FORGOTTEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'ROBBED_US_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'PLOTTED_AGAINST_US_TURNS_UNTIL_FORGIVEN', '14'; -- 0-26 is added to this value based on perceived threat level of the plotter; does not scale with game speed
INSERT INTO Defines (Name, Value) SELECT 'BEATEN_TO_WONDER_TURNS_UNTIL_FORGIVEN', '75'; -- no direct penalty from this, but it affects WonderDisputeLevel
INSERT INTO Defines (Name, Value) SELECT 'LOWERED_OUR_INFLUENCE_TURNS_UNTIL_FORGIVEN', '50'; -- no direct penalty from this, but it affects MinorCivDisputeLevel
INSERT INTO Defines (Name, Value) SELECT 'PERFORMED_COUP_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'EXCAVATED_ARTIFACT_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'MADE_DEMAND_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'RELIGIOUS_CONVERSION_TURNS_UNTIL_FORGIVEN', '25';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';
UPDATE Defines SET Value = '30' WHERE Name = 'OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN';
UPDATE Defines SET Value = '10' WHERE Name = 'OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS';
UPDATE Defines SET Value = '50' WHERE Name = 'OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS';
INSERT INTO Defines (Name, Value) SELECT 'SANCTIONED_US_TURNS_UNTIL_FORGIVEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'UNSANCTIONED_US_TURNS_UNTIL_FORGOTTEN', '50';
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN', '100'; -- How many turns the "gave independence when asked" bonus remains for
INSERT INTO Defines (Name, Value) SELECT 'OPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN', '100'; -- How many turns the "refused to give independence" when asked penalty remains for