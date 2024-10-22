UPDATE Language_en_US
SET Text = '[COLOR_GREEN]War Score[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'War Score is ever-shifting status of a war between players. War Score can fluctuate from 100 to -100, where 100 is a total victory for you, and -100 a total victory for your opponent. War Score will also gradually decay over time, to highlight the declining value of past actions in a long, drawn-out conflict.[NEWLINE][NEWLINE]When declared, War Score starts at zero for both players. As you (or your opponent) destroy units, pillage tiles/trade units, capture civilians and take cities, your warscore will go up. The value of these actions varies based on the overall size of your opponent.[NEWLINE][NEWLINE]When it comes time to make peace, the War Score value gives you a good idea of what you should expect to gain from your opponent, or what they will ask of you. In the trade screen, the War Score value will be translated into a ''Max Peace'' value, which shows you exactly what you can take from your opponent (or vice-versa). When peace is concluded, the War Score returns to zero.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Combat Units in Cities[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Only one land and one naval unit may occupy a city at a time. A military unit in a city is said to "Garrison" the city, and it adds a significant defensive bonus to the city [COLOR_YELLOW]if it''s a land unit. If a city is attacked while a Garrison is in the city, the Garrison will deflect some of the damage onto itself, thus offering the city even more protection. Be careful, however, as a Garrison can be destroyed this way.[ENDCOLOR][NEWLINE][NEWLINE]Additional combat units may move through the city, but they cannot end their turn there. (So if you build a combat unit in a city with a garrison, you have to move one of the two units out before you end your turn.)[NEWLINE][NEWLINE][COLOR_YELLOW]Note also that naval units cannot perform any attacks while they are stationed in a city.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Garrison Units in Cities[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'A city''s owner may "garrison" a military unit inside the city to bolster its defenses. A portion of the garrisoned unit''s combat strength is added to the city''s strength. [COLOR_YELLOW]The garrisoned will divert part of the damage to a city when the city is attacked. This can destroy the garrison, so be careful![ENDCOLOR] If the city is captured, the garrisoned unit is destroyed.[NEWLINE][NEWLINE]'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Forts[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_FORT_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'Once a civ has acquired the Engineering technology, workers can construct "forts" in friendly or neutral territory. Forts provide a hefty defensive bonus to units occupying them. Forts cannot be constructed in enemy territory. They can be constructed atop resources. [COLOR_YELLOW]Melee Units attacking from a Fort don''t leave the Fort even if they destroy the attacked enemy unit.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_FORT_HEADING3_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Siege Weapons[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_SEIGEWEAPONS_HEADING2_TITLE';
UPDATE Language_en_US
SET Text = 'Certain ranged weapons are classified as "siege weapons" - catapults, ballistae, trebuchets, and so forth. These units get combat bonuses when attacking enemy cities. They are extremely vulnerable to melee combat, and should be accompanied by melee units to fend off enemy assault.[NEWLINE][NEWLINE][COLOR_YELLOW]In Vox Populi and the Community Patch, siege units don''t have to be "set up" anymore. Instead, they move at half-speed in enemy territory.[ENDCOLOR][NEWLINE][NEWLINE]Siege weapons are important. It''s really difficult to capture a well-defended city without them!'
WHERE Tag = 'TXT_KEY_COMBAT_SEIGEWEAPONS_HEADING2_BODY';

UPDATE Language_en_US
SET Text = '[COLOR_YELLOW]Melee Combat Results[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_TITLE';
UPDATE Language_en_US
SET Text = 'At the end of melee combat, one or both units may have sustained damage and lost "hit points." If a unit''s hit points are reduced to 0, that unit is destroyed. If after melee combat the defending unit has been destroyed and the attacker survives, the attacking unit moves into the defender''s hex [COLOR_YELLOW]unless defending a Citadel, Fort, or City, at which point the melee unit remains in place[ENDCOLOR]. If it moves, the winner will capture any non-military units in that hex. If the defending unit survives, it retains possession of its hex and any other units in the hex.[NEWLINE][NEWLINE]Most units use up all of their movement when attacking. Some however have the ability to move after combat - if they survive the battle and have movement points left to expend.[NEWLINE][NEWLINE]Any surviving units involved in the combat will receive "experience points" (XPs), which may be expended to give the unit promotions.'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_BODY';

-- Map
UPDATE Language_en_US
SET Text = 'The ruin provides a map of the nearest unrevealed City (lifting the fog of war from a number of tiles).'
WHERE Tag = 'TXT_KEY_BARBARIAN_MAP_HEADING4_BODY';

-- Liberating a City-State
UPDATE Language_en_US
SET Text = 'If another civ has captured a City-State and you capture it from them, you have the option to "liberate" that city-state. If you do so, you will receive a large amount of [ICON_INFLUENCE] Influence from the City-State, usually enough to make you [COLOR_POSITIVE_TEXT]Allies[ENDCOLOR] with it.'
WHERE Tag = 'TXT_KEY_CITYSTATE_LIBERATING_HEADING2_BODY';

-- National Units
UPDATE Language_en_US
SET Text = 'Each civilization in Civilization V has one or more special "national units." These units are unique to that civilization, and they are in some way superior to the standard version of that unit. The American civilization, for example, has a Minuteman unit, which is superior to the standard Musketman available to other civs. The Greek civ has the Hoplite unit, which replaces the Spearman.[NEWLINE][NEWLINE]See each civilization''s Civilopedia entry to discover its special unit.'
WHERE Tag = 'TXT_KEY_UNITS_NATIONAL_HEADING2_BODY';

-- Great Generals
UPDATE Language_en_US
SET Text = 'Great Generals are "Great People" skilled in the art of warfare. They provide combat bonuses - offensive and defensive bonuses both - to any friendly units within two tiles of their location. A Great General itself is a non-combat unit, so it may be stacked with a combat unit for protection. If an enemy unit ever enters the tile containing a Great General, the General is destroyed.[NEWLINE][NEWLINE]A Great General gives a combat bonus of 15% to units in the General''s tile and all friendly units within 2 tiles of the General.[NEWLINE][NEWLINE]Great Generals are created when your units have been in battle and also can be acquired from buildings, policies, beliefs, and tenets. See the section on "Great People" for more details.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

-- Defensive Pact
UPDATE Language_en_US
SET Text = 'Once you have acquired the Chivalry tech, you may engage in a Defensive Pact. Defensive Pacts are always mutual. If a signatory to a Defensive Pact is attacked, the other partner is automatically at war with the attacker.[NEWLINE][NEWLINE]A Defensive Pact lasts for 50 turns (on standard speed). When that time has elapsed, the pact lapses unless it is renegotiated.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_DEFENSIVEPACT_HEADING3_BODY';
