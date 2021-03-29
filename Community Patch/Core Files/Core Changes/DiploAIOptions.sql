-- Diplomacy AI Options
-- All of these options are savegame compatible (enabling/disabling them and then loading a saved game will activate/deactivate them with no issues).

-- [DISABLE VICTORY COMPETITION]
-- If set to 1, AI civilizations will not receive any aggression boosts or reductions towards other players based on their victory progress.
-- Also, you will not receive any diplomacy penalties for victory competition with the AI.
-- This ONLY affects aggression changes that are caused by victory competition.
-- Conquering the world or gaining cultural Influence over the AI, for example, can still result in aggression because you pose a threat to them OR because you're simply in their way.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_VICTORY_COMPETITION', '0';

-- [DISABLE ENDGAME AGGRESSION BOOST]
-- If set to 1, AI civilizations will not receive the large late-game aggression boost towards players that are close to victory.
-- Bonuses/penalties for general victory competition will continue to apply.
-- NOTE: If Victory Competition is disabled, this does nothing.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_ENDGAME_AGGRESSION', '0';

-- [ENABLE NUCLEAR GANDHI]
-- If set to 1, Gandhi (if he has nukes, and Random Personalities isn't enabled) will do what he's best known for in the Civilization series. ;)
-- NOTE: Does nothing if the Community Balance Patch is not enabled.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_ENABLE_NUCLEAR_GANDHI', '1';

-- [DISABLE WAR BRIBES]
-- If set to 1, AI civilizations are unable to bribe other AI civilizations into declaring war, but they can still bribe (and be bribed) by humans.
-- If set to 2, the option to bribe another AI civilization into declaring war on a third party is removed globally.
-- War bribes between human players are unaffected by this setting.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_WAR_BRIBES', '0';

-- [DISABLE CITY TRADING]
-- If set to 1, AI civilizations are unable to trade cities with other AIs, but can still trade cities with human players.
-- If set to 2, AI civilizations cannot trade cities with humans or other AIs, but humans can still trade cities with other humans.
-- If set to 3, all city trading between civilizations is disabled.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_CITY_TRADING', '0';

-- [DISABLE VOLUNTARY VASSALAGE]
-- If set to 1, voluntary vassalage is not permitted, but capitulation is still allowed.
-- NOTE: Does nothing if vassalage is not enabled.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_VOLUNTARY_VASSALAGE', '0';

-- [DISABLE INSULT MESSAGES]
-- If set to 1, AI civilizations will not send threats/insults such as "you're a warmonger" or "you're getting too friendly with our City-State friends" to human players.
-- Note that these messages provide a chance to provoke the AI into declaring war. Disabling them removes this chance.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_INSULT_MESSAGES', '0';

-- [DISABLE COMPLIMENT MESSAGES]
-- If set to 1, AI civilizations will not send friendly compliments such as "FYI, we befriended your friend" to human players.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_COMPLIMENT_MESSAGES', '0';

-- [SHOW ALL OPINION MODIFIERS]
-- If set to 1, AI civilizations will never hide Opinion modifiers. They often hide modifiers when they are FRIENDLY (or pretending to be).
-- Unlike Transparent Diplomacy, the number value of Opinion modifiers will not be displayed to the player.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_SHOW_ALL_OPINION_MODIFIERS', '0';

-- [SHOW ALL OPINION VALUES]
-- If set to 1, AI opinion modifiers will have their number values displayed, so you can see how much a factor is affecting your relationship.
-- Unlike Transparent Diplomacy, the AI can still choose to hide Opinion modifiers from the player.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_SHOW_ALL_OPINION_VALUES', '0';

-- [SHOW BASE HUMAN OPINION]
-- If set to 1, a modifier explaining the AI's instinctive like/dislike of human players will be displayed in their table of opinion modifiers.
-- This value is otherwise hidden unless Show All Opinion Modifiers, Transparent Diplomacy or Debug Mode are enabled.
-- Only does anything if AI Opinion score towards humans has been modified via OPINION_WEIGHT_BASE_HUMAN in DiploOpinionWeights.sql or AttitudeChange in DifficultyMod.xml.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_SHOW_BASE_HUMAN_OPINION', '0';



-- Advanced Options

-- [DISABLE FLAVOR RANDOMIZATION]
-- If set to 1, diplomacy flavors for AI players will not be randomized by +/- 2. They will instead always be set to their base XML value.
-- NOTE: This only affects AI flavors related to diplomacy, not other flavors. Also, changing this setting will have no effect if you load a saved game; you must start a new game!
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_NO_FLAVOR_RANDOMIZATION', '0';

