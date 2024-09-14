-- Turn damage taken on feature for unit that has a promotion with ExtraFeatureDamage = true
ALTER TABLE Features ADD ExtraTurnDamage integer DEFAULT 0;

-- Tech that makes an impassable feature passable.
ALTER TABLE Features ADD PassableTechFeature text REFERENCES Technologies (Type);

-- Promotion to all eligible units when this feature is in owned territory. Applies to new and existing units.
ALTER TABLE Features ADD FreePromotionIfOwned text REFERENCES UnitPromotions (Type);

-- Free promotion when eligible unit enters a plot of this feature
ALTER TABLE Features ADD LocationUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Free promotion when eligible unit spawns on a plot of this feature. Generally used for barbarians.
ALTER TABLE Features ADD SpawnLocationUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Same as above, but for spawning adjacent to a feature type.
ALTER TABLE Features ADD AdjacentSpawnLocationUnitFreePromotion text REFERENCES UnitPromotions (Type);

-- Features with NaturalWonder = true have hardcoded graphics. This column exists to allow additional NWs with working graphics.
ALTER TABLE Features ADD PseudoNaturalWonder boolean DEFAULT 0;
