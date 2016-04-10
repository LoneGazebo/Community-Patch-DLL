--translated by mrlg
INSERT INTO Language_pl_PL
			(Tag,											Text)
VALUES		('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS',		'Barbarzyñskie punkty WG/WA'),
			('TXT_KEY_GAME_OPTION_BARB_GG_GA_POINTS_HELP',	'Pozwala wszystkim graczom zbieraæ punkty wielkich genera³ów i admira³ów z walki z barbarzyñcami.');

-- CS Stuff
UPDATE Language_pl_PL
SET Text = 'Zniewolenie {@1_Unit[2]} - tracisz {2_NumInfluence} [ICON_INFLUENCE] wp³ywów'
WHERE Tag = 'TXT_KEY_POPUP_MINOR_BULLY_UNIT_AMOUNT';

UPDATE Language_pl_PL
SET Text = 'Jeœli to pañstwo-miasto jest bardziej [COLOR_POSITIVE_TEXT]przestraszone[ENDCOLOR] ciebie ni¿ [COLOR_WARNING_TEXT]oporne[ENDCOLOR], mo¿esz za¿¹daæ daniny w postaci {@3_Unit[2]} kosztem [ICON_INFLUENCE] wp³ywów.  {1_FearLevel}{2_FactorDetails}'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_UNIT_TT';

-- Fixed quest text - transitioned to new 'quest rewards' panel
UPDATE Language_pl_PL
SET Text = 'Uda³o ci siê zniszczyæ barbarzyñskie obozowisko, wedle proœby {1_MinorCivName[2]}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CAMP';

UPDATE Language_pl_PL
SET Text = '{3_TargetName:textkey} {3_TargetName: plural 1?wywo³uje; 2? wywo³uj¹} wojnê {1_CivName[3]}, powo³uj¹c siê na przyczyny dyplomatyczne. Otrzymujesz [COLOR_POSITIVE_TEXT]pe³n¹[ENDCOLOR] nagrodê za zadanie od {1_TargetName[2]} przez [COLOR_NEGATIVE_TEXT]zniszczenie[ENDCOLOR] {3_CivName[2]} lub [COLOR_POSITIVE_TEXT]czêœciow¹[ENDCOLOR] nagrodê za zadanie poprzez [COLOR_POSITIVE_TEXT]sprzymierzenie siê[ENDCOLOR] z oboma pañstwami-miastami. Presja miêdzynarodowa zmusi ich do zawarcia pokoju za [COLOR_POSITIVE_TEXT]{2_Num}[ENDCOLOR] {2_Num: plural 1?turê; 2?tury; 3?tur}, wiêc dzia³aj szybko, jeœli chcesz zainterweniowaæ!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_KILL_CITY_STATE';

UPDATE Language_pl_PL
SET Text = 'Wybucha wojna pomiêdzy {2_CivName[5]} a {1_TargetName[5]}'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_KILL_CITY_STATE';

UPDATE Language_pl_PL
SET Text = 'Wojna pomiêdzy {2_CivName[5]} and {1_TargetName[5]} zakoñczy³a siê po wyraŸnym zwyciêstwie {2_CivName[2]}. Twoi doradcy obawiaj¹ siê, ¿e to rozstrzygniecie mo¿e doprowadziæ w przysz³oœci do konfliktu.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_pl_PL
SET Text = '{1_TargetName} {1_TargetName: plural 1?{1_TargetName: gender masculine?pokonany; feminine?pokonana; neuter?pokonane}; 2?{1_TargetName: gender masculine?pokonani; *?pokonane}} przez {2_CivName[4]}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_SUMMARY_QUEST_COMPLETE_KILL_CITY_STATE';

UPDATE Language_pl_PL
SET Text = 'Podbij {1_CityStateName[4]} (pe³na nagroda) lub zawrzyj sojusz zarówno z {1_CityStateName[5]} jaki i tym pañstwem-miastem (czêœciowa nagroda).'
WHERE Tag = 'TXT_KEY_CITY_STATE_QUEST_KILL_CITY_STATE_FORMAL';


