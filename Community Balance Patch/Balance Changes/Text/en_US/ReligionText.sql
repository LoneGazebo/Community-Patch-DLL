-- PANTHEONS

-- Goddess of the Hunt
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_CULTURE] Culture from Camps'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Fertility Rites (Now Goddess of Fertility)
UPDATE Language_en_US
SET Text = '+2 [ICON_FOOD] Food and +1 [ICON_PEACE] Faith from Shrines, Watermills, and Wells. 25% faster [ICON_FOOD] Growth Rate'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Fertility'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Craftsmen (DELETED)

-- God of the Sea
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Fishing Boats and Atolls. +3 [ICON_FOOD] Food in coastal Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Open Sky
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture in City per 2 worked Plains or 2 worked Grassland tiles without Hills or Features. +1 [ICON_PEACE] Faith and +3 [ICON_GOLD] Gold from Pastures'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messenger of the Gods
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every +20 [ICON_GOLD] Gold per Turn you produce. +2 [ICON_PEACE] Faith and [ICON_GOLD] Gold in [ICON_CAPITAL] Capital, and in Cities with a [ICON_CONNECTED] City Connection'
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
SET Text = '+1 [ICON_PEACE] Faith for every 15 [ICON_RESEARCH] Science per turn. +2 [ICON_RESEARCH] Science and +1 [ICON_PEACE] Faith in every City'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Wisdom'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Stone Circles (Now God of Craftsmen)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_PRODUCTION] Production from Quarries and Stone Works. +1 [ICON_CULTURE] Culture and [ICON_GOLD] Gold from Monuments'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of Craftsmen'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of War
UPDATE Language_en_US
SET Text = '+50% increase in city [ICON_RANGE_STRENGTH] Ranged Combat Strength. Gain [ICON_PEACE] Faith from killing a military unit'
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
SET Text = 'Gain 15 [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points every time a [ICON_CITIZEN] Citizen is born. Bonuses scale with Era and Gamespeed'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements (Now God of the Expanse)
UPDATE Language_en_US
SET Text = '+20% faster border growth, and gain 15 [ICON_PEACE] Faith and 6 [ICON_PRODUCTION] Production every time a city expands its borders, scaling with Gamespeed'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements 
UPDATE Language_en_US
SET Text = 'God of the Expanse'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Festivals
UPDATE Language_en_US
SET Text = '+3 [ICON_GOLD] Gold, +1 [ICON_PEACE] Faith, and +1 [ICON_CULTURE] Culture for every unique Luxury Resource in owned lands'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Oral Tradition (Now Goddess of Springtime)
UPDATE Language_en_US
SET Text = '+2 [ICON_GOLD] Gold and +1 [ICON_FOOD] Food from Plantations. +3 [ICON_PEACE] Faith from Markets'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Springtime'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ancestor Worship
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every 3 [ICON_CITIZEN] Citizens in a city. +1 [ICON_CULTURE] Culture from Councils, and +2 [ICON_RESEARCH] Science if you have at least one Specialist in the City'
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
SET Text = '+1 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every 2 Jungle or 2 Forest tiles worked by a City. +1 [ICON_RESEARCH] Science and [ICON_PEACE] Faith from Herbalists'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Renewal'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Protection
UPDATE Language_en_US
SET Text = '+10 HP healed per turn in friendly territory. +2 [ICON_PEACE] Faith and +1 [ICON_CULTURE] Culture from Palace, Walls, and Barracks'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monuments to the Gods (Now Goddess of Beauty)
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith from Palace and World Wonders, +1 [ICON_PEACE] Faith from [ICON_GREAT_WORK] Great Works. +3 [ICON_GREAT_ARTIST] Great Artist and [ICON_GREAT_ENGINEER] Great Engineer Points in your [ICON_CAPITAL] Capital'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Goddess of Beauty'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Dance of the Aurora (now God of the Stars and Sky)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_GOLD] Gold, and [ICON_FOOD] Food from Tundra tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of the Stars and Sky'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

