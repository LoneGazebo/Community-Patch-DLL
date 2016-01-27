-- Note this file does nothing right now. Yes the table is created. It does nothing.

CREATE TABLE VassalageBenefits
(
	ID integer PRIMARY KEY AUTOINCREMENT,
	Type text NOT NULL UNIQUE,
	Description text,
	ShortDescription text,
	ExtraReligiousPressurePercent integer,		-- Extra pressure percent
	ExtraReligiousConversionPercent integer,	-- Extra conversion percent (missionaries and prophets)
	CulturePerTurnPercent integer,				-- Vassal's CulturePerTurn given to Master
	GoldPerTurnPercent integer,					-- Vassal's GPT given to Master
	SciencePerTurnPercent integer,				-- Vassal's Science given to Master
	FoodPercentSpreadToCities integer,			-- Vassal's Food Percent divided between Master's cities
	DoubleHealRate integer,						-- Double Heal Rate in Vassal's lands (0 or 1)
	GiveUpLuxuries integer,						-- Give up all Vassal's Luxuries (0 or 1)
	FreeMilitaryUnitTurns integer,				-- Turns between free Vassal Military Units
)

INSERT INTO VassalageBenefits
(		ID,	Type,										Description,								ShortDescription,									ExtraReligiousPressurePercent,	ExtraReligiousConversionPercent,	CulturePerTurnPercent,	GoldPerTurnPercent,	FoodPercentSpreadToCities,	DoubleHealRate,	GiveUpLuxuries,	FreeMilitaryUnitTurns)
SELECT	0,	'VASSAL_BENEFIT_TAXATION'					'TXT_KEY_VASSAL_BENEFIT_TAXATION'			'TXT_KEY_VASSAL_BENEFIT_TAXATION_SHORT'				0,								0,									0,						33,					0,							0,				0,				0,						UNION ALL
SELECT	1,	'VASSAL_BENEFIT_SERFDOM'					'TXT_KEY_VASSAL_BENEFIT_SERFDOM'			'TXT_KEY_VASSAL_BENEFIT_SERFDOM_SHORT'				0,								0,									0,						0,					25,							0,				0,				0,						UNION ALL
SELECT	2,	'VASSAL_BENEFIT_FORCED_CONVERSION'			'TXT_KEY_VASSAL_BENEFIT_FORCED_CONVERSION'	'TXT_KEY_VASSAL_BENEFIT_FORCED_CONVERSION_SHORT'	50,								50,									0,						0,					0,							0,				0,				0,						UNION ALL
SELECT	3,	'VASSAL_BENEFIT_CULTURAL_DIFFUSION'			'TXT_KEY_VASSAL_BENEFIT_CULTURAL_DIFFUSION'	'TXT_KEY_VASSAL_BENEFIT_CULTURAL_DIFFUSION_SHORT'	0,								0,									25,						0,					0,							0,				0,				0,						UNION ALL
SELECT	4,	'VASSAL_BENEFIT_FIELD_MEDICINE'				'TXT_KEY_VASSAL_BENEFIT_FIELD_MEDICINE'		'TXT_KEY_VASSAL_BENEFIT_FIELD_MEDICINE_SHORT'		0,								0,									0,						0,					0,							1,				0,				0,						UNION ALL
SELECT	5,	'VASSAL_BENEFIT_CONSCRIPTION'				'TXT_KEY_VASSAL_BENEFIT_CONSCRIPTION'		'TXT_KEY_VASSAL_BENEFIT_CONSCRIPTION_SHORT'			0,								0,									0,						0,					0,							0,				0,				1,						UNION ALL
SELECT	6,	'VASSAL_BENEFIT_EXPLOITATION'				'TXT_KEY_VASSAL_BENEFIT_EXPLOITATION'		'TXT_KEY_VASSAL_BENEFIT_EXPLOITATION_SHORT'			0,								0,									0,						0,					0,							0,				1,				0;