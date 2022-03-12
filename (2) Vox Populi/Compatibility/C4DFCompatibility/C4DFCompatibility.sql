-- Insert SQL Rules Here 

UPDATE Buildings
SET VassalLevyEra = '1'
WHERE Type = 'BUILDING_PALACE';

UPDATE Language_en_US
SET Text = '{1_PlayerName} has conquered {2_CityName} and restored {3_CivAdj} sovereignty. The flame of the {4_CivName}, once extinguished, burns again. {4_CivName} is now the vassal of {1_Playername}.'
WHERE Tag = 'TXT_KEY_NOTIFICATION_CIV_RESURRECTED';

UPDATE Language_en_US
SET Text = 'This City was formerly owned by [COLOR_POSITIVE_TEXT]{@1_PlayerName}[ENDCOLOR]. You have the opportunity to return it to them, for which they would be eternally grateful. Doing so will make this player your [COLOR_POSITIVE_TEXT]Voluntary Vassal[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_POPUP_CITY_CAPTURE_INFO_LIBERATE_RESURRECT';

