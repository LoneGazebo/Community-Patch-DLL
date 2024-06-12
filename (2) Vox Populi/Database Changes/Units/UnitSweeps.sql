-- Default values
UPDATE Units
SET
	GoodyHutUpgradeUnitClass = NULL,
	MilitarySupport = 0,
	NoSupply = 0,
	MilitaryProduction = 0,
	Pillage = 0,
	PurchaseCooldown = 1,
	HurryCostModifier = 0,
	RequiresFaithPurchaseEnabled = 0,
	MinAreaSize = 0,
	AirUnitCap = 0,
	-- unused
	IgnoreBuildingDefense = 0,
	NoBadGoodies = 0,
	-- disabled
	Mechanized = 0,
	AdvancedStartCost = 0,
	CombatLimit = 0,
	RangedCombatLimit = 0,
	XPValueAttack = 0,
	XPValueDefense = 0,
	Conscription = 0;

-----------------------------------------------------------------
-- Ancient Ruin upgrade class (Recon only)
-----------------------------------------------------------------
UPDATE Units
SET GoodyHutUpgradeUnitClass = (SELECT UnitClassType FROM Unit_ClassUpgrades WHERE UnitType = Type)
WHERE CombatClass = 'UNITCOMBAT_RECON';

-----------------------------------------------------------------
-- Does this unit cost supply, and can benefit from military
-- production bonuses from buildings and policies?
-----------------------------------------------------------------
UPDATE Units
SET MilitarySupport = 1, MilitaryProduction = 1
WHERE CombatClass IN (
	SELECT Type FROM UnitCombatInfos WHERE IsMilitary = 1
) OR Class IN (
	'UNITCLASS_ASSYRIAN_SIEGE_TOWER',
	'UNITCLASS_ROCKET_MISSILE',
	'UNITCLASS_GUIDED_MISSILE',
	'UNITCLASS_ATOMIC_BOMB',
	'UNITCLASS_NUCLEAR_MISSILE'
);

-- Air units and missiles don't cost supply
UPDATE Units
SET NoSupply = 1
WHERE CombatClass IN (
	SELECT Type FROM UnitCombatInfos WHERE IsAerial = 1
) OR Class IN (
	'UNITCLASS_ROCKET_MISSILE',
	'UNITCLASS_GUIDED_MISSILE'
);

-----------------------------------------------------------------
-- Can this unit pillage improvements?
-----------------------------------------------------------------
UPDATE Units SET Pillage = 1
WHERE CombatClass IN (
	SELECT Type FROM UnitCombatInfos WHERE IsMilitary = 1 AND IsAerial = 0
);

-----------------------------------------------------------------
-- Purchase cooldowns
-----------------------------------------------------------------

-- No cooldown for units that can move immediately
UPDATE Units SET PurchaseCooldown = 0 WHERE MoveAfterPurchase = 1;

UPDATE Units
SET PurchaseCooldown = 5
WHERE CombatClass IN (
	'UNITCOMBAT_WORKER',
	'UNITCOMBAT_WORKBOAT',
	'UNITCOMBAT_CARAVAN',
	'UNITCOMBAT_CARGO',
	'UNITCOMBAT_DIPLOMACY'
);

-- Great People and Spaceship parts
UPDATE Units
SET GlobalFaithPurchaseCooldown = 5
WHERE Special = 'SPECIALUNIT_PEOPLE'
OR SpaceshipProject IS NOT NULL;

-----------------------------------------------------------------
-- Hurry Cost Modifier: how much it costs to purchase
-----------------------------------------------------------------

-- Unbuyables
UPDATE Units
SET HurryCostModifier = -1
WHERE Class IN (
	'UNITCLASS_SETTLER',
	'UNITCLASS_PIONEER',
	'UNITCLASS_COLONIST',
	'UNITCLASS_ARCHAEOLOGIST',
	'UNITCLASS_ATOMIC_BOMB',
	'UNITCLASS_NUCLEAR_MISSILE'
);

-- Spaceship parts
UPDATE Units
SET HurryCostModifier = 25
WHERE SpaceshipProject IS NOT NULL;

-----------------------------------------------------------------
-- RequiresFaithPurchaseEnabled: only applicable to land/air units
-----------------------------------------------------------------

-- All non-naval military units except mercenary units
UPDATE Units
SET RequiresFaithPurchaseEnabled = 1
WHERE CombatClass IN (SELECT Type FROM UnitCombatInfos WHERE IsMilitary = 1 AND IsNaval = 0)
AND PurchaseOnly = 0;

-- Siege Tower and Archaeologist can also be faith purchased
UPDATE Units SET RequiresFaithPurchaseEnabled = 1 WHERE Class IN ('UNITCLASS_ASSYRIAN_SIEGE_TOWER', 'UNITCLASS_ARCHAEOLOGIST');

-----------------------------------------------------------------
-- Minimum size of adjacent water body to train this unit
-----------------------------------------------------------------
UPDATE Units
SET MinAreaSize = (
	SELECT Value FROM Defines
	WHERE Name = 'MIN_WATER_SIZE_FOR_OCEAN'
) WHERE CombatClass IN (
	SELECT Type FROM UnitCombatInfos WHERE IsNaval = 1
) OR CombatClass IN (
	'UNITCOMBAT_WORKBOAT',
	'UNITCOMBAT_CARGO'
);

-----------------------------------------------------------------
-- Number of air slots this unit takes
-----------------------------------------------------------------
UPDATE Units
SET AirUnitCap = 1
WHERE CombatClass IN (
	'UNITCOMBAT_FIGHTER',
	'UNITCOMBAT_BOMBER'
) OR Class IN (
	'UNITCLASS_ROCKET_MISSILE',
	'UNITCLASS_GUIDED_MISSILE',
	'UNITCLASS_ATOMIC_BOMB',
	'UNITCLASS_NUCLEAR_MISSILE'
)
