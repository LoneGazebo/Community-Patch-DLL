-- Building Requirement

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_HEROIC_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_NATIONAL_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_CIRCUS_MAXIMUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_NATIONAL_TREASURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_NATIONAL_COLLEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_IRONWORKS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_OXFORD_UNIVERSITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_HERMITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_INTELLIGENCE_AGENCY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_GRAND_TEMPLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_TOURIST_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '15'
WHERE Type = 'BUILDING_HEROIC_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '15'
WHERE Type = 'BUILDING_NATIONAL_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '20'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '35'
WHERE Type = 'BUILDING_NATIONAL_TREASURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '20'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '40'
WHERE Type = 'BUILDING_IRONWORKS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '55'
WHERE Type = 'BUILDING_OXFORD_UNIVERSITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '70'
WHERE Type = 'BUILDING_HERMITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '90'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '30'
WHERE Type = 'BUILDING_GRAND_TEMPLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NationalPopRequired = '100'
WHERE Type = 'BUILDING_TOURIST_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- City-Based Cost 

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_HEROIC_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_NATIONAL_EPIC' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_CIRCUS_MAXIMUS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_NATIONAL_TREASURY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_NATIONAL_COLLEGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_IRONWORKS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_OXFORD_UNIVERSITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_HERMITAGE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_INTELLIGENCE_AGENCY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_GRAND_TEMPLE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_TOURIST_CENTER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Strategy Text

UPDATE Language_en_US
SET Text = 'This National Wonder gives all units built in this city the "Morale" promotion, increasing their combat strength by +15%. It also provides +1 [ICON_CULTURE] Culture. The Heroic Epic can be constructed when a city has a barracks. Requires a national population of at least 10 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder increases the [ICON_GREAT_PEOPLE] Great People generation of a city by 25%. It also provides +1 [ICON_CULTURE] Culture. A city must have a monument before it can construct a National Epic. Requires a national population of at least 15 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder reduces [ICON_HAPPINESS_3] Disorder threshold by 1 in all cities. It also provides +1 [ICON_CULTURE] Culture. A city must have a Colosseum before it can construct a Circus Maximus. Requires a national population of at least 20 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The East India Company increases the amount of [ICON_GOLD] Gold a city generates. Trade routes other players make to a city with an East India Company will generate an extra 4 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route. Requires a national population of at least 35 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The National College National Wonder increases a the [ICON_RESEARCH] Science output of a city by 50%, and it provides +1 [ICON_CULTURE] Culture. A city must have a library before it can construct a National College. Requires a national population of at least 20 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Ironworks National Wonder increases [ICON_PRODUCTION] Production in a city by 8 [ICON_PRODUCTION]. A city must have a workshop before it can construct an Ironworks. Requires a national population of at least 40 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The Oxford University National Wonder provides 1 free technology and +1 [ICON_CULTURE] Culture. The city must have a university before it can construct Oxford University. Requires a national population of at least 55 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder increases the cultural output of the city in which it is constructed by 50%. It cannot be constructed unless the city has an Opera House. Requires a national population of at least 70 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. The National Intelligence Agency provides an additional spy, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. A city must have a Police Station before it can construct the National Intelligence Agency. Requires a national population of at least 90 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'An important building for a civilization trying to spread their religion world-wide from an empire with few, populous cities. A city must have a Temple before it can construct the Grand Temple. Requires a national population of at least 30 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'With its massive boost to [ICON_TOURISM] Tourism, the National Visitor Center is an excellent choice for civilizations that are trying to achieve a Culture victory, or attempting to increase your ideological impact on other civilizations. Build it in your top [ICON_TOURISM] Tourism city it has a Hotel. Requires a national population of at least 100 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_STRATEGY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'All newly-trained non-air Units in this City receive the [COLOR_POSITIVE_TEXT]Morale[ENDCOLOR] Promotion, improving [ICON_STRENGTH] combat strength by 15%. Contains 1 slot for a Great Work of Writing.[NEWLINE][NEWLINE]Must have built a Barracks in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 10 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HEROIC_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+25% [ICON_GREAT_PEOPLE] Great People generation in this City. Contains 1 slot for a Great Work of Writing.[NEWLINE][NEWLINE]Must have built a Monument in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 15 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_EPIC_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Reduces [ICON_HAPPINESS_3] Disorder threshold by 1 in all cities. Must have built a Colosseum in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 20 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_CIRCUS_MAXIMUS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Market in the city. The cost goes up the more cities there are in the empire. Trade routes other players make to a city with an East India Company will generate an extra 4 [ICON_GOLD] Gold for the city owner and the trade route owner gains an additional 2 [ICON_GOLD] Gold for the trade route. Requires a national population of at least 35 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_TREASURY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Library in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 20 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_NATIONAL_COLLEGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Workshop in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 40 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_IRONWORKS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '1 Free Technology. Contains 2 slots for Great Works of Writing.[NEWLINE][NEWLINE]Must have built a University in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 55 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_OXFORD_UNIVERSITY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+50% [ICON_CULTURE] Culture in this City. Contains 3 slots for Great Works of Art.[NEWLINE][NEWLINE]Must have built an Opera House in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 70 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_HERMITAGE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Provides an additional spy and levels up all your existing spies. Also provides a 15% reduction in enemy spy effectiveness. Must have a Police Station in the city. Requires a national population of at least 90 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Temple in the city. The cost goes up the more cities there are in the empire. Requires a national population of at least 30 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_GRAND_TEMPLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Must have built a Hotel in the city. The cost goes up the more cities there are in the empire. 100% of the [ICON_CULTURE] Culture from World Wonders, Natural Wonders, and Improvements (Landmarks, Moai, Chateau) is added to the [ICON_TOURISM] Tourism output of the city. [ICON_TOURISM] Tourism output from Great Works +100%. Requires a national population of at least 100 before it can be constructed.'
WHERE Tag = 'TXT_KEY_BUILDING_TOURIST_CENTER_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

-- Tooltips

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_NO_ACTION_BUILDING_NEED_NATIONAL_POP', '[NEWLINE]{2_BuildingName:textkey} requires {1_Num} more national citizens to be constructed.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );

INSERT INTO Language_en_US (
Tag, Text)
SELECT 'TXT_KEY_NO_ACTION_BUILDING_NEED_LOCAL_POP', '[NEWLINE]{2_BuildingName:textkey} requires {1_Num} more local citizens to be constructed.'
WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS' AND Value= 1 );