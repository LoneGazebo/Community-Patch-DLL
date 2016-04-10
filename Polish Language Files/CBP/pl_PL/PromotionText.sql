--translated by mrlg
-- Replace Drill with +10% Combat Strength everywhere.

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR], [COLOR_POSITIVE_TEXT]Premia do ataku z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR], [COLOR_POSITIVE_TEXT]Premia do ataku z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR], [COLOR_POSITIVE_TEXT]Premia do ataku z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Replace Shock with bonuses to Combat Strength and Attacking Fortified Units
	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej, +15% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]ufortyfikowanym jednostkom[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej, +15% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]ufortyfikowanym jednostkom[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej, +15% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]ufortyfikowanym jednostkom[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Replace Accuracy with +10% Ranged Combat Strength and damage versus cities.

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Replace Barrage with +5% Ranged Combat Strength and damage versus wounded units.

	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Woodsman, Ambush, Charge and Formation all get buffs in different terrains

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] ataku na [COLOR_POSITIVE_TEXT]OTWARTYM[ENDCOLOR] terenie, +25% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Podwójna szybkość ruchu [COLOR_POSITIVE_TEXT]przez dżunglę i miasta[ENDCOLOR], +10% do [ICON_STRENGTH] obrony w [COLOR_POSITIVE_TEXT]TRUDNYM[ENDCOLOR] terenie.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOODSMAN_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+25% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]kawalerii[ENDCOLOR], +10% do [ICON_STRENGTH] obrony na [COLOR_POSITIVE_TEXT]OTWARTYM[ENDCOLOR] terenie.'
	WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+25% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]kawalerii[ENDCOLOR], +10% do [ICON_STRENGTH] obrony na [COLOR_POSITIVE_TEXT]OTWARTYM[ENDCOLOR] terenie.'
	WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+25% do [ICON_STRENGTH] siły bojowej przeciwko jednostkom [COLOR_POSITIVE_TEXT]pancernym[ENDCOLOR], +10% do [ICON_STRENGTH] ataku w [COLOR_POSITIVE_TEXT]TRUDNYM[ENDCOLOR] terenie.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+25% do [ICON_STRENGTH] siły bojowej przeciwko jednostkom [COLOR_POSITIVE_TEXT]pancernym[ENDCOLOR], +10% do [ICON_STRENGTH] ataku w [COLOR_POSITIVE_TEXT]TRUDNYM[ENDCOLOR] terenie.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
-- Fix Morale tooltip
	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MORALE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Charge now a two-part upgrade

	UPDATE Language_pl_PL
	SET Text = 'Szarża I|Szarży I|Szarży I|Szarżę I', Gender = 'feminine'
	WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Cover now 25 and 25
	UPDATE Language_pl_PL
	SET Text = '+25% do [COLOR_POSITIVE_TEXT]obrony[ENDCOLOR] przeciwko wszystkim [COLOR_POSITIVE_TEXT]atakom dystansowym[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_COVER_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+25% do [COLOR_POSITIVE_TEXT]obrony[ENDCOLOR] przeciwko wszystkim [COLOR_POSITIVE_TEXT]atakom dystansowym[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_COVER_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_STRENGTH] siły bojowej i +5% dodatkowej [ICON_STRENGTH] siły bojowej, jeżeli jednostka sąsiaduje z oddziałami sojuszniczymi.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DISCIPLINE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom pływającym i lądowym[ENDCOLOR], +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+15% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom pływającym i lądowym[ENDCOLOR], +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom pływającym i lądowym[ENDCOLOR], +10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]zranionym jednostkom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Bombardment now specifically anti-City.
	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +20% [ICON_STRENGTH] przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +20% [ICON_STRENGTH] przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+5% do [ICON_RANGE_STRENGTH] ataku dystansowego, +20% [ICON_STRENGTH] przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom morskim[ENDCOLOR], [COLOR_POSITIVE_TEXT]premia z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+15% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom morskim[ENDCOLOR], [COLOR_POSITIVE_TEXT]premia z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]jednostkom morskim[ENDCOLOR], [COLOR_POSITIVE_TEXT]premia z flankowania[ENDCOLOR] zwiększona o 25%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR]. Ukradzione złoto jest równe 33% obrażeń zadanym miastu.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR]. Ukradzione złoto jest równe 33% obrażeń zadanym miastu.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% do [ICON_STRENGTH] siły bojowej przeciwko [COLOR_POSITIVE_TEXT]miastom[ENDCOLOR]. Ukradzione złoto jest równe 34% obrażeń zadanym miastu.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Wolfpack extremely strong

	UPDATE Language_pl_PL
	SET Text = '+20% premii do walki podczas ataku.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% premii do walki podczas ataku.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+20% premii do walki podczas ataku.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_3_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+15% do [ICON_STRENGTH] siły bojowej przeciwko jednostkom morskim i lądowym.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+15% do [ICON_STRENGTH] siły bojowej przeciwko jednostkom morskim i lądowym.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

