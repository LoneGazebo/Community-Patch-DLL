--translated by mrlg
--Tag: [sqlFILENAME]
--C4DFCompatibility.sql
--CorpText_en_US.sql
--Compatibility_en_US.sql (CSD)
--MoreLuxuries.sql

--[sqlFILENAME]C4DFCompatibility.sql
    UPDATE Language_pl_PL
    SET Text = 'Wasalizm'
    WHERE Tag = 'TXT_KEY_GAME_CONCEPT_SECTION_26';

--[sqlFILENAME]CorpText_en_US.sql
-- Civilized Jewelers

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz jeden [ICON_INTERNATIONAL_TRADE] szlak handlowy. Przychód [ICON_GOLD] złota z przychodzących i wychodzących szlaków handlowych zwiększony o 5. [NEWLINE][NEWLINE]Wszystkie targowiska, banki, mennice i giełdy produkują +1 [ICON_GOLD] złota.[NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_CIVILIZED_JEWELERS_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_GOLD] złoto, [ICON_RES_SILVER] srebro, [ICON_RES_GEMS] kamienie szlachetne, [ICON_RES_PEARLS] perły, [ICON_RES_AMBER] bursztyn lub [ICON_RES_JEWELRY] klejnoty.[NEWLINE][NEWLINE]Otrzymujesz darmowe biuro Cywilizowanych Jubilerów w tym mieście i odblokowujesz budowę biur Cywilizowanych Jubilerów w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_CIVILIZED_JEWELERS_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Wymagane surowce: [ICON_RES_GOLD] złoto, [ICON_RES_SILVER] srebro, [ICON_RES_GEMS] kamienie szlachetne, [ICON_RES_PEARLS] perły, [ICON_RES_AMBER] bursztyn lub [ICON_RES_JEWELRY] klejnoty.[NEWLINE][NEWLINE]Premia z biura: +5% do szybkości pojawiania się [ICON_GREAT_PEOPLE] wielkich ludzi za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: szlaki handlowe do obcych miast z koncesją Cywilizowanych Jubilerów zwiększają przychód [ICON_GOLD] złota wyjściowego miasta o 10%.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_CIVILIZED_JEWELERS';

    -- Firaxite Materials

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz jeden [ICON_INTERNATIONAL_TRADE] szlak handlowy. Posiadane szlaki handlowe nie mogą zostać splądrowane, z wyjątkiem przypadku wypowiedzenia wojny. +3 [ICON_RESEARCH] nauki ze wszystkich surowców monopolowych związanych z Firaxite Materials. Wszystkie fabryki produkują +2 [ICON_PRODUCTION] produkcji i +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_FIRAXITE_MATERIALS_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_IRON] żelazo, [ICON_RES_ALUMINUM] aluminium, [ICON_RES_MARBLE] marmur, [ICON_RES_COPPER] miedź, [ICON_RES_JADE] żad lub [ICON_RES_PORCELAIN] porcelanę. [NEWLINE][NEWLINE]Otrzymujesz darmowe biuro Firaxite Materials w tym mieście i odblokowujesz budowę biur Firaxite Materials w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_FIRAXITE_MATERIALS_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Premia z surowców: [ICON_RES_IRON] żelazo, [ICON_RES_ALUMINUM] aluminium, [ICON_RES_MARBLE] marmur, [ICON_RES_COPPER] miedź, [ICON_RES_JADE] żad i [ICON_RES_PORCELAIN] porcelana dostarczają dodatkową [ICON_RESEARCH] naukę.[NEWLINE][NEWLINE]Premia z biur: +3 [ICON_RESEARCH] nauki za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: Szlaki handlowe do zagranicznych miast z koncesją Firaxite Materials Franchise przynoszą +100% [ICON_RESEARCH] nauki a posiadane szlaki nie mogą zostać splądrowane.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_FIRAXITE_MATERIALS';

    -- Giorgio Armeier

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz dwa [ICON_INTERNATIONAL_TRADE] szlaki handlowe. +1 [ICON_CULTURE] kultury ze wszystkich surowców monopolowych związanych z Giorgio Armeierem. +1 [ICON_CULTURE] kultury od wszystkich specjalistów. [NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_GIORIO_ARMEIER_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_SILK] jedwab, [ICON_RES_COTTON] bawełnę, [ICON_RES_DYE] barwniki, [ICON_RES_FUR] futra lub [ICON_RES_LAPIS] lapis lazuli.[NEWLINE][NEWLINE]Otrzymujesz darmowe biuro Giorgio Armeier w tym mieście i odblokowujesz budowę biur Giorgio Armeier Offices w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_GIORIO_ARMEIER_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Premia z surowców: [ICON_RES_SILK] jedwab, [ICON_RES_COTTON] bawełna, [ICON_RES_DYE] barwniki, [ICON_RES_FUR] futra i [ICON_RES_LAPIS] lapis lazuli dostarczają dodatkową [ICON_CULTURE] kulturę.[NEWLINE][NEWLINE]Premia z biur: +3 [ICON_CULTURE] kultury za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: szlaki handlowe do zagranicznych miast z koncesją Giorgio Armeier zwiększają wytwarzaną przez wyjściowe miasto [ICON_CULTURE] kulturę o +10%.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_GIORIO_ARMEIER';

    -- Hexxon Refineries

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz jeden [ICON_INTERNATIONAL_TRADE] szlak handlowy. Posiadane szlaki handlowe mają podwojoną prędkość [ICON_MOVES] ruchu. Otrzymujesz +3 [ICON_RES_COAL] węgla, [ICON_RES_OIL] ropy i [ICON_RES_URANIUM] uranu w tym mieście.[NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_HEXXON_REFINERY_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_COAL] węgiel, [ICON_RES_OIL] ropę, [ICON_RES_INCENSE] kadzidło, [ICON_RES_URANIUM] uran, [ICON_RES_PERFUME] perfumy lub [ICON_RES_GLASS] szkło.[NEWLINE][NEWLINE]Otrzymujesz darmowe biuro Hexxon Refineries w tym mieście i odblokowujesz budowę biur Hexxon Refineries w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_HEXXON_REFINERY_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Potrzebne surowce: [ICON_RES_COAL] węgiel, [ICON_RES_OIL] ropa, [ICON_RES_INCENSE] kadzidło, [ICON_RES_URANIUM] uran, [ICON_RES_PERFUME] perfumy lub [ICON_RES_GLASS] szkło.[NEWLINE][NEWLINE]Premia z biur: +1 [ICON_RES_OIL] ropa i +1 [ICON_RES_COAL] węgiel za każde [COLOR_POSITIVE_TEXT]trzy[ENDCOLOR] globalne koncesje (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: szlaki handlowe do zagranicznych miast z koncesją Hexxon Refineries zwiększają wytwarzaną przez wyjściowe miasto [ICON_PRODUCTION] produkcję o +10%, a posiadane szlaki handlowe są dwukrotnie szybsze niż normalnie.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_HEXXON_REFINERY';

    -- LandSea Extractors

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz dwa [ICON_INTERNATIONAL_TRADE] szlaki handlowe. +2 [ICON_PRODUCTION] produkcji ze wszystkich surowców monopolowych związanych z LandSea Extractors. Wszystkie przystanie i porty morskie wytwarzają +2 [ICON_FOOD] żywności i +2 [ICON_RESEARCH] nauki.[NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_LANDSEA_EXTRACTORS_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_HORSE] konie, [ICON_RES_WHALE] wieloryby, [ICON_RES_CRAB] kraby, [ICON_RES_IVORY] kość słoniową lub [ICON_RES_CORAL] korale.[NEWLINE][NEWLINE] Otrzymujesz darmowe biuro LandSea Extractors w tym mieście i odblokowujesz budowę biur LandSea Extractors w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_LANDSEA_EXTRACTORS_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Premia z surowców: [ICON_RES_HORSE] konie, [ICON_RES_WHALE] wieloryby, [ICON_RES_CRAB] kraby, [ICON_RES_IVORY] kość słoniowa i [ICON_RES_CORAL] korale dostarczają dodatkową [ICON_PRODUCTION] produkcję.[NEWLINE][NEWLINE]Premia z biur: +3 [ICON_PRODUCTION] produkcji za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: szlaki handlowe do zagranicznych miast z koncesją LandSea Extractors zwiększają wytwarzaną przez wyjściowe miasto [ICON_RESEARCH] naukę o +10%.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_LANDSEA_EXTRACTORS';

    -- Trader Sid's

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz dwa [ICON_INTERNATIONAL_TRADE] szlaki handlowe a szlaki lądowe mają 100% większy zasięg.[NEWLINE][NEWLINE]+2 [ICON_GOLD] złota ze wszystkich surowców monopolowych eksploatowanych przez to miasto, związanych z Handlowcami Sida. Wszystkie karawanseraje wytwarzają +2 [ICON_GOLD] złota i +2 [ICON_FOOD] żywności.[NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_TRADER_SIDS_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_SPICES] przyprawy, [ICON_RES_SUGAR] cukier, [ICON_RES_SALT] sól, [ICON_RES_NUTMEG] muszkatołowca, [ICON_RES_CLOVES] goździki, [ICON_RES_PEPPER] pieprz, [ICON_RES_COCOA] kakao lub [ICON_RES_TOBACCO] tytoń.[NEWLINE][NEWLINE]Otrzymujesz darmowe biuro Handlowców Sida w tym mieście i odblokowujesz budowę biur Handlowców Sida w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_TRADER_SIDS_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Premia z surowców: [ICON_RES_SPICES] przyprawy, [ICON_RES_SUGAR] cukier, [ICON_RES_SALT] sól, [ICON_RES_NUTMEG] muszkatołowiec, [ICON_RES_CLOVES] goździki, [ICON_RES_PEPPER] pieprz, [ICON_RES_COCOA] kakao i [ICON_RES_TOBACCO] tytoń dostarczają dodatkowe [ICON_GOLD] złoto.[NEWLINE][NEWLINE]Premia z biur: +4 [ICON_GOLD] za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: Szlaki handlowe do zagranicznych miast z koncesją Handlowców Sida dostarczają +50% [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_TRADER_SIDS';

    -- TwoKay Foods

    UPDATE Language_pl_PL
    SET Text = 'Otrzymujesz jeden [ICON_INTERNATIONAL_TRADE] szlak handlowy. Wszystkie szlaki handlowe +1 do zasięgu widzenia i odkrywają mgłę wojny. +2 [ICON_FOOD] żywności ze wszystkich surowców monopolowych związanych z TwoKay Foods. Wszystkie sklepy wytwarzają +2 [ICON_FOOD] żywności i +2 [ICON_GOLD] złota. [NEWLINE][NEWLINE]Pamiętaj, że każda cywilizacja może założyć tylko [COLOR_POSITIVE_TEXT]jedną[ENDCOLOR] korporację, więc upewnij się, że płynące z niej korzyści przydadzą się do twojej strategii na wygraną!'
    WHERE Tag = 'TXT_KEY_BUILDING_TWOKAY_FOODS_HQ_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga globalnego monopolu na [ICON_RES_WINE] wino, [ICON_RES_TRUFFLES] trufle, [ICON_RES_CITRUS] cytrusy, [ICON_RES_OLIVE] oliwki, [ICON_RES_COFFEE] kawę lub [ICON_RES_TEA] herbatę.[NEWLINE][NEWLINE]Otrzymujesz darmowe biuro TwoKay Foods w tym mieście i odblokowujesz budowę biur TwoKay Foods w pozostałych.'
    WHERE Tag = 'TXT_KEY_BUILDING_TWOKAY_FOODS_HQ_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Premia z surowców: [ICON_RES_WINE] wino, [ICON_RES_TRUFFLES] trufle, [ICON_RES_CITRUS] cytrusy, [ICON_RES_OLIVE] oliwki, [ICON_RES_COFFEE] kawa i [ICON_RES_TEA] herbata dostarczają dodatkową [ICON_FOOD] żywność.[NEWLINE][NEWLINE]Premia z biur: +3 [ICON_FOOD] żywności za każdą globalną koncesję (do twojego limitu koncesji).[NEWLINE][NEWLINE]Premia ze szlaków handlowych: szlaki handlowe do zagranicznych miast z koncesją TwoKay Foods zwiększają wytwarzaną przez wyjściowe miasto [ICON_FOOD] żywność o +10%, a wszystkie posiadane szlaki handlowe zyskuję +1 do zasięgu widzenia i odkrywają mgłę wojny.'
    WHERE Tag = 'TXT_KEY_CORP_HELPER_TWOKAY_FOODS';

--[sqlFILENAME]Compatibility_en_US.sql
	UPDATE Language_pl_PL
	SET Text = 'Specjaliści zapewniają następujące korzyści:[NEWLINE][NEWLINE]Artyści zwiększają wytwarzaną przez miasto kulturę i szybkość powstawania wielkich artystów.[NEWLINE][NEWLINE]Kupcy zwiększają wytwarzane przez miasto złoto i szybkość powstawania wielkich kupców.[NEWLINE][NEWLINE]Naukowcy zwiększają wytwarzaną przez miasto naukę i szybkość powstawania wielkich naukowców.[NEWLINE][NEWLINE]Inżynierowie zwiększają wytwarzaną przez miasto produkcję i szybkość powstawania wielkich inżynierów.[NEWLINE][NEWLINE]Urzędnicy dostarczają małą ilość wielu uzysków i zwiększają szybkość powstawania wielkich dyplomatów.'
	WHERE Tag = 'TXT_KEY_CITIES_BENEFITSSPECIALISTS_HEADING3_BODY';

    UPDATE Language_pl_PL
    SET Text = 'Ty i {1_CityStateName:textkey} jesteście [COLOR_POSITIVE_TEXT]sojusznikami[ENDCOLOR]. Każdej tury, twoje [ICON_INFLUENCE] wpływy na nich będą się zmieniać o {2_Num}. Jeśli wrogie jednostki uszkodzą {1_CityStateName[4]}, ta wartość ulegnie potrojeniu.'
    WHERE Tag = 'TXT_KEY_ALLIES_CSTATE_TT';

    UPDATE Language_pl_PL
    SET Text = 'Co turę twoje [ICON_INFLUENCE] wpływy u nich będą zmieniać się o {1_InfluenceRate}. Zatrzymają się na {2_InfluenceAnchor}. Jeśli wrogie jednostki uszkodzą to miasto, ta wartość ulegnie potrojeniu.'
    WHERE Tag = 'TXT_KEY_CSTATE_INFLUENCE_RATE';

    -- Policy Changes

    UPDATE Language_pl_PL
    SET Text = '[COLOR_POSITIVE_TEXT]Dyplomacja[ENDCOLOR] zwiększa twoje korzyści z przyjaźni z państwami-miastami i globalnej dyplomacji.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjęcie Dyplomacji zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET] Punkt równowagi [ICON_INFLUENCE] wpływu na państwa-miasta zwiększony o 20. [NEWLINE] [ICON_BULLET] Otrzymujesz 25% większe [ICON_INFLUENCE] nagrody za zadania wykonane dla państw-miast.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Każdy przyjęty ustrój Dyplomacji zapewnia:[ENDCOLOR] [NEWLINE] [ICON_BULLET]Twój [ICON_INFLUENCE] wpływ na państwa-miasta spada 5% wolniej.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Przyjęcie wszystkich ustrojów Dyplomacji zapewnia:[ENDCOLOR][NEWLINE] [ICON_BULLET] Odblokowanie budynku [COLOR_CYAN]Zakazany Pałac[ENDCOLOR]. [NEWLINE] [ICON_BULLET] Sojusznicze państwa-miasta będą ci okazjonalnie podarowywać [ICON_GREAT_PEOPLE] wielkiego człowieka. [NEWLINE] [ICON_BULLET] Kiedy ukańczasz [ICON_INTERNATIONAL_TRADE] szlak handlowy do państwa-miasta, otrzymujesz premię do [ICON_TOURISM] turystki we wszystkich znanych cywilizacjach opartą na twoim ostatnim wytwarzaniu [ICON_CULTURE] kultury.[NEWLINE] [ICON_BULLET] Zapewnia dostęp do renesansowych drzew ustrojów, ignorując wymagania epoki. [NEWLINE] [ICON_BULLET] Dostarcza [COLOR_CYAN]1[ENDCOLOR] punkt (z [COLOR_CYAN]3[ENDCOLOR] w sumie wymaganych) do uzyskania dostępu do ideologii.[NEWLINE] [ICON_BULLET] Pozwala na zakup [ICON_DIPLOMAT] wielkich dyplomatów za [ICON_PEACE] wiarę poczynając od epoki przemysłowej.'
    WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PATRONAGE_HELP';

    UPDATE Language_pl_PL
    SET Text = '[COLOR_POSITIVE_TEXT]Scholastyka[ENDCOLOR][NEWLINE]Otrzymujesz wielkich dyplomatów 25% szybciej. Wszystkie [COLOR_POSITIVE_TEXT]sojusznicze[ENDCOLOR] państwa-miasta zapewniają premię do [ICON_RESEARCH] nauki równą 33% tego, co same wytwarzają.'
    WHERE Tag = 'TXT_KEY_POLICY_SCHOLASTICISM_HELP';

    UPDATE Language_pl_PL
    SET Text = '[COLOR_POSITIVE_TEXT]Informatorzy[ENDCOLOR][NEWLINE]Otrzymujesz jednego lub więcej [ICON_SPY] szpiegów (zależy od ilości państw-miast w grze). Prasa Drukarska zwiększa [ICON_CULTURE] kulturę miasta o +10.'
    WHERE Tag = 'TXT_KEY_POLICY_PHILANTHROPY_HELP';

    -- Cultural Diplomacy
    UPDATE Language_pl_PL
    SET Text = '[COLOR_POSITIVE_TEXT]Dyplomacja kulturowa[ENDCOLOR][NEWLINE]Ilość surowców darowanych przez państwa-miasta zwiększa się o 100%. [ICON_HAPPINESS_1] zadowolenia z kancelarii.'
    WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_DIPLOMACY_HELP';

    UPDATE Language_pl_PL
    SET Text = '+1 [ICON_RES_PAPER] papier. +10% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Można wybudować tylko w stolicy. Koszt rośnie wraz ze wzrostem liczby miast w imperium. [NEWLINE][NEWLINE] 10% [ICON_PRODUCTION] produkcji miasta jest dodawane do obecnej [ICON_PRODUCTION] produkcji za każdym razem, gdy miasto zyskuje [ICON_CITIZEN] mieszkańca.'
    WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy jest unikatowy z tego względu, że gracze mogą go zbudować tylko w swojej stolicy. Daje małą premię do produkcji jednostek dyplomatycznych w stolicy i jeden surowiec [ICON_RES_PAPER] papieru. Postaw ten budynek, jeśli chcesz zabezpieczyć swoje sojusze z jednym czy dwoma państwami-miastami w pierwszych epokach gry. Dodatkowa [ICON_PRODUCTION] produkcja dostarczana przy wzroście miasta, co czyni ten budynek ważnym do zmaksymalizowania rozwoju na wczesnych etapach.'
    WHERE Tag = 'TXT_KEY_BUILDING_COURT_SCRIBE_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy nie może zostać zbudowany, dopóki w mieście nie zostanie zbudowana szkoła publiczna. Wybuduj go, aby uzyskać dodatkowe głosy na na Kongresie Światowym zależnie od ilości paktów obronnych, które masz obecnie zawarte z innymi graczami. Otrzymasz też dużą premię do kultury i nauki w mieście, gdzie go zbudowano.'
    WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga Ładu. +15% [ICON_CULTURE] kultury i [ICON_RESEARCH] nauki w mieście, gdzie go wybudowano. +2 [ICON_RES_PAPER] papieru. Otrzymujesz jeden głos za każdą [COLOR_POSITIVE_TEXT]pakt obronny[ENDCOLOR], którą masz obecnie zawartą z innymi graczami. [NEWLINE][NEWLINE]W mieście musi być wybudowana szkoła publiczna. Koszt rośnie wraz z liczbą miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy nie może zostać zbudowany, dopóki w mieście nie zostanie zbudowana akademia wojskowa. Wybuduj go, aby uzyskać dodatkowe głosy na na Kongresie Światowym zależnie od liczby zdobytych wrogich stolic. Otrzymasz też dużą premię do turystyki i wiary w mieście, gdzie ją zbudowano.'
    WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga Autokracji. +15 [ICON_TOURISM] turystyki i +15 [ICON_PEACE] wiary w mieście, gdzie ją wybudowano. +2 [ICON_RES_PAPER] papieru. Otrzymujesz jeden głos za każdą [COLOR_POSITIVE_TEXT]wrogą stolicę[ENDCOLOR], którą kontrolujesz. [NEWLINE][NEWLINE]W mieście musi być wybudowana akademia wojskowa.'
    WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy nie może zostać zbudowany, dopóki w mieście nie zostanie zbudowana giełda. Wybuduj go, aby uzyskać dodatkowe głosy na na Kongresie Światowym zależnie od ilości deklaracji przyjaźni, które masz obecnie zawarte z innymi graczami. Otrzymasz też dużą premię do żywności i złota w mieście, gdzie je zbudowano.'
    WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Wymaga Swobód. +15% [ICON_FOOD] żywności i [ICON_GOLD] złota w mieście, gdzie je wybudowano. +2 [ICON_RES_PAPER] papieru. Otrzymujesz jeden głos za każdą [COLOR_POSITIVE_TEXT]deklarację przyjaźni[ENDCOLOR], którą masz podpisaną z innymi graczami. [NEWLINE][NEWLINE]W mieście musi być wybudowana giełda. Koszt rośnie wraz z liczbą miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy nie może zostać zbudowany, dopóki w mieście nie zostanie zbudowana kancelaria. Zbuduj go, aby przyspieszyć produkcję jednostek dyplomatycznych w tym mieście, zwiększyć ich ruch, wpływy oraz zapewnić zdolność ignorowania kar terenowych.'
    WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = '+2 [ICON_RES_PAPER] papieru. +20% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Wszystkie jednostki dyplomatyczne otrzymują awans "[COLOR_POSITIVE_TEXT]Piśmienność[ENDCOLOR]".[NEWLINE][NEWLINE]W mieście musi być wybudowana kancelaria. Koszt rośnie wraz z liczbą miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Ten cud narodowy nie może zostać zbudowany, dopóki w mieście nie zostanie zbudowana agencja prasowa. Zbuduj go, aby przyspieszyć produkcję jednostek dyplomatycznych w tym mieście, zwiększyć ich ruch, wpływy oraz zdolność wysyłania ich na terytoria przeciwników bez porozumienia o [COLOR_POSITIVE_TEXT]otwartych granicach[ENDCOLOR].'
    WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = '+2 [ICON_RES_PAPER] papieru. +20% do [ICON_PRODUCTION] produkcji jednostek dyplomatycznych. Wszystkie jednostki dyplomatyczne otrzymują awans "[COLOR_POSITIVE_TEXT]Immunitet dyplomatyczny[ENDCOLOR]".[NEWLINE][NEWLINE]W mieście musi być wybudowana agencja prasowa. Koszt rośnie wraz z liczbą miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_HELP';

    -- Grand Temple Help Text

    UPDATE Language_pl_PL
    SET Text = 'Rozpoczyna się [ICON_GOLDEN_AGE] złota era. Redukuje [ICON_HAPPINESS_3] niepokoje religijne. Musisz mieć wybudowaną świątynię w tym mieście. Koszt rośnie wraz z ilością miast w imperium.'
    WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP';

    -- Merchant Mission

    UPDATE Language_pl_PL
    SET Text = 'Jeśli jednostka znajduje się na terenie państwa-miasta, z którym nie prowadzisz wojny, rozkaz ten spowoduje jej zniknięcie.[COLOR_POSITIVE_TEXT]Są trzy jednostki zdolne do przeprowadzania tej misji, każda ma inne zdolności:[ENDCOLOR][NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Jednostki dyplomatyczne:[ENDCOLOR] otrzymujesz [ICON_INFLUENCE] wpływy w tym państwie-mieście zależne o awansów jednostki.[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Wielki dyplomata:[ENDCOLOR] zyskujesz dużą ilość [ICON_INFLUENCE] wpływów na państwo-miasto a [ICON_INFLUENCE] wpływy wszystkich innych głównych cywilizacji znanych temu państwu-miastu zostaną obniżone o tą samą wartość.[NEWLINE][ICON_BULLET]   [COLOR_POSITIVE_TEXT]Wielki kupiec:[ENDCOLOR] uzyskasz duże ilości [ICON_GOLD] złota i natychmiastowy "Dzień uwielbienia dla króla" we wszystkich miastach. Spowoduje to zniknięcie jednostki.'
    WHERE Tag = 'TXT_KEY_MISSION_CONDUCT_TRADE_MISSION_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Zyskujesz [ICON_PEACE] wiarę, gdy twoja jednostka zginie w walce. Premia zależy od epoki.[NEWLINE][NEWLINE]+5 [ICON_PEACE] wiary ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście, i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%. [NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_MAUSOLEUM_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_TOURISM] turystyki ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%. [NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.[NEWLINE][NEWLINE]Zawiera 4 miejsca na wielkie dzieła sztuki lub artefakty. +10 [ICON_PEACE] wiary jeśli jest zapełniony sztuką.'
    WHERE Tag = 'TXT_KEY_BUILDING_RELIQUARY_HELP';

    UPDATE Language_pl_PL
    SET Text = '+15% do [ICON_PRODUCTION] produkcji jednostek wojskowych.[NEWLINE][NEWLINE]+5 [ICON_PRODUCTION] produkcji ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%.[NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_GREAT_ALTAR_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_PEACE] wiary ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię.[NEWLINE][NEWLINE]Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%.[NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_HEAVENLY_THRONE_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_GOLD] złota ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%. [NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_DIVINE_COURT_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_FOOD] żywności ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%. [NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_SACRED_GARDEN_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_RESEARCH] nauki ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%.[NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_HOLY_COUNCIL_HELP';

    UPDATE Language_pl_PL
    SET Text = '+4 punkty [ICON_GOLDEN_AGE] złotej ery.[NEWLINE][NEWLINE]+5 punktów [ICON_GOLDEN_AGE] złotej ery ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 25% i podnosi odporność miasta na nawracanie o 20%.[NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_APOSTOLIC_PALACE_HELP';

    UPDATE Language_pl_PL
    SET Text = '+5 [ICON_CULTURE] kultury ze wszystkich świętych miejsc.[NEWLINE][NEWLINE]Można wybudować jedynie w świętym mieście i tylko jeśli co najmniej 20% globalnej populacji wyznaje twoją religię. Redukuje [ICON_HAPPINESS_3] rozłam religijny i pozwala wybrać wierzenie reformacyjne. [NEWLINE][NEWLINE]Zwiększa nacisk większości [ICON_RELIGION] religijnej, wychodzący z tego miasta o 50% i podnosi odporność miasta na nawracanie o 20%. [NEWLINE][NEWLINE]Otrzymujesz 1 dodatkowego delegata na Kongresie Światowym za każde 8 miast wyznających twoją religię.'
    WHERE Tag = 'TXT_KEY_BUILDING_GRAND_OSSUARY_HELP';
--[sqlFILENAME]MoreLuxuries.sql
    UPDATE Language_pl_PL
    SET Text = 'Redukuje [ICON_HAPPINESS_3] niepokoje religijne.[NEWLINE]Pobliskie [ICON_RES_INCENSE] kadzidło: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE]Pobliskie [ICON_RES_WINE] wino: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_AMBER] bursztyn: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_BUILDING_TEMPLE_HELP';

    UPDATE Language_pl_PL
    SET Text = '+1 [ICON_PEACE] wiary na każdych 3 [ICON_CITIZEN] mieszkańców w mieście. Redukuje [ICON_HAPPINESS_3] niepokoje religijne i podwaja nacisk religijny generowany przez szlaki handlowe.[NEWLINE]Pobliskie [ICON_RES_INCENSE] kadzidło: +1 [ICON_CULTURE] kultury, +1 [ICON_PEACE] wiary.[NEWLINE]Pobliskie [ICON_RES_WINE] wino: +1 [ICON_CULTURE] kultury, +1 [ICON_PEACE] wiary.[NEWLINE]Pobliski [ICON_RES_AMBER] bursztyn: +1 [ICON_CULTURE] kultury, +1 [ICON_PEACE] wiary.'
    WHERE Tag = 'TXT_KEY_BUILDING_BASILICA_HELP';

    UPDATE Language_pl_PL
    SET Text = '+33% do szybkości pojawiania się [ICON_GREAT_WRITER] wielkich pisarzy w tym mieście, wszystkie gildie pisarzy produkują +1 [ICON_GOLD] złota.[NEWLINE]Pobliskie [ICON_RES_DYE] barwniki: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE] Pobliski [ICON_RES_SILK] jedwab: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_LAPIS] lapis lazuli: +2 [ICON_CULTURE] kultury.'
    WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Amfiteatr zwiększa [ICON_CULTURE] kulturę miasta. Podnosi szybkości pojawiania się wielkich pisarzy w tym mieście i wartość gildii pisarzy. Zapewnia premię ze wszystkich pobliskich źródeł barwników, jedwabiu i lapis lazuli, przyspieszając rozrost terytorium miasta i zdobywanie ustrojów społecznych. Zawiera 1 miejsce na wielkie dzieło literackie.'
    WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = '+2 [ICON_TOURISM] turystyki po odkryciu filozofii. +33% do szybkości pojawiania się [ICON_GREAT_WRITER] wielkich pisarzy w tym mieście, wszystkie gildie pisarzy produkują +1 [ICON_GOLD] złota.[NEWLINE][NEWLINE]Za każdym razem, gdy niszczysz wrogą jednostkę, otrzymujesz +5 [ICON_CULTURE] kultury w mieście, skaluje się zależnie od epoki.[NEWLINE]Pobliskie [ICON_RES_DYE] barwniki: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_SILK] jedwab: +1 [ICON_CULTURE] kultury, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_LAPIS] lapis lazuli: +2 [ICON_CULTURE] kultury.'
    WHERE Tag = 'TXT_KEY_BUILDING_ODEON_HELP';
	
	UPDATE Language_pl_PL
	SET Text = 'Unikatowy budynek grecki zastępujący amfiteatr. Buduj akropole, aby polepszyć obronę, kulturę i turystykę miasta, zwłaszcza podczas wojny dla dwóch ostatnich. Podnosi też szybkości pojawiania się wielkich pisarzy w tym mieście i wartość gildii pisarzy. Produkuje +2 [ICON_TOURISM] turystyki po odkryciu filozofii.'
	WHERE Tag = 'TXT_KEY_BUILDING_ODEON_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = '15% wytwarzanej przez miasto [ICON_FOOD] żywności dodaje się każdej tury do jego [ICON_RESEARCH] nauki. 25% [ICON_FOOD] żywności zostaje zachowane po wzroście miasta (efekt kumuluje się z akweduktem). Redukuje [ICON_HAPPINESS_3] ubóstwo.[NEWLINE]Pobliski [ICON_RES_TOBACCO] tytoń: +3 [ICON_GOLD] złota.[NEWLINE]Pobliska [ICON_RES_COFFEE] kawa: +1 [ICON_GOLD] złota, +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_TEA] herbata: +2 [ICON_GOLD] złota and +1 [ICON_PRODUCTION] produkcji.[NEWLINE][NEWLINE]Wymaga akweduktu w mieście.'
    WHERE Tag = 'TXT_KEY_BUILDING_GROCER_HELP';

    UPDATE Language_pl_PL
    SET Text = '15% wytwarzanej przez miasto [ICON_FOOD] żywności dodaje się każdej tury do jego [ICON_RESEARCH] nauki. +33% [ICON_GREAT_PEOPLE] wielkich ludzi generowanych w tym mieście i +25 [ICON_GOLD] złota (skaluje się zależnie od epoki), kiedy naradza się [ICON_GREAT_PEOPLE] wielka osoba.[NEWLINE][NEWLINE]Zachowuje ponad 25% [ICON_FOOD] żywności po wzroście miasta (efekt kumuluje się z akweduktem) i redukuje [ICON_HAPPINESS_3] ubóstwo.[NEWLINE][NEWLINE]Wymaga akweduktu w tym mieście.[NEWLINE]Pobliski [ICON_RES_TOBACCO] tytoń: +3 [ICON_GOLD] złota.[NEWLINE]Pobliska [ICON_RES_COFFEE] kawa: +1 [ICON_GOLD] złota, +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_TEA] herbata: +2 [ICON_GOLD] złota and +1 [ICON_PRODUCTION] produkcji.'
    WHERE Tag = 'TXT_KEY_BUILDING_COFFEE_HOUSE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

    UPDATE Language_pl_PL
    SET Text = 'Pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzdłuż szlaków handlowych wewnątrz twojej cywilizacji.[NEWLINE]Pobliski [ICON_RES_MARBLE] marmur: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_STONE] kamień: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_SALT] sól: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_JADE] żad: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.[NEWLINE][NEWLINE]Miasto potrzebuje co najmniej jednego z tych surowców, aby ulepszyć kamieniołom.'
    WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Zakład kamieniarski może być zbudowany tylko w mieście, które posiada dostęp do ulepszonych [ICON_RES_STONE] zasobów kamienia, [ICON_RES_SALT] soli, [ICON_RES_MARBLE] marmuru lub [ICON_RES_JADE] żadu. Zakład kamieniarski zwiększa [ICON_PRODUCTION] produkcję i pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzdłuż szlaków handlowych wewnątrz twojej cywilizacji.'
    WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Zapewnia +1 [ICON_CULTURE] kultury ze wszystkich pól rzecznych przy mieście i +10% [ICON_PRODUCTION] produkcji podczas wznoszenia budynków w tym mieście. Pozwala na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzdłuż szlaków handlowych wewnątrz twojej cywilizacji.[NEWLINE]Pobliski [ICON_RES_MARBLE] marmur: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_STONE] kamień: +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliska [ICON_RES_SALT] sól: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.[NEWLINE]Pobliski [ICON_RES_JADE] żad: +1 [ICON_PRODUCTION] produkcji, +1 [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Tabya to unikatowy budynek Songhajów, zastępujący zakład kamieniarski. Znacznie powiększa zysk [ICON_CULTURE] kultury z pól rzecznych, zwiększa wartość kamienia, marmuru, soli i żadu oraz przyspiesza produkcję przyszłych budynków o 10%. Pozwala też na przenoszenie [ICON_PRODUCTION] produkcji z tego miasta wzdłuż szlaków handlowych wewnątrz twojej cywilizacji.'
    WHERE Tag = 'TXT_KEY_BUILDING_MUD_PYRAMID_MOSQUE_STRATEGY';

    -- Olives, Perfume -- Colosseum
    UPDATE Language_pl_PL
    SET Text = 'Redukuje nieco [ICON_HAPPINESS_3] znudzenie. Koszary, kuźnia i zbrojownia w tym mieście dostarczają +2 [ICON_PRODUCTION] produkcji.[NEWLINE]Pobliskie [ICON_RES_PERFUME] perfumy: +2 [ICON_CULTURE] kultury.[NEWLINE]Pobliskie [ICON_RES_OLIVE] oliwki: +1 [ICON_FOOD] żywności, +1 [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Arena redukuje znudzenie w mieście, zapewnia dodatkową kulturę i zwiększa zysk z pobliskich źródeł [ICON_RES_PERFUME] perfum i [ICON_RES_OLIVE] oliwek. Wybuduj je, aby zwalczać niezadowolenie ze znudzenia, zwiększyć twoją kulturę i produkcję.'
    WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY';

    UPDATE Language_pl_PL
    SET Text = 'Za każdym razem, gdy pokonasz jednostkę lądową, produkuje 5 punktów [ICON_GREAT_GENERAL] wielkiego generała (5 punktów [ICON_GREAT_ADMIRAL] wielkiego admirała w przypadku jednostki morskiej) i 5 punktów [ICON_GOLDEN_AGE] złotej ery, skaluje się zależnie od epoki. [NEWLINE][NEWLINE]Koszary, kuźnia i zbrojownia w tym mieście wytwarzają +2 [ICON_PRODUCTION] produkcji.[ICON_CITY_CONNECTED] Połączenia miast dostarczają +2% więcej [ICON_GOLD] złota a [ICON_HAPPINESS_3] znudzenie zostaje nieco zredukowane.[NEWLINE]Pobliskie [ICON_RES_PERFUME] perfumy: +2 [ICON_CULTURE] kultury.[NEWLINE]Pobliskie [ICON_RES_OLIVE] oliwki: +1 [ICON_FOOD] żywności, +1 [ICON_GOLD] złota.'
    WHERE Tag = 'TXT_KEY_BUILDING_FLAVIAN_COLOSSEUM_HELP';

    UPDATE Language_pl_PL
    SET Text = 'Unikatowy budynek rzymski zastępujący arenę. Wybuduj koloseum, aby polepszyć kulturę, turystykę i produkcję miasta oraz zmaksymalizować zyski imperium ze zwycięstw twoich wojujących armii. Każde koloseum podnosi nieco wartość połączeń miast, co pozwala Rzymowi czerpać korzyści z dużego, rozciągniętego imperium. Ulepsza pobliskie źródła [ICON_RES_PERFUME] perfum i [ICON_RES_OLIVE] oliwek.'
    WHERE Tag = 'TXT_KEY_BUILDING_FLAVIAN_COLOSSEUM_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );
