ALTER TABLE Buildings
  ADD GreatWorkYieldType TEXT DEFAULT 'YIELD_CULTURE';
  -- FOREIGN KEY (GreatWorkYieldType) references Yields(Type);

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('GLOBAL_GREATWORK_YIELDTYPES', 1);
