-- Unit stacking limit in cities
INSERT INTO Defines(Name, Value) VALUES('CITY_UNIT_LIMIT', 1);
-- Additional units allowed by improvements
ALTER TABLE Improvements
  ADD AdditionalUnits INTEGER DEFAULT 0;
 --Additional units that can stack with this unit 
ALTER TABLE Units
  ADD NumberStackingUnits INTEGER DEFAULT 0;
 
ALTER TABLE Units
  ADD StackCombat INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('GLOBAL_STACKING_RULES', 1);