--  Tears of the Gods (Now God of Creation)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Monuments. +1 [ICON_FOOD] Food, [ICON_PRODUCTION] Production, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital for every two Pantheons ever founded'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'God of All Creation'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Earth Mother
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith and [ICON_GOLD] Gold from Mines on improved resources, and +1 [ICON_PEACE] Faith from [ICON_RES_IRON] Iron. +1 [ICON_PRODUCTION] Production for every 3 [ICON_CITIZEN] Citizens in a city'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God-King
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith in [ICON_CAPITAL] Capital. +1 [ICON_CULTURE] Culture, [ICON_GOLDEN_AGE] Golden Age Points, [ICON_PEACE] Faith, [ICON_GOLD] Gold, and [ICON_RESEARCH] Science in [ICON_CAPITAL] Capital for every 6 Followers of your Pantheon in owned Cities'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sun God
UPDATE Language_en_US
SET Text = '+2 [ICON_PEACE] Faith and [ICON_GOLD] Gold from Granaries. +3 [ICON_FOOD] Food from Farms on [ICON_RES_WHEAT] Wheat, and +1 [ICON_RESEARCH] Science for every 2 Flood Plains worked by the City.'
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

-- Defender Faith
UPDATE Language_en_US
SET Text = 'Land Units gain +15% [ICON_STRENGTH] Combat Strength versus Land Units in own lands, and an additional +15% versus Land Units of players that do not follow your Religion. +2 [ICON_PEACE] Faith and +3 [ICON_CULTURE] Culture from all Defensive Buildings.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Jesuit Education 
UPDATE Language_en_US
SET Text = 'May build Universities, Public Schools, and Research Labs with [ICON_PEACE] Faith. These buildings produce +5 [ICON_RESEARCH] Science each.'
WHERE Tag = 'TXT_KEY_BELIEF_JESUIT_EDUCATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Fervor (Now The One True Faith)
UPDATE Language_en_US
SET Text = 'Receive 1 additional Delegate in the World Congress for every 8 City-States in the game. Missionaries performing the Spread Religion action erode existing pressure from other religions.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'One World, One Religion'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Sites
UPDATE Language_en_US
SET Text = 'Hotels and all buildings purchased with Faith provide +3 [ICON_TOURISM] Tourism each. Hermitage provides +10 [ICON_CULTURE] Culture and +10 [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Glory of God
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase any type of [ICON_GREAT_PEOPLE] Great Person (in Industrial Era). When you expend a [ICON_GREAT_PEOPLE] Great Person, Gain 2 [ICON_GOLD] Gold, [ICON_RESEARCH] Science, [ICON_CULTURE] Culture, and [ICON_PEACE] Faith per City following your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_TO_GLORY_OF_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Underground Sect (Now Faith of the  Masses)
UPDATE Language_en_US
SET Text = 'May build Opera Houses, Museums and Broadcast Towers with [ICON_PEACE] Faith. These buildings produce +5 [ICON_CULTURE] Culture each.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Faith of the Masses'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Unity of the Prophets (Now Knowledge Through Faith)

