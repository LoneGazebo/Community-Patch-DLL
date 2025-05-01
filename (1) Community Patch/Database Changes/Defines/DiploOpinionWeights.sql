-- Diplo Opinion Weights
-- All opinion weights are listed here, even the unmodified ones, for easier modding.

REPLACE INTO Defines
	(Name, Value)
VALUES
-- Opinion Thresholds
-- Defines how much positive or negative Opinion is required for a specific opinion rating.
	('OPINION_THRESHOLD_UNFORGIVABLE', 160),
	('OPINION_THRESHOLD_ENEMY', 80),
	('OPINION_THRESHOLD_COMPETITOR', 30),

	('OPINION_THRESHOLD_FAVORABLE', -30),
	('OPINION_THRESHOLD_FRIEND', -80),
	('OPINION_THRESHOLD_ALLY', -160),

-- Major Modifier Thresholds
-- Opinion modifiers with a value higher than these thresholds will display in bright red/green instead of dark red/green
	('OPINION_THRESHOLD_MAJOR_POSITIVE', -15), -- for positive modifiers
	('OPINION_THRESHOLD_MAJOR_NEGATIVE', 15), -- for negative modifiers

-- Opinion Modifiers

--	//////////////////////////////////////
--	// DISPUTE MODIFIERS
--	//////////////////////////////////////

-- Territorial disputes strain your relationship. / You have no contested borders.
-- NOTE: This modifier scales based on the AI's Boldness flavor and the player's difficulty level.
	('OPINION_WEIGHT_LAND_FIERCE', 40),
	('OPINION_WEIGHT_LAND_STRONG', 25),
	('OPINION_WEIGHT_LAND_WEAK', 15),
	('OPINION_WEIGHT_LAND_NONE', -10),
	('OPINION_WEIGHT_LAND_WARMONGER', 10), -- penalty for Conqueror AIs
	('OPINION_WEIGHT_LAND_NONE_WARMONGER', -5), -- bonus for Conqueror AIs (increases based on Neediness flavor)

-- You are [not] competing for World Wonders.
-- NOTE: This modifier scales based on the AI's WonderCompetitiveness flavor and the player's difficulty level.
	('OPINION_WEIGHT_WONDER_FIERCE', 50),
	('OPINION_WEIGHT_WONDER_STRONG', 35),
	('OPINION_WEIGHT_WONDER_WEAK', 20),
	('OPINION_WEIGHT_WONDER_NONE', 0),
	('OPINION_WEIGHT_WONDER_CULTURAL', 10), -- penalty for Cultural AIs
	('OPINION_WEIGHT_WONDER_NONE_CULTURAL', -5), -- bonus for Cultural AIs (increases based on Neediness flavor)

-- You are [not] competing for the favor of the same City-States!
-- NOTE: This modifier scales based on the AI's MinorCivCompetitiveness flavor and the player's difficulty level.
	('OPINION_WEIGHT_MINOR_CIV_FIERCE', 30),
	('OPINION_WEIGHT_MINOR_CIV_STRONG', 20),
	('OPINION_WEIGHT_MINOR_CIV_WEAK', 10),
	('OPINION_WEIGHT_MINOR_CIV_NONE', 0),
	('OPINION_WEIGHT_MINOR_CIV_DIPLOMAT', 10), -- penalty for Diplomat AIs
	('OPINION_WEIGHT_MINOR_CIV_NONE_DIPLOMAT', -5), -- bonus for Diplomat AIs (increases based on Neediness flavor)

-- You are [not] competing for technological advancement. (only for Scientific AIs)
-- NOTE: This modifier scales based on the AI's DiploBalance flavor and the player's difficulty level.
	('OPINION_WEIGHT_TECH_FIERCE', 30),
	('OPINION_WEIGHT_TECH_STRONG', 20),
	('OPINION_WEIGHT_TECH_WEAK', 10),
	('OPINION_WEIGHT_TECH_NONE', -5), -- increases based on Neediness flavor

-- You are [not] competing for cultural advancement. (only for Cultural AIs)
-- NOTE: This modifier scales based on the AI's DiploBalance flavor and the player's difficulty level.
	('OPINION_WEIGHT_POLICY_FIERCE', 30),
	('OPINION_WEIGHT_POLICY_STRONG', 20),
	('OPINION_WEIGHT_POLICY_WEAK', 10),
	('OPINION_WEIGHT_POLICY_NONE', -5), -- increases based on Neediness flavor

-- They fear/suspect you are competing with them. / They know you are competing with them, and they hate it!
-- This modifier is applied if the AI is upset about your competition with them over the same victory condition as the one they're pursuing. It doesn't apply if Victory Block is higher.
-- If AI is not competing for victory, the modifier is not applied.
-- NOTE: This modifier scales based on the AI's VictoryCompetitiveness flavor and the player's difficulty level.
	('OPINION_WEIGHT_VICTORY_FIERCE', 40),
	('OPINION_WEIGHT_VICTORY_STRONG', 30),
	('OPINION_WEIGHT_VICTORY_WEAK', 20),
	('OPINION_WEIGHT_VICTORY_NONE', 0), -- this value is applied if both Victory Dispute AND Victory Block are at none AND the AI doesn't consider you a "major competitor"
	('OPINION_WEIGHT_VICTORY_PER_ERA', 4), -- penalty per era if > 0
	('OPINION_WEIGHT_VICTORY_NONE_PER_ERA', 0), -- bonus per era if < 0

-- Your success worries/angers/infuriates them.
-- This modifier is applied if the AI is upset about your competition with them over a different victory condition than the one they're pursuing. It doesn't apply if Victory Dispute is equal or higher.
-- If AI is not competing for victory, the modifier is not applied.
-- NOTE: This modifier scales based on the AI's VictoryCompetitiveness flavor and the player's difficulty level.
	('OPINION_WEIGHT_VICTORY_BLOCK_FIERCE', 40),
	('OPINION_WEIGHT_VICTORY_BLOCK_STRONG', 30),
	('OPINION_WEIGHT_VICTORY_BLOCK_WEAK', 20),
	('OPINION_WEIGHT_VICTORY_BLOCK_PER_ERA', 4), -- penalty per era if > 0

