-- Ahmad -- Boost Kasbah, Ability
UPDATE Improvement_Yields
SET Yield = '2'
WHERE YieldType = 'YIELD_GOLD' AND ImprovementType = 'IMPROVEMENT_KASBAH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Receives +3 Gold [ICON_GOLD], +1 [ICON_CULTURE] Culture, and +1 [ICON_GOLDEN_AGE] Golden Age Point for each Trade Route with a different civ or City-State. The Trade Route owners receive +2 [ICON_GOLD] Gold for each Trade Route sent to Morocco.'
WHERE Tag = 'TXT_KEY_TRAIT_GATEWAY_AFRICA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Ashurbanipal -- Boost Royal Library
UPDATE Building_DomainFreeExperiencePerGreatWork
SET Experience = '15'
WHERE BuildingType = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET GreatWorkCount = '2'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistCount = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET SpecialistType = 'SPECIALIST_SCIENTIST'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

UPDATE Buildings
SET IlliteracyHappinessChange = '1'
WHERE Type = 'BUILDING_ROYAL_LIBRARY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Unique Assyrian Library replacement. +1 [ICON_RESEARCH] Science for every 2 [ICON_CITIZEN] Citizens in this City. Reduces [ICON_HAPPINESS_3] Illiteracy. Also has a slot for a Great Work of Writing which, when filled, gives extra XP to trained Units.'
WHERE Tag = 'TXT_KEY_BUILDING_ROYAL_LIBRARY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Ashurbanipal -- Earlier Siege Tower

UPDATE Units
SET PrereqTech = 'TECH_ARCHERY'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Units
SET Cost = '65'
WHERE Type = 'UNIT_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_UNITS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Powerful melee siege unit which grants a city attack bonus to nearby units when next to an enemy city. Available at Military Training instead of Mathematics. May only be built by the Assyrians.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ASSYRIAN_SIEGE_TOWER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Enrico - Unique Building (Doge's Palace)
DELETE FROM Units
WHERE Type = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Civilization_UnitClassOverrides
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_AITypes
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_ClassUpgrades
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_FreePromotions
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM Unit_Flavors
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

DELETE FROM UnitGameplay2DScripts
WHERE UnitType = 'UNIT_VENETIAN_GALLEASS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LEADERS' AND Value= 1 );

-- Gajah Mada -- Boost Candi, No Isolation Unhappiness
UPDATE Language_en_US
SET Text = 'Unique Indonesian Garden replacement. +25% [ICON_GREAT_PEOPLE] Great People generation in this City, and +2 [ICON_PEACE] Faith for each World Religion that has at least 1 follower in the city. Receive 30 [ICON_CULTURE] every time this city increases its [ICON_CITIZEN] population. [NEWLINE][NEWLINE]Unlike the Garden the Candi does not have to be built next to a River or Lake.'
WHERE Tag = 'TXT_KEY_BUILDING_CANDI_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

UPDATE Traits
SET NoConnectionUnhappiness = 'true'
WHERE Type = 'TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The first 3 cities founded on continents other than where Indonesia started each provide 2 unique Luxury Resources (and can never be razed). No [ICON_HAPPINESS_3] Unhappiness from Isolation.'
WHERE Tag = 'TXT_KEY_TRAIT_SPICE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );

UPDATE Buildings
SET Cost = '100'
WHERE Type = 'BUILDING_CANDI' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_BUILDINGS' AND Value= 1 );

-- Casimir III -- Ducal Stable +1 Production
UPDATE Language_en_US
SET Text = 'Unique Polish Stable replacement. +15% [ICON_PRODUCTION] Production and  +15 XP for Mounted Units.[NEWLINE][NEWLINE]Each Pasture worked by this City produces +1 [ICON_PRODUCTION] Production and +1 [ICON_GOLD] Gold, and the City itself produces +1 [ICON_PRODUCTION] Production.'
WHERE Tag = 'TXT_KEY_BUILDING_DUCAL_STABLE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );