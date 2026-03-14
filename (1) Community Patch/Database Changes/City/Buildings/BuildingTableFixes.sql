-- Convert bool type to correct boolean type on Building_ThemingBonuses
UPDATE Building_ThemingBonuses
SET
	SameEra = CASE WHEN SameEra IN (1, 'true') THEN 1 ELSE 0 END,
	UniqueEras = CASE WHEN UniqueEras IN (1, 'true') THEN 1 ELSE 0 END,
	MustBeArt = CASE WHEN MustBeArt IN (1, 'true') THEN 1 ELSE 0 END,
	MustBeArtifact = CASE WHEN MustBeArtifact IN (1, 'true') THEN 1 ELSE 0 END,
	MustBeEqualArtArtifact = CASE WHEN MustBeEqualArtArtifact IN (1, 'true') THEN 1 ELSE 0 END,
	RequiresOwner = CASE WHEN RequiresOwner IN (1, 'true') THEN 1 ELSE 0 END,
	RequiresAnyButOwner = CASE WHEN RequiresAnyButOwner IN (1, 'true') THEN 1 ELSE 0 END,
	RequiresSamePlayer = CASE WHEN RequiresSamePlayer IN (1, 'true') THEN 1 ELSE 0 END,
	RequiresUniquePlayers = CASE WHEN RequiresUniquePlayers IN (1, 'true') THEN 1 ELSE 0 END;

-- Migrate to new table
INSERT INTO Building_ThemingBonuses_new (
	BuildingType,
	Description,
	Bonus,
	SameEra,
	UniqueEras,
	ConsecutiveEras,
	MustBeArt,
	MustBeArtifact,
	MustBeEqualArtArtifact,
	RequiresOwner,
	RequiresAnyButOwner,
	RequiresSamePlayer,
	RequiresUniquePlayers,
	AIPriority
)
SELECT
	BuildingType,
	Description,
	Bonus,
	SameEra,
	UniqueEras,
	0,
	MustBeArt,
	MustBeArtifact,
	MustBeEqualArtArtifact,
	RequiresOwner,
	RequiresAnyButOwner,
	RequiresSamePlayer,
	RequiresUniquePlayers,
	AIPriority
FROM Building_ThemingBonuses;

DROP TABLE Building_ThemingBonuses;

ALTER TABLE Building_ThemingBonuses_new RENAME TO Building_ThemingBonuses;
