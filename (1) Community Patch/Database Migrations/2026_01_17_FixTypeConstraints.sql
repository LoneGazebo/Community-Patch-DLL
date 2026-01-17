-- Tables missing proper UNIQUE NOT NULL constraint on `Type`
-- Migration fix

--------------------------------------------------------------------------------
-- START Calendars
--------------------------------------------------------------------------------

CREATE TABLE Calendars_FIX (
	`ID`           INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`         TEXT UNIQUE NOT NULL,
	`Description`  TEXT
);

INSERT INTO Calendars_FIX (
	ID,
	Type,
	Description
)
SELECT
	ID,
	Type,
	Description
FROM Calendars;

DROP TABLE Calendars;
ALTER TABLE Calendars_FIX RENAME TO Calendars;

--------------------------------------------------------------------------------
-- END Calendars
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START SpecialUnits
--------------------------------------------------------------------------------

CREATE TABLE SpecialUnits_FIX (
	`ID`           INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`         TEXT UNIQUE NOT NULL,
	`Description`  TEXT,
	`Valid`        boolean,
	`CityLoad`     boolean
);

INSERT INTO SpecialUnits_FIX (
	ID,
	Type,
	Description,
	Valid,
	CityLoad
)
SELECT
	ID,
	Type,
	Description,
	Valid,
	CityLoad
FROM SpecialUnits;

DROP TABLE SpecialUnits;
ALTER TABLE SpecialUnits_FIX RENAME TO SpecialUnits;

--------------------------------------------------------------------------------
-- END SpecialUnits
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START EntityEvents
--------------------------------------------------------------------------------

CREATE TABLE EntityEvents_FIX (
	`ID`               INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`             TEXT UNIQUE NOT NULL,
	`UpdateFormation`  boolean DEFAULT 0
);

INSERT INTO EntityEvents_FIX (
	ID,
	Type,
	UpdateFormation
)
SELECT
	ID,
	Type,
	UpdateFormation
FROM EntityEvents;

DROP TABLE EntityEvents;
ALTER TABLE EntityEvents_FIX RENAME TO EntityEvents;

--------------------------------------------------------------------------------
-- END EntityEvents
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START AnimationPaths
--------------------------------------------------------------------------------

CREATE TABLE AnimationPaths_FIX (
	`ID`           INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`         TEXT UNIQUE NOT NULL,
	`MissionPath`  boolean DEFAULT 0
);

INSERT INTO AnimationPaths_FIX (
	ID,
	Type,
	MissionPath
)
SELECT
	ID,
	Type,
	MissionPath
FROM AnimationPaths;

DROP TABLE AnimationPaths;
ALTER TABLE AnimationPaths_FIX RENAME TO AnimationPaths;

--------------------------------------------------------------------------------
-- END AnimationPaths
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Automates
--------------------------------------------------------------------------------

CREATE TABLE Automates_FIX (
	`ID`                 INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`               TEXT UNIQUE NOT NULL,
	`Description`        TEXT,
	`Help`               TEXT,
	`DisabledHelp`       TEXT,
	`HotKey`             TEXT,
	`HotKeyAlt`          TEXT,
	`HotKeyPriority`     INTEGER DEFAULT 0,
	`HotKeyPriorityAlt`  INTEGER DEFAULT 0,
	`OrderPriority`      INTEGER DEFAULT 0,
	`AltDown`            boolean DEFAULT 0,
	`AltDownAlt`         boolean DEFAULT 0,
	`ShiftDown`          boolean DEFAULT 0,
	`ShiftDownAlt`       boolean DEFAULT 0,
	`CtrlDown`           boolean DEFAULT 0,
	`CtrlDownAlt`        boolean DEFAULT 0,
	`Visible`            boolean DEFAULT 0,
	`ConfirmCommand`     boolean DEFAULT 0,
	`Automate`           TEXT,
	`Command`            TEXT,
	`IconIndex`          INTEGER DEFAULT -1,
	`IconAtlas`          TEXT DEFAULT NULL REFERENCES IconTextureAtlases (Atlas)
);

INSERT INTO Automates_FIX
SELECT
	*
FROM Automates;

DROP TABLE Automates;
ALTER TABLE Automates_FIX RENAME TO Automates;

--------------------------------------------------------------------------------
-- END Automates
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START GoodyHuts
--------------------------------------------------------------------------------

CREATE TABLE GoodyHuts_FIX (
	`ID`                            INTEGER PRIMARY KEY AUTOINCREMENT,
	`Type`                          TEXT UNIQUE NOT NULL,
	`Description`                   TEXT,
	`ChooseDescription`             TEXT,
	`Sound`                         TEXT,
	`Gold`                          INTEGER DEFAULT 0,
	`NumGoldRandRolls`              INTEGER DEFAULT 0,
	`GoldRandAmount`                INTEGER DEFAULT 0,
	`MapOffset`                     INTEGER DEFAULT 0,
	`MapRange`                      INTEGER DEFAULT 0,
	`MapProb`                       INTEGER DEFAULT 0,
	`Experience`                    INTEGER DEFAULT 0,
	`Healing`                       INTEGER DEFAULT 0,
	`DamagePrereq`                  INTEGER DEFAULT 0,
	`Population`                    INTEGER DEFAULT 0,
	`Culture`                       INTEGER DEFAULT 0,
	`Faith`                         INTEGER DEFAULT 0,
	`ProphetPercent`                INTEGER DEFAULT 0,
	`RevealNearbyBarbariansRange`   INTEGER DEFAULT 0,
	`Tech`                          boolean DEFAULT 0,
	`RevealUnknownResource`         boolean DEFAULT 0,
	`UpgradeUnit`                   boolean DEFAULT 0,
	`PantheonFaith`                 boolean DEFAULT 0,
	`Bad`                           boolean DEFAULT 0,
	`UnitClass`                     TEXT DEFAULT NULL,
	`BarbarianUnitClass`            TEXT DEFAULT NULL,
	`BarbarianUnitProb`             INTEGER DEFAULT 0,
	`MinBarbarians`                 INTEGER DEFAULT 0,
	`Production`                    INTEGER DEFAULT 0,
	`GoldenAge`                     INTEGER DEFAULT 0,
	`FreeTiles`                     INTEGER DEFAULT 0,
	`Science`                       INTEGER DEFAULT 0,
	`PantheonPercent`               INTEGER DEFAULT 0,
	`Food`                          INTEGER DEFAULT 0,
	`BorderGrowth`                  INTEGER DEFAULT 0
);

INSERT INTO GoodyHuts_FIX
SELECT
	*
FROM GoodyHuts;

DROP TABLE GoodyHuts;
ALTER TABLE GoodyHuts_FIX RENAME TO GoodyHuts;

--------------------------------------------------------------------------------
-- END GoodyHuts
--------------------------------------------------------------------------------