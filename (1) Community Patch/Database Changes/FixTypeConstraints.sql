-- Migration fixes for table schemas with wrong constraints or column types
-- Each table is rebuilt (create _FIX, copy, drop, rename) or its values are converted in place

--------------------------------------------------------------------------------
-- Tables missing proper UNIQUE NOT NULL constraint on `Type`
--------------------------------------------------------------------------------

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
	`MinBarbarians` integer DEFAULT 0
);

INSERT INTO GoodyHuts_FIX
SELECT * FROM GoodyHuts; -- noqa: AM04

DROP TABLE GoodyHuts;
ALTER TABLE GoodyHuts_FIX RENAME TO GoodyHuts;

--------------------------------------------------------------------------------
-- END GoodyHuts
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- Columns declared with the wrong type
-- `YieldType` holds Yields.Type text but was declared integer
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_BuildingClassYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_BuildingClassYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`BuildingClassType` text REFERENCES BuildingClasses (Type),
	`YieldType` text REFERENCES Yields (Type),
	`YieldChange` integer DEFAULT 0
);

INSERT INTO Belief_BuildingClassYieldChanges_FIX
	(BeliefType, BuildingClassType, YieldType, YieldChange)
SELECT
	BeliefType, BuildingClassType, YieldType, YieldChange
FROM Belief_BuildingClassYieldChanges;

DROP TABLE Belief_BuildingClassYieldChanges;
ALTER TABLE Belief_BuildingClassYieldChanges_FIX RENAME TO Belief_BuildingClassYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_BuildingClassYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_CityYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_CityYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_CityYieldChanges_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_CityYieldChanges;

