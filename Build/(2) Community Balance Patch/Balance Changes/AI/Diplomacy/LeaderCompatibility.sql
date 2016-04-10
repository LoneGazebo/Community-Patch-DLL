CREATE TRIGGER Leaders_Personality
AFTER INSERT ON Leaders
WHEN EXISTS ( SELECT Type FROM Leaders WHERE Type = NEW.Type )
BEGIN

UPDATE Leaders SET Personality=CASE
WHEN (DoFWillingness >= 6 OR Loyalty >= 6) THEN 'PERSONALITY_COALITION' 
WHEN (VictoryCompetitiveness >= 6 OR DenounceWillingness >= 6) THEN 'PERSONALITY_CONQUEROR'
WHEN (DiploBalance >= 6 OR MinorCivCompetitiveness >= 6) THEN 'PERSONALITY_DIPLOMAT'
WHEN (Boldness >= 6 OR WarmongerHate >= 6) THEN 'PERSONALITY_EXPANSIONIST'
ELSE Personality END
WHERE Type = NEW.Type;

UPDATE Leaders
SET WonderCompetitiveness=6, MinorCivCompetitiveness=7, Boldness=4, WarmongerHate=7, DenounceWillingness=9, DoFWillingness=6, VictoryCompetitiveness=8, DiploBalance=8, Loyalty=7, Neediness=4, Forgiveness=8, Chattiness=4, Meanness=5
WHERE Type = NEW.Type AND Personality = 'PERSONALITY_COALITION';

UPDATE Leaders
SET WonderCompetitiveness=4, MinorCivCompetitiveness=3, Boldness=6, WarmongerHate=5, DenounceWillingness=7, DoFWillingness=2, VictoryCompetitiveness=8, DiploBalance=4, Loyalty=5, Neediness=6, Forgiveness=2, Chattiness=4, Meanness=9
WHERE Type = NEW.Type AND Personality = 'PERSONALITY_CONQUEROR';

UPDATE Leaders
SET WonderCompetitiveness=8, MinorCivCompetitiveness=9, Boldness=2, WarmongerHate=9, DenounceWillingness=3, DoFWillingness=8, VictoryCompetitiveness=8, DiploBalance=6, Loyalty=9, Neediness=8, Forgiveness=6, Chattiness=4, Meanness=3
WHERE Type = NEW.Type AND Personality = 'PERSONALITY_DIPLOMAT';

UPDATE Leaders
SET WonderCompetitiveness=2, MinorCivCompetitiveness=5, Boldness=8, WarmongerHate=5, DenounceWillingness=5, DoFWillingness=4, VictoryCompetitiveness=8, DiploBalance=2, Loyalty=3, Neediness=2, Forgiveness=4, Chattiness=4, Meanness=7
WHERE Type = NEW.Type AND Personality = 'PERSONALITY_EXPANSIONIST';

END;

CREATE TRIGGER Leader_MinorCivApproachBiases_Personality
AFTER INSERT ON Leader_MinorCivApproachBiases 
WHEN EXISTS ( SELECT LeaderType FROM Leader_MinorCivApproachBiases WHERE LeaderType = NEW.LeaderType )
BEGIN

UPDATE Leader_MinorCivApproachBiases
SET Personality = (SELECT Personality FROM Leaders WHERE Type = NEW.LeaderType)
WHERE LeaderType = NEW.LeaderType;

UPDATE Leader_MinorCivApproachBiases SET Bias=CASE
WHEN (Personality = 'PERSONALITY_COALITION') THEN 4
WHEN (Personality = 'PERSONALITY_CONQUEROR') THEN 6
WHEN (Personality = 'PERSONALITY_DIPLOMAT') THEN 2
WHEN (Personality = 'PERSONALITY_EXPANSIONIST') THEN 6
ELSE Bias END
WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_IGNORE' AND LeaderType = NEW.LeaderType;

UPDATE Leader_MinorCivApproachBiases SET Bias=CASE
WHEN (Personality = 'PERSONALITY_COALITION') THEN 8
WHEN (Personality = 'PERSONALITY_CONQUEROR') THEN 2
WHEN (Personality = 'PERSONALITY_DIPLOMAT') THEN 6
WHEN (Personality = 'PERSONALITY_EXPANSIONIST') THEN 2
ELSE Bias END
WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_FRIENDLY' AND LeaderType = NEW.LeaderType;

UPDATE Leader_MinorCivApproachBiases SET Bias=CASE
WHEN (Personality = 'PERSONALITY_COALITION') THEN 6
WHEN (Personality = 'PERSONALITY_CONQUEROR') THEN 4
WHEN (Personality = 'PERSONALITY_DIPLOMAT') THEN 8
WHEN (Personality = 'PERSONALITY_EXPANSIONIST') THEN 6
ELSE Bias END
WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_PROTECTIVE' AND LeaderType = NEW.LeaderType;

UPDATE Leader_MinorCivApproachBiases SET Bias=CASE
WHEN (Personality = 'PERSONALITY_COALITION') THEN 2
WHEN (Personality = 'PERSONALITY_CONQUEROR') THEN 8
WHEN (Personality = 'PERSONALITY_DIPLOMAT') THEN 4
WHEN (Personality = 'PERSONALITY_EXPANSIONIST') THEN 8
ELSE Bias END
WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_CONQUEST' AND LeaderType = NEW.LeaderType;

UPDATE Leader_MinorCivApproachBiases SET Bias=CASE
WHEN (Personality = 'PERSONALITY_COALITION') THEN 6
WHEN (Personality = 'PERSONALITY_CONQUEROR') THEN 6
WHEN (Personality = 'PERSONALITY_DIPLOMAT') THEN 6
WHEN (Personality = 'PERSONALITY_EXPANSIONIST') THEN 4
ELSE Bias END
WHERE MinorCivApproachType = 'MINOR_CIV_APPROACH_BULLY' AND LeaderType = NEW.LeaderType;

END;