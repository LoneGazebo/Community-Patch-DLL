-- Palace
UPDATE Language_en_US
SET Text = 'Automatically built in [ICON_CAPITAL] Capitals.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_HELP';

-- Water Mill
UPDATE Language_en_US
SET Text = 'The Water Mill can only be constructed in a City located next to a river. The Water Mill increases the City''s [ICON_PRODUCTION] Production more efficiently (per Citizen in the City) than the Well.'
WHERE Tag = 'TXT_KEY_BUILDING_WATERMILL_STRATEGY';

-- Aqueduct
UPDATE Language_en_US
SET Text = 'The Aqueduct decreases the amount of [ICON_FOOD] Food a City needs to increase in size by 15%. Build Aqueducts in cities that you want to grow large over time. The City needs to have a Granary in order to construct the Aqueduct.'
WHERE Tag = 'TXT_KEY_BUILDING_AQUEDUCT_STRATEGY';

-- Medical Lab
UPDATE Language_en_US
SET Text = 'The Medical Lab is a late-game building which decreases the amount of [ICON_FOOD] Food a City needs to increase in size by 15%, immediately increases the City''s size, and generates instant [ICON_RESEARCH] Science for every [ICON_CITIZEN] Citizen born in the City (applying retroactively). The City needs to have a Hospital in order to construct the Medical Lab.'
WHERE Tag = 'TXT_KEY_BUILDING_MEDICAL_LAB_STRATEGY';

-- Garden
UPDATE Language_en_US
SET Text = 'The Garden increases the speed at which [ICON_GREAT_PEOPLE] Great People are generated in the City by 25%, and buffs the food output of Oases. Gardens require an Aqueduct in the City in order to be built.'
WHERE Tag = 'TXT_KEY_BUILDING_GARDEN_STRATEGY';

-- Amphitheater
UPDATE Language_en_US
SET Text = 'The Amphitheater increases the [ICON_CULTURE] Culture of a city. Also boosts the City''s Great Writer rate and the value of Writers'' Guilds. Grants bonuses to all nearby [ICON_RES_DYE] Dye, [ICON_RES_SILK] Silk, [ICON_RES_LAPIS] Lapis Lazuli, and [ICON_RES_BRAZILWOOD] Brazilwood resources, speeding the growth of the territory of the city and the acquisition of Social Policies. Contains 2 slots for [ICON_GW_WRITING] Great Works of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_STRATEGY';

-- Opera House
UPDATE Language_en_US
SET Text = 'The Opera House is a Renaissance-era building which increases the [ICON_CULTURE] Culture of a City. Also boosts the City''s Great Musician rate and the value of Musicians'' Guilds. Contains 1 slot for a Great Work of Music. Requires an Amphitheater in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OPERA_HOUSE_STRATEGY';

-- Museum
UPDATE Language_en_US
SET Text = 'The Museum is a mid-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output and reduces Boredom. Contains 2 slots for Great Works of Art. Requires a Gallery in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MUSEUM_STRATEGY';

-- Broadcast Tower
UPDATE Language_en_US
SET Text = 'A Broadcast Tower is a late-game building which increases [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism output of the City, and reduces Boredom. Requires an Opera House in the City before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_STRATEGY';

-- Forge
UPDATE Language_en_US
SET Text = 'The Forge improves sources of [ICON_RES_IRON] Iron and [ICON_RES_COPPER] Copper nearby, and boosts the [ICON_PRODUCTION] Production of Mines.'
WHERE Tag = 'TXT_KEY_BUILDING_FORGE_STRATEGY';

-- Stone Works
UPDATE Language_en_US
SET Text = 'Stone Works can only be constructed in a City near an improved [ICON_RES_STONE] Stone, [ICON_RES_MARBLE] Marble, [ICON_RES_SALT] Salt, [ICON_RES_JADE] Jade, [ICON_RES_AMBER] Amber, [ICON_RES_LAPIS] Lapis Lazuli, or [ICON_RES_URANIUM] Uranium resource. Stone Works increase [ICON_PRODUCTION] Production and allow [ICON_PRODUCTION] Production to be moved from this City along trade routes inside your Civilization.'
WHERE Tag = 'TXT_KEY_BUILDING_STONE_WORKS_STRATEGY';

