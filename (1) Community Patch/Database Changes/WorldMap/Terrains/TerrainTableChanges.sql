-- Turn damage taken on terrain unless unit has a promotion with IgnoreTerrainDamage = true
ALTER TABLE Terrains ADD TurnDamage integer DEFAULT 0;

-- Turn damage taken on terrain for unit that has a promotion with ExtraTerrainDamage = true
ALTER TABLE Terrains ADD ExtraTurnDamage integer DEFAULT 0;

-- Tech that makes an impassable terrain passable.
ALTER TABLE Terrains ADD PassableTechTerrain text REFERENCES Technologies (Type);

-- Free promotion when eligible unit enters a plot of this terrain
ALTER TABLE Terrains ADD LocationUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Same as above, but for entering a plot adjacent to a terrain type.
ALTER TABLE Terrains ADD AdjacentUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Free promotion when eligible unit spawns on a plot of this terrain. Generally used for barbarians.
ALTER TABLE Terrains ADD SpawnLocationUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Same as above, but for spawning adjacent to a terrain type.
ALTER TABLE Terrains ADD AdjacentSpawnLocationUnitFreePromotion text REFERENCES UnitPromotions (Type);
