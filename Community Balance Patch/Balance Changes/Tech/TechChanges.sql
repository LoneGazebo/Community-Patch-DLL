-- Delete Prereqs
DELETE FROM Technology_PrereqTechs;

-- AI obsoletion logic
UPDATE Resources
SET AIStopTradingEra = 'ERA_FUTURE'
WHERE Type = 'RESOURCE_IRON';

-- Move Wealth to Currency
UPDATE Processes
SET TechPrereq = 'TECH_CURRENCY'
WHERE Type = 'PROCESS_WEALTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Move Research to Education
UPDATE Processes
SET TechPrereq = 'TECH_EDUCATION'
WHERE Type = 'PROCESS_RESEARCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Move Embarkation to Sailing (Help AI) -- Move Rearrange Trade Routes

UPDATE Technologies
SET AllowsEmbarking = '0'
WHERE Type = 'TECH_OPTICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_POTTERY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_ANIMAL_HUSBANDRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_CURRENCY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET AllowsEmbarking = '1'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_HORSEBACK_RIDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_ENGINEERING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_OPTICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_ECONOMICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_BANKING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_PENICILIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_COMPUTERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '0'
WHERE Type = 'TECH_BIOLOGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET InternationalTradeRoutesChange = '1'
WHERE Type = 'TECH_ELECTRICITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- City Embark Less/No Cost at Navigation, Rocketry
UPDATE Technologies
SET CityLessEmbarkCost = '1'
WHERE Type = 'TECH_NAVIGATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET CityNoEmbarkCost = '1'
WHERE Type = 'TECH_ROCKETRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rename Sailing 'Fishing'
UPDATE Technologies
SET IconAtlas = 'COMMUNITY_ATLAS'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET PortraitIndex = '26'
WHERE Type = 'TECH_SAILING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Fishing'
WHERE Tag = 'TXT_KEY_TECH_SAILING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Fishing is the activity of trying to catch fish. Fishing usually takes place in the wild. Techniques for catching fish include hand gathering, spearing, netting, angling and trapping. The term fishing may be applied to catching other aquatic animals such as molluscs, cephalopods, crustaceans, and echinoderms. The term is not normally applied to catching farmed fish, or to aquatic mammals, such as whales, where the term whaling is more appropriate.'
WHERE Tag = 'TXT_KEY_TECH_SAILING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rename Archery 'Military Training' -- Move Barracks there.

UPDATE Language_en_US
SET Text = 'Military Theory'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Military theory is the analysis of normative behavior and trends in military affairs and military history, beyond simply describing events in war, Military theories, especially since the influence of Clausewitz in the nineteenth century, attempt to encapsulate the complex cultural, political and economic relationships between societies and the conflicts they create. Theories and conceptions of warfare have varied in different places throughout human history. The Chinese Sun Tzu is recognized by scholars to be one of the earliest military theorists. His now-iconic Art of War laid the foundations for operational planning, tactics, strategy and logistics. In India, Chanakya (350 – 275 BCE) laid the foundations of military theory through his seminal text called Arthashastra. While the views of Clausewitz, Sun Tzu and Kautilya are not directly applicable to the modern battlefield, they are still referenced and acknowledged by military theorists for the insights they provide, which are then adapted to modern times.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Barracks[ENDCOLOR], an essential building for early war.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rename Horseback Riding 'Trade' -- Move Market there.

UPDATE Language_en_US
SET Text = 'Trade'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Trade involves the transfer of the ownership of goods or services from one person or entity to another in exchange for other goods or services or for money. Possible synonyms of "trade" include "commerce" and "financial transaction". Types of trade include barter. A network that allows trade is called a market. The original form of trade, barter, saw the direct exchange of goods and services for other goods and services. Later one side of the barter started to involve precious metals, which gained symbolic as well as practical importance. Modern traders generally negotiate through a medium of exchange, such as money. As a result, buying can be separated from selling, or earning. The invention of money (and later credit, paper money and non-physical money) greatly simplified and promoted trade. Trade between two traders is called bilateral trade, while trade between more than two traders is called multilateral trade.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Horseman[ENDCOLOR], a fast and powerful mounted unit. Also allows you to build the [COLOR_POSITIVE_TEXT]Market[ENDCOLOR], an excellent source of early [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Rename Optics 'Sailing'
UPDATE Language_en_US
SET Text = 'Sailing'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Technologies
SET PortraitIndex = '13'
WHERE Type = 'TECH_OPTICS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sailing is the art of harnessing the power of the wind to move a boat over the water. To be successful, sailing required the mastery of a number of diverse skills. First, the culture must be able to construct a seaworthy craft. Second, they must be able to construct some sort of sturdy sheets (sails) which can catch the wind and transmit its energy to the hull. Third they must be able to build the various ropes and cleats and pulleys used to control the sails of the ship, and finally they must be able to successfully navigate the vessel from point to point without getting lost or capsizing or suffering some other misfortune.[NEWLINE][NEWLINE]The earliest recorded evidence of watercraft can be found in illustrations in Egypt which date from around 4,000 BC. As a riparian (river-based) civilization, the Egyptians were excellent sailors. Many of their vessels contained both oars and sails, the former being used when the winds were not strong enough or weren not coming from a favorable direction.[NEWLINE][NEWLINE]By 3000 BC the Egyptians were venturing out into the Mediterranean Sea in their vessels, steering the lengthy journey across the open water to Crete and later Phoenicia. The Egyptians also sailed down the coast of Africa, looking for knowledge, trade and treasure.[NEWLINE][NEWLINE]The earliest warships - biremes and triremes and the like - were powered by oar and sail and possessed rams or beaks on their prows. During battle the helmsman would attempt to ram the enemy vessel at high speed, while avoiding enemy attempts to do the same thing. Some vessels were equipped with archers to fire at enemy craft from longer distance, while others had soldiers aboard; these vessels sought to come alongside the enemy craft so that their soldiers could board the other ship and take it by storm.[NEWLINE][NEWLINE]The Greeks - especially the Athenians and the island colonies - were masters at naval warfare. One of the reasons that they were able to defeat their much larger and more powerful neighbor, Persia, was that the Athenian navy dominated the Aegean Sea and thus constantly threatened the increasingly lengthy supply chain of Persia.'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Chemistry Tech

UPDATE Language_en_US
SET Text = 'Allows you to build the Leaning Tower of Pisa, and is an gateway to vital Renaissance-Era techs.'
WHERE Tag = 'TXT_KEY_TECH_CHEMISTRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Metallurgy Tech
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Cannon[ENDCOLOR], a powerful Renaissance-Era siege unit, and the [COLOR_POSITIVE_TEXT]Arsenal[ENDCOLOR], which boosts City defense.'
WHERE Tag = 'TXT_KEY_TECH_METALLURGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Industrialization Text

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Factory[ENDCOLOR], a building which greatly improves the [ICON_PRODUCTION] Production of a city.'
WHERE Tag = 'TXT_KEY_TECH_INDUSTRIALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Steam Power Text

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_COAL] [COLOR_POSITIVE_TEXT]Coal[ENDCOLOR], an essential Industrial-Era resource.'
WHERE Tag = 'TXT_KEY_TECH_STEAM_POWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rifling Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Fusilier[ENDCOLOR], a front-line infantry unit of the mid-game eras.'
WHERE Tag = 'TXT_KEY_TECH_RIFLING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Replaceable Parts Text
UPDATE Language_en_US
SET Text = 'Allows you to build the basic land unit [COLOR_POSITIVE_TEXT]Rifleman[ENDCOLOR], and the [COLOR_POSITIVE_TEXT]Military Base[ENDCOLOR], which boosts City defense.'
WHERE Tag = 'TXT_KEY_TECH_REPLACEABLE_PARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combustion Text

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Landship[ENDCOLOR], an incredibly powerful armored Unit that can move after attacking.'
WHERE Tag = 'TXT_KEY_TECH_COMBUSTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Plastics Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Research Lab[ENDCOLOR], a building which improves [ICON_RESEARCH] Science in a City.'
WHERE Tag = 'TXT_KEY_TECH_PLASTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

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
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Baths[ENDCOLOR], which increase the creation of [ICON_GREAT_PEOPLE] Great People.'
WHERE Tag = 'TXT_KEY_TECH_METAL_CASTING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Construction Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Colosseum[ENDCOLOR], which improves [ICON_HAPPINESS_1] Happiness in the empire, which in turn helps your city growth and makes [ICON_GOLDEN_AGE] Golden Ages more likely.'
WHERE Tag = 'TXT_KEY_TECH_CONSTRUCTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Wheel Text
UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Chariot Archer[ENDCOLOR], a fast and powerful ranged unit which requires [ICON_RES_HORSE] Horses. Also allows Workers to construct [COLOR_POSITIVE_TEXT]Roads[ENDCOLOR], which allow units to move across the map faster and provide extra [ICON_GOLD] Gold when connecting cities to your capital.'
WHERE Tag = 'TXT_KEY_TECH_THE_WHEEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Currency Text