-- Lighthouse
UPDATE Language_en_US
SET Text = 'The Lighthouse can only be constructed in a City next to a Coast Tile. It increases the [ICON_FOOD] Food and [ICON_GOLD] Gold output of water tiles. Also allows for City Connections over Water (see Concepts for more details).'
WHERE Tag = 'TXT_KEY_BUILDING_LIGHTHOUSE_STRATEGY';

-- Harbor
UPDATE Language_en_US
SET Text = 'The Harbor is a Medieval-era building, requiring a Lighthouse. It improves the range and [ICON_GOLD] Gold yield of sea trade routes, and boosts the value of sea tiles and resources. The Harbor also increases the [ICON_PRODUCTION] Production of Naval units by 15%.'
WHERE Tag = 'TXT_KEY_BUILDING_HARBOR_STRATEGY';

-- Seaport
UPDATE Language_en_US
SET Text = 'The Seaport is an Industrial-era building, requiring a Harbor. It increases the [ICON_PRODUCTION] Production and [ICON_GOLD] Gold output of water tiles and increases Military Units supplied by this City''s population by 10%. Also allows for Industrial City Connections over Water (see Concepts for more details).'
WHERE Tag = 'TXT_KEY_BUILDING_SEAPORT_STRATEGY';

-- Market
UPDATE Language_en_US
SET Text = 'Markets increase the amount of [ICON_GOLD] Gold a City generates, and improves nearby [ICON_RES_SPICES] Cinnamon and [ICON_RES_SUGAR] Sugar. Incoming [ICON_INTERNATIONAL_TRADE] Trade Routes generate +1 [ICON_GOLD] Gold for the City, and +1 [ICON_GOLD] Gold for Trade Route owner.'
WHERE Tag = 'TXT_KEY_BUILDING_MARKET_STRATEGY';

UPDATE Language_en_US
SET Text = 'Customs House'
WHERE Tag = 'TXT_KEY_BUILDING_MINT';

UPDATE Language_en_US
SET Text = 'The Customs House boosts the Gold value of Trade Routes, and boosts the Tourism you generate from Trade Routes to foreign Civilizations. Build these buildings in all Cities if you wish to improve your Gold output as well as the cultural value of your Trade Routes.'
WHERE Tag = 'TXT_KEY_BUILDING_MINT_STRATEGY';

UPDATE Language_en_US
SET Text = 'A custom house or customs house was a building housing the offices for the government officials who processed the paperwork for the import and export of goods into and out of a country. Customs officials also collected customs duty on imported goods. The custom house was typically located in a seaport or in a city on a major river with access to the ocean. These cities acted as a port of entry into a country. The government had officials at such locations to collect taxes and regulate commerce. Due to advances in electronic information systems, the increased volume of international trade and the introduction of air travel, the custom house is now often a historical anachronism. There are many examples of buildings around the world whose former use was as a custom house but that have since been converted for other use, such as museums or civic buildings.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_MINT_TEXT';

-- Bank
UPDATE Language_en_US
SET Text = 'The Bank is a Renaissance-era building which increases the City''s output of [ICON_GOLD] Gold, boosts the value of investments in the City, and reduces Poverty. The City must already possess a Market before a Bank can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_BANK_STRATEGY';

-- University
UPDATE Language_en_US
SET Text = 'The University is a Medieval-era building. It increases the [ICON_RESEARCH] Science output of the City, and it''s even more effective if the City is near to Jungle or Snow tiles. The City must already possess a Library before a University can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_UNIVERSITY_STRATEGY';

-- Public School
UPDATE Language_en_US
SET Text = 'The Public School is a mid-game building which increase a City''s output of [ICON_RESEARCH] based on the population of the City. The City must already possess a Univeristy before a Public School can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_PUBLIC_SCHOOL_STRATEGY';

