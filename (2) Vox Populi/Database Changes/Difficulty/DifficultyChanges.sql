CREATE TEMP Table TriggerYields (
	HistoricEventType text,
	YieldType text
);

CREATE TEMP Table DifficultyBonusAmounts (
	HandicapType text,
	EraType text,
	Amount INTEGER
);

-- Yield types given for each type of difficulty bonus trigger
INSERT INTO TriggerYields
	(HistoricEventType, YieldType)
VALUES
	('HISTORIC_EVENT_WON_WAR', 'YIELD_FOOD'),
	('HISTORIC_EVENT_WON_WAR', 'YIELD_GOLD'),
	('HISTORIC_EVENT_WON_WAR', 'YIELD_CULTURE'),
	('HISTORIC_EVENT_WON_WAR', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_CITY_CONQUEST', 'YIELD_FOOD'),
	('DIFFICULTY_BONUS_CITY_CONQUEST', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_CITY_CONQUEST', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_CITY_CONQUEST', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_CITY_FOUND', 'YIELD_FOOD'),
	('DIFFICULTY_BONUS_CITY_FOUND', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_CITY_FOUND', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_CITY_FOUND', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT', 'YIELD_SCIENCE'),
	('HISTORIC_EVENT_GOLDEN_AGE', 'YIELD_GOLD'),
	('HISTORIC_EVENT_GOLDEN_AGE', 'YIELD_CULTURE'),
	('HISTORIC_EVENT_GOLDEN_AGE', 'YIELD_SCIENCE'),
	('HISTORIC_EVENT_WORLD_WONDER', 'YIELD_GOLD'),
	('HISTORIC_EVENT_WORLD_WONDER', 'YIELD_CULTURE'),
	('HISTORIC_EVENT_WORLD_WONDER', 'YIELD_SCIENCE'),
	('DIFFICULTY_BONUS_RESEARCHED_TECH', 'YIELD_FOOD'),
	('DIFFICULTY_BONUS_RESEARCHED_TECH', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_RESEARCHED_TECH', 'YIELD_CULTURE'),
	('DIFFICULTY_BONUS_ADOPTED_POLICY', 'YIELD_FOOD'),
	('DIFFICULTY_BONUS_ADOPTED_POLICY', 'YIELD_GOLD'),
	('DIFFICULTY_BONUS_ADOPTED_POLICY', 'YIELD_SCIENCE'),
	('HISTORIC_EVENT_DIG', 'YIELD_FOOD'),
	('HISTORIC_EVENT_DIG', 'YIELD_GOLD'),
	('HISTORIC_EVENT_GREAT_PERSON', 'YIELD_FOOD'),
	('HISTORIC_EVENT_GREAT_PERSON', 'YIELD_GOLD'),
	('HISTORIC_EVENT_TRADE_LAND', 'YIELD_FOOD'),
	('HISTORIC_EVENT_TRADE_LAND', 'YIELD_GOLD'),
	('HISTORIC_EVENT_TRADE_SEA', 'YIELD_FOOD'),
	('HISTORIC_EVENT_TRADE_SEA', 'YIELD_GOLD'),
	('HISTORIC_EVENT_TRADE_CS', 'YIELD_FOOD'),
	('HISTORIC_EVENT_TRADE_CS', 'YIELD_GOLD');

-- Base amounts by Difficulty Level and Era
INSERT INTO DifficultyBonusAmounts
	(HandicapType, EraType, Amount)
VALUES
	-- PRINCE
	('HANDICAP_PRINCE', 'ERA_ANCIENT', 24),
	('HANDICAP_PRINCE', 'ERA_CLASSICAL', 24),
	('HANDICAP_PRINCE', 'ERA_MEDIEVAL', 44),
	('HANDICAP_PRINCE', 'ERA_RENAISSANCE', 92),
	('HANDICAP_PRINCE', 'ERA_INDUSTRIAL', 107),
	('HANDICAP_PRINCE', 'ERA_MODERN', 150),
	('HANDICAP_PRINCE', 'ERA_POSTMODERN', 202),
	('HANDICAP_PRINCE', 'ERA_FUTURE', 262),

	-- KING
	('HANDICAP_KING', 'ERA_ANCIENT', 46),
	('HANDICAP_KING', 'ERA_CLASSICAL', 46),
	('HANDICAP_KING', 'ERA_MEDIEVAL', 87),
	('HANDICAP_KING', 'ERA_RENAISSANCE', 189),
	('HANDICAP_KING', 'ERA_INDUSTRIAL', 220),
	('HANDICAP_KING', 'ERA_MODERN', 312),
	('HANDICAP_KING', 'ERA_POSTMODERN', 421),
	('HANDICAP_KING', 'ERA_FUTURE', 548),

	-- EMPEROR
	('HANDICAP_EMPEROR', 'ERA_ANCIENT', 72),
	('HANDICAP_EMPEROR', 'ERA_CLASSICAL', 72),
	('HANDICAP_EMPEROR', 'ERA_MEDIEVAL', 139),
	('HANDICAP_EMPEROR', 'ERA_RENAISSANCE', 304),
	('HANDICAP_EMPEROR', 'ERA_INDUSTRIAL', 359),
	('HANDICAP_EMPEROR', 'ERA_MODERN', 512),
	('HANDICAP_EMPEROR', 'ERA_POSTMODERN', 695),
	('HANDICAP_EMPEROR', 'ERA_FUTURE', 906),

	-- IMMORTAL
	('HANDICAP_IMMORTAL', 'ERA_ANCIENT', 91),
	('HANDICAP_IMMORTAL', 'ERA_CLASSICAL', 91),
	('HANDICAP_IMMORTAL', 'ERA_MEDIEVAL', 177),
	('HANDICAP_IMMORTAL', 'ERA_RENAISSANCE', 393),
	('HANDICAP_IMMORTAL', 'ERA_INDUSTRIAL', 465),
	('HANDICAP_IMMORTAL', 'ERA_MODERN', 667),
	('HANDICAP_IMMORTAL', 'ERA_POSTMODERN', 907),
	('HANDICAP_IMMORTAL', 'ERA_FUTURE', 1185),

	-- DEITY
	('HANDICAP_DEITY', 'ERA_ANCIENT', 114),
	('HANDICAP_DEITY', 'ERA_CLASSICAL', 114),
	('HANDICAP_DEITY', 'ERA_MEDIEVAL', 226),
	('HANDICAP_DEITY', 'ERA_RENAISSANCE', 504),
	('HANDICAP_DEITY', 'ERA_INDUSTRIAL', 601),
	('HANDICAP_DEITY', 'ERA_MODERN', 864),
	('HANDICAP_DEITY', 'ERA_POSTMODERN', 1178),
	('HANDICAP_DEITY', 'ERA_FUTURE', 1542);

-- Fill the table with entries
INSERT INTO HandicapInfo_AIDifficultyBonus
	(HandicapType, EraType, HistoricEventType, YieldType, Amount)
SELECT
	a.HandicapType, a.EraType, b.HistoricEventType, b.YieldType, a.Amount
FROM DifficultyBonusAmounts a, TriggerYields b;

DROP TABLE TriggerYields;
DROP TABLE DifficultyBonusAmounts;

-- Multipliers for specific triggers
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 200 / 100 WHERE HistoricEventType = 'HISTORIC_EVENT_WON_WAR'; -- 2x for winning a war
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 150 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_CITY_CONQUEST'; -- 1.5x for city conquest
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 33 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY'; -- 0.33x for adopting a policy
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 20 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH'; -- 0.2x for researching a tech
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 300 / 100 WHERE HistoricEventType = 'HISTORIC_EVENT_GOLDEN_AGE' AND YieldType = 'YIELD_GOLD'; -- 3x Gold for Golden Ages
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 300 / 100 WHERE HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_GOLD'; -- 3x Gold for World Wonders
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 20 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_MAJOR_UNIT' AND YieldType = 'YIELD_GOLD'; -- 0.2x Gold for killing a major civ unit
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 10 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_MAJOR_UNIT' AND YieldType <> 'YIELD_GOLD'; -- 0.1x other yields for killing a major civ unit
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 20 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT' AND YieldType = 'YIELD_GOLD'; -- 0.2x Gold for killing a City-State unit
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 10 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT' AND YieldType <> 'YIELD_GOLD'; -- 0.1x other yields for killing a City-State unit
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 10 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT' AND YieldType = 'YIELD_GOLD'; -- 0.1x Gold for killing a Barbarian unit
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = Amount * 5 / 100 WHERE HistoricEventType = 'DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT' AND YieldType <> 'YIELD_GOLD'; -- 0.05x other yields for killing a Barbarian unit

-- Exceptions
-- Reduce Science and Culture in Ancient Era on all difficulties
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 20 WHERE HandicapType = 'HANDICAP_PRINCE' AND HistoricEventType = 'DIFFICULTY_BONUS_CITY_FOUND' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 20 WHERE HandicapType = 'HANDICAP_PRINCE' AND HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 6  WHERE HandicapType = 'HANDICAP_PRINCE' AND HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 3  WHERE HandicapType = 'HANDICAP_PRINCE' AND HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH' AND YieldType = 'YIELD_CULTURE' AND EraType = 'ERA_ANCIENT';

UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 41 WHERE HandicapType = 'HANDICAP_KING' AND HistoricEventType = 'DIFFICULTY_BONUS_CITY_FOUND' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 41 WHERE HandicapType = 'HANDICAP_KING' AND HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 12 WHERE HandicapType = 'HANDICAP_KING' AND HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 7  WHERE HandicapType = 'HANDICAP_KING' AND HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH' AND YieldType = 'YIELD_CULTURE' AND EraType = 'ERA_ANCIENT';

UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 64 WHERE HandicapType = 'HANDICAP_EMPEROR' AND HistoricEventType = 'DIFFICULTY_BONUS_CITY_FOUND' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 64 WHERE HandicapType = 'HANDICAP_EMPEROR' AND HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 18 WHERE HandicapType = 'HANDICAP_EMPEROR' AND HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 11 WHERE HandicapType = 'HANDICAP_EMPEROR' AND HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH' AND YieldType = 'YIELD_CULTURE' AND EraType = 'ERA_ANCIENT';

UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 81 WHERE HandicapType = 'HANDICAP_IMMORTAL' AND HistoricEventType = 'DIFFICULTY_BONUS_CITY_FOUND' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 81 WHERE HandicapType = 'HANDICAP_IMMORTAL' AND HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 23 WHERE HandicapType = 'HANDICAP_IMMORTAL' AND HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 14 WHERE HandicapType = 'HANDICAP_IMMORTAL' AND HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH' AND YieldType = 'YIELD_CULTURE' AND EraType = 'ERA_ANCIENT';

UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 101 WHERE HandicapType = 'HANDICAP_DEITY' AND HistoricEventType = 'DIFFICULTY_BONUS_CITY_FOUND' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 101 WHERE HandicapType = 'HANDICAP_DEITY' AND HistoricEventType = 'HISTORIC_EVENT_WORLD_WONDER' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 29 WHERE HandicapType = 'HANDICAP_DEITY' AND HistoricEventType = 'DIFFICULTY_BONUS_ADOPTED_POLICY' AND YieldType = 'YIELD_SCIENCE' AND EraType = 'ERA_ANCIENT';
UPDATE HandicapInfo_AIDifficultyBonus SET Amount = 17 WHERE HandicapType = 'HANDICAP_DEITY' AND HistoricEventType = 'DIFFICULTY_BONUS_RESEARCHED_TECH' AND YieldType = 'YIELD_CULTURE' AND EraType = 'ERA_ANCIENT';