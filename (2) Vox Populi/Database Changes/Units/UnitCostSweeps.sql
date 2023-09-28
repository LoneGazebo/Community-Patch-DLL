-- Note that CS/Policy-exclusive units already have their own UnitClass here

-- Default
UPDATE Units SET Cost = -1, FaithCost = 0;

-- Settlers have 0 cost (special)
UPDATE Units SET Cost = 0 WHERE CombatClass = 'UNITCOMBAT_SETTLER';

-- Civilians
UPDATE Units SET Cost = 80 WHERE Class = 'UNITCLASS_WORKER';
UPDATE Units SET Cost = 40 WHERE Class = 'UNITCLASS_WORKBOAT';
UPDATE Units SET Cost = 100, FaithCost = 200 WHERE Class = 'UNITCLASS_ASSYRIAN_SIEGE_TOWER';
UPDATE Units SET Cost = 450, FaithCost = 450 WHERE Class = 'UNITCLASS_ARCHAEOLOGIST';

-- Trade and diplo unit costs scale with era
UPDATE Units SET Cost = 90, ProductionCostAddedPerEra = 75 WHERE Class = 'UNITCLASS_CARAVAN';
UPDATE Units SET Cost = 140, ProductionCostAddedPerEra = 75 WHERE Class = 'UNITCLASS_CARGO_SHIP';
UPDATE Units SET Cost = 100, ProductionCostAddedPerEra = 150 WHERE CombatClass = 'UNITCOMBAT_DIPLOMACY';

-- Religious units
UPDATE Units SET FaithCost = 200 WHERE CombatClass = 'UNITCOMBAT_MISSIONARY';
UPDATE Units SET FaithCost = 300 WHERE CombatClass = 'UNITCOMBAT_INQUISITOR';

-- Missiles and nukes
UPDATE Units SET Cost = 1200 WHERE Class = 'UNITCLASS_GUIDED_MISSILE';
UPDATE Units SET Cost = 3500 WHERE Class = 'UNITCLASS_ATOMIC_BOMB';
UPDATE Units SET Cost = 5600 WHERE Class = 'UNITCLASS_NUCLEAR_MISSILE';

-- Spaceship parts
UPDATE Units SET Cost = 8000 WHERE SpaceshipProject IS NOT NULL;

-- Military units
CREATE TEMP TABLE TechTier_UnitCosts (
	TechTier INTEGER,
	LandBasicCost INTEGER,
	LandAdvancedCost INTEGER,
	UnitFaithCost INTEGER
);

INSERT INTO TechTier_UnitCosts
VALUES
	(0, 40, 45, 100),
	(1, 50, 55, 100),
	(2, 70, 90, 150),
	(3, 90, 100, 200),
	(4, 110, 130, 250),
	(5, 135, 175, 300),
	(6, 160, 200, 350),
	(7, 300, 350, 400),
	(8, 325, 350, 500),
	(9, 625, 900, 600),
	(10, 700, 800, 700),
	(11, 900, 1000, 800),
	(12, 950, 1300, 900),
	(13, 1300, 1800, 1000),
	(14, 1500, 2000, 1200),
	(15, 1800, 2250, 1400),
	(16, 2600, 3000, 1600),
	(17, 2600, 3000, 1600);

-- Melee, Ranged, Recon
UPDATE Units
SET Cost = (
	SELECT LandBasicCost FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
)
WHERE EXISTS (
	SELECT 1 FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
) AND CombatClass IN (
	'UNITCOMBAT_MELEE',
	'UNITCOMBAT_GUN',
	'UNITCOMBAT_ARCHER',
	'UNITCOMBAT_RECON'
) AND IsMounted = 0 AND MinorCivGift = 0;

-- Mounted Melee, Mounted Ranged, Siege
UPDATE Units
SET Cost = (
	SELECT LandAdvancedCost FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
)
WHERE EXISTS (
	SELECT 1 FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
) AND (
	CombatClass IN (
		'UNITCOMBAT_MOUNTED',
		'UNITCOMBAT_ARMOR',
		'UNITCOMBAT_SIEGE'
	) OR IsMounted = 1
) AND MinorCivGift = 0;

-- Faith costs
UPDATE Units
SET FaithCost = (
	SELECT UnitFaithCost FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
)
WHERE EXISTS (
	SELECT 1 FROM TechTier_UnitCosts WHERE PrereqTech IN (
		SELECT Type FROM Technologies WHERE GridX = TechTier
	)
)
AND CombatClass IN (SELECT Type FROM UnitCombatInfos WHERE IsMilitary = 1)
AND PurchaseOnly = 0
AND MinorCivGift = 0;

