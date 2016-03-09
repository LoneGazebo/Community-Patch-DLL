-- PANTHEONS

-- Goddess of the Hunt
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_CULTURE] Culture from Camps'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Fertility Rites (Now Goddess of Fertility)
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food and +1 [ICON_PEACE] Faith from Shrines and Wells, and 15% faster [ICON_FOOD] Growth Rate'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Fertility'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Craftsmen (DELETED)

-- God of the Sea
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Fishing Boats and Atolls. +3 [ICON_FOOD] Food in coastal Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Open Sky
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every 3 Plains or Grassland tiles (without hills or features) worked by a City, +1 [ICON_PEACE] Faith and +1 [ICON_GOLD] Gold from Pastures'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messenger of the Gods
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every +20 [ICON_GOLD] Gold produced by owned Cities. +2 [ICON_PEACE] Faith and [ICON_GOLD] Gold in Cities with a [ICON_CONNECTED] City Connection'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Commerce'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- One With Nature (Now Goddess of Nature)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every 2 Mountains within 3 tiles of a City. Natural Wonders gain +3 [ICON_PEACE] Faith and +2 [ICON_CULTURE] Culture'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Nature'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgy (Now Goddess of Wisdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 10 [ICON_RESEARCH] Science per turn. +1 [ICON_RESEARCH] Science and [ICON_PEACE] Faith in every City'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Wisdom'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Stone Circles (Now God of Craftsmen)
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from Quarries and Stone Works, and +3 [ICON_PRODUCTION] Production in Cities with a Specialist'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Craftsmen'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of War
UPDATE Language_en_US
SET Text = '+50% increase in city [ICON_RANGE_STRENGTH] Ranged Combat Strength. Gain [ICON_PEACE] Faith if you win a battle'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Waters (Now Goddess of Purity)
UPDATE Language_en_US
SET Text = 'Goddess of Purity'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_FOOD] Food from Oases, Lakes, and Marshes. +1 [ICON_HAPPINESS_1] Happiness from Cities on rivers'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Love
UPDATE Language_en_US
SET Text = 'Gain 10 [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points every time a [ICON_CITIZEN] Citizen is born. Bonuses scale with Era and Gamespeed'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements (Now God of the Expanse)
UPDATE Language_en_US
SET Text = '+25% faster border growth, and gain 20 [ICON_PEACE] Faith every time a city expands its borders. Bonus scales with Gamespeed'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements 
UPDATE Language_en_US
SET Text = 'God of the Expanse'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Festivals
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_GOLD] Gold for every unique Luxury Resource you control'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Oral Tradition (Now Goddess of Springtime)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture from Plantations, and +2 [ICON_GOLD] Gold from Markets'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Springtime'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ancestor Worship
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 4 [ICON_CITIZEN] Citizens in a city. +1 [ICON_CULTURE] Culture and [ICON_PEACE] Faith from Councils'
WHERE Tag = 'TXT_KEY_BELIEF_ANCESTOR_WORSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Desert Folklore (Now Spirit of the Desert)

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_GOLD] Gold from Desert tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Spirit of the Desert'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Path (Now Goddess of Renewal)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every 2 Jungle or 2 Forest tiles worked by a City. +1 [ICON_RESEARCH] Science and [ICON_PEACE] Faith from Herbalists'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Renewal'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Protection
UPDATE Language_en_US
SET Text = '+10 HP healed per turn in friendly territory. +1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture from Palace, Walls, and Barracks'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monuments to the Gods (Now Goddess of Beauty)
UPDATE Language_en_US
SET Text = '+3 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from World Wonders. +15% [ICON_PRODUCTION] Production of Pre-Renaissance Wonders'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Beauty'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Dance of the Aurora (now God of the Stars and Sky)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_PRODUCTION] Production, and [ICON_CULTURE] Culture from Tundra tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of the Stars and Sky'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

