--translated by mrlg

--------------------
-- America
--------------------
UPDATE Language_pl_PL
SET Text = 'Wszystkie l¹dowe jednostki wojskowe otrzymuj¹ +1 do zasiêgu wzroku. Dodatkowo 50% zni¿ki na zakup pól. Mo¿esz kupowaæ pola posiadane przez inne cywilizacje, ale kosztem wiêkszej iloœci [ICON_GOLD] z³ota ni¿ normalnie.'
WHERE Tag = 'TXT_KEY_TRAIT_RIVER_EXPANSION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Milicja to unikalna jednostka amerykañska, zastêpuj¹ca arkebuzerów. Milicja porusza siê po trudnym terenie bez kar i mo¿e strzelaæ z odleg³oœci 2 heksów.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_MINUTEMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Jedna z pierwszych jednostek u¿ywaj¹cych broni palnej. Mog¹ j¹ stworzyæ tylko Amerykanie. Zastêpuje arkebuzerów. Ta jednostka porusza siê po trudnym terenie jak po otwartym.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MINUTEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Bombowce B17 to unikatowa jednostka dla ideologii Swobód. Jest podobna do bombowców, ale jest odporniejsza na ataki artylerii przeciwlotniczej i myœliwców wroga. B17 otrzymuj¹ te¿ premiê podczas ataku na wrogie miasta. W przeciwieñstwie do bombowców, B17 maj¹ zasiêg 8 pól. Wiêcej szczegó³ów znajdziesz w zasadach dotycz¹cych lotnictwa.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_B17_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka powietrzna zrzucaj¹ca œmieræ na nieprzyjacielskie jednostki i miasta. Ta jednostka ma szansê unikn¹æ przechwycenia oraz zadaje miastom wiêksze uszkodzenia ni¿ bombowce. Wymaga Swobód.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_B17' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[COLOR_NEGATIVE_TEXT]Ukrad³eœ ich terytorium![ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_CULTURE_BOMB' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Arabia
--------------------
UPDATE Language_pl_PL
SET Text = '10% wytwarzanego przez miasto [ICON_GOLD] z³ota dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki. Szlaki handlowe uzyskuj¹ 50% zasiêgu. +1 [ICON_GOLD] z³ota ze [ICON_INTERNATIONAL_TRADE] szlaków handlowych z albo do miasta.[NEWLINE]Pobliskie [ICON_RES_SPICES] przyprawy: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliski [ICON_RES_SUGAR] cukier: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bazar to unikatowy budynek arabski, zastêpuj¹cy targowisko. Bazar zapewnia [ICON_GOLD] z³oto i zwiêksza zasiêg szlaków handlowych. Zwiêksza te¿ zyski z [ICON_RES_SPICES] przypraw i [ICON_RES_SUGAR] cukru.'
WHERE Tag = 'TXT_KEY_BUILDING_BAZAAR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kiedy ukoñczysz wydarzenie historyczne, twoja [ICON_CAPITAL] stolica otrzymuje +1 [ICON_RESEARCH] nauki, +1 [ICON_CULTURE] kultury i 20% do postêpu losowej [ICON_GREAT_PEOPLE] wielkiej osoby. [ICON_TOURISM] turystyka z wydarzeñ historycznych zwiêkszona o 20%.'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tysi¹c i jedna noc'
WHERE Tag = 'TXT_KEY_TRAIT_LAND_TRADE_GOLD2_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Szybka jednostka œredniowieczna, s³aba w walce z pikinierami. Mog¹ j¹ stworzyæ tylko Arabowie. Jest znacznie silniejsza (w obronie) od strzelców konnych, których zastêpuje.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CAMEL_ARCHER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '£ucznicy arabscy to unikatowa jednostka zastêpuj¹ca strzelców konnych. Jest to silna jednostka strzelaj¹ca, mog¹ca raziæ przeciwników odleg³ych o 2 pola. Mo¿e siê poruszyæ po walce. Jako kawaleria, ³ucznicy arabscy s¹ wra¿liwi w walce z pikinierami ale nie tak bardzo jak strzelcy konni'
WHERE Tag = 'TXT_KEY_UNIT_ARABIAN_CAMELARCHER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Assyria
--------------------
UPDATE Language_pl_PL
SET Text = 'W mieœcie musi byæ wybudowana biblioteka.[NEWLINE][NEWLINE]+1 [ICON_RESEARCH] nauki za ka¿dych 2 [ICON_CITIZEN] mieszkañców tego miasta a wszystkie biblioteki wytwarzaj¹ +2 [ICON_RESEARCH] nauki. Ka¿de wielkie dzie³o literackie, które posiadasz zapewnia +5 PD wszystkim jednostkom wyprodukowanym w dowolnym mieœcie (maksymalnie 45 PD)[NEWLINE][NEWLINE]Koszt budowy roœnie wraz z iloœci¹ miast w imperium. Zawiera 2 miejsca na wielkie dzie³a literackie. [NEWLINE][NEWLINE]+3 [ICON_PRODUCTION] produkcji jeœli jest motyw.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );
 
UPDATE Language_pl_PL
SET Text = 'Poza wydajnoœci¹ [ICON_RESEARCH] naukow¹ gwarantowan¹ przez szko³ê filozofii, któr¹ zastêpuje ten budynek, biblioteka królewska uwzglêdnia tak¿e dwa miejsca na wielkie dzie³a literackie i jest wczeœniej dostêpna. Zapewnia premiê dla wszystkich bibliotek a wszystkie posiadane miasta generuj¹ dodatkowe PD dla jednostek w nich trenowanych, zale¿nie od iloœæ wielkich dzie³ literackich w twoim imperium (do 45 PD). Budynek dostêpny tylko dla Asyryjczyków.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Unikatowa jednostka wsparcia Asyryjczyków. Pobliskie jednostki otrzymuj¹ +40% [ICON_STRENGTH] si³y bojowej przeciwko miastom, gdy stoj¹ w s¹siedztwie wrogiej metropolii lub +20% [ICON_STRENGTH] si³y bojowej gdy stoi ona w odleg³oœci dwóch pól od wrogiej metropolii. Zaczyna z awansem "[COLOR_POSITIVE_TEXT]Medyk I[ENDCOLOR]" i "[COLOR_POSITIVE_TEXT]Wiêkszy zasiêg widzenia[ENDCOLOR]". Nie mo¿e atakowaæ ani siê broniæ.[NEWLINE][NEWLINE]Mo¿na mieæ tylko [COLOR_POSITIVE_TEXT]2[ENDCOLOR] wie¿e oblê¿nicze w tym samym czasie.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Unikatowa jednostka wsparcia Asyryjczyków. Musi znajdowaæ siê obok miasta wroga, by dzia³aæ skutecznie, ale gdy znajdzie siê tak blisko, sieje olbrzymie zniszczenie. [COLOR_NEGATIVE_TEXT]Nie mo¿e atakowaæ ani siê broniæ[ENDCOLOR], wiêc chroñ j¹ za ka¿d¹ cenê! Nie mo¿esz mieæ wiêcej ni¿ 2 aktywne wie¿e oblê¿nicze w tym samym czasie.[NEWLINE][NEWLINE]Jeœli jest w s¹siedztwie miasta, zapewnia olbrzymi¹ premiê bitewn¹ wszystkim okolicznym jednostkom, które równie¿ atakuj¹ miasto. Jeœli jest oddalona o dwa pola od miasta, wszystkie pobliskie jednostki atakuj¹ce miasto otrzymuj¹ po³owê tej premii. Dodaj do wie¿y oblê¿niczej jednostki walcz¹ce w zwarciu lub dystansowo i doprowadŸ j¹ do celu, zanim zostanie zniszczona, a o wiele szybciej zdobêdziesz miasto.' 
WHERE Tag = 'TXT_KEY_UNIT_ASSYRIAN_SIEGE_TOWER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Gdy znajduje siê obok wrogiego miasta, inne jednostki w odleg³oœci do 2 pól otrzymuj¹ premiê 40% podczas ataku na to miasto. Jeœli znajduje jest w odleg³oœci 2 pól od niego, pozosta³e jednostki w promieniu 2 pól otrzymuj¹ premiê 20% podczas ataku na to miasto. Premie siê nie kumuluj¹.'
WHERE Tag = 'TXT_KEY_PROMOTION_SAPPER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Gdy zdobywasz miasto, zyskujesz technologiê odkryt¹ ju¿ przez w³aœciciela miasta lub, jeœli to niemo¿liwe, du¿¹ premiê do [ICON_RESEARCH] nauki. Wszystkie wielkie dzie³a wytwarzaj¹ +2 [ICON_RESEARCH] nauki.'
WHERE Tag = 'TXT_KEY_TRAIT_SLAYER_OF_TIAMAT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

--------------------
-- Austria
--------------------
UPDATE Language_pl_PL
SET Text = 'Mo¿e wydaæ [ICON_GOLD] z³oto, aby aran¿owaæ ma³¿eñstwa z pañstwami-miastami. Podczas pokoju z pañstwem-miastem, ma³¿eñstwo eliminuje spadek [ICON_INFLUENCE] wp³ywu, zapewnia 1 dodatkowego delegata na kongresie œwiatowym i przyspiesza o 10$ powstawanie [ICON_GREAT_PEOPLE] wielkich ludzi w twojej [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dyplomacja habsburska'
WHERE Tag = 'TXT_KEY_TRAIT_ANNEX_CITY_STATE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+33% [ICON_GREAT_PEOPLE] wielkich ludzi generowanych w tym mieœcie i +25 [ICON_GOLD] z³ota (skaluje siê zale¿nie od epoki), kiedy naradza siê [ICON_GREAT_PEOPLE] wielka osoba.[NEWLINE][NEWLINE]Zachowuje ponad 25% [ICON_FOOD] ¿ywnoœci po wzroœcie miasta (efekt kumuluje siê z akweduktem) i redukuje [ICON_HAPPINESS_3] ubóstwo.[NEWLINE][NEWLINE]Wymaga akweduktu w tym mieœcie.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kawiarnia to renesansowy budynek dostêpny dla Austrii, zastêpuj¹cy sklep. Zwiêksza [ICON_GROWTH] wzrost miasta i szybkoœæ generowania [ICON_GREAT_PEOPLE] wielkich ludzi o 33%.'
WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- Aztecs
--------------------
UPDATE Language_pl_PL
SET Text = '+1 [ICON_FOOD] ¿ywnoœci i +1 [ICON_PRODUCTION] produkcji za ka¿dych 4 [ICON_CITIZEN] mieszkañców w tym mieœcie. Ka¿de wykorzystywane pole jeziora dostarcza +2 [ICON_FOOD] ¿ywnoœci a pola rzek +1 [ICON_FOOD] ¿ywnoœci.[NEWLINE][NEWLINE]'
WHERE Tag = 'TXT_KEY_BUILDING_FLOATING_GARDENS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz [ICON_GOLD] z³oto i [ICON_PEACE] wiarê za ka¿d¹ zniszczon¹ jednostkê wroga. Kiedy podpiszesz korzystny traktat pokojowy, rozpoczyna siê [ICON_GOLDEN_AGE] z³ota era.'
WHERE Tag = 'TXT_KEY_TRAIT_CULTURE_FROM_KILLS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Doœæ s³aba jednostka z pocz¹tku gry. Mog¹ j¹ stworzyæ tylko Aztekowie. Ta jednostka ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ od wojowników, walczy skuteczniej walczy w d¿ungli i leczy siê, gdy zniszczy jednostkê wroga.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JAGUAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
--------------------
-- Babylon
--------------------
UPDATE Language_pl_PL
SET Text = 'Po wynalezieniu pisma otrzymuje darmowego wielkiego naukowca, a czêstotliwoœæ pojawiania siê kolejnych jest o 50% wiêksza. Inwestowanie [ICON_GOLD] z³ota w budynki obni¿a ich koszt [ICON_PRODUCTION] produkcji o kolejne 15%.'
WHERE Tag = 'TXT_KEY_TRAIT_INGENIOUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Babiloñska unikalna jednostka zastêpuj¹ca ³uczników z ³ukiem kompozyt. £ucznicy babiloñscy s¹ silniejsi i mog¹ lepiej przetrwaæ atak wrêcz ni¿ standardowi. Rozpoczynaj¹ z ostrza³em poœrednim.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Strzelec Babilonu jest unikatow¹ jednostk¹ zastêpuj¹c¹ ³ucznika z ³ukiem kompozytowym. S¹ oni silniejsi od ³uczników z ³ukiem kompozytowym defensywie, co pozwala na umieszczanie ich na linii frontu. Zwiêkszona si³a bojowa strzelców pozwala im na szybkie zaradzenie wiêkszoœci problemów przez naszpikowanie ich strza³ami.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_BOWMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mury Babilonu zwiêkszaj¹ obra¿enia wszystkich dystansowych ataków miasta i zwiêkszaj¹ jego obronê.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_INFO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Mury Babilonu to unikalny budynek Babiloñczyków. Zastêpuje zwyk³e mury miejskie. Zwiêksza obronnoœæ miasta o 9, a punkty ¿ywotnoœci o 100 (znacznie wiêcej jednego i drugiego ni¿ zwyk³e mury). Mury Babilonu zapewniaj¹ te¿ miejsce na naukowca i naukê.'
WHERE Tag = 'TXT_KEY_CIV5_BABYLON_WALLS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Brazil
--------------------

UPDATE Language_pl_PL
SET Text = 'Generowana [ICON_TOURISM] turystyka zwiêkszana jest o 33% podczas z³otych er. W czasie ich trwania wszyscy wielcy ludzie pojawiaj¹ siê o 50% szybciej, 33% kultury twojej [ICON_CAPITAL] stolicy dodaje siê do twojej [ICON_TOURISM] turystyki.'
WHERE Tag = 'TXT_KEY_TRAIT_CARNIVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka piechoty z epoki wspó³czesnej gromadz¹ca punkty przybli¿aj¹ce [ICON_GOLDEN_AGE] z³ot¹ erê, gdy pokona przeciwnika. Rozpoczyna ze sztuk¹ przetrwania I. Mo¿e powstawaæ tylko w Brazylii.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BRAZILIAN_PRACINHA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Dziêki twojej unikalnej premii karnawa³owej, zyskujesz dodatkowe 25% generowanej [ICON_TOURISM] turystyki we wszystkich cywilizacjach. Ponadto o 25% szybciej zdobywasz wielkich ludzi o 25% i 25% kultury twojej [ICON_CAPITAL] stolicy dodaje jest przekszta³cane na [ICON_TOURISM] turystykê.'
WHERE Tag = 'TXT_KEY_TP_CARNIVAL_EFFECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tworzy obóz wyrêbu brezylki zwiêkszaj¹cy iloœæ zysków z tego pola, ale nie niszczy d¿ungli.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zwiêkszy dostarczan¹ przez to pole iloœæ [ICON_GOLD] z³ota, nie niszcz¹c d¿ungli.'
WHERE Tag = 'TXT_KEY_BUILD_BRAZILWOOD_CAMP_REC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Obóz wyrêbu brezylki mo¿na zbudowaæ tylko na polu z d¿ungl¹. Zapewnia on dodatkowe zyski.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Obóz wyrêbu brezylki mo¿na zbudowaæ tylko na polu z d¿ungl¹. Zapewnia on dodatkowe zyski. [NEWLINE][NEWLINE]Brezylka ciernista to gatunek drzewa z rodziny bobowatych (Fabaceae) z podrodziny brezylkowych (Caesalpinioideae). Wystêpuje w Ameryce Œrodkowej i Brazylii. Drewno znane jako drzewo fernambukowe lub (drzewo pernambukowe) dostarcza cennego materia³u u¿ywanego w przemyœle okrêtowym, meblarskim, w snycerstwie i lutnictwie. Z drzewa pozyskuje siê równie¿ czerwony barwnik.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_BRAZILWOOD_CAMP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Byzantium
--------------------
UPDATE Language_pl_PL
SET Text = 'Zawsze mo¿e za³o¿yæ religiê i wybiera jedno wierzenie wiêcej ni¿ zwykle podczas zak³adania religii. Mo¿esz wybieraæ spoœród wierzeñ ju¿ wykorzystanych w innych religiach.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_BELIEF' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Carthage
--------------------
UPDATE Language_pl_PL
SET Text = 'Miasta produkuj¹ du¿¹ sumê [ICON_GOLD] z³ota po za³o¿eniu. Premia zale¿y od epoki. Wszystkie miasta nadmorskie otrzymuj¹ darmow¹ przystañ.'
WHERE Tag = 'TXT_KEY_TRAIT_PHOENICIAN_HERITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Celts
--------------------
INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_CEILIDH_HALL_HELP', 'Redukuje [ICON_HAPPINESS_3] znudzenie i zapewnia skromn¹ sumê punktów [ICON_CULTURE] kultury po ukoñczeniu.[NEWLINE] Pobliska [ICON_RES_IVORY] koœæ s³oniowa: +3 [ICON_CULTURE] kultury.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Sala ceilidh to œredniowieczny budynek dostêpny dla Celtów, zastêpuj¹cy cyrk. Redukuje nieco [ICON_HAPPINESS_3] znudzenie i zwiêksza [ICON_CULTURE] kulturê i [ICON_PEACE] wiarê miasta. Pobliskie surowce koœci s³oniowej daj¹ +3 [ICON_CULTURE] kultury. Zapewnia jedno miejsce na specjalistê muzyka i na wielkie dzie³o muzyczne.'
WHERE Tag = 'TXT_KEY_BUILDING_CEILIDH_HALL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Maj¹ unikalny zestaw wierzeñ panteonu. Miasta z twoim panteonem lub za³o¿on¹ religi¹ nie generuj¹ ani nie otrzymuj¹ zagranicznego nacisku religijnego. +3 [ICON_PEACE] wiary w posiadanych miastach, gdzie twój panteon (lub religia, jeœli jesteœ za³o¿ycielem) dominuje.'
WHERE Tag = 'TXT_KEY_TRAIT_FAITH_FROM_NATURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jedynie Celtowie mog¹ stworzyæ tê jednostkê. Podwojony ruch i +25% obrony na wzgórzach, œniegu oraz tundrze. Mo¿e pl¹drowaæ wrogie ulepszenie nie wydaj¹c dodatkowych punktów ruchu i zyskuje 200% si³y wroga w postaci [ICON_PEACE] wiary za zabójstwa.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CELT_PICTISH_WARRIOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- China
--------------------
UPDATE Language_pl_PL
SET Text = '+10% [ICON_CULTURE] kultury w tym mieœcie. +1 [ICON_RESEARCH] +1 [ICON_RESEARCH] nauki za ka¿dych 4 [ICON_CITIZEN] mieszkañców w tym mieœcie. +33% do szybkoœci pojawiania siê [ICON_GREAT_MUSICIAN] wielkich muzyków, wszystkie gildie muzyków produkuj¹ +1 [ICON_GOLD] z³ota.[NEWLINE]Wymaga amfiteatru w tym mieœcie.'
WHERE Tag = 'TXT_KEY_BUILDING_PAPER_MAKER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pawilon herbaciany|Pawilonu herbacianego|Pawilonowi herbacianemu|Pawilon herbaciany|Pawilony herbaciane|Pawilonów herbacianych|Pawilonom herbacianym|Pawilony herbaciane', Gender = 'masculine'
WHERE Tag = 'TXT_KEY_BUILDING_PAPER_MAKER_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'W Chinach i Nepalu pawilony herbaciane s¹ tradycyjnymi miejscami, w których oferuje siê herbatê ich klientom. Ludzie zbieraj¹ siê w domach herbacianych aby porozmawiaæ, dla towarzystwa i cieszyæ siê herbat¹, a m³odzi ludzie czêsto spotykaj¹ siê w nich na randkach. Guangdongski (kantoñski) styl domu herbacianego jest szczególnie znany poza Chinami, zw³aszcza w Himalajach. Te domy herbaciane, zwane chalou, oferuj¹ dim sumy i to jedzenie na ma³ych talerzach jest spo¿ywane wraz z herbat¹.'
WHERE Tag = 'TXT_KEY_BUILDING_PAPER_MAKER_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pawilon herbaciany to unikalny budynek chiñski zastêpuj¹cy operê. Budowla ta przyspiesza [ICON_RESEARCH] badania naukowe w mieœcie oraz zwiêksza jego wytwarzanie [ICON_CULTURE] kultury. Podnosi te¿ szybkoœci pojawiania siê wielkich muzyków w tym mieœcie i wartoœæ gildii muzyków. Do zbudowania wymaga amfiteatru.'
WHERE Tag = 'TXT_KEY_BUILDING_PAPER_MAKER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+50% do [ICON_FOOD] wzrostu w miastach podczas z³otych er i "Dni uwielbienia dla króla". [ICON_GREAT_PEOPLE] wielcy ludzie dostarczaj¹ punkty [ICON_GOLDEN_AGE] z³otej ery i wywo³uj¹ 10 tur "Dnia uwielbienia dla króla" po narodzinach.'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Przegl¹d imperium'
WHERE Tag = 'TXT_KEY_TRAIT_ART_OF_WAR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Denmark
--------------------
UPDATE Language_pl_PL
SET Text = 'Zaokrêtowane jednostki otrzymuj¹ +1 do [ICON_MOVES] ruchu i p³ac¹ tylko 1 punkt ruchu za zejœcie z morza na l¹d. Jednostki walcz¹ce wrêcz nie wydaj¹ punktów ruchu na pl¹drowanie i grabi¹ z³oto podczas ataków na miasto.'
WHERE Tag = 'TXT_KEY_TRAIT_VIKING_FURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Partyzanci|Partyzantów|Partyzantom|Partyzantów', Gender = 'masculine', Plurality = '2'
WHERE Tag = 'TXT_KEY_UNIT_DANISH_SKI_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Partyzanci to forma nieregularnej armii, w której ma³e grupy kombatantów, jak uzbrojeni cywile ala oddzia³y paramilitarne, u¿ywaj¹ taktyk, do których zaliczaj¹ siê, zasadzki, sabota¿e, napady, ataki zaczepne, ataki typu uderz i ucieknij oraz mobilnoœci, aby móc walczyæ z wiêkszymi i mniej mobilnymi si³ami.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Unikatowa, póŸna jednostka l¹dowa specjalizuj¹ca siê we flankowaniu, atakowaniu zranionych jednostek i szybkim leczeniu siê na w³asnym terytorium. Ignoruje wrogie strefy kontroli i kary terenowe do ruchu. Wymaga £adu.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Partyzanci s¹ unikatowi dla ideologii £adu. S¹ tañsi i pocz¹tkowo s³absi ni¿ inne jednostki ze swojej ery, maj¹ jednak unikalny zestaw awansów, który czyni ich dosyæ potê¿nymi. Otrzymuj¹ premiê bitewn¹ przeciwko zranionym jednostkom i podczas flankowania, mog¹ siê te¿ poruszaæ po ka¿dym terenie i wrogich strefach kontroli bez kar do ruchu. Ponadto, bardzo szybko lecz¹ siê na w³asnym terytorium.'
 WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Berserkowie to jedna z unikatowych jednostek duñskich, zastêpuj¹ca ciê¿kozbrojnych. Ma o 1 [ICON_MOVES] punkt ruchu wiêcej ni¿ ciê¿kozbrojni i posiada awans „Desant” oraz „Szar¿ê”, umo¿liwiaj¹ce atak na l¹d z pól wybrze¿a bez kary i premiê przeciwko zranionym jednostkom. Dostêpni po wynalezieniu odlewania metalu zamiast stali.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_BERSERKER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Egypt
--------------------
UPDATE Language_pl_PL
SET Text = 'Kiedy wywodz¹cy siê st¹d [ICON_INTERNATIONAL_TRADE] l¹dowy szlak handlowy do innej cywilizacji zostanie ukoñczony, otrzymujesz premiê do [ICON_TOURISM] turystki w cywilizacji zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury. Otrzymujesz 10 punktów [ICON_GOLDEN_AGE] z³otej ery bo ka¿dej wygranej bitwie. Premia zale¿y od epoki. L¹dowe szlaki handlowe uzyskuj¹ +50% zasiêgu +2 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_TRUFFLES] trufle: +2 [ICON_GOLD] z³ota.[NEWLINE]Pobliska [ICON_RES_COTTON] bawe³na: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_CULTURE] kultury.[NEWLINE]Pobliskie [ICON_RES_FUR] futra: +1 [ICON_GOLD] z³ota, +1 [ICON_PRODUCTION] produkcji.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Grobowiec to budynek epoki klasycznej, który zwiêksza wytwarzan¹ przez miasto [ICON_PEACE] wiarê, [ICON_TOURISM] turystykê i [ICON_CULTURE] kulturê, ponadto zwiêksza wartoœæ szlaków handlowych do tego miasta. Jeœli jednak osiedle, w którym zbudowano grobowiec, zostanie zajête, cywilizacja straci dwa razy wiêcej z³ota, ni¿ gdyby grobowca nie by³o. Budynek ten zastêpuje œwi¹tyniê. Zastêpuje on karawanseraj.'
WHERE Tag = 'TXT_KEY_BUILDING_BURIAL_TOMB_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Szybka jednostka dystansowa. Tylko Egipcjanie mog¹ j¹ rekrutowaæ. W przeciwieñstwie do strzelców rydwanowych, których zastêpuje, nie wymaga [ICON_RES_HORSE] koni. Jest równie¿ silniejsza od strzelców rydwanowych i otrzymuje za darmo awans "[COLOR_POSITIVE_TEXT]Przymus pracy[ENDCOLOR]".'
WHERE Tag = 'TXT_KEY_UNIT_HELP_EGYPTIAN_WAR_CHARIOT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+20% [ICON_PRODUCTION] produkcji do budowy cudów. Artefakty dostarczaj¹ +2 [ICON_RESEARCH] nauki, [ICON_PEACE] wiary i [ICON_CULTURE] kultury, a s³ynne miejsca +4 [ICON_GOLD] z³ota, punktów [ICON_GOLDEN_AGE] z³otej ery i [ICON_TOURISM] turystyki.'
WHERE Tag = 'TXT_KEY_TRAIT_WONDER_BUILDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- England
--------------------
UPDATE Language_pl_PL
SET Text = '+2 [ICON_MOVES] ruchu dla wszystkich jednostek morskich i zaokrêtowanych, ponadto koszt utrzymania w [ICON_GOLD] z³ocie jednostek morskich jest mniejszy o 25%. Otrzymujesz jednego lub wiêcej dodatkowych [ICON_SPY] szpiegów w zale¿noœci od liczby pañstw-miast.'
WHERE Tag = 'TXT_KEY_TRAIT_OCEAN_MOVEMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '£ucznicy angielscy mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LONGBOWMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '£ucznicy angielscy, maj¹ wiêkszy zasiêg od kuszników, mog¹ ostrzeliwaæ przeciwników z odleg³oœci trzech pól, czêsto niszcz¹c ich, zanim zd¹¿¹ zareagowaæ na atak. Jak inne jednostki strzelaj¹ce, ³ucznicy angielscy s¹ s³abi w walce wrêcz.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_LONGBOWMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Potê¿na jednostka p³ywaj¹ca z epoki renesansu s³u¿¹ca do walki o przewagê na morzu. Mog¹ j¹ budowaæ tylko Anglicy. Jest s³absza od jednostki, któr¹ zastêpuje, mo¿e jednak atakowaæ dwa razy w jednej turze.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHIPOFTHELINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Okrêt liniowy to unikatowa jednostka angielska zastêpuj¹ca fregatê.  Dysponuje nieco mniejsz¹ si³¹ ognia ni¿ fregata, jednak jego zasiêg widzenia jest o jedno pole wiêkszy od fregaty, dziêki czemu wykrywa przeciwników na wiêkszej czêœci oceanu i mo¿e atakowaæ dwa razy na turê.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_SHIPOFTHELINE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Ethiopia
--------------------
UPDATE Language_pl_PL
SET Text = 'Kiedy koñczysz drzewko ustroju, przyjmujesz wierzenie albo wybierasz swoj¹ pierwsz¹ ideologiê, otrzymujesz darmow¹ technologiê.'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Salomonowa m¹droœæ'
WHERE Tag = 'TXT_KEY_TRAIT_BONUS_AGAINST_TECH_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
  
UPDATE Language_pl_PL 
SET Text = 'Silna, frontowa jednostka l¹dowa specjalizuj¹ca siê w walce na w³asnym terytorium, szczególnie w obronie pobli¿a etiopskiej stolicy. Tylko Etiopczycy mog¹ j¹ zbudowaæ.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MEHAL_SEFARI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Mehal Sefari stanowi¹ krêgos³up etiopskiej armii. Rozpoczynaj¹ z awansami, które daj¹ im premie podczas walki na w³asnym terytorium. S¹ nieco tañsi ni¿ fizylierzy, których zastêpuj¹.'
WHERE Tag = 'TXT_KEY_UNIT_MEHAL_SEFARI_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Tag, Text)
SELECT 'TXT_KEY_BUILDING_STELE_HELP', 'Koszt[ICON_CULTURE] kultury pozyskiwania nowych pól zmniejszony w tym mieœcie o 33%. +25% [ICON_PEACE] wiary podczas [ICON_GOLDEN_AGE] z³otych er.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

--------------------
-- France
--------------------
UPDATE Language_pl_PL
SET Text = 'Podwojone premie z zape³nienia sztuk¹ w [ICON_CAPITAL] stolicy. Kiedy podbijasz wrogie miasto, pl¹drujesz [ICON_GREAT_WORK] wielkie dzie³a z ich innych miast, jeœli jest to niemo¿liwe, otrzymujesz tymczasowy wzrost [ICON_CULTURE] kultury.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Bogactwa z podbojów'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_CULTURE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Muszkieterowie to unikatowa jednostka francuska, zastêpuj¹ca tercio. Muszkieterowie s¹ od nich znacznie silniejsi, uzyskuj¹ +1 [ICON_MOVES] ruchu i ignoruj¹ wrogie strefy kontroli.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_MUSKETEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jedna z pierwszych jednostek z broni¹ paln¹. Mog¹ j¹ stworzyæ tylko Francuzi. Ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ ni¿ tercio, których zastêpuje i zaczyna z awansem "Wojna b³yskawiczna".'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Napoleon wykrad³ {1_Num} [ICON_GREAT_WORK] wielkie {1_Num: plurality 1?dzie³o; other?dzie³a} z miast niedaleko {2_City[2]} pod jego zdobyciu!' , 'TXT_KEY_ART_PLUNDERED'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Wykradziona sztuka!' , 'TXT_KEY_ART_PLUNDERED_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Wykrad³eœ {1_Num} [ICON_GREAT_WORK] {1_Num: plurality 1?dzie³o; other?dzie³a} z miast niedaleko {2_City[2]} pod jego zdobyciu!' , 'TXT_KEY_ART_STOLEN'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Spl¹drowana sztuka!' , 'TXT_KEY_ART_STOLEN_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Podbój {2_City[2]} podwaja twoje wytwarzanie [ICON_CULTURE] kultury przez kolejne {1_Num} {1_num: plurality 1?turê; 2?tury; 3?tur}!' , 'TXT_KEY_CULTURE_BOOST_ART'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

INSERT INTO Language_pl_PL (Text, Tag)
SELECT 'Wzrost kulturalny!' , 'TXT_KEY_CULTURE_BOOST_ART_SUMMARY'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z tymczasowego wzrostu kulturalnego (Pozosta³e tury: {2_TurnsLeft}).'
WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_BONUS_TURNS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Chateau nale¿y budowaæ obok zasobów luksusowych. Nie mo¿e s¹siadowaæ z innym chateau. Podobnie jak fort daje premiê +50% do obrony. Musi byæ budowane na terytorium francuskim.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Chateau nale¿y budowaæ obok zasobów luksusowych. Nie mo¿e s¹siadowaæ z innym chateau. Daje dodatkowo 1 [ICON_GOLD] z³ota i 2 [ICON_CULTURE] kultury. Podobnie jak fort daje premiê +50% do obrony.[NEWLINE][NEWLINE]Chateau to dworek lub posiad³oœæ wiejska szlachty zwykle nie posiadaj¹ca ¿adnych umocnieñ. W œredniowieczu chateau by³o w wiêkszoœci samowystarczalne, utrzymywane przez ziemie nale¿¹ce do w³adcy. W XVII wieku lordowie francuscy wybudowali liczne eleganckie, luksusowe, architektonicznie wyrafinowane posiad³oœci takie jak Chateau de Maisons. Dziœ nazwy chateau u¿ywa siê doœæ swobodnie, czêsto okreœla siê tym mianem winnicê lub gospodê, niewa¿ne jak skromne.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CHATEAU_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Germany
--------------------
UPDATE Language_pl_PL
SET Text = '+5% [ICON_PRODUCTION] produkcji za ka¿dy szlak handlowy prowadz¹cy z twojej cywilizacji do pañstwa-miasta.[NEWLINE][NEWLINE]Kiedy dowolny [ICON_INTERNATIONAL_TRADE] szlak handlowy st¹d, prowadz¹cy do innej cywilizacji zostanie ukoñczony, otrzymujesz premiê do [ICON_TOURISM] turystki w cywilizacji zale¿n¹ od twojego ostatniego wytwarzania [ICON_CULTURE] kultury.[NEWLINE][NEWLINE]Szlaki handlowe z lub do tego miasta generuj¹ +2 dodatkowe sztuki [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]Wymaga targowiska.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Oprócz mo¿liwoœci zwyk³ego urzêdu celnego (wiêksza produkcja [ICON_GOLD] z³ota, dodatkowe [ICON_GOLD] z³oto ze szlaków handlowych), Hanza zapewnia dodatkow¹ [ICON_PRODUCTION] produkcjê za ka¿dy szlak handlowy na terenie twojej cywilizacji, prowadz¹cy do pañstwa-miasta. Szlaki handlowe mog¹ prowadziæ z ró¿nych miast, nawet tych, w których nie ma Hanzy. (Przyk³ad: je¿eli masz szlaki handlowe z Berlina do Genewy, z Monachium do Genewy, z Monachium do Berlina i z Berlina do Brukseli, to wszystkie miasta z Hanz¹ otrzymaj¹ +15% [ICON_PRODUCTION] produkcji). Ten budynek mog¹ wznieœæ tylko Niemcy.'
WHERE Tag = 'TXT_KEY_BUILDING_HANSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Otrzymujesz +3 punkty [ICON_GOLDEN_AGE] z³otej ery i +3 [ICON_CULTURE] kultury w twojej [ICON_CAPITAL] stolicy za ka¿de pañstwo-miasto, które jest twoim sojusznikiem. Premia zale¿y od epoki. Za ka¿de 2 sojusze z pañstwami-miastami otrzymujesz 1 dodatkowego delegata na Kongresie Œwiatowym.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Realpolitik'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_LAND_BARBARIANS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Panzery to unikatowa jednostka niemiecka zastêpuj¹ca czo³gi. Jest od nich silniejsza, szybsza i wczeœniej dostêpna. Podobnie jak czo³gi, otrzymuje karê podczas atakowania miast. Mo¿e tak¿e ruszyæ siê po walce, dziêki czemu jest w stanie wykorzystaæ wyrwê w liniach wroga, zanim ów zd¹¿y j¹ za³ataæ.'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_PANZER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Szybka i silna jednostka epoki przemys³owej. Otrzymuje karê podczas atakowania miast, ale jest niezwykle skuteczna na otwartym terenie.[NEWLINE][NEWLINE]Tylko Niemcy mog¹ j¹ budowaæ. Jest ona o jeden punkt szybsza ni¿ czo³g, który zastêpuje i wczeœniej od niego dostêpna.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PANZER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Ta jednostka jest znacznie tañsza od pikinierów i potrafi pl¹drowaæ teren bez zu¿ywania punktów ruchu, a tak¿e kradnie z³oto podczas ataku na miasto. Mo¿na j¹ odblokowaæ po ukoñczeniu drzewa W³adzy. Jednostki tej nie mo¿na wyszkoliæ, nale¿y j¹ kupiæ za z³oto. Mo¿e siê ruszyæ od razu po zakupie.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LANDSKNECHT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );



--------------------
-- Greece
--------------------
UPDATE Language_pl_PL
SET Text = '[ICON_INFLUENCE] wp³ywy w pañstwach-miastach spadaj¹ o po³owê wolniej i odnawiaj¹ siê dwukrotnie szybciej. Ka¿dy sojusz z pañstwem-miastem daje premiê do [ICON_STRENGTH] si³ bojowej posiadanych i sprzymierzonych jednostek o 4% (w sumie do 20%).'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_FRIENDSHIP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Hetajrowie mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_COMPANION_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Hetajrowie s¹ szybsi i silniejsi od konnicy, co czyni ich najgroŸniejsz¹ jednostk¹ kawalerii, a¿ do pojawienia siê rycerzy. Hetajrowie pomagaj¹ w tworzeniu wielkich genera³ów bardziej ni¿ inne jednostki. Ponadto mog¹ siê poruszaæ po przeprowadzeniu ataku. Hetajrowie to kawaleria, wiêc s¹ podatni na ataki w³óczników i pikinierów.'
WHERE Tag = 'TXT_KEY_UNIT_GREEK_COMPANIONCAVALRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Jednostka z epoki staro¿ytnej specjalizuj¹ca siê w zwalczaniu kawalerii. Mog¹ j¹ tworzyæ tylko Grecy. Ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ ni¿ w³ócznicy, których zastêpuje i bardzo szybko produkuje wielkich genera³ów.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HOPLITE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- Huns
--------------------
UPDATE Language_pl_PL
SET Text = 'Jednostki kawaleryjskie zadaj¹ wiêcej obra¿eñ podczas flankowania i przejmuj¹ jednostki, pokonani barbarzyñcy w obozowiskach do³¹czaj¹ do ciebie. Kiedy zyskujesz pola ³¹k lub równin w naturalny sposób, przylegaj¹ce niekontrolowane pola tego samego typu równie¿ s¹ przy³¹czane.'
WHERE Tag = 'TXT_KEY_TRAIT_RAZE_AND_HORSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka oblê¿nicza zabójcza podczas ataków na miasta. Mo¿e byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HUN_BATTERING_RAM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Tarany mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê. U¿ywaj taranów w celu niszczenia fortyfikacji miast. Awans os³ony I zapewnia im ochronê przed jednostkami dystansowymi; wysy³aj przeciw nim jednostki walcz¹ce wrêcz, aby je pokonaæ.'
WHERE Tag = 'TXT_KEY_UNIT_HUN_BATTERING_RAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Barbarzyñcy z tego obozu przy³¹czyli siê do twojej armii. Niech ¿yje wielki Attyla!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_BARB_CAMP_CONVERTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '£ucznicy konni to szybkie jednostki dystansowe, œmiercionoœne na otwartym terenie. Rozpoczynaj¹ z awansem "Celnoœæ I". Jako jednostka kawaleryjska, s¹ wra¿liwi w walce z w³ócznikami. W przeciwieñstwie do strzelców rydwanowych nie wymagaj¹ koni.'
 WHERE Tag = 'TXT_KEY_UNIT_HUN_HORSE_ARCHER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Inca
--------------------
UPDATE Language_pl_PL
SET Text = 'Jednostki ignoruj¹ koszt terenu na wzgórzach i mog¹ przekraczaæ góry. Miasta, drogi i kolej mog¹ byæ budowane na górach.'
WHERE Tag = 'TXT_KEY_TRAIT_GREAT_ANDEAN_ROAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Ten unikatowy oddzia³ Inków zastêpuje ³uczników. Procarze nie s¹ silniejsi od ³uczników - w walce w zwarciu s¹ wrêcz od nich s³absi - posiadaj¹ jednak zdolnoœæ daj¹c¹ du¿e szanse na wycofanie siê na ty³y zanim nast¹pi atak wrêcz i mog¹ atakowaæ dwa razy na turê.' 
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Procarze to staro¿ytna jednostka piechoty atakuj¹ca z dystansu. Ten unikatowy oddzia³ Inków mo¿e uciec przed wiêkszoœci¹ ataków w zwarciu i atakuje dwa razy: warto z nich korzystaæ do nêkania wrogów. Procarze mog¹ jednak zostaæ ³atwo rozgromieni, jeœli przeciwnik zablokuje im drogê ucieczki lub bêdzie ich œciga³ znacznie szybszymi oddzia³ami.'
WHERE Tag = 'TXT_KEY_CIV5_INCA_SLINGER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Uprawy tarasowe mo¿na zbudowaæ jedynie na wzgórzach. Nie wymagaj¹ one dostêpu do œwie¿ej wody. Zbudowanie ich na zboczu góry bêdzie dostarczaæ dodatkowej ¿ywnoœci (1 na przylegaj¹c¹ gór¹). +1 [ICON_FOOD] ¿ywnoœci na przylegaj¹ce uprawy tarasowe a wszystkie przylegaj¹ce farmy produkuj¹ +1 [ICON_FOOD] ¿ywnoœci.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TERRACE_FARM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- India
--------------------
UPDATE Language_pl_PL
SET Text = 'Rozpoczyna z panteonem. Ka¿dy wyznawca twojej dominuj¹cej lub za³o¿onej religii w mieœcie zwiêksza nacisk religijny i [ICON_GROWTH] wzrost w tym mieœcie. Nie mo¿e budowaæ misjonarzy.'
WHERE Tag = 'TXT_KEY_TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Naga-Malla', Gender = 'masculine', Plurality = '1' WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Przera¿aj¹cy widok na polu walki, czêsto by³y dla sojuszników takim samym zagro¿eniem, jak dla wrogów, i mog¹ zostaæ uznane za pierwsz¹ broñ masowego ra¿enia. Naga-Malla (termin z jêzyka Hindi okreœlaj¹cy s³onia bojowego) mo¿na uznaæ za pierwsz¹ broñ masowego zniszczenia. Hindusi u¿ywali s³oni w walce a¿ do wynalezienie prochu, co spowodowa³o, ¿e sta³y siê bezu¿yteczne. W walkach przed wynalezieniem prochu s³onie bojowe mia³y dwa podstawowe zadania. Po pierwsze, ich zapach œmiertelnie przera¿a³ konie, wy³¹czaj¹c kawaleriê wroga z u¿ytku. Po drugie, mog³y przebiæ siê przez najsilniejsz¹ nawet liniê piechoty, mia¿d¿¹c œcianê w³óczni, która by³a nie do pokonania dla ¿adnego konia. S³onie by³y wyj¹tkowo trudne do zabicia, historia odnotowuje przypadki, gdy prze¿y³y trafienia szeœædziesiêciu i wiêcej strza³. Podstawowy problem z u¿ywaniem s³oni polega³ na tym, ¿e z wœciek³oœci lub bólu czêsto wpada³y w sza³ i jeŸdziec nie móg³ ich opanowaæ. JeŸdŸcy dosiadaj¹cy s³oni czêsto uzbrojeni byli we gwóŸdŸ i m³ot, których mogli u¿yæ do zabicia zwierzêcia, je¿eli zaatakowa³o ono w³asne jednostki.'
WHERE Tag = 'TXT_KEY_CIV5_ANTIQUITY_INDIANWARELEPHANT_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Potê¿na jednostka strzelaj¹ca, bardzo skuteczna na p³askim terenie. Mog¹ j¹ stworzyæ tylko Hindusi. Ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ i si³ê ostrza³u ni¿ kirasjerzy oraz nie otrzymuje kary podczas ataku na miasto, ale posiada mniejsz¹ prêdkoœæ ruchu. Nie wymaga [ICON_RES_HORSE] koni i jest dostêpna wczeœniej ni¿ kirasjerzy.' 
WHERE Tag = 'TXT_KEY_UNIT_HELP_INDIAN_WAR_ELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Naga-Malla (s³oñ bojowy) to unikatowa jednostka indyjska, zastêpuj¹ca kirasjerów. Jest od nich silniejsza (ale wolniejsza). Nie wymaga koni i jest dostêpna wczeœniej ni¿ kirasjerzy. Naga-Malla to jednostka kawaleryjska, s³aba w walce z pikinierami.' 
WHERE Tag = 'TXT_KEY_UNIT_INDIAN_WARELEPHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Traits
SET ShortDescription = 'TXT_KEY_TRAIT_POPULATION_GROWTH_SHORT_CBP'
WHERE Type = 'TRAIT_POPULATION_GROWTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Indonesia
--------------------
UPDATE Language_pl_PL
SET Text = 'Unikalny indonezyjski budynek zastêpuj¹cy ogród. Surowiec: [ICON_RES_CLOVES] goŸdziki, [ICON_RES_PEPPER] pieprz lub [ICON_RES_NUTMEG] muszkato³owiec pojawi siê obok albo pod tym miastem po wybudowaniu.[NEWLINE][NEWLINE]10% wytwarzanej przez miasto [ICON_CULTURE] kultury dodaje siê ka¿dej tury do jego [ICON_RESEARCH] nauki. +25% do tworzenia [ICON_GREAT_PEOPLE] wielkich ludzi w mieœcie oraz +2 punkty [ICON_PEACE] wiary za ka¿d¹ œwiatow¹ religiê maj¹c¹ w mieœcie co najmniej 1 wyznawcê.[NEWLINE]Pobliskie [ICON_RES_CITRUS] cytrusy: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_COCOA] kakao: +1 [ICON_FOOD] ¿ywnoœci, +1 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Za ka¿dym razem, gdy za³o¿ysz miasto, jeden z trzech unikalnych surowców luksusowych pojawi siê obok lub pod miastem. Brak [ICON_HAPPINESS_3] niezadowolenia z izolacji.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '50% szansa na wyleczenie 10 P¯, jeœli zakoñczy turê na terenie wroga.'
 WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Skaza wroga'
 WHERE Tag = 'TXT_KEY_PROMOTION_ENEMY_BLADE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '+5% [ICON_STRENGTH] si³y ataku, +30% [ICON_STRENGTH] si³y bojowej w obronie.'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Ochrona przodków'
WHERE Tag = 'TXT_KEY_PROMOTION_EVIL_SPIRITS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = '+30% [ICON_STRENGTH] si³y ataku, +5% [ICON_STRENGTH] si³y bojowej w obronie.'
WHERE Tag = 'TXT_KEY_PROMOTION_AMBITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Sumpah Palapa'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pojawia siê jako wyj¹tkowy towar luksusowy w lub obok indonezyjskiego miasta.'
WHERE Tag = 'TXT_KEY_RESOURCE_NUTMEG_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pojawia siê jako wyj¹tkowy towar luksusowy w lub obok indonezyjskiego miasta.'
WHERE Tag = 'TXT_KEY_RESOURCE_CLOVES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Pojawia siê jako wyj¹tkowy towar luksusowy w lub obok indonezyjskiego miasta.'
WHERE Tag = 'TXT_KEY_RESOURCE_PEPPER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Iroquois
--------------------
UPDATE Language_pl_PL
SET Text = 'Jednostki poruszaj¹ siê przez las i d¿unglê jak po drodze i mo¿na u¿yæ tych pól do [ICON_CONNECTED] po³¹czenia miast. L¹dowe jednostki wojskowe zaczynaj¹ z awansem "Leœnik".'
WHERE Tag = 'TXT_KEY_TRAIT_IGNORE_TERRAIN_IN_FOREST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+1 [ICON_PRODUCTION] produkcji i +1 [ICON_FOOD] ¿ywnoœci ze wszystkich lasów i d¿ungli eksploatowanych przez to miasto. Wymaga pobliskiego, daj¹cego siê wykorzystaæ pola lasu lub d¿ungli.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'D³ugi dom to unikalny budynek Irokezów, zastêpuj¹cy aptekê. Zwiêksza wydajnoœæ produkcji ka¿dego uprawianego pola leœnego le¿¹cego w zasiêgu miasta.'
WHERE Tag = 'TXT_KEY_BUILDING_LONGHOUSE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

--------------------
-- Japan
--------------------
UPDATE Language_pl_PL
SET Text = '[ICON_STRENGTH] si³a bojowa twoich jednostek roœnie wraz z otrzymywanymi obra¿eniami. +2 [ICON_CULTURE] kultury i +1 [ICON_CULTURE] ¿ywnoœci z ³odzi rybackich i atoli. JEdnostki morskie walcz¹ce w zwarciu mog¹ budowaæ ³odzie rybackie.'
WHERE Tag = 'TXT_KEY_TRAIT_FIGHT_WELL_DAMAGED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Samurajowie to unikalna jednostka japoñska zastêpuj¹ca ciê¿kozbrojnych. S¹ równie silni jak zbrojni, sukces w walce samurajami zapewnia wiêksze szanse stworzenia wielkich genera³ów i otrzymuj¹ wiêcej doœwiadczenia z walki.'
 WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_SAMURAI_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Potê¿na œredniowieczna jednostka piechoty. Mog¹ j¹ wyprodukowaæ tylko Japoñczycy. Walczy o wiele skuteczniej, otrzymuje wiêcej doœwiadczenia z walki i pozwala szybciej wyprodukowaæ wielkich genera³ów, ni¿ ciê¿kozbrojni, których zastêpuje.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_SAMURAI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka lotnicza przeznaczona do walki o panowanie w powietrzu i przechwytywania nadlatuj¹cych maszyn wroga. Jednostka otrzymuje premiê do walki z innymi myœliwcami i nie wymaga ropy naftowej. Wymaga Autokracji.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_ZERO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Zero jest doœæ mocn¹ jednostk¹ lotnicz¹, wyj¹tkow¹ dla Autokracji. Jest podobna do myœliwców, ale otrzymuje du¿e premie do walki z innymi myœliwcami, a [COLOR_POSITIVE_TEXT]do budowy nie wymaga Ÿród³a ropy naftowej[ENDCOLOR]. Mo¿e stacjonowaæ w ka¿dym posiadanym przez ciebie mieœcie lub na pok³adzie lotniskowca. Mo¿e poruszaæ siê z miasta do miasta (lub lotniskowca), a tak¿e wykonywaæ misje w odleg³oœci do 6 pól. W zasadach dotycz¹cych jednostek powietrznych znajduje siê wiêcej informacji.'
 WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_ZERO_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

--------------------
-- Korea 
--------------------
UPDATE Language_pl_PL
SET Text = '+1 do [ICON_RESEARCH] nauki dla wszystkich specjalistów i +2 do [ICON_RESEARCH] nauki ze wszystkich ulepszeñ pól wielkich ludzi. Otrzymuje premiê technologiczn¹ za ka¿dym razem, gdy w stolicy Korei pojawi siê budynek naukowy lub cud. Premia zale¿y od epoki.'
WHERE Tag = 'TXT_KEY_TRAIT_SCHOLARS_JADE_HALL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Kobukson mo¿e byæ jedynie podarowany przez pomniejsz¹ cywilizacjê.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kobukson ma on wiêksz¹ si³ê ognia ni¿ karawela, jest te¿ wyj¹tkowo trudny do zniszczenia. Nie mo¿e on jednak wyp³ywaæ na pola otwartego morza poza granicami miasta.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- Maya
--------------------
UPDATE Language_pl_PL
SET Text = 'Po zbadaniu matematyki otrzymasz dodatkowego wielkiego cz³owieka na koniec ka¿dego cyklu d³ugiej rachuby kalendarza Majów (co 394 lata). Ka¿dy dodatkowy cz³owiek mo¿e byæ wybrany tylko raz.'
WHERE Tag = 'TXT_KEY_TRAIT_LONG_COUNT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jedynie Majowie mog¹ wybudowaæ tê jednostkê. Jest wczeœniej dostêpna ni¿ ³ucznik z ³ukiem kompozytowym, którego zastêpuje i zdaje dodatkowe obra¿enia zranionym jednostkom.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MAYAN_ATLATLIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Oszczepnicy to unikatowa jednostka Majów, która zastêpuje ³uczników z ³ukiem kompozytowym. Jest od nich tañsza, wczeœniej dostêpna i zdaje dodatkowe obra¿enia zranionym jednostkom. Dziêki tej przewadze twoi ³ucznicy mog¹ stosowaæ w potyczkach taktykê ataku i ucieczki.'
WHERE Tag = 'TXT_KEY_UNIT_MAYAN_ATLATLIST_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Mongols
--------------------
UPDATE Language_pl_PL
SET Text = 'Konne jednostki strzelaj¹ce otrzymuj¹ +2 [ICON_MOVES] ruchu i ignoruj¹ wrog¹ strefê kontroli. Anektujesz si³¹ pañstwa-miasta zamiast otrzymywania du¿ej daniny oraz otrzymujesz punkty [ICON_GOLDEN_AGE] z³otej ery kiedy podbijasz lub anektujesz pañstwo-miasto.'
WHERE Tag = 'TXT_KEY_TRAIT_TERROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Harcownicy|Harcowników|Harcownikom|Harcowników', Gender = 'masculine', Plurality = '2'WHERE Tag = 'TXT_KEY_UNIT_MONGOL_KESHIK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'W XII wieku plemiona nomadów zaczê³y ekspansjê militarn¹ z Azji centralnej, podbijaj¹c wiêksz¹ czêœæ Azji, Wschodni¹ Europê, i znaczn¹ czêœæ Bliskiego Wschodu zaledwie w ci¹gu jednego stulecia. Ich g³ówn¹ broni¹ byli nie maj¹cy sobie równych strzelcy konni. Mongo³owie byli jednym z takich nomadycznych plemion a ich dzieci by³y niemal dos³ownie "urodzone w siodle". Je¿d¿¹c na niskich, ale szybkich i wytrwa³ych kucykach, lekkozbrojni mongolscy Keshikowie, rodzaj harcowników, mogli pokonywaæ niesamowite wrêcz odleg³oœci w ci¹gu jednego dnia. To dawa³o im przewagê nad wszelk¹ piechot¹, a nawet nad ciê¿sz¹ kawaleri¹ Europejsk¹.[NEWLINE][NEWLINE]W walce mongolscy Keshikowie strzelali z koñskiego grzbietu ze œmierteln¹ precyzj¹, rozbijaj¹c zarówno szyki, jak i morale wrogich armii. Na tak zmiêkczonego przeciwnika spada³a ciê¿ka kawaleria, która koñczy³a bitwê. W walce z ciê¿kozbrojn¹ kawaleri¹ europejsk¹ Mongo³owie strzelali w bardziej ods³oniête wierzchowce, ignoruj¹c lub dobijaj¹c pozbawionych koni rycerzy - zale¿nie od sytuacji.'
 WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
		
UPDATE Language_pl_PL 
SET Text = 'Strzelcy konni s¹ silni w atakach dystansowych, maj¹ zwiêkszon¹ szybkoœæ ruchu, co pozwala im na uderzenie i ucieczkê.'
 WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Harcownicy s¹ szybk¹ jednostk¹ strzeleck¹, zabójcz¹ na otwartym terenie. W przeciwieñstwie do poprzedzaj¹cych ich rydwanów, mog¹ siê poruszaæ po trudnym terenie bez kary do ruchu. Jako kawaleria, harcownicy s¹ wra¿liwi na ataki jednostek z w³óczniami.'
 WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Chan to unikalny mongolski rodzaj wielkiego cz³owieka zastêpuj¹cy zwyk³ego wielkiego genera³a. Stosuj¹ siê do niego te same premie w walce, lecz porusza siê on z prêdkoœci¹ 5 pkt na turê i mo¿e leczyæ s¹siaduj¹ce jednostki o dodatkowych pkt ¿ycia na turê. Ten wzmocniony genera³ potrafi dopilnowaæ, by jego kawaleria zawsze by³a gotowa do walki.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KHAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} {1: plurality 1?{1: gender masculine?zmusi³; feminine?zmusi³a; neuter?zmusi³o} 2?{1: gender masculine?zmusili; *?zmusi³y}} {2_CS[4]} do poddañstwa przez zastraszanie ich!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', 'Pañstwo-miasto {1_CS} podda³o siê!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN', '{1_Bully} {1: plurality 1?{1: gender masculine?zmusi³; feminine?zmusi³a; neuter?zmusi³o} 2?{1: gender masculine?zmusili; *?zmusi³y}} niepoznane pañstwo-miasto do poddañstwa przez zastraszanie ich!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_UNKNOWN_CS_BULLY_ANNEXED_KNOWN_SUMMARY', '{1_Bully} {1: plurality 1?{1: gender masculine?zmusi³; feminine?zmusi³a; neuter?zmusi³o} 2?{1: gender masculine?zmusili; *?zmusi³y}} pañstwo-miasto do poddañstwa!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN', 'Niespotkany gracz zmusi³ {1_CS[4]} do poddañstwa przez zastraszanie ich!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_KNOWN_CS_BULLY_ANNEXED_UNKNOWN_SUMMARY', 'PAñstwo-miasto {1_CS} podda³o siê!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_ANNEXED_CS', 'Zmusi³eœ {1_CS[4]} do poddañstwa przez zastraszanie ich!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT_ANNEX', 'Zaanektowano si³¹ pañstwo-miasto'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT_ANNEX', 'Jeœli to pañstwo miasto jest ciebie bardziej [COLOR_POSITIVE_TEXT]wystraszone[ENDCOLOR] ni¿ [COLOR_WARNING_TEXT]odporne[ENDCOLOR], mo¿esz je zaanektowaæ. Robi¹c to zyskasz wiele punktów [ICON_GOLDEN_AGE] z³otej ery. {1_FearLevel}{2_FactorDetails}'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

INSERT INTO Language_pl_PL (
Tag, Text)
SELECT 'TXT_KEY_BALANCE_ANNEXED_CS_SUMMARY', 'Zastraszy³eœ {1_CS[4]}!'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

--------------------
-- Morocco
--------------------
UPDATE Language_pl_PL
SET Text = 'Kasbê zbudowaæ mo¿na jedynie na polu przylegaj¹cym do miasta; generuje ona dodatkow¹ [ICON_FOOD] ¿ywnoœæ, [ICON_PRODUCTION] produkcjê i [ICON_GOLD] z³oto. Zapewnia równie¿ 30% premiê do obrony i do³¹cza wszystkie surowce luksusowe lub strategiczne pod sob¹ do twojej sieci handlowej.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Kasba mo¿e powstaæ jedynie na polu przylegaj¹cym do posiadanego miasta. generuje ona dodatkow¹ [ICON_FOOD] ¿ywnoœæ, [ICON_PRODUCTION] produkcjê, [ICON_GOLD] z³oto i [ICON_CULTURE] kulturê. Zapewnia równie¿ 30% premiê do obrony i do³¹cza wszystkie surowce luksusowe lub strategiczne pod sob¹ do twojej sieci handlowej.[NEWLINE][NEWLINE]Kasba jest rodzajem medyny (otoczonej murem dzielnicy miasta), któr¹ mo¿na znaleŸæ na prowincji, zazwyczaj w ma³ych osiedlach na szczycie lub zboczu wzgórza. Zazwyczaj w kasbie zamieszkiwa³ kacyk plemienia lub wa¿ny islamski imam, kasbê charakteryzuj¹ wiêc wysokie mury, domy bez okien i w¹skie, krête uliczki. Zwykle dominuje nad nimi pojedyncza ufortyfikowana wie¿a. Kasby czêsto budowano wzd³u¿ pó³nocnego wybrze¿a Afryki i na Bliskim Wschodzie, a¿ do pocz¹tków XX w. Zbudowanie kasby by³o dla marokañskich i algierskich rodzin pochodzenia arabskiego oznak¹ bogactwa, wp³ywów i potêgi.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_KASBAH_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '+3 [ICON_GOLD] z³ota, punktów [ICON_GOLDEN_AGE] z³otej ery i [ICON_CULTURE] kultury za ka¿dy szlak do albo z innej cywilizacji lub pañstwa-miasta. Premia zale¿na od epoki. W³aœciciele szlaków handlowych otrzymuj¹ +2 [ICON_GOLD] z³ota za ka¿dy szlak poprowadzony do Maroka.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Jednostka kawalerii wyspecjalizowana w obronie terytorium Maroka. Otrzymuje premiê do wartoœci bojowej podczas walki na terenie Maroka. Ignoruje kary terenowe. Mo¿e powstawaæ tylko w Maroku.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BERBER_CAVALRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );


--------------------
-- Netherlands
--------------------
UPDATE Language_pl_PL
SET Text = '+4 [ICON_CULTURE] kultury za ka¿dy ró¿ny surowiec luksusowy, który importujesz od innych cywilizacji i pañstw-miast, +4 [ICON_GOLD] z³ota za ka¿dy ró¿ny surowiec luksusowy, który eksportujesz do innych cywilizacji. Premia zale¿y od epoki.'
WHERE Tag = 'TXT_KEY_TRAIT_LUXURY_RETENTION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Polder mo¿na budowaæ wy³¹cznie na obszarach z dostêpem do s³odkiej wody. Generuje [ICON_FOOD] ¿ywnoœæ, [ICON_GOLD] z³oto, i [ICON_PRODUCTION] produkcjê oraz dostarcza [ICON_GOLD] z³ota s¹siednim wioskom i miasteczkom. Dostarcza dodatkowe zyski po odkryciu póŸniejszych technologii.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_POLDER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Polder mo¿na budowaæ wy³¹cznie na obszarach z dostêpem do s³odkiej wody. Generuje [ICON_FOOD] ¿ywnoœæ, [ICON_GOLD] z³oto, i [ICON_PRODUCTION] produkcjê oraz dostarcza [ICON_GOLD] z³ota s¹siednim wioskom i miasteczkom. Dostarcza dodatkowe zyski po odkryciu póŸniejszych technologii.[NEWLINE][NEWLINE]Polder to nisko po³o¿ony grunt otoczony groblami i osuszony. Ogólnie rzecz bior¹c s¹ to tereny odebrane jeziorom, morzu, terenom zalewowym lub bagnom. Z czasem osuszony grunt osiada i po pewnym czasie ca³y polder znajduje siê poni¿ej poziomu otaczaj¹cej go wody. Sprawia to, ¿e wdziera siê tam ona i musi zostaæ wypompowana lub osuszona w inny sposób. Groble robi siê zazwyczaj ze znajduj¹cych siê pod rêk¹ materia³ów, piasku lub ziemi. W dzisiejszych czasach pokrywa siê je lub odlewa w ca³oœci z betonu. Osuszony grunt jest niezwykle ¿yzny i stanowi doskona³e pastwisko lub pole uprawne.[NEWLINE][NEWLINE]Pierwsze poldery stworzono w XI wieku naszej ery, choæ budowanie wa³ów zatrzymuj¹cych wodê znane by³o ju¿ za czasów rzymskich. D³ug¹ historiê odzyskiwania ziemi spod bagien, a nawet morza, maj¹ Holendrzy – posiadaj¹ oni po³owê wszystkich polderów w Europie. Znajduj¹ siê one te¿ w innych krajach, równie¿ w Azji i Ameryce Pó³nocnej, Holandia jednak ma oko³o 3000 polderów, które stanowi¹ 27% jej powierzchni. Amsterdam w du¿ej czêœci zbudowany jest na polderze. Jak mówi holenderskie przys³owie – „Bóg stworzy³ œwiat, lecz Holendrzy Holandiê”.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_POLDER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Ottomans
--------------------
UPDATE Language_pl_PL
SET Text = 'Tanzimat'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Janczarzy to unikatowa jednostka osmañska, zastêpuj¹ca arkebuzerów. S¹ od nich silniejsi i wczeœniej dostêpni. Na dodatek janczarzy lecz¹ siê w ka¿dej turze. Daje to im ogromn¹ przewagê w walce przeciwko broni¹cemu siê przeciwnikowi.'
 WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_JANISSARY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
 