UPDATE Language_pl_PL
SET Text = 'Wedle proœby {2_MinorCivName[2]}, udaje ci siê w³¹czyæ {1_ResourceName[4]} do swojej sieci handlowej! Ich kupcy s¹ dosyæ zadowoleni!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONNECT_RESOURCE';

UPDATE Language_pl_PL
SET Text = 'Wedle proœby {2_MinorCivName[2]}, udaje siê skonstruowaæ {1_WonderName[4]}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONSTRUCT_WONDER';

UPDATE Language_pl_PL
SET Text = 'Wedle proœby {2_MinorCivName[2]}, udaje ci siê stworzyæ {1_UnitName[4]}! Napawa to podziwem ich uczonych.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GREAT_PERSON';

UPDATE Language_pl_PL
SET Text = 'Twoje odkrycie nowego cudu natury zainspirowa³o mieszkañców {1_MinorCivName[2]}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_NATURAL_WONDER';

UPDATE Language_pl_PL
SET Text = 'Twoje odkrycie {1_TargetName[2]} wielce zadowoli³o kupców z {2_MinorCivName[2]}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_FIND_PLAYER';

UPDATE Language_pl_PL
SET Text = 'Twój podarunek z³ota pomóg³ {2_MinorCivName[3]} odbudowaæ siê po zap³aceniu daniny {1_BullyName[3]}!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_GIVE_GOLD';

UPDATE Language_pl_PL
SET Text = 'Twoja obietnica ochrony {2_MinorCivName[2]} pomog³a im odbudowaæ siê po zap³aceniu daniny {1_BullyName[3]}!!'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_PLEDGE_TO_PROTECT';

UPDATE Language_pl_PL
SET Text = 'Wywarliœmy wra¿enie na {1_MinorCivName[6]} nasz¹ kultur¹!  Nie zwracaj¹ uwagi na kultury innych cywilizacji. Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_CULTURE';

UPDATE Language_pl_PL
SET Text = 'Inna cywilizacja zaimponowa³a {1_MinorCivName[3]} swoj¹ kultur¹.  Wzrost twojej kultury nie wystarczy³. Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_CULTURE';

UPDATE Language_pl_PL
SET Text = 'Kler z {1_MinorCivName[2]} jest poruszony twoj¹ pobo¿noœci¹.  Odrzuca on podrzêdne wiary innych cywilizacji. Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_FAITH';

UPDATE Language_pl_PL
SET Text = 'Inna cywilizacja zaimponowa³a {1_MinorCivName[3]} swoj¹ wiar¹.  Wzrost twojej wiary nie wystarczy³.  Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_FAITH';

UPDATE Language_pl_PL
SET Text = '{1_MinorCivName:textkey} {1_MinorCivName: plural 1?jest; 2?s¹} pod wra¿eniem twojego szybkiego postêpu technologicznego!  Ich naukowcy i intelektualiœci odwracaj¹ siê od innych cywilizacji, które uwa¿aj¹ za bêd¹ce w stanie zastoju. Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_CONTEST_TECHS';

UPDATE Language_pl_PL
SET Text = 'Inna cywilizacja zaimponowa³a {1_MinorCivName[3]} swoim postêpem technologicznym.  Twoje odkrycia naukowe nie wystarczy³y. Cywilizacje, którym siê powiod³o (mo¿liwe remisy):[NEWLINE]'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_ENDED_CONTEST_TECHS';

UPDATE Language_pl_PL
SET Text = '{2_MinorCivName[3]} spodoba³ siê widok ich rywala, {1_TargetName[2]}, wij¹cego siê i oddaj¹cego ciê¿ko zdobyte surowce.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_BULLY_CITY_STATE';

UPDATE Language_pl_PL
SET Text = 'Ul¿y³o {2_MinorCivName[3]}, poniewa¿ doœwiadczone przez nich niesprawiedliwoœci ze strony {1_TargetName[2]} s¹ teraz znane g³ównym potêgom œwiata.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_DENOUNCE_MAJOR';

UPDATE Language_pl_PL
SET Text = 'Mieszkañcy {@2_MinorCivName[2]} ciesz¹ siê, ¿e {@1_ReligionName} pojawia siê w ich mieœcie i staje siê dominuj¹c¹ religi¹.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_SPREAD_RELIGION';

UPDATE Language_pl_PL
SET Text = 'Mieszkañcy {@1_MinorCivName} ciesz¹ siê z po³¹czenia szlaku handlowego.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_QUEST_COMPLETE_TRADE_ROUTE';

-- Land Dispute Text
UPDATE Language_pl_PL
SET Text = '[COLOR_NEGATIVE_TEXT]Roszczenia terytorialne napinaj¹ wasze relacje.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_LAND_DISPUTE';

-- Reversed Tourism for Open Borders
UPDATE Language_pl_PL
SET Text = '+{1_Num}% premii za otwarte granice od:[NEWLINE]   '
WHERE Tag = 'TXT_KEY_CO_CITY_TOURISM_OPEN_BORDERS_BONUS';

-- Citadel Text
UPDATE Language_pl_PL
SET Text = 'Cytadela to potê¿na fortyfikacja, która mo¿e byæ wybudowana wy³¹cznie przez wielkiego genera³a. Mo¿esz zbudowaæ cytadelê w dowolnym miejscu na swoim terytorium.[NEWLINE][NEWLINE]Po zbudowaniu cytadeli, twoje granice kulturalne powiêksz¹ siê o pola otaczaj¹ce z ka¿dej strony cytadelê w promieniu jednego pola. Jeœli nowa granica bêdzie narusza³a ju¿ zajête pola przez inn¹ cywilizacjê, otrzymasz w rezultacie karê w relacjach dyplomatycznych. Ka¿da stacjonuj¹ca w obrêbie cytadeli jednostka otrzymuje premiê 100% do obronnoœci. Dodatkowo ka¿da wroga jednostka, która zakoñczy ruch na s¹siaduj¹cym z cytadel¹ polu, otrzyma 30 punktów obra¿eñ (obra¿enia nie nak³adaj¹ siê z innymi cytadelami).[NEWLINE][NEWLINE]Akropol w greckich Atenach jest wczesnym przyk³adem potê¿nej cytadeli. Takich budowli praktycznie nie mo¿na by³o zdobyæ w bezpoœrednim ataku, a zanim upad³y, potrafi³y wytrzymaæ d³ugotrwa³e oblê¿enie.'
WHERE Tag = 'TXT_KEY_CIV5_IMPROVEMENTS_CITADEL_TEXT';

-- CS Stuff
UPDATE Language_pl_PL
SET Text = 'Otrzyma³eœ daninê bardzo niedawno.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_VERY_RECENTLY';

UPDATE Language_pl_PL
SET Text = 'Otrzyma³eœ niedawno daninê.'
WHERE Tag = 'TXT_KEY_POP_CSTATE_BULLY_FACTOR_BULLIED_RECENTLY';

-- Razing
UPDATE Language_pl_PL	
SET Text = '[COLOR_POSITIVE_TEXT]Usuwane[ENDCOLOR] miasto bêdzie p³on¹æ [ICON_RAZING] w ka¿dej turze a¿ populacja osi¹gnie 0 i miasto zostanie usuniête z gry. Wytwarza to du¿o [ICON_HAPPINESS_4] niezadowolenia, ale te¿ wielce zwiêksza twoj¹ iloœæ [COLOR_POSITIVE_TEXT]punkty wojny[ENDCOLOR] przeciw temu graczowi.'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_RAZE';

-- LUA
UPDATE Language_pl_PL	
SET Text = 'Bazowe dochody:'
WHERE Tag = 'TXT_KEY_PEDIA_YIELD_LABEL';

