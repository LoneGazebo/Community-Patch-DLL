ALTER TABLE Terrains
  ADD TurnDamage INTEGER DEFAULT 0;
UPDATE Terrains
  SET TurnDamage=50 WHERE Type='TERRAIN_MOUNTAIN';

ALTER TABLE UnitPromotions
  ADD IgnoreTerrainDamage INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions
  ADD IgnoreFeatureDamage INTEGER DEFAULT 0;

ALTER TABLE Terrains
  ADD ExtraTurnDamage INTEGER DEFAULT 0;
ALTER TABLE Features
  ADD ExtraTurnDamage INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions
  ADD ExtraTerrainDamage INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions
  ADD ExtraFeatureDamage INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('API_PLOT_BASED_DAMAGE', 1);
