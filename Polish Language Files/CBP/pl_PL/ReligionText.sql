--translated by mrlg
-- PANTHEONS

-- Goddess of the Hunt
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary, [ICON_FOOD] żywności i [ICON_CULTURE] kultury z obozów'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_HUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Fertility Rites (Now Goddess of Fertility)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_FOOD] żywności i [ICON_PEACE] wiary z kaplic oraz studni oraz 15% szybsze [ICON_FOOD] tempo wzrostu'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini płodności|Bogini płodności|Bogini płodności|Boginię płodności|Boginią płodności|Bogini płodności', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_FERTILITY_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Craftsmen (DELETED)

-- God of the Sea
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary z łodzi rybackich i atoli. +3 [ICON_FOOD] żywności w miastach nadmorskich'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of the Open Sky
UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury za każde 3 pola łąk lub równin (bez wzgórz bądź ulepszeń) w pobliżu miasta, +1 [ICON_PEACE] wiary i [ICON_GOLD] złota z pastwisk'
WHERE Tag = 'TXT_KEY_BELIEF_OPEN_SKY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messenger of the Gods
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary za każde +20 [ICON_GOLD] złota wytwarzanego przez posiadane miasta. +2 [ICON_PEACE] wiary i [ICON_GOLD] złota w miastach [ICON_CONNECTED] połączonych ze stolicą'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bóg handlu|Boga|Bogu handlu|Boga handlu|Bogiem handlu|Bogu handlu', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_MESSENGER_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- One With Nature (Now Goddess of Nature)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary i [ICON_CULTURE] kultury za każde 2 góry w obrębie 3 pól od miasta. Cuda natury dają +3 [ICON_PEACE] wiary i +2 [ICON_CULTURE] kultury'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini natury|Bogini natury|Bogini natury|Boginię natury|Boginią natury|Bogini natury', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_ONE_WITH_NATURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgy (Now Goddess of Wisdom)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary za każde 10 [ICON_RESEARCH] nauki na turę. +1 [ICON_RESEARCH] nauki i [ICON_PEACE] wiary w każdym mieście'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini mądrości|Bogini mądrości|Bogini mądrości|Boginię mądrości|Boginią mądrości|Bogini mądrości', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_FORMAL_LITURGY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Stone Circles (Now God of Craftsmen)
UPDATE Language_pl_PL
SET Text = '+3 [ICON_PEACE] wiary z kamieniołomów i zakładów kamieniarskich oraz +3 [ICON_PRODUCTION] produkcji w miastach ze specjalistą'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bóg rzemieślników|Boga rzemieślników|Bogu rzemieślników|Boga rzemieślników|Bogiem rzemieślników|Bogu rzemieślników', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_STONE_CIRCLES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of War
UPDATE Language_pl_PL
SET Text = '+50% do miejskiej [ICON_RANGE_STRENGTH] siły ostrzału. Zyskujesz [ICON_PEACE] wiarę ze zwycięskich bitew'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Waters (Now Goddess of Purity)
UPDATE Language_pl_PL
SET Text = 'Bogini czystości|Bogini czystości|Bogini czystości|Boginię czystości|Boginią czystości|Bogini czystości', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary i [ICON_FOOD] żywności  z oaz, jezior i bagien. +1 [ICON_HAPPINESS_1] zadowolenia z miast nad rzekami'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_WATERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Love
UPDATE Language_pl_PL
SET Text = 'Zyskujesz 10 [ICON_PEACE] wiary i punktów [ICON_GOLDEN_AGE] złotej ery za każdym razem, kiedy rodzi się [ICON_CITIZEN] obywatel. Premia zależy od ery i prędkości gry'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_LOVE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements (Now God of the Expanse)
UPDATE Language_pl_PL
SET Text = '25% szybszy rozrost granic i zyskujesz 20 [ICON_PEACE] wiary za każdym razem, kiedy miasto rozszerza swoje granice. Premia zależy od prędkości gry'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Settlements 
UPDATE Language_pl_PL
SET Text = 'Bóg ekspansji|Boga ekspansji|Bogu ekspansji|Boga ekspansji|Bogiem ekspansji|Bogu ekspansji', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_SETTLEMENTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God of Festivals
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary, [ICON_CULTURE] kultury i [ICON_GOLD] złota za każdy unikalny surowiec luksusowy pod kontrolą'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_FESTIVALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Oral Tradition (Now Goddess of Springtime)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary i [ICON_CULTURE] kultury z plantacji oraz +2 [ICON_CULTURE] złota z targowisk'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini wiosny|Bogini wiosny|Bogini wiosny|Boginię wiosny|Boginią wiosny|Bogini wiosny', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_ORAL_TRADITION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ancestor Worship
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary za każdych 3 [ICON_CITIZEN] mieszkańców miasta. +1 [ICON_CULTURE] kultury i [ICON_PEACE] wiary z rad'
WHERE Tag = 'TXT_KEY_BELIEF_ANCESTOR_WORSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Desert Folklore (Now Spirit of the Desert)

UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary, [ICON_FOOD] żywności i [ICON_GOLD] złota z pól pustyni z ulepszonymi surowcami'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Duch pustyni|Ducha pustyni|Duchowi pustyni|Ducha pustyni|Duchem pustyni|Duchu pustyni', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Path (Now Goddess of Renewal)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary i [ICON_CULTURE] kultury za każde 2 wykorzystywane przez miasto pola dżungli lub lasu. +1 [ICON_RESEARCH] nauki i [ICON_PEACE] wiary z apteki'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini odnowy|Bogini odnowy|Bogini odnowy|Boginię odnowy|Boginią odnowy|Bogini odnowy', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_PATH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Goddess of Protection
UPDATE Language_pl_PL
SET Text = '+10 PŻ leczonych na turę w sojuszniczym terytorium. +1 [ICON_PEACE] wiary i [ICON_CULTURE] kultury z pałacu, murów i koszarów'
WHERE Tag = 'TXT_KEY_BELIEF_GODDESS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monuments to the Gods (Now Goddess of Beauty)
UPDATE Language_pl_PL
SET Text = '+3 [ICON_PEACE] wiary i +1 [ICON_CULTURE] kultury z cudów świata . +15% [ICON_PRODUCTION] do produkcji cudów przedrenesansowych'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogini piękna|Bogini piękna|Bogini piękna|Boginię piękna|Boginią piękna|Bogini piękna', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_MONUMENT_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Dance of the Aurora (now God of the Stars and Sky)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary, [ICON_PRODUCTION] produkcji i [ICON_CULTURE] kultury z pól tundry z ulepszonymi surowcami'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bóg nieba i gwiazd|Boga nieba i gwiazd|Bogu nieba i gwiazd|Boga nieba i gwiazd|Bogiem nieba i gwiazd|Bogu nieba i gwiazd', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

