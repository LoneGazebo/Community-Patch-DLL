-- Ranking popup
INSERT INTO Language_en_US
	(Tag, Text)
VALUES
	('TXT_KEY_DUDE_8', 'Herodotus'),
	('TXT_KEY_DUDE_9', 'Confucius'),
	('TXT_KEY_DUDE_10', 'Aristotle'),
	('TXT_KEY_DUDE_11', 'Aesop'),
	('TXT_KEY_DUDE_12', 'Ovid'),
	('TXT_KEY_DUDE_13', 'Sima Qian'),
	('TXT_KEY_DUDE_14', 'Ban Zhao'),
	('TXT_KEY_DUDE_15', 'Gibbon'),
	('TXT_KEY_DUDE_16', 'Nostradamus'),
	('TXT_KEY_DUDE_17', 'Scheherazade'),
	('TXT_KEY_DUDE_18', 'Hypatia'),
	('TXT_KEY_DUDE_19', 'Aguilar'),
	('TXT_KEY_DUDE_20', 'Pamphila'),
	('TXT_KEY_DUDE_21', 'Livy'),
	('TXT_KEY_DUDE_22', 'Plutarch'),
	('TXT_KEY_DUDE_23', 'Bede'),
	('TXT_KEY_DUDE_24', 'Nestor'),
	('TXT_KEY_DUDE_25', 'Nennius');

-- Corporation Franchise (text update)
-- Modmodders can copy this code if they're adding more resources toward a corporation!
UPDATE Language_en_US
SET Text = '+1 [ICON_GOLD] Gold on all Trader Sid''s Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_TRADER_SIDS' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_TRADER_SIDS}'
WHERE Tag = 'TXT_KEY_BUILDING_TRADER_SIDS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production on all Centaurus Extractors'' Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_LANDSEA_EXTRACTORS' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_LANDSEA_EXTRACTORS}'
WHERE Tag = 'TXT_KEY_BUILDING_LANDSEA_EXTRACTORS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_PRODUCTION] Production on Hexxon Refineries Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_HEXXON_REFINERY' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_HEXXON_REFINERY}'
WHERE Tag = 'TXT_KEY_BUILDING_HEXXON_REFINERY_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_CULTURE] Culture on all Giorgio Armeier''s Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_GIORGIO_ARMEIER' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_GIORGIO_ARMEIER}'
WHERE Tag = 'TXT_KEY_BUILDING_GIORGIO_ARMEIER_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_RESEARCH] Science on all Firaxite Materials'' Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_FIRAXITE_MATERIALS' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_FIRAXITE_MATERIALS}'
WHERE Tag = 'TXT_KEY_BUILDING_FIRAXITE_MATERIALS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_FOOD] Food on all TwoKay Foods'' Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_TWOKAY_FOODS' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_TWOKAY_FOODS}'
WHERE Tag = 'TXT_KEY_BUILDING_TWOKAY_FOODS_FRANCHISE_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_GOLDEN_AGE] Golden Age Points on all Civilized Jewelers'' Monopoly Resources ('
	|| (SELECT GROUP_CONCAT(r.IconString, ", ") FROM Corporation_ResourceMonopolyOrs c, Resources r WHERE c.CorporationType = 'CORPORATION_CIVILIZED_JEWELERS' AND c.ResourceType = r.Type) 
	|| ') near the City. {TXT_KEY_CORPORATION_TRADE_ROUTE_BONUS_CIVILIZED_JEWELERS}'
WHERE Tag = 'TXT_KEY_BUILDING_CIVILIZED_JEWELERS_FRANCHISE_HELP';

--CORPORATION CITYBANNERMANAGER
INSERT INTO Language_en_US (Tag, Text)
SELECT 'TXT_KEY_CORPORATION_OFFICEHQ_TT', '[ICON_CAPITAL] Headquarters of [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR]' UNION ALL
SELECT 'TXT_KEY_CORPORATION_OFFICEHQ_TT2', 'Capturing [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR] will always result in the destruction of the Headquarters, closing all active Offices and Franchises but permits others to rebuild its Headquarters once more.' UNION ALL
SELECT 'TXT_KEY_CORPORATION_OFFICE_TT', 'Office of [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR]' UNION ALL
SELECT 'TXT_KEY_CORPORATION_OFFICE_TT2', 'A completed [ICON_INTERNATIONAL_TRADE] Trade Route from or to [COLOR_POSITIVE_TEXT]{1_Name}[ENDCOLOR] will attempt to establish a [ICON_VP_FRANCHISE] [COLOR_POSITIVE_TEXT]Franchise[ENDCOLOR] in foreign cities.';

--INFLUENTIAL DOMINANT NOTIFICATION
UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Exotic[ENDCOLOR] to {1_Num}. We will no longer get any bonuses from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_1_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Exotic[ENDCOLOR] to {1_Num}! See Culture Overview for the bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_1';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Familiar[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_2_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Familiar[ENDCOLOR] to {1_Num}!  See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_2';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Popular[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_3_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Popular[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_3';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Influential[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_4_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Influential[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_4';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is [COLOR_NEGATIVE_TEXT]no longer[ENDCOLOR] [COLOR_MAGENTA]Dominant[ENDCOLOR] to {1_Num}. Our bonuses have weakened from them.'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_5_BAD';

UPDATE Language_en_US
SET Text = 'Our [ICON_CULTURE] Culture is now [COLOR_MAGENTA]Dominant[ENDCOLOR] to {1_Num}! See Culture Overview for the increased bonuses!'
WHERE Tag = 'TXT_KEY_INFLUENCE_US_5';