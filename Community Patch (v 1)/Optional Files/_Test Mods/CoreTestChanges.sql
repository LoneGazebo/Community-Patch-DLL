UPDATE Beliefs
SET RequiresResource = 'true'
WHERE Type = 'BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Beliefs
SET RequiresImprovement = 'true'
WHERE Type = 'BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Beliefs
SET RequiresImprovement = 'true'
WHERE Type = 'BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Beliefs
SET RequiresResource = 'true'
WHERE Type = 'BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Buildings
SET Mountain='false', NearbyMountainRequired='1' 
WHERE Type = 'BUILDING_OBSERVATORY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Tundra tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DANCE_AURORA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PEACE] Faith from Desert tiles with improved resources'
WHERE Tag = 'TXT_KEY_BELIEF_DESERT_FOLKLORE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'City must be built within two tiles of a Mountain.'
WHERE Tag = 'TXT_KEY_BUILDING_OBSERVATORY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Natural_Wonder_Placement
SET AdjacentTilesCareAboutTerrainTypes ='true' 
WHERE NaturalWonderType = 'FEATURE_VOLCANO' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '0'
WHERE Type = 'ERA_ANCIENT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '0'
WHERE Type = 'ERA_CLASSICAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '1'
WHERE Type = 'ERA_MEDIEVAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '1'
WHERE Type = 'ERA_RENAISSANCE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '2'
WHERE Type = 'ERA_INDUSTRIAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '2'
WHERE Type = 'ERA_MODERN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '3'
WHERE Type = 'ERA_POSTMODERN' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Eras
Set StartingMinorDefenseUnits = '3'
WHERE Type = 'ERA_FUTURE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
Set StartingMinorDefenseUnits = '1'
WHERE Type = 'HANDICAP_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
Set StartingMinorDefenseUnits = '2'
WHERE Type = 'HANDICAP_EMPEROR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
Set StartingMinorDefenseUnits = '2'
WHERE Type = 'HANDICAP_IMMORTAL' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE HandicapInfos
Set StartingMinorDefenseUnits = '3'
WHERE Type = 'HANDICAP_DEITY' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production from Fishing Boats. 1 Great Admiral Point per turn'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_SEA' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Use [ICON_PEACE] Faith to purchase pre-Industrial land units. 1 Great General Point per turn'
WHERE Tag = 'TXT_KEY_BELIEF_HOLY_WARRIORS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Palace provides +1 to these yields: [ICON_CULTURE] , [ICON_PEACE] , [ICON_GOLD], [ICON_PRODUCTION], [ICON_RESEARCH], [ICON_GOLDEN_AGE]'
WHERE Tag = 'TXT_KEY_BELIEF_GOD_KING' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Piety[ENDCOLOR] increases the [ICON_PEACE] Faith of empires.[NEWLINE][NEWLINE]Adopting Piety allows you to build Shrines and Temples in half the usual time and you receive a free Shrine in your first four cities. Unlocks building the Great Mosque of Djenne.[NEWLINE][NEWLINE]Adopting all Policies in the Piety tree causes a Great Prophet to appear and Holy Sites provide +3 [ICON_CULTURE] Culture.'
WHERE Tag = 'TXT_KEY_POLICY_BRANCH_PIETY_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Warrior Code[ENDCOLOR][NEWLINE]+15% [ICON_PRODUCTION] Production when training Melee units and a Great General appears outside the [ICON_CAPITAL] Capital. Great Generals are earned 50% faster. Receive a free Barracks in your first four cities.'
WHERE Tag = 'TXT_KEY_POLICY_WARRIOR_CODE_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Cultural Centers[ENDCOLOR][NEWLINE]Construct Monuments, Amphitheaters, Opera Houses, Museums and Broadcast Towers 50% faster. Receive 3 [ICON_GOLDEN_AGE] Golden Age points per turn.'
WHERE Tag = 'TXT_KEY_POLICY_CULTURAL_CENTERS_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Military Tradition[ENDCOLOR][NEWLINE]Military Units gain 50% more Experience from combat. 1 Great General Point per turn.'
WHERE Tag = 'TXT_KEY_POLICY_MILITARY_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Naval Tradition[ENDCOLOR][NEWLINE]+1 [ICON_HAPPINESS_1] Happiness for each Harbor, Seaport, or Lighthouse. 1 Great Admiral Point per turn.'
WHERE Tag = 'TXT_KEY_POLICY_NAVAL_TRADITION_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE' AND Value= 1 );