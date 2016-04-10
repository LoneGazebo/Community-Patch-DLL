--translated by mrlg
	
	-- Civilians
	
	-- Great Merchant Text

	UPDATE Language_pl_PL SET Text = 'Jeœli jednostka znajduje siê na terenie pañstwa-miasta, z którym nie prowadzisz wojny, rozkaz ten j¹ wykorzysta. [NEWLINE][NEWLINE]Uzyskasz du¿e iloœci [ICON_GOLD] z³ota i natychmiastowy "Dzieñ uwielbienia dla króla" we wszystkich miastach. Spowoduje to znikniêcie jednostki.' WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL SET Text = 'Wielki kupiec mo¿e zbudowaæ ulepszenie - izbê celn¹, która przynosi mnóstwo [ICON_GOLD] z³ota. Mo¿e tak¿e udaæ siê do pañstwa-miasta i przeprowadziæ „misjê handlow¹”, która daje góry [ICON_GOLD] z³ota, rozpoczyna "Dzieñ uwielbienia dla króla" i (jeœli nie u¿ywasz CSD) zapewnia wiele punktów wp³ywów w pañstwie-mieœcie. Wielki kupiec zostaje wykorzystany, gdy u¿yje siê go w jeden z opisanych sposobów.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Changed how Musician Great Tour Works
	UPDATE Language_pl_PL SET Text = 'Wartoœæ tej akcji zale¿y od twojego wp³ywu [ICON_TOURISM] kulturalnego na cywilizacjê docelow¹.[NEWLINE][NEWLINE][ICON_BULLET]Jeœli jest [COLOR_MAGENTA]nieznany[ENDCOLOR] lub [COLOR_MAGENTA]egzotyczny[ENDCOLOR]: twoja [ICON_TOURISM] turystyka w docelowej cywilizacji natychmiast osi¹ga status [COLOR_MAGENTA]znanej[ENDCOLOR] i otrzymujesz 2 [ICON_HAPPINESS_1] zadowolenia w twojej [ICON_CAPITAL] stolicy.[NEWLINE][NEWLINE][ICON_BULLET]Jeœli jest [COLOR_MAGENTA]znany[ENDCOLOR] lub [COLOR_MAGENTA]popularny[ENDCOLOR]: zyskujesz du¿¹ iloœæ [ICON_TOURISM] turystyki w zale¿noœæ od wytwarzania przez ciebie turystyki i kultury jak wp³yw kulturalny na tê cywilizacjê (20% tej [ICON_TOURISM] turystyki trafia do wszystkich pozosta³ych znanych cywilizacji). Dodatkowo, otrzymujesz [ICON_HAPPINESS_1] zadowolenie w twojej [ICON_CAPITAL] stolicy (skaluje siê do epoki), a docelowa cywilizacja doœwiadcza "Dnia uwielbienia dla króla" we wszystkich posiadanych miastach, w zale¿noœci od twojego wp³ywu kulturalnego i szybkoœci gry. Ta akcja poch³onie jednostkê.[NEWLINE][NEWLINE]Nie mo¿esz przeprowadziæ tej czynnoœci jeœli jesteœ w stanie wojny z docelow¹ cywilizacj¹, albo jeœli twój wp³yw [ICON_TOURISM] kulturalny na tê cywilizacjê jest [COLOR_MAGENTA]wp³ywowy[ENDCOLOR] lub wiêkszy.' WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL SET Text = 'Wielki muzyk mo¿e stworzyæ wielkie dzie³o muzyczne (generuje [ICON_CULTURE] kulturê i [ICON_TOURISM] turystykê), umieszczane w najbli¿szym mieœcie maj¹cym odpowiedni budynek z pustym miejscem (na przyk³ad opera lub wie¿a transmisyjna). Wielki muzyk mo¿e te¿ udaæ siê do innej cywilizacji z tras¹ koncertow¹, która da ci jednorazow¹ premiê do [ICON_TOURISM] turystyki na tym terytorium. Wielcy muzycy znikaj¹, jeœli u¿yje siê ich na jeden z tych sposobów.[NEWLINE][NEWLINE] Wielki muzyk mo¿e te¿ udaæ siê do innej cywilizacji z [COLOR_POSITIVE_TEXT]tras¹ koncertow¹[ENDCOLOR]. Wartoœæ tej akcji zale¿y od twojego wp³ywu [ICON_TOURISM] kulturalnego na cywilizacjê docelow¹.[NEWLINE][NEWLINE][ICON_BULLET]Jeœli jest [COLOR_MAGENTA]nieznany[ENDCOLOR] lub [COLOR_MAGENTA]egzotyczny[ENDCOLOR]: twoja [ICON_TOURISM] turystyka w docelowej cywilizacji natychmiast osi¹ga status [COLOR_MAGENTA]znanej[ENDCOLOR] i otrzymujesz 2 [ICON_HAPPINESS_1] zadowolenia w twojej [ICON_CAPITAL] stolicy.[NEWLINE][NEWLINE][ICON_BULLET]Jeœli jest [COLOR_MAGENTA]znany[ENDCOLOR] lub [COLOR_MAGENTA]popularny[ENDCOLOR]: zyskujesz du¿¹ iloœæ [ICON_TOURISM] turystyki w zale¿noœæ od wytwarzania przez ciebie turystyki i kultury jak wp³yw kulturalny na tê cywilizacjê (20% tej [ICON_TOURISM] turystyki trafia do wszystkich pozosta³ych znanych cywilizacji). Dodatkowo, otrzymujesz [ICON_HAPPINESS_1] zadowolenie w twojej [ICON_CAPITAL] stolicy (skaluje siê do epoki), a docelowa cywilizacja doœwiadcza "Dnia uwielbienia dla króla" we wszystkich posiadanych miastach, w zale¿noœci od twojego wp³ywu kulturalnego i szybkoœci gry.[NEWLINE][NEWLINE]Wielcy muzycy znikaj¹, jeœli u¿yje siê ich na jeden z tych sposobów. Nie mo¿esz przeprowadziæ tej czynnoœci jeœli jesteœ w stanie wojny z docelow¹ cywilizacj¹, albo jeœli twój wp³yw [ICON_TOURISM] kulturalny na tê cywilizacjê jest [COLOR_MAGENTA]wp³ywowy[ENDCOLOR] lub wiêkszy.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_MUSICIAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_pl_PL SET Text = 'Wielki admira³ mo¿e natychmiast przekraczaæ za darmo ocean, co czyni go przydatn¹ (ale wra¿liw¹) jednostk¹ eksploracyjn¹. Ponadto, gdy znajduje siê na twoim terytorium, mo¿e byæ wys³any na "ekspedycjê odkrywcz¹", co wykorzystuje jednostkê i daje ci dwie sztuki surowców luksusowych, które nie s¹ dostêpne na obecnej mapie.[NEWLINE][NEWLINE]Wielki admira³ mo¿e natychmiast zreperowaæ wszystkie jednostki morskie i zaokrêtowane znajduj¹ce siê na tym samym i przyleg³ych polach. Po wykonaniu tej akcji wielki admira³ zostaje wykorzystany. Ponadto zapewnia 15% premii do walki wszystkim jednostkom morskim bêd¹cym w posiadaniu gracza i znajduj¹cym siê w odleg³oœci 2 pól. Wielki admira³ NIE zostaje wykorzystany, gdy dostarcza tê premiê.' WHERE Tag = 'TXT_KEY_UNIT_GREAT_ADMIRAL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Archaeologist Text

	UPDATE Language_pl_PL SET Text = 'Archeolodzy to specjalni robotnicy wykorzystywani na wykopaliskach œladów przesz³oœci do stworzenia s³ynnych miejsc lub znalezienia artefaktów, umieszczanych w miejscach na wielkie dzie³a sztuki w muzeach, pa³acach, Ermita¿u i wybranych cudach. Archeolodzy mog¹ pracowaæ na terytorium dowolnego gracza i znikaj¹, gdy zakoñcz¹ wykopaliska archeologiczne. Archeologów nie mo¿na kupiæ za [ICON_GOLD] z³oto i mog¹ powstawaæ tylko w mieœcie ze szko³¹ publiczn¹.' WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHAEOLOGIST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Miasteczko to osiedle ludzkie wiêksze od wsi ale mniejsze od miasta. Definicja wielkoœci, od której zaczyna siê "miasto" ró¿ni siê znacz¹co w ró¿nych czêœciach œwiata. W nomenklaturze anglo-saskiej wystêpuje podzia³ na mniejsze "town" (pochodz¹ce od niemieckiego "Zaun", duñskiego "tuin" i staronordyckiego "tun") i wiêksze "city".[NEWLINE][NEWLINE]Miasteczka otrzymuj¹ +1 [ICON_GOLD] z³ota i [ICON_PRODUCTION] produkcji jeœli s¹ zbudowane na drodze, która ³¹czy dwa posiadane miasta oraz +2 [ICON_GOLD] i [ICON_PRODUCTION] produkcji jeœli na torach kolejowych. Otrzymujesz dodatkowe [ICON_GOLD] z³oto i [ICON_PRODUCTION] produkcjê (+1 z dróg, +2 z torów kolejowych) jeœli szlak handlowy, miêdzynarodowy lub wewnêtrzny, przechodzi przez to miasteczko.'
	WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CUSTOMS_HOUSE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Mo¿esz wykorzystaæ wielkiego kupca do stworzenia miasteczka. Kiedy jest wykorzystywane, miasteczko generuje du¿o z³ota i ¿ywnoœci na turê dla swojego miasta. [NEWLINE][NEWLINE]Miasteczko zyskuje dodatkowe [ICON_GOLD] z³oto i [ICON_PRODUCTION] produkcjê, jeœli jest zbudowane na drodze lub torach, które ³¹cz¹ dwa posiadane przez ciebie miasta[NEWLINE][NEWLINE]Otrzymujesz dodatkowe [ICON_GOLD] z³oto i [ICON_PRODUCTION] produkcjê (+1 przed epok¹ przemys³ow¹, +2 w epoce przemys³owej i póŸniej) jeœli szlak handlowy, miêdzynarodowy lub wewnêtrzny, przechodzi przez to miasteczko.'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Budowa miasteczka'
	WHERE Tag = 'TXT_KEY_BUILD_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Miasteczko|Miasteczka|Miasteczku|Miasteczko|Miasteczka|Miasteczek|Miasteczkom|Miasteczka', Gender = 'neuter'
	WHERE Tag = 'TXT_KEY_IMPROVEMENT_CUSTOMS_HOUSE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Wielki kupiec mo¿e zbudowaæ specjalne ulepszenie miasteczko, które, kiedy jest wykorzystywane, produkuje [ICON_GOLD] z³oto i [ICON_FOOD] ¿ywnoœæ. Mo¿e tak¿e udaæ siê do pañstwa-miasta i przeprowadziæ „misjê handlow¹” daj¹c¹ góry [ICON_GOLD] z³ota, rozpoczyna "Dzieñ uwielbienia dla króla" we wszystkich twoich miastach i (jeœli nie u¿ywasz CSD) wytwarza wiele punktów wp³ywów w pañstwie-mieœcie. Wielki kupiec zostaje wykorzystany, gdy u¿yje siê go w jeden z opisanych sposobów.[NEWLINE][NEWLINE]]Miasteczka otrzymuj¹ +1 [ICON_GOLD] z³ota i [ICON_PRODUCTION] produkcji jeœli s¹ zbudowane na drodze, która ³¹czy dwa posiadane miasta oraz +2 [ICON_GOLD] i [ICON_PRODUCTION] produkcji jeœli na torach kolejowych. Otrzymujesz dodatkowe [ICON_GOLD] z³oto i [ICON_PRODUCTION] produkcjê (+1 z dróg, +2 z torów kolejowych) jeœli szlak handlowy, miêdzynarodowy lub wewnêtrzny, przechodzi przez to miasteczko.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Co daje ulepszenie „miasteczko”?'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_ADV_QUEST' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Specjalne ulepszenie: miasteczko'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_CUSTOMSHOUSE_HEADING4_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Zyski specjalistów'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_HEADING1_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Zyski z ulepszeñ wielkich ludzi'
	WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_TITLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Wszyscy specjaliœci rozpoczynaj¹ grê z bazowym zestawem zysków. Zyski te zmieniaj¹ siê wraz z postêpem gry zale¿nie od technologii, ustrojów, budynków, wierzeñ.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Globalna premia do zysków (wszyscy specjaliœci):[ENDCOLOR][NEWLINE][NEWLINE]Cuda œwiata: [NEWLINE]    [COLOR_CYAN]Statua Wolnoœci[ENDCOLOR] - +1 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Miêdzynarodowa Stacja Kosmiczna[ENDCOLOR] - +1 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Empire State Building[ENDCOLOR] - +1 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Giorgio Armeier (korporacja)[ENDCOLOR] - +1 [ICON_CULTURE] kultury.[NEWLINE][NEWLINE]Wierzenia: [NEWLINE]    [COLOR_CYAN]Mistrzostwo[ENDCOLOR] - +2 do zysków bazowych.[NEWLINE][NEWLINE]Ustroje: [NEWLINE]    [COLOR_CYAN]Spo³ecznoœæ akademicka (Racjonalizm)[ENDCOLOR] - +2 [ICON_RESEARCH] nauki, +1 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Ikonografia (Pobo¿noœæ)[ENDCOLOR] - +1 [ICON_PEACE] wiary. [NEWLINE]    [COLOR_CYAN]Ukoñczenie Przemys³u[ENDCOLOR] - +1 [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Premie zale¿ne od specjalisty:[ENDCOLOR][NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]In¿ynier:[ENDCOLOR][NEWLINE]   Zyski bazowe: +3 [ICON_PRODUCTION] produkcji.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Mechanika[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Industrializacja[ENDCOLOR] - +2 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Technika rakietowa[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Lasery[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Kupiec:[ENDCOLOR][NEWLINE]   Zyski bazowe: +3 [ICON_GOLD] z³ota.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Waluta[ENDCOLOR] - +2 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Ekonomia[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Lot[ENDCOLOR] - +2 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Telekomunikacja[ENDCOLOR] - +2 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Naukowiec:[ENDCOLOR][NEWLINE]   Zyski bazowe: +3 [ICON_RESEARCH] nauki. [NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Filozofia[ENDCOLOR] - +2 [ICON_CULTURE] kultury. [NEWLINE]    [COLOR_CYAN]Astronomia[ENDCOLOR] - +2 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Teoria naukowa[ENDCOLOR] - +2 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Teoria atomu[ENDCOLOR] - +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Artysta:[ENDCOLOR][NEWLINE]   Zyski bazowe: +3 [ICON_CULTURE] kultury.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Architektura[ENDCOLOR] - +3 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Tworzywo sztuczne[ENDCOLOR] - +3 [ICON_CULTURE] kultury. [NEWLINE]    [COLOR_CYAN]Komputery[ENDCOLOR] - +3 [ICON_CULTURE] kultury.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Pisarz:[ENDCOLOR][NEWLINE]   Zyski bazowe: +3 [ICON_CULTURE] kultury.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Prasa drukarska[ENDCOLOR] - +3 [ICON_CULTURE] kultury. [NEWLINE]    [COLOR_CYAN]Nauki militarne[ENDCOLOR] - +3 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Internet[ENDCOLOR] - +3 [ICON_CULTURE] kultury.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Muzyk:[ENDCOLOR][NEWLINE]   Zyski bazowe: +4 [ICON_CULTURE] kultury.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Radio[ENDCOLOR] - +4 [ICON_CULTURE] kultury. [NEWLINE]    [COLOR_CYAN]Satelity[ENDCOLOR] - +4 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Urzêdnik (CSD):[ENDCOLOR][NEWLINE]   Zyski bazowe: +1 [ICON_CULTURE] kultury, [ICON_RESEARCH] nauki i [ICON_GOLD] z³ota.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Nawigacja[ENDCOLOR] - +2 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Czêœci zamienne[ENDCOLOR] - +2 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Internet[ENDCOLOR] - +2 [ICON_CULTURE] kultury.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Specjaliœci konsumuj¹ ró¿ne iloœci ¿ywnoœci, skaluje siê zale¿nie od epoki, w której jesteœ.[ENDCOLOR][NEWLINE][NEWLINE]    [COLOR_CYAN]Staro¿ytnoœæ-Œredniowiecze[ENDCOLOR]: specjaliœci konsumuj¹ 2 [ICON_FOOD] ¿ywnoœci.[NEWLINE]    [COLOR_CYAN]Renesans[ENDCOLOR]: specjaliœci konsumuj¹ 3 [ICON_FOOD] ¿ywnoœci.[NEWLINE]    [COLOR_CYAN]Przemys³owa[ENDCOLOR]: specjaliœci konsumuj¹ 4 [ICON_FOOD] ¿ywnoœci.[NEWLINE]    [COLOR_CYAN]Wspó³czesna[ENDCOLOR]: specjaliœci konsumuj¹ 5 [ICON_FOOD] ¿ywnoœci.[NEWLINE]    [COLOR_CYAN]Atomowa[ENDCOLOR]: specjaliœci konsumuj¹ 6 [ICON_FOOD] ¿ywnoœci.[NEWLINE]    [COLOR_CYAN]Informacji[ENDCOLOR]: specjaliœci konsumuj¹ 8 [ICON_FOOD] ¿ywnoœci.'
	WHERE Tag = 'TXT_KEY_SPECIALISTSANDGP_HEADING1_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Wszystkie ulepszenia wielkich ludzi rozpoczynaj¹ grê z bazowym zestawem zysków. Zyski te zmieniaj¹ siê wraz z postêpem gry zale¿nie od technologii, ustrojów, budynków, wierzeñ.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Globalna premia do zysków (wszystkie ulepszenia):[ENDCOLOR][NEWLINE][NEWLINE]Ustroje: [NEWLINE]    [COLOR_CYAN]Nowy ³ad[ENDCOLOR] - +4 do zysku bazowego ulepszenia (+2 na zysk, jeœli ulepszenie ma wiêcej ni¿ jeden zysk bazowy).[NEWLINE]Wierzenia: [NEWLINE]    [COLOR_CYAN]Wiedza przez oddanie[ENDCOLOR] - +3 [ICON_PEACE] wiary, +1 [ICON_CULTURE] kultury.[NEWLINE]Cechy: [NEWLINE]    [COLOR_CYAN]Uczeni z Jadeitowej Komnaty[ENDCOLOR] - +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Akademia (wielki naukowiec):[ENDCOLOR][NEWLINE]   Zysk bazowy: +6 [ICON_RESEARCH] nauki. [NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Fizyka[ENDCOLOR] - +3 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Teoria naukowa[ENDCOLOR] - +3 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Balistyka[ENDCOLOR] - +3 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Rozszczepianie j¹dra atomu[ENDCOLOR] - +3 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Miasteczko (wielki kupiec):[ENDCOLOR][NEWLINE]   Zysk bazowy: +2 [ICON_GOLD] z³ota, +2 [ICON_FOOD] ¿ywnoœci.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Bankowoœæ[ENDCOLOR] - +2 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Architektura[ENDCOLOR] - +2 [ICON_FOOD] ¿ywnoœci. [NEWLINE]    [COLOR_CYAN]Kolej[ENDCOLOR] - +2 [ICON_FOOD] ¿ywnoœci. [NEWLINE]    [COLOR_CYAN]Ch³odzenie[ENDCOLOR] - +2 [ICON_GOLD] z³ota.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Œwiête miejsce (wielki prorok):[ENDCOLOR][NEWLINE]   Zysk bazowy: +3 [ICON_PEACE] wiary, +1 [ICON_GOLD] z³ota.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Teologia[ENDCOLOR] - +2 [ICON_PEACE] wiary. [NEWLINE]    [COLOR_CYAN]Archeologia[ENDCOLOR] - +2 [ICON_PEACE] wiary, +1 [ICON_CULTURE] kultury.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Manufaktura (wielki in¿ynier):[ENDCOLOR][NEWLINE]   Zysk bazowy: +4 [ICON_PRODUCTION] produkcji.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Odlewanie metalu[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Nawóz[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji. [NEWLINE]    [COLOR_CYAN]Po³¹czone rodzaje wojsk[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Cytadela (wielki genera³):[ENDCOLOR][NEWLINE]   Zysk bazowy: +1 [ICON_RESEARCH] nauki, +1 [ICON_PRODUCTION] produkcji.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]Nauki militarne[ENDCOLOR] - +2 [ICON_PRODUCTION] produkcji [NEWLINE]    [COLOR_CYAN]Balistyka zaawansowana[ENDCOLOR] - +2 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Taktyki mobilne[ENDCOLOR] - +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Ambasada (wielki dyplomata, CSD):[ENDCOLOR][NEWLINE]   Zysk bazowy: +2 [ICON_GOLD] z³ota, +2 [ICON_CULTURE] kultury, +2 [ICON_RESEARCH] nauki.[NEWLINE]Technologie:[NEWLINE]    [COLOR_CYAN]S³u¿ba cywilna[ENDCOLOR] - +1 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Prasa drukarska[ENDCOLOR] - +1 [ICON_CULTURE] kultury. [NEWLINE]    [COLOR_CYAN]Nauki militarne[ENDCOLOR] - +1 [ICON_RESEARCH] nauki. [NEWLINE]    [COLOR_CYAN]Teoria atomu[ENDCOLOR] - +1 [ICON_RESEARCH] nauki, +1 [ICON_GOLD] z³ota. [NEWLINE]    [COLOR_CYAN]Telekomunikacja[ENDCOLOR] - +1 [ICON_CULTURE] kultury.'
	WHERE Tag = 'TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jak pokazuje historia, na œwiecie zawsze pojawiali siê ludzie, którzy potrafili w znacznym stopniu zmieniæ otaczaj¹c¹ ich rzeczywistoœæ - artyœci, naukowcy, genera³owie, kupcy i tym podobni - i których geniusz stanowi³ czynnik wyró¿niaj¹cy ich spoœród mas. W tej grze takie postacie zwane s¹ „wielkimi ludŸmi”.[NEWLINE][NEWLINE]Wielcy ludzie dziel¹ siê na piêæ typów: wielkich kupców, wielkich artystów, wielkich naukowców, wielkich in¿ynierów i wielkich genera³ów (szósty, wielki dyplomata, zostaje dodany przez CSD). Ka¿dy z typów posiada specjalne zdolnoœci.[NEWLINE][NEWLINE]Twoja cywilizacja pozyskuje wielkich ludzi poprzez stawianie odpowiednich budowli i cudów, a nastêpnie obsadzanie w nich specjalistów, obywateli twoich miast, którzy nie pracuj¹ na polach czy w kopalniach. Pomimo tego, ¿e specjaliœci nie pomagaj¹ przy zbiorach ¿ywnoœci czy przy produkcji w miastach, przyczyniaj¹ siê do szybszego pojawiania siê wielkich ludzi. Odpowiednie roz³o¿enie potrzeb pomiêdzy ¿ywnoœæ i tworzenie wielkich ludzi jest wa¿nym wyzwaniem przy zarz¹dzaniu miastem.[NEWLINE][NEWLINE]{TXT_KEY_SPECIALISTSANDGP_HEADING1_BODY}'
	WHERE Tag = 'TXT_KEY_PEDIA_SPEC_HELP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'W czasie gry tworzyæ bêdziesz robotników – jednostki cywilne, które umo¿liwi¹ ulepszenie terenów wokó³ twoich miast, zwiêkszenie produktywnoœci lub umo¿liwi¹ dostêp do pobliskich surowców. Wœród ulepszeñ znajduj¹ siê farmy, punkty handlowe, tartaki, kamienio³omy, kopalnie i inne. W czasie wojny twój przeciwnik mo¿e spl¹drowaæ (zniszczyæ) twoje ulepszenia. Spl¹drowane ulepszenia nie bêd¹ dzia³aæ, dopóki nie zostan¹ naprawione przez robotnika.[NEWLINE][NEWLINE]{TXT_KEY_BUILDINGS_SPECIALISTS_HEADING2_BODY}'
	WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENT_HELP_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka musi byæ na polu nale¿¹cym do innej g³ównej cywilizacji, z któr¹ nie jesteœ w stanie wojny.'
	WHERE Tag = 'TXT_KEY_MISSION_ONE_SHOT_TOURISM_DISABLED_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

	-- Air Units

	UPDATE Language_pl_PL
	SET Text = 'Bombowiec|Bombowca|Bombowcowi|Bombowiec'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Bombowiec to wczesna jednostka powietrzna. Skuteczna przeciwko jednostkom naziemnym, mniej groŸna w starciu z jednostkami morskimi. Musi uwa¿aæ na ataki wrogich samolotów. Mo¿e stacjonowaæ w ka¿dym posiadanym przez ciebie mieœcie lub na pok³adzie lotniskowca. Mo¿e poruszaæ siê z bazy do bazy, a tak¿e wykonywaæ misje w odleg³oœci do 4 pól. Bombowca u¿ywa siê do atakowania wrogich jednostek oraz miast. Jeœli to mo¿liwe, u¿yj najpierw trójp³atów lub myœliwców, aby „zu¿yæ” obronê powietrzn¹ wroga na tê turê. W zasadach dotycz¹cych jednostek powietrznych znajduje siê wiêcej informacji.'
	WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Myœliwce to jednostka powietrzna o œredniej sile. Mog¹ stacjonowaæ w twoim mieœcie lub na pok³adzie lotniskowca, a tak¿e przemieszczaæ siê miêdzy miastami (lub lotniskowcami) i wykonywaæ „misje” w promieniu 6 pól. Wykorzystuj myœliwce do zwalczania nieprzyjacielskich samolotów i jednostek l¹dowych, do rozpoznawania pozycji wroga i do obrony przed atakami nieprzyjacielskiego lotnictwa. Myœliwce s¹ szczególnie skuteczne przeciwko œmig³owcom wroga. Wiêcej informacji znajdziesz w zasadach dotycz¹cych lotnictwa.'
	WHERE Tag = 'TXT_KEY_UNIT_FIGHTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Myœliwce odrzutowe to potê¿ne jednostki lotnicze. Mog¹ stacjonowaæ w dowolnym z twoich miast lub na pok³adzie twojego lotniskowca. Potrafi¹ poruszaæ siê miêdzy bazami i wykonywaæ „misje” w promieniu 8 pól. Wykorzystuj myœliwce odrzutowe do atakowania nieprzyjacielskiego lotnictwa i jednostek l¹dowych, do rozpoznawania pozycji wroga i do obrony przed nalotami przeciwnika. S¹ wyj¹tkowo skuteczne w zwalczaniu œmig³owców wroga. Myœliwce odrzutowe maj¹ zdolnoœæ „rozpoznanie lotnicze”, co oznacza, ¿e na pocz¹tku tury widoczne jest wszystko w zasiêgu 6 pól od ich pozycji pocz¹tkowej. Wiêcej informacji znajdziesz w zasadach poœwiêconym lotnictwu.'
	WHERE Tag = 'TXT_KEY_UNIT_JET_FIGHTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Trójp³at to wczesna jednostka powietrzna. Mo¿e stacjonowaæ w ka¿dym posiadanym przez ciebie mieœcie lub na pok³adzie lotniskowca. Mo¿e poruszaæ siê z miasta do miasta (lub lotniskowca), mo¿e tak¿e wykonywaæ misje w odleg³oœci do 4 pól. U¿ywaj trójp³atów w celu atakowania wrogich jednostek powietrznych oraz naziemnych, zwiadów wrogich pozycji oraz obrony przed atakami powietrznymi. W zasadach dotycz¹cych jednostek powietrznych znajduje siê wiêcej informacji.'
	WHERE Tag = 'TXT_KEY_UNIT_TRIPLANE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Borrowed for WWI Bomber
	UPDATE Language_pl_PL
	SET Text = 'Ciê¿ki bombowiec|Ciê¿kiego bombowca|Ciê¿kiemu bombowcowi|Ciê¿ki bombowiec'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Ciê¿kie bombowce to jednostki powietrzne. S¹ skuteczne w walce przeciwko celom naziemnym a tak¿e, w mniejszym stopniu, morskim. Ich s³abym punktem jest du¿a podatnoœæ na ataki nieprzyjacielskich samolotów. Bombowce mog¹ stacjonowaæ w nale¿¹cym do gracza mieœcie lub na pok³adzie jego lotniskowca, a tak¿e przemieszczaæ siê od bazy do bazy i wykonywaæ misje o zasiêgu 6 pól. U¿ywaj ich do atakowania nieprzyjacielskich miast i jednostek. Je¿eli to mo¿liwe, najpierw wysy³aj myœliwce, by „zu¿yæ” wrogie œrodki obrony przeciwlotniczej w danej turze. W zasadach dotycz¹cych lotnictwa znajdziesz wiêcej informacji na ten temat.'
	WHERE Tag = 'TXT_KEY_UNIT_BOMBER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Rakieta kierowana to pocisk, który ulega zniszczeniu po trafieniu w cel. Mo¿na umieœciæ j¹ w kontrolowanym przez gracza mieœcie lub na pok³adzie jego okrêtu podwodnego lub kr¹¿ownika rakietowego. Mog¹ przemieszczaæ siê miêdzy bazami lub zaatakowaæ nieprzyjacielsk¹ jednostk¹ albo miasto w odleg³oœci do 6 pól. Wiêcej informacji znajdziesz w zasadach opisuj¹cych pociski rakietowe.'
	WHERE Tag = 'TXT_KEY_UNIT_GUIDED_MISSILE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Tania jednostka s³u¿¹ca do atakowania nieprzyjacielskich miast lub jednostek. Wymaga 1 [ICON_RES_OIL] ropy.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GUIDED_MISSILE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Melee

	-- Change Name of Great War Infantry/Bomber to be more 'generic'
	UPDATE Language_pl_PL
	SET Text = 'Strzelcy|Strzelców|Strzelcom|Strzelców', Gender = 'masculine', Plurality = '2'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Strzelcy to podstawowa jednostka bojowa ery wspó³czesnoœci. Jest znacznie silniejsza ni¿ jej poprzednik - fizylierzy.'
	WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Musketman

	UPDATE Language_pl_PL
	SET Text = 'Pierwsza dystansowa jednostka z broni¹ paln¹ w grze. Doœæ tania i silna.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_pl_PL
	SET Text = 'Arkebuzerzy to pierwsza jednostka z broni¹ prochow¹ w grze, zastêpuj¹ca starsze jednostki strzeleckie - kuszników, ³uczników, itp. Jest to jednak jednostka strzelaj¹ca i mo¿e atakowaæ przeciwnika znajduj¹cego siê w odleg³oœci nie wiêkszej ni¿ dwa heksy.'
	WHERE Tag = 'TXT_KEY_UNIT_MUSKETMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Borrowed for WWI Infantry - Fusiliers more fiting.
	
	UPDATE Language_pl_PL
	SET Text = 'Fizylierzy|Fizylierzy|Fizylierzy|Fizylierzy'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Fizylierzy to jednostka z broni¹ paln¹, która zastêpuje arkebuzerów. Jest od nich zdecydowanie potê¿niejsza, daj¹c wyraŸn¹ przewagê militarn¹ wobec cywilizacji, które ni¹ nie dysponuj¹.'
	WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Piechota jest znacznie silniejsza ni¿ jej poprzednicy - strzelcy. Wspó³czesna walka jest coraz bardziej z³o¿ona, w pojedynkê piechota stanowi ³atwy ³up dla lotnictwa, artylerii i czo³gów. Jeœli to mo¿liwe, powinna mieæ wsparcie artylerii, czo³gów i jednostek powietrznych (lub przeciwlotniczych).'
	WHERE Tag = 'TXT_KEY_UNIT_INFANTRY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Silna jednostka piechoty zdolna do dokonania zrzutu spadochronowego do 5 pól od w³asnego terytorium. Spadochroniarze mog¹ po skoku wykonaæ ruch i pl¹drowaæ, lecz walcz¹ dopiero w nastêpnej turze. Posiada premiê bojow¹ przeciwko jednostkom oblê¿niczym.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PARATROOPER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Si³y specjalne|Si³ specjalnych|Si³om specjalnym|Si³y specjalne', Gender = 'feminine'
	WHERE Tag = 'TXT_KEY_UNIT_MARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka epoki informacji, u¿yteczna przy okrêtowaniu oraz inwazjach zamorskich oraz pozbywania siê jednostek z broni¹ paln¹ (walcz¹cych wrêcz).  Mo¿e te¿ dokonaæ zrzutu za linie wroga.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MARINE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka si³ specjalnych posiada awanse, które ulepszaj¹ jej zasiêg widzenia oraz si³ê ataku, gdy jest zaokrêtowana na morzu. Jest równie¿ silniejszy przeciwko jednostkom z broni¹ paln¹ (walcz¹cym wrêcz) i mo¿e dokonania zrzutu spadochronowego do 9 pól od w³asnego terytorium.'
	WHERE Tag = 'TXT_KEY_UNIT_MARINE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Si³y specjalne, jednostki specjalne, komandosi to okreœlenia elitarnych formacji wojskowych lub policyjnych. Jednostki wojskowe przystosowane s¹ do wykonywania zadañ niekonwencjonalnych, prowadzenia dzia³añ specjalnych i nadzwyczaj trudnych w czasie pokoju, kryzysu oraz wojny: antyterrorystycznych, rozpoznawczych (g³ównie g³êbokie b¹dŸ rozpoznanie specjalne), dywersyjnych itp. Zwykle sk³adaj¹ siê z ma³ych wyselekcjonowanych grup ¿o³nierzy b¹dŸ funkcjonariuszy, œwietnie wyszkolonych i nadzwyczajnie wytrzyma³ych psychofizycznie, u¿ywaj¹cych wyspecjalizowanego uzbrojenia i wyposa¿enia. Dzia³aj¹ na l¹dzie, wodzie i z powietrza. Gwa³towny rozwój si³ specjalnych nast¹pi³ podczas II wojny œwiatowej. Obecnie jednostki tego typu wchodz¹ w sk³ad si³ zbrojnych lub policyjnych wiêkszoœci pañstw œwiata.'
	WHERE Tag = 'TXT_KEY_CIV5_MARINE_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = 'Te wyspecjalizowane jednostki artylerii automatycznie atakuj¹ jednostki powietrzne bombarduj¹ce cel w odleg³oœci 3 pól. (Mog¹ przechwyciæ tylko jedn¹ jednostkê na turê.) S¹ s³abe w walce z innymi oddzia³ami l¹dowymi i powinny byæ chronione przez silniejsze jednostki, jeœli zagra¿a im atak z ziemi.'
	WHERE Tag = 'TXT_KEY_UNIT_ANTI_AIRCRAFT_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Samobie¿ne jednostki SAM (ziemia-powietrze) zapewniaj¹ nacieraj¹cej armii obronê przeciwlotnicz¹. Samobie¿ne jednostki SAM mog¹ przechwytywaæ i zestrzeliwaæ nieprzyjacielskie samoloty bombarduj¹ce cele w promieniu 4 pól (ale tylko jedn¹ jednostkê na turê). Te jednostki s¹ podatne na ataki innych formacji ni¿ lotnictwo i powinny byæ chronione przez piechotê lub czo³gi.'
	WHERE Tag = 'TXT_KEY_UNIT_MOBILE_SAM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_pl_PL SET Text = 'Samochody pancerne|Samochodów pancernych|Samochodom pancernym|Samochody pancerne' WHERE Tag = 'TXT_KEY_UNIT_AT_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_pl_PL SET Text = 'Samochód pancerny to opancerzony pojazd mechaniczny, zwykle uzbrojony w armatê lub karabiny maszynowe, zazwyczaj umieszczone w wie¿y obrotowej. Podstawowym przeznaczeniem wiêkszoœci samochodów pancernych by³o rozpoznanie, rzadziej walka. Zazwyczaj pancerz samochodów pancernych chroni³ tylko przed pociskami karabinowymi i od³amkami. W zale¿noœci od masy i uzbrojenia czasami wyró¿nia siê lekkie, œrednie i ciê¿kie samochody pancerne. Samochodów pancernych nie nale¿y myliæ z ko³owymi transporterami opancerzonymi, których zadaniem jest przewo¿enie ¿o³nierzy.' WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_ANTITANKGUN_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_pl_PL SET Text = 'Samochód pancerny to wyspecjalizowana jednostka bojowa przeznaczona do akcji typu uderz i ucieknij. Zapewnij im wsparcie piechoty. czo³gów i artylerii, aby osi¹gn¹æ wiêksz¹ si³ê bojow¹.' WHERE Tag = 'TXT_KEY_UNIT_AT_GUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	UPDATE Language_pl_PL SET Text = 'Potê¿na strzelecka jednsotka wyspecjalizowana w akcjach typu uderz i ucieknij oraz walce zaczepnej.' WHERE Tag = 'TXT_KEY_UNIT_HELP_ANTI_TANK_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Naval Units

	INSERT INTO Language_pl_PL (Tag, Text, Gender, Plurality)
		SELECT 'TXT_KEY_DESC_CRUISER', 'Kr¹¿ownik|Kr¹¿ownika|Kr¹¿ownikowi|Kr¹¿ownik|Kr¹¿ownikiem|Kr¹¿owniku', 'masculine', '1'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_PEDIA', 'Kr¹¿ownik to rodzaj okrêtu wojennego. Termin jest w u¿yciu od paru setek lat i mia³ ró¿ne znaczenia w tym okresie. Podczas wieku ¿agli, s³owo "kr¹¿enie" odnosi³o s¹ do konkretnych rodzaju misji - niezale¿nego zwiadu, napadu na eskortê handlow¹ - wykonywanych przez fregaty lub slupy, które by³y we flotach w³aœnie kr¹¿ownikami. W po³owie XIX wieku, kr¹¿ownik sta³ siê klasyfikacj¹ dla statków przeznaczonych to tego rodzaju roli, chocia¿ kr¹¿owniki mia³y szerok¹ ró¿norodnoœæ rozmiarów, od ma³ych os³anianych kr¹¿owników po wielkie, uzbrojone (choæ nie tak potê¿ne) jak pancerniki. We wczesnych latach XX wieku, kr¹¿owniki by³y mniej wiêcej sta³ego rozmiaru okrêtami bojowymi, mniejsze od pancerników ale wiêksze od niszczycieli. W 1922r., Waszyngtoñski Traktat Morski ustanowi³ oficjalny limit dla iloœci kr¹¿owników, które by³y definiowane jako okrêty bojowe do 10000 ton wypornoœci z dzia³ami o kalibrze nie wiêkszym ni¿ 8 cali. Te limity ukszta³towa³y obraz kr¹¿owników do koñca II Wojny Œwiatowej. Najwiêksze kr¹¿owniki bojowe I Wojny Œwiatowej zosta³y sklasyfikowane, razem z pancernikami, jako du¿e okrêty wojenne (ang. capital ships).'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_STRATEGY', 'Zbuduj kr¹¿owniki, aby wesprzeæ swoje pancerniki i przej¹æ kontrolê nad morzami! Upewnij siê jednak, ¿e masz wystarczaj¹co du¿o ¿elaza.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CRUISER_HELP', 'Dystansowa jednostka morska ery przemys³owej przeznaczona do wspierania inwazji morskich i przynoszenia spustoszenia poœród drewnianych statków z poprzednich epok.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	UPDATE Language_pl_PL
	SET Text = 'G³ówna jednostka morska staro¿ytnoœci. U¿ywana do kontrolowania mórz w epoce klasycznej, jak równie¿ œredniowieczu, dziêki swej broni dystansowej.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_DROMON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Atakuj¹ zabójczym greckim ogniem, co sprawia, ¿e jest to pierwsza jednostka morska zdolna do ataku dystansowego. Nie mo¿e wp³ywaæ na pola g³êbokiego oceanu poza granicami miasta.'
	WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_DROMON_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Korweta to ma³y okrêt wojenny. Jest tradycyjnie najmniejsz¹ klas¹ jednostek uznawanych za w³aœciwie okrêty wojenne. Okrêt wojenny wy¿szej klasy to fregata. Klasê ni¿ej s¹ historycznie slupy wojenny. Wspó³czesne typy okrêtów klasy ni¿szej ni¿ korweta to jednostki ochrony nabrze¿a i szybkiego ataku. We wspó³czesnym rozumieniu korwety masa korwet wynosi od 500 do 2000 ton, chocia¿ ostatnie projekty mog¹ siêgaæ nawet 3000, przez co mo¿na ja uznawaæ za ma³e fregaty.'
	WHERE Tag = 'TXT_KEY_CIV5_PRIVATEER_TEXT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = 'Karawela to znacz¹ce zwiêkszenie potêgi morskiej cywilizacji. Jest jednostk¹ do walki z bliska, silniejsz¹ i szybsz¹ ni¿ przestarza³a trirema, mo¿e te¿ wyp³ywaæ na pola otwartego oceanu. Jest, jednak, wolna, co czyni j¹ podatn¹ na ataki dystansowe wroga. U¿yj j¹ do (stopniowego) odkrywania œwiata lub do obrony twoich miast.'
	WHERE Tag = 'TXT_KEY_UNIT_CARAVEL_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = 'Jednostka eksploracyjna póŸnego œredniowiecza, która mo¿e wyp³ywaæ na ocean, ale jest wolna. Bierze udzia³ w morskich bitwach z bliska.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARAVEL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka morska wyspecjalizowana w atakowaniu miast przybrze¿nych w celu zdobycia z³ota i w zajmowaniu wrogich statków. Dostêpna wczeœniej ni¿ korweta, któr¹ zastêpuje. Jedynie Holendrzy mog¹ j¹ wytworzyæ.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_SEA_BEGGAR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Korweta|Korwety|Korwecie|Korwetê', Gender = 'feminine'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Szybka jednostka morska specjalizuj¹ca siê w walce z bliska.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_PRIVATEER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka morska, która atakuje z bliska. Jest znacz¹co silniejsza i szybsza od karaweli.'
	WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Potê¿na jednostka morska, pancerniki dominuj¹ na oceanach w epoce przemys³owej.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_IRONCLAD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Staro¿ytna jednostka morska, u¿ywana do zdobycia kontroli na morzach.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Silna jednostka morska staro¿ytnoœci, dostêpna wczeœniej ni¿ trirema. U¿ywana, aby podbijaæ morza za pomoc¹ ataków z bliska na inne jednostki morskie oraz miasta. Otrzymuje darmowy awans [COLOR_POSITIVE_TEXT]Rekonesans[ENDCOLOR], pozwalaj¹cy jej otrzymywaæ doœwiadczenie za eksploracjê. Jedynie Kartagiñczycy mog¹ j¹ zbudowaæ.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARTHAGINIAN_QUINQUEREME' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Pocz¹tkowo transportuje 2 samoloty; pojemnoœæ mo¿na zwiêkszyæ poprzez awanse. Bêdzie przechwytywaæ wrogie jednostki powietrzne próbuj¹ce atakowaæ pobliskie jednostki.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_CARRIER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Lotniskowiec to wyspecjalizowany okrêt przenosz¹cy samoloty myœliwskie, bombowce (ale nie bombowce stealth) oraz bomby atomowe. Sam lotniskowiec nie mo¿e atakowaæ, ale samoloty, które przenosi, czyni¹ go najpotê¿niejsz¹ p³ywaj¹c¹ broni¹ ofensywn¹. Jednak lotniskowiec jest s³aby w obronie, dlatego powinny go eskortowaæ niszczyciele i okrêty podwodne. Lotniskowce s¹ te¿ wyposa¿one w broñ przeciwlotnicz¹ i automatycznie bêd¹ atakowaæ jednostki powietrzne bombarduj¹ce cele w promieniu 4 pól (mog¹ przechwyciæ tylko jedn¹ jednostkê na turê).'
	WHERE Tag = 'TXT_KEY_UNIT_CARRIER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka morska wyspecjalizowana w zwalczaniu lotnictwa wroga i polowaniu na okrêty podwodne. Mo¿e przenosiæ 3 pociski. Wymaga 1 [ICON_RES_IRON] ¿elaza.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MISSILE_CRUISER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Kr¹¿ownik rakietowy to nowoczesny okrêt wojenny. Jest szybki, wytrzyma³y, wyposa¿ony w pokaŸny arsena³ broni przeciwlotniczej i doskonale sobie radzi z przechwytywaniem wrogich samolotów. Co wa¿niejsze, kr¹¿ownik rakietowy mo¿e przenosiæ kierowane pociski rakietowe i pociski atomowe, dziêki czemu mo¿esz dotrzeæ z nimi tu¿ pod nieprzyjacielskie wybrze¿e. Kr¹¿owniki rakietowe po³¹czone z lotniskowcami, okrêtami podwodnymi i pancernikami to piekielnie potê¿na armada.'
	WHERE Tag = 'TXT_KEY_UNIT_MISSILE_CRUISER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );


	-- Mounted
	
	INSERT INTO Language_pl_PL (Tag, Text, Gender, Plurality)
		SELECT 'TXT_KEY_DESC_CUIRASSIER', 'Kirasjerzy|Kirasjerów|Kirasjerom|Kirasjerów', 'masculine', '2'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_PEDIA', 'Kirasjerzy (z fran. cuirassier – pancerny) to istniej¹cy w XVI-XX wieku, zachodnioeuropejski rodzaj ciê¿kiej jazdy w zbrojach z metalowych p³yt (kirysach lub zbrojach kirasjerskich). Kirasjerzy stanowili uprzywilejowan¹ formacjê i byli zwolnieni z wielu obowi¹zkowych elementów s³u¿by. Jazda tego typu powsta³a w drugiej po³owie XVI w. z przekszta³cenia w ni¹ ciê¿kozbrojnej jazdy kopijniczej, choæ nowo¿ytnych kirasjerów poprzedzali w staro¿ytnoœci macedoñscy hetairoi o zbli¿onym przeznaczeniu bojowym i rynsztunku, co ich spadkobiercy. Jednostka ta oznacza³a nowe stadium rozwoju europejskiej ciê¿kiej jazdy. Pancerz u¿ywany przez tych jeŸdŸców by³ znacznie l¿ejszy, zapewniaj¹c wiêksz¹ szybkoœæ i zwinnoœæ. Znaczn¹ uwagê poœwiêcano technikom jeŸdzieckim. Kirasjerzy potrafili zadawaæ potê¿ne, œmiercionoœne uderzenia zarówno nieprzyjacielskiej konnicy, jak i piechocie. W XVI wieku i w pocz¹tku XVII wieku kirasjerzy walczyli ogniem z konia, stosuj¹c tzw. karakol. Dopiero w wojnie trzydziestoletniej maj¹cy doœwiadczenie w polskiej armii Gottfried Heinrich von Pappenheim i wzoruj¹cy siê na nim Octavio Piccolomini u¿ywali kirasjerów do szar¿ cwa³em na bia³¹ broñ. Odt¹d kirasjerzy atakowali w zwartym szyku, galopem z odleg³oœci 1800-700 kroków. Do prze³amywania linii nieprzyjaciela u¿ywali pa³aszy, podczas gdy pistolety stanowi³y broñ rezerwow¹.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_STRATEGY', 'Kirasjerzy s¹ potê¿n¹ jednostk¹ epoki renesansu zdoln¹ do szybkiego przemieszczania siê i zabójczów ataków z dystansu. U¿ywaj ich do nêkania wroga i wspierania twoich wysi³ków wojennych.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_CUIRASSIER_HELP', 'Potê¿na strzelecka kawaleria z epoki renesansu, przydatna do nêkania wroga oraz akcji typu uderz i ucieknij.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text, Gender, Plurality)
		SELECT 'TXT_KEY_DESC_MOUNTED_BOWMAN', 'Ciê¿cy harcownicy|Ciê¿kich harcowników|Ciê¿kim harcownikom|Ciê¿kich harcowników', 'masculine', '2'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_PEDIA', 'Rycerze konni uzbrojeni w lance okazali siê byæ nieskutecznymi przeciwko formacjom pikinierów wspieranych przez kuszników, których broñ by³a w stanie przebiæ wiêkszoœæ rycerskich zbroi. Wynalezienie spustu i mechanizmów zapadkowych umo¿liwi³o u¿ywanie kusz podczas jazdy konnej. Rycerze i najemnicy walczyli w trójk¹tnych formacjach, gdzie najciê¿ej opancerzeni rycerze byli z przodu. Czêœæ z tych jeŸdŸców mia³a ze sob¹ w³asne lekkie, potê¿ne kusze wykonane w ca³oœci z metalu. Zosta³y one w koñcu zast¹pione w walce przez potê¿niejsze bronie czarnoprochowe, chocia¿ wczesne bronie palne mia³y wolniejsz¹ prêdkoœæ ognia i znacznie gorsz¹ celnoœæ ni¿ kusze z tego samego okresu. PóŸniej, w podobne mieszane formacje wchodzi³a arkebuzeria i muszkieterzy wraz z pikinierami (piki i strza³y), której szyki by³ dziurawione przez kawaleriê strzelaj¹c¹ z pistoletów lub karabinów.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_STRATEGY', 'Ciê¿cy harcownicy to potê¿na jednostka epoki œredniowiecza zdolna do szybkiego ruchu i zabójczych ataków z dystansu. U¿ywaj ich do nêkania wroga i wspierania twoich wysi³ków wojennych, ale uwa¿aj aby nie zostali z³apani w pojedynkê, poniewa¿ maj¹ ograniczone zdolnoœci obronne.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_MOUNTED_BOWMAN_HELP', 'Potê¿na kawaleria strzelecka z epoki œredniowiecza, przyrzynana do nêkania wroga oraz akcji typu uderz i ucieknij.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Ranged

	INSERT INTO Language_pl_PL (Tag, Text, Gender, Plurality)
		SELECT 'TXT_KEY_DESC_VOLLEY_GUN', 'Kartaczownice|Kartaczownic|Kartaczownicom|Kartaczownice|Kartaczownicami|Kartaczownicach', 'feminine', '2'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_PEDIA', 'Kartaczownica to dzia³o z wieloma lufami wystrzeliwuj¹ce seriê pocisków, albo jednoczeœnie albo w sekwencyjne. Ró¿ni siê ona od wspó³czesnych broni maszynowych brakiem automatycznego ³adowania i ognia oraz ograniczon¹ iloœci¹ pocisków za³adowanych jednoczeœnie.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_STRATEGY', 'Kartaczownica to potê¿na broñ dystansowa z renesansu, chocia¿ jest dosyæ wra¿liwa w walce z bliska. Trzymaj je z daleka od pierwszych linii.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	INSERT INTO Language_pl_PL (Tag, Text)
		SELECT 'TXT_KEY_CIV5_VOLLEY_GUN_HELP', 'Dystansowa l¹dowa jednostka strzelecka ery renesansu przeznaczona do wspierania twoich arkebuzerów.'
		WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka dystansowa œrodkowej czêœci gry, która os³abia pobliskie jednostki wroga.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_GATLINGGUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Karabiny Gatlinga to nieoblê¿nicza jednostka walcz¹ca z dystansu ze œrodkowej czêœci gdy, która mo¿e zasypaæ wroga przera¿aj¹cym gradem pocisków. S¹ znacznie silniejsze od wczeœniejszych jednostek dystansowych, jak kartaczownica, ale te¿ znacznie s³absze od innych jednostek wojskowych ze swojej epoki. Z tego powodu powinny byæ one wykorzystywane w wojnie z wyczerpania. Os³abiaj¹ okoliczne jednostki i otrzymuj¹ premiê do obrony. Podczas ataku, karabiny Gatlinga zadaj¹ mniej obra¿eñ jednostkom pancernym i ufortyfikowanym, jak równie¿ miastom. Umieœæ karabiny Gatlinga w swoich miastach lub w kluczowych punktach, aby jak najlepiej wykorzystaæ ich potencja³ obronny.'
	WHERE Tag = 'TXT_KEY_UNIT_GATLINGGUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	UPDATE Language_pl_PL
	SET Text = 'Jednostka dystansowa z koñcowej czêœci gry, która os³abia okoliczne wrogie jednostki.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_MACHINE_GUN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Karabiny maszynowe to przedostatnia nieoblê¿nicza jednostka walcz¹ca z dystansu, która mo¿e zasypaæ wroga przera¿aj¹cym gradem ognia zas³onowego. S¹ silniejsze od wczeœniejszych jednostek strzelaj¹cych, ale te¿ znacznie s³absze od innych jednostek wojskowych ze swojej epoki. Z tego powodu powinny byæ one wykorzystywane w wojnie z wyczerpania. Tak jak karabiny Gatlinga os³abiaj¹ pobliskie jednostki wroga. Podczas ataku, karabiny maszynowe zadaj¹ mniej obra¿eñ jednostkom pancernym i ufortyfikowanym, jak równie¿ miastom. S¹ bardzo s³abe w walce w zwarciu. Umieœæ w swoim mieœcie karabiny maszynowe, by lepiej go broniæ, albo wykorzystaj je do obrony kluczowych punktów'
	WHERE Tag = 'TXT_KEY_UNIT_MACHINE_GUN_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Jednostka epoki atomowej walcz¹ca z dystansu, która jest tym silniejsza im bli¿ej wroga siê znajduje i zadaje wielkie obra¿enia jednostkom pancernym.'
	WHERE Tag = 'TXT_KEY_UNIT_HELP_BAZOOKA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Bazooka to ostatnia nieoblê¿nicza jednostka walcz¹ca z dystansu i jest zdolna zadawaæ naprawdê ogromne iloœci obra¿eñ, zw³aszcza jednostkom pancernym. To najsilniejsza jednostka walcz¹ca na odleg³oœæ, ale te¿ znacznie s³absza od innych jednostek wojskowych ze swojej epoki. Z tego powodu powinna byæ ona wykorzystywane w wojnie z wyczerpania. Tak jak karabiny maszynowe os³abia pobliskie jednostki wroga. Podczas ataku, bazooka zadaje mniej obra¿eñ ufortyfikowanym jednostkom oraz miastom, ale zadaje dodatkowe obra¿enia jednostkom pancernym. Jest doskona³¹ jednostk¹ obronn¹.'
	WHERE Tag = 'TXT_KEY_UNIT_BAZOOKA_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

