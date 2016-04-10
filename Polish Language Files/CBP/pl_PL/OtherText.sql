--translated by mrlg

	UPDATE Language_pl_PL
	SET Text = '[ICON_TOURISM] Turystyka roœnie o 50% na 20 tur. Darmowy ustrój spo³eczny.'
	WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = '[ICON_CULTURE] Kultura wiêksza o 33z% przez 20 tur..'
	WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- CS Text Change
	UPDATE Language_pl_PL
	SET Text = 'Dawanie du¿e daniny'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_UNIT_RELUCTANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Aesthetics Change
	UPDATE Language_pl_PL
	SET Text = 'Dziêki ulepszeniu w postaci wykopalisk archeologicznych, ukryte œlady przesz³oœci mog¹ wydaæ artefakt, który mo¿na umieœciæ w miejscu na wielkie dzie³o sztuki lub zmieniæ w s³ynne miejsce. Ukryte œlady przesz³oœci widoczne s¹ tylko dla cywilizacji, które odblokowa³y wszystkie ustroje w drzewku Estetyki.'
	WHERE Tag = 'TXT_KEY_RESOURCE_HIDDEN_ARTIFACTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Help Text Changes
	UPDATE Language_pl_PL
	SET Text = '[ICON_TOURISM] turystyka jest podstawowym uzyskiem, którego bêdziesz u¿ywaæ do szerzenia twoich wp³ywów kulturowych wœród innych cywilizacji. Jest ona generowana biernie przez [COLOR_POSITIVE_TEXT]wielkie dzie³a[ENDCOLOR] lub [COLOR_POSITIVE_TEXT]artefakty[ENDCOLOR, i czynnie przez [COLOR_POSITIVE_TEXT]wydarzenia historyczne[ENDCOLOR] (wiêcej w sekcji "Wydarzenia historyczne"). Gdy umieszczasz coraz wiêcej wielkich dzie³ i artefaktów w swoich muzeach, amfiteatrach, operach itd., twoja [ICON_TOURISM] turystyka roœnie. Ponadto mo¿esz zwiêkszyæ generowan¹ [ICON_TOURISM] turystykê dziêki otwartym granicom, szlakom handlowym, wspólnym religiom i pewnym zasadom ideologicznym.'
	WHERE Tag = 'TXT_KEY_CULTURE_TOURISM_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Capture Popup

	UPDATE Language_pl_PL
	SET Text = 'Miasto zamienione w [COLOR_POSITIVE_TEXT]marionetkowe[ENDCOLOR] bêdzie wytwarzaæ dla ciebie z³oto, naukê, kulturê itd. lecz [COLOR_POSITIVE_TEXT]nie mo¿esz wybraæ, co wytwarza[ENDCOLOR] ani dostosowaæ go do swoich potrzeb.[NEWLINE][NEWLINE]Bêdzie wnosiæ znacznie mniej [ICON_HAPPINESS_4] niezadowolenia ni¿ miasto przy³¹czone i nie bêdzie zwiêkszaæ kosztu polityki spo³ecznej oraz wielkich ludzi. Jeœli siê na to zdecydujesz, mo¿esz póŸniej przy³¹czyæ miasto w dowolnej chwili.'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_PUPPET' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[COLOR_POSITIVE_TEXT]Przy³¹czenie[ENDCOLOR] miasta do twojego imperium uczyni je normalnym miastem, które bêdzie jednak wytwarzaæ nieco wiêcej [ICON_HAPPINESS_4] niezadowolenia (dopóki nie zbudujesz s¹du).[NEWLINE][NEWLINE]Natychmiastowe przy³¹czenie miasta (zamiast uczynienia go marionetkowym lub zniszczenia go) daje [COLOR_POSITIVE_TEXT]inwestycjê[ENDCOLOR] w s¹d w mieœcie, redukuj¹c czas jego budowy o co najmniej 50% i [COLOR_POSITIVE_TEXT]obni¿a o po³owê[ENDCOLOR] opór [ICON_RESISTANCE] miasta.'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_ANNEX' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[COLOR_POSITIVE_TEXT]Niszczone[ENDCOLOR] miasto bêdzie [ICON_RAZING] p³on¹æ ka¿dej tury, a¿ jego populacja osi¹gnie 0 i miasto zostanie usuniête z gry. Wytwarza to [ICON_HAPPINESS_4] niezadowolenie równe populacji miasta, ale równie¿ zwiêksza iloœæ twoich [COLOR_POSITIVE_TEXT]punktów wojny[ENDCOLOR] przeciwko temu graczowi.[NEWLINE][NEWLINE]Podczas niszczenia, istnieje du¿e ryzyko pojawienia siê [COLOR_NEGATIVE_TEXT]partyzantów[ENDCOLOR] lojalnych prawowitemu w³aœcicielowi (jeœli wci¹¿ toczysz wojnê z tym graczem).'
	WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Citizen Food Consumption
	UPDATE Language_pl_PL
	SET Text = '-{1_Num} {2_IconString} zjadane przez [ICON_CITIZEN] obywateli'
	WHERE Tag = 'TXT_KEY_YIELD_EATEN_BY_POP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Citizen Food Consumption

	UPDATE Language_pl_PL
	SET Text = 'Pracownicy'
	WHERE Tag = 'TXT_KEY_CITIES_UNEMPLOYEDCITIZENS_HEADING3_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Pracownicy zapewniaj¹ niewielk¹ iloœæ uzysku w porównaniu do innych specjalistów, mo¿esz jednak przydzieliæ na to miejsce tylu mieszkañców, ile pragniesz. Powinni byæ oni, jeœli to mo¿liwe, przydzieleni do pracy na polach gry lub wyszkoleni na specjalistów.'
	WHERE Tag = 'TXT_KEY_SPECIALIST_CITIZEN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Pracownik|Pracownika|Pracownikowi|Pracownika|Pracownicy|Pracowników|Pracownikom|Pracowników'
	WHERE Tag = 'TXT_KEY_SPECIALIST_CITIZEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Pracownicy'
	WHERE Tag = 'TXT_KEY_CITYVIEW_UNEMPLOYED_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- CSs
	UPDATE Language_pl_PL
	SET Text = 'Mo¿na zastraszaæ pañstwa-miasta aby odda³y one du¿e iloœci uzysku, lecz wydzieranie go si³¹ zmniejszy twój [ICON_INFLUENCE] wp³yw na nie.'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- Trade Stuff
	UPDATE Language_pl_PL
	SET Text = 'Nieznane ci technologie odkryte przez {1_CivName[4]}: {2_Num}. Otrzymujesz +{4_Num} [ICON_RESEARCH] nauki.[NEWLINE]+{3_Num} [ICON_RESEARCH] nauki na tym szlaku dziêki twoim kulturowym wp³ywom na nich.'
	WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Nieznane {2_CivName[3]} technologie odkryte przez ciebie: {1_Num}. Otrzymuj¹ +{4_Num} [ICON_RESEARCH] nauki.[NEWLINE]+{3_Num} [ICON_RESEARCH] nauki na tym szlaku dziêki kulturowym wp³ywom na ciebie.'
	WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- CS Gift Stuff

	UPDATE Language_pl_PL
	SET Text = '({1_NumHappiness} [ICON_HAPPINESS_1] zadowolenia)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[COLOR_POSITIVE_TEXT]Kupieckie[ENDCOLOR] pañstwo-miasto oferuje egzotyczne dobra twojemu ludowi (+{1_NumHappiness} do [ICON_HAPPINESS_1] zadowolenia, +{2_z³ota} [ICON_GOLD] z³ota)!'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jako [COLOR_POSITIVE_TEXT]kupieckie[ENDCOLOR] pañstwo-miasto, ich rynki rozpowszechniaj¹ handel egzotycznymi dobrami w twoim imperium! (+{1_NumHappiness} do [ICON_HAPPINESS_1] zadowolenia, +{2_z³ota} [ICON_GOLD] z³ota)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MERCANTILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Bêd¹ wspieraæ ciê mniejsz¹ iloœci¹ jednostek i nauki! ({1_nauki} [ICON_RESEARCH] nauki)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_ALLIES_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Nie otrzymasz od nich wiêcej jednostek i nauki. ({1_nauki} [ICON_RESEARCH] nauki)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_LOST_FRIENDS_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jako [COLOR_POSITIVE_TEXT]militarne[ENDCOLOR] pañstwo-miasto, bêd¹ mogli dostarczaæ ci co jakiœ czas jednostki wojskowe i naukê. (+{1_nauki} [ICON_RESEARCH] nauki)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_FRIENDS_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jako [COLOR_POSITIVE_TEXT]militarne[ENDCOLOR] pañstwo-miasto, bêd¹ regularnie zasilaæ szeregi twoich wojsk nowymi jednostkami. (+{1_nauki} [ICON_RESEARCH] nauki)'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_MINOR_NOW_ALLIES_MILITARISTIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- Change instructions for Ideologies

	UPDATE Language_pl_PL
	SET Text = 'Trzy drzewa ideologii: Swobody, £ad i Autokracja, znacznie powiêkszy³y siê w „Nowym wspania³ym œwiecie”. Wszystkie cywilizacje w grze po ukoñczeniu trzech drzewek ustrojów po epoce renesansu lub osi¹gniêciu epoki wspó³czesnej bêd¹ musia³y wybraæ ideologiê swojego kraju. Ka¿de z drzewek ideologii zawiera trzy poziomy „zasad”, z pomoc¹ których je personalizujesz. Trzeci poziom zawiera zasady najpotê¿niejsze, daj¹ce najwiêksze premie. Tak jak w przypadku zwyk³ych ustrojów spo³ecznych, gracze u¿ywaj¹ kultury, by kupowaæ dodatkowe zasady w miarê postêpów w grze.[NEWLINE][NEWLINE]Cywilizacje o tej samej ideologii otrzymuj¹ premie do stosunków dyplomatycznych. Jeœli ideologie bêd¹ sprzeczne, wyst¹pi¹ ró¿ne efekty uboczne. Przyk³adem mo¿e byæ kara do stosunków dyplomatycznych i zadowolenia, jeœli wroga ideologia ma wiêkszy wp³yw kulturowy na twój lud. Jeœli sprawisz, ¿e twoi poddani bêd¹ zbyt niezadowoleni, miasta mog¹ oderwaæ siê od twego pañstwa i do³¹czyæ do wrogiego imperium. Jeœli wszystko inne zawiedzie, ty i inni gracze mo¿ecie przeprowadziæ rewolucjê, by zmieniæ ideologiê na tak¹, któr¹ wasi mieszkañcy bêd¹ akceptowaæ.[NEWLINE][NEWLINE]Aby dowiedzieæ siê wiêcej o zasadach ideologicznych, kliknij zak³adkê „Ustroje” w górnej czêœci Civilopedii, a nastêpnie przewiñ tekst do sekcji Swobód, £adu i Autokracji.'
	WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
    SET Text = 'Gracze zainteresowani stworzeniem wielkich, rozleg³ych cywilizacji powinni wybraæ ³ad, poniewa¿ si³a ich imperium zale¿eæ bêdzie od liczby miast, które bêdzie ono posiadaæ. £ad staje siê dostêpny po wejœciu w epokê wspó³czesn¹ lub ukoñczeniu trzech drzewek ustrojów po epoce renesansu, zale¿nie co nast¹pi pierwsze'
	WHERE Tag = 'TXT_KEY_SOCIALPOLICY_ORDER_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
    SET Text = 'Ideologia Autokracji nadaje siê dla tych, którzy pragn¹ jedynie zmia¿d¿yæ swoich wrogów stalowymi buciorami. Odblokowuje siê ona po wejœciu w epokê wspó³czesn¹ lub ukoñczeniu trzech drzewek ustrojów po epoce renesansu, zale¿nie co nast¹pi pierwsze'
	WHERE Tag = 'TXT_KEY_SOCIALPOLICY_AUTOCRACY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
    SET Text = 'Ideologia Swobód zapewnia premie do kultury, turystyki, produkcji specjalistów i innych. Wolnoœæ staje siê dostêpna po wejœciu w epokê wspó³czesn¹ lub ukoñczeniu trzech drzewek ustrojów po epoce renesansu, zale¿nie co nast¹pi pierwsze.'
	WHERE Tag = 'TXT_KEY_SOCIALPOLICY_IDEOLOGY_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
    SET Text = 'Twój kraj jest postrzegany jako czêœæ epoki wspó³czesnej i idee modernizacji przenikaj¹ twoje spo³eczeñstwo. Twój lud domaga siê wybrania ideologii twojej cywilizacji.'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_CHOOSE_IDEOLOGY_FACTORIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
    SET Text = 'Zasady ideologiczne nie mog¹ zostaæ kupione, póki nie wybierzesz ideologii. Wybór stanie siê dostêpny, gdy ukoñczysz trzy drzewka ustrojów lub wkroczysz w epokê wspó³czesn¹, zale¿nie co nast¹pi pierwsze.'
	WHERE Tag = 'TXT_KEY_POLICYSCREEN_IDEOLOGY_NOT_STARTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	
