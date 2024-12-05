----------------------------------
-- Recreate Leader Tables
----------------------------------

-- 3 new columns: Personality, PrimaryVictoryPursuit, SecondaryVictoryPursuit
-- Default diplomacy flavors are now 5 (except Chattiness)
CREATE TABLE Leaders_NEW (
	ID integer PRIMARY KEY,
	Type text NOT NULL UNIQUE,
	Description text REFERENCES Language_en_US (Tag),
	Civilopedia text, -- unused?
	CivilopediaTag text,
	ArtDefineTag text,
	Personality text,
	PrimaryVictoryPursuit text REFERENCES VictoryPursuitTypes (Type),
	SecondaryVictoryPursuit text REFERENCES VictoryPursuitTypes (Type),
	VictoryCompetitiveness integer DEFAULT 5,
	WonderCompetitiveness integer DEFAULT 5,
	MinorCivCompetitiveness integer DEFAULT 5,
	Boldness integer DEFAULT 5,
	DiploBalance integer DEFAULT 5,
	WarmongerHate integer DEFAULT 5,
	DoFWillingness integer DEFAULT 5,
	DenounceWillingness integer DEFAULT 5,
	WorkWithWillingness integer DEFAULT 5,
	WorkAgainstWillingness integer DEFAULT 5,
	Loyalty integer DEFAULT 5,
	Forgiveness integer DEFAULT 5,
	Neediness integer DEFAULT 5,
	Meanness integer DEFAULT 5,
	Chattiness integer DEFAULT 4,
	PortraitIndex integer DEFAULT -1,
	IconAtlas text REFERENCES IconTextureAtlases (Atlas),
	PackageID text
);

INSERT INTO Leaders_NEW
SELECT
	NULL, Type, Description, Civilopedia, CivilopediaTag, ArtDefineTag, NULL, NULL, NULL, VictoryCompetitiveness, WonderCompetitiveness, MinorCivCompetitiveness, Boldness, DiploBalance, WarmongerHate, DoFWillingness, DenounceWillingness, Loyalty, Forgiveness, Neediness, Meanness, Chattiness, 0, 0, PortraitIndex, IconAtlas, PackageID
FROM Leaders;

DROP TABLE Leaders;
ALTER TABLE Leaders_NEW RENAME TO Leaders;

-- Default bias is now 5
CREATE TABLE Leader_MajorCivApproachBiases_NEW (
	LeaderType text REFERENCES Leaders (Type),
	MajorCivApproachType text REFERENCES MajorCivApproachTypes (Type),
	Bias integer DEFAULT 5
);

INSERT INTO Leader_MajorCivApproachBiases_NEW
	(LeaderType, MajorCivApproachType)
SELECT
	LeaderType, MajorCivApproachType
FROM Leader_MajorCivApproachBiases;

DROP TABLE Leader_MajorCivApproachBiases;
ALTER TABLE Leader_MajorCivApproachBiases_NEW RENAME TO Leader_MajorCivApproachBiases;

-- A default bias of -1 (which will always return 1) is intentionally used here because the Firaxis Friendly/Protective approaches are merged into a single approach in the modded DLL, using the highest value of the two.
-- This prevents situations where one approach is set below 5 and the other is not set, but the default of 5 overrides the modder's flavor because it's greater in value.
CREATE TABLE Leader_MinorCivApproachBiases_NEW (
	LeaderType text REFERENCES Leaders (Type),
	MinorCivApproachType text REFERENCES MinorCivApproachTypes (Type),
	Bias integer DEFAULT -1
);

INSERT INTO Leader_MinorCivApproachBiases_NEW
	(LeaderType, MinorCivApproachType)
SELECT
	LeaderType, MinorCivApproachType
FROM Leader_MinorCivApproachBiases;

DROP TABLE Leader_MinorCivApproachBiases;
ALTER TABLE Leader_MinorCivApproachBiases_NEW RENAME TO Leader_MinorCivApproachBiases;