UPDATE Language_pl_PL	
SET Text = 'Po³¹czone przez:'
WHERE Tag = 'TXT_KEY_PEDIA_IMPROVEMENTS_LABEL';

UPDATE Language_pl_PL
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_YIELDCHANGES';

UPDATE Language_pl_PL
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} ({TXT_KEY_ABLTY_NO_FRESH_WATER_STRING})'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_NOFRESHWATERYIELDCHANGES';

UPDATE Language_pl_PL
SET Text = '(s³ona woda)'
WHERE Tag = 'TXT_KEY_ABLTY_NO_FRESH_WATER_STRING';

UPDATE Language_pl_PL
SET Text = '(s³odka woda)'
WHERE Tag = 'TXT_KEY_ABLTY_FRESH_WATER_STRING';

UPDATE Language_pl_PL
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (s³ona woda).'
WHERE Tag = 'TXT_KEY_NO_FRESH_WATER';

UPDATE Language_pl_PL
SET Text = '{1_ImprovementName:textkey}: +{3_Num} {2_YieldType:textkey} (s³odka woda).'
WHERE Tag = 'TXT_KEY_FRESH_WATER';

UPDATE Language_pl_PL
SET Text = '{@1_ImprovementDescription}: +{3_Yield} {4_Icon} {@2_YieldDescription} ({TXT_KEY_ABLTY_FRESH_WATER_STRING})'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_FRESHWATERYIELDCHANGES';

UPDATE Language_pl_PL
SET Text = '{TXT_KEY_ABLTY_FASTER_MOVEMENT_STRING} {@1_RouteDescription}'
WHERE Tag = 'TXT_KEY_CIVILOPEDIA_SPECIALABILITIES_MOVEMENT';

UPDATE Language_pl_PL
SET Text = 'Pozwala jednostkom otrzymaæ awans "[COLOR_POSITIVE_TEXT]{@1_PromotionName}[ENDCOLOR]": {@2_PromotionHelp}'
WHERE Tag = 'TXT_KEY_FREE_PROMOTION_FROM_TECH';


-- Fixed diacritics for spy names.

UPDATE Language_pl_PL
SET Text = 'Antônio'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_0';

UPDATE Language_pl_PL
SET Text = 'Estêvão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_3';

UPDATE Language_pl_PL
SET Text = 'Fernão'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_4';

UPDATE Language_pl_PL
SET Text = 'Tomé'
WHERE Tag = 'TXT_KEY_SPY_NAME_BRAZIL_8';

UPDATE Language_pl_PL
SET Text = 'Daje dodatkowych szpiegów w liczbie zale¿nej od iloœci pañstw-miast i rozwija wszystkich istniej¹cych szpiegów. Os³abia te¿ skutecznoœæ nieprzyjacielskiego wywiadu o 15%. Niezbêdne s¹ posterunki policji we wszystkich miastach.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_pl_PL
SET Text = 'Wa¿ny obronny cud narodowy dla cywilizacji opartej na technologii. Narodowa Agencja Wywiadu daje dodatkowych w szpiegów, rozwija wszystkich istniej¹cych szpiegów i os³abia skutecznoœæ nieprzyjacielskiego szpiega o 15%. Cywilizacja musi mieæ posterunki policji we wszystkich miastach zanim mo¿liwe bêdzie utworzenie Narodowej Agencji Wywiadu.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

UPDATE Language_pl_PL
SET Text = 'Prowadzenie szpiegostwa'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE';

UPDATE Language_pl_PL
SET Text = 'Przerywanie planów'
WHERE Tag = 'TXT_KEY_SPY_STATE_ESTABLISHED_SURVEILLANCE_PROGRESS_BAR';

UPDATE Language_pl_PL
SET Text = '[COLOR_NEGATIVE_TEXT]Twoi szpiedzy zostali schwytani podczas przeprowadzanych operacji.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_CAUGHT_STEALING';