UPDATE Language_pl_PL 
SET Text = 'Jedna z pierwszych jednostek z broni¹ paln¹. Mog¹ j¹ stworzyæ tylko Osmanie. Jest silniejsza ni¿ tercio i wczeœniej dostêpna.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_JANISSARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Spahisi mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_SIPAHI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Spahisi s¹ o wiele szybsi, maj¹ zasiêg widzenia wiêkszy o jedno pole ni¿ lansjerzy, ale tak samo jak oni, s¹ s³abi w obronie. Spahisi mog¹ pl¹drowaæ nieprzyjacielskie ulepszenia, nie ponosz¹c dodatkowych kosztów.'
 WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_SIPAHI_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ukoñczenie [ICON_INTERNATIONAL_TRADE] miêdzynarodowego szlaku handlowego daje +100 [ICON_RESEARCH] nauki, [ICON_PRODUCTION] produkcji, i [ICON_FOOD] ¿ywnoœci wyjœciowemu miastu. Premie zale¿¹ od epoki. Otrzymujesz darmowy karawanseraj w ka¿dym mieœcie.'
WHERE Tag = 'TXT_KEY_TRAIT_CONVERTS_SEA_BARBARIANS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- Persia
--------------------
UPDATE Language_pl_PL
SET Text = '+1 [ICON_GOLD] z³ota i +1 punkt [ICON_GOLDEN_AGE] z³otej ery na turê za ka¿dych 5 [ICON_CITIZEN] mieszkañców w tym mieœcie. Eliminuje dodatkowe [ICON_HAPPINESS_4] niezadowolenie z [ICON_OCCUPIED] okupowania miasta (jeœli jest [ICON_OCCUPIED] okupowane).'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'S¹d satrapy to unikalny budynek perski zastêpuj¹cy s¹d i w przeciwieñstwie, mo¿e byæ zbudowany w ka¿dym mieœcie oraz potrzebuje mniej produkcji. Zwiêksza iloœæ z³ota wytwarzanego przez miasto, zadowolenie i zwiêksza twój przyrost punktów z³otej ery o 1 na ka¿dych 5 mieszkañców w mieœcie.'
WHERE Tag = 'TXT_KEY_BUILDING_SATRAPS_COURT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = '[ICON_GOLDEN_AGE] Z³ote ery trwaj¹ o 50% d³u¿ej i 10% twojej wytwarzanej [ICON_TOURISM] turystyki jest zamieniane ka¿dej tury na punkty [ICON_GOLDEN_AGE] z³otej ery. Podczas z³otej ery jednostki otrzymuj¹ +1 [ICON_MOVES] ruchu i +15% premii do [ICON_STRENGTH] si³y bojowej.'
WHERE Tag = 'TXT_KEY_TRAIT_ENHANCED_GOLDEN_AGES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka z epoki staro¿ytnej specjalizuj¹ca siê w walce z kawaleri¹. Mog¹ j¹ tworzyæ tylko Persowie. Ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹, premiê do [ICON_STRENGTH] obrony i regeneruje siê szybciej ni¿ w³ócznicy, których zastêpuje.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_IMMORTAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Poland
--------------------
UPDATE Language_pl_PL
SET Text = 'Unikalny dla Polski budynek zastêpuj¹cy stajniê. +25% do [ICON_PRODUCTION] produkcji i +15 PD dla jednostek konnych. [NEWLINE][NEWLINE][NEWLINE]Pobliskie [ICON_RES_HORSE] konie: +3 [ICON_PRODUCTION] produkcji, +3 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_SHEEP] owce: +3 [ICON_PRODUCTION] produkcji, +3 [ICON_GOLD] z³ota.[NEWLINE]Pobliskie [ICON_RES_COW] byd³o: +3 [ICON_PRODUCTION] produkcji, +3 [ICON_GOLD] z³ota.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Polynesia
--------------------
UPDATE Language_pl_PL
SET Text = '+1 do pola widzenia podczas zaokrêtowania. Mo¿e siê zaokrêtowaæ i natychmiast wyp³yn¹æ na ocean. +50% zysków z pól cudów natury i +10% do [ICON_STRENGTH] si³y bojowej, jeœli znajduje siê w odleg³oœci do 4 pól od moai lub cudu natury.'
WHERE Tag = 'TXT_KEY_TRAIT_WAYFINDING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka e œrodkowej czêœci gry, która sieje strach w sercach przeciwników, czyni¹c ich mniej skutecznymi w walce. Jest dostêpna wczeœniej ni¿ pikinierzy, których zastêpuje. Tylko Polinezja mo¿e j¹ produkowaæ.'
 WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Unikalny pikinier, którego mo¿e trenowaæ tylko Polinezja. Wojownicy maoryscy s¹ podobni do wielkich genera³ów; jednak¿e zamiast podnosiæ si³ê sojuszniczych jednostek, obni¿aj¹ si³ê pobliskich jednostek wroga. Zbuduj sporo tych jednostek, jako ¿e ich awans pozostaje po ich ulepszeniu.'
WHERE Tag = 'TXT_KEY_CIV5_POLYNESIAN_MAORI_WARRIOR_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Premia do zbli¿aj¹cej siê walki'
WHERE Tag = 'TXT_KEY_EUPANEL_IMPROVEMENT_NEAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Portugal
--------------------

UPDATE Language_pl_PL
SET Text = 'Faktoriê mog¹ wybudowaæ robotnicy i nau (przez sprzeda¿ egzotycznych dóbr) jedynie na obszarze pañstwa-miasta, na wybrze¿u, gdzie nie znajduj¹ siê surowce. Zapewnia ci ona jedn¹ kopiê ka¿dego luksusowego surowca, do jakiego pañstwo-miasto ma dostêp, ale kopia ta nie podlega wymianie. Szlaki handlowe do tego miasta generuj¹ premiê do [ICON_PRODUCTION] produkcji i [ICON_FOOD] ¿ywnoœci zale¿n¹ od twoich przychodów [ICON_GOLD] z³ota ze [ICON_INTERNATIONAL_TRADE] szlaków handlowych i twoich stosunków z pañstwem-miastem (np. neutralne/przyjacielskie/sojusznicze). [NEWLINE][NEWLINE] W dodatku do tych premii faktoria zapewnia widocznoœæ na swoim polu oraz wszystkich polach w promieniu 3 i tak¹ sam¹ premiê +50% do obrony jak fort. Mog¹ j¹ zbudowaæ tylko Portugalczycy.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Faktoriê mog¹ wybudowaæ robotnicy i nau (przez sprzeda¿ egzotycznych dóbr) jedynie na obszarze pañstwa-miasta, na wybrze¿u, gdzie nie znajduj¹ siê surowce. Zapewnia ci ona jedn¹ kopiê ka¿dego luksusowego surowca, do jakiego pañstwo-miasto ma dostêp, ale kopia ta nie podlega wymianie. Szlaki handlowe do tego miasta generuj¹ premiê do [ICON_PRODUCTION] produkcji i [ICON_FOOD] ¿ywnoœci zale¿n¹ od twoich przychodów [ICON_GOLD] z³ota ze [ICON_INTERNATIONAL_TRADE] szlaków handlowych i twoich stosunków z pañstwem-miastem (np. neutralne/przyjacielskie/sojusznicze). [NEWLINE][NEWLINE] W dodatku do tych premii faktoria zapewnia widocznoœæ na swoim polu oraz wszystkich polach w promieniu 3 i tak¹ sam¹ premiê +50% do obrony jak fort. Mog¹ j¹ zbudowaæ tylko Portugalczycy.[NEWLINE][NEWLINE]Faktoria to nazwa, za pomoc¹ której okreœlano placówki handlowe zak³adane w œredniowieczu na cudzych terytoriach. S³u¿y³a ona jednoczeœnie za targowisko, magazyn, osadê i punkt wypadowy dla wypraw badawczych. Faktorie czêsto zak³adane by³y na mocy grantu udzielonego przez portugalsk¹ koronê, a d³ug swój zwraca³y poprzez kupno i sprzeda¿ towarów w jej imieniu i pobieranie c³a za towary, które tam trafia³y. W XV i XVI w. wzd³u¿ brzegów Afryki, Indii, Malezji, Chin i Japonii zbudowano sieæ ponad 50 faktorii; pozwoli³y one Portugalczykom zdominowaæ szlaki handlowe Oceanu Atlantyckiego i Indyjskiego na trzy stulecia.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FEITORIA_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Jednostka morska walcz¹ca w zwarciu, doskona³a do eksploracji mórz. Ma wiêksz¹ si³ê bojow¹ punktów ruchu ni¿ karawela i mo¿e jednorazowo u¿yæ zdolnoœci, znajduj¹c siê obok pañstwa-miasta, by zyskaæ [ICON_GOLD] z³oto i PD. Mog¹ j¹ tworzyæ tylko Portugalczycy.[NEWLINE][NEWLINE]Jeœli to mo¿liwe, kiedy nau sprzedaje swój towar egzotyczny, na jego terytorium [COLOR_POSITIVE_TEXT]automatycznie powstaje[ENDCOLOR] faktoria.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PORTUGUESE_NAU' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Ró¿norodnoœæ zasobów zapewnia Portugalii dwukrotnie wiêcej [ICON_GOLD] z³ota ze szlaków handlowych. Kiedy twoje jednostki handlowe siê poruszaj¹, otrzymujesz +2 punkty [ICON_RESEARCH] nauki, punkty [ICON_GREAT_ADMIRAL] wielkiego admira³a (za statki towarowe) i punkty [ICON_GREAT_GENERAL] wielkiego genera³a (za karawany), zale¿nie od epoki.'
WHERE Tag = 'TXT_KEY_TRAIT_EXTRA_TRADE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Mo¿e sprzedawaæ egzotyczne dobra, gdy znajduje siê w s¹siedztwie ziem pañstwa-miasta, zdobywaj¹c [ICON_GOLD] z³oto i doœwiadczenie w oparciu o odleg³oœæ od stolicy. Ka¿dy nau mo¿e wykonaæ tê akcjê dwukrotnie.[NEWLINE][NEWLINE]Jeœli to mo¿liwe, kiedy nau sprzedaje swój towar egzotyczny, na jego terytorium [COLOR_POSITIVE_TEXT]automatycznie powstaje[ENDCOLOR] faktoria.'
WHERE Tag = 'TXT_KEY_PROMOTION_SELL_EXOTIC_GOODS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Rome
--------------------
UPDATE Language_pl_PL
SET Text = 'Kiedy podbijasz miasto, zatrzymuje ono wszystkie odpowiednie budynki i natychmiast pozyskujesz dodatkowe terytorium dooko³a miasta. +15% do [ICON_PRODUCTION] produkcji budowli, które znajduj¹ siê ju¿ w [ICON_CAPITAL] stolicy.'
WHERE Tag = 'TXT_KEY_TRAIT_CAPITAL_BUILDINGS_CHEAPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Balisty mog¹ byæ tylko podarowane przez pomniejsz¹ cywilizacjê.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = ' Balisty to doskona³a broñ oblê¿nicza. Œwietnie nadaj¹ siê do atakowania miast, ale s¹ podatne na inne ataki. Nale¿y je chroniæ za pomoc¹ innych jednostek. Przed rozpoczêciem ostrza³u balisty musz¹ zostaæ odpowiednio ustawione (co kosztuje 1 punkt ruchu).'
 WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Silna jednostka l¹dowa z epoki klasycznej. Mog¹ j¹ tworzyæ tylko Rzymianie. Ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ ni¿ zbrojni, których zastêpuje, i otrzymujesz za darmo awans "Os³ona I".'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROMAN_LEGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
UPDATE Language_pl_PL
SET Text = 'Legion to unikatowa jednostka rzymska zastêpuj¹ca zbrojnych. Jest od nich potê¿niejszy, co czyni go jedn¹ z najsilniejszych staro¿ytnych jednostek do walki wrêcz. Jako jedyna jednostka, poza robotnikami, mo¿e budowaæ drogi i forty.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_LEGION_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );



--------------------
-- Russia
--------------------
UPDATE Language_pl_PL
SET Text = '+1 [ICON_PRODUCTION] produkcji z obozów, kopalni i tartaków. Obni¿a koszt pól w tym mieœcie o 25%. Wrogie jednostki l¹dowe musz¹ wydaæ 1 dodatkowy punkt [ICON_MOVES] ruchu na pole, jeœli wchodz¹ na pole wykorzystywane przez to miasto.[NEWLINE][NEWLINE]Zwiêksza obronnoœæ miasta o 12 i punkty ¿ywotnoœci o 125.'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ostróg|Ostroga|Ostrogowi|Ostróg|Ostrogi|Ostrogów|Ostrogom|Ostrogi', Gender = 'masculine'
WHERE Tag = 'TXT_KEY_BUILDING_KREPOST_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ostróg to unikalny budynek rosyjski, zastêpuj¹cy arsena³. Poza normalnymi premiami arsena³y, Ostróg zwiêksza kulturalny rozrost granic o 50% i zapewnia +1 produkcji z obozów, kopalni i tartaków. Sprawia te¿, ¿e wrogie jednostki l¹dowe musz¹ wydaæ 1 dodatkowy punkt ruchu na pole, jeœli wchodz¹ na pole wykorzystywane przez to miasto. Wymagania produkcji, z³ota i utrzymania Ostroga s¹ równie¿ wielce obni¿one, pozwalaj¹c ci na jego szybk¹ budowê w nowo za³o¿onych miastach. Warte zachodu jest zbudowanie tych u¿ytecznych budynków we wszystkich rosyjskich miastach, a szczególnie w tych na rosyjskim froncie.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ostróg to rosyjska nazwa dla ma³ej fortyfikacji, zwykle drewnianej i nie stale obs³ugiwanej. Ostrogi by³ szeroko u¿ywane podczas epoki rosyjskiej ekspansji imperium, zw³aszcza w XVIII i pocz¹tkach XIX wieku. Ostrogi by³ otoczone palisad¹ o wysokoœci 6 metrów, zrobion¹ z zaostrzonych pni. Nazwa wywodzi siê od s³owiañskiego "strugaæ". Ostrogi by³ mniejsze i jedynie o wojskowym przeznaczeniu, w porównaniu do wiêkszych kremli, które by³ rdzeniami rosyjskich miast. Ostrogi by³y czêsto budowane w odleg³ych miejscach albo na liniach fortyfikacji, takich jak zasechniaja cherta.'
WHERE Tag = 'TXT_KEY_BUILDINGS_KREPOST_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wszystkie surowce strategiczne dostarczaj¹ +1 [ICON_PRODUCTION] nauki i podwaja siê ich zwyk³a liczba. Kiedy zakupujesz pole za [ICON_GOLD] z³oto, otrzymujesz +20 [ICON_RESEARCH] nauki. Premia zale¿y od epoki.'
WHERE Tag = 'TXT_KEY_TRAIT_STRATEGIC_RICHES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Spain
--------------------
UPDATE Language_pl_PL
SET Text = 'Religie inne ni¿ za³o¿ona przez ciebie lub religia wiêkszoœci mog¹ szerzyæ siê do posiadanych miast i sojuszniczych pañstw-miast. Kiedy zyskujesz nowe miasto po swojej [ICON_CAPITAL] stolicy, jego [ICON_CITIZEN] mieszkañcy s¹ nawracani na twoj¹ religiê i otrzymujesz [ICON_PEACE] wiarê.'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Rekonkwista'
WHERE Tag = 'TXT_KEY_TRAIT_SEVEN_CITIES_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Tercio to potê¿na jednostka walcz¹ca w zwarciu z epoki renesansu. U¿ywaj ich, aby chroniæ twoje wra¿liwe jednostki strzelnicze i zdobywaæ miasta.'
 WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Ten Tercio reprezentuje pojawienie siê wczesnych taktyk w wykorzystaniem broni prochowych ("piki i strza³y") podczas epoki renesansu. Jest to jednostka dwukrotnie skuteczniejsza w walce z jednostkami kawalerii, co czyni j¹ skutecznym przeciwnikiem dla lansjerów oraz wszystkich rycerzy wci¹¿ pozostaj¹cych na polu bitwy.'
 WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

--------------------
-- Shoshone
--------------------

UPDATE Language_pl_PL 
SET Text = 'Konni Komancze mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_SHOSHONE_COMANCHE_RIDERS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Tañsi i szybsi ni¿ kawaleria. Mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
 WHERE Tag = 'TXT_KEY_UNIT_SHOSHONE_COMANCHE_RIDERS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

 
--------------------
-- Siam
--------------------
UPDATE Language_pl_PL
SET Text = 'Redukuje efektywnoœæ wrogich szpiegów o 50%. +4 [ICON_RESEARCH] nauki ze œwi¹tyñ i kaplic oraz +1 [ICON_CULTURE] kultury z pól d¿ungli i lasu eksploatowanych przez to miasto. Redukuje [ICON_HAPPINESS_3] przestêpczoœæ.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Wat jest unikalnym budynkiem syjamskim, zastêpuj¹cym policjê okrêgow¹. Podnosi [ICON_CULTURE] kulturê i [ICON_RESEARCH] naukê w mieœcie, zwiêksza wytwarzan¹ [ICON_RESEARCH] naukê ze œwi¹tyñ i kaplic oraz obni¿a efektywnoœæ wrogich szpiegów znacznie bardziej ni¿ policja okrêgowa. Otrzymujesz te¿ dodatkowego specjalistê naukowca, co pozwala ci na produkcjê wielkich naukowców szybciej ni¿ inne cywilizacje.'
WHERE Tag = 'TXT_KEY_BUILDING_WAT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Potê¿na œredniowieczna jednostka kawaleryjska, s³aba w walce z pikinierami. Mog¹ j¹ tworzyæ wy³¹cznie Syjamczycy. Otrzymuje premiê w walce z innymi jednostkami kawalerii, otrzymuj¹ za darmo awans "Z³owroga reputacja" i ma wiêksz¹ [ICON_STRENGTH] si³ê bojow¹ ni¿ rycerze, których zastêpuje.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIAMESE_WARELEPHANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Zyski z przyjacielskich i sojuszniczych pañstw-miast zwiêkszone o 75%. [ICON_STRENGTH] si³a bojowa sojuszniczych [ICON_CAPITAL] pañstw-miast zwiêkszona o 25%'
WHERE Tag = 'TXT_KEY_TRAIT_CITY_STATE_BONUSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Songhai
--------------------
UPDATE Language_pl_PL
SET Text = 'Zapewnia +1 [ICON_CULTURE] kultury ze wszystkich pól rzecznych przy mieœcie i +10% [ICON_PRODUCTION] produkcji podczas wznoszenia budynków w tym mieœcie.[NEWLINE]Pobliski [ICON_RES_MARBLE] marmur: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.[NEWLINE]Pobliski [ICON_RES_STONE] kamieñ: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_SALT] sól: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE]Miasto musi posiadaæ co najmniej jeden z tych surowców ulepszony przez kamienio³om.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tabya to unikatowy budynek Songhajów, zastêpuj¹cy zak³ad kamieniarski. Znacznie powiêksza zysk [ICON_CULTURE] kultury z pól rzecznych, zwiêksza wartoœæ kamienia, marmuru i soli oraz przyspiesza produkcjê przysz³ych budynków o 10%. Pozwala te¿ na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzd³u¿ szlaków handlowych wewn¹trz twojej cywilizacji.'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Tabya|Tabyi|Tabyi|Tabyê|Tabye|Tabyi|Tabyiom|Tabyie', Gender = 'feminine'
WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_DESC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Adobe, hiszpañskie s³owo dla ceg³y z b³ota, jest naturalnym materia³em budowlanym z piasku, gliny, wody i pewnego rodzaju w³óknistego organicznego materia³u (patyki, s³oma, gnój). Takie niewypalane ceg³y s¹ otrzymywane poprzez umieszczanie takiego materia³u w formie i suszenie go na s³oñcu. Wielki Meczet w D¿enne, w centrum Mali, jest najwiêksz¹ struktur¹ glinian¹ na œwiecie. On, podobnie jak wiele budynków architektury sehelskiej, zosta³ zbudowany z cegie³ glinianych zwanych Banco: mieszanka b³ota i ³upin ziaren, fermentowana, i albo formowana w ceg³y albo k³adziona na powierzchnie jako zaprawa jak pasta szerokimi poci¹gniêciami. Ta zaprawa musia³a byæ ponownie nak³adania co rok. Budynki, w których otrzymywano te materia³y by³y nazywane Tabya (warsztaty lepiankowe) i odgrywa³y znacz¹c¹ role w architekturze Zachodniej Afryki.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDING_MUD_PYRAMID_MOSQUE_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Potrójna iloœæ [ICON_GOLD] z³ota z pl¹drowania obozowisk barbarzyñców i miast. Jednostki l¹dowe otrzymuj¹ awanse „Canoe” i „Desant” i poruszaj¹ siê szybciej wzd³u¿ rzek. Rzeki mog¹ tworzyæ [ICON_CONNECTED] po³¹czenia miast.'
WHERE Tag = 'TXT_KEY_TRAIT_AMPHIB_WARLORD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );


--------------------
-- Sweden
--------------------
UPDATE Language_pl_PL
SET Text = 'L¹dowe jednostki walcz¹ce wrêcz otrzymuj¹ +10% [ICON_STRENGTH] si³y ataku, a jednostki oblê¿nicze uzyskuj¹ +1 [ICON_MOVES] ruchu. Jeœli [ICON_GREAT_GENERAL] narodzi siê podczas wojny, wszystkie jednostki militarne s¹ leczone i otrzymuj¹ +10 PD.'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Karolanie to krêgos³up armii szwedzkiej w epoce renesansu. Rozpoczynaj¹ z awansem „Marsz”, co pozwala im leczyæ siê w ka¿dej turze, nawet jeœli wykonuj¹ inne dzia³ania.'
 WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_CAROLEAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Hakkapelitas mog¹ byæ tylko podarowani przez pomniejsz¹ cywilizacjê.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_SWEDISH_HAKKAPELIITTA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'W miarê mo¿liwoœci do³¹cz do nich wielkiego genera³a. Otrzyma on prêdkoœæ ruchu Hakkapelitas, jeœli rozpoczn¹ turê jako grupa. Dodatkowo, Hakkapelitas otrzyma 15% premii bojowej bêd¹c w grupie z wielkim genera³em.'
 WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_HAKKAPELIITTA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Lew Pó³nocy'
WHERE Tag = 'TXT_KEY_TRAIT_DIPLOMACY_GREAT_PEOPLE_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

--------------------
-- Venice 
--------------------
UPDATE Language_pl_PL 
SET Text = 'Wielki galeas mo¿e byæ jedynie podarowany przez pomniejsz¹ cywilizacjê'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Wielki galeas mo¿e byæ jedynie podarowany przez pomniejsz¹ cywilizacjê. Ma silniejszy atak dystansowy i jest wytrzymalszy w walce.' 
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_GALLEASS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Nie mo¿e zyskiwaæ osadników ani anektowaæ miast. Podwaja normaln¹ liczbê dostêpnych szlaków handlowych. Po wynalezieniu ¿eglugi pojawia siê wenecki kupiec. Mo¿e dokonywaæ zakupów w miastach marionetkowych.'
 WHERE Tag = 'TXT_KEY_TRAIT_SUPER_CITY_STATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

 
--------------------
-- Zulu
--------------------
UPDATE Language_pl_PL
SET Text = 'Pobliskie kopalnie dostarczaj¹ +1 [ICON_PRODUCTION] produkcji. Zapewnia unikalne awanse dla wszystkich jednostek walcz¹cych w zwarciu i redukuje nieco [ICON_STRENGTH] przestêpczoœæ.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Poza mo¿liwoœciami, jakie daj¹ zwyk³e koszary, ikanda zapewnia unikalny zestaw awansów dostêpnych dla wszystkich jednostek walcz¹cych wrêcz wyprodukowanych w mieœcie. Nale¿¹ do nich szybsze poruszanie siê i wiêksza si³a bojowa. Ikandê mog¹ zbudowaæ wy³¹cznie Zulusi.'
WHERE Tag = 'TXT_KEY_BUILDING_IKANDA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL 
SET Text = 'Specjalizuje siê w niszczeniu jednostek konnych i jest ogólnie silniejsza od wiêkszoœci jednostek ze swojej epoki. Mog¹ j¹ tworzyæ tylko Zulusi. Przed walk¹ przeprowadza przygotowawczy atak dystansowy.'
 WHERE Tag = 'TXT_KEY_UNIT_HELP_ZULU_IMPI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_pl_PL
SET Text = 'Koszt utrzymania jednostek walcz¹cych w zwarciu jest mniejszy o 50%, wszystkie jednostki bêd¹ potrzebowaæ o 25% doœwiadczenia mniej do nastêpnego awansu, a premia bojowa twojego [ICON_GREAT_GENERAL] wielkiego genera³a jest zwiêkszona o 10%.'
WHERE Tag = 'TXT_KEY_TRAIT_BUFFALO_HORNS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );