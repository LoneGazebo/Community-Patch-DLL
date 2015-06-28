DELETE FROM Technology_PrereqTechs
WHERE TechType = 'TECH_RADIO' AND PrereqTech = 'TECH_BIOLOGY';

DELETE FROM Technology_PrereqTechs
WHERE TechType = 'TECH_RADIO' AND PrereqTech = 'TECH_REPLACEABLE_PARTS';

DELETE FROM Technology_PrereqTechs
WHERE TechType = 'TECH_FLIGHT' AND PrereqTech = 'TECH_COMBUSTION';

DELETE FROM Technology_PrereqTechs
WHERE TechType = 'TECH_FLIGHT' AND PrereqTech = 'TECH_ELECTRICITY';