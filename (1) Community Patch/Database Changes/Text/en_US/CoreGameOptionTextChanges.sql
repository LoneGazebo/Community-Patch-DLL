-- TODO: Investigate whether the option itself can be removed
UPDATE Language_en_US
SET Text = 'This option is unused and should not be enabled.'
WHERE Tag = 'TXT_KEY_GAME_OPTION_NEW_RANDOM_SEED_HELP';
