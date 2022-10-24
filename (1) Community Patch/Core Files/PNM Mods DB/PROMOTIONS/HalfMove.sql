-- unit movement cost is doubled in the given terrain / feature
ALTER TABLE UnitPromotions_Terrains
  ADD HalfMove boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features
  ADD HalfMove boolean DEFAULT 0;

-- unit movement cost is increased by 1 in the given terrain / feature
ALTER TABLE UnitPromotions_Terrains
  ADD ExtraMove boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features
  ADD ExtraMove boolean DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('PROMOTIONS_HALF_MOVE', 1);

-- Polynesia ignores increased movement cost in OCEAN and COAST.
