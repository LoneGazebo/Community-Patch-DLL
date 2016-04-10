ALTER TABLE Improvements
  ADD AllowsRebaseTo INTEGER DEFAULT 0;

ALTER TABLE Improvements
  ADD AllowsAirliftFrom INTEGER DEFAULT 0;

ALTER TABLE Improvements
  ADD AllowsAirliftTo INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('GLOBAL_RELOCATION', 1);
