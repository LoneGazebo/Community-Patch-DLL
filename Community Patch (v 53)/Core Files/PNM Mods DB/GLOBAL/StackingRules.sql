-- Unit stacking limit in cities
INSERT INTO Defines(Name, Value) VALUES('CITY_UNIT_LIMIT', 1);
-- Additional units allowed by improvements
ALTER TABLE Improvements
  ADD AdditionalUnits INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('GLOBAL_STACKING_RULES', 1);
