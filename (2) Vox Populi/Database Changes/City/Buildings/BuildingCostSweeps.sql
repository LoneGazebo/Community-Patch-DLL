-- Buildings (not World Wonders) are cheaper for every era passed
UPDATE Eras SET LaterEraBuildingConstructMod = -2;

-- Set non-Wonder costs by tech tier

-- Default value
UPDATE Buildings
SET Cost = -1, GoldMaintenance = 0, FaithCost = 0;

-- No prereq
UPDATE Buildings
SET Cost = 65, GoldMaintenance = 0
WHERE BuildingClass = 'BUILDINGCLASS_MONUMENT';

CREATE TEMP TABLE BuildingCost (
	GridXTemp INTEGER,
	CostTemp INTEGER,
	GoldMaintenanceTemp INTEGER
);

INSERT INTO BuildingCost
VALUES
	(0, 65, 0),
	(1, 65, 1),
	(2, 110, 1),
	(3, 150, 1),
	(4, 200, 1),
	(5, 300, 2),
	(6, 350, 2),
	(7, 500, 4),
	(8, 600, 4),
	(9, 1000, 5),
	(10, 1250, 6),
	(11, 1800, 7),
	(12, 2000, 7),
	(13, 2250, 8),
	(14, 2250, 8),
	(15, 2500, 10),
	(16, 2750, 10);

UPDATE Buildings
SET
	Cost = (
		SELECT CostTemp FROM BuildingCost WHERE PrereqTech IN (
			SELECT Type FROM Technologies WHERE GridX = GridXTemp
		)
	),
	GoldMaintenance = (
		SELECT GoldMaintenanceTemp FROM BuildingCost WHERE PrereqTech IN (
			SELECT Type FROM Technologies WHERE GridX = GridXTemp
		)
	)
WHERE WonderSplashImage IS NULL
AND BuildingClass NOT IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances = 1
)
AND EXISTS (
	SELECT 1 FROM BuildingCost WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
);

-- National Wonders
UPDATE Buildings
SET
	Cost = (
		SELECT CostTemp FROM BuildingCost WHERE PrereqTech IN (
			SELECT Type FROM Technologies WHERE GridX = GridXTemp
		)
	) * 100 / 250,
	GoldMaintenance = 0
WHERE BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances = 1
)
AND EXISTS (
	SELECT 1 FROM BuildingCost WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
);

-- Ideology National Wonders should have the same costs
UPDATE Buildings
SET Cost = (SELECT CostTemp FROM BuildingCost WHERE GridXTemp = 12) * 100 / 250
WHERE BuildingClass IN (
	SELECT Type FROM BuildingClasses
	WHERE MaxPlayerInstances = 1
) AND PolicyBranchType IS NOT NULL;

DROP TABLE BuildingCost;

-- World Wonders
CREATE TEMP TABLE WorldWonderCost (
	GridXTemp INTEGER,
	CostTemp INTEGER
);

INSERT INTO WorldWonderCost
VALUES
	(1, 150),
	(2, 185),
	(3, 200),
	(4, 250),
	(5, 400),
	(6, 500),
	(7, 800),
	(8, 900),
	(9, 1000),
	(10, 1250),
	(11, 1600),
	(12, 1700),
	(13, 1900),
	(14, 2150),
	(15, 2300),
	(16, 3000),
	(17, 3250);

UPDATE Buildings
SET Cost = (
	SELECT CostTemp FROM WorldWonderCost WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
)
WHERE WonderSplashImage IS NOT NULL
AND EXISTS (
	SELECT 1 FROM WorldWonderCost WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = GridXTemp
	)
);

DROP TABLE WorldWonderCost;

-- World Congress
UPDATE Buildings
SET Cost = -1, GoldMaintenance = 0
WHERE UnlockedByLeague = 1;

-- Corporations
UPDATE Buildings
SET Cost = 1300, GoldMaintenance = 4
WHERE BuildingClass IN (
	SELECT HeadquartersBuildingClass FROM Corporations
);

UPDATE Buildings
SET Cost = 900, GoldMaintenance = 6
WHERE BuildingClass IN (
	SELECT OfficeBuildingClass FROM Corporations
);

UPDATE Buildings
SET Cost = -1, GoldMaintenance = 1
WHERE BuildingClass IN (
	SELECT FranchiseBuildingClass FROM Corporations
);

