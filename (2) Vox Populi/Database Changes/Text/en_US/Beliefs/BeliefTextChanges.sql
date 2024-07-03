----------------------------------------------
-- Pantheons
----------------------------------------------

-- Ancestor Worship
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 4 [ICON_CITIZEN] Citizens in the City. +2 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from Councils.'
WHERE Tag = 'TXT_KEY_BELIEF_ANCESTOR_WORSHIP';

UPDATE Language_en_US
SET Text = 'God of Fire'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_PRODUCTION] Production, and [ICON_CULTURE] Culture from Mines on improved Resources. +2 [ICON_PEACE] Faith from Forges.'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER';

UPDATE Language_en_US
SET Text = 'God of All Creation'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Monuments and +1 [ICON_CULTURE] from the Palace. +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science in your [ICON_CAPITAL] Capital/Holy City for every two Pantheons ever founded (caps at 8 Pantheons total).'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS';

UPDATE Language_en_US
SET Text = 'God of Commerce'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and [ICON_GOLD] Gold in Cities with a [ICON_CONNECTED] City Connection and per active Trade Route to or from the City. Your [ICON_CAPITAL] Capital/Holy City gains +2 [ICON_GREAT_MERCHANT] Great Merchant Points, and an additional +2 [ICON_PEACE] Faith and [ICON_GOLD] Gold after your second City is founded.'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS';

UPDATE Language_en_US
SET Text = 'God of Craftsmen'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Quarries. +2 [ICON_PEACE] Faith and [ICON_RESEARCH] Science from the Palace. +2 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from Stone Works.'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES';

UPDATE Language_en_US
SET Text = 'God of the Expanse'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS_SHORT';

UPDATE Language_en_US
SET Text = '+25% [ICON_CULTURE_LOCAL] Border Growth. Gain 25 [ICON_PEACE] Faith and 15 [ICON_PRODUCTION] Production every time the City expands its borders naturally.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS';

-- God of the Open Sky
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture in the City for every 2 Plains or 2 Grassland tiles without Hills or Features worked by the City. +1 [ICON_PEACE] Faith and +3 [ICON_GOLD] Gold from Pastures.'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY';

-- God of the Sea
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Fishing Boats and Atolls. +2 [ICON_FOOD] Food and +1 [ICON_PEACE] Faith if the City is [COLOR_POSITIVE]coastal[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA';

UPDATE Language_en_US
SET Text = 'God of the Stars and Sky'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_CULTURE] Culture from Tundra and Snow tiles with Resources.'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA';

UPDATE Language_en_US
SET Text = 'God of the Sun'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and [ICON_GOLD] Gold from Granaries. +1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_RESEARCH] Science from Farms on Resources.'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD';

-- God of War
UPDATE Language_en_US
SET Text = 'Gain [ICON_PEACE] Faith from killing enemy units equal to 175% of its [ICON_STRENGTH] Strength. +2 [ICON_PEACE] Faith and +1 [ICON_PRODUCTION] Production from Barracks.'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_WAR';

-- God-King
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from the Palace. +1 [ICON_PEACE] Faith, [ICON_GOLD] Gold, [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_GOLDEN_AGE] Golden Age Points in your [ICON_CAPITAL] Capital/Holy City for every 5 Followers of your Pantheon in owned Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING';

UPDATE Language_en_US
SET Text = 'Goddess of Beauty'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from the Palace and World Wonders, +1 [ICON_PEACE] Faith from [ICON_GREAT_WORK] Great Works. +2 [ICON_GREAT_ARTIST] Great Artist Points and [ICON_GREAT_ENGINEER] Great Engineer Points in your [ICON_CAPITAL] Capital/Holy City.'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS';

-- Goddess of Festivals
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from the Palace. +1 [ICON_PEACE] Faith, +3 [ICON_GOLD] Gold, and +1 [ICON_CULTURE] Culture in your [ICON_CAPITAL] Capital/Holy City for every unique Luxury Resource owned or imported.'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS';

UPDATE Language_en_US
SET Text = 'Goddess of Nature'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_GOLD] Gold for every 2 Mountains within 3 tiles of the City (capping at the City''s population). Natural Wonders gain +3 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE';

