ALTER TABLE Improvements
  ADD MountainsMakesValid INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('GLOBAL_ALPINE_PASSES', 1);