-- Changed Diplo Text for Trade

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Jesteœmy partnerami handlowymi.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_TRADE_PARTNER';

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Podzieli³eœ siê z nimi intryg¹.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_SHARED_INTRIGUE';

-- Changed Diplo Text for Assistance

UPDATE Language_pl_PL
SET Text = '[COLOR_POSITIVE_TEXT]Twoje ostatnie dzia³ania dyplomatyczne ich ciesz¹.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_ASSISTANCE_TO_THEM';

UPDATE Language_pl_PL
SET Text = '[COLOR_NEGATIVE_TEXT]Twoje ostatnie dzia³ania dyplomatyczne ich rozczarowuj¹.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_REFUSED_REQUESTS';

-- Unit Settle Help

UPDATE Language_pl_PL
SET Text = 'Miasto nie mo¿e tu zostaæ zbudowane. Na standardowej lub mniejszej mapie musz¹ byæ pomiêdzy nimi 3 pola. Na du¿ej i wiêkszej miasta wymagaj¹ 4 pól pomiêdzy sob¹.'
WHERE Tag = 'TXT_KEY_MISSION_BUILD_CITY_DISABLED_HELP';

-- Diplo Text for DoF changed
UPDATE Language_pl_PL
SET Text = 'Nasza deklaracja przyjaŸni musi zostaæ zerwana.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_MESSAGE_END_WORK_WITH_US';

UPDATE Language_pl_PL
SET Text = 'Nie chcieliœmy, ¿eby tak to siê skoñczy³o, ale nie mo¿emy d³u¿ej patrzeæ na wasze wojenne zapêdy. Uwa¿aj nasz¹ deklaracjê przyjaŸni za skoñczon¹.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WARMONGER_1';

UPDATE Language_pl_PL
SET Text = 'Wasza wspó³praca z pañstwami-miastami znajduj¹cymi siê w naszej strefie wp³ywów jest dla nas nie do przyjêcia. Uwa¿aj nasz¹ deklaracjê przyjaŸni za skoñczon¹.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_MINORS_1';

UPDATE Language_pl_PL
SET Text = 'Obawiamy siê, ¿e niedu¿a odleg³oœæ dziel¹ca nasze pañstwa Ÿle wp³ywa na nasze relacje. Uwa¿aj nasz¹ deklaracjê przyjaŸni za skoñczon¹.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_LAND_1';

UPDATE Language_pl_PL
SET Text = 'Wasza chciwoœæ wzglêdem cudów œwiata sta³a siê dla nas zbyt ra¿¹ca. Uwa¿aj nasz¹ deklaracjê przyjaŸni za skoñczon¹.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_WONDERS_1';

UPDATE Language_pl_PL
SET Text = 'Sta³o siê dla nas oczywistym, ¿e podró¿ujemy zupe³nie innymi œcie¿kami. Uwa¿aj nasz¹ deklaracjê przyjaŸni za skoñczon¹.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_VICTORY_1';

UPDATE Language_pl_PL
SET Text = 'Wiele siê ostatnio zmieni³o. W zwi¹zku z tym uwa¿amy, ¿e nie jest nam ju¿ potrzebna deklaracja przyjaŸni. Mamy nadziejê, ¿e zrozumiecie.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_1';

UPDATE Language_pl_PL
SET Text = 'Przykro nam, ale jesteœmy zmuszeni wypowiedzieæ nasz¹ deklaracjê przyjaŸni.'
WHERE Tag = 'TXT_KEY_END_WORK_WITH_US_DEFAULT_2';

UPDATE Language_pl_PL
SET Text = 'Nie zaproponujemy ci tyle, by ta transakcja by³a uczciwa. Mo¿e jednak zaoferowaæ ci tê umowê.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_1';

UPDATE Language_pl_PL
SET Text = 'Umowa, któr¹ proponujemy mo¿e byæ dla ciebie nieuczciwa. Proszê, przyjrzyj siê jej dobrze.'
WHERE Tag = 'TXT_KEY_GENERIC_TRADE_CANT_MATCH_OFFER_2';

