-- Tables missing proper UNIQUE NOT NULL constraint on `Type`
-- Migration fix

--------------------------------------------------------------------------------
-- START Calendars
--------------------------------------------------------------------------------

CREATE TABLE Calendars_FIX (
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`Description` text -- Not referencing text tags since the texts actually aren't defined (and are unused)
);

INSERT INTO Calendars_FIX
	(ID, Type, Description)
SELECT
	ID, Type, Description
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
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`Description` text REFERENCES Language_en_US (Tag),
	`Valid` boolean,
	`CityLoad` boolean
);

INSERT INTO SpecialUnits_FIX
	(ID, Type, Description, Valid, CityLoad)
SELECT
	ID, Type, Description, Valid, CityLoad
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
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`UpdateFormation` boolean DEFAULT 0
);

INSERT INTO EntityEvents_FIX
	(ID, Type, UpdateFormation)
SELECT
	ID, Type, UpdateFormation
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
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`MissionPath` boolean DEFAULT 0
);

INSERT INTO AnimationPaths_FIX
	(ID, Type, MissionPath)
SELECT
	ID, Type, MissionPath
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
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`Description` text REFERENCES Language_en_US (Tag),
	`Help` text REFERENCES Language_en_US (Tag),
	`DisabledHelp` text REFERENCES Language_en_US (Tag),
	`HotKey` text,
	`HotKeyAlt` text,
	`HotKeyPriority` integer DEFAULT 0,
	`HotKeyPriorityAlt` integer DEFAULT 0,
	`OrderPriority` integer DEFAULT 0,
	`AltDown` boolean DEFAULT 0,
	`AltDownAlt` boolean DEFAULT 0,
	`ShiftDown` boolean DEFAULT 0,
	`ShiftDownAlt` boolean DEFAULT 0,
	`CtrlDown` boolean DEFAULT 0,
	`CtrlDownAlt` boolean DEFAULT 0,
	`Visible` boolean DEFAULT 0,
	`ConfirmCommand` boolean DEFAULT 0,
	`Automate` text, -- Self-referencing! These tables are wild
	`Command` text REFERENCES Commands (Type),
	`IconIndex` integer DEFAULT -1,
	`IconAtlas` text REFERENCES IconTextureAtlases (Atlas)
);

INSERT INTO Automates_FIX
SELECT * FROM Automates; -- noqa: AM04

DROP TABLE Automates;
ALTER TABLE Automates_FIX RENAME TO Automates;

--------------------------------------------------------------------------------
-- END Automates
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START GoodyHuts
--------------------------------------------------------------------------------

CREATE TABLE GoodyHuts_FIX (
	`ID` integer PRIMARY KEY AUTOINCREMENT,
	`Type` text UNIQUE NOT NULL,
	`Description` text REFERENCES Language_en_US (Tag),
	`ChooseDescription` text REFERENCES Language_en_US (Tag),
	`Sound` text,
	`Gold` integer DEFAULT 0,
	`NumGoldRandRolls` integer DEFAULT 0,
	`GoldRandAmount` integer DEFAULT 0,
	`MapOffset` integer DEFAULT 0,
	`MapRange` integer DEFAULT 0,
	`MapProb` integer DEFAULT 0,
	`Experience` integer DEFAULT 0,
	`Healing` integer DEFAULT 0,
	`DamagePrereq` integer DEFAULT 0,
	`Population` integer DEFAULT 0,
	`Culture` integer DEFAULT 0,
	`Faith` integer DEFAULT 0,
	`ProphetPercent` integer DEFAULT 0,
	`RevealNearbyBarbariansRange` integer DEFAULT 0,
	`Tech` boolean DEFAULT 0,
	`RevealUnknownResource` boolean DEFAULT 0,
	`UpgradeUnit` boolean DEFAULT 0,
	`PantheonFaith` boolean DEFAULT 0,
	`Bad` boolean DEFAULT 0,
	`UnitClass` text REFERENCES UnitClasses (Type),
	`BarbarianUnitClass` text REFERENCES UnitClasses (Type),
	`BarbarianUnitProb` integer DEFAULT 0,
	`MinBarbarians` integer DEFAULT 0,
	`Production` integer DEFAULT 0,
	`GoldenAge` integer DEFAULT 0,
	`FreeTiles` integer DEFAULT 0,
	`Science` integer DEFAULT 0,
	`PantheonPercent` integer DEFAULT 0,
	`Food` integer DEFAULT 0,
	`BorderGrowth` integer DEFAULT 0
);

INSERT INTO GoodyHuts_FIX
SELECT * FROM GoodyHuts; -- noqa: AM04

DROP TABLE GoodyHuts;
ALTER TABLE GoodyHuts_FIX RENAME TO GoodyHuts;

--------------------------------------------------------------------------------
-- END GoodyHuts
--------------------------------------------------------------------------------
