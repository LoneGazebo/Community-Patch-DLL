-- Insert SQL Rules Here 

UPDATE Language_en_US
SET Text = 'The Great General combat bonus is increased by 15%, and their spawn rate is increased by 50%. +1 [ICON_FOOD] Food from Farms.'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_HAPPINESS_4] Unhappiness from number of Cities doubled, [ICON_HAPPINESS_4] Unhappiness from number of [ICON_CITIZEN] Citizens halved. +50% border expansion rate, and cities may work one additional ring of tiles.'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+2 [ICON_MOVES] Movement for all naval units. Receives 1 extra Spy. All ranged units receive the Volley promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Units fight as though they were at full strength even when damaged, and deal +25% damage when attacking. +1 [ICON_CULTURE] Culture from each Fishing Boat and +2 [ICON_CULTURE] Culture from each Atoll.'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All siege units deal +50% to Cities, and receive the Cover I promotion, whereas all melee naval units have the Prize Ships promotion. Pay only one-third the usual cost for naval unit maintenance.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Masters of the Siege'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Upon defeating a Barbarian unit inside an encampment, there is a 67% chance you earn 25 [ICON_GOLD] Gold and they join your side. Pay 25% less for land unit maintenance. +1 [ICON_PRODUCTION] Production from Mines.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Museum and World Wonder theming bonuses are doubled in their capital. Writer, Musician, and Artist Specialists produce +3 [ICON_PRODUCTION] Production and +3 [ICON_RESEARCH] Science.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The first 3 cities founded on continents other than where Indonesia started each provide 2 unique Luxury Resources. +2 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold from Plantations.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tourism [ICON_TOURISM] output is +100% during their Golden Ages. Earn Great Artists, Musicians, and Writers 50% faster during their Golden Ages. +1 [ICON_FOOD] Food and +1 [ICON_PRODUCTION] Production from unimproved Jungles.'
WHERE Tag = 'TXT_KEY_TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive triple [ICON_GOLD] Gold from Barbarian encampments and pillaging Cities. Land units gain the War Canoe and Amphibious promotions. +4 [ICON_RESEARCH] Science from each [ICON_INTERNATIONAL_TRADE] Trade Route with a different civilization.'
WHERE Tag = 'TXT_KEY_TRAIT_AMPHIB_WARLORD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'All land military units have +1 sight. 50% discount when purchasing tiles. +15% [ICON_GREAT_PEOPLE] Great Person generation.'
WHERE Tag = 'TXT_KEY_TRAIT_RIVER_EXPANSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_PRODUCTION] Production towards any buildings that already exist in the [ICON_CAPITAL] Capital. +3 [ICON_GOLD] Gold for each owned City connected to your [ICON_CAPITAL] Capital.'
WHERE Tag = 'TXT_KEY_TRAIT_CAPITAL_BUILDINGS_CHEAPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Units move through Forest and Jungle in friendly territory as if they are Roads. These tiles can be used to establish [ICON_CONNECTED] Trade Routes. All military units receive the Woodsman and Secret Trails promotions.'
WHERE Tag = 'TXT_KEY_TRAIT_IGNORE_TERRAIN_IN_FOREST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Choose one more Belief than normal when you found a Religion. Receive a free Shrine in your Capital at Pottery, and two free Temples in your first two cities at Theology.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_BELIEF' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Retains 50% of the [ICON_HAPPINESS_1] Happiness benefits from a Luxury Resource if your last copy of it is traded away. Naval units receive the Supply promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_LUXURY_RETENTION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith per city with an adjacent unimproved Forest. Bonus increases to +2 [ICON_PEACE] Faith in Cities with 3 or more adjacent unimproved Forest tiles. +1 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from Camps.'
WHERE Tag = 'TXT_KEY_TRAIT_FAITH_FROM_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Raze Cities at double speed. Borrow City names from other in-game Civs. Start with Animal Husbandry technology. +1 [ICON_PRODUCTION] Production, +1 [ICON_CULTURE] Culture, and +1 [ICON_PEACE] Faith per Pasture.'
WHERE Tag = 'TXT_KEY_TRAIT_RAZE_AND_HORSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receive 50% more [ICON_INTERNATIONAL_TRADE] Trade Routes than normal. All coastal Cities get a free Harbor. Units may cross mountains after the first Great General is earned, taking 50 HP damage if they end a turn on a mountain.'
WHERE Tag = 'TXT_KEY_TRAIT_PHOENICIAN_HERITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_STRENGTH] Combat Strength +30% when fighting City-State units or attacking a City-State itself. All mounted units have +1 [ICON_MOVES] Movement, can move after attacking, and receive the March promotion.'
WHERE Tag = 'TXT_KEY_TRAIT_TERROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Embarked units have +1 Movement [ICON_MOVES] and pay just 1 movement point to move from sea to land. Melee units pay no movement point cost to pillage. +15% Combat Bonus when fighting within 3 tiles of Fishing Boats.'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'When a city is conquered, gain a free Technology already discovered by its owner.  Gaining a city through a trade deal does not count, and it can only happen once per enemy city. +1 [ICON_GOLD] Gold from Strategic Resources.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] from City-States and Civilization Traits.'
WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_MINORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

