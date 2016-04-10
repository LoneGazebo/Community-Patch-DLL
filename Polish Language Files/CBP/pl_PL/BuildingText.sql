--translated by mrlg
UPDATE Language_pl_PL
SET Text = '+1 [ICON_RESEARCH] nauki za ka¿dych 3 [ICON_CITIZEN] mieszkañców w tym mieœcie. Zawiera 1 miejsce na wielkie dzie³o sztuki.[NEWLINE][NEWLINE]Kiedy jedno z wymienionych [COLOR_POSITIVE_TEXT]wydarzeñ historycznych[ENDCOLOR] zostaje ukoñczone, otrzymujesz premiê do [ICON_TOURISM] turystyki we wszystkich znanych cywilizacjach zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury:[NEWLINE][ICON_BULLET] Zdob¹dŸ [ICON_GREAT_PEOPLE] wielk¹ osobê[NEWLINE][ICON_BULLET] Zbuduj [ICON_GOLDEN_AGE] cud œwiata[NEWLINE][ICON_BULLET] Wygraj [ICON_WAR] wojnê[NEWLINE][ICON_BULLET] WejdŸ w now¹ [ICON_RESEARCH] epokê[NEWLINE][NEWLINE][ICON_CONNECTED] £¹czenie miast ze [ICON_CAPITAL] stolic¹ drogami bêdzie zapewnia³o dodatkowe [ICON_GOLD] z³oto.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '10% wytwarzanej przez miasto [ICON_FOOD] ¿ywnoœci dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE]Pozwala na transport [ICON_FOOD] ¿ywnoœci z tego miasta szlakami handlowymi w obrêbie twojej cywilizacji. Pobliska [ICON_RES_WHEAT] pszenica: +1 [ICON_FOOD] ¿ywnoœci.[NEWLINE]Pobliskie [ICON_RES_BANANA] banany: +1 [ICON_FOOD] ¿ywnoœci.[NEWLINE]Pobliska [ICON_RES_DEER] zwierzyna: +1 [ICON_FOOD] ¿ywnoœci.[NEWLINE]Pobliskie [ICON_RES_BISON] bizony: +1 [ICON_FOOD] ¿ywnoœci.'
WHERE Tag = 'TXT_KEY_BUILDING_GRANARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_FOOD] ¿ywnoœci i +1 [ICON_PRODUCTION] produkcji za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta. [NEWLINE][NEWLINE]Miasto musi byæ po³o¿one nad rzek¹. Nie mo¿na zbudowaæ w mieœcie, które ju¿ ma studniê.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'M³yn wodny mo¿e powstaæ tylko w mieœcie le¿¹cym nad rzek¹. Budynek ten zwiêksza [ICON_FOOD] ¿ywnoœæ i [ICON_PRODUCTION] produkcjê w mieœcie bardziej efektywnie (na obywatela w mieœcie) ni¿ studnia i ma wiêksze zyski bazowe.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '25% [ICON_FOOD] ¿ywnoœci jest dostarczane po narodzinach nowego [ICON_CITIZEN] obywatela. +1 [ICON_FOOD] ¿ywnoœci z jezior i oaz wykorzystywanych przez to miasto. Redukuje nieco [ICON_HAPPINESS_3] ubóstwo.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] ubóstwo. Jednostki w tym mieœcie lecz¹ 15 P¯ niezale¿nie od tego, czy wykona³y akcjê.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '10% wytwarzanej przez miasto [ICON_CULTURE] kultury dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki. +20% do pojawiania siê [ICON_GREAT_PEOPLE] wielkich ludzi w tym mieœcie.[NEWLINE]Pobliskie [ICON_RES_CITRUS] cytrusy: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_COCOA] kakao: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]Miasto musi le¿eæ nad rzek¹ lub nad jeziorem.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ogród o 20% zwiêksza tempo, w jakim w danym mieœcie pojawiaj¹ siê [ICON_GREAT_PEOPLE] wielcy ludzie. Ogrody mog¹ powstawaæ tylko w miastach NIE po³o¿onych nad rzek¹ lub jeziorem i nie mo¿na ich zbudowaæ jeœli w mieœcie jest ju¿ ³aŸnia.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_SHRINE_HELP', '20% wytwarzanej przez miasto[ICON_PEACE] wiary dodaje siê ka¿dej tury do [ICON_RESEARCH] nauki tego miasta.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_HOSPITAL_HELP', 'Redukuje [ICON_HAPPINESS_3] ubóstwo. Jednostki w tym mieœcie lecz¹ 15 P¯ niezale¿nie od tego, czy wykona³y akcjê.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_TEMPLE_HELP', 'Redukuje [ICON_HAPPINESS_3] niepokoje religijne.[NEWLINE]Pobliskie [ICON_RES_INCENSE] kadzid³o: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_WINE] wino: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] z³ota.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MONUMENT_HELP', 'Koszt [ICON_CULTURE] kulturalny pozyskiwania nowych pól zmniejszony o 25% dla tego miasta.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_AMPHITHEATER_HELP', '+33% do szybkoœci pojawiania siê [ICON_GREAT_WRITER] wielkich pisarzy w tym mieœcie, wszystkie gildie pisarzy produkuj¹ +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_DYE] barwniki: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] z³ota.[NEWLINE] Pobliski [ICON_RES_SILK] jedwab: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] z³ota.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Amfiteatr zwiêksza [ICON_CULTURE] kulturê miasta i zapewnia premiê z pobliskich barwników i jedwabiu. Podnosi te¿ szybkoœci pojawiania siê wielkich pisarzy w tym mieœcie i wartoœæ gildii pisarzy. Zawiera 1 miejsce na wielkie dzie³o literackie.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_OPERA_HOUSE_HELP', '+10% [ICON_CULTURE] kultury w tym mieœcie. +33% do szybkoœci pojawiania siê [ICON_GREAT_MUSICIAN] wielkich muzyków w tym mieœcie, wszystkie gildie muzyków produkuj¹ +1 [ICON_GOLD] z³ota.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Opera jest budynkiem renesansu, który przyczynia siê do rozwoju [ICON_CULTURE] kultury w mieœcie. Podnosi te¿ szybkoœci pojawiania siê wielkich muzyków w tym mieœcie i wartoœæ gildii muzyków. Zawiera 1 miejsce na wielkie dzie³o muzyczne. Przed wybudowaniem opery miasto musi posiadaæ amfiteatr.'
WHERE Tag = 'TXT_KEY_BUILDING_OPERA_HOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Muzeum to budynek ze œrodkowej czêœci gry zwiêkszaj¹cy wytwarzanie [ICON_CULTURE] kultury w mieœcie. Podnosi te¿ szybkoœci pojawiania siê wielkich artystów w tym mieœcie i wartoœæ gildii artystów. Zawiera 2 miejsca na wielkie dzie³a sztuki.  Miejscowoœæ musi posiadaæ operê, by zbudowaæ w niej muzeum.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MUSEUM_HELP', '+1 [ICON_CULTURE] kultury za ka¿dych 5 [ICON_CITIZEN] mieszkañców tego miasta. +33% do szybkoœci pojawiania siê [ICON_GREAT_ARTIS] wielkich artystów w tym mieœcie, wszystkie gildie artystów produkuj¹ +1 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]+3 [ICON_CULTURE] kultury jeœli jest motyw.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta.[NEWLINE][NEWLINE]+5 [ICON_GOLD] z³ota jeœli jest zape³niona sztuk¹.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wie¿a transmisyjna to budynek z póŸnej czêœci gry zwiêkszaj¹cy wytwarzanie [ICON_CULTURE] kultury w mieœcie. Miasto musi mieæ wybudowane muzeum.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pobliskie kopalnie dostarczaj¹ +1 [ICON_PRODUCTION] produkcji. +15 punktów doœwiadczenia dla wszystkich jednostek. Redukuj¹ nieco [ICON_HAPPINESS_3] przestêpczoœæ.'
WHERE Tag = 'TXT_KEY_BUILDING_BARRACKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = '+20 punktów doœwiadczenia dla wszystkich jednostek. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ.[NEWLINE][NEWLINE]W mieœcie musz¹ byæ wybudowane koszary.'
WHERE Tag = 'TXT_KEY_BUILDING_ARMORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+25 punktów doœwiadczenia dla wszystkich jednostek.[NEWLINE][NEWLINE]Miasto musi posiadaæ zbrojowniê.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_ACADEMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_pl_PL
SET Text = '+20% do [ICON_PRODUCTION] produkcji w trakcie szkolenia jednostek konnych.[NEWLINE]Pobliskie [ICON_RES_HORSE] konie: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliskie [ICON_RES_SHEEP] owce: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliskie [ICON_RES_COW] byd³o: +2 [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE]Miasto musi byæ po³o¿one w pobli¿u przynajmniej jednego z wymienionych Ÿróde³ z wybudowanym pastwiskiem.'
WHERE Tag = 'TXT_KEY_BUILDING_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = '10% wytwarzanej przez miasto [ICON_PRODUCTION] produkcji dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE]Pobliskie [ICON_RES_IRON] ¿elazo: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliska [ICON_RES_COPPER] miedŸ: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'KuŸnia ulepsza pobliskie Ÿród³a [ICON_RES_IRON] ¿elaza i [ICON_RES_COPPER] miedzi i zwiêksza [ICON_PRODUCTION] produkcjê z kopalni. Zwiêksza te¿ wytwarzanie przez miasto nauki zale¿nie od wielkoœci jego obecnej produkcji.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_PRODUCTION] produkcji ze wszystkich lasów i d¿ungli wykorzystywanych przez to miasto. Pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzd³u¿ szlaków handlowych wewn¹trz twojej cywilizacji.'
WHERE Tag = 'TXT_KEY_BUILDING_WORKSHOP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzd³u¿ szlaków handlowych wewn¹trz twojej cywilizacji.[NEWLINE]Pobliski [ICON_RES_MARBLE] marmur: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliski [ICON_RES_STONE] kamieñ: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_SALT] sól: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]Miasto potrzebuje co najmniej jednego z tych surowców, aby ulepszyæ kamienio³om.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Zak³ad kamieniarski mo¿e byæ zbudowany tylko w mieœcie, które posiada dostêp do ulepszonych [ICON_RES_STONE] zasobów kamienia, [ICON_RES_SALT] soli, lub [ICON_RES_MARBLE] marmuru. Zak³ad kamieniarski zwiêksza [ICON_PRODUCTION] produkcjê i pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzd³u¿ szlaków handlowych wewn¹trz twojej cywilizacji.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
		
