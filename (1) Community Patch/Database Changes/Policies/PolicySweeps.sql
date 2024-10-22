UPDATE Policies
SET IsDummy = 1
WHERE PolicyBranchType IS NULL
AND NOT EXISTS (
	SELECT 1 FROM PolicyBranchTypes
	WHERE FreePolicy = Policies.Type
	OR FreeFinishingPolicy = Policies.Type
);
