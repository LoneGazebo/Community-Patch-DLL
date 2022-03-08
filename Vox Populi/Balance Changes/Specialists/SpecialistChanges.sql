-- Balance Engineers

UPDATE SpecialistYields
SET Yield = '3'
WHERE SpecialistType = 'SPECIALIST_ENGINEER';

-- Balance Merchants

UPDATE SpecialistYields
SET Yield = '4'
WHERE SpecialistType = 'SPECIALIST_MERCHANT';

-- Balance Scientists

UPDATE SpecialistYields
SET Yield = '3'
WHERE SpecialistType = 'SPECIALIST_SCIENTIST';

-- Balance Artists

UPDATE Specialists
SET CulturePerTurn = '3'
WHERE Type = 'SPECIALIST_ARTIST';

-- Balance Writers

UPDATE Specialists
SET CulturePerTurn = '3'
WHERE Type = 'SPECIALIST_WRITER';

-- Balance Musicians

UPDATE Specialists
SET CulturePerTurn = '5'
WHERE Type = 'SPECIALIST_MUSICIAN';

-- Balance Citizens

UPDATE SpecialistYields
SET Yield = '1'
WHERE SpecialistType = 'SPECIALIST_CITIZEN';