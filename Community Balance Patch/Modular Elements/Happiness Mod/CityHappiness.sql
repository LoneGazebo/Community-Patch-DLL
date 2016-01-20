
	UPDATE Language_en_US
	SET Text = '[ICON_CITIZEN] Citizens in non-occupied Cities do not produce [ICON_HAPPINESS_3] Unhappiness directly. See the tooltips below for the sources of [ICON_HAPPINESS_3] Unhappiness in each City'
	WHERE Tag = 'TXT_KEY_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 0 [ICON_HAPPINESS_3] Unhappiness.'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every (non-occupied) City produces 0 [ICON_HAPPINESS_3] Unhappiness (normally).'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_CITIES_TT_NORMALLY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_OCCUPIED] Occupied City produces 0 [ICON_HAPPINESS_3] Unhappiness.'
	WHERE Tag = 'TXT_KEY_NUMBER_OF_OCCUPIED_CITIES_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Every [ICON_CITIZEN] Citizen in an [ICON_OCCUPIED] Occupied City produces 1 [ICON_HAPPINESS_3] Unhappiness'
	WHERE Tag = 'TXT_KEY_OCCUPIED_POP_UNHAPPINESS_TT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'Sort By Unhappiness'
	WHERE Tag = 'TXT_KEY_EO_SORT_STRENGTH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'City will be [ICON_RAZING] Razed to the ground in {1_Num} turn(s)! While being Razed, 50% of the [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness!'
	WHERE Tag = 'TXT_KEY_CITY_BURNING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'City is in [ICON_RESISTANCE] Resistance for {1_Num} turn(s)! While in Resistance, a City cannot do anything useful and 50% of the [ICON_CITIZEN] Population will produce [ICON_HAPPINESS_3] Unhappiness!'
	WHERE Tag = 'TXT_KEY_CITY_RESISTANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_Num} from [ICON_CITIZEN] Population in [ICON_OCCUPIED] Occupied or [ICON_RESISTANCE] Resisting Cities.'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = '{1_Num : number "#.##"} from [ICON_CITIZEN] Urbanization (Specialists).'
	WHERE Tag = 'TXT_KEY_TP_UNHAPPINESS_SPECIALISTS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- FREE HAPPINESS FROM GAME SPEED

	UPDATE GameSpeeds
	SET StartingHappiness = '10'
	WHERE Type = 'GAMESPEED_MARATHON' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE GameSpeeds
	SET StartingHappiness = '5'
	WHERE Type = 'GAMESPEED_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE GameSpeeds
	SET StartingHappiness = '0'
	WHERE Type = 'GAMESPEED_STANDARD' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	-- DEFINES FOR CITY HAPPINESS
	
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '1.00'
	WHERE Name = 'UNHAPPINESS_PER_OCCUPIED_POPULATION' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
		
	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Defines
	SET Value = '0'
	WHERE Name = 'UNHAPPINESS_PER_CAPTURED_CITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 25 = 0.25 unhappiness per specialist.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_SPECIALIST', '25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base reduction of unhappiness threshold for Puppet cities. -25% is default.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_PUPPET_THRESHOLD_MOD', '-25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base Modifier for Capital Thresholds. Offsets boost from Palace, helps make Capital a source of Unhappiness early on. 25% is default.
		INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_CAPITAL_MODIFIER', '25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- 	Base Value of Test - Modifier to tech % cost. 1.40 is default.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_TECH_BASE_MODIFIER', '1.40'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Base increase of threshold values based on # of citizens in cities you own. Modifier increases as cities grow. 150 is default. (Function is (CityPop x Value (150))/100. This value is added to City as a % modifier to needs.)

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_BASE_CITY_COUNT_MULTIPLIER', '150'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
	
-- Value by which yield/threshold difference is modified. Base value is 1%.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference for boredom is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_BOREDOM', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_ILLITERACY', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_DISORDER', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Value by which yield/threshold difference is divded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPY_CITY_BASE_VALUE_POVERTY', '50'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per religious minority pop. A high faith to population ratio will reduce this penalty. Also note that this is the ONLY unhappiness calculation that goes down as the game progresses (religion makes slightly less unhappiness as you move into new eras)
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_MINORITY_POP', '1.00'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per starving citizen.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_STARVING_PER_POP', '0.34'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pillaged plot owned by city.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_PER_PILLAGED', '0.34'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

-- Unhappiness point per pop if unconnected or blockaded.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_UNHAPPINESS_FROM_UNCONNECTED_PER_POP', '0.25'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );


-- Tutorial Text

	UPDATE Language_en_US
	SET Text = 'The happiness system in the Community Balance Patch is completely revised, favoring neither tall nor wide playstyles. Happiness is now based on a cause-and-effect mechanism that makes your empire feel far more alive, and dynamic, than ever before. [NEWLINE][NEWLINE]Your happiness is displayed on the status line of the Main Screen (in the upper left-hand corner of the game). Watch it carefully. If it reaches zero, your population is getting restless. If it starts to dip into negative numbers, you are in trouble. (Incidentally, you can get an excellent snapshot of your happiness by hovering your cursor over this number.)[NEWLINE][NEWLINE]One important note to remember - happiness generated from city buildings and city-based policies can never exceed its population.'
	WHERE Tag = 'TXT_KEY_HAPPINESS_HEADING1_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'The following increase your happiness:[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Luxury Resources[ENDCOLOR]: Improve resources within your territory or trade for them with other civs. Each kind of resource improves your happiness. Luxuries grant happiness as a factor of total empire population. The higher your pop, the more happiness your luxuries provide. This makes luxuries less vital early on, as +1 Happiness is not as big of a deal, yet it does not punish tall or wide strategies because the focus is on overall population, not the number of cities. In short, Luxuries "level up" as your empire grows in population.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Buildings[ENDCOLOR]: Certain buildings increase your happiness, while others reduce Unhappiness from things like Crime and Poverty (thus indirectly boosting your Happiness). These include the Colosseum, the Circus, the Zoo, and others. Each building constructed anywhere in your civ increases your overall happiness (so two Colosseums produce twice as much happiness as one, unlike Luxuries).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Wonders[ENDCOLOR]: Certain wonders like Notre Dame and the Hanging Gardens can give you a big boost in happiness.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Social Policies[ENDCOLOR]: Policies provide a lot of happiness, but in different ways.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Technologies[ENDCOLOR]: Technologies in themselves do not provide happiness, but they do unlock the buildings, wonders, resources and social policies which do.'
	WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSES_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

	UPDATE Language_en_US
	SET Text = 'City happiness is no longer directly affected by the number of citizens in a city. Instead, happiness is based on a set of functions that look at economic variables. Overall, population is still key, however per-city unhappiness is a bit easier to manage (and tailor to certain play-styles). This makes Tall and Wide gameplay equally feasible, so long as your empire is well-managed. Furthermore, Citizens can only be "unhappy" for one value at a time, based on a scale of want. If a citizen is unhappy from Poverty, for example, that same citizen cannot be unhappy from anything else until the Poverty issue is resolved (thus the unhappiness generated by any one city is capped at the population of the city).[NEWLINE][NEWLINE]How this works: The yield-based functions combine the yields of your city, dividing this value by its population. This value is then compared to the rate value per citizen as a global average of all city yields per pop. If the global rate value per citizen is higher than the rate value of your city, (i.e. you are not producing as much as the rate value expects), the difference between the two is factored by the per-citizen unhappiness value and the unhappiness is generated. In other words, if your cities are behind the global average in yields per pop, the difference will be converted into unhappiness. This Global Average will gradually increase as the game progresses (all cities become more and more wealthy over time, therefore the Global Average of what defines Poverty changes), and increases gradually (around 2%) for every City you found or conquer.[NEWLINE][NEWLINE] Sources of Unhappiness: [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Starvation[ENDCOLOR]: This one can be fatal, so be careful. If a city is starving, this function looks at the value of that starvation (i.e. the negative value of food being consumed) and compares that to the rate value of the function.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Pillaged tiles[ENDCOLOR]: Similar to city connections, this function will generate unhappiness based on a rate per citizen value which looks at the number of pillaged tiles. The more pillaged tiles there are, the higher this unhappiness.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Poverty[ENDCOLOR]: This looks at gold yields. If these yields do not exceed the rate criteria, your city has some element of poverty. This is largely inevitable in all but the wealthiest cities.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Isolation[ENDCOLOR]: Blockades or a lack of a city connection will generate unhappiness based on a rate per citizen value. Should only be a problem if you are besieged from the sea, your routes are pillaged and/or you have not connected your cities together. If you connect cities to your capital via an internal trade route (daisy chain or individually), this unhappiness penalty is negated (so blockades or long-distance cities can be "connected" to your capital via trade).[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Crime[ENDCOLOR]: This function looks at the defensive capabilities of your city and its garrison. This test takes defense as a "yield" (i.e. the defense produced by defensive structures and garrisons) and subtracts city damage, if any. If these yields do not exceed the rate criteria, your city has some element of crime. This is inevitable in large cities or cities under siege, especially if you do not have a garrison. Cities that are damaged or surrounded by enemies have increased crime.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Religious Tension[ENDCOLOR]: This looks at the majority religion of the city, and compares it to the number of citizens that are not part of that religion. If that value exceeds the rate value of the function, you will have some religious strife. This is largely inevitable in most large cities, but should never be a major source of unhappiness. You can reduce this value further by building more faith-producing buildings in the city (high faith production has the ability to subtract from your minority population), or simply converting people to your one, true faith.[NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Illiteracy[ENDCOLOR]: This looks at the science yield of the city. If this yield does not exceed the rate criteria, your city has some element of illiteracy. This is largely inevitable in all but the smartest cities. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Boredom[ENDCOLOR]: This looks at the culture yields of the city. If these yields do not exceed the rate criteria, your city has some element of boredom (i.e. your citizenry is not entertained). This is can be circumvented by building Colosseums and other such buildings, or through Great Works. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]Specialists[ENDCOLOR]: Specialists generate unhappiness at a flat rate based on the number of specialists in your empire. [NEWLINE][NEWLINE][COLOR_POSITIVE_TEXT]War Weariness[ENDCOLOR]: Once you have an ideology, you have the potential to gain empire-wide War Weariness (this is in addition to ideological unrest).'
	WHERE Tag = 'TXT_KEY_HAPPINESS_CAUSESUNHAPPINESS_HEADING2_BODY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );
