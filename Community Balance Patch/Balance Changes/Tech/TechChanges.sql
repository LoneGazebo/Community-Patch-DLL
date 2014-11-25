-- Delete Prereqs
DELETE FROM Technology_PrereqTechs;

-- Move Wealth to Currency
UPDATE Processes
SET TechPrereq = 'TECH_CURRENCY'
WHERE Type = 'PROCESS_WEALTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Move Research to Education
UPDATE Processes
SET TechPrereq = 'TECH_EDUCATION'
WHERE Type = 'PROCESS_RESEARCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Pottery Explanation Fix

UPDATE Language_en_US
SET Text = 'Allows your cities to build the [COLOR_POSITIVE_TEXT]Granary[ENDCOLOR], which provides [ICON_FOOD] Food, helping your cities grow larger.'
WHERE Tag = 'TXT_KEY_TECH_POTTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Move Embarkation to Sailing (Help AI) -- Move Trade Route

UPDATE Technologies
SET AllowsEmbarking = '0'
WHERE Type = 'TECH_OPTICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_OPTICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Lighthouse[ENDCOLOR] in coastal cities, improving [ICON_FOOD] Food output from water tiles, speeding their growth. Also grants you access to the [COLOR_POSITIVE_TEXT]Cargo Ship[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET AllowsEmbarking = '1'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows land units to [COLOR_POSITIVE_TEXT]Embark[ENDCOLOR] onto water tiles and travel along the coast. Also allows you to build economic ships, useful for exploration and harvesting resources in the water like [ICON_RES_FISH] [COLOR_POSITIVE_TEXT]Fish[ENDCOLOR] and [ICON_RES_PEARLS] [COLOR_POSITIVE_TEXT]Pearls[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SAILING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rename Archery 'Military Training' -- Move Barracks there.

UPDATE Language_en_US
SET Text = 'Military Training'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'One of the earliest examples of military training is the art of archery. Archery is the technology by which a person uses the spring power stored in a bent stick (or "bow") to shoot a slender pointed projectile (an "arrow") a great distance at rapid speed. Throughout history bows and arrows have been employed in the hunt and in war. Their earliest use is lost in the mists of time, but probably dates from the Paleolithic era (which ended some 14,000 years ago) or even earlier (archaeologists have found stone points in Africa dating 60,000 years ago that may have been arrowheads). Virtually every early culture employed bows and arrows, excepting for the Aboriginal Australians, who appear never to have invented them.[NEWLINE][NEWLINE]Archers have been used in war about as long as there has been war. The early Greeks and Egyptians deployed ranks of archers, as did the earliest known armies from India, Asia, Japan, and the Americas. By the Middle Ages the crossbow and firearms began to replace the bow and arrow in warfare and hunting, and by the 16th century it had all but disappeared from most modern armies. Today target archery survives as a sport in many cultures around the world. Some enthusiasts still hunt with the bow and arrow which, despite its great antiquity, can take down a deer as effectively as it did ten thousand years ago.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Industrialization Text

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Factory[ENDCOLOR], a building which greatly improves the [ICON_PRODUCTION] Production of a city.'
WHERE Tag = 'TXT_KEY_TECH_INDUSTRIALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Steam Power Text

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_COAL] [COLOR_POSITIVE_TEXT]Coal[ENDCOLOR] and allows you to build the [COLOR_POSITIVE_TEXT]Ironclad[ENDCOLOR], a powerful warship especially suited to combat in coastal waters.'
WHERE Tag = 'TXT_KEY_TECH_STEAM_POWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rocketry Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Rocket Artillery[ENDCOLOR] military unit, and the [COLOR_POSITIVE_TEXT]Mobile SAM[ENDCOLOR], a fast unit specialized in thwarting enemy aircraft.'
WHERE Tag = 'TXT_KEY_TECH_ROCKETRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Satellites Text
UPDATE Language_en_US
SET Text = 'Allow you to build the [COLOR_POSITIVE_TEXT]Apollo Program[ENDCOLOR], a project necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SATELLITES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Advanced Ballistics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Nuclear Missile[ENDCOLOR], a frightening weapon that requires [ICON_RES_URANIUM] Uranium, and is capable of destroying units and cities.'
WHERE Tag = 'TXT_KEY_TECH_ADVANCED_BALLISTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Globalization Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]SS Cockpit[ENDCOLOR], a component necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR]. With Globalization, each Spy you send to another Civilization as a [ICON_DIPLOMAT] Diplomat will increase the number of Delegates you control in the World Congress, bringing you closer to [COLOR_POSITIVE_TEXT]Diplomatic Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_GLOBALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Computers Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Carrier[ENDCOLOR], a naval unit capable of carrying aircraft.'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Nuclear Fusion Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Giant Death Robot[ENDCOLOR], the strongest unit in the game. Also allows you to build the [COLOR_POSITIVE_TEXT]SS Booster[ENDCOLOR], a component necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NUCLEAR_FUSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Telecom Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Nuclear Submarine[ENDCOLOR], a Naval Unit invisible to most other Units and capable of carrying 2 Missiles.'
WHERE Tag = 'TXT_KEY_TECH_TELECOM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Electronics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Battleship[ENDCOLOR], a powerful late-game ranged Naval Unit.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRONICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combined Arms Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Tank[ENDCOLOR], a fast and deadly armor Unit.'
WHERE Tag = 'TXT_KEY_TECH_COMBINED_ARMS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Refrigeration Text
UPDATE Language_en_US
SET Text = 'Allows you to build [COLOR_POSITIVE_TEXT]Stadiums[ENDCOLOR], which increases [ICON_HAPPINESS_1] Happiness within the City. Also allows Work Boats to construct an Offshore Platform.'
WHERE Tag = 'TXT_KEY_TECH_REFRIGERATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Flight Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Triplane[ENDCOLOR], an Air Unit designed to gain control of the skies, and the [COLOR_POSITIVE_TEXT]Bomber[ENDCOLOR], which can devastate Land Units and Cities.'
WHERE Tag = 'TXT_KEY_TECH_FLIGHT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Radar Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Heavy Bomber[ENDCOLOR], a deadly air Unit capable of delivering death from afar, and the [COLOR_POSITIVE_TEXT]Fighter[ENDCOLOR], an air Unit designed to wrest control of the skies from enemy aircraft. Also allows you to build the [COLOR_POSITIVE_TEXT]Paratrooper[ENDCOLOR], a late-game infantry unit capable of paradropping behind enemy lines.'
WHERE Tag = 'TXT_KEY_TECH_RADAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Compass Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Harbor[ENDCOLOR], which creates city connections from cities to the capital over the water, producing [ICON_GOLD] Gold. Additionally, the [ICON_GOLD] Gold yield from Fishing Boats is increased.'
WHERE Tag = 'TXT_KEY_TECH_COMPASS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Metal Casting Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Workshop[ENDCOLOR], which speeds up the production of buildings.'
WHERE Tag = 'TXT_KEY_TECH_METAL_CASTING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Construction Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Colosseum[ENDCOLOR], which improves [ICON_HAPPINESS_1] Happiness in the empire, which in turn helps your city growth and makes [ICON_GOLDEN_AGE] Golden Ages more likely. Also allows Workers to construct [COLOR_POSITIVE_TEXT]Lumbermills[ENDCOLOR] on forested map tiles, increasing their [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_TECH_CONSTRUCTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