DROP TABLE TechTier_UnitCosts;

-- Ships and planes
CREATE TEMP TABLE UnitClass_Costs (
	UnitClass TEXT,
	UnitCost INTEGER
);

INSERT INTO UnitClass_Costs
VALUES
	-- Naval Melee
	('UNITCLASS_GALLEY', 70),
	('UNITCLASS_TRIREME', 120),
	('UNITCLASS_CARAVEL', 160),
	('UNITCLASS_PRIVATEER', 350),
	('UNITCLASS_IRONCLAD', 900),
	('UNITCLASS_DESTROYER', 1600),
	('UNITCLASS_MISSILE_CRUISER', 2400),
	-- Naval Ranged
	('UNITCLASS_BYZANTINE_DROMON', 100),
	('UNITCLASS_GALLEASS', 175),
	('UNITCLASS_FRIGATE', 375),
	('UNITCLASS_CRUISER', 900),
	('UNITCLASS_BATTLESHIP', 1800),
	-- Submarine
	('UNITCLASS_SUBMARINE', 1600),
	('UNITCLASS_NUCLEAR_SUBMARINE', 2250),
	-- Carrier
	('UNITCLASS_CARRIER', 1800),
	-- Fighter
	('UNITCLASS_TRIPLANE', 800),
	('UNITCLASS_FIGHTER', 1400),
	('UNITCLASS_ZERO', 1400),
	('UNITCLASS_JET_FIGHTER', 2100),
	-- Bomber
	('UNITCLASS_WWI_BOMBER', 850),
	('UNITCLASS_BOMBER', 1500),
	('UNITCLASS_B17', 1500),
	('UNITCLASS_STEALTH_BOMBER', 2200);

UPDATE Units
SET Cost = (SELECT UnitCost FROM UnitClass_Costs WHERE UnitClass = Class)
WHERE EXISTS (SELECT 1 FROM UnitClass_Costs WHERE UnitClass = Class);

DROP TABLE UnitClass_Costs;

-- Outliers
UPDATE Units SET Cost = 40, FaithCost = 100 WHERE Class = 'UNITCLASS_WARRIOR'; -- no prereq, so tech tier table above won't work
UPDATE Units SET Cost = 40, FaithCost = 100 WHERE Class = 'UNITCLASS_PATHFINDER'; -- no prereq, so tech tier table above won't work
UPDATE Units SET Cost = 125 WHERE Class = 'UNITCLASS_ELEPHANT_RIDER';
UPDATE Units SET Cost = 100 WHERE Class = 'UNITCLASS_SWORDSMAN';
UPDATE Units SET Cost = 160 WHERE Class = 'UNITCLASS_LONGSWORDSMAN';
UPDATE Units SET Cost = 125 WHERE Class = 'UNITCLASS_FCOMPANY';
UPDATE Units SET Cost = 550 WHERE Class = 'UNITCLASS_FOREIGNLEGION';
UPDATE Units SET Cost = 1750 WHERE Class = 'UNITCLASS_MARINE';
UPDATE Units SET Cost = 2500 WHERE Class = 'UNITCLASS_MODERN_ARMOR';
UPDATE Units SET Cost = 2600 WHERE Class = 'UNITCLASS_XCOM_SQUAD';

-- Unique Units
UPDATE Units SET Cost = 100 WHERE Type = 'UNIT_MAYAN_ATLATLIST'; -- cheaper and earlier
UPDATE Units SET Cost = 135, FaithCost = 300 WHERE Type = 'UNIT_DANISH_BERSERKER'; -- earlier but same cost
UPDATE Units SET Cost = 140, FaithCost = 280 WHERE Type = 'UNIT_POLYNESIAN_MAORI_WARRIOR'; -- cheaper
UPDATE Units SET Cost = 275, FaithCost = 350 WHERE Type = 'UNIT_GERMAN_LANDSKNECHT'; -- cheaper
UPDATE Units SET Cost = 325, FaithCost = 500 WHERE Type = 'UNIT_OTTOMAN_JANISSARY'; -- earlier but same cost
UPDATE Units SET Cost = 350, FaithCost = 500 WHERE Type = 'UNIT_INDIAN_WARELEPHANT'; -- earlier but same cost
UPDATE Units SET Cost = 600, FaithCost = 550 WHERE Type = 'UNIT_ETHIOPIAN_MEHAL_SEFARI'; -- cheaper
UPDATE Units SET FaithCost = 500 WHERE Type = 'UNIT_DUTCH_SEA_BEGGAR'; -- earlier but same cost