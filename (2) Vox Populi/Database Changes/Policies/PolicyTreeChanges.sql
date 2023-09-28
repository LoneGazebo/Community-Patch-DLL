-- Number of policies or era required before a tree is unlocked
UPDATE PolicyBranchTypes
SET NumPolicyRequirement = 0
WHERE Type IN (
	'POLICY_BRANCH_TRADITION',
	'POLICY_BRANCH_LIBERTY',
	'POLICY_BRANCH_HONOR'
);

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = 6, EraPrereq = 'ERA_MEDIEVAL'
WHERE Type IN (
	'POLICY_BRANCH_PIETY',
	'POLICY_BRANCH_PATRONAGE',
	'POLICY_BRANCH_AESTHETICS'
);

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = 12, EraPrereq = 'ERA_INDUSTRIAL'
WHERE Type IN (
	'POLICY_BRANCH_COMMERCE',
	'POLICY_BRANCH_EXPLORATION',
	'POLICY_BRANCH_RATIONALISM'
);

-- Number of free tenets given to first ideology adopters
UPDATE PolicyBranchTypes
SET FirstAdopterFreePolicies = 1
WHERE Type IN (
	'POLICY_BRANCH_FREEDOM',
	'POLICY_BRANCH_ORDER',
	'POLICY_BRANCH_AUTOCRACY'
);
