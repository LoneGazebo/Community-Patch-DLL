-- Brute (Warrior)

-- Spearman

-- Swordsman
DELETE FROM Unit_BuildingClassPurchaseRequireds WHERE UnitType = 'UNIT_BARBARIAN_SWORDSMAN';

UPDATE Units
SET Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN') - 1
WHERE Type = 'UNIT_BARBARIAN_SWORDSMAN';

-- Handaxe (Slinger)
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_VP_SLINGER' WHERE UnitType = 'UNIT_BARBARIAN_AXMAN';

-- Archer
UPDATE Units
SET Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_ARCHER') - 1
WHERE Type = 'UNIT_BARBARIAN_ARCHER';

-- Horseman

-- Galley
