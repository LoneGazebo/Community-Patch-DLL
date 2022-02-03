
-- Set # of policies required before a branch is unlocked.

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '6'
WHERE Type = 'POLICY_BRANCH_PIETY';

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '6'
WHERE Type = 'POLICY_BRANCH_PATRONAGE';

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '6'
WHERE Type = 'POLICY_BRANCH_AESTHETICS';

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '12'
WHERE Type = 'POLICY_BRANCH_COMMERCE';

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '12'
WHERE Type = 'POLICY_BRANCH_EXPLORATION';

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '12'
WHERE Type = 'POLICY_BRANCH_RATIONALISM';