-- Reckless Expander: Too many cities/land without enough military to back it up
-- AI only applies this if nearby, they're competing for victory, and you have more land/cities than they do
-- NOTE: This penalty scales based on the AI's Boldness flavor and the player's difficulty level.

	('RECKLESS_EXPANDER_CITIES_THRESHOLD', 200), -- must have at least this % city count compared to the median
	('RECKLESS_EXPANDER_LAND_THRESHOLD', 250), -- OR must have at least this % tile count compared to the median

	('OPINION_WEIGHT_RECKLESS_EXPANDER', 20), -- base opinion penalty; the highest of the two penalties below is added to it
	('OPINION_WEIGHT_RECKLESS_EXPANDER_PER_CITY', 10), -- penalty for each city above the median city count (unless your cities - their cities = a smaller value, in which case that one is used)
	('OPINION_WEIGHT_RECKLESS_EXPANDER_PER_TILE', 1), -- penalty for each tile above the median tile count (unless your tiles - their tiles = a smaller value, in which case that one is used)

	('OPINION_WEIGHT_RECKLESS_EXPANDER_STRATEGIC_MOD', 20), -- extra penalty for Conqueror AIs

-- Wonder Spammer: Too many World Wonders compared to other civs
-- AI only applies this if nearby or Cultural, they're competing for victory, and you have more Wonders than they do
-- Captured Wonders, Corporations and World Congress Wonders do not count for this penalty!
-- NOTE: This penalty scales based on the AI's WonderCompetitiveness flavor and the player's difficulty level. At lower difficulties the AI is also slightly more tolerant of Wonder spamming.

	('WONDER_SPAMMER_THRESHOLD', 3), -- must have constructed this many more Wonders than the median (only counting Wonder-building civs); must also have at least 50% more than the global average

	('OPINION_WEIGHT_WONDER_SPAMMER', 20), -- base opinion penalty
	('OPINION_WEIGHT_WONDER_SPAMMER_PER_WONDER', 5), -- penalty for each wonder above WONDER_SPAMMER_THRESHOLD (unless your Wonders - their Wonders = a smaller value, in which case that one is used)
	('OPINION_WEIGHT_WONDER_SPAMMER_CAP', 60), -- max. opinion penalty from Wonders constructed count

	('OPINION_WEIGHT_WONDER_SPAMMER_STRATEGIC_MOD', 20), -- extra penalty for Conqueror AIs

--	//////////////////////////////////////
--	// WAR STUFF
--	//////////////////////////////////////

-- You plundered their trade routes!
	('OPINION_WEIGHT_PLUNDERED_TRADE_ROUTE', 5), -- x2 for origin civ

-- You killed or captured their civilians! / You killed or captured civilians during war!
	('OPINION_WEIGHT_CIVILIAN_KILLER_MAX', 50), -- maximum opinion penalty
	('CIVILIAN_KILLER_VALUE_PER_OPINION_WEIGHT', 100), -- increase this to reach the maximum more quickly
	('CIVILIAN_KILLER_VALUE_PER_TURN_DECAY', 100), -- how fast this penalty decays per turn
	('OPINION_WEIGHT_CIVILIAN_KILLER_WORLD_THRESHOLD', 25), -- threshold for global penalty; penalty score must be at least this high with another civ that AI doesn't hate
	('OPINION_WEIGHT_CIVILIAN_KILLER_WORLD', 20), -- opinion penalty if OPINION_WEIGHT_CIVILIAN_KILLER_WORLD_THRESHOLD conditions are met (see line above)

-- You nuked them!
	('OPINION_WEIGHT_NUKED_MAX', 100),

-- You captured their original capital / Holy City!
	('OPINION_WEIGHT_CAPTURED_CAPITAL', 160),
	('OPINION_WEIGHT_CAPTURED_HOLY_CITY', 80),
	('OPINION_WEIGHT_CAPTURED_KEY_CITY_RETURNED_DIVISOR', 200), -- penalty reduction for returning the captured city to the AI
	('OPINION_WEIGHT_CAPTURED_KEY_CITY_CAPITULATION_DIVISOR', 200), -- penalty reduction for capitulated vassals; only applies if not treating them badly

-- Your military deployment is (extremely) threatening.
	('OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_LOW', 10),
	('OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_MEDIUM', 20),
	('OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_HIGH', 40),
	('OPINION_WEIGHT_MILITARY_AGGRESSIVE_POSTURE_INCREDIBLE', 60),

--	//////////////////////////////////////
--	// Player has done nice stuff
--	//////////////////////////////////////

