ALTER TABLE Units
  ADD MaxHitPoints INTEGER DEFAULT 100;
  
ALTER TABLE UnitPromotions
  ADD MaxHitPointsChange INTEGER DEFAULT 0;

ALTER TABLE UnitPromotions
  ADD MaxHitPointsModifier INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('UNITS_MAX_HP', 1);
