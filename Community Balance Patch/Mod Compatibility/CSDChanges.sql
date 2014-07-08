-- Building Requirement - Printing Press

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_PRINTING_PRESS' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Building Requirement - Foreign Office

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_FOREIGN_OFFICE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Building Requirement - Palace S&C

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_PALACE_SCIENCE_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Building Requirement - Finance Center

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_FINANCE_CENTER' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Building Requirement - Ehrenhalle

UPDATE Building_PrereqBuildingClasses
SET NumBuildingNeeded = '1'
WHERE BuildingType = 'BUILDING_EHRENHALLE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '25'
WHERE Type = 'BUILDING_PRINTING_PRESS' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '40'
WHERE Type = 'BUILDING_FOREIGN_OFFICE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '45'
WHERE Type = 'BUILDING_PALACE_SCIENCE_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '45'
WHERE Type = 'BUILDING_FINANCE_CENTER' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NationalPopRequired = '45'
WHERE Type = 'BUILDING_EHRENHALLE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_PRINTING_PRESS' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_FOREIGN_OFFICE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_PALACE_SCIENCE_CULTURE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_FINANCE_CENTER' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Pop Requirement

UPDATE Buildings
SET NumCityCostMod = '35'
WHERE Type = 'BUILDING_EHRENHALLE' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- TEXT CHANGES

UPDATE Language_en_US
SET Text = 'This National Wonder cannot be built unless the city has a Public School and you have a national population of at least 45. Build this national wonder to receive additional votes in the World Congress based on the number of Research Agreements you currently have with other players. You will also receive a large boost to the Culture and Science output of the city where it is built.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_STRATEGY' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'Requires Order. +15% [ICON_CULTURE] Culture and [ICON_RESEARCH] Science in the city where it is built. +2 [ICON_RES_PAPER] Paper. Receive one vote for every [COLOR_POSITIVE_TEXT]Research Agreement[ENDCOLOR] you currently have with other players. [NEWLINE][NEWLINE]Must have built a Public School in the city. Requires a national population of at least 45 before it can be constructed. The cost goes up the more cities there are in the empire.'
WHERE Tag = 'TXT_KEY_BUILDING_PALACE_SCIENCE_CULTURE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'This National Wonder cannot be built unless the city has a Military Academy and you have a national population of at least 45. Build this national wonder to receive additional votes in the World Congress based on the number of enemy Capitals you have conquered. You will also receive a large boost to the Tourism and Faith output of the city where it is built.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_STRATEGY' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'Requires Autocracy. +15 [ICON_TOURISM] Tourism and +15 [ICON_PEACE] Faith in the city where it is built. +2 [ICON_RES_PAPER] Paper. Receive one vote for every [COLOR_POSITIVE_TEXT]enemy Capital[ENDCOLOR] you control. [NEWLINE][NEWLINE]Must have built a Military Academy in the city. Requires a national population of at least 45 before it can be constructed. The cost goes up the more cities there are in the empire.'
WHERE Tag = 'TXT_KEY_BUILDING_EHRENHALLE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );


UPDATE Language_en_US
SET Text = 'This National Wonder cannot be built unless the city has a Stock Exchange and you have a national population of at least 45. Build this national wonder to receive additional votes in the World Congress based on the number of Declaration of Friendships you currently have with other players. You will also receive a large boost to the Food and Gold output of the city where it is built.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_STRATEGY' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = 'Requires Freedom. +15% [ICON_FOOD] Food and [ICON_GOLD] Gold in the city where it is built. +2 [ICON_RES_PAPER] Paper. Receive one vote for every [COLOR_POSITIVE_TEXT]Declaration of Friendship[ENDCOLOR] you currently have with other players. [NEWLINE][NEWLINE]Must have built a Stock Exchange in the city. Requires a national population of at least 45 before it can be constructed. The cost goes up the more cities there are in the empire.'
WHERE Tag = 'TXT_KEY_BUILDING_FINANCE_CENTER_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );


UPDATE Language_en_US
SET Text = 'This National Wonder cannot be built unless the city has a Chancery and you have a national population of at least 25. Build it to receive a production speed increase for Diplomatic Units in the city where it is built, increased movement and influence for all your Diplomatic Units and the ability to allow your Diplomatic Units to ignore terrain penalties.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_STRATEGY' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = '+2 [ICON_RES_PAPER] Paper. +20% [ICON_PRODUCTION] Production of Diplomatic Units. All Diplomatic Units receive the [COLOR_POSITIVE_TEXT]Literacy[ENDCOLOR] Promotion.[NEWLINE][NEWLINE]Must have built a Chancery in the City. Requires a national population of at least 25 before it can be constructed. The cost goes up the more cities there are in the empire.'
WHERE Tag = 'TXT_KEY_BUILDING_PRINTING_PRESS_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );


UPDATE Language_en_US
SET Text = 'This National Wonder cannot be built unless the city has a Wire Service and you have a national population of at least 40. Build it to receive a production speed increase for Diplomatic Units in the city where it is built, increased movement and influence for all your Diplomatic Units and the ability to send your Diplomatic Units through rival territory without an [COLOR_POSITIVE_TEXT]Open Borders[ENDCOLOR] agreement.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_STRATEGY' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );

-- Help Text

UPDATE Language_en_US
SET Text = '+2 [ICON_RES_PAPER] Paper. +20% [ICON_PRODUCTION] Production of Diplomatic Units. All Diplomatic Units receive the [COLOR_POSITIVE_TEXT]Diplomatic Immunity[ENDCOLOR] Promotion.[NEWLINE][NEWLINE]Must have built a Wire Service in the City. Requires a national population of at least 40 before it can be constructed. The cost goes up the more cities there are in the empire.'
WHERE Tag = 'TXT_KEY_BUILDING_FOREIGN_OFFICE_HELP' AND EXISTS (SELECT * FROM CSD WHERE Type='COMMUNITY_CORE_BALANCE_NATIONAL_WONDERS_CSD' AND Value= 1 );
