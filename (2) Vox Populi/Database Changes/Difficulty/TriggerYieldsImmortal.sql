CREATE TEMP TABLE TriggerYieldAmounts (
	EraType text,
	HistoricEventType text,
	YieldType text,
	Amount integer
);

INSERT INTO TriggerYieldAmounts
	(EraType, HistoricEventType, YieldType, Amount)
VALUES
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_FOOD', 182),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_GOLD', 182),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_CULTURE', 182),
	('ERA_ANCIENT', 'HISTORIC_EVENT_WON_WAR', 'YIELD_SCIENCE', 182);

INSERT INTO HandicapInfo_AIDifficultyBonus
	(HandicapType, EraType, HistoricEventType, YieldType, Amount)
SELECT
	'HANDICAP_IMMORTAL', EraType, HistoricEventType, YieldType, Amount
FROM TriggerYieldAmounts;

DROP TABLE TriggerYieldAmounts;