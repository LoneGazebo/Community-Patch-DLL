-- Clear tables
DELETE FROM Civilization_Start_Along_Ocean;
DELETE FROM Civilization_Start_Along_River;
DELETE FROM Civilization_Start_Region_Priority;
DELETE FROM Civilization_Start_Region_Avoid;

-- Start biases
INSERT INTO Civilization_Start_Along_Ocean
	(CivilizationType, StartAlongOcean)
VALUES
	('CIVILIZATION_BYZANTIUM', '1'),   -- Classical UU needs coast (historical)
	('CIVILIZATION_ENGLAND', '1'),     -- UA + Renaissance UU wants coast (historical)
	('CIVILIZATION_POLYNESIA', '1'),   -- UA wants coast, UI needs coast (historical)
	('CIVILIZATION_DENMARK', '1'),     -- UA + Classical UU want coast (historical)
	('CIVILIZATION_CARTHAGE', '1'),    -- UA + Classical UU want coast (historical)
	('CIVILIZATION_PORTUGAL', '1'),    -- UA + Medieval UU want coast, UI needs coast (historical)
	('CIVILIZATION_VENICE', '1'),      -- UA wants coast (historical)
	('CIVILIZATION_INDONESIA', '1'),   -- UI wants coast + Medieval UU needs coast (historical)
	('CIVILIZATION_OTTOMAN', '1');     -- Unique GPTI needs coast (historical)

INSERT INTO Civilization_Start_Along_River
	(CivilizationType, StartAlongRiver)
VALUES
	('CIVILIZATION_SONGHAI', '1');     -- UA wants river (historical)

INSERT INTO Civilization_Start_Region_Priority
	(CivilizationType, RegionType)
VALUES
	('CIVILIZATION_ARABIA', 'REGION_DESERT'),   -- UA wants Petra (historical)
	('CIVILIZATION_EGYPT', 'REGION_DESERT'),    -- Classical UB wants desert (historical)
	('CIVILIZATION_MOROCCO', 'REGION_DESERT'),  -- UU benefits from desert (+CS in Open, ignores Desert movement), UA wants Petra (historical)

	('CIVILIZATION_IROQUOIS', 'REGION_FOREST'), -- UA + Classical UU wants forest/jungle (historical)
	('CIVILIZATION_IROQUOIS', 'REGION_JUNGLE'),
	('CIVILIZATION_BRAZIL', 'REGION_JUNGLE'),   -- UI needs jungle/forest (historical)
	('CIVILIZATION_BRAZIL', 'REGION_FOREST'),
	('CIVILIZATION_MAYA', 'REGION_JUNGLE'),     -- UI needs jungle/forest (historical)
	('CIVILIZATION_MAYA', 'REGION_FOREST'),
	('CIVILIZATION_SIAM', 'REGION_JUNGLE'),     -- UB needs jungle/forest (historical)
	('CIVILIZATION_SIAM', 'REGION_FOREST'),

	('CIVILIZATION_HUNS', 'REGION_GRASS'),      -- UA wants horses, UI needs flat land (historical)
	('CIVILIZATION_HUNS', 'REGION_PLAINS'),
	('CIVILIZATION_MONGOL', 'REGION_GRASS'),    -- UA wants horses, Ancient UB can utilize Sheep/Cattle/Horses in addition to other bonus resources (historical)
	('CIVILIZATION_MONGOL', 'REGION_PLAINS'),

	('CIVILIZATION_SWEDEN', 'REGION_TUNDRA'),   -- UB wants tundra (historical)

	('CIVILIZATION_ROME', 'REGION_HILLS');      -- UU wants Iron, UA needs early production (historical)

INSERT INTO Civilization_Start_Region_Avoid
	(CivilizationType, RegionType)
VALUES
	('CIVILIZATION_CHINA', 'REGION_HILLS'),     -- UI needs flat land
	('CIVILIZATION_INDIA', 'REGION_HILLS'),     -- Classical UB wants farms
	('CIVILIZATION_SHOSHONE', 'REGION_HILLS'),  -- UI needs flat land (historical)
	('CIVILIZATION_POLAND', 'REGION_JUNGLE'),   -- Medieval UB wants Sheep/Cattle/Horses
	('CIVILIZATION_POLAND', 'REGION_FOREST');

INSERT INTO Civilization_Start_Prefer_Mountain
	(CivilizationType, StartPreferMountain)
VALUES
	('CIVILIZATION_INCA', 1);                   -- UA wants Mountains (historical)
