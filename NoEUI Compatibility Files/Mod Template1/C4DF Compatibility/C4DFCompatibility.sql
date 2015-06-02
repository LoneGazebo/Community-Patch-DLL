-- Insert SQL Rules Here 

UPDATE Buildings
SET VassalLevyEra = 'true'
WHERE Type = 'BUILDING_PALACE';