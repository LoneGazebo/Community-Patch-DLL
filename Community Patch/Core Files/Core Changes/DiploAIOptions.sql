-- Advanced Diplo AI Options
-- These options are savegame compatible (enabling/disabling them and then loading the game will activate/deactivate them with no issues).

-- !! NOTE TO PLAYERS !!
-- If you are using any of these advanced settings (except Nuclear Gandhi, which is enabled by default),
-- please be sure to INDICATE IT when posting feedback or bug reports!

-- [SHOW NUMBER VALUES FOR OPINION MODIFIERS]
-- If set to 1, AI opinion modifiers will have their number values displayed, so you can see how much a factor is affecting your relationship.
-- HOWEVER, unlike Transparent Diplomacy, the AI can still hide negative modifiers when they are FRIENDLY (or pretending to be).
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_SHOW_OPINION_VALUES', '0';

-- [SHOW TRUE APPROACHES]
-- If set to 1, the AI's true Approach (FRIENDLY, HOSTILE, WAR, etc.) towards human players will be visible at all times in the AI's opinion table.
-- You can view the opinion table by mousing over the AI's visible Approach.
-- Enabling this debug setting will also activate Transparent Diplomacy, even if disabled in this game's options (and even if the C4DF mod isn't installed).
-- In addition, if C4DF is enabled, the AI will always share their true Approach towards other players when requested (if not at war with you).
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_SHOW_TRUE_APPROACHES', '0';

-- [DISABLE VICTORY COMPETITION]
-- If set to 1, the AI will not receive any aggression boosts towards competitors for victory conditions.
-- Also, you will not receive any diplomacy penalties for victory competition with the AI.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_NO_VICTORY_COMPETITION', '0';

-- NOTE: Enabling the passive AI settings below will cause the AI not to attempt Domination Victories.

-- [PASSIVE AI (HUMAN ONLY)]
-- If set to 1, AI civilizations will never initiate wars with human players.
-- Humans can still initiate wars with AI players.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_WAR_DISALLOWED_HUMAN', '0';

-- [PASSIVE AI (GLOBAL)]
-- If set to 1, AI civilizations will never initiate wars with other civs or City-States.
-- Humans can still initiate wars with AI players.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_WAR_DISALLOWED_GLOBAL', '0';

-- [ENABLE NUCLEAR GANDHI]
-- If set to 1, Gandhi (if he has nukes, and Random Personalities isn't enabled) will do what he's best known for in the Civilization series. ;)
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_ENABLE_NUCLEAR_GANDHI', '1';


-- Visual only

-- [SHOW BASE OPINION SCORE]
-- If set to 1, and the base Opinion score for the human (see DiploOpinionWeights.sql) is non-zero, a modifier explaining the AI's like/dislike will be displayed in the AI's opinion table.
-- Hidden by default. If Transparent Diplomacy or Show True Approaches is enabled, the base opinion score and modifier will be displayed even if this setting is disabled.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_SHOW_BASE_OPINION_WEIGHT', '0';

-- [DISABLE SHOWING NEUTRAL MODIFIER VALUES]
-- If set to 1, if Transparent Diplomacy or Show True Approaches is enabled, white (neutral) opinion modifiers will not display a "(0)" after them in the AI's opinion table.
-- This is because they never affect opinion anyway.
-- Just for players who are OCD about it. :)
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_AI_HIDE_NEUTRAL_OPINION_VALUES', '0';


-- Debug

-- Debug variable to enable certain AI logging and/or behavior changes for testing.
-- Added to avoid needing to make database changes for test builds.
-- Might also be used by players to diagnose certain problems.
-- Keep this set to 0 unless you A) were instructed to change this or B) know what you're doing, otherwise unexpected results may occur.
INSERT INTO Defines (Name, Value)
SELECT 'DIPLO_DEBUG_ENABLED', '0';