-- Goddess of Protection
UPDATE Language_en_US
SET Text = '+10 HP healed per turn in friendly territory. +3 [ICON_PEACE] Faith from the Palace. +2 [ICON_PEACE] Faith and [ICON_CULTURE] Culture from Walls.'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Goddess of Purity'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_FOOD] Food from Lakes. +1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_PRODUCTION] Production from Marshes. +1 [ICON_HAPPINESS_1] Happiness if the City is on River.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS';

UPDATE Language_en_US
SET Text = 'Goddess of Renewal'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_RESEARCH] Science for every 2 Jungle or 2 Forest tiles worked by the City. +2 [ICON_FOOD] Food and [ICON_RESEARCH] Science from Markets.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH';

UPDATE Language_en_US
SET Text = 'Goddess of Springtime'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_GOLD] Gold from Plantations. +2 [ICON_PEACE] Faith and +1 [ICON_RESEARCH] Science from Herbalists.'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION';

UPDATE Language_en_US
SET Text = 'Goddess of the Home'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES_SHORT';

UPDATE Language_en_US
SET Text = '+25% [ICON_FOOD] Growth. +1 [ICON_PEACE] Faith and [ICON_FOOD] Food from Shrines. +8 [ICON_PEACE] Faith and [ICON_FOOD] Food when a Building is constructed, scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES';

-- Goddess of the Hunt
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_CULTURE] Culture from Camps. +2 [ICON_FOOD] Food from Smokehouses.'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT';

UPDATE Language_en_US
SET Text = 'Goddess of Wisdom'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_GREAT_SCIENTIST] Great Scientist Points in your Capital/Holy City. +1 [ICON_PEACE] Faith and [ICON_RESEARCH] Science in the City, and an additional +2 [ICON_PEACE] Faith and [ICON_RESEARCH] Science if the City has a Specialist.'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY';

UPDATE Language_en_US
SET Text = 'Spirit of the Desert'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_PRODUCTION] Production, and [ICON_GOLD] Gold from Desert tiles with Resources, and +3 [ICON_FOOD] Food from Oases.'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE';

UPDATE Language_en_US
SET Text = 'Tutelary Gods'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, +3 [ICON_PRODUCTION] Production, and +2 [ICON_GOLD] Gold if the City has at least 3 [ICON_CITIZEN] Citizens. +1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Engineers.'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE';

----------------------------------------------
-- Founders
----------------------------------------------
UPDATE Language_en_US
SET Text = 'Evangelism'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT';

UPDATE Language_en_US
SET Text = 'When you spread your [ICON_RELIGION] Religion to foreign Cities, gain +15 [ICON_FOOD] Food in Holy City, scaling with the number of new Followers of your [ICON_RELIGION] Religion, and 15 [ICON_TOURISM] Tourism, scaling with the number of Followers of other [ICON_RELIGION] Religions in the City.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Apostolic Palace National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +4 [ICON_GOLDEN_AGE] Golden Age Points; +5 [ICON_TOURISM] Tourism from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE';

-- Ceremonial Burial
UPDATE Language_en_US
SET Text = 'When a [ICON_GREAT_PEOPLE] Great Person is expended, gain 10 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every City following your [ICON_RELIGION] Religion (max 25 Cities), scaling with Era.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Mausoleum National Wonder[ENDCOLOR] (+5 [ICON_PEACE] Faith, and gain [ICON_PEACE] Faith when an owned unit is killed in battle, +5 [ICON_PEACE] Faith from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL';

UPDATE Language_en_US
SET Text = 'Council of Elders'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT';

UPDATE Language_en_US
SET Text = 'When a City adopts your [ICON_RELIGION] Religion for the first time, gain 20 [ICON_RESEARCH] Science and [ICON_PRODUCTION] Production in your Holy City, scaling gradually based on the number of Cities following your [ICON_RELIGION] Religion (bonus caps at 25 Cities).[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Holy Council National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +5 [ICON_FOOD] Food; +5 [ICON_RESEARCH] Science from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY';

UPDATE Language_en_US
SET Text = 'Divine Inheritance'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT';

UPDATE Language_en_US
SET Text = 'Holy City produces +20% of its Yields when your Empire is in a [ICON_GOLDEN_AGE] Golden Age.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Celestial Throne National Wonder[ENDCOLOR] (+2 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production; +5 [ICON_GOLDEN_AGE] Golden Age Points from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING';

UPDATE Language_en_US
SET Text = 'Hero Worship'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT';

