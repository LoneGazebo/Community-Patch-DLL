-- Policy requirement to unlock this process
ALTER TABLE Processes ADD RequiredPolicy text REFERENCES Policies (Type);

-- Add a percentage of the city's production to its strength and healing
ALTER TABLE Processes ADD DefenseValue integer DEFAULT 0;

-- Only the specified civilization can use this process
-- Needs the CIVILIZATIONS_UNIQUE_PROCESSES to function
ALTER TABLE Processes ADD CivilizationType text REFERENCES Civilizations (Type);
