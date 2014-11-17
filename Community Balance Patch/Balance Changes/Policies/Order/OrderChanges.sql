-- Worker's Faculties

UPDATE Policy_BuildingClassYieldModifiers
SET YieldMod = '0'
WHERE PolicyType = 'POLICY_WORKERS_FACULTIES' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Worker Faculties[ENDCOLOR][NEWLINE]Factories increase City [ICON_RESEARCH] Science output by +3. Build Factories in half the usual time.'
WHERE Tag = 'TXT_KEY_POLICY_WORKERS_FACULTIES_HELP' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_POLICIES' AND Value= 1 );