UPDATE Language_en_US
SET Text = 'Receive 100 [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points when you conquer a City, as well as 25 [ICON_GREAT_GENERAL] Great General Points (if City is landlocked) or [ICON_GREAT_ADMIRAL] Great Admiral Points (if Coastal). Bonus scales with City [ICON_CITIZEN] Population and Era.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Great Altar National Wonder[ENDCOLOR] (+5 [ICON_PEACE] Faith, +15% Military Unit [ICON_PRODUCTION] Production; +5 [ICON_PRODUCTION] Production from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE';

UPDATE Language_en_US
SET Text = 'Holy Law'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT';

UPDATE Language_en_US
SET Text = 'When you unlock a Policy, gain 5 [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold for every Follower of your [ICON_RELIGION] Religion (max 250 Followers).[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Divine Court National Wonder[ENDCOLOR] (+4 [ICON_PEACE] Faith, +6 [ICON_GOLD] Gold; +5 [ICON_CULTURE] Culture from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY';

UPDATE Language_en_US
SET Text = 'Theocratic Rule'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH_SHORT';

UPDATE Language_en_US
SET Text = '"We Love the King Day" boosts the [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_GOLD] Gold output of a City by 15%.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Grand Ossuary National Wonder[ENDCOLOR] (+10 [ICON_PEACE] Faith; +5 [ICON_GOLD] Gold from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH';

UPDATE Language_en_US
SET Text = 'Transcendent Thoughts'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT';

UPDATE Language_en_US
SET Text = 'When you enter a new Era, Holy City gains 12 of every Yield for each City following your [ICON_RELIGION] Religion (max 25 Cities), scaling with Era.[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Sacred Garden National Wonder[ENDCOLOR] (+3 [ICON_PEACE] Faith, +5 [ICON_CULTURE] Culture; +5 [ICON_FOOD] Food from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES';

UPDATE Language_en_US
SET Text = 'Revelation'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT';

UPDATE Language_en_US
SET Text = 'When you research a technology, gain +2 [ICON_GOLDEN_AGE] Golden Age Points, [ICON_PEACE] Faith, and [ICON_CULTURE] Culture for every Follower of your [ICON_RELIGION] Religion (max 250 Followers).[NEWLINE]Unlocks [COLOR_POSITIVE_TEXT]Chartarium National Wonder[ENDCOLOR] (+5 [ICON_RESEARCH] Science from [ICON_RELIGION] Holy Sites; unlocks [COLOR_POSITIVE_TEXT]Reformation Belief[ENDCOLOR]).'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE';

----------------------------------------------
-- Followers
----------------------------------------------

-- Asceticism
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every follower in the City (max +15 [ICON_FOOD] Food).'
WHERE Tag = 'TXT_KEY_BELIEF_ASCETISM';

-- Cathedrals
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Cathedrals.'
WHERE Tag = 'TXT_KEY_BELIEF_CATHEDRALS';

UPDATE Language_en_US
SET Text = 'Churches'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES_SHORT';

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Churches.'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES';

