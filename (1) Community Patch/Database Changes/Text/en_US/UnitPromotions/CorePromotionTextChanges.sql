-- Great Generals
UPDATE Language_en_US
SET Text = '+50% [ICON_GREAT_GENERAL] Great General Points from combat.'
WHERE Tag = 'TXT_KEY_PROMOTION_SPAWN_GENERALS_I_HELP';

UPDATE Language_en_US
SET Text = '+100% [ICON_GREAT_GENERAL] Great General Points from combat.'
WHERE Tag = 'TXT_KEY_PROMOTION_SPAWN_GENERALS_II_HELP';

UPDATE Language_en_US
SET Text = 'Cannot End Turn on Ocean Tile'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE';

UPDATE Language_en_US
SET Text = 'Cannot End Turn on Ocean Tile until Astronomy'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO';

UPDATE Language_en_US
SET Text = 'Cannot end turn in [COLOR_NEGATIVE_TEXT]Ocean[ENDCOLOR] until you have researched [COLOR_CYAN]{TXT_KEY_TECH_ASTRONOMY_TITLE}[ENDCOLOR].[NEWLINE]Can move through visible Ocean if destination is visible Coast.'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';

UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PROMOTION_MUST_SET_UP';

UPDATE Language_en_US
SET Text = 'Moves at half-speed in enemy territory'
WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_MUST_SET_UP';

-- Heavy Charge
UPDATE Language_en_US
SET Text = 'Forces defender to retreat if it inflicts more damage than it receives. A defender who cannot retreat takes 50% extra damage.'
WHERE Tag = 'TXT_KEY_PROMOTION_HEAVY_CHARGE_HELP';
