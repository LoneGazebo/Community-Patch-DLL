-- New Goody Hut rewards
CREATE TEMP TABLE GoodyHutRewards (
	GoodyType text
);

INSERT INTO GoodyHutRewards
	(GoodyType)
VALUES
	('GOODY_POPULATION'),
	('GOODY_CULTURE'),
	('GOODY_PANTHEON_FAITH'),
	('GOODY_PROPHET_FAITH'),
	('GOODY_GOLD'),
	('GOODY_MAP'),
	('GOODY_UPGRADE_UNIT'),
	('GOODY_PRODUCTION'),
	('GOODY_GOLDEN_AGE'),
	('GOODY_TILES'),
	('GOODY_SCIENCE');

-- Settler-exclusive rewards
INSERT INTO HandicapInfo_Goodies
	(HandicapType, GoodyType)
VALUES
	('HANDICAP_SETTLER', 'GOODY_WORKER'),
	('HANDICAP_SETTLER', 'GOODY_SETTLER');

-- General rewards
INSERT INTO HandicapInfo_Goodies
	(HandicapType, GoodyType)
SELECT
	a.Type, b.GoodyType
FROM HandicapInfos a, GoodyHutRewards b;

DROP TABLE GoodyHutRewards;

CREATE TEMP TABLE TriggerYields (
	HistoricEventType text,
	YieldType text
);

CREATE TEMP TABLE DifficultyBonusAmounts (
	HandicapType text,
	EraType text,
	Amount integer
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

-- Adjustments start here
-- First multiply by 100 to avoid rounding errors
UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * 100
WHERE Amount > 0;

-- Multipliers for specific triggers
CREATE TEMP TABLE DifficultyBonusMultipliers (
	HistoricEventTypeTemp text,
	IsGold boolean,
	IsNotGold boolean,
	IsFoodOrGold boolean,
	IsFoodOrGoldBeforeRenaissance boolean,
	IsAny boolean,
	MultiplierTimes100 integer
);

INSERT INTO DifficultyBonusMultipliers
	(HistoricEventTypeTemp, IsGold, IsNotGold, MultiplierTimes100)
VALUES
	('HISTORIC_EVENT_GOLDEN_AGE', 1, 0, 300), -- 3x Gold for Golden Ages
	('HISTORIC_EVENT_WORLD_WONDER', 1, 0, 300), -- 3x Gold for World Wonders
	('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT', 1, 0, 20), -- 0.2x Gold for killing a major civ unit
	('DIFFICULTY_BONUS_KILLED_MAJOR_UNIT', 0, 1, 10), -- 0.1x other yields for killing a major civ unit
	('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT', 1, 0, 20), -- 0.2x Gold for killing a City-State unit
	('DIFFICULTY_BONUS_KILLED_CITY_STATE_UNIT', 0, 1, 10), -- 0.1x other yields for killing a City-State unit
	('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT', 1, 0, 10), -- 0.1x Gold for killing a Barbarian unit
	('DIFFICULTY_BONUS_KILLED_BARBARIAN_UNIT', 0, 1, 5); -- 0.05x other yields for killing a Barbarian unit

INSERT INTO DifficultyBonusMultipliers
	(HistoricEventTypeTemp, IsAny, MultiplierTimes100)
VALUES
	('HISTORIC_EVENT_WON_WAR', 1, 200), -- 2x for winning a war
	('DIFFICULTY_BONUS_CITY_CONQUEST', 1, 150), -- 1.5x for city conquest
	('DIFFICULTY_BONUS_ADOPTED_POLICY', 1, 33), -- 0.33x for adopting a policy
	('DIFFICULTY_BONUS_RESEARCHED_TECH', 1, 20); -- 0.2x for researching a tech

INSERT INTO DifficultyBonusMultipliers
	(HistoricEventTypeTemp, IsFoodOrGold, IsFoodOrGoldBeforeRenaissance, MultiplierTimes100)
VALUES
	('DIFFICULTY_BONUS_CITY_FOUND', 1, 0, 200), -- 2x Food and Gold for founding a city
	('DIFFICULTY_BONUS_CITY_FOUND', 0, 1, 0); -- 0x Food and Gold for founding a city before Renaissance Era

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsGold = 1) / 100
WHERE YieldType = 'YIELD_GOLD'
AND EXISTS (SELECT 1 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsGold = 1);

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsNotGold = 1) / 100
WHERE YieldType <> 'YIELD_GOLD'
AND EXISTS (SELECT 1 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsNotGold = 1);

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsFoodOrGold = 1) / 100
WHERE YieldType IN ('YIELD_FOOD', 'YIELD_GOLD')
AND EXISTS (SELECT 1 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsFoodOrGold = 1);

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsFoodOrGoldBeforeRenaissance = 1) / 100
WHERE YieldType IN ('YIELD_FOOD', 'YIELD_GOLD') AND EraType IN (
	SELECT Type FROM Eras WHERE ID < (SELECT ID FROM Eras WHERE Type = 'ERA_RENAISSANCE')
)
AND EXISTS (SELECT 1 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsFoodOrGoldBeforeRenaissance = 1);

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsAny = 1) / 100
WHERE EXISTS (SELECT 1 FROM DifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND IsAny = 1);

DROP TABLE DifficultyBonusMultipliers;

-- Reduce Science and Culture in Ancient Era on all difficulties
CREATE TEMP TABLE AncientDifficultyBonusMultipliers (
	HistoricEventTypeTemp text,
	YieldTypeTemp text,
	MultiplierTimes100 integer
);

INSERT INTO AncientDifficultyBonusMultipliers
VALUES
	('DIFFICULTY_BONUS_CITY_FOUND', 'YIELD_SCIENCE', 89), -- 0.89x Science for founding a city
	('HISTORIC_EVENT_WORLD_WONDER', 'YIELD_SCIENCE', 89), -- 0.89x Science for World Wonders
	('DIFFICULTY_BONUS_ADOPTED_POLICY', 'YIELD_SCIENCE', 78), -- 0.78x Science for adopting a policy
	('DIFFICULTY_BONUS_RESEARCHED_TECH', 'YIELD_CULTURE', 79); -- 0.79x Culture for researching a tech

UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount * (SELECT MultiplierTimes100 FROM AncientDifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND YieldTypeTemp = YieldType) / 100
WHERE EraType = 'ERA_ANCIENT'
AND EXISTS (SELECT 1 FROM AncientDifficultyBonusMultipliers WHERE HistoricEventTypeTemp = HistoricEventType AND YieldTypeTemp = YieldType);

DROP TABLE AncientDifficultyBonusMultipliers;

-- Divide by 100 at the end
UPDATE HandicapInfo_AIDifficultyBonus
SET Amount = Amount / 100
WHERE Amount > 0;