UPDATE Language_pl_PL
SET Text = 'Niemo¿liwe. Posuwasz siê za daleko.'
WHERE Tag = 'TXT_KEY_DIPLO_DISCUSS_HOW_DARE_YOU';

UPDATE Language_pl_PL
SET Text = '[COLOR_NEGATIVE_TEXT]Rywalizujecie ze sob¹ w budowie cudów.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_DIPLO_WONDER_DISPUTE';

UPDATE Language_pl_PL
SET Text = '¯o³nierze do rozstawienia.'
WHERE Tag = 'TXT_KEY_DEMOGRAPHICS_ARMY_MEASURE';

UPDATE Language_pl_PL
SET Text = 'Niemcy pragn¹ utrzymywaæ dobre stosunki z krajami tak potê¿nymi, jak twój.'
WHERE Tag = 'TXT_KEY_LEADER_BISMARCK_GREETING_NEUTRAL_HELLO_1';

UPDATE Language_pl_PL
SET Text = '{1_Num} [ICON_CULTURE] z wielkich dzie³ i motywów'
WHERE Tag = 'TXT_KEY_CULTURE_FROM_GREAT_WORKS';

UPDATE Language_pl_PL
SET Text = 'Nieznane im technologie odkryte przez ciebie: {1_Num}.[NEWLINE]Otrzymaj¹ +{3_Num} [ICON_RESEARCH] nauki na tym szlaku dziêki kulturowym wp³ywom na ciebie.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_THEIR_SCIENCE_EXPLAINED';

UPDATE Language_pl_PL
SET Text = 'Nieznane ci technologie odkryte przez nich: {2_Num}.[NEWLINE]Otrzymujesz +{3_Num} [ICON_RESEARCH] nauki na tym szlaku dziêki twoim kulturowym wp³ywom na nich.'
WHERE Tag = 'TXT_KEY_CHOOSE_INTERNATIONAL_TRADE_ROUTE_ITEM_TT_YOUR_SCIENCE_EXPLAINED';

-- Trade Things
UPDATE Language_pl_PL
SET Text = 'Ten przywódca nie spotka³ jeszcze innych graczy, albo nie masz u niego za³o¿onej ambasady.'
WHERE Tag = 'TXT_KEY_DIPLO_OTHER_PLAYERS_NO_PLAYERS_THEM';

-- Civilopedia Refresh

UPDATE Language_pl_PL
SET Text = 'W mieœcie mo¿e siê znajdowaæ tylko jedna jednostka bojowa. Oddzia³ ten stanowi wówczas miejski garnizon i przyczynia siê znacznie do wzrostu obronnoœci metropolii. Jeœli zostanie ona zaatakowana, gdy jest w niej garnizon, on sam otrzyma te¿ obra¿enia, zapewniaj¹c przez to miastu jeszcze wiêcej ochrony. Jednak uwa¿aj, gdy¿ w ten sposób garnizon mo¿e zostaæ zniszczony.[NEWLINE][NEWLINE]Kolejne jednostki wojskowe mog¹ przechodziæ przez miasto, lecz nie mog¹ koñczyæ tury na jego terenie. Je¿eli wiêc wyszkolisz w mieœcie z garnizonem nowy oddzia³, musisz wyprowadziæ z miasta któr¹œ z jednostek przed koñcem tury.'
WHERE Tag = 'TXT_KEY_CITIES_COMBATUNITS_HEADING3_BODY';

