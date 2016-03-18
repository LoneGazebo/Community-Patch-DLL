--translated by mrlg
--Tag: [sqlFILENAME]
--BuildingRules.sql
--DiploBuildingText.sql
--VanillaBuildingChanges.sql
--GameplayText.sql
--IdeologyChanges.sql

--[sqlFILENAME]BuildingRules.sql
    --Halicarnassus Fix
    UPDATE Language_pl_PL
    SET Text = 'Imperium wchodzi w [ICON_GOLDEN_AGE] złotą erę. Zyskujesz 20 sztuk [ICON_GOLD] złota za każdym razem, gdy poświęcisz wielką osobistość. Każde źródło [ICON_RES_MARBLE] marmuru lub [ICON_RES_STONE] kamienia w zasięgu tego miasta daje +2 sztuki [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_WONDER_MAUSOLEUM_HALICARNASSUS_HELP'
    AND EXISTS (SELECT * FROM CSD WHERE Type='ANCIENT_WONDERS' AND Value= 1 );

--[sqlFILENAME]DiploBuildingText.sql
    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_COURT_SCRIBE_HELP', '+1 [ICON_RES_PAPER] papier. +10% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Można wybudować tylko w stolicy. Koszt rośnie wraz ze wzrostem liczby miast w imperium. [NEWLINE][NEWLINE] 15% [ICON_PRODUCTION] produkcji miasta jest dodawane do obecnej [ICON_PRODUCTION] produkcji za każdym razem, gdy miasto zyskuje [ICON_CITIZEN] mieszkańca.';

    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_COURT_SCRIBE_STRATEGY', 'Ten cud narodowy jest unikatowy z tego względu, że gracze mogą go zbudować tylko w swojej stolicy, ale bez innych wymagań. Daje małą premię do produkcji jednostek dyplomatycznych w stolicy i jeden surowiec [ICON_RES_PAPER] papieru. Postaw ten budynek, jeśli chcesz zabezpieczyć swoje sojusze z jednym czy dwoma państwami-miastami w pierwszych epokach gry. Dodatkowa [ICON_PRODUCTION] produkcja dostarczana przy wzroście miasta, co czyni ten budynek ważnym do zmaksymalizowania rozwoju na wczesnych etapach.';

    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_CHANCERY_HELP', '+1 [ICON_RES_PAPER] papier. +10% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Zapewnia jednostkom dyplomatycznym z tego miasta awans "[COLOR_POSITIVE_TEXT]Sygnet królewski[ENDCOLOR]". [NEWLINE][NEWLINE] 15% [ICON_PRODUCTION] produkcji jest dodawane do obecnej [ICON_PRODUCTION] produkcji za każdym razem, gdy miasto zyskuje [ICON_CITIZEN] mieszkańca.';

    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_CHANCERY_STRATEGY', 'Zwiększa szybkość produkcji jednostek dyplomatycznych i zwiększa ilość wpływu jednostek dyplomatycznych budowanych w tym mieście. Specjaliści urzędnicy w tym mieście z tego budynku oferują trochę [ICON_CULTURE] kultury, [ICON_RESEARCH] nauki i [ICON_GOLD] złota w zamian za [ICON_FOOD] żywność i [ICON_PRODUCTION] produkcję. Dodatkowa [ICON_PRODUCTION] produkcja jest dostarczana przy wzroście miasta, co czyni ten budynek ważnym do wczesnego zmaksymalizowania rozwoju.';

    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_WIRE_SERVICE_STRATEGY', 'Zwiększa szybkość produkcji jednostek dyplomatycznych i zwiększa ilość wpływu jednostek dyplomatycznych budowanych w tym mieście. Specjaliści urzędnicy w tym mieście z tego budynku oferują trochę [ICON_CULTURE] kultury, [ICON_RESEARCH] nauki i [ICON_GOLD] złota w zamian za [ICON_FOOD] żywność i [ICON_PRODUCTION] produkcję. Dodatkowa [ICON_PRODUCTION] jest produkcja dostarczana przy wzroście miasta, co czyni ten budynek ważnym wczesnego do zmaksymalizowania rozwoju.';

    INSERT INTO Language_pl_PL (
      Tag, Text)
    SELECT 'TXT_KEY_BUILDING_WIRE_SERVICE_HELP', '+1 [ICON_RES_PAPER] papier. +10% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Zapewnia jednostkom dyplomatycznym z tego miasta awans "[COLOR_POSITIVE_TEXT]Agencja prasowa[ENDCOLOR]". [NEWLINE][NEWLINE] 15% [ICON_PRODUCTION] produkcji miasta jest dodawane do obecnej [ICON_PRODUCTION] produkcji za każdym razem, gdy miasto zyskuje [ICON_CITIZEN] mieszkańca.';

