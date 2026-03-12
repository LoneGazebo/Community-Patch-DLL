-- Add a policy column that is used for UI purposes
ALTER TABLE Civilizations ADD PolicyForUserInterface text REFERENCES Policies (Type);
