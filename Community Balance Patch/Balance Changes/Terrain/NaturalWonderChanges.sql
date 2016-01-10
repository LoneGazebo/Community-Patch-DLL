UPDATE Language_en_US
SET Text = 'You have found {@1_Name}!'
WHERE Tag = 'TXT_KEY_FOUND_NATURAL_WONDER' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CIVS' AND Value= 1 );

UPDATE Features SET Impassable=0, Movement=2, Defense=10 WHERE Type IN (
	'FEATURE_CRATER'			,
	'FEATURE_GEYSER'			,
	'FEATURE_FOUNTAIN_YOUTH'	,
	'FEATURE_EL_DORADO'			,
	'FEATURE_MESA'				,
	'FEATURE_REEF'				,
	'FEATURE_ULURU'				,
	'FEATURE_SOLOMONS_MINES'	
);

UPDATE Natural_Wonder_Placement SET ChangeCoreTileToMountain=0 WHERE NaturalWonderType IN (
	'FEATURE_CRATER'			,
	'FEATURE_GEYSER'			,
	'FEATURE_FOUNTAIN_YOUTH'	,
	'FEATURE_EL_DORADO'			,
	'FEATURE_MESA'				,
	'FEATURE_REEF'				,
	'FEATURE_ULURU'				
);

UPDATE Features SET Impassable=1 WHERE Type IN (
	'FEATURE_LAKE_VICTORIA'		
);

UPDATE Natural_Wonder_Placement SET ChangeCoreTileToMountain=1 WHERE NaturalWonderType IN (
	'FEATURE_LAKE_VICTORIA'		
);


UPDATE Natural_Wonder_Placement SET OccurrenceFrequency = 10 WHERE OccurrenceFrequency <> 0;

-- More unique than the numerous mountains
UPDATE Natural_Wonder_Placement SET OccurrenceFrequency = 20 WHERE NaturalWonderType IN (
	'FEATURE_GIBRALTAR'		,
	'FEATURE_REEF'			,
	'FEATURE_VOLCANO'		,
	'FEATURE_GEYSER'		,
	'FEATURE_MESA'			,
	'FEATURE_FOUNTAIN_YOUTH',
	'FEATURE_LAKE_VICTORIA'	
);