--  Tears of the Gods (Now God of Creation)
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture, [ICON_PEACE] Faith, and [ICON_HAPPINESS_1] Happiness for every Pantheon ever founded (including this one)'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of All Creation'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Earth Mother
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_GOLD] Gold from Mines on improved resources, and +2 [ICON_PRODUCTION] Production from Monuments'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God-King
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture, [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science for every 6 Followers of your Pantheon'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sun God
UPDATE Language_en_US
SET Text = '+2 [ICON_FOOD] Food from Granaries. +1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture from Farms on [ICON_RES_WHEAT] Wheat'
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
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in enemy lands, and an additional +10% versus Land Units of players that follow a different Religion. Receive [ICON_CULTURE] Culture and [ICON_GOLD] Gold when you conquer Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Crusader Spirit'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Defender Faith
UPDATE Language_en_US
SET Text = 'Land Units gain +10% [ICON_STRENGTH] Combat Strength versus Land Units in own lands, and an additional +10% versus Land Units of players that follow a different Religion. +1 [ICON_PEACE] Faith from all Defensive Buildings.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Jesuit Education 

-- Religious Fervor (Now The One True Faith)
UPDATE Language_en_US
SET Text = 'Receive 1 additional Delegate in the World Congress for every 8 City-States in the game. Missionaries performing the Spread Religion action erode existing pressure from other religions.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'One World, One Religion'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Sites
UPDATE Language_en_US
SET Text = 'All buildings purchased with Faith provide 2 [ICON_TOURISM] Tourism each. Hermitage provides +5 [ICON_CULTURE] Culture and +5 [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Glory of God
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase any type of [ICON_GREAT_PEOPLE] Great Person (in Industrial Era). +50 [ICON_GOLD] Gold, [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith when you expend a [ICON_GREAT_PEOPLE] Great Person, scaling with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_TO_GLORY_OF_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Underground Sect (Now Faith of the  Masses)
UPDATE Language_en_US
SET Text = 'May build Opera Houses, Museums and Broadcast Towers with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Faith of the Masses'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Unity of the Prophets (Now Knowledge Through Faith)

UPDATE Language_en_US
SET Text = 'Landmarks and Great Person Improvements produce +3 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Knowledge Through Devotion'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Buildings
		
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Cathedrals belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Poverty and boosts the [ICON_GOLD] Gold yield of nearby farms.'
WHERE Tag = 'TXT_KEY_BUILDING_CATHEDRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Pagodas belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Boredom and generates yields based on the number of Religions present in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_PAGODA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Mosques belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Illiteracy and generates [ICON_CULTURE] Culture during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Texts (Now Hymns)
UPDATE Language_en_US
SET Text = 'Religion spreads 25% faster (50% with Printing Press). Your spies exert religious pressure on the Cities they occupy.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Scripture'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Reliquary (Now Tithes)
UPDATE Language_en_US
SET Text = '+200 [ICON_GOLD] Gold when each City first converts to this religion, +1 [ICON_GOLD] Gold, +1 [ICON_PEACE] Faith for every 4 followers of this religion in Foreign Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tithes'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Just War (Now Martyrdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science and +1 [ICON_CULTURE] Culture for every 6 followers of this Religion in Foreign Cities. Gain 100 [ICON_PEACE] Faith each time a Great Person is expended. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sainthood'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Heathen Conversion (Now Holy Warriors)
UPDATE Language_en_US
SET Text = 'May spend Faith to purchase land units in Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Zealotry'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Itinerant Preachers (Pious Celebrations)
UPDATE Language_en_US
SET Text = '+1 [ICON_HAPPINESS_1] Happiness for every two Cities following this religion, +15 to [ICON_INFLUENCE] Influence resting point with City-States following this religion.'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Clericalism'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messiah (Now Sainthood)
UPDATE Language_en_US
SET Text = 'Prophets 25% stronger and earned with 25% less [ICON_PEACE] Faith. Inquisitors and Prophets reduce your religious presence by half (instead of eliminating it).'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Resilience'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Missionary Zeal (Now Scholar-Priests)
UPDATE Language_en_US
SET Text = 'Missionary conversion strength +25%, Gain [ICON_RESEARCH] Science when a Missionary spreads this religion to Cities of other religions.'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Evangelism'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Order
UPDATE Language_en_US
SET Text = 'Religion spreads to Cities 30% further away, and to friendly City-States at double rate.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ritual'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Unity
UPDATE Language_en_US
SET Text = 'Missionaries and Inquisitors cost 30% less [ICON_PEACE] Faith, +1 [ICON_HAPPINESS_1] Happiness for every 6 followers of this religion in non-enemy foreign Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Pacifism'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Cathedral
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Cathedrals (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_CATHEDRALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pagoda
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Pagodas (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_PAGODAS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Mosque
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mosques (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_MOSQUES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sword into Plowshare (Now Church)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Churches (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Churches'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Guruship (Now Mandir)
UPDATE Language_en_US
SET Text = 'Mandirs'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Mandirs (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Warriors (Now Synagogue)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Synagogues (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Synagogues'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monasteries (Now Stupa)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Stupas (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Stupas'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Feed the World (Now Thrift)
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold for every two followers in the City (max +20 [ICON_GOLD] Gold)'
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
SET Text = '+1 [ICON_RESEARCH] Science for every two followers in the City (max +15 [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Scholarship'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Art (Now Mastery)
UPDATE Language_en_US
SET Text = 'Specialists produce +2 of their primary yield ([ICON_RESEARCH] Science, [ICON_GOLD] Gold, [ICON_PRODUCTION] Production, or [ICON_CULTURE] Culture)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mastery'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgical Drama (Now Veneration)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every two followers in the City (max +10 [ICON_PEACE] Faith)'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Veneration'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ascetism
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every two followers in the City (max +15 [ICON_FOOD] Food)'
WHERE Tag = 'TXT_KEY_BELIEF_ASCETISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Center (Now Order)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Orders (hover for more info)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Community (Now Diliegence)
UPDATE Language_en_US
SET Text = 'Diligence'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every two followers in the City (max +20 [ICON_PRODUCTION] Production)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Divine Inspiration (Now Inspiration)
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every two followers in the City (max +10 [ICON_CULTURE] Culture)'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Inspiration'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Loving (Now Mandate of Heaven)
UPDATE Language_en_US
SET Text = 'Unlocks the Celestial Throne National Wonder (+2 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production; unlocks Reformation Belief). Holy City produces +15% of its yields during a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mandate of Heaven'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Interfaith Dialog (Now Hero Worship)
UPDATE Language_en_US
SET Text = 'Unlocks Great Altar National Wonder (+5 [ICON_PEACE] Faith, and +15% Military Unit [ICON_PRODUCTION] Production; unlocks Reformation Belief). Receive [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age points when you conquer a City. Bonus scales with City population and Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hero Worship'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ceremonial Burial
UPDATE Language_en_US
SET Text = 'Unlocks Mausoleum National Wonder (+5 [ICON_PEACE] Faith, and gain [ICON_PEACE] Faith when an owned unit is killed in battle; unlocks Reformation Belief). Receive 25 [ICON_PEACE] Faith and 50 [ICON_CULTURE] Culture whenever a Great Person is expended. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Church Property (Now Holy Law)
UPDATE Language_en_US
SET Text = 'Unlocks Divine Court National Wonder (+4 [ICON_PEACE] Faith, +6 [ICON_GOLD] Gold; unlocks Reformation Belief). Receive [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold when you purchase a Policy. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy Law'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Tithe (Now Way of the Pilgrim)
UPDATE Language_en_US
SET Text = 'Unlocks Reliquary National Wonder (+4 [ICON_PEACE] Faith, +2 [ICON_CULTURE] Culture, and 4 [ICON_GREAT_WORK] Art/Artfiact slots; unlocks Reformation Belief). Receive [ICON_TOURISM] Tourism when you spread your Religion to foreign cities. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of the Pilgrim'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Initiation Rites (Now Way of Transcendence)
UPDATE Language_en_US
SET Text = 'Unlocks Sacred Garden National Wonder (+3 [ICON_PEACE] Faith, +5 [ICON_CULTURE] Culture; unlocks Reformation Belief). +300 to all National Yields when you enter a new Era. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of Transcendence'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Papal Primacy (Now Council of Elders)
UPDATE Language_en_US
SET Text = 'Unlocks Holy Council National Wonder (+4 [ICON_PEACE] Faith, +5 [ICON_FOOD] Food; unlocks Reformation Belief). Receive a boost to current [ICON_RESEARCH] Science research when a city adopts your Religion. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Council of Elders'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pilgrimage (Now Apostolic Tradition
UPDATE Language_en_US
SET Text = 'Unlocks Apostolic Palace National Wonder (+4 [ICON_PEACE] Faith, and +4 [ICON_GOLDEN_AGE] Golden Age points; unlocks Reformation Belief). Receive [ICON_GOLDEN_AGE] Golden Age points when you spread your Religion to cities other than your Holy City. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Apostolic Tradition'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- World Church (Now Theocratic Rule)
UPDATE Language_en_US
SET Text = 'Unlocks Grand Ossuary National Wonder (+10 [ICON_PEACE] Faith; unlocks Reformation Belief). We Love the King Day boosts the [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_GOLD] Gold and [ICON_RESEARCH] Science output of a city by 15%.'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Theocratic Rule'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Belief Tooltips for Building beliefs

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CATHEDRAL_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE]+3 [ICON_GOLD]Gold[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Art Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CATHEDRAL_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_PAGODA_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+2 [ICON_CULTURE]Culture[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Art Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_PAGODA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MOSQUE_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE]+3 [ICON_RESEARCH]Science[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Literature Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MOSQUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_STUPA_TOOLTIP', '+5 [ICON_PEACE] Faith[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_STUPA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_SYNAGOGUE_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+3 [ICON_PRODUCTION] Production[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Literature Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_SYNAGOGUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MANDIR_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+5% [ICON_FOOD] Food[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MANDIR_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CHURCH_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CHURCH_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_ORDER_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_ORDER_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