-- We are trade partners.  (n.b. maximum recent trade value = OPINION_WEIGHT_TRADE_MAX * DEAL_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_TRADE_MAX', -40), -- maximum opinion bonus from recent trade value
	('DEAL_VALUE_PER_OPINION_WEIGHT', 5), -- how much recent trade value equals -1 opinion weight
	('DEAL_VALUE_PER_TURN_DECAY', 2), -- how fast recent trade value decays
	('OPINION_WEIGHT_STRATEGIC_TRADE_PARTNER_MULTIPLIER', 150), -- increases current and maximum trade bonus if player is considered a strategic trade partner

-- Your recent diplomatic actions please them. (n.b. maximum recent assist value = OPINION_WEIGHT_ASSIST_MAX * ASSIST_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_ASSIST_MAX', -30), -- maximum opinion bonus from recent assist value
	('ASSIST_VALUE_PER_OPINION_WEIGHT', 5), -- how much recent assist value equals +/- 1 opinion weight
	('ASSIST_VALUE_PER_TURN_DECAY', 3), -- how fast recent assist value decays

-- Your recent diplomatic actions disappoint them. (n.b. minimum recent assist value = OPINION_WEIGHT_FAILED_ASSIST_MAX * ASSIST_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_FAILED_ASSIST_MAX', 30), -- maximum opinion penalty from recent assist value

-- We fought together against a common foe. (n.b. maximum common foe value = OPINION_WEIGHT_COMMON_FOE_MAX * COMMON_FOE_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_COMMON_FOE_MAX', -100),
	('COMMON_FOE_VALUE_PER_OPINION_WEIGHT', 50), -- how much common foe value equals -1 opinion weight
	('COMMON_FOE_VALUE_PER_TURN_DECAY', 25), -- how fast common foe value decays

-- You freed their captured citizens!
	('OPINION_WEIGHT_RETURNED_CIVILIAN', -10),
	('OPINION_WEIGHT_RETURNED_CIVILIAN_SUBSEQUENT', -5), -- any after the first; must be a lesser bonus or equal to the initial value

-- You built a Landmark in their territory.
	('OPINION_WEIGHT_BUILT_LANDMARK', -20),
	('OPINION_WEIGHT_BUILT_LANDMARK_SUBSEQUENT', -10), -- any after the first; must be a lesser bonus or equal to the initial value

-- You restored their civilization after they were annihilated!
	('OPINION_WEIGHT_RESURRECTED', -200),
	('OPINION_WEIGHT_LIBERATOR_CAPTURED_CAPITAL_DIVISOR', 200), -- penalty to resurrection bonus if player captured AI's original capital
	('OPINION_WEIGHT_LIBERATOR_CAPTURED_HOLY_CITY_DIVISOR', 200), -- penalty to several liberation bonuses if player captured AI's Holy City

-- You liberated their original capital / Holy City.
	('OPINION_WEIGHT_LIBERATED_CAPITAL', -120),
	('OPINION_WEIGHT_LIBERATED_CAPITAL_VASSAL_MULTIPLIER', 150),
	('OPINION_WEIGHT_LIBERATED_HOLY_CITY', -80),
	('OPINION_WEIGHT_LIBERATED_HOLY_CITY_VASSAL_MULTIPLIER', 150),

-- You have liberated some of their people!
	('OPINION_WEIGHT_LIBERATED_CITY', -30),
	('OPINION_WEIGHT_LIBERATED_CITY_VASSAL_MULTIPLIER', 200),

-- You gave them back their original capital / Holy City.
	('OPINION_WEIGHT_RETURNED_CAPITAL', -60),
	('OPINION_WEIGHT_RETURNED_CAPITAL_VASSAL_MULTIPLIER', 150),
	('OPINION_WEIGHT_RETURNED_HOLY_CITY', -40),
	('OPINION_WEIGHT_RETURNED_HOLY_CITY_VASSAL_MULTIPLIER', 150),

-- We have shared embassies. / They have an embassy in our capital. / We have an embassy in their capital.
	('OPINION_WEIGHT_EMBASSY_MUTUAL', -3),
	('OPINION_WEIGHT_EMBASSY', -2),
	('OPINION_WEIGHT_EMBASSY_THEM', -1),

-- We have a Diplomat in their capital. (no bonus for capitulated vassals unless Content, no bonus for voluntary vassals unless Content, Disagreeing or Resurrected, no bonus if there are recent spying penalties)
	('OPINION_WEIGHT_DIPLOMAT', -20),
	('OPINION_WEIGHT_DIPLOMAT_MOD', -10), -- bonus for Scientist AIs

-- You forgave them for spying.
	('OPINION_WEIGHT_FORGAVE_FOR_SPYING', -10),

-- You have shared intrigue with them.
	('OPINION_WEIGHT_INTRIGUE_SHARED_BY', -10),
	('OPINION_WEIGHT_INTRIGUE_SHARED_BY_SUBSEQUENT', -5), -- any after the first; must be a lesser bonus or equal to the initial value

--	//////////////////////////////////////
--	// Player has done mean things
--	//////////////////////////////////////

-- You stole their territory!
	('OPINION_WEIGHT_CULTURE_BOMBED', 10), -- multiplied by 1 to 6 for each theft depending on the value of the stolen tile(s)

-- Your spies were caught stealing from them.
	('OPINION_WEIGHT_ROBBED_BY', 20),

-- You were caught plotting against them.
	('OPINION_WEIGHT_PLOTTED_AGAINST_US', 20),
	('OPINION_WEIGHT_PLOTTED_AGAINST_US_SUBSEQUENT', 10), -- any after the first; must be a lesser penalty or equal to the initial value

-- Your spies usurped their City-State Alliance in a coup d'état.
	('OPINION_WEIGHT_PERFORMED_COUP', 30),

-- You excavated their cultural artifacts!
	('OPINION_WEIGHT_EXCAVATED_ARTIFACT', 30),

--	//////////////////////////////////////
--	// Player has asked us to do things we don't like
--	//////////////////////////////////////

-- You demanded they not settle near your lands! (if they refused the request)
	('OPINION_WEIGHT_ASKED_NO_SETTLE', 20),

-- You asked them not to spy on you. (if they refused the request)
	('OPINION_WEIGHT_ASKED_STOP_SPYING', 10),

-- You made a trade demand of them!
	('OPINION_WEIGHT_MADE_DEMAND_OF_US', 20),
	('OPINION_WEIGHT_MADE_DEMAND_OF_US_SUBSEQUENT', 10), -- any after the first; must be a lesser penalty or equal to the initial value
	('OPINION_WEIGHT_MADE_DEMAND_YOU_NO_TAKE_DIVISOR', 200), -- decreases opinion weight if AI is not currently giving anything
	('OPINION_WEIGHT_MADE_DEMAND_BANKRUPT_MULTIPLIER', 300), -- increases opinion weight if AI is currently giving something AND is bankrupt (or will be soon)
	('OPINION_WEIGHT_MADE_DEMAND_BANKRUPT_MULTIPLIER_TURNS', 20), -- max turns for AI "soon" bankruptcy multiplier (penalty gradually increases as turns to bankruptcy decrease)

--	//////////////////////////////////////
--	// DENOUNCING
--	//////////////////////////////////////

-- We have denounced them/each other! / They have denounced us!
	('OPINION_WEIGHT_DENOUNCED_ME', 50), -- player denounced AI
	('OPINION_WEIGHT_DENOUNCED_THEM', 0), -- AI denounced player
	('OPINION_WEIGHT_MUTUAL_DENOUNCEMENT', 50), -- both of these

-- You have Denounced a leader they made a Declaration of Friendship with!
-- NOTE: This penalty scales based on the AI's Loyalty flavor.
	('OPINION_WEIGHT_DENOUNCED_FRIEND', 20),
	('OPINION_WEIGHT_DENOUNCED_FRIEND_SUBSEQUENT', 8), -- any after the first
	('OPINION_WEIGHT_DENOUNCED_MOST_VALUED_FRIEND', 20), -- denounced the AI's most valued friend
	('OPINION_WEIGHT_DENOUNCED_MOST_VALUED_ALLY', 10), -- denounced the AI's most valued DP (if they have a DoF with them)

-- We have Denounced the same leaders!
-- NOTE: This bonus scales based on the AI's WorkAgainstWillingness flavor.
	('OPINION_WEIGHT_DENOUNCED_ENEMY', -20),
	('OPINION_WEIGHT_DENOUNCED_ENEMY_SUBSEQUENT', -8), -- any after the first
	('OPINION_WEIGHT_DENOUNCED_BIGGEST_COMPETITOR', -20), -- mutual denouncement against AI's biggest competitor
	('OPINION_WEIGHT_DENOUNCED_BIGGEST_LEAGUE_RIVAL', -20), -- mutual denouncement against AI's biggest World Congress rival

-- Other civs that they like more than you have denounced you!
-- NOTE: This penalty scales based on the AI's WorkAgainstWillingness flavor.
	('OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND', 15),
	('OPINION_WEIGHT_DENOUNCED_BY_THEIR_FRIEND_SUBSEQUENT', 5), -- any after the first
	('OPINION_WEIGHT_DENOUNCED_BY_THEIR_KEY_FRIEND', 15), -- additional penalty if the denouncement comes from a teammate or their most valued friend/ally

--	//////////////////////////////////////
--	// PROMISES
--	//////////////////////////////////////

-- BROKEN = player made a promise and then broke it
-- IGNORED = player ignored the AI's request to make a promise

-- AI asked player not to declare war on them
	('OPINION_WEIGHT_BROKEN_MILITARY_PROMISE', 80),
	('OPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD', 40), -- global penalty; applied if no personal penalty for breaking the promise
	('OPINION_WEIGHT_IGNORED_MILITARY_PROMISE', 0),

-- AI asked player to stop settling near them
	('OPINION_WEIGHT_BROKEN_EXPANSION_PROMISE', 40),
	('OPINION_WEIGHT_IGNORED_EXPANSION_PROMISE', 30),

-- AI asked player to stop buying land near them
	('OPINION_WEIGHT_BROKEN_BORDER_PROMISE', 40),
	('OPINION_WEIGHT_IGNORED_BORDER_PROMISE', 30),

-- AI asked player not to conquer their protected City-States
	('OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE', 80),
	('OPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD', 40), -- global penalty; applied if no personal penalty for breaking the promise
	('OPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE', 30),

-- AI asked player not to bully their protected City-States
	('OPINION_WEIGHT_BROKEN_BULLY_CITY_STATE_PROMISE', 40),
	('OPINION_WEIGHT_IGNORED_BULLY_CITY_STATE_PROMISE', 20),

-- AI asked player to stop converting their cities to another religion
-- These values are multiplied by the current game era's Diplo Emphasis for Religion.
	('OPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE', 10),
	('OPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE', 5),

-- AI asked player to stop digging up their artifacts
	('OPINION_WEIGHT_BROKEN_NO_DIG_PROMISE', 40),
	('OPINION_WEIGHT_IGNORED_NO_DIG_PROMISE', 30),

-- AI asked player to stop spying on them
	('OPINION_WEIGHT_BROKEN_SPY_PROMISE', 40),
	('OPINION_WEIGHT_IGNORED_SPY_PROMISE', 30),

-- Player and AI made a coop war agreement against another civ
	('OPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE', 40), -- player broke the agreement somehow (befriended the target, ended DoF early, etc.)

--	//////////////////////////////////////
--	// RELIGION/IDEOLOGY
--	//////////////////////////////////////

-- We have similar / divergent Social Policies. (note: AI sometimes chooses to ignore policy differences)
	('OPINION_WEIGHT_SIMILAR_POLICIES', -10), -- min. opinion bonus if similar
	('OPINION_WEIGHT_DIVERGENT_POLICIES', 10), -- min. opinion penalty if divergent
	('OPINION_WEIGHT_PER_SIMILAR_POLICY', -5), -- bonus per same policy branch unlocked
	('OPINION_WEIGHT_PER_DIVERGENT_POLICY', 5), -- penalty per divergent policy branch unlocked
	('POLICY_SCORE_NEEDY_THRESHOLD', 8), -- Neediness flavor value needed to add Needy Bonus
	('POLICY_SCORE_NEEDY_BONUS', 5), -- extra bonus/penalty if Needy

-- [RELIGION]
-- NOTE 1: Values below are multiplied by the current game era's Diplo Emphasis for Religion. AI may increase or decrease their own multiplier (globally) based on their UA and flavors.
-- NOTE 2: You don't get a bonus/penalty for being the founder of a religion if you have 0 cities following that religion.
-- NOTE 3: AI sometimes chooses to ignore religious differences.

-- You have adopted their religion in the majority of your cities.
	('OPINION_WEIGHT_ADOPTING_MY_RELIGION', -8),

-- They have happily adopted your religion in the majority of their cities.
	('OPINION_WEIGHT_ADOPTING_HIS_RELIGION', -4),

-- We have adopted the same religion in the majority of our cities.
	('OPINION_WEIGHT_SAME_STATE_RELIGIONS', -2),

-- Religious differences strain your relationship.
	('OPINION_WEIGHT_DIFFERENT_OWNED_RELIGIONS', 5), -- we founded or control the Holy City of different religions
	('OPINION_WEIGHT_DIFFERENT_STATE_RELIGIONS', 2), -- only one/neither of us founded, but we have different majority religions

-- Increases +/- opinion weights for same/different state religions if one of them is the World Religion
	('OPINION_WEIGHT_WORLD_RELIGION_MODIFIER', 150),

-- They are spreading their own religion, but you converted some of their cities to your religion.
	('OPINION_WEIGHT_PER_NEGATIVE_CONVERSION', 1), -- also multiplied by # of religious conversion points; see below
	('RELIGION_DIPLO_HIT_INITIAL_CONVERT_FRIENDLY_CITY', 1), -- # of points added for converting a city not following their owned religion
	('RELIGION_DIPLO_HIT_RELIGIOUS_FLIP_FRIENDLY_CITY', 3), -- # of points added for converting a city following their owned religion
	('RELIGION_DIPLO_HIT_CONVERT_HOLY_CITY', 25), -- # of points added for converting the Holy City
	('RELIGION_DIPLO_HIT_THRESHOLD', 4), -- point total is reduced to this (if higher) when player makes a promise to stop converting AI's cities.

-- [IDEOLOGY]
-- NOTE 1: Values below are multiplied by the current game era's Diplo Emphasis for Ideology (Late Policies). AI may increase or decrease their own multiplier (globally) based on their UA, flavors, and how close they are to a cultural victory.
-- NOTE 2: AI sometimes chooses to ignore ideological differences.
	('OPINION_WEIGHT_SAME_LATE_POLICIES', -10),
	('OPINION_WEIGHT_DIFFERENT_LATE_POLICIES', 10),
	('OPINION_WEIGHT_WORLD_IDEOLOGY_MODIFIER', 150), -- increases +/- opinion weights for the World Ideology
	('OPINION_WEIGHT_IDEOLOGY_VASSAL_DIVISOR', 200), -- decreases +/- opinion weights if one player is a vassal (doesn't affect voluntary vassals' opinion towards their masters)

--	//////////////////////////////////////
--	// PROTECTED CITY-STATES (MINOR CIVS)
--	//////////////////////////////////////

-- We have Pledged to Protect the same City-States.
	('OPINION_WEIGHT_PTP_SAME_MINOR_MIN', -12), -- min. opinion bonus
	('OPINION_WEIGHT_PTP_SAME_MINOR_EACH', -3), -- opinion bonus per mutual PtP
	('OPINION_WEIGHT_PTP_SAME_MINOR_DIPLOMAT_MULTIPLIER', 200), -- diplomat AIs double the amount of mutual PtPs for this calculation if you aren't a "City-State troublemaker" to them

-- You have conquered City-States under their protection!
	('OPINION_WEIGHT_KILLED_PROTECTED_MINOR', 40),
	('OPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS', 20), -- killed more than one

-- You have attacked City-States under their protection!
	('OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY', 20),
	('OPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS', 15), -- attacked more than one

-- You have bullied City-States under their protection!
	('OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY', 15),
	('OPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS', 10), -- bullied more than one

-- They mistreated your protected City-States, and you didn't look the other way!
	('OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR', 10),
	('OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_AGGRESSIVE_MOD', 10), -- penalty for aggressive AIs

--	//////////////////////////////////////
--	// DECLARATION OF FRIENDSHIP
--	//////////////////////////////////////

-- We have made a public Declaration of Friendship! / We were previously friends.
-- NOTE: This bonus scales inversely (negatively) based on the AI's DoFWillingness flavor.
	('OPINION_WEIGHT_DOF', -30),
	('OPINION_WEIGHT_DOF_MOST_VALUED_FRIEND', -20), -- additional bonus if player is AI's most valued friend
	('OPINION_WEIGHT_DOF_TYPE_FRIENDS', -10), -- bonus for 1 previous DoF that wasn't ended early, without a denouncement/DoW in the interim (if not currently friends)
	('OPINION_WEIGHT_DOF_TYPE_ALLIES', -20), -- bonus for 2 consecutive DoFs ""
	('OPINION_WEIGHT_DOF_TYPE_BATTLE_BROTHERS', -30), -- bonus for 3+ consecutive DoFs ""

-- We have made Declarations of Friendship with the same leaders!
-- NOTE: This bonus scales based on the AI's WorkWithWillingness flavor.
	('OPINION_WEIGHT_DOF_WITH_FRIEND', -20),
	('OPINION_WEIGHT_DOF_WITH_FRIEND_SUBSEQUENT', -8), -- any after the first
	('OPINION_WEIGHT_DOF_WITH_MOST_VALUED_FRIEND', -20), -- made a DoF with AI's most valued friend
	('OPINION_WEIGHT_DOF_WITH_MOST_VALUED_ALLY', -10), -- made a DoF with AI's most valued DP (if they have a DoF with them)

-- You have made a Declaration of Friendship with one of their enemies!
-- NOTE: This penalty scales based on the AI's Neediness flavor.
	('OPINION_WEIGHT_DOF_WITH_ENEMY', 20),
	('OPINION_WEIGHT_DOF_WITH_ENEMY_SUBSEQUENT', 8), -- any after the first
	('OPINION_WEIGHT_DOF_WITH_BIGGEST_COMPETITOR', 20), -- made a DoF with AI's biggest competitor, and AI has denounced them or they're at war
	('OPINION_WEIGHT_DOF_WITH_BIGGEST_LEAGUE_RIVAL', 20), -- made a DoF with AI's biggest World Congress rival, and AI has denounced them or they're at war

--	//////////////////////////////////////
--	// TRADE AGREEMENTS
--	//////////////////////////////////////

-- We have made a Defensive Pact!
-- NOTE: This bonus scales based on the AI's WarmongerHate flavor.
	('OPINION_WEIGHT_DP', -20),
	('OPINION_WEIGHT_DP_MOST_VALUED_ALLY', -20), -- additional bonus if player is AI's most valued DP
	('OPINION_WEIGHT_DP_CONQUEROR_MULTIPLIER', 150), -- multiplier if AI has conquered another player (replaces WarmongerHate scaling)

-- We have made a Defensive Pact with the same leaders!
-- NOTE: This bonus scales based on the AI's WarmongerHate flavor.
	('OPINION_WEIGHT_DP_WITH_FRIEND', -15),
	('OPINION_WEIGHT_DP_WITH_FRIEND_SUBSEQUENT', -10), -- any after the first
	('OPINION_WEIGHT_DP_WITH_MOST_VALUED_ALLY', -10), -- made a DP with AI's most valued ally
	('OPINION_WEIGHT_DP_WITH_MOST_VALUED_FRIEND', -10), -- made a DP with AI's most valued friend (if they have a DP with them)

-- You have made a Defensive Pact with one of their enemies!
-- NOTE: This penalty scales based on the AI's Neediness flavor.
	('OPINION_WEIGHT_DP_WITH_ENEMY', 20),
	('OPINION_WEIGHT_DP_WITH_ENEMY_SUBSEQUENT', 10), -- any after the first
	('OPINION_WEIGHT_DP_WITH_BIGGEST_COMPETITOR', 20), -- made a DP with AI's biggest competitor, and AI has denounced them or they're at war
	('OPINION_WEIGHT_DP_WITH_BIGGEST_LEAGUE_RIVAL', 20), -- made a DoF with AI's biggest World Congress rival, and AI has denounced them or they're at war

-- We have opened our borders to each other.
	('OPINION_WEIGHT_OPEN_BORDERS_MUTUAL', -12),

-- We have opened our borders to them.
	('OPINION_WEIGHT_OPEN_BORDERS_US', -8),

-- They have opened our borders to us. (no bonus for capitulated vassals unless Content, no bonus for voluntary vassals unless Content, Disagreeing or Resurrected)
	('OPINION_WEIGHT_OPEN_BORDERS_THEM', -4),

-- We have made a Research Agreement!
	('OPINION_WEIGHT_RA', -10),
	('OPINION_WEIGHT_RA_SCIENTIFIC_MOD', -10), -- bonus for Scientific AIs

--	//////////////////////////////////////
--	// BACKSTABBING PENALTIES
--	//////////////////////////////////////

-- NOTE 1: The highest of these penalties is applied.
-- NOTE 2: None of these penalties are applied if the player isn't considered "untrustworthy" yet, except for "Your friends found reason to denounce you!"

	('OPINION_WEIGHT_BETRAYED_OUR_FRIEND_MULTIPLIER', 200), -- multiplier to all traitor opinion penalties wherein the player betrayed or was betrayed by AI's friends or allies

-- Your friends found reason to denounce you!
-- NOTE: This penalty scales based on the AI's WorkAgainstWillingness flavor.
	('OPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH', 20), -- if they like the denouncer more than you
	('OPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE', 10), -- if they like you more than the denouncer

-- You have denounced leaders you've made Declarations of Friendship with!
	('OPINION_WEIGHT_DENOUNCED_FRIEND_EACH', 40),

-- We made a Declaration of Friendship and then denounced them!
	('OPINION_WEIGHT_DENOUNCED_ME_FRIENDS', 80),

-- You declared war on your own vassal!
	('OPINION_WEIGHT_ATTACKED_OWN_VASSAL', 50),

-- You declared war on them while they were your vassal!
	('OPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT', 100),

-- You have declared war on leaders you've made Declarations of Friendship with!
	('OPINION_WEIGHT_WAR_FRIEND_EACH', 75),

-- We made a Declaration of Friendship and then declared war on them!
	('OPINION_WEIGHT_WAR_ME_FRIENDS', 150),

-- You have declared war on civilizations that you've resurrected!
	('OPINION_WEIGHT_ATTACKED_RESURRECTED_PLAYER', 75),

-- You restored their civilization and then declared war on them!
	('OPINION_WEIGHT_RESURRECTOR_ATTACKED_US', 200),

--	//////////////////////////////////////
--	// WORLD CONGRESS
--	//////////////////////////////////////

-- Your past World Congress votes have generally supported/undermined their civ's interests.
	('OPINION_WEIGHT_VOTING_HISTORY_MAX', 60),
	('OPINION_WEIGHT_VOTING_HISTORY_DIPLOMAT_MULTIPLIER', 200), -- increase for Diplomat AIs
	('VOTING_HISTORY_SCORE_MAX', 2400), -- max. voting history score value (internally)
	('VOTING_HISTORY_SCORE_PLAYER_VOTE_PERCENT_VALUE', 100), -- the higher this value, the more score is gained/lost for each % of the player's total votes devoted to the AI's favored outcome
	('VOTING_HISTORY_SCORE_OUTCOME_VOTE_PERCENT_VALUE', 200), -- the higher this value, the more score is gained/lost for each % of the total votes for the AI's favored outcome were devoted by this player (compared to the contribution from other players)
	('VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_WEAK', 100), -- if the outcome is WEAKLY (dis)liked by the AI, the score gain is increased by this multiplier.
	('VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STANDARD', 200), -- if the outcome is NORMALLY (dis)liked by the AI, the score gain is increased by this multiplier.
	('VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_STRONG', 300), -- if the outcome is STRONGLY (dis)liked by the AI, the score gain is increased by this multiplier.
	('VOTING_HISTORY_SCORE_DESIRE_MULTIPLIER_OVERWHELMING', 400), -- if the outcome is OVERWHELMINGLY (dis)liked by the AI, the score gain is increased by this multiplier.
	('VOTING_HISTORY_SCORE_LEAGUE_ALIGNMENT_SCALER', 10), -- the higher this value, the more the voting history score affects the AI's "League Alignment" for this player, which affects voting decisions and is used to select the "top World Congress rival".
	('VOTING_HISTORY_SCORE_PRIME_COMPETITOR_THRESHOLD', 12), -- civs that have a voting history score equal to (VOTING_HISTORY_SCORE_MAX) / this value * -1 or worse in order are given greater consideration for "top World Congress rival".

-- They liked our proposal to the World Congress.
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_WEAK', -15),
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL', -30),
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_STRONG', -45),
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_OVERWHELMING', -60),
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', 134), -- increase for Diplomat AIs

-- They disliked our proposal to the World Congress.
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_WEAK', 15),
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL', 30),
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_STRONG', 45),
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_OVERWHELMING', 60),
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_DIPLOMAT_MULTIPLIER', 134), -- increase for Diplomat AIs

-- We passed their proposal in the World Congress.
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL', -10), -- min. opinion bonus
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_MAX', -60), -- max. opinion bonus
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', 167), -- increase for Diplomat AIs

-- We defeated their proposal in the World Congress.
	('OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL', 10), -- min. opinion penalty
	('OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_MAX', 60), -- max. opinion penalty
	('OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_DIPLOMAT_MULTIPLIER', 167), -- increase for Diplomat AIs

-- We helped them become/remain the host of the World Congress.
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING', -20), -- min. opinion bonus
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_MAX', -70), -- max. opinion bonus
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_DIPLOMAT_MULTIPLIER', 150), -- increase for Diplomat AIs

-- This % of the difference between the max and min values is added for each % that you contributed to the triggering vote
	('OPINION_WEIGHT_PER_VOTE_PERCENT', 2),

-- We [tried to place/placed] sanctions on them in the World Congress.
	('OPINION_WEIGHT_THEY_SANCTIONED_US', 50),
	('OPINION_WEIGHT_THEY_SANCTIONED_US_DIPLOMAT_MULTIPLIER', 200), -- increase for Diplomat AIs
	('OPINION_WEIGHT_THEY_SANCTIONED_US_FAILURE_DIVISOR', 200), -- decrease if you don't succeed in placing the sanctions

-- We [tried to free/freed] them from sanctions in the World Congress.
	('OPINION_WEIGHT_THEY_UNSANCTIONED_US', -50),
	('OPINION_WEIGHT_THEY_UNSANCTIONED_US_DIPLOMAT_MULTIPLIER', 200), -- increase for Diplomat AIs
	('OPINION_WEIGHT_THEY_UNSANCTIONED_US_FAILURE_DIVISOR', 200), -- decrease if you don't succeed in removing the sanctions

--	//////////////////////////////////////
--	// CIV IV DIPLOMATIC FEATURES
--	//////////////////////////////////////

-- Opinion Weight for AI being Master
	('OPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER', -40),

-- Opinion Weights for AI being Vassal
	('OPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL', -20),
	('OPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL', 0),
	('OPINION_WEIGHT_SAME_MASTER', -60),
	('OPINION_WEIGHT_SAME_MASTER_DIFFERENT_SURRENDER_DIVISOR', 200), -- reduces the same master bonus between voluntary and capitulated vassals

-- Vassalage Treatment Thresholds
-- Determines vassal treatment "level", based on the Opinion Score from Vassal Treatment
	('VASSALAGE_TREATMENT_THRESHOLD_DISAGREE', 1),
	('VASSALAGE_TREATMENT_THRESHOLD_MISTREATED', 25),
	('VASSALAGE_TREATMENT_THRESHOLD_UNHAPPY', 50),
	('VASSALAGE_TREATMENT_THRESHOLD_ENSLAVED', 80),

-- Vassal Treatment Score Factors
-- Making Demands
	('OPINION_WEIGHT_DEMANDED_WHILE_VASSAL', 100), -- if > 100: reduction to penalty for demanding from vassals

-- Stealing From Them
	('OPINION_WEIGHT_CAPITULATED_VASSAL_PLUNDERED_DIVISOR', 100), -- if > 100: reduction to penalty for plundering TRs from capitulated vassals
	('OPINION_WEIGHT_CAPITULATED_VASSAL_SPYING_DIVISOR', 100), -- if > 100: reduction to penalty for spy actions against capitulated vassals
	('OPINION_WEIGHT_CAPITULATED_VASSAL_CULTURE_BOMB_DIVISOR', 100), -- if > 100: reduction to penalty for stealing territory from capitulated vassals

-- Denouncing Them
	('OPINION_WEIGHT_VASSAL_DENOUNCED_BY_MASTER', 25), -- Master denounced vassal!

-- Taxation Policies
-- Penalty = (((Gold Taxed By Masters Since Vassalage Started * 100 / Gross Gold Collected Since Vassalage Started)^OPINION_WEIGHT_VASSAL_TAX_EXPONENT) / OPINION_WEIGHT_VASSAL_TAX_DIVISOR) + (Current Tax Rate / OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER)
	('OPINION_WEIGHT_VASSAL_TAX_EXPONENT', 1.5),
	('OPINION_WEIGHT_VASSAL_TAX_DIVISOR', 4),
	('OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER', 50),

-- You protected their territory as their master! (n.b. maximum vassal protect value = OPINION_WEIGHT_VASSALAGE_PROTECT_MAX * VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_VASSALAGE_PROTECT_MAX', -50), -- maximum opinion bonus from vassal protection
	('VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT', 50), -- how much vassal protection value equals +/- 1 opinion weight
	('VASSALAGE_PROTECTED_PER_TURN_DECAY', 25), -- how fast vassal protection value decays
	('VASSALAGE_PROTECTED_CITY_DISTANCE', 6), -- How close to a vassal city must an enemy unit be (in tiles) for it to count for protect value.

-- You failed to protect their territory as their master! (n.b. minimum vassal protect value = OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX * VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT * -1; scales with game speed)
	('OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX', 50), -- maximum opinion penalty from failed vassal protection
	('VASSALAGE_FAILED_PROTECT_CITY_DISTANCE', 0), -- How close to a vassal city must a vassal unit be for it to count for failed protect value (IF NOT IN OWNED TERRITORY). 0 = disabled.

-- Trade Routes With Them
	('OPINION_WEIGHT_VASSAL_TRADE_ROUTE', -15), -- only applies for trade routes the master sends to the vassal
	('OPINION_WEIGHT_VASSAL_TRADE_ROUTE_SUBSEQUENT', -10), -- any after the first

-- Opened Our Borders
	('OPINION_WEIGHT_VASSAL_OPEN_BORDERS', -5), -- for giving them Open Borders

-- Shared Religion Interests
	('OPINION_WEIGHT_VASSAL_FOUNDER_MASTER_ADOPTED_RELIGION', -40), -- if master has adopted the vassal's religion in a majority of their cities
	('OPINION_WEIGHT_VASSAL_HAPPILY_ADOPTED_RELIGION', -20), -- if vassal has no state religion and adopted master's religion
	('OPINION_WEIGHT_VASSAL_SAME_STATE_RELIGION', -10), -- master and vassal have same majority religion (not master's owned religion)

-- Multiplier to negative factors in Vassal Treatment Opinion Score when AI is a Voluntary Vassal
	('OPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD', 120),

-- Master liberated Vassal without being asked
	('OPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE', -50),

-- Master liberated Vassal when the Vassal requested
	('OPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED', -25),

-- Master refused to liberate Vassal when the Vassal requested (declared war!)
	('OPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED', 50),

-- Opinion Weight Change from Having 2+ Vassals
-- Each player on a team counts as one vassal. Your own vassals + players you have a DoF or DP with don't apply this penalty.
	('OPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS', 20),

--	//////////////////////////////////////
--	// PROMISE DURATIONS
--	//////////////////////////////////////

-- Defines how many turns a specific promise lasts for (for making the promise), capped below at 1.
-- Defines how many turns until the AI forgets that you ignored/broke a specific promise (for other values). Once the AI forgets that you ignored/broke a promise, they become able to ask you to make it again.
-- Most promise lengths scale with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x

-- Military/Move Troops Request
-- This promise does NOT scale with game speed!
	('MOVE_TROOPS_MEMORY_TURN_EXPIRATION', 20),
	('MILITARY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 40),
	('MILITARY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 80),

-- Stop Settling Near Us
	('EXPANSION_PROMISE_TURNS_EFFECTIVE', 50),
	('EXPANSION_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('EXPANSION_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Stop Buying Land Near Us
	('BORDER_PROMISE_TURNS_EFFECTIVE', 50), 
	('BORDER_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('BORDER_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Stop Bullying Our Protected City-States
	('BULLY_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('BULLY_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Don't Conquer Our Protected City-States
	('ATTACK_CS_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 40),
	('ATTACK_CS_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 60),

-- Stop Spying On Us
	('SPY_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('SPY_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Stop Converting Our Cities
	('CONVERT_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('CONVERT_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Stop Digging Up Our Artifacts
	('DIGGING_PROMISE_IGNORED_TURNS_UNTIL_FORGIVEN', 30),
	('DIGGING_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 50),

-- Broken Coop War Promise
-- This promise does NOT scale with game speed!
	('COOP_WAR_PROMISE_BROKEN_TURNS_UNTIL_FORGIVEN', 60),

--	//////////////////////////////////////
--	// BACKSTABBING PENALTY DURATIONS
--	//////////////////////////////////////

-- Defines how many turns until the AI forgets that you backstabbed them, capped below at 1.
-- The amount of turns scales with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x
-- NOTE: Broken military / CS conquest promises count as backstabbing penalties as well, but they are handled in the Promises section just above.

	('DOF_BROKEN_TURNS_UNTIL_FORGIVEN', 50),
	('DOF_BROKEN_TURNS_UNTIL_FORGIVEN_FRIENDS', 10), -- if Opinion has risen to FRIEND or higher since ending the DoF
	('FRIEND_DENOUNCED_US_TURNS_UNTIL_FORGIVEN', 75),
	('FRIEND_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', 100),
	('MASTER_DECLARED_WAR_ON_US_TURNS_UNTIL_FORGIVEN', 100),

-- Number of turns after prematurely ending a DoF during which you will obtain backstabbing penalties for denouncing or declaring war.
-- It will always take AT LEAST this many turns to clear the penalty for prematurely ending a DoF.
-- This value does not scale with game speed!
	('DOF_BROKEN_BACKSTAB_TIMER', 10),

--	//////////////////////////////////////
--	// OTHER OPINION MODIFIER DURATIONS
--	//////////////////////////////////////

-- Defines the base number of turns until the AI forgets about an opinion modifier, capped below at 1.
-- When there are multiple "stacks" of a modifier, this instead is the time before the # of stacks is halved (rounded down).
-- The amount of turns scales with game speed: Quick 0.67x, Standard 1x, Epic 1.5x, Marathon 3x
-- The amount of turns is also increased or decreased based on leader flavors (and this increase/decrease also scales with game speed).
-- Usually, positive modifiers scale directly with Loyalty, and negative modifiers scale inversely with Forgiveness. Exceptions are noted.
-- Certain modifiers are not included here.

	('PLUNDERED_TRADE_ROUTE_TURNS_UNTIL_FORGIVEN', 15),
	('RETURNED_CIVILIAN_TURNS_UNTIL_FORGOTTEN', 50),
	('BUILT_LANDMARK_TURNS_UNTIL_FORGOTTEN', 50),
	('LIBERATED_CITY_TURNS_UNTIL_FORGOTTEN', 75),
	('FORGAVE_FOR_SPYING_TURNS_UNTIL_FORGOTTEN', 30),
	('SHARED_INTRIGUE_TURNS_UNTIL_FORGOTTEN', 50),
	('ROBBED_US_TURNS_UNTIL_FORGIVEN', 50),
	('PLOTTED_AGAINST_US_TURNS_UNTIL_FORGIVEN', 14), -- 0-26 is added to this value based on perceived threat level of the plotter; does not scale with game speed
	('BEATEN_TO_WONDER_TURNS_UNTIL_FORGIVEN', 60), -- indirect penalty to WonderDisputeLevel
	('LOWERED_OUR_INFLUENCE_TURNS_UNTIL_FORGIVEN', 40), -- indirect penalty to MinorCivDisputeLevel
	('PERFORMED_COUP_TURNS_UNTIL_FORGIVEN', 50),
	('EXCAVATED_ARTIFACT_TURNS_UNTIL_FORGIVEN', 50),
	('MADE_DEMAND_TURNS_UNTIL_FORGIVEN', 50),
	('RELIGIOUS_CONVERSION_TURNS_UNTIL_FORGIVEN', 25),
	('OPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN', 50), -- scales with Protective approach bias towards City-States
	('OPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN', 30), -- scales with Protective approach bias towards City-States
	('OPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN', 30),
	('OPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN', 10),
	('OPINION_WEIGHT_WE_LIKED_THEIR_PROPOSAL_NUM_TURNS', 50), -- scales with WorkWithWillingness
	('OPINION_WEIGHT_WE_DISLIKED_THEIR_PROPOSAL_NUM_TURNS', 50), -- scales with WorkAgainstWillingness
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_PROPOSAL_NUM_TURNS', 50), -- scales with WorkWithWillingness and sometimes Neediness
	('OPINION_WEIGHT_THEY_FOILED_OUR_PROPOSAL_NUM_TURNS', 50), -- scales with WorkAgainstWillingness and sometimes Forgiveness
	('OPINION_WEIGHT_THEY_SUPPORTED_OUR_HOSTING_NUM_TURNS', 50), -- scales with WorkWithWillingness
	('SANCTIONED_US_TURNS_UNTIL_FORGIVEN', 50), -- scales with Meanness if the attempt succeeded, WorkAgainstWillingness otherwise
	('UNSANCTIONED_US_TURNS_UNTIL_FORGOTTEN', 50), -- scales with Loyalty if the attempt succeeded, WorkWithWillingness otherwise
	('OPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN', 100), -- How many turns the "gave independence when asked" bonus remains for
	('OPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN', 100); -- How many turns the "refused to give independence" when asked penalty remains for