-- PANTHEONS

-- Goddess of the Hunt
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +1 [ICON_FOOD] Food from Camps'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Fertility Rites
UPDATE Language_en_US
SET Text = '+2 [ICON_FOOD] Food and +1 [ICON_PEACE] Faith from Shrines, and 10% faster [ICON_FOOD] Growth Rate'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Craftsmen
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 10 [ICON_GOLD] Gold per turn, and +3 [ICON_PRODUCTION] Production in Cities with a Specialist'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_CRAFTSMEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Sea
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Fishing Boats and Atolls. +2 [ICON_FOOD] Food in coastal Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Open Sky
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from Pastures and +1 [ICON_CULTURE] Culture for every 3 Plains tiles worked by the City'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messenger of the Gods
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +2 [ICON_RESEARCH] Science in Cities with a [ICON_CONNECTED] City Connection'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Commerce'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- One With Nature
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture from Natural Wonders and Mountains'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Cult of Nature'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgy (Now Goddess of Wisdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 10 [ICON_RESEARCH] Science per turn, and +2 [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Wisdom'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Stone Circles
UPDATE Language_en_US
SET Text = '+3 [ICON_PEACE] Faith from Quarries, and +2 [ICON_PRODUCTION] Production from Stone Works'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of War
UPDATE Language_en_US
SET Text = '+2 [ICON_RESEARCH] Science from Barracks, and gain [ICON_PEACE] Faith if you win a battle'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Waters
UPDATE Language_en_US
SET Text = 'Sacred Springs'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Oases, Lakes, and Marshes. +1 [ICON_HAPPINESS_1] Happiness from Cities on rivers'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Love
UPDATE Language_en_US
SET Text = 'Gain 5 [ICON_PEACE] Faith and 3 [ICON_GOLDEN_AGE] Golden Age Points every time a [ICON_CITIZEN] Citizen is born. Bonus scales with Era'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements
UPDATE Language_en_US
SET Text = '+10% faster border growth, and gain 5 [ICON_PEACE] Faith every time a city expands its borders. Bonus scales with Era'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Festivals
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture for every unique Luxury Resource you control'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Oral Tradition
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Plantations, and +2 [ICON_CULTURE] Culture from Markets'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ancestor Worship
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 4 [ICON_CITIZEN] Citizens in a city, and +3 [ICON_CULTURE] Culture in [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_ANCESTOR_WORSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Desert Folklore

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food and [ICON_GOLD] Gold from Desert tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Path
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 2 Jungle tiles worked by the City, and +1 [ICON_CULTURE] Culture for every 2 Forest tiles worked by the City'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Protection
UPDATE Language_en_US
SET Text = '+30% increase in city [ICON_RANGE_STRENGTH] Ranged Combat Strength, and +2 [ICON_PEACE] Faith from Walls'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Faith Healers
UPDATE Language_en_US
SET Text = '+15 HP healed per turn in friendly, non-attrition territory. Gain 2 [ICON_PEACE] Faith when healed this way. Bonus scales with Era'
WHERE Tag = 'TXT_KEY_BELIEF_FAITH_HEALERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monuments to the Gods
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from World Wonders. +15% [ICON_PRODUCTION] Production of Ancient/Classical Wonders'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Dance of the Aurora
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_PRODUCTION] Production and [ICON_CULTURE] Culture from Tundra tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

--  Tears of the Gods (Now Polytheism)
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and +1 [ICON_HAPPINESS_1] Happiness in [ICON_CAPITAL] Capital, and +1 [ICON_HAPPINESS_1] Happiness for every known Civilization with a Pantheon'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Polytheism'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Earth Mother
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from Mines on improved resources, and +2 [ICON_PRODUCTION] Production from Monuments'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God-King
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture, [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science for every 6 Followers'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sun God
UPDATE Language_en_US
SET Text = '+2 [ICON_FOOD] Food from Granaries and Farms on improved resources. +2 [ICON_PEACE] Faith in the [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