-- Research Lab
UPDATE Language_en_US
SET Text = 'The Research Lab is a late-game building which greatly increases the [ICON_RESEARCH] Science output of a City. The City must already possess a Public School before Research Lab can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_LABORATORY_STRATEGY';

-- Observatory
UPDATE Language_en_US
SET Text = 'The Observatory increases [ICON_RESEARCH] Science output, especially for Cities near lots of Mountains.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_STRATEGY';

-- Windmill
UPDATE Language_en_US
SET Text = 'The Windmill is a Renaissance-era building which increases the [ICON_PRODUCTION] Production output of a City when constructing buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_WINDMILL_STRATEGY';

UPDATE Language_en_US
SET Text = 'Hydroelectric Power Plant'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT';

UPDATE Language_en_US
SET Text = 'Hydroelectric Power is a power system that greatly improves the yield output of freshwater tiles. Construct this building in cities with lots of rivers and lakes.'
WHERE Tag = 'TXT_KEY_BUILDING_HYDRO_PLANT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Nuclear Power Plant'
WHERE Tag = 'TXT_KEY_BUILDING_NUCLEAR_PLANT';

UPDATE Language_en_US
SET Text = 'Solar Power Plant'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT';

UPDATE Language_en_US
SET Text = 'Solar Power is an Atomic-era electrical system which can only be constructed in a City near a Desert. The Solar Power Plant increases the [ICON_PRODUCTION] Production and [ICON_RESEARCH] Science output of every land tile the City works, but provides triple yields on Desert tiles. The Solar Power Plant also increases the efficiency of working processes.'
WHERE Tag = 'TXT_KEY_BUILDING_SOLAR_PLANT_STRATEGY';

-- Walls
UPDATE Language_en_US
SET Text = 'Walls increase City Strength and Hit Points, making the City more difficult to capture. Increases Military Units supplied by this City''s population by 10%, and increases the City''s Ranged Strike range by 1. Also helps with managing the Empire Size Modifier in this City. Walls are quite useful for cities located along a frontier.'
WHERE Tag = 'TXT_KEY_BUILDING_WALLS_STRATEGY';

-- Castle
UPDATE Language_en_US
SET Text = 'The Castle is a Medieval-era building which increases Defensive Strength and reduces all incoming damage by 2. Increases Military Units supplied by this City''s population by 5%, and increases Production for all nearby Quarries by 1. Also helps with managing the Empire Size Modifier in this City. The City must already possess Walls before a Castle can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CASTLE_STRATEGY';

-- Arsenal
UPDATE Language_en_US
SET Text = 'The Arsenal is a Modern-era military building that increases Defense Strength and Hit Points, making the City more difficult to capture. Increases the City''s [ICON_RANGE_STRENGTH] Ranged Strike Range by 1. Garrisoned units receive an additional 5 Health when healing in this City. Increases Military Units supplied by this City''s population by 5%. Also helps with managing the Empire Size Modifier in this City. The City must already possess a Bastion Fort before an Arsenal can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_ARSENAL_STRATEGY';

-- Military Base
UPDATE Language_en_US
SET Text = 'The Military Base is a late-game building which increases Defensive Strength and Hit Points, and improves defense against air units. Garrisoned units receive an additional 20 Health when healing in this City. Increases Military Units supplied by this City''s population by 5%. Also helps with managing the Empire Size Modifier in this City. The City must already possess an Arsenal before a Military Base can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_MILITARY_BASE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Strategic Defense System'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER';

UPDATE Language_en_US
SET Text = 'Strategic Defense Systems have a 50% chance to detonate nuclear weapons early, which destroys Atomic Bombs outright and makes Nuclear Missiles only as effective as Atomic Bombs. They also reduce population loss from a nuclear attack on this City by 75% if a missile does strike, and deal 10 Damage to Air Units which strike this City. Duck and Cover!'
WHERE Tag = 'TXT_KEY_BUILDING_BOMB_SHELTER_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Strategic Defense Initiative (SDI) was a proposed missile defense system intended to protect the United States from attack by ballistic strategic nuclear weapons (intercontinental ballistic missiles and submarine-launched ballistic missiles). The concept was first announced publicly by President Ronald Reagan on 23 March 1983. Reagan was a vocal critic of the doctrine of mutual assured destruction (MAD), which he described as a ''suicide pact,'' and he called upon the scientists and engineers of the United States to develop a system that would render nuclear weapons obsolete.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_BOMB_SHELTER_TEXT';