--[sqlFILENAME]VanillaBuildingChanges.sql
    UPDATE Language_pl_PL
    SET Text = 'Podwaja nacisk religijny tego miasta i zapewnia 1 głos na Kongresie Światowym za każde [COLOR_POSITIVE_TEXT]6 miast[ENDCOLOR], które wyznają twoją religię. [NEWLINE][NEWLINE]Musi powstać w świętym mieście. Trzeba zbudować świątynię we wszystkich miastach. Koszt rośnie wraz ze wzrostem liczby miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

--[sqlFILENAME]GameplayText.sql
    -- Gameplay Text

    UPDATE Language_pl_PL
    SET Text = 'Jak misje dyplomatyczne wpływają na stosunki?'
    WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_ADV_QUEST' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Jednym z najskuteczniejszych sposobów na zwiększenie swoich wpływów w państwie-mieście jest stworzenie jednostki dyplomatycznej i wysłanie jej na misję dyplomatyczną. Pozwala to zwiększyć bezpośrednie twoje wpływy w państwie-mieście. Aby przeprowadzić taką misję, wprowadź swoją jednostkę dyplomatyczną na terytorium państwa miasta i naciśnij przycisk "Misja dyplomatyczna". Podstawowa ilość wpływów zyskiwanych przez gracza zwiększa się wraz ze zdobywaniem nowych awansów, jednostek, cuda i budynki. [ENDLINE][ENDLINE]Gracze mogę mieć tylko organiczną liczbę jednostek dyplomatycznych w danym czasie. Maksymalna liczba aktywnych jednostek dyplomatycznych zwiększa się w zależności od ilości kontrolowanego przez gracza [ICON_RES_PAPER] papieru. Po wygenerowaniu wpływów, jednostki dyplomatyczne znikają i trzeba je zbudować na nowo.'
    WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Misje dyplomatyczne i ty'
    WHERE Tag = 'TXT_KEY_CITYSTATE_GIVEGOLD_HEADING3_TITLE' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Misje dyplomatyczne'
    WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '{1_CityStateName:textkey} złości się czymś, co zrobiła twoja cywilizacja. Być może naruszyła ostatnio ich granice lub wypowiedziała im wojnę.[NEWLINE][NEWLINE]Twoje [ICON_INFLUENCE] wpływy na nich odnawiają się z prędkością {2_Num} na turę. Powinieneś wysłać jednostkę dyplomatyczną, aby przyspieszyć odnawianie twoich [ICON_INFLUENCE] wpływów.'
    WHERE Tag = 'TXT_KEY_ANGRY_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '{1_CityStateName:textkey} {@1_CityStateName: plural 1?ma; 2?mają;} do ciebie ambiwalentny stosunek. Powinieneś wysłać jednostkę dyplomatyczną, aby zwiększyć twoje [ICON_INFLUENCE] wpływy.'
    WHERE Tag = 'TXT_KEY_NEUTRAL_CSTATE_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Przeprowadź misję dyplomatyczną'
    WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Jeśli jednostka znajduje się na terenie państwa-miasta, z którym nie prowadzisz wojny, rozkaz ten spowoduje jej zniknięcie.[NEWLINE][NEWLINE]Jeśli to wielki dyplomata, zyskasz dużą ilość [ICON_INFLUENCE] wpływów na państwo-miasto a [ICON_INFLUENCE] wpływy wszystkich innych głównych cywilizacji znanych temu państwu-miastu zostaną obniżone o tą samą wartość.[NEWLINE][NEWLINE]Jeśli to wielki kupiec, zyskasz dużą ilość [ICON_GOLD] złota i rozpocznie się "Dzień uwielbienia dla króla" we wszystkich twoich miastach. Spowoduje zniknięcie wielkiego człowieka.'
    WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Zyskujesz {1_Num} [ICON_GOLD] złota i {2_Num} [ICON_INFLUENCE] wpływów z misji dyplomatycznej!'
    WHERE Tag = 'TXT_KEY_MERCHANT_RESULT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Możesz rozpocząć jedną z wymienionych misji dyplomatycznych.'
    WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Nie możesz podarować [ICON_GOLD] złota państwu-miastu. Zamiast tego wyślij jednostkę dyplomatyczną.'
    WHERE Tag = 'TXT_KEY_POPUP_MINOR_GOLD_GIFT_CANT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Możesz rozpocząć misję dyplomatyczną, która zwiększy twoje [ICON_INFLUENCE] wpływy w tym państwie-mieście.'
    WHERE Tag = 'TXT_KEY_POP_CSTATE_GIFT_GOLD_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Gdy zostanie ukończony projekt Organizacji Narodów Zjednoczonych, Kongres Światowy zamieni się w Organizację Narodów Zjednoczonych. Po tym, i aktywowaniu ideologii światowej, stanie się możliwe odniesienie zwycięstwa dyplomatycznego . Żeby odnieść zwycięstwo dyplomatyczne, cywilizacja musi otrzymać wystarczające poparcie dla rezolucji o globalnej hegemonii.[NEWLINE][NEWLINE]W przeciwieństwie do innych rezolucji, rezolucji w sprawie przywódcy światowego nie może przedłożyć żadna cywilizacja. Zamiast tego poświęcona jest temu co druga sesja Kongresu. Podczas tych sesji, przeplatających się z sesjami skoncentrowanymi na zwykłych propozycjach, rezolucja przywódcy światowego zostaje automatycznie przedłożona Kongresowi i nie głosuje się nad innymi.[NEWLINE][NEWLINE]Ilość poparcia delegatów potrzebna do zwycięstwa zależy od liczby cywilizacji i państw-miast w grze - można zobaczyć ją na ekranach Kongresu Światowego i drogi do zwycięstwa. Jeżeli żadna cywilizacja nie uzyska poparcia koniecznego do wygrania rezolucji przywódcy światowego, dwie cywilizacje, które uzyskały najwięcej głosów, otrzymają na stałe większą liczbę delegatów.'
    WHERE Tag = 'TXT_KEY_CONGRESS_VICTORY_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Gdy zostanie ukończony projekt Organizacji Narodów Zjednoczonych, Kongres Światowy przekształca się w Organizację Narodów Zjednoczonych. Po tym, i aktywowaniu ideologii światowej, odblokowana zostanie rezolucja wyboru przywódcy światowego, której zwycięzca osiąga zwycięstwo dyplomatyczne.'
    WHERE Tag = 'TXT_KEY_CONGRESS_UN_HEADING2_BODY' AND EXISTS (SELECT * FROM CSD WHERE Type='CSD_TEXT' AND Value= 1 );