UPDATE Language_en_US
SET Text = 'Landmarks and Great Person Improvements produce +4 [ICON_PEACE] Faith and [ICON_RESEARCH] Science. [ICON_GREAT_WORK] Great Works produce +2 [ICON_CULTURE] Culture, and you can purchase Archaeologists with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Knowledge Through Devotion'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Buildings
		
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Cathedrals belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Poverty and boosts the [ICON_GOLD] Gold yield of nearby farms and pastures.'
WHERE Tag = 'TXT_KEY_BUILDING_CATHEDRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Pagodas belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Boredom and generates yields based on the number of Religions present in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_PAGODA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Mosques belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Illiteracy and generates [ICON_CULTURE] Culture during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Texts (Now Hymns)
UPDATE Language_en_US
SET Text = 'Religion spreads 30% faster (60% with Printing Press). Your spies exert religious pressure on the Cities they occupy.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Scripture'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Reliquary (Now Tithes)
UPDATE Language_en_US
SET Text = 'When a city adopts your Religion for the first time, Holy City gains 10 [ICON_GOLD] Gold for every City following your Religion, scaling with Era. +1 [ICON_GOLD] Gold and [ICON_PEACE] Faith in Holy city for every 8 followers of this Religion in Foreign Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Tithes'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Just War (Now Martyrdom)
UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science and [ICON_CULTURE] Culture in Holy City for every 10 followers of this Religion in Foreign Cities. Holy City gains 100 [ICON_PEACE] Faith each time the owner expends a Great Person. Bonus scales with Era.'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Sainthood'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Heathen Conversion (Now Holy Warriors)
UPDATE Language_en_US
SET Text = 'May spend Faith to purchase land units in Cities. Strategic Resource quantities increase by 1% per following City (up to 50%).'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Zealotry'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Itinerant Preachers (Pious Celebrations)
UPDATE Language_en_US
SET Text = 'Holy City owner gains +1 [ICON_HAPPINESS_1] Happiness for every two Cities following this religion, and +25% Yields from Friendly/Allied City-States following this religion.'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Clericalism'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messiah (Now Sainthood)
UPDATE Language_en_US
SET Text = 'Prophets of this Religion 25% stronger, cost 25% less [ICON_PEACE] Faith. If the majority Religion, Follower reduction from rival Inquisitors and Prophets halved. +3 to all Holy Site yields.'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Resilience'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Missionary Zeal (Now Scholar-Priests)
UPDATE Language_en_US
SET Text = 'Missionary conversion strength +25%. Holy City gains [ICON_RESEARCH] Science when owned Missionaries spread this Religion to Cities of other Religions based the number of followers of that Religion in the target City.'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Evangelism'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Order
UPDATE Language_en_US
SET Text = 'Religion spreads to Cities 20% further away. Pressure to friendly City-States and via owned [ICON_INTERNATIONAL_TRADE] Trade Routes doubled.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Ritual'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Unity
UPDATE Language_en_US
SET Text = 'Missionaries and Inquisitors cost 30% less [ICON_PEACE] Faith. Holy City owner gains +1 [ICON_HAPPINESS_1] Happiness for every 10 followers of this religion in non-enemy foreign Cities.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Pacifism'
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