UPDATE Language_pl_PL
SET Text = 'W³aœciciel miasta mo¿e umieœciæ jednostkê wojskow¹ w mieœcie, ¿eby wzmocniæ jego obronê. Czêœæ si³y bojowej jednostki dodaje siê do si³y miasta. Jednostka garnizonowa przejmie czêœæ wszystkich obra¿eñ, gdy miasto zostanie zaatakowane. Mo¿e to doprowadziæ do zniszczenia garnizonu, wiêc uwa¿aj! W przypadku zdobycia miasta jednostka ulega zniszczeniu.[NEWLINE][NEWLINE]Jednostka stacjonuj¹ca w mieœcie mo¿e atakowaæ pobliskie oddzia³y wroga, ale miasto traci premiê z garnizonu, jeœli jest to atak wrêcz, jednostka otrzymuje obra¿enia podczas tej walki, jak w ka¿dej innej.'
WHERE Tag = 'TXT_KEY_COMBAT_GARRISONINCITIES_HEADING3_BODY';

UPDATE Language_pl_PL
SET Text = 'Starcie w zwarciu mo¿e zakoñczyæ siê utrat¹ P¯ przez obie jednostki i zachowaniem ich pozycji wyjœciowych, b¹dŸ te¿ zniszczeniem jednej z jednostek i zajêciem jej pola przez jednostkê, która zwyciê¿y, [COLOR_POSITIVE_TEXT]chyba ¿e[ENDCOLOR] jest to cytadela, fort lub miasto - wówczas jednostka walcz¹ca w zwarciu pozostaje na swoim polu. Nale¿y pamiêtaæ, i¿ z chwil¹ wejœcia na pole zniszczonej jednostki, jednostka atakuj¹ca przejmuje wszystkie jednostki cywilne, które znajdowa³y siê pod ochron¹ broni¹cego siê. Jeœli broni¹ca siê jednostka prze¿yje, pozostaje ona na swoim heksie.[NEWLINE][NEWLINE]Wiêkszoœæ jednostek nie dysponuje mo¿liwoœci¹ wykonania ruchu po zakoñczeniu ataku. Niektóre jednostki mog¹ jednak wykonaæ ruch po przeprowadzeniu ataku, o ile tylko dysponuj¹ odpowiedni¹ liczb¹ punktów ruchu.[NEWLINE][NEWLINE]Wszystkie jednostki bior¹ce udzia³ w walce otrzymuj¹ PD, które mog¹ zostaæ spo¿ytkowane na awanse.'
WHERE Tag = 'TXT_KEY_COMBAT_MELEERESULTS_HEADING3_BODY';

UPDATE Language_pl_PL
SET Text = 'Wielcy genera³owie to wielcy ludzie z uzdolnieniami wojskowymi. Zapewniaj¹ premie w walce - zarówno ofensywnej, jak i defensywnej - dla wszystkich sprzymierzonych jednostek l¹dowych w zasiêgu dwóch pól od ich po³o¿enia. Wielki genera³ sam w sobie nie jest jednostk¹ bojow¹, mo¿e wiêc znajdowaæ siê na polu z tak¹ jednostk¹, by go chroni³a. Jeœli wroga jednostka wejdzie na pole, na którym jest wielki genera³, ginie on.[NEWLINE][NEWLINE]Wielki genera³ daje premiê 20% jednostce znajduj¹cej siê na jego polu i wszystkich znajduj¹cych siê do 2 pól od niego.[NEWLINE][NEWLINE]Wielcy genera³owie powstaj¹ w wyniku walk toczonych przez twoje jednostki. Mo¿na go zdobyæ równie¿ poprzez budynki, ustroje, wierzenia i zasady. Przeczytaj sekcjê o wielkich ludziach, by dowiedzieæ siê wiêcej.'
WHERE Tag = 'TXT_KEY_COMBAT_GREATGENERALS_HEADING2_BODY';

UPDATE Language_pl_PL
SET Text = 'Punkty wojny'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_TITLE';

