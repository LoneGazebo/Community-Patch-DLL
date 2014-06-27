/* The sql commands and text below were formulated in the Barbarians tab of the Google Spreadsheet 'Cep_Language'.
https://docs.google.com/spreadsheets/d/1ptQRfVsW7UT_8gPexioizS31sM7K_3eBT3tjr4jruTs/edit#gid=846200653
If you make any changes be sure to update the spreadsheet also.*/

REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_ARCHER', '密猟者', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_SPEARMAN', 'Pillager', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_SWORDSMAN', '略奪者', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_PIKEMAN', '傭兵', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_CROSSBOWMAN', '暗殺者', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_LONGSWORDSMAN', 'アウトロー', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_MUSKETMAN', '山賊', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_RIFLEMAN', 'アジテーター', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_INFANTRY', 'パルチザン', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_PARATROOPER', 'ゲリラ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_MECHANIZED_INFANTRY', '反乱', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_CHARIOT_ARCHER', 'ハンター', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_HORSEMAN', '略奪者', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_KNIGHT', '強盗', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_LANCER', '追いはぎ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_CAVALRY', '山賊', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_MECH', 'ならず者', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_GALLEY', 'よだれ', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_TRIREME', '海賊', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_GALLEASS', '海賊', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_FRIGATE', 'プライベーター', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_IRONCLAD', '乗っ取り屋', '', '');
REPLACE INTO Language_JA_JP (Tag, Text, Gender, Plurality) VALUES ('TXT_KEY_UNIT_BARBARIAN_SUBMARINE', '密輸業者', '', '');





UPDATE LoadedFile SET Value=1, JA_JP=1 Where Type='Barbarians.sql';