-- Spaceship Factory
UPDATE Language_en_US
SET Text = 'The Spaceship Factory increases the speed at which a City constructs spaceship parts, and greatly boosts Science in the City. The Spaceship Factory requires one [ICON_RES_ALUMINUM] Aluminum resource, and the City must already possess a Factory before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_SPACESHIP_FACTORY_STRATEGY';

-- Circus
UPDATE Language_en_US
SET Text = 'The Circus reduces Boredom in a City and improves the Culture output of [ICON_RES_IVORY] Ivory. It also starts 10 turns of "We Love the King Day" in the City. Build these to combat Unhappiness from Boredom, and gain quick bursts of Culture.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Arena'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM';

UPDATE Language_en_US
SET Text = 'The Arena provides additional Culture, and generates Tourism from City''s population. Reduces Boredom in a City, and improves the output of nearby [ICON_RES_PERFUME] Perfume and [ICON_RES_OLIVE] Olives. Build these to combat Unhappiness from Boredom, to increase your Culture, and to improve the production of your military buildings.'
WHERE Tag = 'TXT_KEY_BUILDING_COLOSSEUM_STRATEGY';

UPDATE Language_en_US
SET Text = 'An arena is an enclosed area, often circular or oval-shaped, designed to showcase theater, musical performances, or sporting events. The word derives from Latin harena, a particularly fine/smooth sand used to absorb blood in ancient arenas such as the Colosseum in Rome. It is composed of a large open space surrounded on most or all sides by tiered seating for spectators. The key feature of an arena is that the event space is the lowest point, allowing for maximum visibility. Arenas are usually designed to accommodate a large number of spectators.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_COLISEUM_TEXT';

-- Zoo
UPDATE Language_en_US
SET Text = 'The Zoo reduces Boredom in a City, produces additional Culture, and boosts the Culture value of nearby Jungle and Forest tiles.'
WHERE Tag = 'TXT_KEY_BUILDING_THEATRE_STRATEGY';

-- Stadium
UPDATE Language_en_US
SET Text = 'Build the Stadium if you are going for a [COLOR_POSITIVE_TEXT]Culture Victory[ENDCOLOR]. Increases your influence with all City-States by 20 when completed. Coordinate the construction of Stadiums across your empire for a global boost in diplomatic relations.'
WHERE Tag = 'TXT_KEY_BUILDING_STADIUM_STRATEGY';

-- Monastery
UPDATE Language_en_US
SET Text = 'Monasteries increase [ICON_PEACE] Faith, [ICON_FOOD] Food, and [ICON_RESEARCH] Science output. Can only be built if you have adopted the Fealty Policy Branch. Construct this building by purchasing it with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BUILDING_MONASTERY_STRATEGY';

UPDATE Language_en_US
SET Text = 'National Monument'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC';

UPDATE Language_en_US
SET Text = 'This National Wonder increases the [ICON_GREAT_PEOPLE] Great People generation of a City by 25%. Receive [ICON_CULTURE] Culture when a [ICON_CITIZEN] Citizen is born in the City, and [ICON_GOLDEN_AGE] Golden Age Points whenever you unlock a policy. It also provides +1 [ICON_CULTURE] Culture. The City must have a Monument before it can construct the National Monument.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY';

UPDATE Language_en_US
SET Text = 'A National monument is a monument constructed in order to commemorate something of national importance such as a war or the founding of the country. The term may also refer to a specific monument status, such as a national heritage site, which most national monuments are by reason of their cultural importance rather than age. The National monument aims to represent the nation, and serve as a focus for national identity.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_PEDIA';

UPDATE Language_en_US
SET Text = 'School of Philosophy'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE';

