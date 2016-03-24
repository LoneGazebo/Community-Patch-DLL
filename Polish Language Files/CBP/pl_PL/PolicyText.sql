--translated by mrlg
-- Clausewitz's Legacy

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Duch wojenny[ENDCOLOR][NEWLINE]+25% premii do ataku przez 50 tur. [ICON_HAPPINESS_3] Stopieñ zmêczenia wojn¹ zmniejszony o 50% i podwojona prêdkoœæ [ICON_RAZING]burzenia.'
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Duch wojenny'
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = "Duch wojenny reprezentuje dominacjê rywalizacji i sportów walki w spo³eczeñstwie. Dziœ, sportowcy walcz¹ czêsto jeden na jednego, ale wci¹¿ mog¹ u¿ywaæ rozmaitych zestawów umiejêtnoœci, jak ataki w boksie, które mog¹ byæ wyprowadzane tylko piêœci¹, taekwondo, które opiera siê na uderzeniach rêk¹ i kopniêciach czy muay thai i birmañski boks, które zezwalaj¹ równie¿ na u¿ywanie ³okci i kolan. S¹ te¿ sporty oparte na chwytach, w których koncentruje siê na uzyskaniu lepszej pozycji jak w freestyle\'u czy zapasach amerykañskich, gdzie wykorzystuje siê rzuty czy w judo i zapasach greko-romañskich, gdzie dominuje walka w parterze jak w brazylijskim jiu-jitsu. Wspó³czesne zawody w mieszanych sztukach walki s¹ podobne do greckiego sportu olimpijskiego - pankrationu i zezwalaj¹ na szeroki zestaw zarówno technik uderzeñ jak i chwytów. Sporty walki mog¹ te¿ wykorzystywaæ broñ i sportowcy rywalizuj¹ u¿ywaj¹c broni, takich jak rodzaje mieczy w szermierce zachodniej (floret, szpada, szabla) i kendo (shinai). Mog¹ byæ w nich te¿ wykorzystywane z³o¿one uzbrojenie tak jak w SCA Heavy Combat i kendo. W Gatce i Wspó³czesnym Arnisie u¿ywa siê patyki, reprezentuj¹ce czasem no¿e i miecze."
WHERE Tag = 'TXT_KEY_POLICY_NEW_ORDER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cult of Personality

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Kult jednostki[ENDCOLOR][NEWLINE]+50% turystyki [ICON_TOURISM] wzglêdem cywilizacji walcz¹cych ze wspólnym wrogiem. [ICON_GREAT_PEOPLE] Wielki cz³owiek, jakiego wybierzesz pojawia siê obok twojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_POLICY_CULT_PERSONALITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Elite Forces

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wojska specjalne[ENDCOLOR][NEWLINE]Nowo stworzone jednostki wojskowe otrzymuj¹ +15 punktów doœwiadczenia. Jednostki wojskowe dostaj¹ 50% wiêcej doœwiadczenia z walki.'
WHERE Tag = 'TXT_KEY_POLICY_ELITE_FORCES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Fortified Borders

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Nowy porz¹dek œwiata[ENDCOLOR][NEWLINE]Redukuje [ICON_HAPPINESS_3] przestêpczoœæ we wszystkich miastach o 20%. Posterunki policji i policja okrêgowa zapewnia +3 [ICON_CULTURE] kultury i +5 [ICON_PRODUCTION] produkcji.'
WHERE Tag = 'TXT_KEY_POLICY_FORTIFIED_BORDERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Nowy porz¹dek œwiata'
WHERE Tag = 'TXT_KEY_POLICY_FORTIFIED_BORDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Futurism

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Futuryzm[ENDCOLOR][NEWLINE][ICON_TOURISM] Premia do turystyki z wydarzeñ historycznych zwiêkszona o 20%.'
WHERE Tag = 'TXT_KEY_POLICY_FUTURISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Industrial Espionage (now Lebensraum)
UPDATE Language_pl_PL
SET Text = 'Lebensraum'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Lebensraum[ENDCOLOR][NEWLINE]Otrzymujesz [ICON_CULTURE] kulturê i punkty [ICON_GOLDEN_AGE] z³otej ery, kiedy rozszerzaj¹ siê twoje granice. Podwojony promieñ pól pozyskiwanych przez cytadelê.'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Lebensraum (z niemieckiego "siedlisko" albo dos³ownie "przestrzeñ ¿yciowa") by³o ideologi¹ proponuj¹c¹ agresywn¹ ekspansjê Niemiec i ludu niemieckiego. Rozwiniêta w Imperium Niemieckim sta³a siê jednym z celów Niemiec podczas Pierwszej Wojny Œwiatowej a póŸniej zosta³a zaadoptowana jako wa¿na czêœæ ideologii nazistowskiej.'
WHERE Tag = 'TXT_KEY_POLICY_INDUSTRIAL_ESPIONAGE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Lightning Warfare

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]]Wojna b³yskawiczna[ENDCOLOR][NEWLINE]+3 [ICON_MOVES] ruchu dla wielkich genera³ów. Jednostki pancerne i strzelnicze zyskuj¹ +15% ataku, +1 [ICON_MOVES] ruchu i ignoruj¹ wrog¹ strefê kontroli.'
WHERE Tag = 'TXT_KEY_POLICY_LIGHTNING_WARFARE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Militarism
UPDATE Language_pl_PL
SET Text = 'Przewaga powietrzna'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Przewaga powietrzna[ENDCOLOR][NEWLINE]Uzyskujesz darmowe lotnisko w ka¿dym mieœcie. +25% [ICON_PRODUCTION] do produkcji jednostek powietrznych. Umo¿liwia budowê [COLOR_POSITIVE_TEXT]Zero[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przewaga powietrzna to sytuacja podczas wojny, gdzie jedna ze stron utrzymuje ca³kowit¹ kontrolê na walk¹ i si³ami powietrznymi nad przeciwnikiem. Jest definiowana przez NATO i Ministerstwo Obrony Stanów Zjednoczonych jako "taki poziom dominacji w bitwie powietrznej jednej ze stron nad drug¹, i¿ umo¿liwia przeprowadzenie operacji na l¹dzie, morzu i przez si³y powietrzne w danym czasie i miejscu bez wyraŸnej ingerencji ze strony przeciwnika".'
WHERE Tag = 'TXT_KEY_POLICY_MILITARISM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Mobilization
UPDATE Language_pl_PL
SET Text = 'Kompleks wojskowo-przemys³owy'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Kompleks wojskowo-przemys³owy[ENDCOLOR][NEWLINE]-33% [ICON_GOLD] z³ota podczas kupna/ulepszania jednostek. +3 [ICON_RESEARCH] nauki z [ICON_STRENGTH] budynków obronnych, cytadel i unikalnych ulepszeñ.'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kompleks wojskowo-przemys³owy pozwi¹zuje zwi¹zki polityczne i pieniê¿ne, które istniej¹ pomiêdzy prawodawcami, narodowymi si³ami zbrojnymi i przemys³em zbrojeniowym, który je wspiera. Te zwi¹zki zawieraj¹ udzia³ polityków, przyzwolenie polityczne na wydatki militarne, lobbing do wspierania biurokracji i nadzór nad przemys³em.Jest to rodzaj ¿elaznego trójk¹ta. Termin odnosi siê czêsto do systemu stoj¹cego za wojskiem Stanów Zjednoczonych, gdzie zyska³ popularnoœæ po jego przedstawieniu w po¿egnalnym orêdziu prezydenta Dwighta D. Eisenhowera 17 grudnia, 1961 roku, chocia¿ jest w³aœciwy dla ka¿dego kraju z podobnie rozwiniêt¹ infrastruktur¹'
WHERE Tag = 'TXT_KEY_POLICY_MOBILIZATION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Police State

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Pañstwo policyjne[ENDCOLOR][NEWLINE]+3 [ICON_HAPPINESS_1] lokalnego zadowolenia za ka¿dy gmach s¹du i +1 [ICON_HAPPINESS_1] zadowolenia posterunków policji. Budowa s¹du trwa o po³owê krócej.'
WHERE Tag = 'TXT_KEY_POLICY_POLICE_STATE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Third Alternative
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Trzecia droga[ENDCOLOR][NEWLINE]Iloœæ produkowanych zasobów strategicznych roœnie o 100%. +10 do wszystkich zbiorów w [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_POLICY_THIRD_ALTERNATIVE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Total War
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wojna totalna[ENDCOLOR][NEWLINE]+25% [ICON_PRODUCTION] produkcji podczas tworzenia jednostek wojskowych a robotnicy buduj¹ ulepszenia 25% szybciej.'
WHERE Tag = 'TXT_KEY_POLICY_TOTAL_WAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- United Front

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wspólny front[ENDCOLOR][NEWLINE]W przypadku walki ze wspólnym wrogiem militarne pañstwo-miasto zapewnia jednostki z dwukrotnie wiêksz¹ czêstotliwoœci¹. +30 [ICON_INFLUENCE] wp³ywu z podarowywania jednostek pañstwom-miastom.'
WHERE Tag = 'TXT_KEY_POLICY_UNITED_FRONT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Autarky

-- Arsenal of Democracy

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Arsena³ demokracji[ENDCOLOR][NEWLINE]+15% do [ICON_PRODUCTION] produkcji jednostek militarnych. +10 [ICON_INFLUENCE] wp³ywów we wszystkich znanych pañstwach-miastach, kiedy poœwiêcasz [ICON_GREAT_PEOPLE] wielk¹ osobê.'
WHERE Tag = 'TXT_KEY_POLICY_ARSENAL_DEMOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Capitalism

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Kapitalizm[ENDCOLOR][NEWLINE]5 [ICON_CITIZEN] specjalistów w ka¿dym z twoich miast nie wywo³uje d³u¿ej [ICON_HAPPINESS_3] niezadowolenia.'
WHERE Tag = 'TXT_KEY_POLICY_CAPITALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Open Society

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Awangarda[ENDCOLOR][NEWLINE]Szybkoœæ pojawiania siê [ICON_GREAT_PEOPLE] wielkich ludzi roœnie o 33%.'
WHERE Tag = 'TXT_KEY_POLICY_OPEN_SOCIETY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Covert Action

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]]Tajna operacja[ENDCOLOR][NEWLINE]Szansa na sfa³szowanie wyborów w pañstwach-miastach jest dwukrotnie wiêksza. Otrzymujesz jednego lub wiêcej dodatkowych [ICON_SPY] szpiegów (zale¿y od iloœci pañstw-miast w grze).'
WHERE Tag = 'TXT_KEY_POLICY_COVERT_ACTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Creative Expression
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Ekspresja kreatywna[ENDCOLOR][NEWLINE]+1 [ICON_CULTURE] kultury za wielkie dzie³a. Budynki kulturalne, które mog¹ przechowywaæ [ICON_GREAT_WORK] wielkie dzie³a zapewniaj¹ +1 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_POLICY_CREATIVE_EXPRESSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Economic Union
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Unia gospodarcza[ENDCOLOR][NEWLINE]Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy. +3 [ICON_GOLD] z³ota ze wszystkich szlaków handlowych z innymi cywilizacjami opartymi na ideologii swobód.'
WHERE Tag = 'TXT_KEY_POLICY_ECONOMIC_UNION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Media Culture
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Kultura medialna[ENDCOLOR][NEWLINE] +34% [ICON_TOURISM] turystyki i +1 [ICON_HAPPINESS_1] zadowolenia generowanego przez miasta z wie¿¹ transmisyjn¹.'
WHERE Tag = 'TXT_KEY_POLICY_MEDIA_CULTURE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finest Hour
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Godzina próby[ENDCOLOR][NEWLINE][ICON_STRENGTH] Si³a bojowa miast roœnie o 33%. Umo¿liwia budowê [COLOR_POSITIVE_TEXT]bombowców B17[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_THEIR_FINEST_HOUR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Universal Suffrage
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Powszechne prawo wyborcze[ENDCOLOR][NEWLINE][ICON_GOLDEN_AGE] z³ote ery s¹ o 50% d³u¿sze. Miasta produkuj¹ +1 [ICON_HAPPINESS_1] zadowolenia.'
WHERE Tag = 'TXT_KEY_POLICY_UNIVERSAL_SUFFRAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Urbanization

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Urbanizacja[ENDCOLOR][NEWLINE]Farmy, plantacje, obozy i wszystkie unikalne ulepszenia produkuj¹ +1 [ICON_FOOD] ¿ywnoœci.'
WHERE Tag = 'TXT_KEY_POLICY_URBANIZATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );


-- Space Procurements

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Zaopatrzenie kosmiczne[ENDCOLOR][NEWLINE]Mo¿na kupowaæ czêœci statku kosmicznego za [ICON_GOLD] z³oto. Pozwala ci na budowê fabryk SK w o po³owê krótszym czasie.'
WHERE Tag = 'TXT_KEY_POLICY_SPACE_PROCUREMENTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Academy of Sciences

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Akademia nauk[ENDCOLOR][NEWLINE]Redukuje [ICON_HAPPINESS_3] analfabetyzm we wszystkich miastach o 10%. Otrzymujesz szko³ê publiczn¹ w ka¿dym mieœcie.'
WHERE Tag = 'TXT_KEY_POLICY_ACADEMY_SCIENCES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Revolution

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Rewolucja kulturalna[ENDCOLOR][NEWLINE]+34% turystyki [ICON_TOURISM] wzglêdem innych cywilizacji opartych na ³adzie. Szpiedzy kradn¹ technologie dwa razy szybciej.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_REVOLUTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Dictatorship of the Proletariat

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dyktatura proletariatu[ENDCOLOR][NEWLINE]+34% turystyki [ICON_TOURISM] wzglêdem cywilizacji maj¹cych mniejsze [ICON_HAPPINESS_1] zadowolenie. +1 [ICON_HAPPINESS_1] zadowolenia z fabryk.'
WHERE Tag = 'TXT_KEY_POLICY_DICTATORSHIP_PROLETARIAT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Double Agents

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Podwójni agenci[ENDCOLOR][NEWLINE]Otrzymujesz dodatkowego [ICON_SPY] szpiega. Szpiedzy maj¹ dwukrotnie wiêksze szanse schwytania szpiegów wroga w obsadzonych miastach.'
WHERE Tag = 'TXT_KEY_POLICY_DOUBLE_AGENTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Hero of the People

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Bohater ludu[ENDCOLOR][NEWLINE][ICON_GREAT_PEOPLE] Szybkoœæ pojawiania siê [ICON_GREAT_PEOPLE] wielkich ludzi roœnie o 25%. A [ICON_GREAT_PEOPLE] Wielki cz³owiek, którego wybierzesz pojawia siê w pobli¿u twojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_POLICY_HERO_OF_THE_PEOPLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Iron Curtain

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]¯elazna kurtyna[ENDCOLOR][NEWLINE]Darmowy budynek s¹du po zdobyciu miasta. Wewnêtrzne szlaki handlowe zapewniaj¹ o 50% wiêcej [ICON_FOOD] ¿ywnoœci lub [ICON_PRODUCTION] produkcji. [ICON_CONNECTED] Po³¹czenia miast generuj¹ +3 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_POLICY_IRON_CURTAIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Party Leadership

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Przywództwo partii[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] ¿ywnoœci, [ICON_PRODUCTION] produkcji, [ICON_RESEARCH] nauki, [ICON_GOLD] z³ota i [ICON_CULTURE] kultury na ka¿de miasto.'
WHERE Tag = 'TXT_KEY_POLICY_PARTY_LEADERSHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Patriotic War
UPDATE Language_pl_PL
SET Text = 'Wojna partyzancka'
WHERE Tag = 'TXT_KEY_POLICY_PATRIOTIC_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wojna partyzancka[ENDCOLOR][NEWLINE]+15% premii do ataku i +50% doœwiadczenia dla jednostek wojskowych na sojuszniczym terytorium. Umo¿liwia budowê [COLOR_POSITIVE_TEXT]partyzantów[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POLICY_PATRIOTIC_WAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wojna partyzancka to forma nieregularnych dzia³añ zbrojnych, w której ma³e grupy kombatantów takich jak uzbrojeni cywile u¿ywaj¹ nietypowych, dynamicznych taktyk wojennych, takich jak zasadzki, sabota¿e, napady, ma³a wojna, ataki nêkaj¹ce, które s³u¿¹ do walki z mniej mobilnym tradycyjnym wojskiem.'
WHERE Tag = 'TXT_KEY_POLICY_PATRIOTIC_WAR_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Resettlement

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wysiedlenia[ENDCOLOR][NEWLINE]Nowe miasta zaczynaj¹ z [ICON_CITIZEN] populacj¹ wiêksz¹ o 3. Wszystkie miasta zyskuj¹ natychmiast 2 [ICON_CITIZEN] obywateli. Brak partyzantów ze zdobywania wrogich miast.'
WHERE Tag = 'TXT_KEY_POLICY_RESETTLEMENT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Skyscrapers
UPDATE Language_pl_PL
SET Text = 'Komunizm'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Komunizm[ENDCOLOR][NEWLINE]Koszt [ICON_GOLD] z³ota za zakup budynków zmniejsza siê o 33%. +20% do [ICON_PRODUCTION] produkcji cudów.'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Komunizm to socjoekonomiczny system oparty na w³asnoœci wspólnej œrodków produkcji i charakteryzuj¹cy siê brakiem klas spo³ecznych, pieniêdzy i kraju; równie¿ spo³eczna, polityczna i ekonomiczna ideologia oraz ruch maj¹cy na celu ustanowienie tego porz¹dku spo³ecznego. Ruch zmierzaj¹cy do rozwoju komunizmu w jego marksistowsko-leninistycznej interpretacji, znacz¹co wp³yn¹³ na historiê XX wieku, w którym wyst¹pi³a nasilona rywalizacja pomiêdzy krajami, które pod¹¿a³y za t¹ ideologi¹ i ich wrogami.'
WHERE Tag = 'TXT_KEY_POLICY_SKYSCRAPERS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Socialist Realism
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Socrealizm[ENDCOLOR][NEWLINE]Otrzymujesz darmowe muzeum w ka¿dym mieœcie. Muzea produkuj¹ +2 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_POLICY_SOCIALIST_REALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Worker's Faculties

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Udogodnienia robotnicze[ENDCOLOR][NEWLINE]Fabryki zwiêkszaj¹ o 3 wytwarzan¹ przez miasto [ICON_RESEARCH] naukê. Fabryki s¹ budowane o po³owê krócej.'
WHERE Tag = 'TXT_KEY_POLICY_WORKERS_FACULTIES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Young Pioneers
UPDATE Language_pl_PL
SET Text = 'Wielki skok naprzód'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wielki skok naprzód[ENDCOLOR][NEWLINE]Otrzymujesz dwie darmowe technologie.'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Chiñski wielki skok naprzód by³ kampani¹ gospodarcz¹ i spo³eczn¹ przeprowadzon¹ przez Komunistyczn¹ Partiê Chin w latach 1958-1961. Zosta³a ona og³oszona przez Mao Zedonga i mia³a na celu gwa³towne przekszta³cenie kraju z gospodarki agrarnej na spo³eczeñstwo komunistyczne poprzez szybk¹ industrializacjê i kolektywizacjê.'
WHERE Tag = 'TXT_KEY_POLICY_YOUNG_PIONEERS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Five Year Plan

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Plan piêcioletni[ENDCOLOR][NEWLINE]+2 [ICON_PRODUCTION] produkcji w ka¿dym mieœcie i +1 [ICON_PRODUCTION] produkcji w ka¿dej kopalni, kamienio³omie i unikalnym ulepszeniu.'
WHERE Tag = 'TXT_KEY_POLICY_FIVE_YEAR_PLAN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener/Finisher
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Estetyka[ENDCOLOR] zwiêksza zdolnoœæ cywilizacji do wytwarzania [ICON_CULTURE] kultury i [ICON_TOURISM] turystyki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Estetyki zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] +5% prêdkoœci narodzin wielkich pisarzy, wielkich artystów i wielkich muzyków we wszystkich miastach. [NEWLINE] [ICON_BULLET] Otrzymujesz du¿¹ iloœæ [ICON_CULTURE] kultury za ka¿dym razem, gdy poœwiêcasz [ICON_GREAT_PEOPLE] wielk¹ osobê.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój Estetyki zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] +5% prêdkoœci narodzin wielkich pisarzy, wielkich artystów i wielkich muzyków we wszystkich miastach.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów Estetyki zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Galeria Uffizi[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Podwojenie premii do [ICON_TOURISM] turystyki z zape³nienia sztuk¹ i cudów. [NEWLINE] [ICON_BULLET] Pozwala ci widzieæ ukryte œlady przesz³oœci.[NEWLINE] [ICON_BULLET] Zapewnia dostêp do renesansowych drzew ustrojów, ignoruj¹c wymagania epoki. [NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie dostêpnych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_MUSICIAN] wielkich muzyków za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_AESTHETICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Centers (now Humanities)
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Humanizm[ENDCOLOR][NEWLINE]Amfiteatry, opery, muzea i wie¿e transmisyjne produkuj¹ +3 [ICON_RESEARCH] nauki. Wszystkie [ICON_GREAT_WORK] wielkie dzie³a dostarczaj¹ +1 [ICON_GOLD] z³ota i +1 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Humanizm'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Humanizm to filozoficzne i etyczne stanowisko, które podkreœla wartoœæ i znaczenie ludzkich bytów, indywidualnie i kolektywnie. Ogólnie preferuje krytyczne myœlenie i dowody (racjonalizm, empiryzm) ponad ustanowione doktryny lub wierzenia (fideizm). Znaczenie terminu humanizm zmienia³o siê, w zale¿noœci od kolejnych ruchów intelektualnych, które siê z nim identyfikowa³y. Ogólnie jednak, humanizm odnosi siê do pogl¹du, który przyznaje jakieœ wyobra¿enie "ludzkiej natury" (czasami kontrastowany z antyhumanizmem).'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Fine Arts
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wytwornoœæ[ENDCOLOR][NEWLINE]Ca³e dodatnie [ICON_HAPPINESS_1] zadowolenie jest dodawane do przyrostu [ICON_CULTURE] kultury ca³ego imperium. +1 [ICON_HAPPINESS_1] zadowolenie i +3 [ICON_CULTURE] kultury za wszystkie gildie.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wytwornoœæ'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wyrafinowanie jest jakoœci¹ wytwornoœci - okazywanie raczej dobrego smaku, wiedzy i delikatnoœæ ni¿ prostactwa, g³upoty czy wulgarnoœci. W odbiorze klas spo³ecznych, wyrafinowanie mo¿e siê wi¹zaæ z takimi konceptami jak status, uprzywilejowanie i wy¿szoœæ.'
WHERE Tag = 'TXT_KEY_POLICY_FINE_ARTS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Flourishing of the Arts
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dziedzictwo[ENDCOLOR][NEWLINE]Wszystkie cuda œwiata produkuj¹ +2 [ICON_TOURISM] turystyki. Wejœcie w [ICON_GOLDEN_AGE] z³ot¹ erê lub ukoñczenie wykopalisk archeologicznych generuje [ICON_TOURISM] turystykê we wszystkich znanych cywilizacjach.'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dziedzictwo'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dziedzictwo kulturowe to spuœcizna w postaci fizycznych artefaktów i nieuchwytnych cech grup lub spo³eczeñstwa, która jest przejmowana od starszych pokoleñ, utrzymywana obecnie i uznawana jako korzyœæ dla przysz³ych pokoleñ. Dziedzictwo kulturowe zawiera namacaln¹ kulturê (jak budynki, pomniki, s³ynne miejsca, ksi¹¿ki, dzie³a sztuki, artefakty), nienamacaln¹ kulturê (jak folklor, tradycje, jêzyk i wiedzê) oraz dziedzictwo naturalne (zawieraj¹ce znacz¹ce kulturowo miejsca i zró¿nicowanie biologiczne).'
WHERE Tag = 'TXT_KEY_POLICY_FLOURISHING_OF_ARTS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Artistic Genius
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Skarb narodowy[ENDCOLOR][NEWLINE][ICON_GREAT_PEOPLE] wielka osoba, któr¹ wybierzesz pojawia siê obok twojej [ICON_CAPITAL] stolicy. +3 [ICON_CULTURE] kultury z ulepszeñ [ICON_GREAT_PEOPLE] wielkich osób.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Skarb narodowy'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Idea skarbu narodowego, jak epopeje i hymny narodowe, jest czêœci¹ jêzyka narodowego romantyzmu, która powsta³ w koñcówce XVIII i w XIX wieku. Nacjonalizm to ideologia która wspiera naród, jako fundamentaln¹ jednostkê ludzkiego ¿ycia spo³ecznego, do czego zalicza siê wspólny jêzyk, wartoœci i kultura. Zatem skarbem narodowym, czêœci¹ ideologii nacjonalizmu, jest dzielona kultura.'
WHERE Tag = 'TXT_KEY_POLICY_ARTISTIC_GENIUS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Exchange
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wymiana kulturowa[ENDCOLOR][NEWLINE]Zwiêksza modyfikator turystyki za wspóln¹ religiê, szlaki handlowe i otwarte granice o 25%. Imperium natychmiast wkracza w [ICON_GOLDEN_AGE] z³ot¹ erê.'
WHERE Tag = 'TXT_KEY_POLICY_ETHICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener -- now called Industry

UPDATE Language_pl_PL
SET Text = 'Przemys³'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przemys³|Przemys³u|Przemys³owi|Przemys³|Przemys³em|Przemyœle|przemys³owej|przemys³owej'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Przemys³[ENDCOLOR] zapewnia premie imperiom skoncentrowanym na [ICON_GOLD] z³ocie i [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie przemys³u zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] 25 [ICON_GOLD] z³ota za ka¿dym razem, gdy wzniesiesz budynek. Skaluje siê skaluje siê zale¿nie od epoki. [NEWLINE] [ICON_BULLET] -5% [ICON_GOLD] z³ota potrzebnego na zakupy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój przemys³owy zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] -5% [ICON_GOLD] z³ota potrzebnego na zakupy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów przemys³owych zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Big Ben[ENDCOLOR]. [NEWLINE] [ICON_BULLET] +1 [ICON_HAPPINESS_1] zadowolenie za ka¿dy luksus. [NEWLINE] [ICON_BULLET] Specjaliœci dostarczaj¹ +2 [ICON_PRODUCTION] produkcji.[NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_MERCHANT] wielkich kupców za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_COMMERCE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przemys³ to produkcja dóbr lub us³ug w gospodarce. Przemys³ wytwórczy sta³ siê kluczowym sektorem produkcji i pracy w krajach europejskich i pó³nocnoamerykañskich podczas rewolucji przemys³owej, wzburzaj¹c wczeœniejsz¹ kupieck¹ i feudaln¹ gospodarkê. Sta³o siê poprzez wiele kolejnych gwa³townych postêpów technologicznych, jak produkcja stali i wêgla. Po rewolucji przemys³owej, oko³o jedna trzecia œwiatowej produkcji gospodarczej pochodzi z przedsiêbiorstw wytwórczych. Wiele rozwiniêtych krajów i wiele rozwijaj¹cych siê/œrednio rozwiniêtych krajów (Chiny, Indie itp.) znacz¹co na nim polegaj¹. Ga³êzie przemys³u, kraje w których siê one znajduj¹ i gospodarki tych pañstw w po³¹czone skomplikowan¹ sieci¹ zale¿noœci.'
WHERE Tag = 'TXT_KEY_POLICY_COMMERCE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Mercenary Army
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Podzia³ pracy[ENDCOLOR][NEWLINE]+2 [ICON_PRODUCTION] produkcji i +1 [ICON_GOLD] z³ota z wiatraków, warsztatów i fabryk. Inwestowanie [ICON_GOLD] z³ota w budynki redukuje ich koszt [ICON_PRODUCTION] produkcji o dodatkowe 15%.'
WHERE Tag = 'TXT_KEY_POLICY_TRADE_UNIONS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Podzia³ pracy jest specjalizacj¹ wspó³pracuj¹cych jednostek, których spe³niaj¹ specyficzne zadania i role. Z powodu du¿ej iloœæ pracy zaoszczêdzonej przez dawanie robotnikom wyspecjalizowanych zadañ w fabryka epoki rewolucji przemys³owej, niektórzy klasyczny ekonomiœci, jak równie¿ niektórzy in¿ynierowie jak Charles Babbage stali siê orêdownikami podzia³u pracy. Ponadto, maj¹c pracowników wykonuj¹cych pojedyncze lub ograniczone zadania wyeliminowano d³ugi okres nauki, wymagany do wyszkolenia rzemieœlników, którzy zostali zast¹pieni przez mniej op³acanych ale bardziej produktywnych niewykwalifikowanych pracowników. Historycznie, wzrastaj¹ca z³o¿onoœæ podzia³u pracy jest zwi¹zana ze wzrostem ca³kowitego wytwarzania i handlu, powstaniem kapitalizmu i skomplikowanym procesem industrializacji.'
WHERE Tag = 'TXT_KEY_POLICY_TRADE_UNIONS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Podzia³ pracy'
WHERE Tag = 'TXT_KEY_POLICY_TRADE_UNIONS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Wagon Trains (now called Turnpikes)

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wolny handel[ENDCOLOR][NEWLINE]+2 [ICON_GOLD] z³ota z l¹dowych [ICON_INTERNATIONAL_TRADE] miêdzynarodowych szlaków handlowych. Próg ubóstwa zmniejszony o 20% we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wolny handel'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wolny handel to polityka prowadzona przez niektóre rynki miêdzynarodowe, w których rz¹dy nie ograniczaj¹ importu lub eksportu do innych krajów. Przyk³ady wolnego handlu stanowi Europejski Obszar Gospodarczy i Pó³nocnoamerykañski Uk³ad Wolnego Handlu, które stanowi¹ otwarte rynki. Wiêkszoœæ pañstw jest dziœ cz³onkami wielostronnych umów handlowych Œwiatowej Organizacji Handlu (WTO). Jednak¿e, wiêkszoœæ rz¹dów wci¹¿ nak³ada protekcyjne przepisy, które maj¹ na celu wspieranie lokalnego zatrudnienia, jak nak³adanie op³at na import lub ulgi dla eksportu. Rz¹dy mog¹ te¿ ograniczaæ wolny handel, aby limitowaæ eksport surowców naturalnych. Do innych przeszkód, które mog¹ utrudniaæ handel zliczaj¹ siê kwoty importu, podatki i przeszkody nietaryfowe, jak regulacje prawne.'
WHERE Tag = 'TXT_KEY_POLICY_CARAVANS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Mercantilism
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Merkantylizm[ENDCOLOR][NEWLINE] +2 [ICON_RESEARCH] nauki i [ICON_CULTURE] kultury z ka¿dego urzêdu celnego, banku i gie³dy. Zyski z wewnêtrznych [ICON_INTERNATIONAL_TRADE] szlaków handlowych zwiêkszone o 33%.'
WHERE Tag = 'TXT_KEY_POLICY_MERCANTILISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Entrepreneurship
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Przedsiêbiorczoœæ[ENDCOLOR][NEWLINE]Wielcy kupcy pojawiaj¹ siê o 25% szybciej. +1 [ICON_GOLD] z³ota i +1 [ICON_PRODUCTION] produkcji z kopalni, kamienio³omu i tartaku.'
WHERE Tag = 'TXT_KEY_POLICY_ENTREPRENEURSHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

--Protectionism

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Protekcjonizm[ENDCOLOR][NEWLINE]Otrzymujesz dodatkowy [ICON_INTERNATIONAL_TRADE] szlak handlowy i otrzymujesz +2 [ICON_GOLD] z³ota z morskich [ICON_INTERNATIONAL_TRADE] miêdzynarodowych szlaków handlowych.'
WHERE Tag = 'TXT_KEY_POLICY_PROTECTIONISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener -- Now called Imperialism -- Grants +1 Movement i +1 Sight for Naval Units, i speeds production of land units by +5%, with an additional +4% za ka¿dych policy unlocked in Imperialism. Receive 1 dodatkowe happiness for every owned luxury, i 1 Happiness for every garrisoned unit.
UPDATE Language_pl_PL
SET Text = 'Imperializm'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Imperializm|Imperializmu|Imperializmowi|Imperializm|Imperializmem|Imperializmie|Imperializmu|Imperializmu', Gender = 'masculine'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Imperializm[ENDCOLOR] zwiêksza twoje mo¿liwoœci powiêkszania imperium dziêki sile militarnej, zw³aszcza dominacji morskiej (i póŸniej powietrznej).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Imperializmu zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +1 [ICON_MOVES] ruchu i +1 zasiêgu widzenia dla jednostek morskich. [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] do produkcji jednostek morskich i powietrznych.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój Imperializmu zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% [ICON_PRODUCTION] do produkcji jednostek morskich i powietrznych.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów Imperializmu zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Brama Brandenburska[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Jednostki powietrzne rozpoczynaj¹ z awansem [COLOR_POSITIVE_TEXT]Zasiêg[ENDCOLOR] a morskie z awansem [COLOR_POSITIVE_TEXT]Niez³omnoœæ[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Ze wszystkich pól oceanu, wybrze¿a i jeziora dostajesz +1 [ICON_RESEARCH] nauki i +1 [ICON_PRODUCTION] produkcji.[NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_ADMIRAL] wielkich admira³ów za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_EXPLORATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Maritime Infrastructure -- Now Martial Law
UPDATE Language_pl_PL
SET Text = 'Stan wojenny'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Stan wojenny[ENDCOLOR][NEWLINE]Iloœæ [ICON_GOLD] z³ota potrzebna do ulepszania jednostek wojskowych zmniejszona o 25%. [ICON_PUPPET] Miasta marionetkowe otrzymuj¹ modyfikator +25% do [ICON_PRODUCTION] produkcji a [ICON_OCCUPIED] okupowane miasta +33% do [ICON_PRODUCTION] produkcji.'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Stan wojenny jest wprowadzony przez najwy¿szego rang¹ oficera, jako przywódca si³ zbrojnych lub g³owa rz¹dy i odbiera ca³¹ w³adzê wczeœniejszym wykonawczym, prawodawczym i s¹downiczym ga³êziom pañstwa. Jest czêsto wprowadzany tymczasowo, gdy rz¹d lub przedstawiciele obywateli nie s¹ w stanie efektywnie pracowaæ (np. utrzymaæ porz¹dku i bezpieczeñstwa lub zapewniæ podstawowe us³ugi).'
WHERE Tag = 'TXT_KEY_POLICY_MARITIME_INFRASTRUCTURE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Navy -- Now called Exploitation -- +1 Food and +1 Production from Snow, Desert and Mountain Tiles
UPDATE Language_pl_PL
SET Text = 'Eksploatacja'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Eksploatacja[ENDCOLOR][NEWLINE]Farmy i plantacja daj¹ +1 [ICON_PRODUCTION] produkcji i +1 [ICON_FOOD] ¿ywnoœci. Jednostki wojskowe mog¹ byæ ulepszane na terytoriach posiadanych przez sojusznicze pañstwa-miasta.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Eksploatacja to wykorzystywanie kogoœ lub czegoœ w niesprawiedliwy lub okrutny sposób, albo ogólniej ujmuj¹c, do koñca. Najczêœciej s³owo Eksploatacja odnosi siê do wykorzystywania gospodarczego, aktu wykorzystania innej osoby w celu osi¹gniêcia zysku, zw³aszcza wykorzystuj¹c jej pracê bez oferowania albo dostarczenia jej sprawiedliwej zap³aty.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_NAVY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Naval Tradition -- Military Tradition -- +25% to Great Admiral and Great General Production -- Barracks, Armories and Military Academies provide +1 Science.
UPDATE Language_pl_PL
SET Text = 'Tradycja wojskowa'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Tradycja wojskowa[ENDCOLOR][NEWLINE]Pozyskujesz [ICON_GREAT_ADMIRAL] wielkich admira³ów i [ICON_GREAT_GENERAL] genera³ów 33% szybciej. Koszary, zbrojownie i akademie wojskowe dostarczaj¹ +3 [ICON_RESEARCH] nauki i +1 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tradycja wojskowa to specyficzny zestaw praktyk zwi¹zany ogólnie z wojskiem lub ¿o³nierzem. Pojêcie mo¿e okreœlaæ styl mundurów wojskowych, musztry a nawet muzyki jednostki wojskowej.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Navigation School -- Now called Exploration -- Receive a free Great Admiral -- Reveals all Capitals and starts a Golden Age.
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Eksploracja[ENDCOLOR][NEWLINE] Otrzymujesz darmowego [ICON_GREAT_ADMIRAL] wielkiego admira³a. Wszystkie nieodkryte [ICON_CAPITAL] stolice zostaj¹ ukazane. Zaokrêtowane jednostki otrzymuj¹ +2 [ICON_MOVES] ruchu a [ICON_GREAT_GENERAL] wielcy genera³owie +1 [ICON_MOVES] ruchu.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Eksploracja'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Od kiedy cz³owiek po raz pierwszy osiedli³ siê w jednym miejscu, zawsze znajdowali siê tacy, którzy chcieli wiedzieæ, co kryje siê za horyzontem. Odkrywcy, motywowani chêci¹ zysku, ¿¹dz¹ przygody, nakazami religii, koniecznoœci¹ lub czyst¹ ciekawoœci¹, poszerzali nasz¹ wiedzê o œwiecie i jego cudach. Poprzez swe odkrycia ³¹czyli ró¿ne odleg³e kultury na dobre i na z³e. Wielcy odkrywcy, kartagiñski Hannon, chiñski genera³ Wang Dayuan, Anglik sir Richard Burton oraz tysi¹ce innych, napêdzali rozwój cywilizacji i kszta³towali historiê.'
WHERE Tag = 'TXT_KEY_POLICY_NAVIGATION_SCHOOL_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Treasure Fleets -- Now called Civilizing Mission -- Receive a free Military Base, and a lump sum of Gold, when you conquer a city. Reduced maintenance.
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Misja cywilizacyjna[ENDCOLOR][NEWLINE]Otrzymujesz darmow¹ fabrykê i du¿¹ sumê [ICON_GOLD] z³ota,kiedy podbijasz miasto. Brak [ICON_GOLD] kosztów utrzymania garnizonów.'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Misja cywilizacyjna'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mission civilisatrice (francuska nazwa dla misji cywilizacyjnej) to usprawiedliwienie interwencji lub kolonizacji, proponuj¹ce wziêcie udzia³u w szerzeniu cywilizacji, najbardziej odpowiadaj¹ce okcydentalizacji tubylców.  By³a to znacz¹ca, podstawowa zasada kolonializmu francuskiego i portugalskiego w póŸnym wieku XIX i wczesnym XX. By³a wp³ywowa i francuskich koloniach w Algierii, francuskiej Afryce Zachodniej i Indochinach oraz w portugalskich kolonia w Angoli, Gwinei, Mozambiku i Timorze. Europejskie si³y kolonialne poczuwa³y, ¿e ich obowi¹zkiem jest przyniesienie cywilizacji zachodniej do tych, których uznawano za zacofanych ludzi. Zamiast jedynie zarz¹dzaæ ludŸmi w koloniach, Europejczycy starali siê okcydentalizowaæ ich w zwi¹zku z ideologi¹ kolonialn¹ znan¹ jako "asymilacja".'
WHERE Tag = 'TXT_KEY_POLICY_TREASURE_FLEETS_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Honor Opener/Finisher
UPDATE Language_pl_PL
SET Text = 'W³adza'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W³adza|W³adzy|W³adzy|W³adzê|W³adz¹|W³adzy|W³adzy|W³adzy', Gender = 'feminine'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]W³adza[ENDCOLOR] wielce siê przys³u¿y nastawionym na wojnê i ekspansjonistycznym cywilizacjom.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie W³adzy zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +25% premii bojowej przeciw barbarzyñcom i otrzymywanie powiadomieñ, gdy obozowiska barbarzyñców pojawiaj¹ siê na odkrytym terytorium. [NEWLINE] [ICON_BULLET] Zyskujesz [ICON_CULTURE] kulturê za zabijanie jednostek i czyszczenie obozowisk barbarzyñskich [NEWLINE] [ICON_BULLET] +5% [ICON_PRODUCTION] do produkcji jednostek l¹dowych.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój W³adzy zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +3% [ICON_PRODUCTION] do produkcji jednostek l¹dowych.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów W³adzy zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Terakotowa Armia[ENDCOLOR]. [NEWLINE] [ICON_BULLET]Pozwala na zakup landsknechtów, Legii Cudzoziemskiej i najemników po tym, jak zostan¹ odkryte wymagane technologie.[NEWLINE] [ICON_BULLET] Zapewnia dostêp do œredniowiecznych drzew ustrojów, ignoruj¹c wymagania epoki.[NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_GENERAL] wielkich genera³ów za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_HONOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W³adza to mo¿liwoœæ wywierania przez jednostkê b¹dŸ grupê rzeczywistego wp³ywu na istotne okolicznoœci ¿ycia przez ukierunkowywanie w³asnego postêpowania (w³adza nad sob¹ - indywidualna) lub postêpowania innych osób (w³adza nad innymi - spo³eczna). W³adza spo³eczna jest zdolnoœci¹ ukierunkowywania zachowañ innych ludzi niezale¿nie od tego, czy jest to zgodne z ich interesem i wol¹. Tam, gdzie przewa¿a zgodnoœæ interesów i woli wspó³zale¿nych osobników, zbêdne s¹ stosunki w³adcze.'
WHERE Tag = 'TXT_KEY_POLICY_HONOR_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Warrior Code (Now Imperialism)
UPDATE Language_pl_PL
SET Text = 'Imperium'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Imperium[ENDCOLOR][NEWLINE]Darmowy osadnik pojawia siê obok twojej [ICON_CAPITAL] stolicy. Otrzymujesz 50 punktów [ICON_GOLDEN_AGE] z³otej ery i [ICON_CULTURE] kultury kiedy zak³adasz lub podbijasz miasta, skaluje siê zale¿nie od epoki. Premia z podboju zale¿y te¿ od [ICON_CITIZEN] populacji miasta.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Imperium to ³aciñskie s³owo, które, w szerokim znaczeniu, t³umaczy siê jako "si³a do rz¹dzenia". W staro¿ytnym Rzymie, ró¿ne rodzaje w³adzy b¹dŸ sprawowanych stanowisk by³ rozró¿niane przez wiele pojêæ. Imperium odnosi³o siê do zwierzchnictwa pañstwa nad jednostk¹.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIORCODE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Discipline (Now Tribute)
UPDATE Language_pl_PL
SET Text = 'Trybut'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Trybut[ENDCOLOR][NEWLINE]Miasta otrzymuj¹ +25 [ICON_PRODUCTION] produkcji i [ICON_FOOD] ¿ywnoœci, kiedy rozszerzaj¹ swoje granice. Otrzymujesz 50 [ICON_CULTURE] kultury, kiedy ¿¹dasz daniny od pañstw-miast. Premie zale¿¹ od epoki.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Trybut to bogactwa, które jedna grupa daje drugiej jako oznaka respektu, albo czêsto w przypadku historycznych kontekstów, poddañstwa lub przynale¿noœci. Ró¿ne staro¿ytne kraje pobiera³y daniny od rz¹dców ziem przez nie podbite, gro¿¹c w przeciwnym razie najazdem.'
WHERE Tag = 'TXT_KEY_POLICY_DISCIPLINE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Tradition (Now Dominance)
UPDATE Language_pl_PL
SET Text = 'Dominacja'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dominacja[ENDCOLOR][NEWLINE]Zabijanie jednostek wojskowych generuje [ICON_RESEARCH] naukê opart¹ na [ICON_STRENGTH] siê bojowej pokonanej jednostki. Wszystkie jednostki walcz¹ce wrêcz lecz¹ 20 P¯ po zabiciu jednostki wojskowej.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dominacja to przewaga danej jednostki, kilku jednostek lub grupy spo³ecznej nad innymi jednostkami lub grupami, odnosz¹ca siê przede wszystkim do sfery polityczno-ekonomicznej i odnosi siê do umo¿liwienia decydowania o owych jednostkach, grupach ludzi czy pañstwach wed³ug swej woli. W marksistowskiej teorii struktury spo³ecznej typ stosunków pomiêdzy klas¹ panuj¹c¹, uprzywilejowan¹ pod wzglêdem ekonomicznym i pod wzglêdem dostêpu do w³adzy, a innymi klasami spo³ecznymi. Równie¿ przewaga iloœciowa, jakoœciowa lub znaczeniowa jednej rzeczy nad inn¹.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYTRADITION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Military Caste (Now Conscription)
UPDATE Language_pl_PL
SET Text = 'Dyscyplina'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dyscyplina[ENDCOLOR][NEWLINE]Ka¿de miasto z garnizonem generuje +1 [ICON_HAPPINESS_1] zadowolenia i +3 [ICON_CULTURE] kultury. [ICON_GOLD] koszt utrzymania jednostek zmniejszony o 15% a dróg o 50%.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_CASTE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W strukturach wojskowych, dyscyplina odnosi siê do chêci ¿o³nierza do s³uchania siê prawomocnych rozkazów, respektowania dowodz¹cych oficerów i ryzykowania swoim ¿yciem, gdy jest taka potrzeba. Od zdyscyplinowanych ¿o³nierzy oczekuje siê utrzymywania szacunku do swojego munduru poza s³u¿b¹ i bronienia s³abych i bezbronnych. Uchybienia w dyscyplinie mog¹ prowadziæ do kary wojskowej a nawet s¹du wojennego i zwolnienia ze s³u¿by.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARYCASTE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Professional Army (Now Martial Law)
UPDATE Language_pl_PL
SET Text = 'Honor'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Honor[ENDCOLOR][NEWLINE]Jednostki zyskuj¹ +10% [ICON_STRENGTH] si³y bojowej. Jednostka wojskowa pojawia siê obok wszystkich miast które osi¹gnê³y wielkoœæ 8 [ICON_CITIZEN] mieszkañców (albo ju¿ przekroczy³y 8 [ICON_CITIZEN] mieszkañców).'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONAL_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W spo³eczeñstwie kieruj¹cym siê honorem status obywateli opiera siê na spo³ecznym os¹dzie ich osobistych cech. Cechy uwa¿ane za godne pochwa³y to: lojalnoœæ, uczciwoœæ, prawoœæ i odwaga. W wielu takich spo³eczeñstwach od mê¿czyzn oczekuje siê, ¿e bêd¹ broniæ swojego dobrego imienia a¿ do œmierci, poniewa¿ jakakolwiek plama na honorze uwa¿ana jest za rzecz gorsz¹ od utraty ¿ycia.'
WHERE Tag = 'TXT_KEY_POLICY_PROFESSIONALARMY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Foreign Legion Text
UPDATE Language_pl_PL
SET Text = 'Specjalna jednostka piechoty z epoki wspó³czesnej. Mo¿e byæ otrzymana tylko jako darmowe jednostki przez zasadê "Armii ochotniczej" z ideologii Swobód albo poprzez zakup [ICON_GOLD] z³otem po ukoñczeniu drzewa W³adzy. Ta jednostka otrzymuje premiê bitewn¹ poza sojuszniczym terytorium ale poza tym jest podobna do strzelców.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_FOREIGN_LEGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Legia Cudzoziemska to jednostka specjalna dostêpna jedynie, jako darmowa, dziêki zasadzie „Armia ochotnicza” ideologii Swobód, albo poprzez zakup po ukoñczeniu drzewa w³adzy. Otrzymuje znaczn¹ premiê do walki podczas dzia³añ poza w³asnym terytorium, co czyni j¹ doskona³¹ jednostk¹ do przejmowania kontroli nad obcymi terenami.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_FOREIGNLEGION_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Liberty Opener
UPDATE Language_pl_PL
SET Text = 'Postêp'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Postêp|Postêpu|Postêpowi|Postêp|Postêpem|Postêpie|Postêpu|Postêpu', Gender = 'masculine'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Postêp[ENDCOLOR] to najlepszy wybór dla cywilizacji pragn¹cych rozros³ej infrastruktury i postêpu naukowego.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Postêpu zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Otrzymujesz 25 [ICON_RESEARCH] nauki, kiedy rodzi siê [ICON_CITIZEN] mieszkaniec w twojej [ICON_CAPITAL] stolicy, skaluje siê zale¿nie od epoki. [NEWLINE] [ICON_BULLET] Otrzymujesz 10 [ICON_CULTURE] kultury, kiedy odkrywasz technologiê.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój Postêpu zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Otrzymujesz dodatkowe 10 punktów [ICON_CULTURE] kultury, kiedy odkrywasz technologiê, skaluje siê zale¿nie od epoki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów Postêpu zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Partenon[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Otrzymujesz 20 sztuk [ICON_GOLD] z³ota, kiedy rodzi siê mieszkaniec w dowolnym mieœcie, skaluje siê zale¿nie od epoki.[NEWLINE] [ICON_BULLET] Zapewnia dostêp do œredniowiecznych drzew ustrojów, ignoruj¹c wymagania epoki.[NEWLINE] [ICON_BULLET] [NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_WRITER] wielkich pisarzy za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_LIBERTY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W historii intelektualnej, idea postêpu to pogl¹d, ¿e postêp technologiczny, naukowy i organizacja spo³eczna mo¿e polepszyæ warunki ludzkie. To znaczy, ludzie mog¹ staæ siê lepsi w kontekœcie jakoœci ¿ycia (postêp spo³eczny) przez rozwój gospodarczy (modernizacja) i przez wykorzystywanie nauki i technologii (postêp naukowy). To za³o¿enie, ¿e ten proces nastanie gdy ludzie wykorzystaj¹ swój wysi³ek i umiejêtnoœci, wiêc nie jest z góry zapewniony. Rol¹ ekspertów jest zidentyfikowanie utrudnieñ, które zwalniaj¹ lub niweluj¹ rozwój.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_LIBERTY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Collective Rule (Now Code of Laws)
UPDATE Language_pl_PL
SET Text = 'Organizacja'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Organizacja[ENDCOLOR][NEWLINE] Miasta otrzymuj¹ +10 [ICON_FOOD] ¿ywnoœci i [ICON_CULTURE] kultury, kiedy wznosz¹ budynki. Premie skaluje siê zale¿nie od epoki.'
WHERE Tag = 'TXT_KEY_POLICY_COLLECTIVE_RULE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Organizacja miast, zwane te¿ "planowanie urbanistycznym" to techniczny i polityczny proces zajmuj¹cy siê wykorzystaniem ziemi i przygotowaniem œrodowiska miejskiego, w³¹czaj¹c w to powietrze i wodê oraz infrastrukturê przechodz¹c¹ do i z przestrzeni miejskich jak transport czy sieci dystrybucyjne. Planowanie urbanistyczne kieruje i zapewnia uporz¹dkowany rozwój osiedli i otaczaj¹cych miejscowoœci, z których lub do których doje¿d¿aj¹ pracownicy albo dziel¹ siê surowcami. Poœwiêca siê te¿ czas badaniom i analizie, myœleniu strategicznemu, architekturze, planowaniu miejskiemu, konsultacjom spo³ecznym, zaleceniom polityków, wdra¿aniu tego w ¿ycie i zarz¹dzaniu.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_COLLECTIVERULE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Citizenship
UPDATE Language_pl_PL
SET Text = 'Wolnoœæ'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wolnoœæ[ENDCOLOR][NEWLINE]Robotnik pojawia siê obok [ICON_CAPITAL] stolicy.[NEWLINE] +1 [ICON_MOVES] ruchu dla wszystkich jednostek cywilnych.'
WHERE Tag = 'TXT_KEY_POLICY_CITIZENSHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Podstawowym za³o¿eniem wolnoœci jako ustroju jest przekonanie, ¿e obywatele zas³uguj¹, aby byæ wolnymi. Nie podlegaj¹ ograniczeniom fizycznym ani kontroli despoty. Ch³opi pañszczyŸniani, niewolnicy i dziedziczna s³u¿ba nie ciesz¹ siê wolnoœci¹, tak samo jak obywatele zmuszeni do ¿ycia w okreœlonej wiosce, gu³agu lub getcie.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_CITIZENSHIP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Republic (Now Civil Engineers)
UPDATE Language_pl_PL
SET Text = 'Ekspertyza'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Ekspertyza[ENDCOLOR][NEWLINE] +15% [ICON_PRODUCTION] do produkcji budynków. Prêdkoœæ budowy ulepszeñ zwiêkszona o 25%.'
WHERE Tag = 'TXT_KEY_POLICY_REPUBLIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ekspert to ktoœ, znany jako wiarygodne Ÿród³o technik lub umiejêtnoœci, które wykorzystuje do w³aœciwego oceniania lub decydowania, albo szerzej odnosi siê do autorytetu i statusu wœród wspó³pracowników lub publicznie w specyficznej dobrze wyró¿niaj¹cej siê dziedzinie. Ekspert, bardziej ogólnie, to osoba w szerok¹ wiedz¹ albo zdolnoœciami opartymi na badaniach, doœwiadczeniu lub zawodzie w okreœlonej dziedzinie nauk. Eksperci s¹ proszeni o porady zwi¹zane z przedmiotem ich specjalizacji, ale nie zawsze zgadzaj¹ siê ze szczegó³ami pola nauki. Ekspertowi mo¿na ufaæ, opieraj¹c to na podstawie jego kwalifikacji, szkolenia, edukacji, zawodu, publikacji lub doœwiadczenia, aby mieæ specjaln¹ wiedzê w temacie przekraczaj¹c¹ t¹, któr¹ ma zwyk³a osoba, wystarczaj¹c¹ aby inni oficjalnie (i legalnie) mogli polegaæ na opinii tej osoby. Historycznie,ekspert odnosi³ siê do mêdrca (gr. sophos). By³ on zwykle przenikliwym myœlicielem wyró¿niaj¹cym siê swoj¹ wiedz¹ i os¹dem.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPUBLIC_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Representation
UPDATE Language_pl_PL
SET Text = 'Równoœæ'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Równoœæ[ENDCOLOR][NEWLINE]+2 [ICON_PRODUCTION] produkcji we wszystkich miastach i +1 [ICON_HAPPINESS_1] zadowolenia za ka¿dych 15 [ICON_CITIZEN] mieszkañców w [ICON_PUPPET] nie marionetkowych miastach.'
WHERE Tag = 'TXT_KEY_POLICY_REPRESENTATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Równoœæ spo³eczna to stan rzeczy, w którym wszyscy ludzie w okreœlonej spo³ecznoœci lub izolowanej grupie maj¹ taki sam status w okreœlonych sprawach, czêsto zawiera prawa obywatelskie, wolnoœæ wypowiedzi, prawa w³asnoœci i równy dostêp do dóbr spo³ecznych i us³ug. Jednak¿e, zawiera te¿ koncepty równoœci zdrowia, gospodarcze i inne zabezpieczenia socjalne. Oprócz tego równe szanse oraz obowi¹zki i w ten sposób dotyczy ca³oœci spo³eczeñstwa. Równoœæ spo³eczna wymaga braku prawnie uprzywilejowanej klasy spo³ecznej lub granic kast oraz dyskryminacji motywowanej niezbywaln¹ cech¹ danej osoby.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_REPRESENTATION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Meritocracy
UPDATE Language_pl_PL
SET Text = 'Braterstwo'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Braterstwo[ENDCOLOR][NEWLINE]+2 [ICON_FOOD] ¿ywnoœci w ka¿dym mieœcie i +2 [ICON_RESEARCH] nauki z [ICON_CONNECTED] po³¹czeñ miast.'
WHERE Tag = 'TXT_KEY_POLICY_MERITOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W filozofii, braterstwo jest rodzajem etycznego zwi¹zku pomiêdzy ludŸmi opartym na mi³oœci i solidarnoœci. Zosta³o ono wspomniane w haœle narodowym Francji: "Liberté-Égalité-Fraternité(Wolnoœæ, równoœæ, braterstwo)". Dziœ, konotacje braterstwa ró¿ni¹ siê w zale¿noœci od kontekstu, w³¹czaj¹c w to towarzystwa i braterstwa zwi¹zane z religi¹, myœl¹ intelektualn¹, œrodowiskiem akademickim oraz materialne i spo³eczne d¹¿enia ich cz³onków. Dodatkowo, w czasach wspó³czesnych, odnosi siê czasami do tajnych stowarzyszeñ, zw³aszcza do wolnomularzy, ló¿ i ró¿nych stowarzyszeñ akademickich i studenckich.'
WHERE Tag = 'TXT_KEY_CIV5_POLICY_MERITOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher

-- Opener
UPDATE Language_pl_PL
SET Text = 'Dyplomacja'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_CAP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dyplomacja|Dyplomacji|Dyplomacji|Dyplomacjê|Dyplomacj¹|Dyplomacji|Dyplomacji|Dyplomacji', Gender = 'feminine'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dyplomacja[ENDCOLOR] polepsza twoje zdolnoœci oddzia³ywania na pañstwa-miasta, szpiegów i Kongres Œwiatowy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Dyplomacji zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Punkt równowagi [ICON_INFLUENCE] wp³ywu na pañstwa-miasta zwiêkszony o 20. [NEWLINE] [ICON_BULLET] Otrzymujesz +25% nagród z zadañ wykonanych dla pañstw-miast.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój Dyplomacji zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET]Twój [ICON_INFLUENCE] wp³yw na pañstwa-miasta spada 5% wolniej.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich ustrojów Dyplomacji zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Zakazany Pa³ac[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Sojusznicze pañstwa-miasta bêd¹ ci okazjonalnie podarowywaæ [ICON_GREAT_PEOPLE] wielkiego cz³owieka. [NEWLINE] [ICON_BULLET] Kiedy ukañczasz [ICON_INTERNATIONAL_TRADE] szlak handlowy do pañstwa-miasta, otrzymujesz premiê do [ICON_TOURISM] turystki we wszystkich znanych cywilizacjach opart¹ na twoim ostatnim wytwarzaniu [ICON_CULTURE] kultury.[NEWLINE] [ICON_BULLET] Zapewnia dostêp do renesansowych drzew ustrojów, ignoruj¹c wymagania epoki. [NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie dostêpnych) do uzyskania dostêpu do ideologii.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dyplomacja to domena bogatych oraz wp³ywowych ludzi i jest zdolnoœci¹ podmiotu politycznego do oddzia³ywania z innymi podmiotami politycznymi. W wielu spo³eczeñstwach dyplomacja jest wykonywanych przez parê osób: rodziny królewskie lub ksi¹¿êta kupieckie. Dzisiaj, dyplomacja jest w du¿ej mierze kierowana procesami demokratycznymi, chocia¿ niektóre rzeczy pozostaj¹ w gestii tych z odpowiednimi wp³ywami.'
WHERE Tag = 'TXT_KEY_POLICY_PATRONAGE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Philanthropy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Informatorzy[ENDCOLOR][NEWLINE]Otrzymujesz jednego lub wiêcej [ICON_SPY] szpiegów (zale¿y od iloœci pañstw-miast w grze).'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Informatorzy'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Informator to osoba, która dostarcza szczególn¹ informacjê o osobie lub organizacji odpowiedniej instytucji. Termin jest czêsto u¿ywany w œwiecie prawa, gdzie s¹ oni znani jako osobowe Ÿród³a informacji (OZI) i mo¿e czêsto odnosiæ siê pejoratywnie dostarczaniem informacji bez zgody innych w celu wyrz¹dzenia szkody lub dla zysku personalnego lub finansowego. Oprócz tego jest on te¿ u¿ywany w polityce, przemyœle i akademiach.'
WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Consulates
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Konsulaty[ENDCOLOR][NEWLINE]Uzyskujesz dodatkowego delegata na Kongresie Œwiatowym za ka¿de 8 pañstw-miast w grze a szansa na sfa³szowanie wyborów w pañstwie-mieœcie zwiêksza siê o 50%.'
WHERE Tag = 'TXT_KEY_POLICY_CONSULATES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Scholasticism
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Scholastyka[ENDCOLOR][NEWLINE]Wszystkie [COLOR_POSITIVE_TEXT]sojusznicze[ENDCOLOR] pañstwa-miasta zapewniaj¹ premiê do [ICON_RESEARCH] nauki równ¹ 33% tego, co same wytwarzaj¹.'
WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Cultural Diplomacy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Dyplomacja kulturowa[ENDCOLOR][NEWLINE]Iloœæ surowców darowanych przez pañstwa-miasta zwiêksza siê o 100%. [ICON_HAPPINESS_1] zadowolenie wynikaj¹ce z darowanych artyku³ów luksusowych zwiêksza siê o 50%.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Merchant Confederacy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Konfederacja kupiecka[ENDCOLOR][NEWLINE]+3 do [ICON_GOLD] z³ota ze szlaków handlowych z pañstwami-miastami. Zyskujesz +1 [ICON_INFLUENCE] wp³ywu (na turê) w pañstwie-mieœcie, z którym masz [ICON_INTERNATIONAL_TRADE] szlak handlowy.'
WHERE Tag = 'TXT_KEY_POLICY_MERCHANT_CONFEDERACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Piety Opener
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Pobo¿noœæ[ENDCOLOR] kapitalizuje twoje inwestycje w religiê i handel.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Pobo¿noœci zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET]+100% nacisku w pobliskich miastach bez twojej dominuj¹cej [ICON_RELIGION] religii. [NEWLINE] [ICON_BULLET] Wszystkie religie (albo tylko za³o¿ona przez ciebie [ICON_RELIGION] religia, jeœli masz jedn¹) szerz¹ siê do posiadanych miast bez jej dominacji 50% dalej.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty pobo¿ny ustrój zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +3% [ICON_GOLD] z³ota w miastach ze œwi¹tyni¹.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich pobo¿nych ustrojów zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Uniwersytet Sankore[ENDCOLOR]. [NEWLINE] [ICON_BULLET] +33% [ICON_FOOD] ¿ywnoœci lub [ICON_PRODUCTION] produkcji z wewnêtrznych [ICON_INTERNATIONAL_TRADE] szlaków handlowych wychodz¹cych z twojej [ICON_CAPITAL] stolicy lub œwiêtego miasta. [NEWLINE] [ICON_BULLET] Miasta, które wyznaj¹ twoj¹ dominuj¹c¹ [ICON_RELIGION] religiê generuj¹ +2 wszystkich zysków.[NEWLINE] [ICON_BULLET] Zapewnia dostêp do renesansowych drzew ustrojów, ignoruj¹c wymagania epoki. [NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie dostêpnych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_ARTIST] wielkich artystów za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Organized Religion
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Hierarchia koœcielna[ENDCOLOR][NEWLINE] -25% kosztów zakupów w [ICON_PEACE] wierze. +1 [ICON_PEACE] wiary i +1 [ICON_CULTURE] kultury ze œwi¹tyñ.'
WHERE Tag = 'TXT_KEY_POLICY_ORGANIZED_RELIGION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Mandate of Heaven (Now Syncretism)
UPDATE Language_pl_PL
SET Text = 'Synkretyzm'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Synkretyzm[ENDCOLOR][NEWLINE] +2 [ICON_RESEARCH] nauki i +2 [ICON_PRODUCTION] produkcji z targowisk, karawanserajów i przystani w miastach wyznaj¹cych twoj¹ religiê. Twoja religia szerzy siê 50% szybciej przez [ICON_INTERNATIONAL_TRADE] szlaki handlowe.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATE_OF_HEAVEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Synkretyzm to ³¹czenie ró¿nych, czêsto sprzecznych wierzeñ, poprzez mieszanie ze sob¹ praktyk rozmaitych szkó³ naukowych. Synkretyzm wi¹¿e siê z fuzj¹ i przeprowadzaniem analogii pierwotnie odrêbnych tradycji, zw³aszcza w teologii i mitologii religijnej, przez to zapewniaj¹ le¿¹c¹ u podstaw jednoœæ i pozwala na w³¹czeniowe spojrzenie na inne wiary.'
WHERE Tag = 'TXT_KEY_POLICY_MANDATEOFHEAVEN_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Theocracy (Now Divine Right)
UPDATE Language_pl_PL
SET Text = 'Boskie prawo'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Boskie prawo[ENDCOLOR][NEWLINE]Rozrost granic miast jest dwukrotnie szybszy podczas [ICON_GOLDEN_AGE] z³otych er i "Dni uwielbienia dla króla". Œwi¹tynie generuj¹ +1 [ICON_HAPPINESS_1] zadowolenia.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Boskie prawo królów, albo teoria królowania z prawa boskiego, jest polityczn¹ i religijn¹ doktryn¹ królewskiej i politycznej legitymizacji. Zak³ada, ¿e monarcha nie podlega ¿adnej ziemskiej w³adzy, otrzymuj¹c swoje prawo do rz¹dzenia bezpoœrednio z woli boskiej. Król, w zwi¹zku z tym, nie podlega woli jego ludu, arystokracji czy jakichkolwiek innych podmiotów królestwa, w³¹czaj¹c w to (w rozumieniu niektórych, zw³aszcza protestanckich krajów) koœció³. Wedle tej doktryny, tylko Bóg móg³ s¹dziæ króla. Doktryna implikuje, ¿e jakakolwiek próba pozbycia siê króla, albo ograniczenia jego w³adzy jest sprzeczna z wol¹ bosk¹ i mo¿e stanowiæ akt œwiêtokradztwa. Jest czêsto wyra¿ane w s³owach "z Bo¿ej £aski" do³¹czanych do tytu³ów rz¹dz¹cych monarchów.'
WHERE Tag = 'TXT_KEY_POLICY_THEOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Reformation (Now Monasticism)
UPDATE Language_pl_PL
SET Text = 'Monastycyzm'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Monastycyzm[ENDCOLOR][NEWLINE] Mo¿esz kupowaæ klasztory za [ICON_PEACE] wiarê (+2 [ICON_PEACE] wiary, +2 [ICON_FOOD] ¿ywnoœci, +2 [ICON_RESEARCH] nauki, 1 miejsce na naukowca).'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Monastycyzm to forma ¿ycia religijnego, której podstaw¹ jest ¿ycie, zgodne z regu³¹ zakonn¹, we wspólnocie zakonnej (klasztor); w chrzeœcijañstwie, a tak¿e m.in. w religiach pogañskich (westalki), w judaizmie (qumrañczycy), w buddyzmie i in. Monastycyzm ma na celu kult religijny - s³u¿bê Bogu czy d¹¿enie do samorealizacji przez czystoœæ duchow¹, ubóstwo, umartwianie siê, modlitwê, kontemplacjê lub medytacjê.'
WHERE Tag = 'TXT_KEY_POLICY_REFORMATION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Free Religion (Now Tolerance)
UPDATE Language_pl_PL
SET Text = 'Ikonografia'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Ikonografia[ENDCOLOR][NEWLINE]Wszyscy specjaliœci produkuj¹ +1 [ICON_PEACE] wiary. [ICON_GREAT_ARTIST] wielki artysta pojawia siê obok twojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_POLICY_FREE_RELIGION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ikonografia to ga³¹Ÿ historii sztuki, która bada identyfikacjê, opisy i interpretacjê zawartoœci obrazów: przedstawiony podmiot, szczególn¹ kompozycjê i detale u¿yte w tym celu i inne elementy, które s¹ odrêbne dla stylu artystycznego. Jedno ze znaczeñ ikonografii (oparte na niestandardowym t³umaczeniu odpowiednich greckich i rosyjskich odpowiedników) to wytwarzanie obrazów religijnych, nazywanych ikonami, w tradycji bizantyjskiej i prawos³awnej. W historii sztuki "Ikonografia" mo¿e te¿ oznaczaæ szczególne przedstawienie przedmiotu w znaczeniu treœci obrazu, jak liczba u¿ytych postaci, ich umiejscowienie i gestykulacja. Termin jest te¿ u¿ywany w wielu dziedzinach akademickich innych ni¿ historia sztuki, np. w semiotyce i studiach o mediach oraz przez laików na okreœlenie zawartoœci obrazów, typowe przedstawianie rzeczy na obrazach i powi¹zanych scen. Czasami rozró¿nia siê ikonologiê i ikonografiê, chocia¿ definicje, i przez rozró¿nienie bywa ró¿ne. W odniesieniu do filmów, gatunki s¹ natychmiast rozpoznawane przez swoj¹ ikonografiê, motywy, które s¹ zwi¹zane z pewnymi gatunkami przez swoj¹ powtarzalnoœæ.'
WHERE Tag = 'TXT_KEY_POLICY_FREERELIGION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monastery
UPDATE Language_pl_PL
SET Text = '+2 [ICON_FOOD] ¿ywnoœci, +2 [ICON_RESEARCH] nauki, +2 [ICON_PEACE] wiary i 1 miejsce na naukowca.'
WHERE Tag = 'TXT_KEY_BUILDING_MONASTERY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Klasztory zwiêkszaj¹ wytwarzanie [ICON_PEACE] wiary, [ICON_FOOD] ¿ywnoœci i [ICON_RESEARCH] nauki, zw³aszcza jeœli mo¿esz sobie pozwoliæ na u¿ycie specjalisty naukowca. Mog¹ zostaæ zbudowane jedynie, gdy przyjmiesz ustrój "Monastycyzm" z drzewa Pobo¿noœci. Za ich budowê trzeba zap³aciæ [ICON_PEACE] wiar¹.'
WHERE Tag = 'TXT_KEY_BUILDING_MONASTERY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener -- Free Science per city when happy, additional per policy taken. Closer grants 33% boost to GS, 20% boost to Growth.

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Racjonalizm[ENDCOLOR] skupia siê maksymalizacji wytwarzania [ICON_FOOD] wzrostu i [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Racjonalizmu zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Rozpoczyna siê [ICON_GOLDEN_AGE] z³ota era. [NEWLINE] [ICON_BULLET] +5% [ICON_RESEARCH] nauki kiedy twoje imperium jest [ICON_HAPPINESS_1] zadowolone.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty racjonalny ustrój zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +1% [ICON_RESEARCH] nauki kiedy twoje imperium jest [ICON_HAPPINESS_1] zadowolone.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich racjonalnych ustrojów zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Porcelanowa Wie¿a[ENDCOLOR]. [NEWLINE] [ICON_BULLET] +33% do prêdkoœci pozyskiwania [ICON_GREAT_SCIENTIST] wielkich naukowców we wszystkich miastach. [NEWLINE] [ICON_BULLET] +25% do [ICON_FOOD] wzrostu we wszystkich miastach.[NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie dostêpnych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_SCIENTIST] wielkich naukowców za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_RATIONALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Humanism (now Enlightenment) -- boost when happy
UPDATE Language_pl_PL
SET Text = 'Oœwiecenie'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Oœwiecenie[ENDCOLOR][NEWLINE]+10% do [ICON_RESEARCH] nauki i +10% do [ICON_FOOD] wzrostu we wszystkich miastach podczas [ICON_GOLDEN_AGE] z³otych er.'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Epoka oœwiecenia (albo proœciej oœwiecenie lub wiek rozumu) to okres, w którym nasta³y zmiany kulturowe i spo³eczne podkreœlaj¹ce raczej rozs¹dek, analizê i indywidualizm ni¿ tradycyjne sposoby myœlenia. Zapocz¹tkowany przez filozofów, maj¹cy pocz¹tek w póŸnym wieku XVII w Europie Zachodniej, proces napêdzany przez ochotnicze organizacje ludzi, które by³y poœwiêcone polepszeniu spo³eczeñstwa. Osoby te zwo³ywa³y siê w kawiarniach, salonach, lo¿ach masoñskich. Instytucje, które by³y g³êboko zakorzenione w spo³eczeñstwie, takie jak religia czy rz¹d, zaczê³y byæ podwa¿ane i coraz wiêksz¹ uwagê poœwiêcano sposobom reformacji spo³eczeñstwa z tolerancj¹, nauk¹ i sceptycyzmem.'
WHERE Tag = 'TXT_KEY_POLICY_HUMANISM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Scientific Revolution
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Spo³ecznoœæ akademicka[ENDCOLOR][NEWLINE]+1 [ICON_RESEARCH] nauki i +1 [ICON_GOLD] z³ota za ka¿dego specjalistê.'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFIC_REVOLUTION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Spo³ecznoœæ akademicka'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFIC_REVOLUTION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Spo³ecznoœæ akademicka to rozpoznawalny w kraju i na œwiecie establishment nauczycieli i studentów, czêsto zorganizowany wokó³ koled¿ów i uniwersytetów, które s¹ zaanga¿owane w szkolnictwo wy¿sze i badania. S³owo pochodzi od greckiego "akademeia", które pochodzi od ateñskiego herosa, Akademosa. Poza murami Aten, gimnazjon sta³ siê dziêki Platonowi znany jako centrum nauczania. Œwiête miejsce, poœwiêcone bogini m¹droœci, Atenie, by³o dawniej gajem oliwnym, st¹d te¿ wyra¿enie "gaj Akademosa".'
WHERE Tag = 'TXT_KEY_POLICY_SCIENTIFIC_REVOLUTION_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Secularism

UPDATE Language_pl_PL
SET Text = 'Rewolucja naukowa'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Rewolucja naukowa[ENDCOLOR][NEWLINE]+10% modyfikator œwiatów zysków z globalnych monopoli (w³¹czaj¹c [ICON_GOLDEN_AGE] z³ote ery) i +2 do zysków z globalnych monopoli (w³¹czaj¹c [ICON_HAPPINESS_1] zadowolenie).'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rewolucja naukowa to gwa³towne przejœcie od jednego do drugiego sposobu widzenia œwiata o odmiennych za³o¿eniach i kanonach naukowoœci, powoduj¹ce przewartoœciowanie panuj¹cego w nauce paradygmatu. W filozofii nauki i krytycznej historii nauki istniej¹ dwa odmienne spojrzenia na rewolucjê naukow¹. Rewolucje s¹ to doœæ rzadkie i krótkie epizody w dziejach nauki, przetykane znacznie d³u¿szymi okresami nauki normalnej. Jest to ujêcie Thomasa Kuhna. Rewolucja w nauce jest zjawiskiem permanentnym, starcia miêdzy teoriami o du¿ym stopniu ogólnoœci zachodz¹ znacznie czêœciej. Nauka normalna jest zjawiskiem dla nauki niebezpiecznym, poniewa¿ doprowadziæ mo¿e do koñca autentycznej refleksji naukowej. Jest to ujêcie Karla Poppera. Zwolennikiem permanentnego charakteru rewolucji w nauce by³ równie¿ Paul Feyerabend. Rewolucje w nauce s¹ skorelowane z przemianami w kulturze. Nauka jest z³o¿onym zjawiskiem oddaj¹cym z³o¿onoœæ i zmiennoœæ danej kultury, w którym wspó³wystêpuj¹ tendencje rewolucyjne i zachowawcze. Jest to ujêcie umiarkowanej socjologii i antropologii wiedzy: Karol Marks, Richard Rorty, Robert Maxwell Young.'
WHERE Tag = 'TXT_KEY_POLICY_SECULARISM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Sovereignty

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Empiryzm[ENDCOLOR][NEWLINE]+3 [ICON_RESEARCH] nauki i +2 [ICON_PRODUCTION] produkcji ze wszystkich surowców strategicznych.'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Empiryzm'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Empiryzm to teoria stanowi¹ca, ¿e wiedza pochodzi przede wszystkim z doœwiadczenia sensorycznego. Jedno z kilku spojrzeñ na epistemologiê, naukê o wiedzy cz³owieka, obok racjonalizmu i sceptycyzmu, empiryzm stawia rolê doœwiadczeñ i dowodów, zw³aszcza doœwiadczeñ sensorycznych, w formowaniu idei, ponad wyobra¿enia narzucanych z góry koncepcji i tradycji; empirycy mog¹ te¿ jednak dowodziæ, ¿e tradycje (lub zwyczaje) powstaj¹ w wyniku zwi¹zku z wczeœniejszymi doœwiadczeniami empirycznymi.'
WHERE Tag = 'TXT_KEY_POLICY_SOVEREIGNTY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Free Thought
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Wolnomyœlicielstwo[ENDCOLOR][NEWLINE]+2 [ICON_RESEARCH] nauki i [ICON_CULTURE] kultury z wiosek oraz -100% [ICON_HAPPINESS_3] niepokojów religijnych we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_POLICY_FREE_THOUGHT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Opener
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Tradycja[ENDCOLOR] skupia siê na generowaniu wielkich ludzi i tworzeniu imponuj¹cej stolicy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie Tradycji zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +3 [ICON_CULTURE] kultury i +2 [ICON_FOOD] ¿ywnoœci w [ICON_CAPITAL] stolicy. [NEWLINE] [ICON_BULLET] +5% do [ICON_FOOD] wzrostu we wszystkich miastach.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ka¿dy przyjêty ustrój tradycyjny zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] +4% do [ICON_FOOD] wzrostu we wszystkich miastach i +1 [ICON_FOOD] ¿ywnoœci w twojej [ICON_CAPITAL] stolicy.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjêcie wszystkich tradycyjnych ustrojów zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Wisz¹ce Ogrody[ENDCOLOR]. [NEWLINE] [ICON_BULLET] +1 [ICON_FOOD] ¿ywnoœci z ulepszeñ wielkich ludzi i s³ynnych miejsc a w [ICON_CAPITAL] stolicy zostaje wybudowana [COLOR_CYAN]sala tronowa[ENDCOLOR] (+1 do wszystkich zysków, 1 specjalista muzyk, 1  miejsce na [ICON_GREAT_WORK] wielkie dzie³o muzyczne, [ICON_GOLDEN_AGE] z³ote ery trwaj¹ 25% d³u¿ej).[NEWLINE] [ICON_BULLET] Zapewnia dostêp do œredniowiecznych drzew ustrojów, ignoruj¹c wymagania epoki.[NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostêpu do ideologii. [NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_GREAT_ENGINEER] wielkich in¿ynierów za [ICON_PEACE] wiarê poczynaj¹c od epoki przemys³owej.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Aristocracy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Sprawiedliwoœæ[ENDCOLOR][NEWLINE]Miasta z garnizonem zyskuj¹ +25% [ICON_RANGE_STRENGTH] dystansowej si³y bojowej.W [ICON_CAPITAL] stolicy zostaje wybudowana [COLOR_CYAN]królewska kordegarda[ENDCOLOR]  (1 specjalista in¿ynier, +3 [ICON_STRENGTH] obronnoœci, +25 punktów ¿ywotnoœci, +2 [ICON_PRODUCTION] produkcji).'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sprawiedliwoœæ'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sprawiedliwoœæ, w swoim najszerszym znaczeniu, obejmuje zarówno osi¹ganie tego, co jest sprawiedliwe jak i filozoficzn¹ dyskusjê o tym, co takie jest. Koncept sprawiedliwoœci jest oparty o wiele dziedzin i ró¿ni¹cych siê sposobów widzenia, w³¹czaj¹c w to pojêcia moralnej poprawnoœci opartej na prawie, s³usznoœci, etyce, racjonalnoœci, religii i uczciwoœci. Ogólna dyskusja o sprawiedliwoœci jest czêsto dzielona na sferê sprawiedliwoœci spo³ecznej, jak w filozofii, teologii i religii oraz sprawiedliwoœæ formaln¹ jak w nauce i praktyce prawnej.'
WHERE Tag = 'TXT_KEY_POLICY_ARISTOCRACY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Oligarchy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Niezale¿noœæ[ENDCOLOR][NEWLINE]Kulturowy koszt pozyskiwania pól obni¿ony o 20% we wszystkich miastach. W [ICON_CAPITAL] stolicy zostaje wybudowana [COLOR_CYAN]kaplica królewska[ENDCOLOR] (1 specjalista artysta, 1 [ICON_GREAT_WORK] miejsce na wielkie dzie³o sztuki, +2 [ICON_PEACE] wiary).'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Niezale¿noœæ'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Niezale¿noœæ jest rozumiana w prawoznawstwie jako pe³ne prawo i w³adza cia³a rz¹dz¹cego do stanowienia o sobie bez interferencji z zewnêtrznych Ÿróde³ i podmiotów. W teorii politycznej, niezale¿noœæ to istotne wyznaczaj¹ce pojêcie wyznaczaj¹ce nadrzêdn¹ w³adzê nad jak¹œ gesti¹. Jest to podstawowa zasada le¿¹ca u podstaw westfalskiego modelu kraju.'
WHERE Tag = 'TXT_KEY_POLICY_OLIGARCHY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Monarchy
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Majestat[ENDCOLOR][NEWLINE]Specjaliœci w [ICON_CAPITAL] stolicy konsumuj¹ po³owê normalnej iloœci [ICON_FOOD] ¿ywnoœci. W [ICON_CAPITAL] stolicy zostaje wybudowany [COLOR_CYAN]skarb pañstwa[ENDCOLOR] (1 specjalista kupiec, +2 [ICON_GOLD] z³ota, +15% do prêdkoœci narodzin [ICON_GREAT_PEOPLE] wielkich ludzi we wszystkich miastach).'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Majestat'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Majestat(moœæ) to polskie s³owo wywodz¹ce siê z ³aciñskiego "maiestas", oznaczaj¹cego wielkoœæ, i jest u¿ywane w odniesieniu do wielu monarchów, czêsto króli i cesarzów. W praktyce u¿ywa siê zwrotu "Jego/Jej (Królewska) Moœæ". Wystêpuje te¿ w wielu innych jêzykach.'
WHERE Tag = 'TXT_KEY_POLICY_MONARCHY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Landed Elite
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Splendor[ENDCOLOR][NEWLINE]Poœwiêcenie wielkiej osoby zapewnia 50 punktów [ICON_GOLDEN_AGE] z³otej ery i [ICON_CULTURE] kultury, skaluje siê zale¿nie od epoki. W [ICON_CAPITAL] stolicy zostaj¹ wybudowane [COLOR_CYAN]ogrody pa³acowe[ENDCOLOR] (1 specjalista pisarz, 1 [ICON_GREAT_WORK] miejsce na wielkie dzie³o literackie, +2 [ICON_CULTURE] kultury z pomników, ogrodów i ³aŸni).'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Splendor'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Splendor to pojêcie czêsto u¿ywane do okreœlenia publicznych dzia³añ monarchów i arystokratów, odnosi siê szczególnie do pokazywania w³adzy lub presti¿u.'
WHERE Tag = 'TXT_KEY_POLICY_LANDED_ELITE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Legalism
UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Ceremonia[ENDCOLOR][NEWLINE]Pa³ac i cuda narodowe z wymaganiami budynków zapewniaj¹ +2 [ICON_HAPPINESS_1] zadowolenia. W [ICON_CAPITAL] stolicy zostaje wybudowany [COLOR_CYAN]astrolog królewski[ENDCOLOR] (1 specjalista naukowiec, +2 [ICON_RESEARCH] nauki).'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ceremonia'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ceremonia to wydarzenie o znaczeniu rytualnym, wykonywana na specjalne okazje. S³owo mo¿e mieæ etruskie pochodzenie, poprzez ³aciñskie caerimonia. Jednym z g³ównych oznak dworów jest ceremonia. Na wiêkszoœci dworów monarszych odbywa³y siê ceremonie dotycz¹ce inwestytury albo koronacji monarchy i audiencji u niego. Niektóre dwory mia³y ceremonie zwi¹zane z budzeniem siê i spaniem monarchy, zwane levée. Nadawanie szlachectwa jako nadawanie zaszczytów sta³o siê wa¿n¹ czêœci¹ kultury dworskiej poczynaj¹c od XV wieku. Prawem monarchy by³o jego przyznawanie, jako ¿e by³ on fons honorum (dos³. "fontanna zaszczytów").'
WHERE Tag = 'TXT_KEY_POLICY_LEGALISM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

-- Finisher