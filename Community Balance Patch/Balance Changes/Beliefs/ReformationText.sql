-- Charitable Missions (Now Missionary Influence)
UPDATE Language_en_US
SET Text = 'Missionaries grant +20 [ICON_INFLUENCE] Influence with a City-State when you spread your Religion to that City-State. '
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Missionary Influence'
WHERE Tag = 'TXT_KEY_BELIEF_CHARITABLE_MISSIONS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Evangelism (Now Crusader Zeal)
UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength in enemy lands versus players that follow an opposing Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Holy War'
WHERE Tag = 'TXT_KEY_BELIEF_EVANGELISM_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Defender Faith
UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength in own lands versus players that follow an opposing Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_DEFENDER_FAITH' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Jesuit Education 

-- Religious Fervor (Now The One True Faith)
UPDATE Language_en_US
SET Text = 'Receive 1 additional Delegate in the World Congress for every 8 City-States in the game. Missionaries performing the Spread Religion action erode existing pressure from other religions.'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'The One True Faith'
WHERE Tag = 'TXT_KEY_BELIEF_RELIGIOUS_FERVOR_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Sacred Sites
UPDATE Language_en_US
SET Text = 'All buildings purchased with Faith provide 2 [ICON_TOURISM] Tourism each. Hermitage provides +5 [ICON_CULTURE] Culture and +5 [ICON_TOURISM] Tourism.'
WHERE Tag = 'TXT_KEY_BELIEF_SACRED_SITES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Glory of God

-- Underground Sect (Now Faith of the  Masses)
UPDATE Language_en_US
SET Text = 'May build Opera Houses, Museums and Broadcast Towers with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Faith of the Masses'
WHERE Tag = 'TXT_KEY_BELIEF_UNDERGROUND_SECT_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

-- Unity of the Prophets (Now Knowledge Through Faith)

UPDATE Language_en_US
SET Text = 'Holy Sites produce +4 [ICON_RESEARCH] Science, Academies produce +4 [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );

UPDATE Language_en_US
SET Text = 'Knowledge Through Devotion'
WHERE Tag = 'TXT_KEY_BELIEF_UNITY_OF_PROPHETS_SHORT' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_RELIGION' AND Value= 1 );
