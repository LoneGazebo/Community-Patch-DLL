CREATE TRIGGER VP_DummyPolicies
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
WHEN NEW.CreatedByGreatPerson = 1
BEGIN
	UPDATE Improvements
	SET ConnectsAllResources = 1
	WHERE Type = NEW.Type;
END;

CREATE TRIGGER Units_GlobalSeparateGreatAdmiral
AFTER INSERT ON Units
WHEN NEW.Class = 'UNITCLASS_GREAT_ADMIRAL'
BEGIN
	UPDATE Units
	SET
		CanRepairFleet = 1,
		CanChangePort = 1
	WHERE Type = NEW.Type;
END;
