-- Update ResourceUsage column for utility (will be overridden in dll)
UPDATE Resources
SET ResourceUsage = 0;

-- Strategic
UPDATE Resources
SET ResourceUsage = 1
WHERE ResourceClassType IN (
	'RESOURCECLASS_RUSH',
	'RESOURCECLASS_MODERN'
) AND Type NOT IN (
	'RESOURCE_ARTIFACTS',
	'RESOURCE_HIDDEN_ARTIFACTS'
);

-- Luxury
UPDATE Resources
SET ResourceUsage = 2
WHERE ResourceClassType = 'RESOURCECLASS_LUXURY';