--  Tears of the Gods (Now God of Creation)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury, [ICON_PEACE] wiary i [ICON_HAPPINESS_1] zadowolenia za każdy założony panteon (łącznie z tym)'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bóg wszelkiego stworzenia|Boga wszelkiego stworzenia|Bogu wszelkiego stworzenia|Boga wszelkiego stworzenia|Bogiem wszelkiego stworzenia|Bogu wszelkiego stworzenia', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_TEARS_OF_GODS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Earth Mother
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary i [ICON_GOLD] złota z kopalni na ulepszonych surowcach oraz +2 [ICON_PRODUCTION] produkcji z pomników'
WHERE Tag = 'TXT_KEY_BELIEF_EARTH_MOTHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- God-King
UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury, [ICON_PEACE] wiary, [ICON_GOLD] złota i [ICON_RESEARCH] nauki za każdych 6 wyznawców twojego panteonu'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sun God
UPDATE Language_pl_PL
SET Text = '+2 [ICON_FOOD] żywności ze spichlerzy. +1 [ICON_PEACE] wiary i [ICON_CULTURE] kultury z farm na [ICON_RES_WHEAT] pszenicy'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bóg Słońca|Boga Słońca|Bogu Słońca|Boga Słońca|Bogiem Słońca|Bogu Słońca'
WHERE Tag = 'TXT_KEY_BELIEF_SUN_GOD_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Charitable Missions (Now Missionary Influence)
UPDATE Language_pl_PL
SET Text = '+10 [ICON_RESEARCH] nauki, [ICON_CULTURE] kultury, [ICON_GOLD] złota, [ICON_PEACE] wiary i punktów [ICON_GOLDEN_AGE] złotej ery na turę, kiedy jesteś gospodarzem Kongresu Światowego. Otrzymujesz 150 tych dóbr natychmiast, kiedy wejdzie w życie twoja rezolucja. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dowództwo światowe|Dowództwa światowego|Dowództwu światowemu|Dowództwo światowe|Dowództwem światowym|Dowództwie światowym', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Evangelism (Now Crusader Zeal)
UPDATE Language_pl_PL
SET Text = 'Jednostki lądowe otrzymują +10% [ICON_STRENGTH] siły bojowej przeciwko jednostkom lądowym na ziemiach wroga, ponadto +10%  przeciwko jednostkom lądowym graczy wyznających inne religie. Otrzymujesz [ICON_CULTURE] kulturę i [ICON_GOLD] złoto, kiedy zdobywasz miasta.'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Duch krzyżowca|Ducha krzyżowca|Duchowi krzyżowca|Ducha krzyżowca|Duchem krzyżowca|Duchu krzyżowca', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Defender Faith
UPDATE Language_pl_PL
SET Text = 'Jednostki lądowe otrzymują +10% [ICON_STRENGTH] siły bojowej przeciwko jednostkom lądowym na własnych ziemiach, ponadto +10% przeciwko jednostkom lądowym graczy wyznających inne religie. +1 [ICON_PEACE] wiary ze wszystkich budowli obronnych.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Jesuit Education

