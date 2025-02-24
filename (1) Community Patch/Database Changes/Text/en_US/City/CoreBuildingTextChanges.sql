-- National Intelligence Agency
UPDATE Language_en_US
SET Text = 'Provides additional spies based on a fraction of the number of City-States, and levels up all your existing spies. Also provides a 15% reduction in enemy spy effectiveness. Must have a Police Station in all cities.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_HELP';

UPDATE Language_en_US
SET Text = 'An important defensive National Wonder for a technology-driven civilization. The National Intelligence Agency provides additional spies, levels up all your existing spies, and provides a 15% reduction in enemy spy effectiveness. A civilization must have a Police Station in all cities before it can construct the National Intelligence Agency.'
WHERE Tag = 'TXT_KEY_BUILDING_INTELLIGENCE_AGENCY_STRATEGY';

-- Great Firewall
UPDATE Language_en_US
SET Text = '99.9% reduction in effectiveness of enemy spies in the city in which it is built. All other cities in the civilization get a 25% reduction in enemy spy effectiveness. -50% [ICON_TOURISM] Tourism from Civilizations with the Internet technology.'
WHERE Tag = 'TXT_KEY_BUILDING_GREAT_FIREWALL_HELP';