UPDATE Language_en_US
SET Text = 'Allows you to construct the [COLOR_POSITIVE_TEXT]Caravansary[ENDCOLOR], which greatly boosts the range and value of your land [ICON_INTERNATIONAL_TRADE] Trade Routes. Also allows Workers to construct the [COLOR_POSITIVE_TEXT]Village[ENDCOLOR], which increases the [ICON_GOLD] Gold output of map tiles.'
WHERE Tag = 'TXT_KEY_TECH_CURRENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Other Text Fixes
UPDATE Language_en_US
SET Text = 'Allows Workers to construct [COLOR_POSITIVE_TEXT]Pastures[ENDCOLOR] on [ICON_RES_COW] [COLOR_POSITIVE_TEXT]Cows[ENDCOLOR] and [ICON_RES_SHEEP] [COLOR_POSITIVE_TEXT]Sheep[ENDCOLOR]. Also reveals [ICON_RES_HORSE] Horses, which are used to build powerful mounted units.'
WHERE Tag = 'TXT_KEY_TECH_ANIMAL_HUSBANDRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows Workers to construct [COLOR_POSITIVE_TEXT]Mines[ENDCOLOR] to increase the [ICON_PRODUCTION] Production of map tiles.'
WHERE Tag = 'TXT_KEY_TECH_MINING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_IRON] [COLOR_POSITIVE_TEXT]Iron[ENDCOLOR]. Also allows you to build the [COLOR_POSITIVE_TEXT]Spearman[ENDCOLOR], a military unit strong against mounted enemies.'
WHERE Tag = 'TXT_KEY_TECH_BRONZE_WORKING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build [COLOR_POSITIVE_TEXT]Walls[ENDCOLOR], which greatly improve the defense of cities from attack. Also allows Workers to construct Quarries on [ICON_RES_MARBLE] [COLOR_POSITIVE_TEXT]Marble[ENDCOLOR] and [ICON_RES_STONE] [COLOR_POSITIVE_TEXT]Stone[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_MASONRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Horseman[ENDCOLOR], a fast and powerful mounted unit.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Caravan[ENDCOLOR], used to establish lucrative Trade Routes. Also allows your cities to build the [COLOR_POSITIVE_TEXT]Granary[ENDCOLOR], which provides [ICON_FOOD] Food, helping your cities grow larger.'
WHERE Tag = 'TXT_KEY_TECH_POTTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Mint[ENDCOLOR] and [COLOR_POSITIVE_TEXT]East India Company[ENDCOLOR], improving [ICON_GOLD] Gold in your cities.'
WHERE Tag = 'TXT_KEY_TECH_GUILDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows land units to [COLOR_POSITIVE_TEXT]Embark[ENDCOLOR] onto water tiles and travel along the coast. Also allows you to build economic ships, useful for exploration and harvesting resources in the water like [ICON_RES_FISH] [COLOR_POSITIVE_TEXT]Fish[ENDCOLOR] and [ICON_RES_PEARLS] [COLOR_POSITIVE_TEXT]Pearls[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SAILING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Lighthouse[ENDCOLOR] in coastal cities, improving [ICON_FOOD] Food output from water tiles, speeding their growth. Also grants you access to the [COLOR_POSITIVE_TEXT]Cargo Ship[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Observatory[ENDCOLOR], a powerful [ICON_RESEARCH] Science building. Also allows embarked land units to cross ocean tiles.'
WHERE Tag = 'TXT_KEY_TECH_ASTRONOMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Frigate[ENDCOLOR] and the [COLOR_POSITIVE_TEXT]Corvette[ENDCOLOR], powerful Renaissance-era warships. Also allows coastal cities to build the [COLOR_POSITIVE_TEXT]Seaport[ENDCOLOR], which provides [ICON_PRODUCTION] Production from sea resources.'
WHERE Tag = 'TXT_KEY_TECH_NAVIGATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reveals [ICON_RES_ALUMINUM] Aluminum, a resource used for many late-game units. Also allows Cities to build the [COLOR_POSITIVE_TEXT]Stock Exchange[ENDCOLOR], a building which boosts [ICON_GOLD] Gold. Also allows you to build the [COLOR_POSITIVE_TEXT]Hydro Plant[ENDCOLOR], which increases [ICON_PRODUCTION] Production from tiles next to a River.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRICITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Allows you to build the [COLOR_POSITIVE_TEXT]Carrier[ENDCOLOR], a naval unit specialized in supporting a tactic of air supremacy.'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Penicilin and Nanotech 

UPDATE Language_en_US
SET Text = 'All Workers receive the [COLOR_POSITIVE_TEXT]Fallout Reduction[ENDCOLOR] Promotion, which reduces the amount of damage they take while scrubbing Fallout. Allows you to build the [COLOR_POSITIVE_TEXT]Marine[ENDCOLOR], a Unit that specializes in amphibious assaults. Also allows you to build the [COLOR_POSITIVE_TEXT]Medical Lab[ENDCOLOR], a building which speeds the [ICON_FOOD] growth of your Cities.'
WHERE Tag = 'TXT_KEY_TECH_PENICILIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


UPDATE Language_en_US
SET Text = 'All Workers receive the [COLOR_POSITIVE_TEXT]Fallout Immunity[ENDCOLOR] Promotion, which eliminates any damage they would take while scrubbing Fallout. Allows you to build the [COLOR_POSITIVE_TEXT]XCOM Squad[ENDCOLOR], and the [COLOR_POSITIVE_TEXT]SS Stasis Chamber[ENDCOLOR], one of the Spaceship parts necessary to win a [COLOR_POSITIVE_TEXT]Science Victory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NANOTECHNOLOGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Road/Railroad Speed Changes
UPDATE Routes
SET Movement = '50'
WHERE Type = 'ROUTE_ROAD';

UPDATE Routes
SET FlatMovement = '50'
WHERE Type = 'ROUTE_ROAD';

UPDATE Routes
SET Movement = '30'
WHERE Type = 'ROUTE_RAILROAD';

UPDATE Routes
SET FlatMovement = '30'
WHERE Type = 'ROUTE_RAILROAD';