-- Religious Fervor (Now The One True wiary)
UPDATE Language_pl_PL
SET Text = 'Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 państw-miast w grze. Misjonarze przeprowadzający szerzenie wiary zmniejszają istniejący nacisk innych religii.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Jeden świat, jedna religia|Jednego świata, jednej religii|Jednemu światu, jednej religii|Jeden świat, jedną religię|Jednym światem, jedną religią|Jednym świecie, jednej religii', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Sites
UPDATE Language_pl_PL
SET Text = 'Wszystkie budynki zakupione za wiarę dostarczają 2 punkty [ICON_TOURISM] turystki. Ermitaż dostarcza +5 [ICON_CULTURE] kultury i +5 [ICON_TOURISM] turystyki.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Glory of God
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary do zakupu dowolnego [ICON_GREAT_PEOPLE] wielkiego człowieka (w erze przemysłowej). +50 [ICON_GOLD] złota, [ICON_RESEARCH] nauki, [ICON_CULTURE] kultury i [ICON_PEACE] wiary po poświęceniu [ICON_GREAT_PEOPLE] wielkiej osoby, zależne od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_TO_GLORY_OF_GOD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Underground Sect (Now wiary of the  Masses)
UPDATE Language_pl_PL
SET Text = 'Możesz budować opery, muzea i wieże transmisyjne za [ICON_PEACE] wiarę.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT' AND exists (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wiara dla mas|Wiary dla mas|Wierze dla mas|Wiarę dla mas|Wiarą dla mas|Wierze dla mas', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Unity of the Prophets (Now Knowledge Through wiary)

UPDATE Language_pl_PL
SET Text = 'Słynne miejsca i ulepszenia wielkich ludzi produkują +3 [ICON_PEACE] wiary i +1 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wiedza przez oddanie|Wiedzy przez oddanie|Wiedzy przez oddanie|Wiedzę przez oddanie|Wiedzą przez oddanie|Wiedzy przez oddanie', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Buildings

UPDATE Language_pl_PL
SET Text = 'Budynek ten może zostać zbudowany jedynie w miastach z wierzeniem „Katedr”. Budowany poprzez zakup za punkty [ICON_PEACE] wiary. Redukuje [ICON_HAPPINESS_3] ubóstwo i zwiększa zyski [ICON_GOLD] złota z pobliskich farm.'
WHERE Tag = 'TXT_KEY_BUILDING_CATHEDRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Budynek ten może zostać zbudowany jedynie w miastach z wierzeniem „Pagód”. Budowany poprzez zakup za punkty [ICON_PEACE] wiary. Redukuje [ICON_HAPPINESS_3] znudzenie i generuje zyski bazujące na ilości obecnych w mieście religii.'
WHERE Tag = 'TXT_KEY_BUILDING_PAGODA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
		
UPDATE Language_pl_PL
SET Text = 'Budynek ten może zostać zbudowany jedynie w miastach z wierzeniem „Meczetów”. Budowany poprzez zakup za punkty [ICON_PEACE] wiary. Redukuje [ICON_HAPPINESS_3] analfabetyzm i generuje [ICON_CULTURE] kultury podczas [ICON_GOLDEN_AGE] złotych er.'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Texts (Now Hymns)
UPDATE Language_pl_PL
SET Text = 'Religia szerzy się 25% szybciej (50% z prasą drukarską). Twoi szpiedzy wywierają nacisk religijny na miasta, w których się znajdują.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pismo Święte|Pisma Świętego|Pismu Świętemu|Pismo Święte|Pismem Świętym|Piśmie Świętym', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_TEXTS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Reliquary (Now Tithes)
UPDATE Language_pl_PL
SET Text = '+200 [ICON_GOLD] sztuk złota na każde miasto, które pierwszy raz nawraca się na tę religię, +1 [ICON_GOLD] sztuka złota, +1 [ICON_PEACE] punkt wiary na każdych 4 wyznawców tej wiary w zagranicznych miastach.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dziesięcina|Dziesięciny|Dziesięcinie|Dziesięcinę|Dziesięciną|Dziesięcinie', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIQUARY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Just War (Now Martyrdom)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_RESEARCH] nauki i +1 [ICON_CULTURE] kultury na każdych 6 wyznawców tej religii w zagranicznych miastach. Zyskujesz 100 [ICON_PEACE] punktów wiary za każdym razem, gdy poświęcasz wielkiego człowieka. Premia jest zależna od epoki.'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Świętość|Świętości|Świętości|Świętość|Świętością|Świętości', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_JUST_WAR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Heathen Conversion (Now Holy Warriors)
UPDATE Language_pl_PL
SET Text = 'Umożliwa zakup pól w miastach za punkty wiary.'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dewocja|Dewocji|Dewocji|Dewocję|Dewocją|Dewocji', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_HEATHEN_CONVERSION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Itinerant Preachers (Pious Celebrations)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_HAPPINESS_1] zadowolenia na każde dwa miasta z tą religią, +15 do punktu równowagi [ICON_INFLUENCE] wpływu na państwa-miasta wyznające tę religię.'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Klerykalizm|Klerykalizmu|Klerykalizmowi|Klerykalizm|Klerykalizmem|Klerykalizmie', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_ITINERANT_PREACHERS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Messiah (Now Sainthood)
UPDATE Language_pl_PL
SET Text = 'Prorocy są 25% silniejsi i potrzebują 25% mniej [ICON_PEACE] wiary. Inkwizytorzy i prorocy obniżają obecność religijną o połowę (zamiast ją eliminować).'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Prężność|Prężności|Prężności|Prężność|Prężnością|Prężności', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_MESSIAH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Missionary Zeal (Now Scholar-Priests)
UPDATE Language_pl_PL
SET Text = 'Siła nawracania misjonarzy +25%, dostajesz [ICON_RESEARCH] naukę, gdy misjonarz szerzy tę wiarę w miastach innych religii.'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ewangelizacja|Ewangelizacji|Ewangelizacji|Ewangelizację|Ewangelizacją|Ewangelizacji', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_MISSIONARY_ZEAL_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Order
UPDATE Language_pl_PL
SET Text = 'Religia szerzy się do miast 30% dalej, a to przyjacielskich państw-miast dwa razy szybciej.'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rytualizm|Rytualizmu|Rytualizmowi|Rytualizm|Rytualizmem|Rytualizmie', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_ORDER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Unity
UPDATE Language_pl_PL
SET Text = 'Misjonarze i inkwizytorzy kosztują 30% mniej [ICON_PEACE] wiary, +1 [ICON_HAPPINESS_1] do zadowolenia na każdych 6 wyznawców tej religii w zagranicznych miastach, które nie są wrogie.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pacyfizm|Pacyfizmu|Pacyfizmie|Pacyfizm|Pacyfizmem|Pacyfizmie', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_UNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Cathedral
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować katedry (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_CATHEDRALS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pagoda
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować pagody (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_PAGODAS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Mosque
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować meczety (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_MOSQUES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sword into Plowshare (Now Church)
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować kościoły (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kościoły|Kościołów|Kościołom|Kościoły|Kościołami|Kościołach', Gender = 'masculine', Plurality = '2'
WHERE Tag = 'TXT_KEY_BELIEF_SWORD_PLOWSHARES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Guruship (Now Mandir)
UPDATE Language_pl_PL
SET Text = 'Mandirs'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować mandiry (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_GURUSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Holy Warriors (Now Synagogue)
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować synagogi (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Synagogi|Synagog|Synagogom|Synagogi|Synagogami|Synagogach', Gender = 'feminine', Plurality = '2'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Monasteries (Now Stupa)
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować stupy (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Stupy|Stup|Stupom|Stupy|Stupami|Stupach', Gender = 'feminine', Plurality = '2'
WHERE Tag = 'TXT_KEY_BELIEF_MONASTERIES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Feed the World (Now Thrift)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_GOLD] złota na każdych dwóch wyznawców w mieście (max +20 [ICON_GOLD] złota)'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Gospodarność|Gospodarności|Gospodarności|Gospodarność|Gospodarnością|Gospodarności', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_FEED_WORLD_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Gardens (Now Cooperation)
UPDATE Language_pl_PL
SET Text = 'Otrzymujesz +5 do zbiorów wszystkich miast za każdym razem, kiedy [ICON_CITIZEN] obywatel rodzi się w mieście. Premia zależna od epoki'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Współpraca|Współpracy|Współpracy|Współpracę|Współpracą|Współpracy', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_GARDENS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Choral Music (Now Scholarship)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_RESEARCH] nauki za każdych dwóch wyznawców w mieście (max +15 [ICON_RESEARCH] nauki)'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Stypendium|Stypendium|Stypendium|Stypendium|Stypendium|Stypendium', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_CHORAL_MUSIC_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Art (Now Mastery)
UPDATE Language_pl_PL
SET Text = 'Specjaliści produkują +2 swoich podstawowych zbiorów ([ICON_RESEARCH] nauka, [ICON_GOLD] złota, [ICON_PRODUCTION] produkcja lub [ICON_CULTURE] kultura)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mistrzostwo|Mistrzostw|Mistrzostwom|Mistrzostwa|Mistrzostwami|Mistrzostwach', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_ART_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Liturgical Drama (Now Veneration)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PEACE] wiary za każdych dwóch wyznawców w mieście (max +10 [ICON_PEACE] wiary)'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Cześć|Czci|Czci|Cześć|Czcią|Czci', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_LITURGICAL_DRAMA_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ascetism
UPDATE Language_pl_PL
SET Text = '+1 [ICON_FOOD] żywności za każdych dwóch wyznawców w mieście (max +15 [ICON_FOOD] żywności)'
WHERE Tag = 'TXT_KEY_BELIEF_ASCETISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Center (Now Order)
UPDATE Language_pl_PL
SET Text = 'Używaj [ICON_PEACE] wiary, aby kupować zakony (najedź kursorem, aby uzyskać więcej informacji)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zakony|Zakonów|Zakonom|Zakony|Zakonami|Zakonach', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_CENTER_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Religious Community (Now Diliegence)
UPDATE Language_pl_PL
SET Text = 'Gorliwość|Gorliwości|Gorliwości|Gorliwość|Gorliwością|Gorliwości', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_PRODUCTION] produkcji za każdych dwóch wyznawców w mieście (max +20 [ICON_PRODUCTION] produkcji)'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_COMMUNITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Divine Inspiration (Now Inspiration)
UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury za każdych dwóch wyznawców w mieście (max +10 [ICON_CULTURE] kultury)'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Inspiracja|Inspiracji|Inspiracji|Inspirację|Inspiracją|Inspiracji', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_DIVINE_INSPIRATION_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Peace Loving (Now Mandate of Heaven)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Niebiański Tron (+2 [ICON_PEACE] wiary, [ICON_CULTURE] kultury, [ICON_FOOD] żywności, [ICON_RESEARCH] nauki, [ICON_GOLD] złota, i [ICON_PRODUCTION] produkcji; odblokowuje wierzenie reformacyjne). Święte miasto produkuje +15% swoich zysków podczas [ICON_GOLDEN_AGE] złotej ery.'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mandat Niebios|Mandatu Niebios|Mandatowi Niebios|Mandat Niebios|Mandacie Niebios|Mandatem Niebios', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_PEACE_LOVING_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Interfaith Dialog (Now Hero Worship)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Wielki Ołtarz (+5 [ICON_PEACE] wiary i +15% do [ICON_PRODUCTION] produkcji jednostek militarnych; odblokowuje wierzenie reformacyjne). Otrzymujesz [ICON_PEACE] wiarę i punkty [ICON_GOLDEN_AGE] złotej ery po podbiciu miasta. Premia zależy od populacji miasta i ery.'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kult Herosa|Kultu Herosa|Kultowi Herosa|Kult Herosa|Kultem Herosa|Kulcie Herosa', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_INTERFAITH_DIALOGUE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Ceremonial Burial
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Mauzoleum (+5 [ICON_PEACE] wiary i zyskujesz [ICON_PEACE] wiarę, kiedy twoja własna jednostka ginie w walce; odblokowuje wierzenie reformacyjne). Otrzymujesz 25 [ICON_PEACE] wiary i 50 [ICON_CULTURE] kultury po każdym poświęceniu wielkiej osoby. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_CEREMONIAL_BURIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Church Property (Now Holy Law)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Sąd Boski(+4 [ICON_PEACE] wiary, +6 [ICON_GOLD] złota; odblokowuje wierzenie reformacyjne). Otrzymujesz [ICON_PEACE] wiarę, [ICON_RESEARCH] naukę, i [ICON_GOLD] złoto po zakupieniu ustroju. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Prawo Boskie|Prawa Boskiego|Prawu Boskiemu|Prawo Boskie|Prawem Boskim|Prawie Boskim', Gender = 'neuter', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_CHURCH_PROPERTY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Tithe (Now Way of the Pilgrim)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Relikwiarz (+4 [ICON_PEACE] wiary, +2 [ICON_CULTURE] kultury i 4 miejsca na [ICON_GREAT_WORK] sztukę/artefakty; odblokowuje wierzenie reformacyjne). Otrzymujesz [ICON_TOURISM] turystykę, gdy twoja religia szerzy się do zagranicznych miast. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Droga Pielgrzyma|Drogi Pielgrzyma|Drodze Pielgrzyma|Drogę Pielgrzyma|Drogą Pielgrzyma|Drodze Pielgrzyma', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_TITHE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Initiation Rites (Now Way of Transcendence)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Święty Ogród (+3 [ICON_PEACE] wiary, +5 [ICON_CULTURE] kultury; odblokowuje wierzenie reformacyjne). +300 do wszystkich zbiorów w kraju, kiedy wchodzisz w nową erę. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Droga Transcendencji|Drogi Transcendencji|Drodze Transcendencji|Drogę Transcendencji|Drogą Transcendencji|Drodze Transcendencji', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_INITIATION_RITES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Papal Primacy (Now Council of Elders)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Sobór Powszechny (+4 [ICON_PEACE] wiary, +5 [ICON_FOOD] żywności; odblokowuje wierzenie reformacyjne). Otrzymujesz premię do obecnego [ICON_RESEARCH] postępu naukowego, gdy miasto przyjmuje twoją religię. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sobór Powszechny|Soboru Powszechnego|Soborze Powszechnemu|Sobór Powszechny|Soborem Powszechnym|Soborze Powszechnym', Gender = 'masculine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_PAPAL_PRIMACY_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Pilgrimage (Now Apostolic Tradition
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Pałac Apostolski (+4 [ICON_PEACE] wiary i +4 punkty [ICON_GOLDEN_AGE] złotej ery; odblokowuje wierzenie reformacyjne). Otrzymujesz punkty [ICON_GOLDEN_AGE] złotej ery, kiedy twoja religia szerzy się w mieście innym niż twoje święte miasto. Premia zależy od ery.'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tradycja Apostolska|Tradycji Apostolskiej|Tradycji Apostolskiej|Tradycję Apostolską|Tradycją Apostolską|Tradycji Apostolskiej', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_PILGRIMAGE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- World Church (Now Theocratic Rule)
UPDATE Language_pl_PL
SET Text = 'Odblokowuje cud narodowy Wielkie Ossuarium (+10 [ICON_PEACE] wiary; odblokowuje wierzenie reformacyjne). Dzień uwielbienia dla króla daje premię do [ICON_PEACE] wiary, [ICON_CULTURE] kultury, [ICON_GOLD] złota i [ICON_RESEARCH] nauki wynoszącą 15%.'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Teokracja|Teokracji|Teokracji|Teokrację|Teokracją|Teokracji', Gender = 'feminine', Plurality = '1'
WHERE Tag = 'TXT_KEY_BELIEF_WORLD_CHURCH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Belief Tooltips for Building beliefs

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CATHEDRAL_TOOLTIP', '+3 [ICON_PEACE] wiary[NEWLINE]+3 [ICON_GOLD]Gold[NEWLINE1 miejsce na [ICON_GREAT_WORK] wielkie dzieło sztuki[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CATHEDRAL_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_PAGODA_TOOLTIP', '+2 [ICON_PEACE] wiary[NEWLINE]+2 [ICON_CULTURE] kultury[NEWLINE]1 miejsce na [ICON_GREAT_WORK] wielkie dzieło sztuki[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_PAGODA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MOSQUE_TOOLTIP', '+3 [ICON_PEACE] wiary[NEWLINE]+3 [ICON_RESEARCH] nauki[NEWLINE]1 miejsce na [ICON_GREAT_WORK] wielkie dzieło literackie[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MOSQUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_STUPA_TOOLTIP', '+5 [ICON_PEACE] wiary[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_STUPA_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_SYNAGOGUE_TOOLTIP', '+2 [ICON_PEACE] wiary[NEWLINE]+3 [ICON_PRODUCTION] produkcji[NEWLINE]1 miejsce na [ICON_GREAT_WORK] wielkie dzieło literackie[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_SYNAGOGUE_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_MANDIR_TOOLTIP', '+2 [ICON_PEACE] wiary[NEWLINE]+5% [ICON_FOOD] żywności[NEWLINE]1 miejsce na [ICON_GREAT_WORK] wielkie dzieło muzyczne[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_MANDIR_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_CHURCH_TOOLTIP', '+3 [ICON_PEACE] wiary[NEWLINE]1 miejsce na [ICON_GREAT_WORK] wielkie dzieło muzyczne[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_CHURCH_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
	SELECT 'TXT_KEY_BUILDING_ORDER_TOOLTIP', '+2 [ICON_PEACE] wiary[NEWLINE][NEWLINE]{TXT_KEY_BUILDING_ORDER_HELP}'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