UPDATE Language_en_US
SET Text = 'The School of Philosophy National Wonder produces [ICON_RESEARCH] Science and [ICON_CULTURE] Culture, especially during [ICON_GOLDEN_AGE] Golden Ages. The City must have a Library before it can construct the School of Philosophy.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Philosophy is the study of general and fundamental problems, such as those connected with reality, existence, knowledge, values, reason, mind, and language. Philosophy is organized into schools of thought and distinguished from other ways of addressing such problems by its critical, generally systematic approach and its reliance on rational argument. In more casual speech, by extension, philosophy can refer to the most basic beliefs, concepts, and attitudes of an individual or group. The word philosophy comes from the Ancient Greek philosophia, which literally means "love of wisdom". The introduction of the terms philosopher and philosophy has been ascribed to the Greek thinker Pythagoras.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_PEDIA';

UPDATE Language_en_US
SET Text = 'Royal Collection'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE';

UPDATE Language_en_US
SET Text = 'This National Wonder produces +1 [ICON_CULTURE] Culture for every 4 [ICON_CITIZEN] Citizens in the City, and +10% [ICON_CULTURE] Culture for the City. The City must have a Gallery before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY';

UPDATE Language_en_US
SET Text = 'The largest private collections of fine art pieces have historically accrued to royalty, who purchase and patronize artistic works as an exercise of their prestige. Many of the largest art collections in the world have started from the private collections of monarchs which were handed down for generations. In response to public pressure and popular desire for a stronger national identity, many royal collections (and often the private residences that house them) have been nationalized by their respective governments to form State Galleries and National Art Museums in the modern day.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_PEDIA';

-- Heroic Epic
UPDATE Language_en_US
SET Text = 'This National Wonder gives all land units built in this City (past and future) the "Morale" promotion, increasing their combat strength by +10%. +1 [ICON_PRODUCTION] Production in the City for every 5 [ICON_CITIZEN] Citizens. Increases Military Units supplied by this City''s population by 10%. Also creates a free Great Writer upon completion. The City must have a Barracks before it can construct the Heroic Epic.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY';

-- Circus Maximus
UPDATE Language_en_US
SET Text = 'This National Wonder provides +2 [ICON_HAPPINESS_1] Happiness, 2 [ICON_RES_HORSE] Horses, +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Boredom, and generates +10% [ICON_CULTURE] Culture and [ICON_GOLD] Gold during "We Love the King Day" in the City where it is built. The City must have an Arena before it can construct the Circus Maximus.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Chartered Company'
WHERE Tag = 'TXT_KEY_BUILDING_EAST_INDIA';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_BUILDING_EAST_INDIA} increases the amount of [ICON_GOLD] Gold a City generates and reduces [ICON_HAPPINESS_3] Poverty. Resource Diversity Modifiers for Trade Routes from this City increase by 25% if positive, and decrease by 25% if negative. You also receive a free copy of all Luxury Resources around the City.[NEWLINE][NEWLINE]Trade routes other players make to a City with a {TXT_KEY_BUILDING_EAST_INDIA} will generate an extra 4 [ICON_GOLD] Gold for the City owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY';

UPDATE Language_en_US
SET Text = 'A chartered company was an early modern form of business organization typically engaged in long-distance trade, operating in Europe, the Atlantic World, and Asia.[NEWLINE]Many chartered companies were joint-stock companies; capital was accumulated with the help of transferable shares and, if the company was profitable, shareholders received dividends. The liability of the owners was limited to what they had invested or guaranteed to the company.[NEWLINE]These companies were granted exclusive rights through royal charters, enabling them to trade, explore, and colonize specific regions. Acting as quasi-sovereign entities, chartered companies established settlements, raised armies, and conducted diplomacy with indigenous peoples, wielding considerable political power and generating immense wealth for their home countries.[NEWLINE]Notable examples include the Dutch East India Company (VOC, 1602-1799) and the English East India Company (EIC, 1600-1874), which played pivotal roles in European expansion and colonialism in South and East Asia.[NEWLINE][NEWLINE]Operating in various geographical regions, chartered companies faced differing levels of success. While those in Asia, like the VOC and the EIC, thrived due to large investments and state support, those in the Atlantic world struggled amidst private competition and limited profitability. A successful exception was the Hudson Bay Company (HBC): established in 1670, the company traded in furs and European goods within the area where the rivers fed the bay.[NEWLINE][NEWLINE]Chartered companies were integral to early modern political economy, reflecting responses to Spanish and Portuguese expansion in the 16th century. While perceived as tools for promoting state wealth and power, they faced criticism for draining national economies and generating private fortunes, notably in the case of the EIC.[NEWLINE]By the late 18th century, chartered companies fell out of favor in Europe, although the British revived the concept in colonial ventures in Africa during the late 19th century.'
WHERE Tag = 'TXT_KEY_CIV5_BUILDINGS_EAST_INDIA_TEXT';