UPDATE Language_pl_PL
SET Text = '+1 [ICON_FOOD] ¿ywnoœci i [ICON_GOLD] z³ota z pól wybrze¿a i oceanu. +1 [ICON_PRODUCTION] produkcji z surowców morskich wykorzystywanych przez to miasto.[NEWLINE][NEWLINE]Miasto musi byæ po³o¿one na wybrze¿u.'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kiedy wywodz¹cy siê st¹d [ICON_INTERNATIONAL_TRADE] morski szlak handlowy prowadz¹cy do innej cywilizacji zostanie ukoñczony, otrzymujesz premiê do [ICON_TOURISM] turystki w tej cywilizacji zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury.[NEWLINE][NEWLINE]+1 [ICON_FOOD] ¿ywnoœci z pól wybrze¿a i oceanu. Morskie szlaki handlowe maj¹ o 50% wiêkszy zasiêg i zapewniaj¹ 2 dodatkowe sztuki [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]+15% do [ICON_PRODUCTION] produkcji jednostek morskich. Tworzy wodny [ICON_CONNECTED] szlak handlowy ze [ICON_CAPITAL] stolic¹. Miasto musi le¿eæ na wybrze¿u.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przystañ to budynek epoki œredniowiecznej. Zwiêksza zasiêg i zysk [ICON_GOLD] z³ota z morskich szlaków handlowych. Pozwala na tworzenie „szlaków wodnych” (zobacz zasady) miêdzy miastami. Przyspiesza te¿ [ICON_PRODUCTION] produkcjê jednostek wodnych 15%.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga 1 [ICON_RES_COAL] wêgla. +1 [ICON_FOOD] produkcji i +1 [ICON_GOLD] z³ota ze wszystkich pól wodnych i wybrze¿a. +2 [ICON_PRODUCTION] produkcji i [ICON_GOLD] z³ota z zasobów morskich wykorzystywanych przez to miasto.[NEWLINE][NEWLINE]Miasto musi le¿eæ na wybrze¿u.'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_MARKET_HELP', '10% wytwarzanego przez miasto [ICON_GOLD] z³ota dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki. +1 [ICON_GOLD] z³ota ze [ICON_INTERNATIONAL_TRADE] szlaków handlowych do/z miasta.[NEWLINE][NEWLINE]Pobliskie [ICON_RES_SPICES] przyprawy: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliski [ICON_RES_SUGAR] cukier: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Targowisko zwiêksza iloœæ [ICON_GOLD] z³ota generowanego przez miasto i ulepsza pobliskie Ÿród³a [ICON_RES_SPICES] przypraw i [ICON_RES_SUGAR] cukru. Szlaki handlowe tworzone przez innych graczy do tego miasta bêd¹ generowaæ 1 dodatkow¹ sztukê [ICON_GOLD] z³ota dla w³aœciciela miasta i dla w³aœciciela szlaku handlowego.'
WHERE Tag = 'TXT_KEY_BUILDING_MARKET_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kiedy wywodz¹cy siê st¹d [ICON_INTERNATIONAL_TRADE] dowolny szlak handlowy prowadz¹cy do innej cywilizacji zostanie ukoñczony, otrzymujesz premiê do [ICON_TOURISM] turystyki w tej cywilizacji zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury.[NEWLINE][NEWLINE]Szlaki handlowe do/z tego miasta zapewniaj¹ +2 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]Miasto musi posiadaæ targowisko.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Urz¹d celny zwiêksza wartoœæ w z³ocie szlaków handlowych i zapewnia premiê do turystki, któr¹ generujesz ze szlaków handlowych do innych cywilizacji. Postaw te budynki we wszystkich miastach, jeœli chcesz polepszyæ swoje wytwarzanie z³ota i wartoœæ kulturaln¹ twoich szlaków handlowych.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Urz¹d celny|Urzêdu celnego|Urzêdzie celnemu|Urz¹d celny|Urzêdy celne|Urzêdów celnych|Urzêdom celnym|Urzêdy celne', Gender = 'masculine', Plurality = '1|1|1|1|2|2|2|2'
WHERE Tag = 'TXT_KEY_BUILDING_MINT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Urz¹d celny do budynek mieszcz¹cy biura rz¹dowych oficjeli wykonuj¹cy papierkow¹ robotê zwi¹zan¹ z importem oraz eksportem dóbr do luz z kraju. Pobierali oni równie¿ op³aty celne za importowane dobra. Urzêdy celne by³y zwykle umieszczone w porcie morskim lub w mieœcie nad g³ówn¹ rzek¹ z dostêpem do oceanu. Te miasta s³u¿y³y jako porty wejœciowe do kraju. Rz¹dy mia³y urzêdników w takich lokacjach, aby zbieraæ podatki i regulowaæ handel. Z powodu rozwoju elektronicznych systemów informatycznych, zwiêkszenia siê rozmiaru handlu miêdzynarodowego i wprowadzeniem transportu powietrznego, urzêdy celne s¹ teraz czêsto historycznymi anachronizmami. Jest na œwiecie wiele przyk³adów budynków, które pierwotnie s³u¿y³y za urzêdy celne, ale teraz zosta³y przerobione do innych celów, jak na muzea lub budynki u¿ytecznoœci publicznej.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_MINT_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W mieœcie musi byæ targowisko. +1 [ICON_GOLD] z³ota za ka¿dych 5 [ICON_CITIZEN] mieszkañców tego miasta.[NEWLINE]Pobliskie [ICON_RES_GOLD] z³oto: +3 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_SILVER] srebro: +1 [ICON_GOLD] z³ota, +1 [ICON_PRODUCTION] produkcji, +1 [ICON_CULTURE] kultury.[NEWLINE]Pobliskie [ICON_RES_GEMS] kamienie szlachetne: +2 [ICON_CULTURE] kultury, +1 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W mieœcie musi byæ wybudowany bank. +1 [ICON_GOLD] z³ota za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta.'
WHERE Tag = 'TXT_KEY_BUILDING_STOCK_EXCHANGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kiedy wywodz¹cy siê st¹d [ICON_INTERNATIONAL_TRADE] l¹dowy szlak handlowy prowadz¹cy do innej cywilizacji zostanie ukoñczony, otrzymujesz premiê do [ICON_TOURISM] turystyki w tej cywilizacji zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury. L¹dowe szlaki handlowe maj¹ o 50% wiêkszy zasiêg i zapewniaj¹ 2 dodatkowe sztuki [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_TRUFFLES] trufle: +2 [ICON_GOLD] z³ota.[NEWLINE]Pobliska [ICON_RES_COTTON] bawe³na: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_CULTURE] kultury.[NEWLINE]Pobliskie [ICON_RES_FUR] futra: +1 [ICON_GOLD] z³ota, +1 [ICON_PRODUCTION] produkcji.'
WHERE Tag = 'TXT_KEY_BUILDING_CARAVANSARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje nieco [ICON_HAPPINESS_3] analfabetyzm. W mieœcie musi byæ wybudowana rada.'
WHERE Tag = 'TXT_KEY_BUILDING_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_RESEARCH] nauki z pól d¿ungli i lasu wykorzystywanych przez to miasto. Uzyskujesz 33% wytwarzanej w tym mieœcie [ICON_RESEARCH] nauki jako natychmiastow¹ premiê do obecnych prac naukowych, kiedy rodzi siê tu [ICON_CITIZEN] obywatel. Redukuje [ICON_HAPPINESS_3] analfabetyzm.[NEWLINE][NEWLINE]W mieœcie musi byæ wybudowana biblioteka.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
    
UPDATE Language_pl_PL
SET Text = '+1 [ICON_RESEARCH] za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta. Redukuje [ICON_HAPPINESS_3] analfabetyzm.[NEWLINE][NEWLINE]Pozwala na produkcjê archeologów w tym mieœcie.[NEWLINE][NEWLINE]W mieœcie musi byæ wybudowany uniwersytet.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ka¿de Ÿród³o [ICON_RES_ALUMINUM] aluminium i [ICON_RES_URANIUM] uranu wykorzystywane przez to miasto produkuje +3 [ICON_RESEARCH] nauki. +1 [ICON_RESEARCH] za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta. Znacz¹co redukuje [ICON_HAPPINESS_3] analfabetyzm.[NEWLINE][NEWLINE]Szpitale, fabryki i laboratoria medyczne wytwarzaj¹ +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE]Miasto musi posiadaæ szko³ê publiczn¹.'
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Miasto musi byæ po³o¿one w odleg³oœci dwóch pól od góry. Miasto otrzymuje +1 [ICON_RESEARCH] nauki na ka¿d¹ górê w odleg³oœci najwy¿ej 3 pól od miasta.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Obserwatorium mo¿e powstaæ jedynie w odleg³oœci dwóch pól od gór. Budynek ten znacz¹co zwiêksza [ICON_RESEARCH] wydajnoœæ naukow¹ miasta.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+15% [ICON_PRODUCTION] produkcji podczas wznoszenia budynków. Sklepy i spichlerze w tym mieœcie wytwarzaj¹ +2 [ICON_FOOD] ¿ywnoœci.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Wiatrak to budynek epoki renesansowej, który zwiêksza [ICON_PRODUCTION] produkcjê w mieœcie podczas wznoszenia budynków.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+3 [ICON_PRODUCTION] produkcji na ka¿dym polu obok rzeki. [NEWLINE][NEWLINE]Miasto musi byæ po³o¿one nad rzek¹. Wymaga [ICON_RES_ALUMINUM] aluminium.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Elektrownia wodna to budynek epoki wspó³czesnej, który mo¿e powstaæ jedynie w mieœcie po³o¿onym nad rzek¹. Zwiêksza [ICON_PRODUCTION] produkcjê na ka¿dym eksploatowanym przez miasto polu s¹siaduj¹cym z rzek¹.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Eliminuje dodatkowe [ICON_HAPPINESS_4] niezadowolenie w [ICON_OCCUPIED] okupowanym mieœcie. Koszt zwiêksza siê zale¿nie od iloœci miast w twoim imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_COURTHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mury zwiêkszaj¹ si³ê obronn¹ miasta o 6 i punkty ¿ywotnoœci o 50, dziêki czemu miasto jest trudniejsze do zdobycia. Budowla ta jest bardzo przydatna w miastach le¿¹cych na pograniczu cywilizacji.'
WHERE Tag = 'TXT_KEY_BUILDING_WALLS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zamek to budynek epoki œredniowiecznej, który zwiêksza si³ê obronn¹ o 8 i punkty ¿ywotnoœci o 75. Aby móc wznieœæ zamek, miasto musz¹ wczeœniej okalaæ mury.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Arsena³ jest budynkiem wojskowym z epoki przemys³owej, który zwiêksza si³ê obronn¹ o 10 i punkty ¿ywotnoœci o 100, co sprawia, ¿e miasto staje siê trudniejsze do zdobycia. Aby wznieœæ arsena³, w mieœcie musi byæ wybudowany zamek.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W mieœcie musi byæ wybudowany zamek.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+20% do [ICON_PRODUCTION] produkcji jednostek powietrznych. Zgarnizonowane jednostki otrzymuj¹ dodatkowe 10 P¯, kiedy lecz¹ siê w tym mieœcie. W mieœcie musi byæ zbudowany arsena³.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Baza wojskowa to budynek zwiêkszaj¹cy si³ê obronn¹ miasta o 12 i punkty ¿ywotnoœci o 125. Aby móc postawiæ bazê wojskow¹, w mieœcie musi byæ wybudowany arsena³. Zgarnizonowane jednostki otrzymuj¹ dodatkowe 5 P¯, kiedy lecz¹ siê w tym mieœcie.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'O 75% zmniejsza straty populacji po ataku atomowym, zwiêksza si³ê obronn¹ o 5 i [ICON_HAPPINESS_1] zadowolenie o 1.'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga 1 sztuki [ICON_RES_COAL] wêgla. +1 [ICON_PRODUCTION] produkcji za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta.'
WHERE Tag = 'TXT_KEY_BUILDING_FACTORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '50% [ICON_CULTURE] kultury z cudów œwiata, cudów naturalnych i ulepszeñ dodaje siê do wytwarzanej [ICON_TOURISM] turystyki przez miasto. [ICON_TOURISM] turystyka z wielkich prac o +50%.'
WHERE Tag = 'TXT_KEY_BUILDING_HOTEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Umo¿liwia transport powietrzny z miasta lub do miasta. [COLOR_POSITIVE_TEXT]Zwiêksza liczbê jednostek lotniczych z 6 do 10.[ENDCOLOR] 25% [ICON_CULTURE] kultury z cudów œwiata, cudów naturalnych i ulepszeñ dodaje siê do wytwarzanej [ICON_TOURISM] turystyki przez miasto. [ICON_TOURISM] turystyka z wielkich prac o +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_AIRPORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] znudzenie i zapewnia skromn¹ sumê [ICON_CULTURE] kultury po ukoñczeniu.[NEWLINE]Pobliska [ICON_RES_IVORY] koœæ s³oniowa: +3 [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Cyrk redukuje znudzenie w mieœcie i polepsza wytwarzanie kultury z [ICON_RES_IVORY] koœci s³oniowej. Wybuduj je, aby zwalczaæ niezadowolenie ze znudzenia i zyskaæ szybkie premie kultury.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BUILDING_COLOSSEUM_HELP', '+2 [ICON_TOURISM] turystyki. Koszary, kuŸnia i zbrojownia w tym mieœcie dostarczaj¹ +2 [ICON_PRODUCTION] produkcji. Redukuje nieco [ICON_HAPPINESS_3] znudzenie.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Arena generuje turystykê, redukuje znudzenie w mieœcie i zapewnia dodatkow¹ kulturê. Wybuduj je, aby zwalczaæ niezadowolenie ze znudzenia, zwiêkszyæ twoj¹ kulturê i polepszyæ produkcjê twoich budynków wojskowych.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Arena (z ³ac. harena, arena - piasek) – w staro¿ytnym teatrze greckim i rzymskim oraz w cyrku rzymskim, usytuowany w centrum takiego obiektu i wysypany piaskiem, plac w kszta³cie ko³a, elipsy. Miejsce walk, turniejów, popisów, igrzysk i widowisk. Arena cyrku oddzielona by³a od amfiteatralnej widowni rowem w celu zapewnienia widzom bezpieczeñstwa. Tor by³ podwójny, z wira¿em wokó³ s³upów na jednym koñcu. Przy drugim koñcu mieœci³y siê stajnie. Tory rozdziela³ niski mur (spina). W œredniowieczu arena by³a placem gonitw i turniejów rycerskich. W wieku XVIII areny bywa³y jednym z elementów barokowych kompozycji ogrodowych. Wspó³czeœnie termin u¿ywany jest w odniesieniu do cyrków, a tak¿e w licznych nazwach w³asnych, np. stadionów oraz w znaczeniu przenoœnym.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_COLISEUM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Arena|Areny|Arenie|Arenê|Areny|Aren|Arenom|Areny', Gender = 'feminine', Plurality = '1|1|1|1|2|2|2|2'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] znudzenie.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zoo redukuje znudzenie w mieœcie i produkuje dodatkowe punkty nauki i kultury.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala ci na wybudowanie [COLOR_POSITIVE_TEXT]Globe Theatre[ENDCOLOR], budynku zwiêkszaj¹cego [ICON_TOURISM] turystykê. Umo¿liwia te¿ za³o¿enie Kongresu Œwiatowego.'
WHERE Tag = 'TXT_KEY_TECH_PRINTING_PRESS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wybuduj stadion, jeœli zmierzasz do [COLOR_POSITIVE_TEXT]zwyciêstwa kulturalnego[ENDCOLOR], albo jeœli masz problemy z niezadowoleniem ze znudzenia w twoim imperium. Zapewnia du¿¹ iloœæ punktów z³otej ery po wybudowaniu.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Znacz¹co redukuje [ICON_HAPPINESS_3] znudzenie. Zapewnia du¿¹ sumê punktów [ICON_GOLDEN_AGE] z³otej ery po ukoñczeniu.[NEWLINE][NEWLINE]25% [ICON_CULTURE] kultury z cudów œwiata, cudów naturalnych i ulepszeñ dodaje siê do wytwarzanej [ICON_TOURISM] turystyki przez miasto. [ICON_TOURISM] turystyka z wielkich prac o +25%.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wielce redukuje [ICON_HAPPINESS_3] przestêpczoœæ. O 25% zmniejsza szybkoœæ, z jak¹ szpieg wroga dokonuje kradzie¿y.[NEWLINE][NEWLINE]W mieœcie musi byæ policja okrêgowa.'
WHERE Tag = 'TXT_KEY_BUILDING_POLICE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] przestêpczoœæ. Zmniejsza o 33% szybkoœæ, z jak¹ szpieg wroga dokonuje kradzie¿y.'
WHERE Tag = 'TXT_KEY_BUILDING_CONSTABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zapewnia 5 punktów [ICON_GREAT_PEOPLE] wielkich ludzi (PWL) niezbêdne do narodzin wielkiego muzyka. Umieœæ w tym budynku do dwóch specjalistów, by zdobywaæ [ICON_CULTURE] kulturê i zwiêkszyæ czêstotliwoœæ narodzin wielkich muzyków. [NEWLINE][NEWLINE]Maksymalnie 3 takie budynki w twoim imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSICIANS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zapewnia 4 punkty [ICON_GREAT_PEOPLE] wielkich ludzi (PWL) potrzebne do narodzin wielkiego artysty. Umieœæ w tym budynku do dwóch specjalistów, by zdobywaæ [ICON_CULTURE] kulturê i zwiêkszyæ czêstotliwoœæ narodzin wielkich artystów. [NEWLINE][NEWLINE]Maksymalnie 3 takie budynki w twoim imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_ARTISTS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zapewnia 3 punkty [ICON_GREAT_PEOPLE] wielkich ludzi (PWL) niezbêdny do narodzin wielkiego pisarza. Umieœæ w tym budynku do dwóch specjalistów, by zdobywaæ [ICON_CULTURE] kulturê i zwiêkszyæ czêstotliwoœæ narodzin wielkich pisarzy. [NEWLINE][NEWLINE]Maksymalnie 3 takie budynki w twoim imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_WRITERS_GUILD_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Monument Narodowy|Monumentu Narodowego|Monumentowi Narodowemu|Monument Narodowy|Monumenty Narodowe|Monumentów Narodowych|Monumentom Narodowym|Monumenty Narodowe', Gender = 'masculine', Plurality = '1|1|1|1|2|2|2|2'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Monument Narodowy to pomnik konstruowany w celu upamiêtnienia czegoœ o znaczeniu narodowym, jak wojna lub za³o¿enie nowego kraju. Okreœlenie mo¿e siê te¿ odnosiæ to specyficznego statusu pomnika, jak miejsce dziedzictwa narodowego, którym wiêkszoœæ z nich jest ze wzglêdu na swoje znaczenie kulturalne a nie na wiek. Monument Narodowy stara siê reprezentowaæ naród i s³u¿y jako ognisko dla to¿samoœci narodowej.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Szko³a Filozofii|Szko³y Filozofii|Szkole Filozofii|Szko³ê Filozofii|Szko³y Filozofii|Szkó³ Filozofii|Szko³om Filozofii|Szko³y Filozofii', Gender = 'feminine', Plurality = '1|1|1|1|2|2|2|2'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Filozofia to nauka o ogólnych i fundamentalnych problemach, takich jak te powi¹zane z rzeczywistoœci¹, egzystencj¹, wiedz¹, wartoœciami, przyczynami, œwiadomoœci¹ i jêzykiem. By³a ona nauczana w szko³ach myœli i odró¿nia³a siê od innych dróg sposobem odnoszenia siê do tych zagadnieñ poprzez krytyczne, usystematyzowane podejœcie i jej poleganiu na racjonalnych argumentach. W prostszym jêzyku, rozszerzaj¹c, filozofia mo¿e siê odnosiæ do wiêkszoœci podstawowych wierzeñ, konceptów oraz postaw pojedynczych ludzi b¹dŸ grup. S³owo filozofia pochodzi z greki i oznacza dos³ownie "umi³owanie m¹droœci". Wprowadzenie terminów filozof i filozofia jest przypisywane greckiemu myœlicielowi, Pitagorasowi.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );
UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy daje wszystkim jednostkom szkolonym w tym mieœcie awans „Morale”, zwiêkszaj¹cy ich si³ê bojow¹ o +10%. Daje równie¿ +1 do [ICON_CULTURE] kultury. Epos mo¿e zostaæ stworzony, gdy miasto posiada koszary.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy zwiêksza tempo pojawiania siê [ICON_GREAT_PEOPLE] wielkich ludzi w mieœcie o 25%. Daje równie¿ +1 do [ICON_CULTURE] kultury. Aby móc stworzyæ Monument Narodowy, miasto musi posiadaæ pomnik.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy zapewnia +1 do [ICON_CULTURE] kultury, redukuje [ICON_HAPPINESS_3] znudzenie i generuje +10% [ICON_CULTURE] kultury i [ICON_GOLD] z³ota podczas "Dnia uwielbienia dla króla" w mieœcie, w którym zosta³ wybudowany. Aby móc stworzyæ Circus Maximus, miasto musi posiadaæ arenê.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy zwiêksza produkcjê [ICON_GOLD] z³ota w mieœcie i redukuje [ICON_HAPPINESS_3] ubóstwo. Uzyskujesz te¿ darmow¹ kopiê wszystkich surowców luksusowych dooko³a miasta. Szlaki handlowe innych graczy do miasta z Kompani¹ Wschodnioindyjsk¹ bêd¹ generowa³y dodatkowe 4 sztuki [ICON_GOLD] z³ota dla w³aœciciela miasta a w³aœciciel szlaku handlowego otrzymuje dodatkowe 2 sztuki [ICON_GOLD] z³ota ze szlaku.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy wytwarza +1 [ICON_RESEARCH] nauki za ka¿dych 2 [ICON_CITIZEN] mieszkañców tego miasta i zapewnia +1 do [ICON_CULTURE] kultury. Aby móc zbudowaæ Szko³ê Filozofii, miasto musi posiadaæ bibliotekê. Do skonstruowania wymaga populacji kraju wynosz¹cej co najmniej 35.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten narodowy cud zwiêksza produkcjê w mieœcie o 10 [ICON_PRODUCTION]. i generuje [ICON_RESEARCH] naukê za ka¿dym razem, gdy wybudujesz budynek. Aby móc zbudowaæ Hutê, miasto musi posiadaæ kuŸniê.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Uniwersytet Oksfordzki dostarcza +1 [ICON_CULTURE] kultury i redukuje [ICON_HAPPINESS_3] analfabetyzm o 2. +50 [ICON_CULTURE] kultury za ka¿dym razem, gdy odkrywasz technologiê. Aby móc zbudowaæ Uniwersytet Oksfordzki, miasto musi posiadaæ Uniwersytet.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten cud narodowy produkuje +1 [ICON_CULTURE] kultury za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta. Nie mo¿e powstaæ, je¿eli dane miasto nie posiada opery.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wa¿ny obronny cud narodowy dla cywilizacji opartej na technologii. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ o 2. Narodowa Agencja Wywiadu daje dodatkowego szpiega, rozwija wszystkich istniej¹cych szpiegów i os³abia skutecznoœæ nieprzyjacielskiego szpiega o 15%. Miasto musi mieæ posterunek policji zanim mo¿liwe bêdzie utworzenie Narodowej Agencji Wywiadu.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wa¿ny budynek dla cywilizacji próbuj¹cej rozpowszechniæ sw¹ religiê na ca³y œwiat z imperium posiadaj¹cego niewiele ludnych miast. Z³ota era rozpoczyna siê po wybudowaniu. Miasto musi mieæ œwi¹tyniê zanim mo¿liwe bêdzie utworzenie Wielkiej Œwi¹tyni.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Daj¹ce ogromn¹ premiê do [ICON_TOURISM] turystyki Narodowe Centrum Odwiedzaj¹cych to doskona³a budowla dla cywilizacji pragn¹cych osi¹gn¹æ zwyciêstwo kulturowe lub zwiêkszyæ swój ideologiczny wp³yw na inne cywilizacje. Zbuduj j¹ w swoim mieœcie o najwy¿szym wskaŸniku [ICON_TOURISM] turystyki, w którym jest hotel.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Help Text

