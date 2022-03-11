ALTER TABLE Terrains
  ADD TurnDamage INTEGER DEFAULT 0;

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