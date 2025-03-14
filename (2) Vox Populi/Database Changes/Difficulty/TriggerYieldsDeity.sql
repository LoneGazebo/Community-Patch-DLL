CREATE TEMP TABLE TriggerYieldAmounts (
	EraType text,
	HistoricEventType text,
	YieldType text,
	Amount integer
);

INSERT INTO TriggerYieldAmounts
	(EraType, HistoricEventType, YieldType, Amount)
VALUES
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_FOOD', 228),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_GOLD', 228),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_CULTURE', 228),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_SCIENCE', 228);

INSERT INTO HandicapInfo_AIDifficultyBonus
	(HandicapType, EraType, HistoricEventType, YieldType, Amount)
SELECT
	'HANDICAP_DEITY', EraType, HistoricEventType, YieldType, Amount
FROM TriggerYieldAmounts;

DROP TABLE TriggerYieldAmounts;