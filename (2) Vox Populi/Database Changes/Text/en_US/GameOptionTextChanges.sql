-- No Barbarians
UPDATE Language_en_US
SET Text = 'Barbarians and their Encampments do not appear on the map. Rebel (barbarian) uprisings from [ICON_HAPPINESS_4] Unhappiness do not occur.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_NO_BARBARIANS_HELP';

-- Complete Kills
UPDATE Language_en_US
SET Text = 'In order to be eliminated from the game, a player must have all of his Cities AND Units destroyed. A player who remains alive for 10 turns with no Cities or Settlers will receive a free Settler.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_COMPLETE_KILLS_HELP';