-- Ironworks
UPDATE Language_en_US
SET Text = 'The Ironworks National Wonder provides 2 [ICON_RES_IRON] Iron, increases [ICON_PRODUCTION] Production in a City by 10, and generates [ICON_RESEARCH] Science every time you construct a building. The City must have a Forge before it can construct an Ironworks.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Imperial College'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY';

UPDATE Language_en_US
SET Text = 'The {TXT_KEY_BUILDING_OXFORD_UNIVERSITY} National Wonder provides +1 [ICON_CULTURE] Culture, and reduces [ICON_HAPPINESS_3] Illiteracy. +50 [ICON_CULTURE] Culture every time you research a Technology. The City must have a University before it can construct the {TXT_KEY_BUILDING_OXFORD_UNIVERSITY}.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY';

UPDATE Language_en_US
SET Text = 'In the medieval period, higher education evolved in various regions, each with its own distinct institutions and traditions. In Europe, the earliest universities emerged spontaneously as a scholastic guild, whether of masters or students, without any express authorization of a prince, king, pope or prelate.[NEWLINE][NEWLINE]In many cases, however, universities petitioned secular power for privileges. Emperor Frederick I in Authentica Habita (1158) gave the first privileges to students in Bologna. Another step was Pope Alexander III in 1179 requiring that masters of church schools give licenses to properly qualified teachers and forbidding them from charging for it. The integrity of a university was only preserved in such an internally regulated corporation, which protected the scholars from external intervention. This independently evolving organization was absent in the universities of southern Italy and Spain, which served the bureaucratic needs of monarchs, and were therefore their artificial creations.[NEWLINE][NEWLINE]The University of Paris was formally recognized when Pope Gregory IX issued the bull Parens scientiarum (1231). This was a revolutionary step: studium generale (universities) and universitas (corporations of students or teachers) existed beforehand, but after the issuing of the bull, they attained autonomy. The papal bull of 1233 stipulated that anyone admitted as a teacher in Toulouse had the right to teach everywhere without further examinations. In time, this privilege became the single most important defining characteristic of the university and the symbol of its institutional autonomy. By the year 1292, even the two oldest universities, Bologna and Paris, felt the need to seek similar bulls from Pope Nicholas IV.[NEWLINE][NEWLINE]Meanwhile, in the Islamic world, institutions such as madrasas and the House of Wisdom in Baghdad (established in the late 8th century) played vital roles in preserving and advancing knowledge, spanning fields from theology to medicine.[NEWLINE]Outside of Europe and the Islamic world, East Asia saw the rise of educational centers like the Imperial Academy in China (founded in 3 AD under Emperor Ping of Han as Taixue, then renamed Guozijian after the Sui dynasty) as well as the National Confucian Academy in Korea (Sungkyunkwan, 1398), which emphasized Confucian teachings and governmental administration. The Imperial Examination system in China further democratized access to education by selecting officials based on merit rather than birthright, fostering social mobility and intellectual development. These diverse educational systems in medieval times contributed to the spread of knowledge, cultural exchange, and societal progress, leaving a lasting impact on the intellectual landscape of their respective regions.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_PEDIA';