UPDATE Language_pl_PL
SET Text = 'Pozwala twoim miastom na budowê bomb atomowych oraz pocisków atomowych.[NEWLINE][NEWLINE]Jeœli ukoñczysz go jako [COLOR_POSITIVE_TEXT]pierwszy[ENDCOLOR], otrzymujesz darmowe laboratorium badawcze i bombê atomow¹ w swojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Projekt Manhattan pozwala cywilizacji na konstruowanie broni j¹drowej. Ka¿da cywilizacja musi ukoñczyæ Projekt Manhattan, zanim bêdzie mog³a robiæ atomówki. Jeœli jesteœ czêœci¹ pierwszej œwiatowej dru¿yny, która go ukoñczy, otrzymasz darmowe laboratorium badawcze i bombê atomow¹ w swojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_PROJECT_MANHATTAN_PROJECT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Oznacza start wyœcigu kosmicznego, pozwalaj¹c twoim miastom na budowê czêœci statku kosmicznego. Gdy statek kosmiczny jest ukoñczony, osi¹gasz [COLOR_POSITIVE_TEXT]zwyciêstwo naukowe[ENDCOLOR]![NEWLINE][NEWLINE]Jeœli ukoñczysz go jako [COLOR_POSITIVE_TEXT]pierwszy[ENDCOLOR], otrzymujesz darmowego [ICON_GREAT_SCIENTIST] wielkiego naukowca obok twojej [ICON_CAPITAL] stolicy i rozpoczyna siê [ICON_GOLDEN_AGE] z³ota era.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Program Apollo stanowi pocz¹tek wyœcigu kosmicznego. Pozwala na konstrukcjê czêœci do statku kosmicznego. Ka¿da z cywilizacji musi ukoñczyæ ten projekt, zanim zacznie konstruowaæ statek kosmiczny.Jeœli jesteœ czêœci¹ pierwszej œwiatowej dru¿yny, która go ukoñczy, otrzymasz darmowego wielkiego naukowca obok twojej stolicy i rozpocznie siê z³ota era.'
WHERE Tag = 'TXT_KEY_PROJECT_APOLLO_PROGRAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rybo³ówstwo|Rybo³ówstwa|Rybo³ówstwu|Rybo³ówstwo', Gender = 'neuter'
WHERE Tag = 'TXT_KEY_TECH_SAILING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rybo³ówstwo to ga³¹Ÿ gospodarki obejmuj¹ca pozyskiwanie (po³ów) ryb. Odbywa siê zwykle w naturalnym œrodowisku. Techniki ³apania ryb obejmuj¹ ³apanie rêk¹, u¿ywanie oszczepów, sieci, wêdek i pu³apek. Termin rybo³ówstwo mo¿e siê te¿ odnosiæ do po³owu innych zwierz¹t oprócz wielorybów (wielorybnictwo), jak miêczaków, g³owonogów, skorupiaków i szkar³upni. Pojêcie zwykle nie ma zastosowania przy pozyskiwaniu ryb hodowlanych.'
WHERE Tag = 'TXT_KEY_TECH_SAILING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sztuka wojenna|Sztuki wojennej|Sztuce wojennej|Sztukê wojenn¹', Gender="feminine"
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sztuka wojenna to ogó³ uporz¹dkowanej i uzasadnionej wiedzy o wojnie, o istocie i charakterze wojen jako zjawisku spo³ecznym oraz o zasadach i sposobach przygotowania pañstwa i si³ zbrojnych do wojny. Równie¿ uzasadniona historycznie i naukowo wiedza o sposobach prowadzenia dzia³añ wojennych. W staro¿ytnoœci zalicza³a siê do filozofii. Teorie i pojêcia wojenne ró¿ni³y siê w zale¿noœci od zak¹tka œwiata i epoki. Chiñczyk Sun Tzu jest uwa¿any przez naukowców za jednego z pierwszych teoretyków wojennych. Jego, obecnie kultowa, Sztuka Wojny k³adzie podstawy pod planowanie operacyjne, taktykê, strategiê i logistykê. W Indiach, Chanakya (350-275r. p.n.e.) przed³o¿y³ podstawy teorii wojskowej w swoim nowatorskim tekœcie Arthaœastra. Podczas gdy podgl¹dy Clausewitza, Sun i Tzu i Kautilyi nie odnosz¹ siê bezpoœrednio do wspó³czesnego pola bitwy, wci¹¿ s¹ cytowane i wykorzystywane przez teoretyków wojennych ze wzglêdu na spostrze¿enia, które dostarczaj¹ i s¹ adaptowane do czasów wspó³czesnych.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]koszary[ENDCOLOR], podstawowy budynku we wczesnych wojnach.'
WHERE Tag = 'TXT_KEY_TECH_ARCHERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Handel|Handlu|Handlowi|Handel', Gender = 'masculine'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Handel to proces gospodarczy polegaj¹cy na sprzeda¿y, czyli na wymianie dóbr i us³ug na pieni¹dze, b¹dŸ na inne towary lub us³ugi (handel wymienny). Proces ten jest realizowany przez zawodowych poœredników w celu osi¹gniêcia zysku. Synonimami dla "handlu" s¹ "biznes" i "transakcja finansowa". Sieæ, która umo¿liwia handel to rynek. Pierwotnym rodzajem handlu by³, wspomniany wczeœniej, handel wymienny, gdy nie istnia³o jeszcze pojêcie pieni¹dza albo nie by³ on w powszechnym u¿ytku. Dzisiejsi handlarze prowadz¹ zwykle negocjacje u¿ywaj¹c œrodka p³atniczego, jakim s¹ w³aœnie pieni¹dze. W rezultacie, mo¿na wyró¿niæ sprzeda¿ i kupno. Wynalezienie pieniêdzy (potem kredytów, pieniêdzy papierowych i nie fizycznych) wielce u³atwi³a i rozpowszechni³o handel. Transakcje pomiêdzy dwoma kupcami s¹ nazywane "dwustronnymi", a pomiêdzy wiêksz¹ ich iloœci¹ "wielostronnymi".'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Umo¿liwia tworzenie [COLOR_POSITIVE_TEXT]konnicy[ENDCOLOR], szybkiej i potê¿nej jednostki kawaleryjskiej. Pozwala te¿ zbudowaæ [COLOR_POSITIVE_TEXT]targowisko[ENDCOLOR], doskona³e wczesne Ÿród³o [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_TECH_HORSEBACK_RIDING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '¯egluga to sztuka ujarzmiania si³y wiatru w celu poruszania ³odzi po wodzie. ¯eglowanie wymaga opanowania wielu ró¿nych umiejêtnoœci. Po pierwsze, dana kultura musi zbudowaæ statek morski. Po drugie, musi mieæ mo¿liwoœæ wytwarzania wytrzyma³ych tkanin (¿agli) ³api¹cych wiatr i przenosz¹cych jego energiê na kad³ub. Po trzecie, niezbêdna jest umiejêtnoœæ produkcji ró¿nych lin, bloków i zacisków, s³u¿¹cych do regulowania ustawienia ¿agli okrêtu. Musi te¿ umieæ nawigowaæ statkiem od punktu od punktu, nie gubi¹c przy tym drogi i nie rozbijaj¹c siê.[NEWLINE][NEWLINE]Pierwsze pojazdy wodne wystêpuj¹ na egipskich rysunkach powsta³ych 4000 lat p.n.e. Egipcjanie, jako cywilizacja nadbrze¿na, byli doskona³ymi ¿eglarzami. Wiele z ich okrêtów posiada³o wios³a i ¿agle, przy czym z tych pierwszych korzystano, gdy wiatr by³ zbyt s³aby lub wia³ z niedogodnego kierunku.[NEWLINE][NEWLINE]Oko³o roku 3000 p.n.e. Egipcjanie podró¿owali po Morzu Œródziemnym, kieruj¹c siê na Kretê, a póŸniej do Fenicji. ¯eglowali te¿ wzd³u¿ wybrze¿a Afryki w poszukiwaniu wiedzy, skarbów i mo¿liwoœci handlu.[NEWLINE][NEWLINE]Pierwsze okrêty wojenne – biremy, triremy i im podobne – porusza³y siê dziêki wios³om i ¿aglom, a na ich dziobach montowano tarany. Podczas bitwy sternik stara³ siê z du¿¹ prêdkoœci¹ staranowaæ nieprzyjacielsk¹ jednostkê, jednoczeœnie unikaj¹c wrogów staraj¹cych siê zrobiæ to samo. Na niektórych okrêtach znajdowali siê ³ucznicy, którzy ostrzeliwali przeciwnika, a niektóre przewozi³y ¿o³nierzy – te jednostki stara³y siê dobiæ do burty wroga, aby ich ¿o³nierze mogli przej¹æ okrêt si³¹.[NEWLINE][NEWLINE]Grecy – w szczególnoœci Ateñczycy i kolonie wyspiarskie – byli mistrzami wojny na morzu. Ateñczycy pokonali o wiele potê¿niejsz¹ Persjê miêdzy innymi dlatego, ¿e ich flota opanowa³a Morze Egejskie i stanowi³a zagro¿enie dla rozci¹gniêtych linii zaopatrzeniowych Persów.'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '¯egluga|¯eglugi|¯egludze|¯eglugê'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]wiatrak[ENDCOLOR] w miastach wzniesionych na p³askim terenie, co zwiêksza [ICON_PRODUCTION] produkcjê.'
WHERE Tag = 'TXT_KEY_TECH_ECONOMICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Umo¿liwia ci wzniesienie Krzywej Wie¿y w Pizie i stanowi wrota do istotnych technologii renesansu.'
WHERE Tag = 'TXT_KEY_TECH_CHEMISTRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala na konstrukcjê [COLOR_POSITIVE_TEXT]dzia³[ENDCOLOR], potê¿nych renesansowych jednostek oblê¿niczych, i  [COLOR_POSITIVE_TEXT]arsena³u[ENDCOLOR] zwiêkszaj¹cego wspó³czynnik obrony miasta.'
WHERE Tag = 'TXT_KEY_TECH_METALLURGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]fabrykê[ENDCOLOR], budynek, który znacz¹co zwiêksza [ICON_PRODUCTION] produkcjê w mieœcie.'
WHERE Tag = 'TXT_KEY_TECH_INDUSTRIALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
-- Fix Steam Power Text