-- [ENABLE LUMP SUM GOLD TRADES]
-- If set to 1, enables human players to trade/demand lump sum Gold with AI players at all times.
-- If set to 2, enables all players to trade/demand lump sum Gold with each other at all times.
-- This is exploitable, so enabling this option reduces the difficulty if you take advantage of it!
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_ENABLE_LUMP_GOLD_TRADES', '0';

-- [DISABLE FRIENDSHIP REQUESTS]
-- If set to 1, AI civilizations will not ask human players to make a Declaration of Friendship.
-- Humans can still ask the AI for a Declaration of Friendship.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_FRIENDSHIP_REQUESTS', '0';

-- [DISABLE GIFT OFFERS]
-- If set to 1, AI civilizations will not offer gifts to human players.
-- Humans can still request help from AI players on their own turn.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_GIFT_OFFERS', '0';

-- [DISABLE COOP WAR REQUESTS]
-- If set to 1, AI civilizations will not request that friends join them in a cooperative war against other players.
-- This does not affect offers in the Trade screen to declare war on a third party.
-- Humans can still request that AI friends join them in a cooperative war.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_COOP_WAR_REQUESTS', '0';

-- [DISABLE HELP REQUESTS]
-- If set to 1, AI civilizations will not request help (gold/resources/etc.) from human players.
-- Does not affect the human's ability to request help from the AI.
-- Humans can still offer gifts to AI players on their own turn.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_HELP_REQUESTS', '0';

-- [DISABLE TRADE OFFERS]
-- If set to 1, AI civilizations will not send trade offers to human players (except peace offers).
-- Humans can still trade with AI players on their own turn.
-- Does not prevent the AI from making demands.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_TRADE_OFFERS', '0';

-- [DISABLE PEACE OFFERS]
-- If set to 1, AI civilizations will not request peace from human players.
-- NOTE: Offers to make peace with a third party are treated as a trade offer.
-- Humans can still make peace with AI players on their own turn.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_PEACE_OFFERS', '0';

-- [DISABLE ALL STATEMENTS]
-- If set to 1, AI civilizations will never send ANY messages to human players on their turn.
-- This also disables popup messages, e.g. from returning civilians or stealing territory.
-- Humans can still interact with the AI normally through the diplomacy screen.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_ALL_STATEMENTS', '0';

-- [PASSIVE MODE]
-- If set to 1, AI civilizations will never declare war on human players. They also won't attempt Domination Victories, unless they can win one without declaring war on a human.
-- If set to 2, AI civilizations will never declare war on any other civilizations or City-States. They also won't attempt Domination Victories, unless they can win one without declaring war.
-- Humans can still declare war on AI players, and AI players already at war will still fight their opponents.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_PASSIVE_MODE', '0';

-- [AGGRESSIVE MODE]
-- If set to 1, AI civilizations will be much more aggressive towards human players. This is just for fun, and is not balanced at all! NOTE: Has no effect if Passive Mode is enabled.
-- If set to 2, AI civilizations will be much more aggressive towards all other civilizations and City-States. NOTE: If Passive Mode is set to 1, this has no effect towards human players, and if set to 2, this has no effect.
-- Unless disabled with DISABLE DOMINATION ONLY AGGRESSION BOOST below, this setting is set to 2 automatically if Passive Mode is not enabled and only Domination and/or Time victories are possible.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_AGGRESSIVE_MODE', '0';

-- [DISABLE DOMINATION ONLY AGGRESSION BOOST]
-- If set to 1, AI civilizations' Aggressive Mode (see above) will not be automatically enabled if only Domination and/or Time victories are possible.
-- Instead, they will be set to their normal levels of aggression. NOTE: If Aggressive Mode is enabled manually above, this setting does nothing.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION', '0';


-- Debug Mode

-- [ENABLE DEBUG MODE]
-- If set to 1, enables the Diplomacy AI Debug Mode.
-- If set to 2, enables the Diplomacy AI Debug Mode and forces the AI to accept all Discuss requests you make of them.
-- Transparent Diplomacy is activated (even if C4DF is not enabled).
-- AI civilizations will display their true approach towards you in their table of opinion modifiers.
-- AI will always agree to share their Approach towards other players, and will always share their true Approach.
-- You also have full knowledge of all AI civs' World Congress desires while this option is enabled, regardless of ideology/diplomats.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_ENABLE_DEBUG_MODE', '0';