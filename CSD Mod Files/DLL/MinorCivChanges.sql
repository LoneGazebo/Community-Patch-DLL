-- Minor Civ Stuff

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_MINOR_ANCHOR_ATTACK', '-10';

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_CS_WAR_COOLDOWN_RATE', '15';

INSERT INTO Defines (
Name, Value)
SELECT 'BALANCE_CS_FORGIVENESS_CHANCE', '75';

UPDATE Language_en_US
SET Text = 'You are at war with {1_CityStateName:textkey}, which means it will not make peace with you! (If you declared war on this City-State, it will not make peace with you until {2_JerkRate} or more turns have expired.)'
WHERE Tag = 'TXT_KEY_PEACE_BLOCKED_CSTATE_TT';
