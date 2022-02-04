
-- Set # of policies required before a branch is unlocked.

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '6'
WHERE Type IN 
('POLICY_BRANCH_PIETY',
'POLICY_BRANCH_PATRONAGE',
'POLICY_BRANCH_AESTHETICS');

UPDATE PolicyBranchTypes
SET NumPolicyRequirement = '12'
WHERE Type IN 
('POLICY_BRANCH_COMMERCE',
'POLICY_BRANCH_EXPLORATION',
'POLICY_BRANCH_RATIONALISM');
