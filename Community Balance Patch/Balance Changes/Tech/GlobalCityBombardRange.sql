ALTER TABLE Technologies ADD BombardRange INTEGER DEFAULT 0;
ALTER TABLE Technologies ADD BombardIndirect INTEGER DEFAULT 0;

UPDATE Technologies SET BombardRange=1 WHERE Type='TECH_AGRICULTURE';
UPDATE Technologies SET BombardRange=2 WHERE Type='TECH_MATHEMATICS';
UPDATE Technologies SET BombardRange=2, BombardIndirect=1 WHERE Type='TECH_GUNPOWDER';
UPDATE Technologies SET BombardRange=3, BombardIndirect=1 WHERE Type='TECH_DYNAMITE';

INSERT OR REPLACE INTO Defines(Name, Value) SELECT 'MAX_CITY_ATTACK_RANGE', max(BombardRange) FROM Technologies;

UPDATE CustomModOptions SET Value=1 WHERE Name='EVENTS_CITY_BOMBARD';