UPDATE Language_pl_PL
SET Text = 'Punkty wojny to ca³y czas zmieniaj¹cy siê stan wojny pomiêdzy graczami. Mog¹ siê one wahaæ od 100 do -100, gdzie 100 oznacza twoje ca³kowite zwyciêstwo a -100 ca³kowite zwyciêstwo przeciwnika. Punkty wojny bêd¹ z czasem stopniowo spadaæ, aby podkreœliæ malej¹c¹ wagê ostatnich dzia³añ w d³ugich, przeci¹gaj¹cych siê konfliktach.[NEWLINE][NEWLINE]Po wywo³aniu wojny, punktacja wynosi zero dla obu graczy. Wraz z niszczeniem wrogich jednostek, pl¹drowaniem pól/jednostkê handlowych, pojmanie cywilów i zdobywaniem miast, bêdzie przybywaæ punktów wojny. Wartoœæ tych dzia³añ zale¿y od ogólnego rozmiaru twojego przeciwnika.[NEWLINE][NEWLINE]Kiedy przyjdzie do zawierania pokoju, iloœæ punktów wojny daje ci dobre pojêcie o tym, czego powinno siê oczekiwaæ od przeciwnika lub tego, czego on za¿¹da. W oknie handlu, iloœæ punktów wojny mo¿na rozumieæ jako wartoœæ "maksymalnego pokoju", co pokazuje ci dok³adnie, co mo¿esz wzi¹æ od przeciwnika (lub vice-versa). Kiedy traktat pokojowy zostanie podpisany, wartoœæ punktów wojny zostaje wyzerowana.'
WHERE Tag = 'TXT_KEY_DIPLOMACY_PEACE_HEADING3_BODY';

UPDATE Language_pl_PL
SET Text = 'W ruinach mo¿na znaleŸæ mapê najbli¿szego nieodkrytego miasta (która zniesie mg³ê wojny z pewnej liczby pól).'
WHERE Tag = 'TXT_KEY_BARBARIAN_MAP_HEADING4_BODY';

UPDATE Language_pl_PL
SET Text = 'Walcz¹c z barbarzyñcami, twoje gorzej wyszkolone jednostki bêd¹ zdobywaæ doœwiadczenie. Nale¿y jednak pamiêtaæ, i¿ jednostka posiadaj¹ca ju¿ 45 punktów doœwiadczenia (lub taka, która wymieni³a je na awanse), nie zdobêdzie ich wiêcej za walkê z barbarzyñcami.'
WHERE Tag = 'TXT_KEY_BARBARIAN_POINTLIMITS_HEADING3_BODY';

UPDATE Language_pl_PL
SET Text = 'Ka¿da cywilizacja w Civilization V ma jedn¹ lub wiêcej specjalnych jednostek narodowych. To unikatowe jednostki tej cywilizacji, które s¹ pod pewnym wzglêdem lepsze od standardowych wersji tych jednostek. Cywilizacja amerykañska ma na przyk³ad jednostkê milicji lepsz¹ od zwyk³ych arkebuzerów dostêpnych dla innych cywilizacji. Cywilizacja grecka ma hoplitów, zastêpuj¹cych w³óczników.[NEWLINE][NEWLINE]Specjalne jednostki ka¿dej cywilizacji opisano w Civilopedii.'
WHERE Tag = 'TXT_KEY_UNITS_NATIONAL_HEADING2_BODY';

UPDATE Language_pl_PL
SET Text = 'Je¿eli obca cywilizacja zdobêdzie pañstwo-miasto, a ty je jej odbijesz, to pojawi siê opcja jego wyzwolenia. Je¿eli z niej skorzystasz, natychmiast otrzymasz premiê [ICON_INFLUENCE] PW u tego miasta, czêsto wystarczaj¹c¹ aby sta³o siê ono twoim [COLOR_POSITIVE_TEXT]sojusznikiem[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_CITYSTATE_LIBERATING_HEADING2_BODY';

-- Barbarians

UPDATE Language_pl_PL
SET Text = 'Crom'
WHERE Tag = 'TXT_KEY_LEADER_BARBARIAN';

-- Religion

UPDATE Language_pl_PL
SET Text = 'G³owa {1_ReligionName[2]}'
WHERE Tag = 'TXT_KEY_RO_STATUS_FOUNDER';

