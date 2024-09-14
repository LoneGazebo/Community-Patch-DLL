-- Treat mountain as a normal tile which is simply impassable without the proper trait/promotion.
-- Note: +2 will be added to see from and see through values for mountain (see Defines). Many other behaviors are also hardcoded in the DLL.
UPDATE Terrains
SET
	Movement = 3,
	Defense = 25,
	Impassable = 1,
	SeeFrom = 1,
	SeeThrough = 1,
	Water = 0,
	Found = 1
WHERE Type = 'TERRAIN_MOUNTAIN';
