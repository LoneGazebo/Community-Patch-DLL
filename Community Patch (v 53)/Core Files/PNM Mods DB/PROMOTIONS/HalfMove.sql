ALTER TABLE UnitPromotions_Terrains
  ADD HalfMove INTEGER DEFAULT 0;
ALTER TABLE UnitPromotions_Features
  ADD HalfMove INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('PROMOTIONS_HALF_MOVE', 1);
