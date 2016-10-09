-- Insert SQL Rules Here 

UPDATE Buildings
SET VassalLevyEra = '1'
WHERE Type = 'BUILDING_PALACE';

UPDATE Language_en_US
SET Text = '{1_PlayerName} has conquered {2_CityName} and restored {3_CivAdj} sovereignty. The flame of the {4_CivName}, once extinguished, burns again. {4_CivName} is now the vassal of {1_Playername}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CIV_RESURRECTED';