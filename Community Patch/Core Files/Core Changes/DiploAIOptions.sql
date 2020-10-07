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
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_ENABLE_NUCLEAR_GANDHI', '1';

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

-- [PASSIVE MODE (TOWARDS HUMANS)]
-- If set to 1, AI civilizations will never declare war on human players. They also won't attempt Domination Victories, unless they can win one without declaring war on a human.
-- Humans can still declare war on AI players, and AI players already at war will still fight their opponents.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_PASSIVE_MODE_HUMANS', '0';

-- [PASSIVE MODE (GLOBAL)]
-- If set to 1, AI civilizations will never declare war on any other civilizations or City-States. They also won't attempt Domination Victories, unless they can win one without declaring war.
-- Humans can still declare war on AI players, and AI players already at war will still fight their opponents.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_PASSIVE_MODE_GLOBAL', '0';

-- [AGGRESSIVE MODE (TOWARDS HUMANS)]
-- If set to 1, AI civilizations will be much more aggressive towards human players.
-- This is just for fun, and is not balanced at all!
-- NOTE: If Passive Mode (Towards Humans) or Passive Mode (Global) are enabled, this does nothing.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_AGGRESSIVE_MODE_HUMANS', '0';

-- [AGGRESSIVE MODE (GLOBAL)]
-- If set to 1, AI civilizations will be much more aggressive towards all other civilizations and City-States.
-- They will still prioritize their aggression, however, so they will not declare war on the entire planet at once.
-- NOTE: If Passive Mode (Global) is enabled, this does nothing. If Passive Mode (Towards Humans) is enabled, Aggressive Mode does not affect attitudes towards human players.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_AGGRESSIVE_MODE_GLOBAL', '0';


-- Debug Mode

-- [ENABLE DEBUG MODE]
-- If set to 1, enables the Diplomacy AI Debug Mode.
-- If set to 2, enables the Diplomacy AI Debug Mode and forces the AI to accept all Discuss requests you make of them.
-- Transparent Diplomacy is activated (even if C4DF is not enabled).
-- AI civilizations will display their true approach towards you in their table of opinion modifiers.
-- AI will always agree to share their Approach towards other players, and will always share their true Approach.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLOAI_ENABLE_DEBUG_MODE', '0';