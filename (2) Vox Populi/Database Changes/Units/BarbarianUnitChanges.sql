-- Brute (Warrior)

-- Spearman
UPDATE Units
SET
ShowInPedia = 1
WHERE Type = 'UNIT_BARBARIAN_SPEARMAN';

-- Swordsman
DELETE FROM Unit_BuildingClassPurchaseRequireds WHERE UnitType = 'UNIT_BARBARIAN_SWORDSMAN';

UPDATE Units
SET Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_SWORDSMAN') - 1,
ShowInPedia = 1
WHERE Type = 'UNIT_BARBARIAN_SWORDSMAN';

-- Handaxe (Slinger)
UPDATE Civilization_UnitClassOverrides SET UnitClassType = 'UNITCLASS_SLINGER' WHERE UnitType = 'UNIT_BARBARIAN_AXMAN';

-- Archer
UPDATE Units
SET Combat = (SELECT Combat FROM Units WHERE Type = 'UNIT_ARCHER') - 1,
ShowInPedia = 1
WHERE Type = 'UNIT_BARBARIAN_ARCHER';

-- Horseman
UPDATE Units
SET
ShowInPedia = 1
WHERE Type = 'UNIT_BARBARIAN_HORSEMAN';

-- Galley