-- Outliers

-- Council, Agribusiness, guild support, and Gold buildings
UPDATE Buildings
SET GoldMaintenance = 0
WHERE BuildingClass IN (
	'BUILDINGCLASS_GROVE',
	'BUILDINGCLASS_STOCKYARD',
	'BUILDINGCLASS_AMPHITHEATER',
	'BUILDINGCLASS_GALLERY',
	'BUILDINGCLASS_OPERA_HOUSE',
	'BUILDINGCLASS_MARKET',
	'BUILDINGCLASS_CARAVANSARY',
	'BUILDINGCLASS_MINT',
	'BUILDINGCLASS_BANK',
	'BUILDINGCLASS_STOCK_EXCHANGE'
);

-- Maintenance-free Unique Buildings
UPDATE Buildings
SET GoldMaintenance = 0
WHERE Type IN (
	'BUILDING_SATRAPS_COURT',
	'BUILDING_BASILICA'
);

-- Herbalist
UPDATE Buildings
SET Cost = 65
WHERE BuildingClass = 'BUILDINGCLASS_HERBALIST';

-- Non-unique Courthouse, Garden and Chancery
UPDATE Buildings
SET GoldMaintenance = 3
WHERE Type IN (
	'BUILDING_COURTHOUSE',
	'BUILDING_GARDEN',
	'BUILDING_CHANCERY'
);

-- Skola
UPDATE Buildings
SET Cost = 800, GoldMaintenance = 4
WHERE Type = 'BUILDING_SKOLA';

-- Museum, Military Academy
UPDATE Buildings
SET GoldMaintenance = 6
WHERE BuildingClass IN (
	'BUILDINGCLASS_MUSEUM',
	'BUILDINGCLASS_MILITARY_ACADEMY'
);

-- Mine Field
UPDATE Buildings
SET Cost = 1600
WHERE BuildingClass = 'BUILDINGCLASS_MINEFIELD';

-- Interpretive Center, Solar Power Plant
UPDATE Buildings
SET Cost = 2250
WHERE BuildingClass IN (
	'BUILDINGCLASS_INTERPRETIVE_CENTER',
	'BUILDINGCLASS_SOLAR_PLANT'
);

-- Unique National Wonders that unlock earlier (but should not be cheaper)
UPDATE Buildings SET Cost = 80 WHERE Type IN ('BUILDING_ROYAL_LIBRARY', 'BUILDING_PIAZZA_SAN_MARCO');
UPDATE Buildings SET Cost = 140 WHERE Type = 'BUILDING_GREAT_COTHON';

-- Faith Costs

-- Culture line (Faith of the Masses)
UPDATE Buildings SET FaithCost = 300, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_AMPHITHEATER';
UPDATE Buildings SET FaithCost = 500, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_OPERA_HOUSE';
UPDATE Buildings SET FaithCost = 600, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_MUSEUM';
UPDATE Buildings SET FaithCost = 800, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_BROADCAST_TOWER';

-- Science line (Divine Teachings)
UPDATE Buildings SET FaithCost = 300, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_LIBRARY';
UPDATE Buildings SET FaithCost = 400, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_UNIVERSITY';
UPDATE Buildings SET FaithCost = 600, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_PUBLIC_SCHOOL';
UPDATE Buildings SET FaithCost = 800, UnlockedByBelief = 1 WHERE BuildingClass = 'BUILDINGCLASS_LABORATORY';

-- Religious buildings
UPDATE Buildings SET Cost = -1, FaithCost = 200
WHERE BuildingClass IN (
	'BUILDINGCLASS_MONASTERY',
	'BUILDINGCLASS_CATHEDRAL',
	'BUILDINGCLASS_MOSQUE',
	'BUILDINGCLASS_PAGODA',
	'BUILDINGCLASS_STUPA',
	'BUILDINGCLASS_CHURCH',
	'BUILDINGCLASS_MANDIR',
	'BUILDINGCLASS_SYNAGOGUE',
	'BUILDINGCLASS_ORDER',
	'BUILDINGCLASS_TEOCALLI',
	'BUILDINGCLASS_GURDWARA'
);

-- Reformation buildings
UPDATE Buildings SET Cost = 125 WHERE IsReformation = 1;
