-- Bad goodies are never triggered even in BNW, so we can remove this ability to reduce clutter
UPDATE Units SET NoBadGoodies = 0;

-- With the current city strength formula, buildings make up a large chunk of city strength and this ability is too strong
-- This ability has been restored in DLL and should be fully functional (without explicit AI support)
UPDATE Units SET IgnoreBuildingDefense = 0;
