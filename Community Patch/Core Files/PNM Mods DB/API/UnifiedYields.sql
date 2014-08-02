-- DO NOT DELETE THIS FILE!!!

ALTER TABLE Yields ADD ImageTexture TEXT DEFAULT 'YieldAtlas.dds';
ALTER TABLE Yields ADD ImageOffset INTEGER DEFAULT 0;

UPDATE Yields SET ImageOffset=128 WHERE Type='YIELD_PRODUCTION';
UPDATE Yields SET ImageOffset=256 WHERE Type='YIELD_GOLD';
UPDATE Yields SET ImageOffset=384 WHERE Type='YIELD_SCIENCE';
UPDATE Yields SET ImageTexture='YieldAtlas_128_Culture.dds' WHERE Type='YIELD_CULTURE';
UPDATE Yields SET ImageTexture='YieldAtlas_128_Faith.dds' WHERE Type='YIELD_FAITH';