UPDATE Language_en_US
SET Text = 'Indulgences'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith in a City for every 10 [ICON_GOLD] Gold per turn it produces, capped at half the number of Followers in the City. 10% of the cost of [ICON_PEACE] Faith Purchases in this City is converted into [ICON_GOLD] Gold and [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS';

UPDATE Language_en_US
SET Text = 'Creativity'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 3 followers in the City (max +6 [ICON_CULTURE] Culture), and +2 [ICON_PEACE] Faith if you have at least one Specialist in the City.'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION';

UPDATE Language_en_US
SET Text = 'Diligence'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every 2 followers in the City (max +15 [ICON_PRODUCTION] Production).'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY';

UPDATE Language_en_US
SET Text = 'Mandirs'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP_SHORT';

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mandirs.'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP';

UPDATE Language_en_US
SET Text = 'Mastery'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART_SHORT';

UPDATE Language_en_US
SET Text = 'Specialists generate +1 [ICON_GOLDEN_AGE] Golden Age Point and +1 of their primary Yield ([ICON_RESEARCH] Scientist, [ICON_GOLD] Merchant/Civil Servant, [ICON_PRODUCTION] Engineer, [ICON_CULTURE] Writer/Artist/Musician).'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART';

-- Mosques
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mosques.'
WHERE Tag = 'TXT_KEY_BELIEF_MOSQUES';

UPDATE Language_en_US
SET Text = 'Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER_SHORT';

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Orders.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER';

-- Pagodas
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Pagodas'
WHERE Tag = 'TXT_KEY_BELIEF_PAGODAS';

UPDATE Language_en_US
SET Text = 'Scholarship'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science for every 2 followers in the City (max +15 [ICON_RESEARCH] Science).'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC';

UPDATE Language_en_US
SET Text = 'Stupas'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES_SHORT';

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Stupas.'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES';

UPDATE Language_en_US
SET Text = 'Synagogues'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS_SHORT';

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Synagogues.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS';

UPDATE Language_en_US
SET Text = 'Thrift'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every follower in the City (max +10 [ICON_GOLD] Gold).'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD';

UPDATE Language_en_US
SET Text = 'Gurukulam'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith in a City for every 5 [ICON_RESEARCH] Science per turn it produces, capped at half the number of Followers in the City. 10% of the cost of [ICON_PEACE] Faith Purchases in this City is converted into [ICON_CULTURE] Culture and [ICON_FOOD] Food.'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA';

----------------------------------------------
-- Enhancers
----------------------------------------------
UPDATE Language_en_US
SET Text = 'Abode of Peace'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR_SHORT';

UPDATE Language_en_US
SET Text = '+100% Pressure to friendly [ICON_CITY_STATE] City-States. +1 [ICON_GOLD] Gold and [ICON_PEACE] Faith in Holy City for every 2 followers of this [ICON_RELIGION] Religion in City-States. Your resting point for [ICON_INFLUENCE] Influence with City-States following your Religion is increased by [COLOR_POSITIVE_TEXT]35[ENDCOLOR], and their Quest rewards are increased by 25%.'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR';

UPDATE Language_en_US
SET Text = 'Inquisition'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS_SHORT';

UPDATE Language_en_US
SET Text = '[ICON_INQUISITOR] Inquisitors cost 33% less [ICON_PEACE] Faith, and generate 25 [ICON_GOLD] Gold per converted [ICON_CITIZEN] Citizen when Removing Heresy. Your [ICON_SPY] Spies exert +52 Religious Pressure on the Cities they occupy (Standard Speed), and +2 [ICON_HAPPINESS_1] Happiness if stationed in a foreign City.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS';

UPDATE Language_en_US
SET Text = 'Mendicancy'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY_SHORT';

UPDATE Language_en_US
SET Text = '+2 [ICON_CULTURE] Culture and [ICON_PEACE] Faith in all owned Cities following this [ICON_RELIGION] Religion. [ICON_MISSIONARY] Missionaries of this Religion erode existing pressure from other religions by 10% when Spreading Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY';

UPDATE Language_en_US
SET Text = 'Orthodoxy'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER_SHORT';

UPDATE Language_en_US
SET Text = '[ICON_RELIGION] Religion spreads to Cities 40% further away. +200% Pressure to foreign Cities connected via [ICON_INTERNATIONAL_TRADE] Trade Routes.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER';

UPDATE Language_en_US
SET Text = 'Prophecy'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH_SHORT';

UPDATE Language_en_US
SET Text = 'Reduces minimum Policy requirement for Wonders by 1. Prophets of this [ICON_RELIGION] Religion 25% stronger, cost 25% less [ICON_PEACE] Faith. +3 to base Holy Site yields. If this is the majority Religion, Follower reduction from rival [ICON_INQUISITOR] Inquisitors and [ICON_PROPHET] Prophets is halved.'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH';

UPDATE Language_en_US
SET Text = 'Sacred Calendar'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL_SHORT';

UPDATE Language_en_US
SET Text = '+33% [ICON_GREAT_PEOPLE] Great Person Rate in Holy City during [ICON_GOLDEN_AGE] Golden Ages. +3 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_GOLD] Gold in [ICON_RELIGION] Holy City for every Foreign City following this Religion. Missionaries of this Religion 25% stronger.'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL';

UPDATE Language_en_US
SET Text = 'Symbolism'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS_SHORT';

UPDATE Language_en_US
SET Text = 'Holy City gains +5 [ICON_GOLDEN_AGE] Golden Age Points and +2 [ICON_GREAT_PEOPLE] Great Person Points per turn for all [ICON_GREAT_PEOPLE] Great People.'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS';

UPDATE Language_en_US
SET Text = 'Universalism'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY_SHORT';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science, and [ICON_PRODUCTION] Production in Holy City for every 5 followers of other Religions in owned Cities. +1 [ICON_GOLD] Gold and [ICON_PEACE] Faith in Holy City for every 10 followers of this [ICON_RELIGION] Religion in Foreign Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY';

UPDATE Language_en_US
SET Text = 'Zealotry'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION_SHORT';

UPDATE Language_en_US
SET Text = 'May spend [ICON_PEACE] Faith to purchase Land Units in Cities. Strategic Resource quantities increase by 1% per following City (up to 25%).'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION';

----------------------------------------------
-- Reformations
----------------------------------------------
UPDATE Language_en_US
SET Text = 'Crusader Spirit'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM_SHORT';

UPDATE Language_en_US
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in enemy territory, and an additional +10% versus Land Units of players that do not follow your Religion. Receive 50 [ICON_CULTURE] Culture and [ICON_GOLD] Gold when you conquer Cities, scaling with Era and City [ICON_CITIZEN] Population.'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM';

-- Defender of the Faith
UPDATE Language_en_US
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in friendly territory, and an additional +10% versus Land Units of players that do not follow your Religion. +1 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture from all Defensive Buildings.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH';

UPDATE Language_en_US
SET Text = 'Divine Teachings'
WHERE Tag = 'TXT_KEY_BELIEF_JESUIT_EDUCATION_SHORT';

UPDATE Language_en_US
SET Text = 'May build Libraries, Universities, Public Schools, and Research Labs with [ICON_PEACE] Faith, and each building gains +2 [ICON_RESEARCH] Science. Holy City gains 20 [ICON_PEACE] Faith each time the owner expends a [ICON_GREAT_PEOPLE] Great Person, scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_JESUIT_EDUCATION';

UPDATE Language_en_US
SET Text = 'Faith of the Masses'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT';

UPDATE Language_en_US
SET Text = 'May build Amphitheaters, Opera Houses, Museums, and Broadcast Towers with [ICON_PEACE] Faith. These buildings produce +2 [ICON_CULTURE] Culture each. Holy City owner gains +1 [ICON_HAPPINESS_1] Happiness for every two Cities following this Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT';

UPDATE Language_en_US
SET Text = 'Global Commandments'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS_SHORT';

UPDATE Language_en_US
SET Text = '[ICON_RELIGION] Religion spreads 15% faster (30% with Printing Press). Receive 350 [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, [ICON_GOLD] Gold, [ICON_PEACE] Faith, and [ICON_GOLDEN_AGE] Golden Age Points when you pass a Proposal in the World Congress or United Nations, scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS';

UPDATE Language_en_US
SET Text = 'Holy Land'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT';

UPDATE Language_en_US
SET Text = 'Receive 1 additional [ICON_DIPLOMAT] Delegate in the World Congress for every 2 [ICON_RELIGION] Holy Sites and [ICON_TOURISM] Landmarks you own. +50% Yields from Friendly/Allied [ICON_CITY_STATE] City-States following this Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR';

UPDATE Language_en_US
SET Text = 'Inspired Works'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT';

UPDATE Language_en_US
SET Text = 'Landmarks and Great Person Improvements produce +2 [ICON_PEACE] Faith and [ICON_RESEARCH] Science. [ICON_GREAT_WORK] Great Works produce +2 [ICON_CULTURE] Culture. Can purchase Archaeologists with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS';

-- Sacred Sites
UPDATE Language_en_US
SET Text = 'Hotels and buildings that can only be purchased with Faith provide +3 [ICON_TOURISM] Tourism each. World and Natural Wonders gain +4 [ICON_TOURISM] Tourism each. Royal Collection provides +10 [ICON_CULTURE] Culture and +10 [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES';

-- To the Glory of God
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase any type of [ICON_GREAT_PEOPLE] Great Person (in Industrial Era). When you expend a [ICON_GREAT_PEOPLE] Great Person, gain 3 [ICON_GOLD] Gold, [ICON_RESEARCH] Science, and [ICON_CULTURE] Culture per City following your Religion (max 20 Cities).'
WHERE Tag = 'TXT_KEY_BELIEF_TO_GLORY_OF_GOD';
