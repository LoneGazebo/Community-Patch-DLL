ALTER TABLE Buildings
  ADD GlobalConversionModifier INTEGER DEFAULT 0;
ALTER TABLE Buildings
  ADD ConversionModifier INTEGER DEFAULT 0;
ALTER TABLE Policies
  ADD ConversionModifier INTEGER DEFAULT 0;

INSERT INTO CustomModDbUpdates(Name, Value) VALUES('RELIGION_CONVERSION_MODIFIERS', 1);
