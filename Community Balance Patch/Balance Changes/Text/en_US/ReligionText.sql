-- PANTHEONS

-- Goddess of the Hunt
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_CULTURE] Culture from Camps, +1 [ICON_FOOD] Food from Resources on Tundra'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Fertility Rites (Now Goddess of Fertility)
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and [ICON_PEACE] Faith from Shrines, and +8 [ICON_FOOD] Food and [ICON_PEACE] Faith when a Building is constructed, scaling with Era. 25% faster [ICON_FOOD] Growth Rate'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of the Home'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Craftsmen (DELETED)

-- God of the Sea
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Fishing Boats and Atolls. +2 [ICON_FOOD] Food and +1 [ICON_PEACE] Faith in coastal Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Open Sky
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith in City per 2 worked Plains or 2 worked Grassland tiles without Hills or Features. +1 [ICON_PEACE] Faith and +3 [ICON_GOLD] Gold from Pastures'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messenger of the Gods (Now God of Commerce)
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and [ICON_GOLD] Gold in Cities with a [ICON_CONNECTED] City Connection and per active Trade Route to or from the City. Capital/Holy City gains +2 [ICON_PEACE] Faith and [ICON_GOLD] Gold after your second City is founded, and +2 [ICON_GREAT_PEOPLE] Great Merchant Points.'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Commerce'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- One With Nature (Now Goddess of Nature)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_FOOD] Food for every 2 Mountains within 3 tiles of a City (capping at the City''s population). Natural Wonders gain +3 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Nature'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgy (Now Goddess of Wisdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science, +1 [ICON_PEACE] Faith in every City, and +2 [ICON_GREAT_PEOPLE] Great Scientist Points in Capital/Holy City. +2 [ICON_PEACE] Faith and [ICON_RESEARCH] Science in any City with a Specialist'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Wisdom'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Stone Circles (Now God of Craftsmen)
UPDATE Language_en_US

SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Quarries and Stone Works. +1 [ICON_CULTURE] Culture, +1 [ICON_RESEARCH] Science, and +2 [ICON_PEACE] Faith from the Palace'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Craftsmen'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of War
UPDATE Language_en_US
SET Text = '+10% increase in City [ICON_RANGE_STRENGTH] Ranged Combat Strength, and +10% [ICON_PRODUCTION] Production towards Units. Gain [ICON_PEACE] Faith from killing military units'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Waters (Now Goddess of Purity)
UPDATE Language_en_US
SET Text = 'Goddess of Purity'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_FOOD] Food from Lakes, +1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_PRODUCTION] Production from Marshes. +1 [ICON_HAPPINESS_1] Happiness from Cities on rivers'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Love
UPDATE Language_en_US
SET Text = '+3 [ICON_PRODUCTION] Production, +2 [ICON_GOLD] Gold, and +1 [ICON_PEACE] Faith in Cities with at least 3 [ICON_CITIZEN] Citizens. +1 [ICON_PRODUCTION] Production and [ICON_PEACE] Faith from Engineer Specialists' 
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tutelary Gods'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements (Now God of the Expanse)
UPDATE Language_en_US
SET Text = '+25% faster border growth, and gain 20 [ICON_PEACE] Faith and 10 [ICON_PRODUCTION] Production every time a City expands its borders naturally, scaling with Gamespeed'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements 
UPDATE Language_en_US
SET Text = 'God of the Expanse'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Festivals
UPDATE Language_en_US
SET Text = '+3 [ICON_GOLD] Gold, +1 [ICON_CULTURE] Culture, and +1 [ICON_PEACE] Faith for every unique Luxury Resource owned or imported, and +1 [ICON_PEACE] Faith in the [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Oral Tradition (Now Goddess of Springtime)
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold, +1 [ICON_FOOD] Food, and [ICON_PEACE] Faith from Plantations. +1 [ICON_RESEARCH] Science and +2 [ICON_PEACE] Faith from Herbalists'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Springtime'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ancestor Worship
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 3 [ICON_CITIZEN] Citizens in a City. +1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith from Councils'
WHERE Tag = 'TXT_KEY_BELIEF_ANCESTOR_WORSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Desert Folklore (Now Spirit of the Desert)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_PRODUCTION] Production, and [ICON_GOLD] Gold from Desert tiles with resources, and +3 [ICON_FOOD] Food from Oases'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Spirit of the Desert'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Path (Now Goddess of Renewal)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_RESEARCH] Science for every 2 Jungle or 2 Forest tiles worked by a City. +2 [ICON_FOOD] Food and [ICON_RESEARCH] Science from Markets'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Renewal'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Protection
UPDATE Language_en_US
SET Text = '+10 HP healed per turn in friendly territory. +2 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from Palace, Walls, and Barracks'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monument to the Gods (Now Goddess of Beauty)
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from Palace and World Wonders, +1 [ICON_PEACE] Faith from [ICON_GREAT_WORK] Great Works. +2 [ICON_GREAT_ARTIST] Great Artist and [ICON_GREAT_ENGINEER] Great Engineer Points in your [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Beauty'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Dance of the Aurora (Now God of the Stars and Sky)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_GOLD] Gold from Tundra tiles with resources'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of the Stars and Sky'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

--  Tears of the Gods (Now God of All Creation)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Monuments and +1 [ICON_CULTURE] in [ICON_CAPITAL] Capital. +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital for every two Pantheons ever founded (caps at 8 Pantheons total)'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of All Creation'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Earth Mother
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture from Mines on improved resources, +1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Monuments'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God-King
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith in [ICON_CAPITAL] Capital. +1 [ICON_CULTURE] Culture, [ICON_GOLDEN_AGE] Golden Age Points, [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital for every 5 Followers of your Pantheon in owned Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sun God
UPDATE Language_en_US
SET Text = '+3 [ICON_PEACE] Faith and +2 [ICON_GOLD] Gold from Granaries. +3 [ICON_FOOD] Food from Farms on [ICON_RES_WHEAT] Wheat'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of the Sun'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Charitable Missions (Now Missionary Influence)
UPDATE Language_en_US
SET Text = '+10 [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, [ICON_GOLD] Gold, [ICON_PEACE] Faith, and [ICON_GOLDEN_AGE] Golden Age Points per turn while the Host of the World Congress. Receive 150 of these yields instantly when you pass a Proposal. Bonuses scale with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Global Commandments'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Evangelism (Now Crusader Zeal)
UPDATE Language_en_US
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in enemy lands, and an additional +10% versus Land Units of players that do not follow your Religion. Receive [ICON_CULTURE] Culture and [ICON_GOLD] Gold when you conquer Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Crusader Spirit'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Defender of the Faith
UPDATE Language_en_US
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in own lands, and an additional +10% versus Land Units of players that do not follow your Religion. +1 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture from all Defensive Buildings.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Jesuit Education 
UPDATE Language_en_US
SET Text = 'May build Universities, Public Schools, and Research Labs with [ICON_PEACE] Faith, and each building gains +2 [ICON_RESEARCH] Science. Holy City gains 20 [ICON_PEACE] Faith each time the owner expends a [ICON_GREAT_PEOPLE] Great Person, scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_JESUIT_EDUCATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Divine Teachings'
WHERE Tag = 'TXT_KEY_BELIEF_JESUIT_EDUCATION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Fervor (Now The One True Faith)
UPDATE Language_en_US
SET Text = 'Receive 1 additional [ICON_DIPLOMAT] Delegate in the World Congress for every 2 [ICON_RELIGION] Holy Sites or 2 [ICON_TOURISM] Landmarks you own. +50% Yields from Friendly/Allied [ICON_CITY_STATE] City-States following this Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy Land'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Sites
UPDATE Language_en_US
SET Text = 'Hotels and all buildings purchased with Faith provide +3 [ICON_TOURISM] Tourism each. World and Natural Wonders gain +4 [ICON_TOURISM] Tourism each. Hermitage provides +10 [ICON_CULTURE] Culture and +10 [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Glory of God
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase any type of [ICON_GREAT_PEOPLE] Great Person (in Industrial Era). When you expend a [ICON_GREAT_PEOPLE] Great Person, gain 3 [ICON_GOLD] Gold, [ICON_RESEARCH] Science, and [ICON_CULTURE] Culture per City following your Religion (max 20 Cities).'
WHERE Tag = 'TXT_KEY_BELIEF_TO_GLORY_OF_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Underground Sect (Now Faith of the  Masses)
UPDATE Language_en_US
SET Text = 'May build Amphitheaters, Opera Houses, Museums, Broadcast Towers, and Stadiums with [ICON_PEACE] Faith. These buildings produce +2 [ICON_CULTURE] Culture each. Holy City owner gains +1 [ICON_HAPPINESS_1] Happiness for every two Cities following this Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Faith of the Masses'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Unity of the Prophets (Now Knowledge Through Faith)
UPDATE Language_en_US
SET Text = 'Landmarks and Great Person Improvements produce +4 [ICON_PEACE] Faith and [ICON_RESEARCH] Science. [ICON_GREAT_WORK] Great Works produce +2 [ICON_CULTURE] Culture, and you can purchase Archaeologists with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Inspired Works'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Buildings
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Cathedrals belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Poverty and boosts the [ICON_GOLD] Gold yield of nearby farms and pastures. Gain [ICON_GOLD] Gold in the City when its borders expand.'
WHERE Tag = 'TXT_KEY_BUILDING_CATHEDRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Pagodas belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Boredom and [ICON_HAPPINESS_3] Religious Unrest, and generates yields based on the number of Religions present in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_PAGODA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Mosques belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Illiteracy and generates [ICON_CULTURE] Culture during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Texts (Now Hymns)
UPDATE Language_en_US
SET Text = '[ICON_INQUISITOR] Inquisitors cost 25% less [ICON_PEACE] Faith, and generate 25 [ICON_GOLD] Gold per converted [ICON_CITIZEN] Citizen when Removing Heresy. Removing Heresy no longer causes [ICON_RESISTANCE] Resistance in Cities. Your [ICON_SPY] Spies exert +50 Religious Pressure on the Cities they occupy (Standard Speed), and +2 [ICON_HAPPINESS_1] Happiness if stationed in a foreign City.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Inquisition'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Reliquary (Now Tithes)
UPDATE Language_en_US
SET Text = '+2 [ICON_CULTURE] Culture and [ICON_PEACE] Faith in all owned Cities following this [ICON_RELIGION] Religion. +1 [ICON_PEACE] Faith in a City for every 10 [ICON_GOLD] Gold per Turn it produces, capped at half the number of Followers in the City.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mendicancy'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Just War (Now Martyrdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold and [ICON_PEACE] Faith in Holy City for every 10 followers of this [ICON_RELIGION] Religion in Foreign Cities. Minimum [ICON_INFLUENCE] Influence with City-States following your Religion is [COLOR_POSITIVE_TEXT]35[ENDCOLOR], and you receive +20% rewards from City-State Quests.'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Abode of Peace'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Heathen Conversion (Now Holy Warriors)
UPDATE Language_en_US
SET Text = 'May spend [ICON_PEACE] Faith to purchase land units in Cities. Strategic Resource quantities increase by 1% per following City (up to 25%).'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Zealotry'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Itinerant Preachers (Now Iconography)
UPDATE Language_en_US
SET Text = 'Holy City gains +5 [ICON_GOLDEN_AGE] Golden Age Points and +2 [ICON_GREAT_PEOPLE] Great Person Points per turn for all [ICON_GREAT_PEOPLE] Great People. [ICON_MISSIONARY] Missionaries of this Religion erode existing pressure from other religions by 15% when Spreading Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Symbolism'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messiah (Now Sainthood)
UPDATE Language_en_US
SET Text = 'Reduce minimum Policy requirement for Wonders by 1. Prophets of this [ICON_RELIGION] Religion 25% stronger, cost 25% less [ICON_PEACE] Faith. +3 to base Holy Site yields. If this is the majority Religion, Follower reduction from rival [ICON_INQUISITOR] Inquisitors and [ICON_PROPHET] Prophets halved.'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Prophecy'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Missionary Zeal (Now Scholar-Priests)
UPDATE Language_en_US
SET Text = '+15% [ICON_GREAT_PEOPLE] Great Person Rate in Holy City during [ICON_GOLDEN_AGE] Golden Ages. +2 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_GOLD] Gold in [ICON_RELIGION] Holy City for every Foreign City following this Religion. Missionaries of this Religion 25% stronger.'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sacred Calendar'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Order
UPDATE Language_en_US
SET Text = '[ICON_RELIGION] Religion spreads 15% faster (30% with Printing Press), and to Cities 25% further away. Pressure doubled to friendly [ICON_CITY_STATE] City-States and foreign Cities connected via [ICON_INTERNATIONAL_TRADE] Trade Routes.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Orthodoxy'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Unity
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_PRODUCTION] Production in Holy City for every 2 followers of other Religions in owned Cities. +1 [ICON_PEACE] Faith in a City for every 15 [ICON_RESEARCH] Science per turn it produces, capped at half the number of Followers in the City.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Syncretism'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Cathedral
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Cathedrals'
WHERE Tag = 'TXT_KEY_BELIEF_CATHEDRALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pagoda
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Pagodas'
WHERE Tag = 'TXT_KEY_BELIEF_PAGODAS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Mosque
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mosques'
WHERE Tag = 'TXT_KEY_BELIEF_MOSQUES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Swords into Plowshares (Now Church)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Churches'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Churches'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Guruship (Now Mandir)
UPDATE Language_en_US
SET Text = 'Mandirs'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mandirs'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Warriors (Now Synagogue)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Synagogues'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Synagogues'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monasteries (Now Stupa)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Stupas'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Stupas'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Feed the World (Now Thrift)
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every follower in the City (max +10 [ICON_GOLD] Gold)'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Thrift'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Gardens (Now Cooperation)
UPDATE Language_en_US
SET Text = 'Receive +5 to all City yields every time a [ICON_CITIZEN] Citizen is born in the City. Bonus scales with Era'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cooperation'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Choral Music (Now Scholarship)
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 2 followers in the City (max +15 [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Scholarship'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Art (Now Mastery)
UPDATE Language_en_US
SET Text = 'Specialists generate +1 [ICON_GOLDEN_AGE] Golden Age Point and +1 of their primary Yield ([ICON_RESEARCH]/[ICON_GOLD]/[ICON_PRODUCTION]/[ICON_CULTURE]).'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mastery'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgical Drama (Now Veneration)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_RESEARCH] Science, [ICON_PRODUCTION] Production, and [ICON_FOOD] Food in the City. 10% of the cost of [ICON_PEACE] Faith Purchases in this City is converted into [ICON_CULTURE] Culture and [ICON_GOLD] Gold.'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Veneration'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ascetism
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every follower in the City (max +15 [ICON_FOOD] Food)'
WHERE Tag = 'TXT_KEY_BELIEF_ASCETISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Center (Now Order)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Community (Now Diligence)
UPDATE Language_en_US
SET Text = 'Diligence'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 2 followers in the City (max +15 [ICON_PRODUCTION] Production)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Divine Inspiration (Now Inspiration)
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 3 followers in the City (max +6 [ICON_CULTURE] Culture), and +2 [ICON_PEACE] Faith if you have at least one Specialist in the City'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Creativity'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Loving (Now Mandate of Heaven)
UPDATE Language_en_US
SET Text = 'Unlocks the [COLOR_POSITIVE_TEXT]Celestial Throne National Wonder[ENDCOLOR] (+2 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production; +5 [ICON_PEACE] Faith from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). Holy City produces +20% of its Yields during a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Divine Inheritance'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Interfaith Dialogue (Now Hero Worship)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Great Altar National Wonder[ENDCOLOR] (+5 [ICON_PEACE] Faith, +15% Military Unit [ICON_PRODUCTION] Production; +5 [ICON_PRODUCTION] Production from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). Receive [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points when you conquer a City, as well as [ICON_GREAT_GENERAL] Great General Points (if City is landlocked) or [ICON_GREAT_ADMIRAL] Great Admiral Points (if City is coastal). Bonus scales with City population and Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hero Worship'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ceremonial Burial
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Mausoleum National Wonder[ENDCOLOR] (+5 [ICON_PEACE] Faith, and gain [ICON_PEACE] Faith when an owned unit is killed in battle, +5 [ICON_PEACE] Faith from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). When a [ICON_GREAT_PEOPLE] Great Person is expended, gain 15 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every City following your [ICON_RELIGION] Religion (max 25 Cities).'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Church Property (Now Holy Law)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Divine Court National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +6 [ICON_GOLD] Gold; +5 [ICON_GOLD] Gold from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). When you unlock a Policy, gain 5 [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold for every Follower of your [ICON_RELIGION] Religion (max 250 Followers).'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy Law'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Tithe (Now Way of the Pilgrim)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Reliquary National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +2 [ICON_CULTURE] Culture, 4 [ICON_GREAT_WORK] Art/Artfiact slots; +5 [ICON_TOURISM] Tourism from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). Receive 15 [ICON_TOURISM] Tourism and [ICON_CULTURE] Culture when you spread your [ICON_RELIGION] Religion to foreign Cities, scaling with the number of Followers of other [ICON_RELIGION] Religions.'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of the Pilgrim'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Initiation Rites (Now Way of Transcendence)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Sacred Garden National Wonder[ENDCOLOR] (+3 [ICON_PEACE] Faith, +5 [ICON_CULTURE] Culture; +5 [ICON_FOOD] Food from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). When you enter a new Era, Holy City gains 20 of every Yield for each City following your [ICON_RELIGION] Religion (max 25 Cities), scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of Transcendence'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Papal Primacy (Now Council of Elders)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Holy Council National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +5 [ICON_FOOD] Food; +5 [ICON_RESEARCH] Science from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). When a City adopts your [ICON_RELIGION] Religion for the first time, gain 20 [ICON_RESEARCH] Science and [ICON_PRODUCTION] Production in your Holy City, scaling gradually based on the number of Cities following your [ICON_RELIGION] Religion (bonus caps at 15 Cities).'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Council of Elders'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pilgrimage (Now Apostolic Tradition)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Apostolic Palace National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +4 [ICON_GOLDEN_AGE] Golden Age Points; +5 [ICON_GOLDEN_AGE] Golden Age Points from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). When you increase the number of Followers in a City by spreading [ICON_RELIGION] Religion, receive 25 [ICON_CULTURE] Culture and [ICON_FOOD] Food in the Holy City, scaling with the number of new Followers of your [ICON_RELIGION] Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Apostolic Tradition'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- World Church (Now Theocratic Rule)
UPDATE Language_en_US
SET Text = 'Unlocks [COLOR_POSITIVE_TEXT]Grand Ossuary National Wonder[ENDCOLOR] (+10 [ICON_PEACE] Faith; +5 [ICON_CULTURE] Culture from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]). "We Love the King Day" boosts the [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_GOLD] Gold output of a City by 15%.'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Theocratic Rule'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Belief Tooltips for Building beliefs

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CATHEDRAL_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+2 [ICON_GOLD]Gold[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Art Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CATHEDRAL_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_PAGODA_TOOLTIP', '1 [ICON_GREAT_WORK] Great Work of Art Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_PAGODA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MOSQUE_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE]+2 [ICON_RESEARCH]Science[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Literature Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MOSQUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_STUPA_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_STUPA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_SYNAGOGUE_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+3 [ICON_PRODUCTION] Production[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Literature Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_SYNAGOGUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MANDIR_TOOLTIP', '+3 [ICON_PEACE] Faith, +2 [ICON_FOOD] Food[NEWLINE]+10% [ICON_FOOD] Food[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MANDIR_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CHURCH_TOOLTIP', '+4 [ICON_PEACE] Faith, +2 [ICON_CULTURE] Culture[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CHURCH_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_ORDER_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_ORDER_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
