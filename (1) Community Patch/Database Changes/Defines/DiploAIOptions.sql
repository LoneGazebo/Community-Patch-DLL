-- Advanced Diplomacy AI Options
-- All of these options (except LIMIT VICTORY PURSUIT RANDOMIZATION, where only option 3 works, and DISABLE FLAVOR RANDOMIZATION, which doesn't work at all) are savegame compatible.
-- This means that enabling/disabling them and then loading a saved game will activate/deactivate them with no issues.

REPLACE INTO Defines
	(Name, Value)
VALUES

-- [ENABLE NUCLEAR GANDHI]
-- If set to 1, Gandhi (if he has nukes, and Random Personalities isn't enabled) will do what he's best known for in the Civilization series. ;)
-- If set to 2, he'll do it even if Random Personalities is enabled.
	('DIPLOAI_ENABLE_NUCLEAR_GANDHI', 1),

-- [ENABLE TEMPORARY FOR PERMANENT TRADING]
-- If set to 0, humans cannot trade temporary items (gold per turn, resources, etcetera) for permanent items (lump sum gold, cities, etcetera) with AI players.
-- If set to 1 (default), AI players can trade their temporary items for the human's permanent items, but not vice versa.
-- If set to 2, there are no restrictions on trading temporary items for permanent items.
-- Option 2 is exploitable, so enabling this option reduces the difficulty if you take advantage of it!
-- Human-to-human trades, trades with "Permanent Peace" or "Permanent War or Peace" enabled, and trades between teammates are always unrestricted.
	('DIPLOAI_TEMPORARY_FOR_PERMANENT_TRADING_SETTING', 1),

-- [DISABLE WAR BRIBES]
-- If set to 1, AI civilizations are unable to bribe other AI civilizations into declaring war, but they can still bribe (and be bribed) by humans.
-- If set to 2, the option to bribe another AI civilization into declaring war on a third party is removed globally.
-- War bribes between human players are unaffected by this setting.
	('DIPLOAI_DISABLE_WAR_BRIBES', 0),

-- [DISABLE CITY TRADING]
-- If set to 1, AI civilizations are unable to trade cities with other AIs, but can still trade cities with human players.
-- If set to 2, AI civilizations cannot trade cities with humans or other AIs, but humans can still trade cities with other humans.
-- If set to 3, all city trading between civilizations is disabled.
	('DIPLOAI_DISABLE_CITY_TRADING', 0),

-- [DISABLE VOLUNTARY VASSALAGE]
-- If set to 1, voluntary vassalage is not permitted, but capitulation is still allowed.
-- NOTE: Does nothing if vassalage is not enabled.
	('DIPLOAI_DISABLE_VOLUNTARY_VASSALAGE', 0),

-- [LIMIT VICTORY PURSUIT RANDOMIZATION]
-- If set to 1, the AI will always select the Primary Victory Pursuit that it receives a hint for in XML. If Random Personalities is enabled, it will use the hint of the leader it's acting as.
-- If there isn't a Primary Victory Pursuit hint defined in XML, or if the Primary Victory Pursuit hint is for a disabled victory condition, one will be selected randomly.
-- If set to 2, the AI will ALSO avoid selecting a Secondary Victory Pursuit unless one has been defined in XML.
-- If set to 3, all AI players will also *ALWAYS* choose their Primary Victory Pursuit hint as their Current Victory Pursuit. If the victory condition is disabled but the secondary pursuit is valid, it will use that one instead.
-- NOTE 1: Options 1 and 2 only increase the likelihood that the AI will go for that pursuit (and modify diplo AI behavior in favor of that). Option 3 guarantees it, but forbids the AI from adapting to circumstances.
-- NOTE 2: Option 3 also ignores the Random Personalities game option.
-- NOTE 3: For Option 3, Domination Victory will be attempted if it is possible for the AI to eliminate all other major civs, ignoring whether or not Domination Victory is enabled in game options.
	('DIPLOAI_LIMIT_VICTORY_PURSUIT_RANDOMIZATION', 0),

-- [DISABLE VOTING FOR OTHER PLAYERS AS WORLD CONGRESS HOST]
-- If set to 1, the AI will never vote for others (EXCEPT teammates) as host of the World Congress.
	('DIPLOAI_NO_OTHER_HOST_VOTES', 0),

-- [DISABLE VOTING FOR OTHER PLAYERS AS WORLD LEADER]
-- If set to 1, the AI will never freely vote for others (EXCEPT teammates) for World Leader / Global Hegemony.
-- If set to 2, they will also refuse to sell their World Leader votes to anyone.
	('DIPLOAI_NO_OTHER_WORLD_LEADER_VOTES', 0),

-- [DISABLE INSULT MESSAGES]
-- If set to 1, AI civilizations will not send threats/insults such as "you're a warmonger" or "you're getting too friendly with our City-State friends" to human players.
-- Note that these messages provide a chance to provoke the AI into declaring war. Disabling them removes this chance.
	('DIPLOAI_DISABLE_INSULT_MESSAGES', 0),

-- [DISABLE COMPLIMENT MESSAGES]
-- If set to 1, AI civilizations will not send friendly compliments such as "FYI, we befriended your friend" to human players.
	('DIPLOAI_DISABLE_COMPLIMENT_MESSAGES', 0),

-- [NO FAKE OPINION MODIFIERS]
-- If set to 1, AI civilizations are unable to fake having no disputes by displaying false modifiers in the opinion table (i.e. no contested borders, no competition with City-States, etc.).
-- Does not prevent the AI from concealing their true Approach (i.e. pretending to be FRIENDLY or displaying "They desire friendly relations with our empire").
-- NOTE: If Show Hidden Opinion Modifiers, Transparent Diplomacy, or Debug Mode are enabled, this option does nothing.
	('DIPLOAI_NO_FAKE_OPINION_MODIFIERS', 1),

-- [SHOW HIDDEN OPINION MODIFIERS]
-- If set to 1, AI civilizations will never hide Opinion modifiers. They often hide modifiers when they are FRIENDLY (or pretending to be).
-- Unlike Transparent Diplomacy, the number value of Opinion modifiers will not be displayed to the player.
-- NOTE: This will not reveal the base human opinion bonus on lower difficulties; Show Base Human Opinion or Debug Mode must be enabled to view that.
	('DIPLOAI_SHOW_HIDDEN_OPINION_MODIFIERS', 0),

-- [SHOW ALL OPINION VALUES]
-- If set to 1, AI opinion modifiers will have their number values displayed, so you can see how much a factor is affecting your relationship.
-- Unlike Transparent Diplomacy, the AI can still choose to hide Opinion modifiers from the player.
	('DIPLOAI_SHOW_ALL_OPINION_VALUES', 0),

-- [SHOW BASE HUMAN OPINION]
-- If set to 1, a modifier explaining the AI's instinctive like/dislike of human players will be displayed in their table of opinion modifiers.
-- This value is otherwise hidden unless Debug Mode is enabled due to being immersion-breaking.
-- Only does anything if AI Opinion score towards humans has been modified via HumanOpinionChange in DifficultyMod.xml.
	('DIPLOAI_SHOW_BASE_HUMAN_OPINION', 0),


-- Advanced Options

-- [HIDE OPINION TABLE]
-- If set to 1, no AI opinion modifiers or values will appear in the table (except a short message explaining their visible approach).
-- For those who want extra challenge/immersion.
-- NOTE: Overrides Transparent Diplomacy, Show Hidden Opinion Modifiers, and Show All Opinion Values. Does NOT override Debug Mode.
	('DIPLOAI_HIDE_OPINION_TABLE', 0),

-- [DISABLE FLAVOR RANDOMIZATION]
-- If set to 1, diplomacy flavors for AI players will not be randomized by +/- 2. They will instead always be set to their base XML value.
-- NOTE: This only affects AI flavors related to diplomacy, not other flavors. Also, changing this setting will have no effect if you load a saved game; you must start a new game!
	('DIPLOAI_NO_FLAVOR_RANDOMIZATION', 0),

-- [CUSTOMIZE WARSCORE THRESHOLD FOR THIRD PARTY PEACE]
-- The minimum warscore required before someone can make a third party peace offer to the winner of a war.
-- Valid values range from 1 to 100. Default value is 75. Lower values are more exploitable.
	('DIPLOAI_THIRD_PARTY_PEACE_WARSCORE', 75),

-- [DISABLE FRIENDSHIP REQUESTS]
-- If set to 1, AI civilizations will not ask human players to make a Declaration of Friendship.
-- Humans can still ask the AI for a Declaration of Friendship.
	('DIPLOAI_DISABLE_FRIENDSHIP_REQUESTS', 0),

-- [DISABLE GIFT OFFERS]
-- If set to 1, AI civilizations will not offer gifts to human players.
-- Humans can still request help from AI players on their own turn.
	('DIPLOAI_DISABLE_GIFT_OFFERS', 0),

-- [DISABLE COOP WAR REQUESTS]
-- If set to 1, AI civilizations will not request that HUMAN friends join them in a cooperative war against other players.
-- If set to 2, AI civilizations will also not request coop wars from AI friends.
-- This does not affect offers in the Trade screen to declare war on a third party.
-- Humans can still request that AI friends join them in a cooperative war.
	('DIPLOAI_DISABLE_COOP_WAR_REQUESTS', 0),

-- [DISABLE HELP REQUESTS]
-- If set to 1, AI civilizations will not request help (gold/resources/etc.) from human players.
-- Does not affect the human's ability to request help from the AI.
-- Humans can still offer gifts to AI players on their own turn.
	('DIPLOAI_DISABLE_HELP_REQUESTS', 0),

-- [DISABLE TRADE OFFERS]
-- If set to 1, AI civilizations will not send trade offers to human players, except peace offers and offers to renew an existing deal.
-- If set to 2, AI civilizations will only send peace offers, NOT offers to renew an existing deal.
-- Humans can still trade with AI players on their own turn.
-- Does not prevent the AI from making demands.
-- NOTE: To disable peace offers, see the setting directly below.
	('DIPLOAI_DISABLE_TRADE_OFFERS', 0),

-- [DISABLE PEACE OFFERS]
-- If set to 1, AI civilizations will not request peace from human players.
-- NOTE: Offers to make peace with a third party are treated as a trade offer.
-- Humans can still make peace with AI players on their own turn.
	('DIPLOAI_DISABLE_PEACE_OFFERS', 0),

-- [DISABLE DEMANDS]
-- If set to 1, AI civilizations are unable to make demands of human players.
-- Humans can still make demands of AI players.
	('DIPLOAI_DISABLE_DEMANDS', 0),

-- [DISABLE INDEPENDENCE REQUESTS]
-- If set to 1, AI vassals cannot request independence from human players.
-- Humans can still request independence from the AI.
	('DIPLOAI_DISABLE_INDEPENDENCE_REQUESTS', 0),

-- [DISABLE ALL STATEMENTS]
-- If set to 1, AI civilizations will never send ANY messages to human players on their turn.
-- This also disables popup messages, e.g. from returning civilians or stealing territory.
-- Humans can still interact with the AI normally through the diplomacy screen.
	('DIPLOAI_DISABLE_ALL_STATEMENTS', 0),

-- [PASSIVE MODE]
-- If set to 1, AI civilizations will never declare war on human players. They also won't attempt Domination Victories, unless they can win one without declaring war on a human.
-- If set to 2, AI civilizations will never declare war on any other civilizations or City-States. They also won't attempt Domination Victories, unless they can win one without declaring war.
-- Humans can still declare war on AI players, and AI players already at war will still fight their opponents.
	('DIPLOAI_PASSIVE_MODE', 0),

-- [AGGRESSIVE MODE]
-- If set to 1, AI civilizations will be much more aggressive towards human players. This is just for fun, and is not balanced at all! NOTE: Has no effect if Passive Mode is enabled.
-- If set to 2, AI civilizations will be much more aggressive towards all other civilizations and City-States. NOTE: If Passive Mode is set to 1, this has no effect towards human players, and if set to 2, this has no effect.
-- Unless disabled with DISABLE DOMINATION ONLY AGGRESSION BOOST below, this setting is set to 2 automatically if Passive Mode is not enabled and only Domination and/or Time victories are possible.
	('DIPLOAI_AGGRESSIVE_MODE', 0),

-- [DISABLE DOMINATION ONLY AGGRESSION BOOST]
-- If set to 1, AI civilizations' Aggressive Mode (see above) will not be automatically enabled if only Domination and/or Time victories are possible.
-- Instead, they will be set to their normal levels of aggression. NOTE: If Aggressive Mode is enabled manually above, this setting does nothing.
	('DIPLOAI_DISABLE_DOMINATION_ONLY_AGGRESSION', 0),


-- Debug Mode

-- [ENABLE DEBUG MODE]
-- If set to 1, enables the Diplomacy AI Debug Mode.
-- If set to 2, enables the Diplomacy AI Debug Mode and forces the AI to accept all requests you make of them in the Discuss menu.
	('DIPLOAI_ENABLE_DEBUG_MODE', 0);

-- EFFECTS:
-- Activates Show Hidden Opinion Modifiers, Show All Opinion Values, and Show Base Human Opinion options. Deactivates Hide Opinion Table option.
-- AI civilizations will display their true approach towards you in their table of opinion modifiers.
-- AI civilizations will display their top friend choice, top DP choice, top competitor, top World Congress ally, and top World Congress competitor in their table of opinion modifiers.
-- AI will always agree to share their Approach towards other players, and will always share their true Approach.
-- Notifications will be displayed when any AI civ agrees to a bribed war or peace deal.
-- You also have full knowledge of all AI civs' World Congress desires while this option is enabled, regardless of ideology/diplomats.
