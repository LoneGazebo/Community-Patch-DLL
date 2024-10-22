CREATE TRIGGER CBP_DummyPolicies
AFTER INSERT ON Policies
WHEN (
	NEW.PolicyBranchType IS NULL
	AND NOT EXISTS (
		SELECT 1 FROM PolicyBranchTypes
		WHERE FreePolicy = NEW.Type
		OR FreeFinishingPolicy = NEW.Type
	)
)
BEGIN
	UPDATE Policies
	SET IsDummy = 1
	WHERE Type = NEW.Type;
END;

CREATE TRIGGER Improvements_GPTIConnectsResources
AFTER INSERT ON Improvements
BEGIN
	UPDATE Improvements
	SET ConnectsAllResources = 1
	WHERE Type = NEW.Type AND CreatedByGreatPerson = 1;
END;

CREATE TRIGGER Leaders_Personality
AFTER INSERT ON Leaders
WHEN (EXISTS (SELECT 1 FROM Leaders WHERE Type = NEW.Type))
BEGIN
	UPDATE Leaders
	SET
		Personality = CASE
			WHEN (DoFWillingness >= 7 OR Loyalty >= 7) THEN 'PERSONALITY_COALITION'
			WHEN (DiploBalance >= 7 OR MinorCivCompetitiveness >= 7) THEN 'PERSONALITY_DIPLOMAT'
			WHEN (Loyalty <= 3 OR WonderCompetitiveness <= 3) THEN 'PERSONALITY_EXPANSIONIST'
			ELSE 'PERSONALITY_CONQUEROR'
		END
	WHERE Type = NEW.Type;
END;