DROP TABLE Belief_CityYieldChanges;
ALTER TABLE Belief_CityYieldChanges_FIX RENAME TO Belief_CityYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_CityYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_FeatureYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_FeatureYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`FeatureType` text REFERENCES Features (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_FeatureYieldChanges_FIX
	(BeliefType, FeatureType, YieldType, Yield)
SELECT
	BeliefType, FeatureType, YieldType, Yield
FROM Belief_FeatureYieldChanges;

DROP TABLE Belief_FeatureYieldChanges;
ALTER TABLE Belief_FeatureYieldChanges_FIX RENAME TO Belief_FeatureYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_FeatureYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_HolyCityYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_HolyCityYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_HolyCityYieldChanges_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_HolyCityYieldChanges;

DROP TABLE Belief_HolyCityYieldChanges;
ALTER TABLE Belief_HolyCityYieldChanges_FIX RENAME TO Belief_HolyCityYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_HolyCityYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_ResourceYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_ResourceYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`ResourceType` text REFERENCES Resources (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_ResourceYieldChanges_FIX
	(BeliefType, ResourceType, YieldType, Yield)
SELECT
	BeliefType, ResourceType, YieldType, Yield
FROM Belief_ResourceYieldChanges;

DROP TABLE Belief_ResourceYieldChanges;
ALTER TABLE Belief_ResourceYieldChanges_FIX RENAME TO Belief_ResourceYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_ResourceYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_TerrainYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Belief_TerrainYieldChanges_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`TerrainType` text REFERENCES Terrains (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_TerrainYieldChanges_FIX
	(BeliefType, TerrainType, YieldType, Yield)
SELECT
	BeliefType, TerrainType, YieldType, Yield
FROM Belief_TerrainYieldChanges;

DROP TABLE Belief_TerrainYieldChanges;
ALTER TABLE Belief_TerrainYieldChanges_FIX RENAME TO Belief_TerrainYieldChanges;

--------------------------------------------------------------------------------
-- END Belief_TerrainYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangeAnySpecialist
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangeAnySpecialist_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangeAnySpecialist_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_YieldChangeAnySpecialist;

DROP TABLE Belief_YieldChangeAnySpecialist;
ALTER TABLE Belief_YieldChangeAnySpecialist_FIX RENAME TO Belief_YieldChangeAnySpecialist;

--------------------------------------------------------------------------------
-- END Belief_YieldChangeAnySpecialist
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangeNaturalWonder
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangeNaturalWonder_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangeNaturalWonder_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_YieldChangeNaturalWonder;

DROP TABLE Belief_YieldChangeNaturalWonder;
ALTER TABLE Belief_YieldChangeNaturalWonder_FIX RENAME TO Belief_YieldChangeNaturalWonder;

--------------------------------------------------------------------------------
-- END Belief_YieldChangeNaturalWonder
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangePerForeignCity
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangePerForeignCity_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangePerForeignCity_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_YieldChangePerForeignCity;

DROP TABLE Belief_YieldChangePerForeignCity;
ALTER TABLE Belief_YieldChangePerForeignCity_FIX RENAME TO Belief_YieldChangePerForeignCity;

--------------------------------------------------------------------------------
-- END Belief_YieldChangePerForeignCity
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangePerXForeignFollowers
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangePerXForeignFollowers_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`ForeignFollowers` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangePerXForeignFollowers_FIX
	(BeliefType, YieldType, ForeignFollowers)
SELECT
	BeliefType, YieldType, ForeignFollowers
FROM Belief_YieldChangePerXForeignFollowers;

DROP TABLE Belief_YieldChangePerXForeignFollowers;
ALTER TABLE Belief_YieldChangePerXForeignFollowers_FIX RENAME TO Belief_YieldChangePerXForeignFollowers;

--------------------------------------------------------------------------------
-- END Belief_YieldChangePerXForeignFollowers
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangeTradeRoute
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangeTradeRoute_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangeTradeRoute_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_YieldChangeTradeRoute;

DROP TABLE Belief_YieldChangeTradeRoute;
ALTER TABLE Belief_YieldChangeTradeRoute_FIX RENAME TO Belief_YieldChangeTradeRoute;

--------------------------------------------------------------------------------
-- END Belief_YieldChangeTradeRoute
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Belief_YieldChangeWorldWonder
--------------------------------------------------------------------------------

CREATE TABLE Belief_YieldChangeWorldWonder_FIX (
	`BeliefType` text REFERENCES Beliefs (Type),
	`YieldType` text REFERENCES Yields (Type),
	`Yield` integer DEFAULT 0
);

INSERT INTO Belief_YieldChangeWorldWonder_FIX
	(BeliefType, YieldType, Yield)
SELECT
	BeliefType, YieldType, Yield
FROM Belief_YieldChangeWorldWonder;

DROP TABLE Belief_YieldChangeWorldWonder;
ALTER TABLE Belief_YieldChangeWorldWonder_FIX RENAME TO Belief_YieldChangeWorldWonder;

--------------------------------------------------------------------------------
-- END Belief_YieldChangeWorldWonder
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START Building_BuildingClassYieldChanges
--------------------------------------------------------------------------------

CREATE TABLE Building_BuildingClassYieldChanges_FIX (
	`BuildingType` text REFERENCES Buildings (Type),
	`BuildingClassType` text REFERENCES BuildingClasses (Type),
	`YieldType` text REFERENCES Yields (Type),
	`YieldChange` integer DEFAULT 0
);

INSERT INTO Building_BuildingClassYieldChanges_FIX
	(BuildingType, BuildingClassType, YieldType, YieldChange)
SELECT
	BuildingType, BuildingClassType, YieldType, YieldChange
FROM Building_BuildingClassYieldChanges;

DROP TABLE Building_BuildingClassYieldChanges;
ALTER TABLE Building_BuildingClassYieldChanges_FIX RENAME TO Building_BuildingClassYieldChanges;

--------------------------------------------------------------------------------
-- END Building_BuildingClassYieldChanges
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- Values stored with the wrong type
--------------------------------------------------------------------------------



--------------------------------------------------------------------------------
-- START MovementRates
--------------------------------------------------------------------------------

-- CurveRoll is declared float but holds C-style literals like '1.3f'
UPDATE MovementRates
SET CurveRoll = CAST(REPLACE(CurveRoll, 'f', '') AS real)
WHERE typeof(CurveRoll) = 'text';

--------------------------------------------------------------------------------
-- END MovementRates
--------------------------------------------------------------------------------