-- Air Ambush - More Interceptions
	UPDATE Language_pl_PL
	SET Text = '1 dodatkowe [COLOR_POSITIVE_TEXT]przechwycenie[ENDCOLOR] na turę.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_1_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '1 dodatkowe [COLOR_POSITIVE_TEXT]przechwycenie[ENDCOLOR] na turę.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_2_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+100% premii przeciwko miastom.'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Premia przeciwko miastom (100)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = '+100% premii przeciwko miastom.'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_II_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Premia przeciwko miastom (100)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_II' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = 'Premia przeciwko miastom (150)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+150% premii przeciwko miastom.'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
-- Amphibious Change

	UPDATE Language_pl_PL
	SET Text = 'Anuluje karę za atak z morza lub przez rzekę i zapewnia premię do ruchu podczas przekraczania rzek.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMPHIBIOUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Medic I/II
	UPDATE Language_pl_PL
	SET Text = 'Ta jednostka i wszystkie na sąsiednich heksach odzyskują [COLOR_POSITIVE_TEXT]dodatkowe 5 PŻ[ENDCOLOR] na turę.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Ta jednostka i wszystkie jednostki w sąsiadujących polach [COLOR_POSITIVE_TEXT]odzyskują 5 dodatkowych PŻ[ENDCOLOR] na turę, a ta jednostka [COLOR_POSITIVE_TEXT]odzyskuje 5 dodatkowych PŻ[ENDCOLOR] na turę, gdy jest [COLOR_POSITIVE_TEXT]poza przyjaznym terytorium[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_II_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Buffalo Loins/Chest

	UPDATE Language_pl_PL
	SET Text = '+10% do [ICON_STRENGTH] siły bojowej. Premia z flankowania zwiększona o 25%. +10% do [COLOR_POSITIVE_TEXT]obrony[ENDCOLOR] przeciwko wszystkim [COLOR_POSITIVE_TEXT]atakom dystansowym[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_CHEST_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '+15% do [ICON_STRENGTH] siły bojowej. Premia z flankowania zwiększona o 50%. +15% do [COLOR_POSITIVE_TEXT]obrony[ENDCOLOR] przeciwko wszystkim [COLOR_POSITIVE_TEXT]atakom dystansowym[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_LOINS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

  UPDATE Language_pl_PL
	SET Text = 'Nie może wypływać na pełne morze, dopóki nie odkryjesz nawigacji.'
	WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Wymagana nawigacja'
	WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	
	-- Text change for Khan ability
	UPDATE Language_pl_PL
	SET Text = 'Jednostki na tym i sąsiednich polach[COLOR_POSITIVE_TEXT]leczą dodatkowych 10 PŻ[ENDCOLOR] co turę.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_GENERAL_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Fix Anti-Air Promotion Info
	UPDATE Language_pl_PL
	SET Text = 'Celowanie przeciwlotnicze I'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Celowanie przeciwlotnicze II'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR_II' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	-- Second Attack Explanations
	UPDATE Language_pl_PL
	SET Text = 'Może atakować dwa razy, ale drugi zużywa wszystkie punkty ruchu'
	WHERE Tag = 'TXT_KEY_PROMOTION_SECOND_ATTACK' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = 'Może atakować dwa razy, ale drugi zużywa wszystkie punkty ruchu'
	WHERE Tag = 'TXT_KEY_PROMOTION_LOGISTICS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

	UPDATE Language_pl_PL
	SET Text = '1 dodatkowy punkt ruchu. Może atakować dwa razy, ale drugi zużywa wszystkie punkty ruchu'
	WHERE Tag = 'TXT_KEY_PROMOTION_RESTLESSNESS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );
	