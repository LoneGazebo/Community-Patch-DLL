-- Farm
UPDATE Language_en_US
SET Text = 'Farms can be constructed on most any land to improve the output of food on the tile. Farms gain +1 Food for every two adjacent Farms and/or if Fresh Water is available.[NEWLINE][NEWLINE]Farming is one of the earliest and most important of all human professions, as it allowed mankind to stop migrating and settle in one location without depleting the local resources.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FARM_TEXT';

-- Mine
UPDATE Language_en_US
SET Text = 'Mines are among the most important improvements in Civilization V. Not only do they increase a hex''s productivity output - and high productivity allows quicker construction of units, buildings, and wonders - but they also provide access to some of the most important resources in the game: iron, coal, and aluminium, to name a few.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_MINE_TEXT';

-- Lumber Mill
UPDATE Language_en_US
SET Text = 'A Lumber Mill allows you to improve the Production and Gold output of forest and jungle tiles. Gains additional Gold and Production for every two adjacent Lumber Mills, so try to build them in clusters of three if possible.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LUMBERMILL_TEXT';

UPDATE Language_en_US
SET Text = 'It will increase the amount of [ICON_PRODUCTION] Production (Forest) or [ICON_GOLD] Gold (Jungle) provided by this tile, while keeping the Forest/Jungle intact. A Lumber Mill adjacent to two other Lumber Mills gains +1 [ICON_PRODUCTION] Production and [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BUILD_LUMBERMILL_REC';

-- Village
UPDATE Language_en_US
SET Text = 'Village'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_TRADING_POST';

UPDATE Language_en_US
SET Text = 'Villages are smaller settlements scattered around the countryside of a civilization, representative of the people that live off of the land and trade. They can generate a lot of wealth for a society, particularly when placed on Roads, Railroads, or on Trade Routes.[NEWLINE][NEWLINE]Receive [ICON_GOLD] Gold [ICON_PRODUCTION] Production if built on a Road or Railroad that connects two owned Cities.[NEWLINE][NEWLINE]Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 pre-Industrial Era, +2 Industrial Era or later) if a Trade Route, either internal or international, passes over this Village.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADING_POST_TEXT';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_TRADING_POST]Village[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_TRADING_POST';

-- Fort
UPDATE Language_en_US
SET Text = 'A fort is a special improvement that improves the defensive bonus of the tile by 50% for units stationed in that tile. However, forts do not provide a defensive bonus to units in enemy territory. Cannot be built adjacent to one another. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking and a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through it.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FORT_TEXT';

UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking and a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through it.'
WHERE Tag = 'TXT_KEY_BUILD_FORT_HELP';

-- Town
UPDATE Language_en_US
SET Text = 'Town'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_CUSTOMS_HOUSE';

UPDATE Language_en_US
SET Text = 'A town is a human settlement larger than a village but smaller than a city. The size definition for what constitutes a "town" varies considerably in different parts of the world. The word town shares an origin with the German word Zaun, the Dutch word tuin, and the Old Norse tun. The German word Zaun comes closest to the original meaning of the word: a fence of any material.[NEWLINE][NEWLINE]Towns receive +2 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road that connects two owned Cities, and +4 [ICON_GOLD] and [ICON_PRODUCTION] Production if a Railroad. Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+2 for Roads, +4 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CUSTOMS_HOUSE_TEXT';

UPDATE Language_en_US
SET Text = 'Construct a [LINK=IMPROVEMENT_CUSTOMS_HOUSE]Town[\LINK]'
WHERE Tag = 'TXT_KEY_BUILD_CUSTOMS_HOUSE';

-- Citadel
UPDATE Language_en_US
SET Text = 'A Citadel is a mighty fortification that can only be constructed by a Great General, usually near a city or on a particularly defensible piece of terrain. The Acropolis in Athens, Greece, is an example of an early powerful Citadel. Such structures were almost impossible to take by direct attack, and they were capable of withstanding protracted sieges before they fell.[NEWLINE][NEWLINE]You can construct a Citadel anywhere within your territory, or directly adjacent to your territory. Upon constructing the Citadel, your Military Unit Supply Cap will increase by 1, and your Culture borders will also expand to surround the Citadel on all sides by one hex. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result. Any unit stationed within a Citadel receives a 100% defensive strength bonus. Additionally, any enemy unit which ends its turn next to a Citadel takes 30 damage (damage does not stack with other Citadels). Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking and a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through it.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CITADEL_TEXT';

UPDATE Language_en_US
SET Text = 'Can only be built in owned territory or adjacent to your own borders in an unowned tile, and cannot be adjacent to other Citadels. Constructing the Citadel will expand your Culture borders to surround the Citadel, increase your [ICON_WAR] Military Unit Supply Cap by 1, and consume this unit. If the new Culture border claims hexes already owned by another civilization, you will incur a diplomatic penalty as a result.[NEWLINE][NEWLINE]+100% [ICON_STRENGTH] Defensive Strength for any Unit stationed in this tile. Any enemy unit which ends its turn next to this tile takes 30 damage (damage does not stack with other improvements). Contains [COLOR_POSITIVE_TEXT]Fortifications[ENDCOLOR], allowing owned Units to stay stationed in this tile after attacking and a [COLOR_POSITIVE_TEXT]Canal[ENDCOLOR], allowing friendly [COLOR_POSITIVE_TEXT]Sea[ENDCOLOR] Units to traverse through it.'
WHERE Tag = 'TXT_KEY_BUILD_CITADEL_HELP';

-- Landmark
UPDATE Language_en_US
SET Text = 'Landmarks provide +3 Empire-Wide [ICON_HAPPINESS_1] Happiness to the builder and plot owner when constructed, as well as +1 [ICON_CULTURE] Culture and +1 [ICON_GOLD] Gold on the Tile for each additional Era that has passed in comparison to the original Era of the Artifact.[NEWLINE][NEWLINE]A Landmark is any magnificent artifact, structure, work of art, or wonder of nature that draws visitors to a location. Nelson''s Column in London is a landmark, as is Mount Rushmore in the United States. Not every significantly sized object, however, is a landmark: the World''s Largest Ball of Twine may never rise to that stature (though it might well be worth a visit).'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LANDMARK_TEXT';

-- Barbarian Encampment
UPDATE Language_en_US
SET Text = 'Barbarian Encampment'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_ENCAMPMENT';

UPDATE Language_en_US
SET Text = 'A Barbarian Encampment is a temporary settlement by a group of rampaging barbarians intent upon overthrowing and destroying your civilization. These perpetual thorns in the sides of civilizations are capable of spitting out an unending stream of angry barbarians, all who seek to overrun your cities, pillage your lands and stare lewdly at your women. Encampments near to your cities should be destroyed without question. Encampments nearer to competing civilizations may be left to harass your foes, if you are that kind of sneaky leader...'
WHERE Tag = 'TXT_KEY_IMPROVEMENT_BARBARIAN_CAMP_PEDIA';
