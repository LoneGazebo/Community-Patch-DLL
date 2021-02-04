CREATE TRIGGER Leaders_Personality
AFTER INSERT ON Leaders
WHEN EXISTS ( SELECT Type FROM Leaders WHERE Type = NEW.Type )
BEGIN

UPDATE Leaders SET Personality = 'PERSONALITY_CONQUEROR' WHERE Type = NEW.Type; -- basic default

UPDATE Leaders SET Personality=CASE
WHEN (DoFWillingness >= 7 OR Loyalty >= 7) THEN 'PERSONALITY_COALITION'
WHEN (DiploBalance >= 7 OR MinorCivCompetitiveness >= 7) THEN 'PERSONALITY_DIPLOMAT'
WHEN (Loyalty >= 3 OR WonderCompetitiveness <= 3) THEN 'PERSONALITY_EXPANSIONIST'
ELSE Personality END
WHERE Type = NEW.Type;

END;