UPDATE Language_pl_PL
SET Text = 'Wszystkie nowo wyszkolone w tym mieœcie jednostki poza lotniczymi otrzymuj¹ awans „[COLOR_POSITIVE_TEXT]Morale[ENDCOLOR]”, który zwiêksza ich [ICON_STRENGTH] si³ê bojow¹ o 10%. Zawiera 1 miejsce na wielkie dzie³o literackie.[NEWLINE][NEWLINE]Musisz mieæ wybudowane koszary w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+25% do pojawienia siê [ICON_GREAT_PEOPLE] wielkiej osobistoœci w tym mieœcie. Zawiera 1 miejsce na wielkie dzie³o sztuki lub artefakt.[NEWLINE][NEWLINE]Musisz mieæ wybudowany pomnik w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+10% [ICON_CULTURE] kultury i [ICON_GOLD] z³ota podczas "Dnia uwielbienia dla króla" w mieœcie, w którym zosta³ wybudowany. Redukuje [ICON_HAPPINESS_3] znudzenie. Musisz mieæ wybudowane arenê w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] ubóstwo. Otrzymujesz dodatkow¹ kopiê wszystkich surowców luksusowych dooko³a tego miasta. Przychodz¹ce [ICON_INTERNATIONAL_TRADE] szlaki handlowe generuj¹ +4 [ICON_GOLD] z³ota dla miasta i +2 [ICON_GOLD] z³ota dla w³aœciciela szlaku.[NEWLINE][NEWLINE]Musisz mieæ wybudowany urz¹d celny w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Musisz mieæ wybudowan¹ bibliotekê w tym mieœcie.[NEWLINE][NEWLINE]+1 [ICON_RESEARCH] nauki za ka¿dych 2 [ICON_CITIZEN] mieszkañców tego miasta. Zawiera 1 miejsce na wielkie dzie³o literackie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+50 [ICON_RESEARCH] nauki kiedy zbudujesz jakiœ budynek w tym mieœcie. Premia zale¿y od ery. Musisz mieæ wybudowan¹ kuŸniê w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wielki naukowiec pojawia siê w pobli¿u miasta. +50 [ICON_CULTURE] kultury za ka¿dym razem gdy odkryjesz technologiê, zale¿ne od ery. Zawiera 2 miejsca na wielkie dzie³a literackie. Redukuje [ICON_HAPPINESS_3] analfabetyzm.[NEWLINE][NEWLINE]Musisz mieæ wybudowany uniwersytet w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.[NEWLINE][NEWLINE]+3 [ICON_RESEARCH] nauki jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_CULTURE] kultury za ka¿dych 4 [ICON_CITIZEN] mieszkañców tego miasta. Zawiera 3 miejsca na wielkie dzie³a sztuki.[NEWLINE][NEWLINE]Musisz mieæ wybudowan¹ operê w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.[NEWLINE][NEWLINE]+5 [ICON_GOLD] z³ota jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] przestêpczoœæ. Daje dodatkowego szpiega i rozwija wszystkich istniej¹cych szpiegów. Os³abia te¿ skutecznoœæ nieprzyjacielskiego wywiadu o 15%.[NEWLINE][NEWLINE]Musisz mieæ wybudowany posterunek policji w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rozpoczyna siê [ICON_GOLDEN_AGE] z³ota era. Redukuje [ICON_HAPPINESS_3] niepokoje religijne. Musisz mieæ wybudowan¹ œwi¹tyniê w tym mieœcie. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Musisz mieæ wybudowany hotel w tym mieœcie.[NEWLINE][NEWLINE]100% [ICON_CULTURE] kultury z cudów œwiata, cudów naturalnych i ulepszeñ jest dodawane do wytwarzanej przez to miasto [ICON_TOURISM] turystki a [ICON_TOURISM] turystyka wytwarzana z wielkich dzie³ jest zwiêkszona o 100%. Koszt roœnie wraz z iloœci¹ miast w imperium.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Szybkoœæ budowy ulepszeñ pól roœnie o 25%, a w pobli¿u miasta pojawia siê osadnik.'
WHERE Tag = 'TXT_KEY_WONDER_CHICHEN_ITZA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wszystkie jednostki zyskuj¹ +15% [ICON_STRENGTH] si³y bojowej podczas atakowania miast. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ we wszystkich miastach. Otrzymujesz w mieœcie darmow¹ kuŸniê.'
WHERE Tag = 'TXT_KEY_WONDER_STATUE_ZEUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_STONEHENGE_HELP', 'Zapewnia 30 [ICON_PEACE] wiary po ukoñczeniu. Otrzymujesz w mieœcie darmow¹ radê.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W mieœcie rozpoczyna siê "Dzieñ uwielbienia dla króla". Podczas trwania "Dnia uwielbienia dla króla" [ICON_PRODUCTION] produkcja miasta wzrasta o 10%. [NEWLINE][NEWLINE]Otrzymujesz 30 [ICON_CULTURE] kultury za ka¿dym razem, gdy poœwiêcasz wielk¹ osobê, skaluje siê zale¿nie od epoki. Otrzymujesz w mieœcie darmowy zak³ad kamieniarski.'
WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+10% do przyrostu [ICON_FOOD] ¿ywnoœci we wszystkich miastach. +15% do [ICON_PRODUCTION] produkcji podczas szkolenia jednostek strzeleckich. Otrzymujesz w mieœcie darmowy spichlerz.'
WHERE Tag = 'TXT_KEY_WONDER_TEMPLE_ARTEMIS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 [ICON_FOOD] z³ota na wszystkich polach pustyni obs³ugiwanych przez to miasto. Zyskuje dodatkowe miejsce na szlak handlowy, a w mieœcie pojawia siê karawana. +6 [ICON_CULTURE] kultury po wynalezieniu archeologii. Miasto musi byæ zbudowane na pustyni lub w jej pobli¿u.'
WHERE Tag = 'TXT_KEY_WONDER_PETRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowa technologia. Daje darmow¹ bibliotekê w mieœcie, w którym wybudowano ten cud. Zawiera 2 miejsca na wielkie dzie³a literackie.[NEWLINE][NEWLINE]+3 [ICON_RESEARCH] nauki jeœli jest zape³niona sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymagaj¹ ukoñczenia drzewa Tradycji. Daj¹ darmow¹ ³aŸniê w mieœcie, w którym wybudowano ten cud.'
WHERE Tag = 'TXT_KEY_WONDER_HANGING_GARDEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa W³adzy. Imperium wkracza w [ICON_GOLDEN_AGE] z³ot¹ erê. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ we wszystkich miastach. Tworzy kopiê ka¿dego typu kontrolowanych przez ciebie l¹dowych jednostek wojskowych i umieszcza jednostkê w pobli¿u miasta, w którym powsta³a Terakotowa Armia. Otrzymujesz du¿¹ sumê [ICON_CULTURE] kultury po ukoñczeniu.'
WHERE Tag = 'TXT_KEY_WONDER_TERRA_COTTA_ARMY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Postêpu. Redukuje [ICON_HAPPINESS_3] znudzenie we wszystkich miastach. Zawiera w miejscu na wielkie dzie³o przygotowane wielkie dzie³o sztuki.[NEWLINE][NEWLINE]+3 [ICON_CULTURE] kultury jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_PARTHENON_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowy ustrój spo³eczny. Redukuje [ICON_HAPPINESS_3] analfabetyzm we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_ORACLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz darmowego wielkiego genera³a. [NEWLINE][NEWLINE]Jednostki l¹dowe wroga musz¹ wydaæ 1 dodatkowy [ICON_MOVES] punkt ruchu na ka¿dym polu twojego terytorium. Zapewnia darmowe mury w mieœcie, w którym powsta³a budowla. Po wynalezieniu dynamitu Wielki Mur staje siê przestarza³y.'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_WALL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Nowo wyszkolone jednostki (poza powietrznymi) w tym mieœcie otrzymuj¹ awans „[COLOR_POSITIVE_TEXT]Musztra I[ENDCOLOR]”, co zwiêksza ich [ICON_STRENGTH] si³ê bojow¹ i daje premiê przeciwko miastom. Miasto otrzymuje darmowy zamek. Ponadto zwiêksza iloœæ [ICON_CULTURE] kultury generowanej przez miasto o 20%.'
WHERE Tag = 'TXT_KEY_WONDER_ALHAMBRA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Pobo¿noœci. Dostajesz 50 [ICON_RESEARCH] nauki za ka¿dym razem, gdy poœwiêcasz [ICON_GREAT_PEOPLE] wielk¹ osobê. Daje darmowy meczet w mieœcie, w którym wybudowano ten cud.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Uniwersytet Sankore|Uniwersytetu Sankore|Uniwersytecie Sankore|Uniwersytet Sankore'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_OF_DJENNE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Centrum islamskiej spo³ecznoœci naukowej, Uniwersytet Sankore ró¿ni³ siê znacznie swoj¹ organizacj¹ od uniwersytetów œredniowiecznej Europy. Nie mia³ ¿adnej centralnej administracji poza w³adc¹. Nie mia³ spisu studentów, ale przechowywano w nim kopie ich publikacji. Sk³ada³o siê na niego kilka ca³kowicie niezale¿nych szkó³ i koled¿ów, ka¿dy zarz¹dzany przez jednego mistrza lub imama. Studenci do³¹czali do jednego nauczyciela, a lekcje odbywa³y siê na otwartych podwórzach meczetu lub w prywatnych rezydencjach.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pozwala na wybór wierzenia reformacyjnego bez normalnych wymaga. Musi zostaæ zbudowany w œwiêtym mieœcie.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sobór Wasyla B³ogos³awionego|Soboru Wasyla B³ogos³awionego|Soborowi Wasyla B³ogos³awionego|Sobór Wasyla B³ogos³awionego'
WHERE Tag = 'TXT_KEY_BUILDING_KREMLIN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sobór Wasyla B³ogos³awionego, znany te¿ jako Sobór Opieki Matki Bo¿ej na Fosie, jest soborem znajduj¹cym siê na Placu Czerwonym w Moskwie, w Rosji. Zbudowany w latach 1555–1561 przez cara Iwana GroŸnego mia³ upamiêtniaæ zwyciêstwo w wojnie z kazañskimi Tatarami i zdobycie ich stolicy – miasta Kazañ. by³ wielokrotnie restaurowany i przebudowywany. W XVII w. dodano kopu³y nad wejœciami i kolorowe dekoracje kopu³ g³ównych (wczeœniej wszystkie by³y poz³acane), wewnêtrzne i zewnêtrzne ornamenty i malunki. W 1929 sobór zosta³ zamkniêty, a jego dzwony zdjête. Obecnie znajduje siê tam muzeum, w którego posiadaniu znajduje siê równie¿ niezwyk³a kolekcja broni z czasów Iwana GroŸnego.'
WHERE Tag = 'TXT_KEY_WONDER_KREMLIN_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Musi powstaæ w œwiêtym mieœcie. Pojawi siê 3 darmowych misjonarzy. Daje darmowy klasztor w mieœcie, w którym wybudowano ten cud.'
WHERE Tag = 'TXT_KEY_WONDER_BOROBUDUR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+15% [ICON_GOLD] z³ota z [ICON_CONNECTED] po³¹czeñ miast. Miasto zyskuje +1 [ICON_FOOD] ¿ywnoœci, [ICON_PRODUCTION] produkcji, [ICON_CULTURE] kultury i [ICON_PEACE] wiary za ka¿d¹ górê w promieniu 3 pól od miasta.[NEWLINE][NEWLINE]Miasto musi byæ zbudowane w odleg³oœci do 2 pól od gór, znajduj¹cych siê na twoim terytorium.'
WHERE Tag = 'TXT_KEY_WONDER_MACHU_PICHU_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_NOTRE_DAME_HELP', 'Zawiera dwa miejsca na wielkie dzie³a sztuki lub artefakty.[NEWLINE][NEWLINE]+3 [ICON_PEACE] wiary jeœli jest zape³nione sztuk¹.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Dyplomacji. Zapewnia 2 dodatkowych delegatów na Kongresie Œwiatowym za ka¿de 8 pañstw-miast w grze. Redukuje [ICON_HAPPINESS_3] ubóstwo we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_FORBIDDEN_PALACE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Czas trwania [ICON_GOLDEN_AGE] z³otych er zwiêksza siê o 50%. Redukuje [ICON_HAPPINESS_3] niepokoje religijne we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_TAJ_MAHAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowy wielki pisarz pojawi siê w pobli¿u miasta, w którym wybudowano cud. Zawiera 2 miejsca na wielkie dzie³a literackie.[NEWLINE][NEWLINE]+5 [ICON_GOLD] z³ota jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_GLOBE_THEATER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Estetyki. 1 darmowy wielki artysta pojawia siê w pobli¿u miasta, w którym powsta³ ten cud. Zawiera 3 miejsca na wielkie dzie³a sztuki.[NEWLINE][NEWLINE]+5 [ICON_CULTURE] kultury jeœli jest zape³niona sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_UFFIZI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Imperium wchodzi w [ICON_GOLDEN_AGE] z³ot¹ erê.. +1 punkt [ICON_GOLDEN_AGE] z³otej ery na turê za ka¿dych 5 [ICON_CITIZEN] mieszkañców w tym mieœcie. Redukuje [ICON_HAPPINESS_3] niepokoje religijne we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_PYRAMIDS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+15% [ICON_CULTURE] kultury we wszystkich miastach. Zawiera 2 miejsca na wielkie dzie³a sztuki.[NEWLINE][NEWLINE]+5 [ICON_PEACE] wiary jeœli jest zape³niona sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_SISTINE_CHAPEL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz darmowego wielkiego in¿yniera i zbrojowniê w mieœcie. Wielce powiêksza [ICON_STRENGTH] obronnoœæ miasta.'
WHERE Tag = 'TXT_KEY_WONDER_RED_FORT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Racjonalizmu. Wielki naukowiec pojawia siê w pobli¿u miasta, w którym zbudowano ten cud. Redukuje [ICON_HAPPINESS_3] analfabetyzm we wszystkich miastach. 50% wiêcej [ICON_RESEARCH] nauki generowanej z umów naukowych. Jeœli umowy naukowe s¹ wy³¹czone, zapewnia premiê +25% do [ICON_RESEARCH] nauki w mieœcie, w którym zosta³a zbudowana.'
WHERE Tag = 'TXT_KEY_WONDER_PORCELAIN_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Imperializmu. Wielki genera³ pojawia siê w pobli¿u miasta, w którym wybudowano cud, a wszystkie jednostki tworzone w tym mieœcie otrzymuj¹ +15 PD. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_BRANDENBURG_GATE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowy wielki artysta pojawia siê w pobli¿u miasta, w którym wybudowano ten cud. Zawiera 4 miejsca na wielkie dzie³a sztuki.[NEWLINE][NEWLINE]+8 [ICON_CULTURE] kultury jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_LOUVRE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Redukuje [ICON_HAPPINESS_3] znudzenie we wszystkich miastach. Wie¿a Eiffla daje równie¿ +12 punktów do [ICON_TOURISM] turystyki po wynalezieniu radia.'
WHERE Tag = 'TXT_KEY_WONDER_EIFFEL_TOWER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wymaga ukoñczenia drzewa Przemys³u. Koszty zakupów za [ICON_GOLD] z³oto we wszystkich miastach zostaj¹ zmniejszone o 15%. Redukuje [ICON_HAPPINESS_3] ubóstwo we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_WONDER_BIG_BEN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_GOLD] z³ota, [ICON_CULTURE] kultury i [ICON_HAPPINESS_1] zadowolenia z ka¿dego zamku.[NEWLINE][NEWLINE]Miasto musi znajdowaæ siê maksymalnie dwa pola od góry bêd¹cej wewn¹trz twojego terytorium.'
WHERE Tag = 'TXT_KEY_WONDER_NEUSCHWANSTEIN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowy wielki muzyk pojawi siê w pobli¿u miasta, w którym wybudowano cud. Zawiera 3 miejsca na wielkie dzie³a muzyczne.[NEWLINE][NEWLINE]+10 [ICON_CULTURE] kultury jeœli jest zape³niony sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_BROADWAY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '1 darmowy ustrój spo³eczny i +25% [ICON_CULTURE] kultury w tym mieœcie. Zawiera 2 miejsca na wielkie dzie³a muzyczne. Musi powstaæ w mieœcie na wybrze¿u.[NEWLINE][NEWLINE]+10 [ICON_CULTURE] kultury jeœli jest zape³niona sztuk¹.'
WHERE Tag = 'TXT_KEY_WONDER_SYDNEY_OPERA_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zmniejsza o 99,9% skutecznoœæ szpiegów wroga w mieœcie, w którym zosta³a zbudowana. W pozosta³ych miastach cywilizacji skutecznoœæ agentów przeciwnika spada o 25%. Anuluje premiê do [ICON_TOURISM] turystyki z technologii innych graczy.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_FIREWALL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przeznacz [ICON_PRODUCTION] produkcjê tego miasta na projekt Wystawy Œwiatowej. Koszt roœnie w zale¿noœci od iloœci graczy w grze i obecnej epoki.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_FAIR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przeznacz [ICON_PRODUCTION] produkcjê tego miasta na projekt igrzysk miêdzynarodowych. Koszt roœnie w zale¿noœci od iloœci graczy w grze i obecnej epoki.'
WHERE Tag = 'TXT_KEY_PROCESS_WORLD_GAMES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przeznacz [ICON_PRODUCTION] produkcjê tego miasta na projekt Miêdzynarodowej Stacji Kosmicznej. Koszt roœnie w zale¿noœci od iloœci graczy w grze i obecnej epoki.'
WHERE Tag = 'TXT_KEY_PROCESS_INTERNATIONAL_SPACE_STATION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