-- Sword into Plowshare (Now Church)
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
SET Text = '+1 [ICON_RESEARCH] Science for every two followers in the City (max +15 [ICON_RESEARCH] Science)'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Scholarship'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Art (Now Mastery)
UPDATE Language_en_US
SET Text = 'Specialists generate +1 [ICON_GOLDEN_AGE] Golden Age Point and +1 of their primary Yield ([ICON_RESEARCH]/[ICON_GOLD]/[ICON_PRODUCTION]/[ICON_CULTURE].'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Mastery'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgical Drama (Now Veneration)
UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith for every two followers in the City (max +15 [ICON_PEACE] Faith)'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Veneration'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ascetism
UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food for every follower in the City (max +10 [ICON_FOOD] Food)'
WHERE Tag = 'TXT_KEY_BELIEF_ASCETISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Center (Now Order)
UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Orders'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Community (Now Diliegence)
UPDATE Language_en_US
SET Text = 'Diligence'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production for every two followers in the City (max +15 [ICON_PRODUCTION] Production)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Divine Inspiration (Now Inspiration)
UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture for every two followers in the City (max +15 [ICON_CULTURE] Culture)'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Inspiration'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Loving (Now Mandate of Heaven)
UPDATE Language_en_US
SET Text = 'Unlocks the Celestial Throne National Wonder (+2 [ICON_PEACE] Faith, [ICON_CULTURE] Culture, [ICON_FOOD] Food, [ICON_RESEARCH] Science, [ICON_GOLD] Gold, and [ICON_PRODUCTION] Production; +5 [ICON_PEACE] Faith from Holy Sites; unlocks Reformation Belief). Holy City produces +10% of its yields during a [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Divine Inheritance'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Interfaith Dialog (Now Hero Worship)
UPDATE Language_en_US
SET Text = 'Unlocks Great Altar National Wonder (+5 [ICON_PEACE] Faith, +15% Military Unit [ICON_PRODUCTION] Production; +5 [ICON_PRODUCTION] Production from Holy Sites; unlocks Reformation Belief). Receive [ICON_PEACE] Faith and [ICON_GOLDEN_AGE] Golden Age Points when you conquer a City. Bonus scales with City population and Era.'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Hero Worship'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ceremonial Burial
UPDATE Language_en_US
SET Text = 'Unlocks Mausoleum National Wonder (+5 [ICON_PEACE] Faith, and gain [ICON_PEACE] Faith when an owned unit is killed in battle, +5 [ICON_PEACE] Faith from Holy Sites; unlocks Reformation Belief). When a Great Person is expended, gain 10 [ICON_PEACE] Faith and [ICON_CULTURE] Culture for every City following your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Church Property (Now Holy Law)
UPDATE Language_en_US
SET Text = 'Unlocks Divine Court National Wonder (+4 [ICON_PEACE] Faith, +6 [ICON_GOLD] Gold; +5 [ICON_GOLD] Gold from Holy Sites; unlocks Reformation Belief). When you unlock a Policy, gain 4 [ICON_PEACE] Faith, [ICON_RESEARCH] Science, and [ICON_GOLD] Gold for every Follower of your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy Law'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Tithe (Now Way of the Pilgrim)
UPDATE Language_en_US
SET Text = 'Unlocks Reliquary National Wonder (+4 [ICON_PEACE] Faith, +2 [ICON_CULTURE] Culture, 4 [ICON_GREAT_WORK] Art/Artfiact slots; +5 [ICON_TOURISM] Tourism from Holy Sites; unlocks Reformation Belief). Receive 15 [ICON_TOURISM] Tourism and [ICON_CULTURE] Culture when you spread your Religion to foreign cities, scaling with the number of Followers of other Religions. World Wonders require 1 less unlocked Policy per every 8 Cities following your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of the Pilgrim'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Initiation Rites (Now Way of Transcendence)
UPDATE Language_en_US
SET Text = 'Unlocks Sacred Garden National Wonder (+3 [ICON_PEACE] Faith, +5 [ICON_CULTURE] Culture; +5 [ICON_FOOD] Food from Holy Sites; unlocks Reformation Belief). When you enter a new Era, Holy City gains 30 of every Yield for each City following your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Way of Transcendence'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Papal Primacy (Now Council of Elders)
UPDATE Language_en_US
SET Text = 'Unlocks Holy Council National Wonder (+4 [ICON_PEACE] Faith, +5 [ICON_FOOD] Food; +5 [ICON_RESEARCH] Science from Holy Sites; unlocks Reformation Belief). When a city adopts your Religion for the first time, gain 10 [ICON_RESEARCH] Science and [ICON_PRODUCTION] Production in your Holy City for every City following your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Council of Elders'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pilgrimage (Now Apostolic Tradition)
UPDATE Language_en_US
SET Text = 'Unlocks Apostolic Palace National Wonder (+4 [ICON_PEACE] Faith, +4 [ICON_GOLDEN_AGE] Golden Age Points; +5 [ICON_GOLDEN_AGE] Golden Age Points from Holy Sites; unlocks Reformation Belief). When you increase the number of Followers in a City by spreading Religion, receive 25 [ICON_GOLDEN_AGE] Golden Age Points and [ICON_FOOD] Food in the Holy City, scaling with the number of new Followers of your Religion.'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Apostolic Tradition'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- World Church (Now Theocratic Rule)
UPDATE Language_en_US
SET Text = 'Unlocks Grand Ossuary National Wonder (+10 [ICON_PEACE] Faith; +5 [ICON_CULTURE] Culture from Holy Sites; unlocks Reformation Belief). We Love the King Day boosts the [ICON_PEACE] Faith, [ICON_CULTURE] Culture, and [ICON_GOLD] Gold output of a city by 10%.'
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
	SELECT 'TXT_KEY_BUILDING_STUPA_TOOLTIP', '+3 [ICON_PEACE] Faith[NEWLINE]+2 [ICON_GOLDEN_AGE] Golden Age Points[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_STUPA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_SYNAGOGUE_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE]+3 [ICON_PRODUCTION] Production[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Literature Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_SYNAGOGUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MANDIR_TOOLTIP', '+3 [ICON_PEACE] Faith, +2 [ICON_FOOD] Food[NEWLINE]+10% [ICON_FOOD] Food[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MANDIR_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CHURCH_TOOLTIP', '+4 [ICON_PEACE] Faith[NEWLINE]1 [ICON_GREAT_WORK] Great Work of Music Slot[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CHURCH_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_en_US (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_ORDER_TOOLTIP', '+2 [ICON_PEACE] Faith[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_ORDER_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