UPDATE Language_pl_PL
SET Text = 'Ods³ania na mapie [ICON_RES_COAL] [COLOR_POSITIVE_TEXT]wêgiel[ENDCOLOR], podstawowy surowiec epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_TECH_STEAM_POWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rifling Text
UPDATE Language_pl_PL
SET Text = 'Pozwala stworzyæ [COLOR_POSITIVE_TEXT]fizylierów[ENDCOLOR], frontow¹ jednostkê œrodkowej fazy gry.'
WHERE Tag = 'TXT_KEY_TECH_RIFLING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Replaceable Parts Text
UPDATE Language_pl_PL
SET Text = 'Pozwala stworzyæ [COLOR_POSITIVE_TEXT]strzelców[ENDCOLOR], podstawow¹ jednostkê l¹dow¹, i zbudowaæ [COLOR_POSITIVE_TEXT]bazê wojskow¹[ENDCOLOR], zwiêkszaj¹c¹ si³ê obrony miasta.'
WHERE Tag = 'TXT_KEY_TECH_REPLACEABLE_PARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combustion Text

UPDATE Language_pl_PL
SET Text = 'Pozwala na budowê potê¿nych [COLOR_POSITIVE_TEXT]okrêtów l¹dowych[ENDCOLOR], niesamowicie potê¿nych opancerzonych jednostek, które mog¹ wykonaæ ruch po ataku.'
WHERE Tag = 'TXT_KEY_TECH_COMBUSTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Plastics Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia budowê [COLOR_POSITIVE_TEXT]laboratorium badawczego[ENDCOLOR], budynku zwiêkszaj¹cego [ICON_RESEARCH] naukê w mieœcie.'
WHERE Tag = 'TXT_KEY_TECH_PLASTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Rocketry Text
UPDATE Language_pl_PL
SET Text = 'Technika rakietowa pozwala stworzyæ [COLOR_POSITIVE_TEXT]artyleriê rakietow¹[ENDCOLOR] i [COLOR_POSITIVE_TEXT]wyrzutnie SAM[ENDCOLOR], szybkie jednostki s³u¿¹ce do zwalczania wrogiego lotnictwa.'
WHERE Tag = 'TXT_KEY_TECH_ROCKETRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Satellites Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia rozpoczêcie [COLOR_POSITIVE_TEXT]programu Apollo[ENDCOLOR], projektu niezbêdnego do [COLOR_POSITIVE_TEXT]zwyciêstwa naukowego[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_SATELLITES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Advanced Ballistics Text
UPDATE Language_pl_PL
SET Text = 'Pozwala na budowê przera¿aj¹cych [COLOR_POSITIVE_TEXT]pocisków atomowych[ENDCOLOR], których konstrukcja wymaga [ICON_RES_URANIUM]uranu. Jest to broñ zdolna zniszczyæ ka¿d¹ jednostkê i zrównaæ z ziemi¹ ca³e miasta.'
WHERE Tag = 'TXT_KEY_TECH_ADVANCED_BALLISTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Globalization Text
UPDATE Language_pl_PL
SET Text = 'Pozwala skonstruowaæ [COLOR_POSITIVE_TEXT]kabinê SK[ENDCOLOR], jedn¹ z czêœci statku kosmicznego niezbêdn¹ do [COLOR_POSITIVE_TEXT]zwyciêstwa naukowego[ENDCOLOR]. Po odkryciu globalizacji ka¿dy szpieg, którego wyœlesz do innej cywilizacji jako [ICON_DIPLOMAT] dyplomatê, zwiêkszy iloœæ kontrolowanych przez ciebie delegatów w Kongresie Œwiatowym, zbli¿aj¹c ciê do [COLOR_POSITIVE_TEXT]zwyciêstwa dyplomatycznego[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_GLOBALIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Computers Text
UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Pozwala na budowê [COLOR_POSITIVE_TEXT]lotniskowców[ENDCOLOR], które mog¹ transportowaæ jednostki lotnicze'
WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Nuclear Fusion Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia zbudowanie [COLOR_POSITIVE_TEXT]wielkiego œmierciobota[ENDCOLOR], najpotê¿niejszej jednostki w grze. Pozwala te¿ wyprodukowaæ [COLOR_POSITIVE_TEXT]dopalacz SK[ENDCOLOR], jedn¹ z czêœci statku kosmicznego potrzebn¹ do [COLOR_POSITIVE_TEXT]zwyciêstwa naukowego[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NUCLEAR_FUSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Telecom Text
UPDATE Language_pl_PL
SET Text = 'Pozwala na zbudowanie [COLOR_POSITIVE_TEXT]atomowego okrêtu podwodnego[ENDCOLOR], jednostki morskiej niewidocznej dla wiêkszoœci innych wojsk i mog¹cej przenosiæ 2 g³owice.'
WHERE Tag = 'TXT_KEY_TECH_TELECOM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Electronics Text
UPDATE Language_pl_PL
SET Text = 'Pozwala na budowê [COLOR_POSITIVE_TEXT]pancernika[ENDCOLOR] - potê¿nej jednostki morskiej z dalszych etapów gry.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRONICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Combined Arms Text
UPDATE Language_pl_PL
SET Text = 'Pozwala na zbudowanie [COLOR_POSITIVE_TEXT]czo³gów[ENDCOLOR], szybkich i œmiercionoœnych jednostek pancernych.'
WHERE Tag = 'TXT_KEY_TECH_COMBINED_ARMS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Refrigeration Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia zbudowanie [COLOR_POSITIVE_TEXT]stadionu[ENDCOLOR], zwiêkszaj¹cego [ICON_HAPPINESS_1] zadowolenie w mieœcie. Pozwala te¿ kutrom na budowê platform wiertniczych.'
WHERE Tag = 'TXT_KEY_TECH_REFRIGERATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Flight Text
UPDATE Language_pl_PL
SET Text = 'Pozwala na budowê [COLOR_POSITIVE_TEXT]trójp³atów[ENDCOLOR], bêd¹cych myœliwcami przewagi powietrznej, oraz [COLOR_POSITIVE_TEXT]bombowców[ENDCOLOR], które œwietnie nadaj¹ siê do atakowania jednostek l¹dowych i miast.'
WHERE Tag = 'TXT_KEY_TECH_FLIGHT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Radar Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia wyprodukowanie [COLOR_POSITIVE_TEXT]ciê¿kiego bombowca[ENDCOLOR], zabójczej jednostki powietrznej siej¹cej zniszczenie z du¿ej odleg³oœci. Ponadto pozwala na stworzenie [COLOR_POSITIVE_TEXT]myœliwców[ENDCOLOR], jednostki powietrznej przeznaczonej do zdobywania panowania w powietrzu, i [COLOR_POSITIVE_TEXT]spadochroniarzy[ENDCOLOR], jednostki piechoty z póŸniejszych etapów gry, mog¹cej l¹dowaæ na spadochronach za liniami wroga.'
WHERE Tag = 'TXT_KEY_TECH_RADAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Compass Text
UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]przystañ[ENDCOLOR], tworz¹c¹ morskie po³¹czenia miast ze stolic¹, generuj¹ce [ICON_GOLD] z³oto. Ponadto roœnie iloœæ [ICON_GOLD] ¿ywnoœci generowanej przez ³odzie rybackie.'
WHERE Tag = 'TXT_KEY_TECH_COMPASS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Metal Casting Text
UPDATE Language_pl_PL
SET Text = 'Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]³aŸniê[ENDCOLOR], które zwiêkszaj¹ tempo, w jakim pojawiaj¹ siê [ICON_GREAT_PEOPLE] wielcy ludzie.'
WHERE Tag = 'TXT_KEY_TECH_METAL_CASTING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Construction Text
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia zbudowanie [COLOR_POSITIVE_TEXT]areny[ENDCOLOR], które zwiêksza [ICON_HAPPINESS_1] zadowolenie w imperium, co z kolei u³atwia powiêkszanie miast i zwiêksza szanse na [ICON_GOLDEN_AGE] z³ote ery.'
WHERE Tag = 'TXT_KEY_TECH_CONSTRUCTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Wheel Text
UPDATE Language_pl_PL
SET Text = 'Pozwala stworzyæ [COLOR_POSITIVE_TEXT]strzelców rydwanowych[ENDCOLOR], szybk¹ i siln¹ jednostkê strzelaj¹c¹, potrzebuj¹c¹ [ICON_RES_HORSE] koni. Ponadto robotnicy mog¹ budowaæ [COLOR_POSITIVE_TEXT]drogi[ENDCOLOR], pozwalaj¹ce jednostkom szybciej przemieszczaæ siê po mapie i przynosz¹ce dodatkowe [ICON_GOLD] z³oto, gdy ³¹cz¹ twoje miasta ze stolic¹.'
WHERE Tag = 'TXT_KEY_TECH_THE_WHEEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Fix Currency Text

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Umo¿liwia zbudowanie [COLOR_POSITIVE_TEXT]karawanserajów[ENDCOLOR], które znacz¹co zwiêkszaj¹ zasiêg i wartoœæ twoich l¹dowych [ICON_INTERNATIONAL_TRADE] szlaków handlowych. Ponadto robotnicy mog¹ budowaæ [COLOR_POSITIVE_TEXT]wioski[ENDCOLOR], które zwiêkszaj¹ iloœæ otrzymywanego z pól mapy [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_TECH_CURRENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Other Text Fixes
UPDATE Language_pl_PL
SET Text = 'Umo¿liwia stworzenie [COLOR_POSITIVE_TEXT]katapult[ENDCOLOR], potê¿nej broni oblê¿niczej, i [COLOR_POSITIVE_TEXT]³uczników konnych[ENDCOLOR], siln¹ strzelaj¹c¹ jednostkê kawaleryjsk¹.'
WHERE Tag = 'TXT_KEY_TECH_MATHEMATICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]œwi¹tyniê[ENDCOLOR], która zwiêksza iloœæ [ICON_PEACE] wiary generowanej przez miasto. Ponadto pozwala zbudowaæ [COLOR_POSITIVE_TEXT]s¹d[ENDCOLOR], budynek zmniejszaj¹cy [ICON_HAPPINESS_4] niezadowolenie wywo³ane przez [ICON_OCCUPIED] okupowane miasta.'
WHERE Tag = 'TXT_KEY_TECH_PHILOSOPHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Umo¿liwia robotnikom budowanie [COLOR_POSITIVE_TEXT]kolei[ENDCOLOR] na polach mapy. £¹cz¹c ni¹ miasta zwiêkszysz [ICON_PRODUCTION] produkcjê, a jednostki bêd¹ siê po niej szybciej poruszaæ.'
WHERE Tag = 'TXT_KEY_TECH_RAILROAD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala te¿ robotnikom budowaæ [COLOR_POSITIVE_TEXT]pastwiska[ENDCOLOR] na polach z [ICON_RES_COW] [COLOR_POSITIVE_TEXT]byd³em[ENDCOLOR] i [ICON_RES_SHEEP] [COLOR_POSITIVE_TEXT]owcami[ENDCOLOR]. Odkrywa równie¿ [ICON_RES_HORSE] konie, które s¹ wykorzystywane do produkcji potê¿nych jednostek konnych.'
WHERE Tag = 'TXT_KEY_TECH_ANIMAL_HUSBANDRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Umo¿liwia robotnikom budowanie [COLOR_POSITIVE_TEXT]kopalñ[ENDCOLOR], zwiêkszaj¹cych [ICON_PRODUCTION] produkcjê na polach mapy.'
WHERE Tag = 'TXT_KEY_TECH_MINING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Odkrywa na mapie [ICON_RES_IRON] [COLOR_POSITIVE_TEXT]¿elazo[ENDCOLOR]. Pozwala równie¿ szkoliæ [COLOR_POSITIVE_TEXT]w³óczników[ENDCOLOR], jednostki wojskowe szczególnie u¿yteczne przeciwko przeciwnikom konnym.'
WHERE Tag = 'TXT_KEY_TECH_BRONZE_WORKING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala wznosiæ [COLOR_POSITIVE_TEXT]mury[ENDCOLOR], poprawiaj¹ce mo¿liwoœci obronne miast. Ponadto umo¿liwia robotnikom tworzenie kamienio³omów na [ICON_RES_MARBLE] [COLOR_POSITIVE_TEXT]marmurze[ENDCOLOR] i [ICON_RES_STONE] [COLOR_POSITIVE_TEXT]kamieniu[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_MASONRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Pozwala ci na zbudowanie [COLOR_POSITIVE_TEXT]karawany[ENDCOLOR] u¿ywanej do tworzenia lukratywnych szlaków handlowych i [COLOR_POSITITVE_TEXT]osadników[ENDCOLOR], jednostki niezbêdnej do powiêkszania twojego imperium. Pozwala te¿ twoim miastom budowaæ [COLOR_POSITIVE_TEXT]spichlerz[ENDCOLOR], dostarczaj¹cy [ICON_FOOD] ¿ywnoœæ, dziêki czemu twoje miasta szybciej siê powiêkszaj¹.'
WHERE Tag = 'TXT_KEY_TECH_POTTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala zbudowaæ [COLOR_POSITIVE_TEXT]urzêdy celne[ENDCOLOR] i [COLOR_POSITIVE_TEXT]Kompaniê Wschodnioindyjsk¹[ENDCOLOR], polepszaj¹ce pozyskiwanie [ICON_GOLD] z³ota w twoich miastach.'
WHERE Tag = 'TXT_KEY_TECH_GUILDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Umo¿liwia jednostkom l¹dowym [COLOR_POSITIVE_TEXT]okrêtowanie[ENDCOLOR] na pola z wod¹ i przemieszczanie siê wzd³u¿ wybrze¿a. Pozwala te¿ budowaæ statki handlowe przydatne w eksploracji i pozyskiwaniu z wody zasobów, takich jak [ICON_RES_FISH] [COLOR_POSITIVE_TEXT]ryby[ENDCOLOR] i [ICON_RES_PEARLS].'
WHERE Tag = 'TXT_KEY_TECH_SAILING_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ponadto pozwala zbudowaæ [COLOR_POSITIVE_TEXT]latarniê morsk¹[ENDCOLOR] w miastach nadmorskich, co zwiêksza produkcjê [ICON_FOOD] ¿ywnoœci na polach z wod¹ i przyspiesza ich rozwój. Zapewnia ci te¿ dostêp do [COLOR_POSITIVE_TEXT]statków towarowych[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_OPTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala na budowê [COLOR_POSITIVE_TEXT]obserwatorium[ENDCOLOR], które znacznie zwiêksza poziom [ICON_RESEARCH] nauki. Dziêki tej technologii zaokrêtowane jednostki mog¹ poruszaæ siê po heksach oceanicznych.'
WHERE Tag = 'TXT_KEY_TECH_ASTRONOMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Umo¿liwia wybudowanie [COLOR_POSITIVE_TEXT]fregat[ENDCOLOR] i [COLOR_POSITIVE_TEXT]korwet[ENDCOLOR], potê¿nych okrêtów wojennych epoki renesansu. Pozwala tak¿e na wzniesienie [COLOR_POSITIVE_TEXT]portów morskich[ENDCOLOR] zapewniaj¹cych [ICON_PRODUCTION] produkcjê z surowców morskich.'
WHERE Tag = 'TXT_KEY_TECH_NAVIGATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. Odkrywa [ICON_RES_ALUMINUM] aluminium, surowiec wykorzystywany do produkcji wielu jednostek w póŸniejszych etapach gry. Ponadto pozwala budowaæ w miastach [COLOR_POSITIVE_TEXT]gie³dê papierów wartoœciowych[ENDCOLOR], budowlê zwiêkszaj¹c¹ przychód w [ICON_GOLD] z³ocie. Pozwala te¿ postawiæ [COLOR_POSITIVE_TEXT]elektrowniê wodn¹[ENDCOLOR], zwiêkszaj¹c¹ [ICON_PRODUCTION] produkcjê na polach znajduj¹cych siê przy rzece.'
WHERE Tag = 'TXT_KEY_TECH_ELECTRICITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Penicilin and Nanotech 

UPDATE Language_pl_PL
SET Text = 'Wszyscy robotnicy otrzymuj¹ awans [COLOR_POSITIVE_TEXT]Odpornoœæ na radioaktywnoœæ[ENDCOLOR], który redukuje iloœæ otrzymywanych obra¿eñ podczas oczyszczania opadu radioaktywnego. Pozwala na werbowanie [COLOR_POSITIVE_TEXT]piechoty morskiej[ENDCOLOR], która œwietnie nadaje siê do ataków desantowych. Umo¿liwia tak¿e zbudowanie [COLOR_POSITIVE_TEXT]laboratorium medycznego[ENDCOLOR], które przyspiesza przyrost [ICON_FOOD] ¿ywnoœci w twoich miastach.'
WHERE Tag = 'TXT_KEY_TECH_PENICILIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wszyscy robotnicy otrzymuj¹ awans [COLOR_POSITIVE_TEXT]Niewra¿liwoœæ na radioaktywnoœæ[ENDCOLOR], który niweluje otrzymywane obra¿enia podczas oczyszczania opadu radioaktywnego. Pozwala na wyszkolenie [COLOR_POSITIVE_TEXT]oddzia³u XCOM[ENDCOLOR] oraz budowê [COLOR_POSITIVE_TEXT]komory statycznej SK[ENDCOLOR], która jest integralnym elementem statku kosmicznego wymaganym do odniesienia [COLOR_POSITIVE_TEXT]zwyciêstwa naukowego[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_TECH_NANOTECHNOLOGY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kto wie, co kryje przysz³oœæ?[NEWLINE][NEWLINE]Powtarzaj¹ca siê technologia, która powiêkszy twój wynik i wytworzy 10 [ICON_HAPPINESS_1] zadowolenia w twojej stolicy po ka¿dym wynalezieniu.'
WHERE Tag = 'TXT_KEY_TECH_FUTURE_TECH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