--[sqlFILENAME]IdeologyChanges.sql
    UPDATE Language_pl_PL
    SET Text = 'Umożliwia zbudowanie [COLOR_POSITIVE_TEXT]wieży transmisyjnej[ENDCOLOR], budowli znacznie zwiększającej ilość punktów [ICON_CULTURE] kultury wytwarzanych przez miasto i wytwarzanie [ICON_TOURISM] turystki we wszystkich miastach o 15%.'
    WHERE Tag = 'TXT_KEY_TECH_RADIO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Pozwala na budowę [COLOR_POSITIVE_TEXT]śmigłowca bojowego[ENDCOLOR], który świetnie nadaje się do zwalczania czołgów wroga i zwiększa wytwarzanie [ICON_TOURISM] turystki we wszystkich miastach o 15%.'
    WHERE Tag = 'TXT_KEY_TECH_COMPUTERS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Podwaja generowaną przez całe imperium [ICON_TOURISM] turystykę.'
    WHERE Tag = 'TXT_KEY_DOUBLE_TOURISM' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    --Resolution Changes


    -- Sanctions

    UPDATE Language_pl_PL
    SET Text = 'Sankcje państw-miast'
    WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Sankcje nałożone na państwa-miasta'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_CITY_STATES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Sankcje nałożone na: {1_CivsList}'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_EMBARGO_PLAYERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Nie można stworzyć [ICON_INTERNATIONAL_TRADE] szlaków handlowych prowadzących do państw-miast. Kara za podżeganie znacznie zmniejszona, podczas wypowiadania wojny państwom-miastom i zdobywania ich miast.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_ALL_CITY_STATES_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Sankcja'
    WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Między wybraną a dowolną inną cywilizacją nie można tworzyć [ICON_INTERNATIONAL_TRADE] szlaków handlowych. Kara za podżeganie znacznie zmniejszona, podczas wypowiadania wojny cywilizacjom z nałożonymi na nie sankcjami i zdobywania ich miast.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_PLAYER_EMBARGO_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    -- World Peace

    UPDATE Language_pl_PL
    SET Text = 'Porozumienie o pokoju na świecie'
    WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Koszta utrzymania jednostek rosną o 25% [ICON_GOLD] złota.[ICON_VICTORY_DOMINATION] Kary za podżeganie podczas zdobywania miast i wypowiadania wojen znacznie zwiększone a punkty [ICON_VICTORY_DOMINATION] podżegania spadają znacznie wolniej niż normalnie.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_STANDING_ARMY_TAX_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '+{1_UnitMaintenancePercent}% [ICON_GOLD] złota na utrzymanie jednostek. [ICON_VICTORY_DOMINATION] Kary za podżeganie znacznie zwiększone'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_UNIT_MAINTENANCE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    -- World Religion

    UPDATE Language_pl_PL
    SET Text = 'Wyznacz religię, która stanie się oficjalnym wyznaniem świata. Cywilizacje, w których większość miast wyznaje daną religię, zyskują dodatkowego delegata. Założyciel religii lub ten, kto kontroluje jej święte miasto otrzymuje jeden dodatkowy głos na każdą inną cywilizację wyznającą tę religię. Religia rozprzestrzenia się o 25% szybciej, a jej święte miasto otrzymuje 50% premii do [ICON_TOURISM] turystyki. W danym momencie może istnieć tylko jedna religia światowa.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_RELIGION_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    -- World Ideology

    UPDATE Language_pl_PL
    SET Text = 'Wyznacz ideologię, która stanie się oficjalnym stanowiskiem świata. Cywilizacje ją wyznające zyskują dodatkowego delegata, plus 1 za każdą inną cywilizację wyznającą tę ideologię. Poparcie danej ideologii wśród opinii społecznej zwiększa się we wszystkich cywilizacjach. Umożliwia [COLOR_POSITIVE_TEXT]zwycięstwo dyplomatyczne[ENDCOLOR], jeśli skonstruowano Narody Zjednoczone. [NEWLINE][NEWLINE]Dostępna, gdy wszystkie cywilizacje wynajdą radio.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_IDEOLOGY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    -- Scholars in Residence

    UPDATE Language_pl_PL
    SET Text = 'Cywilizacje opracowują technologię o 20% szybciej, jeśli została ona wcześniej odkryta przez przynajmniej jedną inną cywilizację. Ta wartość jest zwiększana o dodatkowe 5% za każdy sojusz z państwem-miastem, który ma cywilizacja, maksymalnie do 50%.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_MEMBER_DISCOVERED_TECH_DISCOUNT_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '-{1_ResearchCostPercent}% [ICON_RESEARCH] kosztu badań technologii, które zostały już odkryte przez inną cywilizację. Dodatkowe -5% za każdy sojusz z państwem-miastem, do -50%'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_DISCOVERED_TECH_MODIFIER' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '+{1_CulturePerWorldWonder} [ICON_CULTURE] kultury z cudów świata, +1 [ICON_RESEARCH] nauki z wielkich dzieł'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Każdy cud świata daje swojemu miastu +2 [ICON_CULTURE] kultury a każde wielkie dzieło +1 [ICON_RESEARCH] nauki.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_CULTURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Każdy cud natury daje +2 do wszystkich zysków, gdy jest eksploatowany.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_NATURAL_HERITAGE_SITES_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '+{1_CulturePerWorldWonder} do zysków z cudów natury'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_CULTURE_FROM_NATURAL_WONDERS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Ulepszenia pól tworzone przez wielkich ludzi zapewniają +1 do [ICON_PRODUCTION] produkcji, [ICON_FOOD] żywności i [ICON_GOLD] złota podczas eksploatacji, zaś słynne miejsca tworzone przez archeologów zapewniają +1 [ICON_CULTURE] kultury, [ICON_RESEARCH] nauki, i [ICON_PEACE] wiary podczas eksploatacji.[NEWLINE][NEWLINE]Dostępne po wynalezieniu przez dowolną cywilizację archeologii.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_HISTORICAL_LANDMARKS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '+{1_NumCulture} [ICON_PRODUCTION]/[ICON_FOOD]/[ICON_GOLD] z ulepszeń pól wielkich ludzi'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_GREAT_PERSON_TILE_IMPROVEMENT_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '+{1_NumCulture} [ICON_CULTURE]/[ICON_RESEARCH]/[ICON_PEACE] z archeologicznych słynnych miejsc'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_EFFECT_SUMMARY_LANDMARK_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Darmowy ustrój społeczny.'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '[ICON_CULTURE] Kultura większa o 33% przez 20 tur.'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Cud Pałac Kryształowy pojawia się w [ICON_CAPITAL] stolicy. ({TXT_KEY_BUILDING_CRYSTAL_PALACE_HELP})'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Inicjuje projekt Wystawy Światowej. Podczas jego trwania cywilizacje mogą przekazywać [ICON_PRODUCTION] produkcję, by przyspieszyć prace - w tym celu należy wybrać odpowiednią pozycję na liście produkcji w mieście. Po zakończeniu projektu cywilizacje otrzymają premie zależne od wysokości ich wkładu.[NEWLINE][NEWLINE][ICON_TROPHY_GOLD]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_3_HELP}[NEWLINE][ICON_TROPHY_SILVER]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_2_HELP}[NEWLINE][ICON_TROPHY_BRONZE]: {TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_FAIR_1_HELP} [NEWLINE][NEWLINE]Dostępne po wynalezieniu przez dowolną cywilizację industrializacji.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_WORLD_FAIR_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '[ICON_TOURISM] Turystyka większa o 50% przez 20 tur.'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_1_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz wielką osobę twojego wyboru.'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_2_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Cud Wioska Olimpijska pojawia się w [ICON_CAPITAL] stolicy. ({TXT_KEY_BUILDING_OLYMPIC_VILLAGE_HELP})'
    WHERE Tag = 'TXT_KEY_LEAGUE_PROJECT_REWARD_WORLD_GAMES_3_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Zwiększa produkcję wielkich pisarzy, artystów i muzyków o 33%. Zmniejsza produkcję wielkich naukowców, inżynierów i kupców o 33%. [NEWLINE][NEWLINE]Cywilizacje z punktacją [COLOR_POSITIVE_TEXT]poniżej średniej światowej[ENDCOLOR] otrzymują premię do [ICON_PRODUCTION] produkcji i [ICON_CULTURE] kultury wynoszącą 10-30%. [NEWLINE][NEWLINE]Szczegóły: premia jest różnicą pomiędzy punktacją cywilizacji i obecnego lidera, podzielona przez 50.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Dofinansowywane sztuki'
    WHERE Tag = 'TXT_KEY_RESOLUTION_ARTS_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Zwiększa produkcję wielkich naukowców, inżynierów i kupców o 33%. Zmniejsza produkcję wielkich pisarzy, artystów i muzyków o 33%. [NEWLINE][NEWLINE]Cywilizacje z punktacją [COLOR_POSITIVE_TEXT]poniżej średniej światowej[ENDCOLOR] otrzymują premię do [ICON_PRODUCTION] produkcji i [ICON_CULTURE] kultury wynoszącą 10-30%. [NEWLINE][NEWLINE]Szczegóły: premia jest różnicą pomiędzy punktacją cywilizacji i obecnego lidera, podzielona przez 50.'
    WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Światowa inicjatywa na rzecz nauki'
    WHERE Tag = 'TXT_KEY_RESOLUTION_SCIENCES_FUNDING' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    -- World Leader

    UPDATE Language_pl_PL
    SET Text = 'Globalna hegemonia'
    WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Cywilizacja {1_CivName[2]} zyskała globalną hegemonię.'
    WHERE Tag = 'TXT_KEY_VP_DIPLO_SOMEONE_WON' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Wyznacz gracza, który może uzyskać globalną hegemonię. Jeśli cywilizacja uzyska poparcie wystarczającej liczby delegatów w kongresie, lider odniesie [COLOR_POSITIVE_TEXT]zwycięstwo dyplomatyczne[ENDCOLOR]. Liczba potrzebnych delegatów uzależniona jest od ilości cywilizacji oraz państw-miast, jakie uczestniczą w rozgrywce, i można ją sprawdzić na ekranie Kongresu Światowego.[NEWLINE][NEWLINE]Jeśli żadna z cywilizacji nie uzyska poparcia wymaganego do zwycięstwa, dwie cywilizacje, które mają największe poparcie, na stałe zyskują [COLOR_POSITIVE_TEXT]dodatkowego delegata[ENDCOLOR] do wykorzystania podczas kolejnych obrad.[NEWLINE][NEWLINE]Cywilizacja nie może zgłosić takiej propozycji. Automatycznie zgłaszana w regularnych odstępach od czasu, gdy zostanie aktywowana [COLOR_POSITIVE_TEXT]Rezolucja o ideologii światowej[ENDCOLOR].'
    WHERE Tag = 'TXT_KEY_RESOLUTION_DIPLO_VICTORY_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Liczba tur do sesji o globalnej hegemonii: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Delegaci niezbędni do globalnej hegemonii: [COLOR_POSITIVE_TEXT]{1_DelegatesForWin}[ENDCOLOR]'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Od kiedy Kongres Światowy stał się Organizacją Narodów Zjednoczonych, można odnieść zwycięstwo dyplomatyczne. Aby odblokować specjalne sesje, należy uchwalić [COLOR_POSITIVE_TEXT]Rezolucję o ideologii światowej[ENDCOLOR]. Po aktywowaniu, cywilizacja musi otrzymać poparcie {1_DelegatesForWin} lub więcej delegatów w głosowaniu na rezolucję o globalnej hegemonii. Ta ilość zależy od liczby cywilizacji i państw-miast w grze.[NEWLINE][NEWLINE]Głosowanie na rezolucję o globalnej hegemonii odbywa się co drugą sesję Kongresu, po uchwaleniu [COLOR_POSITIVE_TEXT]Rezolucji o ideologii światowej[ENDCOLOR], na przemian ze zwykłymi sesjami, dotyczącymi zwykłych propozycji.'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_WORLD_LEADER_INFO_TT' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '[NEWLINE][ICON_BULLET]{1_NumVotes} za poprzednie głosowania na globalną hegemonię'
    WHERE Tag = 'TXT_KEY_LEAGUE_OVERVIEW_MEMBER_DETAILS_EXTRA_VOTES' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = '[ICON_BULLET]Propozycja [COLOR_POSITIVE_TEXT]globalnej hegemonii[ENDCOLOR] co drugą sesję, jeśli aktywowano [COLOR_POSITIVE_TEXT]Rezolucję o ideologii światowej[ENDCOLOR]'
    WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_ERA_DIPLO_VICTORY_POSSIBLE' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Cywilizacja {@1_CivName[2]} zyskuje globalną hegemonię'
    WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Nie osiągnięto globalnej hegemonii'
    WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Po spełnieniu wymagań w postaci {1_NumDelegates} {1_NumDelegates: plural 1?delegata; other?delegatów;}, cywilizacja {@2_CivName[2]} zyskuje globalną hegemonię.'
    WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_PASS_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Nie uzyskano zdecydowanego poparcia przynajmniej {1_NumDelegates} {1_NumDelegates: plural 1?delegata; other?delegatów;}, zatem rezolucja o światowej hegemonii została odrzucona. Miast tego {2_NumCivilizations} {2_NumCivilizations: plural 1?cywilizacji; other?cywilizacjom;} z góry listy udało się zdobyć na stałe dodatkowego delegata.'
    WHERE Tag = 'TXT_KEY_NOTIFICATION_LEAGUE_VOTING_RESULT_WORLD_LEADER_FAIL_DETAILS' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'W Kongresie Światowym zakorzeniła się idea trwałego pokoju i zgromadzenie to odradza się jako Organizacja Narodów Zjednoczonych. Zwołano sesję specjalną, by wybrać gospodarza nowej potęgi dyplomatycznej. Organizacja Narodów Zjednoczonych będzie nadal decydować o różnych sprawach, ale zwykłe sesje odbywać się będą na przemian z głosowaniami dotyczącymi globalnej hegemonii, co umożliwia odniesienie [COLOR_POSITIVE_TEXT]zwycięstwa dyplomatycznego[ENDCOLOR]. Aby umożliwić te sesje, musi zostać uchwalona [COLOR_POSITIVE_TEXT]Rezolucja o ideologii światowej[ENDCOLOR].'
    WHERE Tag = 'TXT_KEY_LEAGUE_SPECIAL_SESSION_START_UNITED_NATIONS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Dyplomacja[NEWLINE][NEWLINE]Liczba tur do posiedzenia ONZ: {1_TurnsUntilSession}[NEWLINE][NEWLINE]Liczba tur do następnego głosowania dotyczącego [COLOR_POSITIVE_TEXT]globalnej hegemonii[ENDCOLOR]: {2_TurnsUntilVictorySession}[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Zwycięstwo dyplomatyczne[ENDCOLOR] jest już możliwe, jeśli uchwalono [COLOR_POSITIVE_TEXT]Rezolucję o ideologii światowej[ENDCOLOR].'
    WHERE Tag = 'TXT_KEY_EO_DIPLOMACY_AND_VICTORY_SESSION' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Na świecie zaczyna się {@1_EraName},albo dzięki postępowi technologicznemu, albo dzięki uchwalonym rezolucjom.'
    WHERE Tag = 'TXT_KEY_LEAGUE_SPLASH_MESSAGE_GAME_ERA' AND EXISTS (SELECT * FROM CSD WHERE Type='IDEOLOGY_CSD' AND Value= 1 );
