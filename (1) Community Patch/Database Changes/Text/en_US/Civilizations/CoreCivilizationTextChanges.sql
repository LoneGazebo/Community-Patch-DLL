UPDATE Language_en_US
SET Text = 'Adds a Moai (+1 [ICON_CULTURE] Culture) to the tile. Generates +1 [ICON_CULTURE] Culture for each adjacent Moai. Also provides +1 [ICON_GOLD] Gold after Flight is researched. Can only be built on coastal tiles.'
WHERE Tag = 'TXT_KEY_BUILD_MOAI_HELP';

UPDATE Language_en_US
SET Text = 'Moai can only be built on coastal tiles. If built next to another Moai, it provides additional [ICON_CULTURE] Culture. After the Flight technology is researched it also provides extra [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MOAI_HELP';

-- Fixed diacritics for spy names
UPDATE Language_en_US
SET Text = 'Antônio'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_0';

UPDATE Language_en_US
SET Text = 'Estêvão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_3';

UPDATE Language_en_US
SET Text = 'Fernão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_4';

UPDATE Language_en_US
SET Text = 'Tomé'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_8';
