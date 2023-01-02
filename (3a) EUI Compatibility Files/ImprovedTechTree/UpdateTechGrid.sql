-- Update the grid so the tech buttons could expand
UPDATE Technologies SET GridY = 8 WHERE Type = 'TECH_ARCHERY';          -- VP: GridY = 7
UPDATE Technologies SET GridY = 2 WHERE Type = 'TECH_HORSEBACK_RIDING'; -- VP: GridY = 3