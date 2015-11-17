UPDATE Leaders
SET ArtDefineTag = 'barbarian_scene.xml'
WHERE Type = 'LEADER_BARBARIAN';

UPDATE Language_en_US
SET Text = 'Crom'
WHERE Tag = 'TXT_KEY_LEADER_BARBARIAN';

UPDATE Leaders
SET IconAtlas = 'UNIT_ATLAS_1'
WHERE Type = 'LEADER_BARBARIAN';

UPDATE Leaders
SET PortraitIndex = '25'
WHERE Type = 'LEADER_BARBARIAN';