-- National Intelligence Agency
UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. Reduces [ICON_HAPPINESS_3] Distress in all Cities. The National Intelligence Agency provides 100 [ICON_SPY_POINT] Spy Points and improves City Security by 10. It also provides instant yields when performing Spy Missions or when defending against them. Empires with a lot of offensive spies will benefit greatly from this building. The City must have a Police Station before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

-- Grand Temple
UPDATE Language_en_US
SET Text = 'An important building for a civilization trying to spread their religion world-wide from an empire with few, populous cities. A City must have a Temple before it can construct the Grand Temple.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY';

UPDATE Language_en_US
SET Text = 'University of Sankore'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_OF_DJENNE';

UPDATE Language_en_US
SET Text = 'As the center of an Islamic scholarly community, the University of Sankore was very different in organization from the universities of medieval Europe. It had no central administration other than the Emperor. It had no student registers but kept copies of its student publishings. It was composed of several entirely independent schools or colleges, each run by a single master or imam. Students associated themselves with a single teacher, and courses took place in the open courtyard of the mosque or at private residences.'
WHERE Tag = 'TXT_KEY_WONDER_MOSQUE_OF_DJENNE_DESC';

UPDATE Language_en_US
SET Text = 'Palace of Westminster'
WHERE Tag = 'TXT_KEY_BUILDING_BIG_BEN';

-- United Nations
UPDATE Language_en_US
SET Text = 'When paired with a high rate of Great Person production, the United Nations grants a significant boost to diplomatic power in the World Congress for its owner. It also provides an additional Delegate, and triggers a Golden Age.[NEWLINE][NEWLINE]In addition to these immediate benefits, the construction of the United Nations triggers the start of the United Nations Special Session of the World Congress, which enables the Diplomatic Victory. May only be built collaboratively through the World Congress.'
WHERE Tag = 'TXT_KEY_BUILDING_UNITED_NATIONS_STRATEGY';

-- Cathedral
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Cathedrals belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Poverty and boosts the [ICON_GOLD] Gold yield of nearby farms and pastures. Gain [ICON_GOLD] Gold in the City when its borders expand.'
WHERE Tag = 'TXT_KEY_BUILDING_CATHEDRAL_STRATEGY';

-- Mosque
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Mosques belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Illiteracy and generates [ICON_CULTURE] Culture during [ICON_GOLDEN_AGE] Golden Ages.'
WHERE Tag = 'TXT_KEY_BUILDING_MOSQUE_STRATEGY';

-- Pagoda
UPDATE Language_en_US
SET Text = 'Can only be built in cities following a religion with the Pagodas belief. Construct this building by purchasing it with [ICON_PEACE] Faith. Reduces [ICON_HAPPINESS_3] Boredom and [ICON_HAPPINESS_3] Religious Unrest, and generates yields based on the number of Religions present in the City.'
WHERE Tag = 'TXT_KEY_BUILDING_PAGODA_STRATEGY';

-- Corporation franchises
UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_TRADER_SIDS}'
WHERE Tag = 'TXT_KEY_BUILDING_TRADER_SIDS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_CENTAURUS_EXTRACTORS}'
WHERE Tag = 'TXT_KEY_BUILDING_CENTAURUS_EXTRACTORS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_HEXXON_REFINERY}'
WHERE Tag = 'TXT_KEY_BUILDING_HEXXON_REFINERY_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_GIORGIO_ARMEIER}'
WHERE Tag = 'TXT_KEY_BUILDING_GIORGIO_ARMEIER_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_FIRAXITE_MATERIALS}'
WHERE Tag = 'TXT_KEY_BUILDING_FIRAXITE_MATERIALS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_TWOKAY_FOODS}'
WHERE Tag = 'TXT_KEY_BUILDING_TWOKAY_FOODS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '{TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_CIVILIZED_JEWELERS}'
WHERE Tag = 'TXT_KEY_BUILDING_CIVILIZED_JEWELERS_FRANCHISE_HELP';

-- Theming bonuses
UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +4 [ICON_CULTURE] and [ICON_TOURISM] for two [COLOR_POSITIVE_TEXT]Domestic[ENDCOLOR] [ICON_GW_WRITING] Great Works of Writing.
  [NEWLINE][ICON_BULLET] +4 [ICON_CULTURE] and [ICON_TOURISM] for two [COLOR_POSITIVE_TEXT]Foreign[ENDCOLOR] [ICON_GW_WRITING] Great Works of Writing.'
WHERE Tag = 'TXT_KEY_BUILDING_AMPHITHEATER_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +14 [ICON_CULTURE] and [ICON_TOURISM] for two [ICON_GW_MUSIC] Great Works of Music.'
WHERE Tag = 'TXT_KEY_BUILDING_BROADCAST_TOWER_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +25 [ICON_CULTURE] and [ICON_TOURISM] for three [ICON_GW_MUSIC] Great Works of Music from the [COLOR_POSITIVE_TEXT]same Civilization[ENDCOLOR] and the [COLOR_CYAN]same Era[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_BROADWAY_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +10 [ICON_CULTURE] and [ICON_TOURISM] for two [ICON_GW_WRITING] Great Works of Writing from the [COLOR_POSITIVE_TEXT]same Civilization[ENDCOLOR] and the [COLOR_CYAN]same Era[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GLOBE_THEATER_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +10 [ICON_CULTURE] and [ICON_TOURISM] for three [ICON_GW_WRITING] Great Works of Writing from [COLOR_POSITIVE_TEXT]different Civilizations[ENDCOLOR] and [COLOR_CYAN]different Eras[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_GREAT_LIBRARY_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +15 [ICON_CULTURE] and [ICON_TOURISM] for three [ICON_GW_ART] Great Works of Art from [COLOR_POSITIVE_TEXT]different Civilizations[ENDCOLOR] and [COLOR_CYAN]different Eras[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +15 [ICON_CULTURE] and [ICON_TOURISM] for two [ICON_GW_ART] Great Works of Art and two [ICON_ARTIFACT] Artifacts from [COLOR_POSITIVE_TEXT]different Civilizations[ENDCOLOR] and [COLOR_CYAN]different Eras[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_LOUVRE_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +10 [ICON_CULTURE] and [ICON_TOURISM] for two [COLOR_POSITIVE_TEXT]Foreign[ENDCOLOR] [ICON_GW_WRITING] from [COLOR_CYAN]different Eras[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +10 [ICON_CULTURE] and [ICON_TOURISM] for two [ICON_GW_ART] Great Works of Art from the [COLOR_POSITIVE_TEXT]same Civilization[ENDCOLOR] and the [COLOR_CYAN]same Era[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SISTINE_CHAPEL_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +25 [ICON_CULTURE] and [ICON_TOURISM] for two [ICON_GW_MUSIC] Great Works of Music from the [COLOR_POSITIVE_TEXT]same Civilization[ENDCOLOR] and [COLOR_CYAN]different Eras[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_SYDNEY_OPERA_HOUSE_HELP';

UPDATE Language_en_US 
SET Text = 'Possible Theming Bonuses:
  [NEWLINE][ICON_BULLET] +15 [ICON_CULTURE] and [ICON_TOURISM] for three [ICON_GW_ART] Great Works of Art from the [COLOR_POSITIVE_TEXT]same Civilization[ENDCOLOR] and the [COLOR_CYAN]same Era[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_WONDER_UFFIZI_HELP';

-- Theming bonus flavor text
UPDATE Language_en_US
SET Text = '{2_CivAdjective} Museum of the {1_EraAdjective} Era'
WHERE Tag = 'TXT_KEY_THEMING_BONUS_MUSEUM_5';

UPDATE Language_en_US
SET Text = 'Museum of {2_CivAdjective} Art'
WHERE Tag = 'TXT_KEY_THEMING_BONUS_MUSEUM_7';

UPDATE Language_en_US
SET Text = 'Museum of {2_CivAdjective} Warfare'
WHERE Tag = 'TXT_KEY_THEMING_BONUS_MUSEUM_8';
