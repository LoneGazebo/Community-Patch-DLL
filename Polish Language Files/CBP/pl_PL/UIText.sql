--translated by mrlg
-- Difficulty Names

	UPDATE Language_pl_PL
	SET Text = 'Łatwizna. AI ma nieco utrudnień.'
	WHERE Tag = 'TXT_KEY_HANDICAP_SETTLER_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Standardowe wyzwanie. AI utrzymuje parę premii.'
	WHERE Tag = 'TXT_KEY_HANDICAP_CHIEFTAIN_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Nieco wymagający. AI otrzymuje zauważalne premie.'
	WHERE Tag = 'TXT_KEY_HANDICAP_WARLORD_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Dobry test umiejętności. AI ma wiele przewag.'
	WHERE Tag = 'TXT_KEY_HANDICAP_PRINCE_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Dosyć trudny. AI ma zasadnicze premie.'
	WHERE Tag = 'TXT_KEY_HANDICAP_KING_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Bardzo trudny. AI ma istotne premie.'
	WHERE Tag = 'TXT_KEY_HANDICAP_EMPEROR_HELP';

	UPDATE Language_pl_PL
	SET Text = 'Ekstremalnie wymagający. Gracze AI mają bardzo duże przewagi.'
	WHERE Tag = 'TXT_KEY_HANDICAP_IMMORTAL_HELP';

	UPDATE Language_pl_PL
	SET Text = 'AI ma ogromne premie na każdym etapie gry. Zostałeś ostrzeżony!'
	WHERE Tag = 'TXT_KEY_HANDICAP_DEITY_HELP';

-- Top Panel Tooltip Adjustment (make generic for more than just founder beliefs)
	UPDATE Language_pl_PL
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z religii.'
	WHERE Tag = 'TXT_KEY_TP_FAITH_FROM_RELIGION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Umożliwia umowy naukowe (jeśli są włączone)'
	WHERE Tag = 'TXT_KEY_ABLTY_R_PACT_STRING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Każde miasto dysponuje atrybutem „siła ostrzału”, równym jego całkowitej sile na początku walki i ma zasięg 1. Zasięg ten zwiększa się wraz z postępem gry w zależności od odkrytych technologii (szukaj ikony "ataku dystansowego" w drzewie technologii). Może w ciągu tury ostrzelać jedną jednostkę znajdującą się w zasięgu. Należy pamiętać, że siła rażenia jest stała i nie ulega zmniejszeniu nawet, jeżeli punkty PŻ miasta ulegną redukcji; pozostaje równa wyjściowej sile miasta do czasu jego zdobycia.'
	WHERE Tag = 'TXT_KEY_COMBAT_CITYFIRINGATTACKERS_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostki otrzymują PD za przetrwanie starcia (lub, w przypadku zwiadowców, z eksploracji), przy czym nie muszą one ani zwyciężyć, ani też zniszczyć wroga. PD narastają z każdą kolejną rundą, jaką zdoła przetrwać dana jednostka.[NEWLINE][NEWLINE]Liczba przyznanych PD zależy od okoliczności starcia. Ogólnie rzecz biorąc, jednostki otrzymują więcej doświadczenia za atak niż obronę. Ponadto premiowana jest również walka w zwarciu. Oto kilka przykładowych liczb (pełne zestawienie znajduje się w sekcji „Wykresy i tabele”):[NEWLINE]Jednostka atakująca w zwarciu: 5 PD[NEWLINE]Jednostka broniąca się w zwarciu: 4 PD[NEWLINE]Jednostka atakująca z dystansu: 2 PD[NEWLINE]Jednostka broniąca się z dystansu: 2 PD[NEWLINE]Ograniczenia związane z barbarzyńcami: gdy jednostka zdobędzie 45 PD, nie otrzymuje PD za walkę z barbarzyńcami.'
	WHERE Tag = 'TXT_KEY_COMBAT_ACQUIRINGXP_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Kiedy cywilizacja pozna odpowiednie technologie (zobacz drzewo technologii) robotnicy będą mogli karczować lasy, dżunglę, osuszać mokradła. Raz usunięte elementy terenu znikają na zawsze.'
	WHERE Tag = 'TXT_KEY_WORKERS_CLEARINGLAND_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	UPDATE Language_pl_PL
	SET Text = 'Jeśli twa cywilizacja będzie niezadowolona, miasta zaczną produkować mniej żywności. Będą one produkowały jej wystarczająco dużo, aby wyżywić swoich mieszkańców, ale wzrost miast będzie spowolniony i spadną przychody państwowe, np. złota, wiary i nauki.'
	WHERE Tag = 'TXT_KEY_FOOD_UNHAPPINESS_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Resource Limit breached
	UPDATE Language_pl_PL
	SET Text = 'Obecnie zużywasz więcej {@1_Resource[2]} niż posiadasz! Wszystkie jednostki potrzebujące tych zasobów będą [COLOR_NEGATIVE_TEXT]niezdolne do leczenia się[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_OVER_RESOURCE_LIMIT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Text Changes for Spies in Cities
	UPDATE Language_pl_PL
	SET Text = 'Potencjał wskazuje, jak szybko można wykradać z miasta technologie. Im wyższa jego wartość, tym szybciej się to odbywa. Wartość bazowa [COLOR_POSITIVE_TEXT](skala od 1 do 10)[ENDCOLOR] obliczana jest na podstawie ogólnego dobrobytu i zadowolenia w mieście, zwłaszcza wytwarzanej przez niego nauki i złota. Mogą go obniżyć budynki obronne miasta, takie jak policja okręgowa i posterunek policji. Potencjał może wzrosnąć, gdy do wykradania technologii użyjesz szpiega o wysokim poziomie.[NEWLINE][NEWLINE]Kliknij, by posortować miasta według potencjału.'
	WHERE Tag = 'TXT_KEY_EO_POTENTIAL_SORT_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Potencjał {1_CityName[2]} wynosi {2_Num}.[NEWLINE][NEWLINE]Jeśli twoje miasta mają wysoki potencjał, możesz rozważyć chronienie ich. Są na to dwa sposoby. Możesz wysłać do własnych szpiegów, by działali jako kontrwywiadowcy, którzy wyłapują i eliminują wrogich szpiegów, nim zdążą oni cokolwiek ukraść. Możesz też spowolnić tempo kradzieży technologii, konstruując budynki takie jak policja okręgowa, posterunek policji czy Wielka Zapora.'
	WHERE Tag = 'TXT_KEY_EO_OWN_CITY_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_SpyRank} {2_SpyName} dokonuje kradzieży w {4_CityName[6]}.[NEWLINE]Obecny potencjał {5_CityName[2]} wynosi {6_Num}.[NEWLINE][NEWLINE]Potencjał wskazuje, jak bardzo miasto jest wrażliwe na szpiegostwo. Im wyższa jego wartość, tym bardziej jest wrażliwe. Wartość bazowa [COLOR_POSITIVE_TEXT](skala od 1 do 10)[ENDCOLOR] obliczana jest na podstawie ogólnego dobrobytu i zadowolenia w mieście, zwłaszcza wytwarzanej przez nie nauce i złocie. Mogą go obniżyć budynki obronne miasta, takie jak policja okręgowa i posterunek policji.'
	WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_SpyRank} {2_SpyName} nie może wykraść technologii z {3_CityName[2]}, może jednak kontynuować dezorganizację gracza poprzez akcje takie jak sabotaż.[NEWLINE][NEWLINE]Potencjał podstawowy {4_CityName[2]} wynosi {5_Num}.[NEWLINE][NEWLINE]Potencjał wskazuje, jak bardzo miasto jest wrażliwe na szpiegostwo. Im wyższa jego wartość, tym bardziej jest wrażliwe. Wartość bazowa [COLOR_POSITIVE_TEXT](skala od 1 do 10)[ENDCOLOR] obliczana jest na podstawie ogólnego dobrobytu i zadowolenia w mieście, zwłaszcza wytwarzanej przez nie nauce i złocie. Mogą go obniżyć budynki obronne miasta, takie jak policja okręgowa i posterunek policji.'
	WHERE Tag = 'TXT_KEY_EO_CITY_POTENTIAL_CANNOT_STEAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Zdaje się, że potencjał {1_CityName[2]} wynosi {2_Num}.[NEWLINE][NEWLINE]Potencjał wskazuje, jak bardzo miasto jest wrażliwe na szpiegostwo. Im wyższa jego wartość, tym bardziej jest wrażliwe. Wartość bazowa [COLOR_POSITIVE_TEXT](skala od 1 do 10)[ENDCOLOR] obliczana jest na podstawie ogólnego dobrobytu i zadowolenia w mieście, zwłaszcza wytwarzanej przez nie nauce i złocie. Mogą go obniżyć budynki obronne miasta, takie jak policja okręgowa i posterunek policji.'
	WHERE Tag = 'TXT_KEY_EO_CITY_ONCE_KNOWN_POTENTIAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_SpyRank} {2_SpyName} jest w mieście {3_CityName[6]}. Gdy {4_SpyRank[7]} {5_SpyName} jest w twoim mieście, będzie wykonywać operacje kontrwywiadowcze i zabijać szpiegów innych nacji próbujących wykraść twoje technologie albo wykonać zaawansowane akcje'
	WHERE Tag = 'TXT_KEY_CITY_SPY_YOUR_CITY_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_SpyRank} {2_SpyName} jest w {@3_CityName[6]}. Gdy {@4_SpyRank[7]} {5_SpyName} jest w mieście, ustanawia obserwację, będzie próbować wykraść jakąś technologię tej cywilizacji i wykonać zaawansowane akcja, jak kradzież złota. {6_SpyRank} {7_SpyName} poinformuje cię też o każdym spisku, który odkryje w trakcie swoich działań.'
	WHERE Tag = 'TXT_KEY_CITY_SPY_OTHER_CIV_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Lua for Cities
	UPDATE Language_pl_PL
	SET Text = 'Nie mamy żadnych miast na sprzedaż lub nie mamy podpisanej deklaracji przyjaźni.'
	WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Nie mają żadnych miast na sprzedaż lub nie mamy podpisanej deklaracji przyjaźni.'
	WHERE Tag = 'TXT_KEY_DIPLO_TO_TRADE_CITY_NO_THEM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );
	
-- Public Opinion

	UPDATE Language_pl_PL
	SET Text = '{1_Num} z opinii publicznej (nacisk ideologiczny lub zmęczenie wojną).'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_PUBLIC_OPINION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Building Purchased
	UPDATE Language_pl_PL
	SET Text = 'Gdy masz wystarczająco dużo [ICON_GOLD] złota lub [ICON_PEACE] wiary, możesz kupić jednostkę lub budynek.'
	WHERE Tag = 'TXT_KEY_CITYVIEW_PURCHASE_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Możesz zmienić rozkazy dotyczące budowy z poziomu ekranu miasta. Możesz wydać złoto, aby kupić jednostkę (albo zainwestować w budynek). Szczegóły znajdziesz w rozdziale poświęconym ekranowi miasta.'
	WHERE Tag = 'TXT_KEY_BUILDINGS_PURCHASING_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Z poziomu ekranu miasta możesz wydawać złoto na zakup jednostek lub zainwestować je w budynki, aby zmniejszyć koszt ich wzniesienia. Metoda ta jest przydatna, gdy bardzo ci się spieszy: na przykład kiedy potrzebujesz jednostek do obrony przed najeźdźcą.'
	WHERE Tag = 'TXT_KEY_CITIES_PURCHASINGITEM_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Możesz wydawać złoto na zakup jednostek lub zainwestować je w budynki w mieście. Kliknij na jednostkę (jeśli cię na to stać!) a zostanie ona natychmiast wytrenowana w mieście. Z twojego skarbca zostanie odliczona odpowiednia kwota. Jeśli klikniesz na budynek, zainwestujesz w niego, obniżając koszt jego produkcji o 50%.[NEWLINE][NEWLINE]Należy pamiętać, że nie można kupić projektów takich jak „Projekt Utopia” i „Projekt Manhattan” itp.'
	WHERE Tag = 'TXT_KEY_GOLD_PURCHASEUNITS_HEADING3_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Happiness from Policies to Culture/Science change
	UPDATE Language_pl_PL
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z ustrojów.'
	WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] z ustrojów.'
	WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] ze scholastyki.'
	WHERE Tag = 'TXT_KEY_TP_SCIENCE_FROM_MINORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[ICON_RES_ARTIFACTS] Pozostałe ślady: {1_Num}'
	WHERE Tag = 'TXT_KEY_ANTIQUITY_SITES_TO_EXCAVATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '[ICON_RES_HIDDEN_ARTIFACTS] Pozostałe ukryte ślady: {1_Num}'
	WHERE Tag = 'TXT_KEY_HIDDEN_SITES_TO_EXCAVATE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- Update text for diplomat.
	UPDATE Language_pl_PL
	SET Text = '{1_Num}% premia z dyplomaty[NEWLINE]'
	WHERE Tag = 'TXT_KEY_CO_PLAYER_TOURISM_PROPAGANDA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Update text for tooltips based on what you changed above.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[NEWLINE][NEWLINE][COLOR_CYAN]Korzyści z wpływów:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Szlaki handlowe do tej cywilizacji generują:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+3%[ENDCOLOR] [ICON_FOOD] do wzrostu w wyjściowym mieście[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_GOLD] złota[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+1[ENDCOLOR] [ICON_RESEARCH] nauki (jeśli już pozyskuje [ICON_RESEARCH] naukę)[NEWLINE][COLOR_POSITIVE_TEXT]Premia do szpiegostwa przeciw tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Szpiedzy ustanawiają obserwację w [COLOR_POSITIVE_TEXT]5[ENDCOLOR] tur[NEWLINE][COLOR_POSITIVE_TEXT]Premia do podbijania miast tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_RESISTANCE] czasu niepokojów[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-15%[ENDCOLOR] [ICON_CITIZEN] strat obywateli podczas podbijania miast' , 'TXT_KEY_CO_INFLUENCE_BONUSES_EXOTIC'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Update texts for tooltips based on what you changed above.
	UPDATE Language_pl_PL
	SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Korzyści z wpływów:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Szlaki handlowe do tej cywilizacji generują:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6%[ENDCOLOR] [ICON_FOOD] do wzrostu w wyjściowym mieście[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_GOLD] złota[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+2[ENDCOLOR] [ICON_RESEARCH] nauki (jeśli już pozyskuje [ICON_RESEARCH] naukę)[NEWLINE][COLOR_POSITIVE_TEXT]Premia do szpiegostwa przeciw tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Szpiedzy ustanawiają obserwację w [COLOR_POSITIVE_TEXT]4[ENDCOLOR] tur[NEWLINE][COLOR_POSITIVE_TEXT]Premia do podbijania miast tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_RESISTANCE] czasu niepokojów[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-30%[ENDCOLOR] [ICON_CITIZEN] strat obywateli podczas podbijania miast'
	WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_FAMILIAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Update texts for tooltips based on what you changed above.
	UPDATE Language_pl_PL
	SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Korzyści z wpływów:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Szlaki handlowe do tej cywilizacji generują:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+9%[ENDCOLOR] [ICON_FOOD] do wzrostu w wyjściowym mieście[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+6[ENDCOLOR] [ICON_GOLD] złota[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+3[ENDCOLOR] [ICON_RESEARCH] nauki (jeśli już pozyskuje [ICON_RESEARCH] naukę)[NEWLINE][COLOR_POSITIVE_TEXT]Premia do szpiegostwa przeciw tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Szpiedzy ustanawiają obserwację w [COLOR_POSITIVE_TEXT]3[ENDCOLOR] tur[NEWLINE][COLOR_POSITIVE_TEXT]Premia do podbijania miast tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_RESISTANCE] czasu niepokojów[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-55%[ENDCOLOR] [ICON_CITIZEN] strat obywateli podczas podbijania miast'
	WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_POPULAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Update texts for tooltips based on what you changed above.
	UPDATE Language_pl_PL
	SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Korzyści z wpływów:[ENDCOLOR][NEWLINE][COLOR_POSITIVE_TEXT]Szlaki handlowe do tej cywilizacji generują:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+12%[ENDCOLOR] [ICON_FOOD] do wzrostu w wyjściowym mieście[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+8[ENDCOLOR] [ICON_GOLD] złota[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+4[ENDCOLOR] [ICON_RESEARCH] nauki (jeśli już pozyskuje [ICON_RESEARCH] naukę)[NEWLINE][COLOR_POSITIVE_TEXT]Premia do szpiegostwa przeciw tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Szpiedzy ustanawiają obserwację w [COLOR_POSITIVE_TEXT]2[ENDCOLOR] tury[NEWLINE][COLOR_POSITIVE_TEXT]Premia do podbijania miast tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_RESISTANCE] czasu niepokojów[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-80%[ENDCOLOR] [ICON_CITIZEN] strat obywateli podczas podbijania miast'
	WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_INFLUENTIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Update texts for tooltips based on what you changed above.
	UPDATE Language_pl_PL
	SET Text = '[NEWLINE][NEWLINE][COLOR_CYAN]Korzyści z wpływów:[ENDCOLOR][NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Szlaki handlowe do tej cywilizacji generują:[ENDCOLOR][NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+15%[ENDCOLOR] [ICON_FOOD] do wzrostu w wyjściowym mieście[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+10[ENDCOLOR] [ICON_GOLD] złota[NEWLINE][ICON_BULLET]  [COLOR_POSITIVE_TEXT]+5[ENDCOLOR] [ICON_RESEARCH] nauki (jeśli już pozyskuje [ICON_RESEARCH] naukę)[NEWLINE][COLOR_POSITIVE_TEXT]Premia do szpiegostwa przeciw tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [ICON_SPY] Szpiedzy ustanawiają obserwację w [COLOR_POSITIVE_TEXT]1[ENDCOLOR] turę[NEWLINE][COLOR_POSITIVE_TEXT]Premia do podbijania miast tej cywilizacji:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_RESISTANCE] czasu niepokojów[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]-100%[ENDCOLOR] [ICON_CITIZEN] strat obywateli podczas podbijania miast'
	WHERE Tag = 'TXT_KEY_CO_INFLUENCE_BONUSES_DOMINANT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Text for trade view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '+{2_Num} [ICON_GOLD] złota z twoich wpływów kulturalnych na {1_CivName[4]}', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_GOLD_EXPLAINED'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Text for trade view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '+{2_Num} [ICON_GOLD] złota z wpływów kulturalnych {1_CivName[2]} na ciebie', 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_GOLD_EXPLAINED'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	-- Text changes for Vote Purchase
	UPDATE Language_pl_PL
	SET Text = 'Podczas następnej sesji Kongresu Światowego ten przywódca wykorzysta swą delegację, liczącą [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?przedstawiciela; other?przedstawicieli;}, by poprzeć opcję „{2_ChoiceText}” odnośnie propozycji [COLOR_POSITIVE_TEXT]wprowadzenia[ENDCOLOR] następujących zmian:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]Liczba powyższych delegatów zależy od liczby delegatów kontrolowanych przez tego gracza i poziomu twojego [ICON_DIPLOMAT] dyplomaty. Wyższe poziomy pozwalają ci na wykupywanie większej ilości delegatów.'
	WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_ENACT_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Podczas następnej sesji Kongresu Światowego ten przywódca wykorzysta swą delegację, liczącą [COLOR_POSITIVE_TEXT]{1_NumVotes}[ENDCOLOR] {1_NumVotes: plural 1?przedstawiciela; other?przedstawicieli;}, by poprzeć opcję „{2_ChoiceText}” odnośnie propozycji [COLOR_WARNING_TEXT]unieważnienia[ENDCOLOR] następujących efektów:[NEWLINE][NEWLINE]{3_ProposalText}[NEWLINE][NEWLINE]Liczba powyższych delegatów zależy od liczby delegatów kontrolowanych przez tego gracza i poziomu twojego [ICON_DIPLOMAT] dyplomaty. Wyższe poziomy pozwalają ci wykupywanie większej ilości delegatów.'
	WHERE Tag = 'TXT_KEY_DIPLO_VOTE_TRADE_REPEAL_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'W najbliższym miejscu na wielkie dzieło sztuki zostanie umieszczony artefakt. Zapewnia on +1 do [ICON_CULTURE] kultury i +2 do [ICON_TOURISM] turystyki. Archeolodzy zostaną pochłonięci.'
	WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_ARTIFACT_RESULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Starożytne pisma zostaną umieszczone w miejscu na wielkie dzieło literackie. Zapewnią +1 [ICON_CULTURE] kultury i +2 [ICON_TOURISM] turystyki. Archeolodzy zostaną pochłonięci.'
	WHERE Tag = 'TXT_KEY_CHOOSE_ARCH_WRITTEN_ARTIFACT_RESULT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_TOURISM_CHANGES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Barbarzyńskie obozowisko'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_ENCAMPMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Barbarzyńskie obozowisko|Barbarzyńskiego obozowiska|Barbarzyńskiemu obozowisku|Barbarzyńskie obozowisko|Barbarzyńskie obozowiska|Barbarzyńskich obozowisk|Barbarzyńskim obozowiskom|Barbarzyńskie obozowiska'
	WHERE Tag = 'TXT_KEY_IMPROVEMENT_ENCAMPMENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Barbarzyńskie obozowisko jest tymczasową osadą rozszalałych barbarzyńców chcących podbić i zniszczyć twoją cywilizację. Te wieczne ciernie na obrzeżach cywilizacji są zdolne do wypluwania niekończącego się strumienia wściekłych barbarzyńców, którzy nieustannie nękają twoje miasta, plądrują twoje ziemie i spoglądają lubieżnie na twoje kobiety. Obozowiska w pobliżu twoich miast powinny być bez wątpliwości niszczone. Te niedaleko miast wroga warto jest zostawić w spokoju, by osłabiły rywala. Oczywiście o ile twoja moralność na to pozwala...'
	WHERE Tag = 'TXT_KEY_IMPROVEMENT_BARBARIAN_CAMP_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	-- CS Protection Tooltip for new influence perk
	UPDATE Language_pl_PL
	SET Text = 'Roztaczając ochronę nad państwem-miastem dajesz innym władcom do zrozumienia, że jest ono pod twoją opieką i zainterweniujesz w razie agresji lub próby wymuszenia daniny. Państwo-miasto będzie od ciebie oczekiwało wzięcia jego strony, kiedy będzie atakowane lub zastraszane.[NEWLINE][NEWLINE]Gdy ochraniasz państwo-miasto, wasze wyjściowe stosunki kształtują się na poziomie {1_InfluenceMinimum} i będziesz otrzymywać [COLOR_POSITIVE_TEXT]{3_InfluenceBoost}%[ENDCOLOR] [ICON_INFLUENCE] wpływów za ukończone dla tego miasta zadania. Bazowa [ICON_STRENGTH] obronność [ICON_CAPITAL] stolicy państwa-miasta zostanie zwiększona o [COLOR_POSITIVE_TEXT]{4_DefenseBoost}%[ENDCOLOR], do globalnego maksimum wynoszącego [COLOR_POSITIVE_TEXT]{5_DefenseTotal}%[ENDCOLOR].[NEWLINE][NEWLINE]Otaczając takie miasto ochroną, nie możesz cofnąć danego słowa przez {2_TurnsMinimum} {2_TurnsMinimum: plural 1?turę; 2?tury; 3?tur;}.'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );
	
	
	UPDATE Language_pl_PL
	SET Text = '[NEWLINE][NEWLINE][COLOR_WARNING_TEXT]Musisz być w stanie pokoju i mieć co najmniej {1_InfluenceNeededToPledge} punktów [ICON_INFLUENCE] wpływu, aby złożyć państwu-miastu obietnicę ochrony i być w czołowych 60% (pod względem militarnym) głównych cywilizacji.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_PLEDGE_DISABLED_INFLUENCE_TT';

	UPDATE Language_pl_PL
	SET Text = 'Zbyt wysokie [ICON_INFLUENCE] wpływy'
	WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_LOW_INFLUENCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_MINORS' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = '[COLOR_NEGATIVE_TEXT]Opad radioaktywny zadaje 15 obrażeń jednostek, które kończą swoją turę na polu z nim.[ENDCOLOR][NEWLINE][NEWLINE]Opad radioaktywny to pozostałości materiału radioaktywnego, występujące po eksplozji atomowej. Powstaje w wyniku unoszenia się w powietrzu cząsteczek radioaktywnych, które są wysoce szkodliwe dla roślin i zwierząt. Powodują one ich natychmiastową śmierć lub uszkodzenie DNA, co jest przyczyną raka, różnego rodzaju chorób oraz mutacji. Zależnie od typu wybuchu atomowego teren może zostać skażony na dekady, a nawet wieki. Przywrócenie stanu sprzed wybuchu wymaga usunięcia skażonych budynków, gleby oraz roślin.'
	WHERE Tag = 'TXT_KEY_FEATURE_FALLOUT_PEDIA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
	
	INSERT INTO Language_pl_PL (
	Tag, Text)
	SELECT 'TXT_KEY_BUILD_FARM_REC', 'Zwiększy otrzymywaną przez ciebie [ICON_FOOD] żywność z tego pola. Za każde dwie sąsiadujące ze sobą farmy, będzie generowane dodatkowe +1 [ICON_FOOD] żywności!'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Farmy można budować na prawie każdej ziemi, by poprawić ilość żywności na danym polu. Za każde dwie sąsiadujące ze sobą farmy, farmy będą generować dodatkową żywność.[NEWLINE][NEWLINE]Rolnictwo to jedno z najwcześniejszych i najważniejszych ludzkich zajęć. Pozwoliło ono ludzkości zaprzestać migracji i osiąść w jednym miejscu bez wyniszczania miejscowych surowców.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FARM_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	INSERT INTO Language_pl_PL (
	Tag, Text)
	SELECT 'TXT_KEY_BUILD_FARM_HELP', 'Otrzymujesz dodatkowe +1 [ICON_FOOD] żywności za każde 2 sąsiadujące ze sobą farmy.'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

	-- Trading Post - name change

	UPDATE Language_pl_PL
	SET Text = 'Wioski to mniejsze osady rozrzucone po wiejskich terenach cywilizacji, reprezentujące ludzi żyjących z uprawy ziemi i handlu. Mogą generować wiele bogactw dla społeczeństwa, zwłaszcza jeśli zostaną umieszczone na drogach, torach kolejowych i szlakach handlowych. [NEWLINE][NEWLINE]Zyskujesz [ICON_GOLD] złoto i [ICON_PRODUCTION] produkcję, jeśli są zbudowane na drodze lub torach, które łączą dwa posiadane przez ciebie miasta[NEWLINE][NEWLINE]Otrzymujesz dodatkowe [ICON_GOLD] złoto i [ICON_PRODUCTION] produkcję (+1 przed epoką przemysłową, +2 w epoce przemysłowej i później) jeśli szlak handlowy, międzynarodowy lub wewnętrzny, przechodzi przez tę wioskę.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADING_POST_TEXT';

	UPDATE Language_pl_PL
	SET Text = 'Wioska'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_TRADINGPOST';

	UPDATE Language_pl_PL
	SET Text = 'Wioska|Wioski|Wiosce|Wioskę|Wioski|Wiosek|Wioskom|Wioski', Gender = 'feminine'
	WHERE Tag = 'TXT_KEY_IMPROVEMENT_TRADING_POST';

	UPDATE Language_pl_PL
	SET Text = 'Budowa [LINK=IMPROVEMENT_TRADING_POST]wioski[\LINK]'
	WHERE Tag = 'TXT_KEY_BUILD_TRADING_POST';

	UPDATE Language_pl_PL
	SET Text = 'Czy wioski generują przychód w złocie?'
	WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_ADV_QUEST';

	UPDATE Language_pl_PL
	SET Text = 'Budowa wioski spowoduje zwiększenie ilości złota generowanej przez dane pole. Dostarcza dodatkowe złoto i produkcję, jeśli jest umieszczona na drodze lub torach kolejowych, które łączą dwa posiadane przez ciebie miasta oraz jeśli szlak handlowy, międzynarodowy lub wewnętrzny, przez nią przechodzi. Nie może zostać zbudowana obok drugiej.'
	WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_BODY';

	UPDATE Language_pl_PL
	SET Text = 'Wioska zwiększa wydajność pola o 3 złota i 1 kultury. Nie daje dostępu do zasobów. Generuje dodatkowe złoto i produkcję jeśli wybudowana na drodze lub szlaku handlowym.[NEWLINE]Wymagana technologia: waluta[NEWLINE]Czas budowy: 5 tur[NEWLINE]Można budować na: dowolne pole lądowe oprócz lodu. Nie może zostać zbudowana obok drugiej.'
	WHERE Tag = 'TXT_KEY_WORKERS_TRADINGPOST_HEADING3_BODY';

	UPDATE Language_pl_PL
	SET Text = 'Wioska'
	WHERE Tag = 'TXT_KEY_GOLD_TRADINGPOST_HEADING3_TITLE';

	UPDATE Language_pl_PL
	SET Text = 'W czasie gry tworzyć będziesz robotników – jednostki cywilne, które umożliwią ulepszenie terenów wokół twoich miast, zwiększenie produktywności lub umożliwią dostęp do pobliskich surowców. Wśród ulepszeń znajdują się farmy, punkty handlowe, tartaki, kamieniołomy, kopalnie i inne. W czasie wojny twój przeciwnik może splądrować (zniszczyć) twoje ulepszenia. Splądrowane ulepszenia nie będą działać, dopóki nie zostaną naprawione przez robotnika.'
	WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT';

	-- Fort
	UPDATE Language_pl_PL
	SET Text = 'Fort to szczególne ulepszenie, które nie poprawia wydajności pola, ani nie zapewnia dostępu do surowców. W zamian za to fort zwiększa premię do obrony pola o 50% dla wszystkich jednostek stacjonujących na tym polu. Nie daje jednak premii obronnej jednostkom na terytorium wroga. Nie może zostać zbudowany obok drugiego.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_FORT_TEXT';

	-- Landmark
	UPDATE Language_pl_PL
	SET Text = 'Słynne miejsce to dowolny wspaniały artefakt, budowla, dzieło sztuki lub cud natury, który zwabia w okolice gości. Słynnym miejscem jest Kolumna Nelsona w Londynie czy Mount Rushmore w Stanach Zjednoczonych. Jednak nie każdy odpowiednio duży obiekt to słynne miejsce. Największy na świecie kłębek wełny może nigdy nie osiągnąć tego statusu (mimo że, być może, warto go zobaczyć). Słynne miejsca zapewniają +1 do [ICON_CULTURE] kultury i [ICON_GOLD] złota za każdą epokę, która minęła od epoki powstania artefaktu.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_LANDMARK_TEXT';

	UPDATE Language_pl_PL
	SET Text = 'Znajdujesz {@1_Name[4]}!'
	WHERE Tag = 'TXT_KEY_FOUND_NATURAL_WONDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- Theming
	UPDATE Language_pl_PL
	SET Text = '{1_Num} [ICON_TOURISM] turystyki z premii z motywów'
	WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_THEMING_BONUSES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

	-- Happiness
	
	UPDATE Language_pl_PL
	SET Text = '[ICON_CITIZEN] Obywatele w nieokupowanych miastach nie wytwarzają bezpośrednio [ICON_HAPPINESS_3] niezadowolenia. Zobacz porady poniżej, aby ujrzeć źródła [ICON_HAPPINESS_3] niezadowolenia w każdym mieście'
	WHERE Tag = 'TXT_KEY_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Każde (niezajęte) miasto produkuje 0 [ICON_HAPPINESS_3] niezadowolenia'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Każde (niezajęte) miasto produkuje normalnie 0 [ICON_HAPPINESS_3] niezadowolenia'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Każde [ICON_OCCUPIED] zajęte miasto produkuje 0 [ICON_HAPPINESS_3] niezadowolenia'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Każdy [ICON_CITIZEN] obywatel w [ICON_OCCUPIED] zajętym mieście produkuje 1 [ICON_HAPPINESS_3] niezadowolenia'
	WHERE Tag = 'TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Sortuj według niezadowolenia'
	WHERE Tag = 'TXT_KEY_EO_SORT_STRENGTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Miasto zostanie [ICON_RAZING] spalone do cna w ciągu {1_Num} tur(y)! Podczas palenia, 50% [ICON_CITIZEN] populacji będzie generowało [ICON_HAPPINESS_3] niezadowolenie!'
	WHERE Tag = 'TXT_KEY_CITY_BURNING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Miasto będzie stawiać [ICON_RESISTANCE] opór jeszcze przez {1_Num} tur(y)! Opierające się miasto nie może robić niczego użytecznego i 50% [ICON_CITIZEN] populacji będzie generowało [ICON_HAPPINESS_3] niezadowolenie!'
	WHERE Tag = 'TXT_KEY_CITY_RESISTANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_Num} z [ICON_CITIZEN] populacji w [ICON_OCCUPIED] okupowanych lub stawiających [ICON_RESISTANCE] opór miastach.'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{1_Num : number "#,##"} z [ICON_CITIZEN] urbanizacji (specjalistów).'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_SPECIALISTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Tutorial Text

	UPDATE Language_pl_PL
	SET Text = 'System zadowolenia w Community Balance Patch jest kompletnie poprawiony i nie faworyzuje ani wysokiego ani szerokiego stylu gry. Zadowolenie jest teraz oparte na mechanice przyczyny i efektu, co sprawia, że twoje imperium zdaje się być bardziej żywe i dynamiczne niż kiedykolwiek wcześniej. [NEWLINE][NEWLINE]Poziom zadowolenia twojego społeczeństwa wyświetlany jest w lewym górnym rogu ekranu głównego. Miej na niego oko. Gdy spadnie do poziomu 0, twoja ludność zacznie okazywać pierwsze oznaki niezadowolenia. Gdy spadnie poniżej zera, twoja cywilizacja znajdzie się w nie lada tarapatach (przypadkiem, możesz uzyskać doskonały przegląd twojego zadowolenia najeżdżają kursorem na ten numer).[NEWLINE][NEWLINE]Ważna rzecz do zapamiętania – zadowolenie wygenerowane przez budynki w mieście i oparte na miastach ustroje nie może nigdy przekroczyć populacji tego miasta.[NEWLINE][NEWLINE]Dodatnia wartość zadowolenia zwiększa wartość twoich przychodów narodowych (np. całkowitej nauki na turę), podczas gdy niezadowolenie je obniża. Możesz sprawdzić swój obecny modyfikator poprzez najechanie kursorem na ikonę [ICON_HAPPINESS_1] w górnym panelu.'
	WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Następujące elementy zwiększają zadowolenie twego ludu::[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Surowce luksusowe[ENDCOLOR]: ulepszaj złoża znajdujące się na twoim terytorium lub kupuj je od innych cywilizacji. Każdy z nich zwiększa zadowolenie twego ludu. Dostarczają one zadowolenie jako mnożnik populacji całego imperium. Im większa populacja, tym więcej zadowolenia dostarczają twoje surowce luksusowe. Czyni to je mniej istotnymi na początku, jako że +1 zadowolenia to niezbyt dużo, ale nie karzą wysokich ani szerokich strategii, ponieważ skupiają się na całkowitej populacji, anie liczbie miast. W skrócie, surowce luksusowe "zyskują poziom" wraz ze wzrostem populacji twojego imperium.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Budynki[ENDCOLOR]: niektóre budynki zwiększają zadowolenie twego ludu, podczas gdy inne zmniejszają niezadowolenie z rzeczy jak [ICON_HAPPINESS_3] przestępczość i ubóstwo (przez co pośrednio zwiększają twoje zadowolenie). Zaliczają się do nich arena, cyrk, zoo i inne. Każdy z tych budynków zbudowany w twojej cywilizacji zwiększa ogólne zadowolenie (dwie areny dają go więc dwukrotnie więcej niż jedna - inaczej niż surowce).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Cuda[ENDCOLOR]:  niektóre cuda, jak Notre Dame czy Wiszące Ogrody dają dużą premię do zadowolenia.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ustroje społeczne[ENDCOLOR]: dostarczają dużo zadowolenie, ale różnymi sposobami.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Technologie[ENDCOLOR]: same w sobie nie zapewniają zadowolenia, odblokowują jednak budynki, cuda, surowce i ustroje, które to robią.'
	WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Na zadowolenie miasta nie wpływa dłużej bezpośrednio liczba jego mieszkańców. Zamiast tego, zadowolenie bazuje na zestawie funkcji, które sprawdzają zmienne gospodarcze. Ogólnie, populacja wciąż jest kluczem, jednak łatwiej jest sobie poradzić z niezadowoleniem "na miasto" (i dopasowuje się do pewnych stylów gry). Czyni to grę wysoką i szeroką równie wykonalną, dopóki twoje imperium jest dobrze zarządzane. Ponadto, obywatele mogą być niezadowoleni za jedną wartość w danym czasie, bazując na skali potrzeb. Na przykład, gdy obywatel jest niezadowolony z ubóstwa, nie może być niezadowolony z niczego innego, dopóki problem ubóstwa nie zostanie rozwiązany (przez to niezadowolenie generowane przez jedno miasto jest ograniczone przez populację miasta).[NEWLINE][NEWLINE]Jak to działa: funkcje oparte na uzysku sumują zyski twojego miasta i dzielą tę wartość przez jego populację. Ta wartość jest porównywana do współczynnika wartości na obywatela, stanowiącego globalną średnią zysków na populację, różnica zostaje zamieniona na niezadowolenie. Ta globalna średnia będzie stopniowo rosnąć wraz z postępem gry (wszystkie miasta stają się z czasem coraz bogatsze, przez co globalna średnia, którą definiuje ubóstwo zmienia się) i wzrasta (o ok. 2%) za każde miasto, które zbudujesz lub podbijesz.[NEWLINE][NEWLINE] Źródła niezadowolenia: [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Głód[ENDCOLOR]: to jedno może być fatalne, więc uważaj. Jeśli miasto głoduje, funkcja patrzy na wartość tego głodu (tj. ujemną wartość spożywanego jedzenia) i porównuje ją do wartości współczynnika funkcji[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Splądrowane pola[ENDCOLOR]: podobnie do połączeń miast, ta funkcja będzie generowała niezadowolenie zależne od wartości współczynnika na obywatela, który zależy od ilości splądrowanych pól. Im więcej splądrowanych pól, tym większe niezadowolenie.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ubóstwo[ENDCOLOR]: tu patrzy się na zyski w złocie. Jeśli nie przekraczają one kryteriów współczynnika, twoje miasto ma jakiś element ubóstwa. Jest to w dużej mierze nieuniknione we wszystkich miastach z wyjątkiem tych najbogatszych.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Izolacja[ENDCOLOR]: blokady lub brak połączeń miast będzie generował niezadowolenie bazujące na wartości współczynnika na obywatela. Powinno to stanowić problem tylko, jeśli oblężono cię z morza, twoje drogi zostały splądrowane lub twoje miasta nie są ze sobą połączone. Jeśli twoje miasta są połączone ze stolicą wewnętrznymi szlakami handlowymi (łańcuchowo lub pojedynczo), ta kara do niezadowolenia jest negowana (więc zablokowane lub oddalone miasta mogą być "połączone" z twoją stolicą przez handel).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przestępczość[ENDCOLOR]: ta funkcja patrzy na możliwości obronne twoich miast i ich garnizonów. Ten test bierze obronność jako "uzysk" (tj. obronność generowaną przez struktury obronne i garnizony) i odejmuje uszkodzenia miasta, jeśli takie są. Jeśli te uzyski nie przekraczają kryterium współczynnika, twoje miasto ma jakiś element przestępczości. Jest to nieuniknione w większych miastach czy podczas oblężenia miasta, zwłaszcza jeśli nie posiadasz garnizonu. Miasta, które są uszkodzone lub otoczone przez wrogów mają podwyższoną przestępczość.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Napięcia religijne[ENDCOLOR]: ta patrzy na większościową religię w mieście i porównuje ją z ilością mieszkańców, którzy nie są częścią tej religii. Jeśli ta wartość przekracza wartość współczynnika funkcji, będziesz miał pewne spory religijne. Jest to raczej nieuniknione w większości dużych miast, ale nigdy nie powinno być głównym źródłem niezadowolenia, Możesz obniżyć bardziej tę wartość przez budowę większej ilości budynków religijnych w tym mieście (wysoka produkcja wiary ma zdolność bycia odejmowaną od twojej mniejszościowej populacji), albo zwyczajnie nawracając ludzi na twoją jedną, prawdziwą religię.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Analfabetyzm[ENDCOLOR]: ta patrzy na wytwarzaną przez miasto naukę. Jeśli ten zysk nie przekracza kryterium współczynnika, twoje miasto ma jakiś element analfabetyzmu. Jest to raczej nieuniknione we wszystkich miastach, z wyjątkiem tych najmądrzejszych. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Znudzenie[ENDCOLOR]: ta patrzy na wytwarzaną przez miasto kulturę. Jeśli ten zysk nie przekracza kryterium współczynnika, twoje miasto ma jakiś element znudzenia (tj. twoi mieszkańcy nie mają rozrywek). Można to ominąć, budując areny i innego tego typu budynki lub poprzez wielkie dzieła. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Specjaliści[ENDCOLOR]: Specjaliści generują niezadowolenie opierając się na "płaskim" współczynniku opartym na ilości specjalistów w twoim imperium. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Zmęczenie wojną[ENDCOLOR]: kiedy już masz ideologię, istnieje możliwość zyskania dotyczącego całego imperium zmęczenia wojną (w dodatku do niepokojów ideologicznych).'
	WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSESUNHAPPINESS_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	--UPDATE Language_pl_PL JUZ JEST
	--SET Text = 'System zadowolenia w Community Balance Patch jest kompletnie poprawiony i nie faworyzuje ani wysokiego ani szerokiego stylu gry. Zadowolenie jest teraz oparte na mechanice przyczyny i efektu, co sprawia, że twoje imperium zdaje się być bardziej żywe i dynamiczne niż kiedykolwiek wcześniej. [NEWLINE][NEWLINE]Poziom zadowolenia twojego społeczeństwa wyświetlany jest w lewym górnym rogu ekranu głównego. Miej na niego oko. Gdy spadnie do poziomu 0, twoja ludność zacznie okazywać pierwsze oznaki niezadowolenia. Gdy spadnie poniżej zera, twoja cywilizacja znajdzie się w nie lada tarapatach (przypadkiem, możesz uzyskać doskonały przegląd twojego zadowolenia najeżdżają kursorem na ten numer).[NEWLINE][NEWLINE]Ważna rzecz do zapamiętania – zadowolenie wygenerowane przez budynki w mieście i oparte na miastach ustroje nie może nigdy przekroczyć populacji tego miasta.'
	--WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Są trzy poziomy niezadowolenia. Niezadowolenie, niepokój i rewolucja. W niezadowolonej cywilizacji przyrost naturalny we wszystkich miastach znacząco zwalnia i spadają przychody narodowe (np. całkowita nauka na turę). Ogarnięta niepokojem ma już o wiele więcej problemów, w tym kary do siły bojowej oraz jawne bunty, podczas gdy cywilizacja ogarnięta rewolucją może tracić miasta na rzecz innych imperiów.'
	WHERE Tag = 'TXT_KEY_HAPPINESS_LEVELSUNHAPPINESS_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET][ICON_HAPPINESS_1] Modyfikator zadowolenia: {1_Num}%', 'TXT_KEY_FOODMOD_HAPPY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );


	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET]Zmiana z powodu zadowolenia: {1_Num}%', 'TXT_KEY_PRODMOD_BALANCE_HAPPINESS_MOD'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 0 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET][ICON_HAPPINESS_1] Modyfikator zadowolenia: {1_Num}%', 'TXT_KEY_FOODMOD_HAPPY'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[NEWLINE][ICON_BULLET]Zmiana z powodu zadowolenia: {1_Num}%', 'TXT_KEY_PRODMOD_BALANCE_HAPPINESS_MOD'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	UPDATE Language_pl_PL
	SET Text = '[ICON_BULLET][COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z twojej [ICON_GOLDEN_AGE] złotej ery.'
	WHERE Tag = 'TXT_KEY_TP_CULTURE_FROM_GOLDEN_AGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Rebels!
	UPDATE Language_pl_PL
	SET Text = 'Ponieważ imperium ma co najmniej 20 punktów [ICON_HAPPINES_4] niezadowolenia, na naszym terytorium wybuchło powstanie!'
	WHERE Tag = 'TXT_KEY_NOTIFICATION_REBELS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

-- Update Text for Very Unhappy and Super Unhappy

-- Update text for top panel depending on which yields you have enabled above. Change as desired.
	UPDATE Language_pl_PL
	SET Text = 'Twoje imperium jest bardzo [ICON_HAPPINESS_3] niezadowolone![ENDCOLOR] [NEWLINE][NEWLINE]Z tego powodu, w całym imperium [ICON_CULTURE] kultura, [ICON_PEACE] wiara, [ICON_GOLD] złoto, [ICON_FOOD] wzrost i [ICON_RESEARCH] nauka zostają obniżone o [COLOR_NEGATIVE_TEXT] {1_num}% [ENDCOLOR] a skuteczność [ICON_STRENGTH] bojowa o [COLOR_NEGATIVE_TEXT] {1_Num}%[ENDCOLOR]!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_VERY_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Ponieważ [ICON_HAPPINESS_4] niezadowolenie osiągnęło 20 punktów, w imperium wybuchła rewolucja! Miasta mogą odłączyć się od twojego kraju i dołączyć do cywilizacji, która ma największy wpływ kulturalny na twój lud i mogą wybuchać powstania z pojawieniem się rebeliantów (barbarzyńców) na twoim terytorium!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_SUPER_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );
	
-- Update text for top panel depending on which yields you have enabled above. Change as desired.
	UPDATE Language_pl_PL
	SET Text = 'Twoje imperium jest bardzo [ICON_HAPPINESS_3] niezadowolone![ENDCOLOR] [NEWLINE][NEWLINE]Z tego powodu, w całym imperium [ICON_CULTURE] kultura, [ICON_PEACE] wiara, [ICON_GOLD] złoto, [ICON_FOOD] wzrost i [ICON_RESEARCH] nauka zostają obniżone o [COLOR_NEGATIVE_TEXT] {1_num}% [ENDCOLOR] a skuteczność [ICON_STRENGTH] bojowa o [COLOR_NEGATIVE_TEXT] {1_Num}%[ENDCOLOR]!'
	WHERE Tag = 'TXT_KEY_TP_EMPIRE_UNHAPPY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Całkowite [ICON_HAPPINESS_1] zadowolenie w imperium wynosi {1_Num}. Z tego powodu, w całym imperium [ICON_CULTURE] kultura, [ICON_PEACE] wiara, [ICON_GOLD] złoto, [ICON_FOOD] wzrost i [ICON_RESEARCH] nauka zostają zwiększone o [COLOR_NEGATIVE_TEXT] {2_num}%.'
	WHERE Tag = 'TXT_KEY_TP_TOTAL_HAPPINESS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- TOOLTIPS FOR TOP BAR

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '{1_Num} z powodu [ICON_HAPPINESS_1] zadowolenia.', 'TXT_KEY_TP_GOLD_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '{1_NUM} due to [ICON_HAPPINESS_3] Unhappiness.', 'TXT_KEY_TP_GOLD_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z powodu [ICON_HAPPINESS_1] zadowolenia.', 'TXT_KEY_TP_FAITH_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] z powodu [ICON_HAPPINESS_3] niezadowolenia.', 'TXT_KEY_TP_FAITH_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] [ICON_RESEARCH] z powodu [ICON_HAPPINESS_1] zadowolenia.', 'TXT_KEY_TP_SCIENCE_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );


-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_Num}[ENDCOLOR] [ICON_RESEARCH] z powodu [ICON_HAPPINESS_3] niezadowolenia.', 'TXT_KEY_TP_SCIENCE_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_NEGATIVE_TEXT]{1_NUM}[ENDCOLOR] z powodu [ICON_HAPPINESS_3] niezadowolenia.', 'TXT_KEY_TP_CULTURE_LOST_FROM_UNHAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

-- Text for city view tooltip.
	INSERT INTO Language_pl_PL (
	Text, Tag)
	SELECT '[ICON_BULLET] [COLOR_POSITIVE_TEXT]+{1_Num}[ENDCOLOR] z powodu [ICON_HAPPINESS_1] zadowolenia.', 'TXT_KEY_TP_CULTURE_GAINED_FROM_HAPPINESS'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_HAPPINESS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '{@1_CityName} nie może dłużej tworzyć {@2_BldgName[2]}. Utracona [ICON_PRODUCTION] zostaje zamieniona na {3_NumGold}[ICON_CULTURE]!'
	WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 2 );

	UPDATE Language_pl_PL
	SET Text = '{@1_CityName} nie może dłużej tworzyć {@2_BldgName[2]}. Utracona [ICON_PRODUCTION] zostaje zamieniona na {3_NumGold}[ICON_GOLDEN_AGE]!'
	WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 3 );

	UPDATE Language_pl_PL
	SET Text = '{@1_CityName} nie może dłużej tworzyć {@2_BldgName[2]}. Utracona [ICON_PRODUCTION] zostaje zamieniona na {3_NumGold}[ICON_RESEARCH]!'
	WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 4 );

	UPDATE Language_pl_PL
	SET Text = '{@1_CityName} nie może dłużej tworzyć {@2_BldgName[2]}. Utracona [ICON_PRODUCTION] zostaje zamieniona na {3_NumGold}[ICON_PEACE]!'
	WHERE Tag = 'TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_WONDER_CONSOLATION_TWEAK' AND Value= 5 );
