-- TODO: Documentation

ALTER TABLE Units ADD CargoCombat integer DEFAULT 0;

-- Adds ability for units to have max HP values other than 100 (whoward)
ALTER TABLE Units ADD MaxHitPoints integer DEFAULT 100;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD FoundMid boolean DEFAULT 0;

-- Adds ability for settlers to get free buildings when a city is founded.
ALTER TABLE Units ADD FoundLate boolean DEFAULT 0;

-- Adds ability for Venetian merchants to create puppets.
ALTER TABLE Units ADD FoundColony integer DEFAULT 0;

-- Adds marker for city attack only units (for AI)
ALTER TABLE Units ADD CityAttackOnly boolean DEFAULT 0;

-- Adds Culture from experience to owner of unit when disbanded or upgraded
ALTER TABLE Units ADD CulExpOnDisbandUpgrade boolean DEFAULT 0;

-- Increases the cost by this amount every time you build the unit
ALTER TABLE Units ADD CostScalerNumRepeats integer DEFAULT 0;

-- Tourism Bomb can now alternative give 50% of tourism to all civs, and 100% of tourism output to target civ for x turns
ALTER TABLE Units ADD TourismBonusTurns integer DEFAULT 0;

-- Allows for Unit to be unlocked by a specific resource being owned (can be strategic or luxury)
ALTER TABLE Units ADD ResourceType text REFERENCES Resources (Type);

-- Assigns a unit to a belief type. If you lack this belief in the city where you wish to buy a unit, you won't be able to buy it with faith.
ALTER TABLE Units ADD BeliefRequired text REFERENCES Beliefs (Type);

-- Allows for Unit to be purchased in puppet city
ALTER TABLE Units ADD PuppetPurchaseOverride boolean DEFAULT 0;

-- Allows for Unit to gain more yields and experience from a ruin
ALTER TABLE Units ADD GoodyModifier integer DEFAULT 0;
-- Allows for Unit to increase your supply cap.
ALTER TABLE Units ADD SupplyCapBoost integer DEFAULT 0;

-- Gain Production from best X cities from previous turns - Replacement for Population-based Hurry Production on Great Engineers, etc.
ALTER TABLE Units ADD BaseProductionTurnsToCount integer DEFAULT 0;
-- Gain Gold from previous turns - Replacement for Era-based Gold Trade Missions on Great Merchants, etc.
ALTER TABLE Units ADD BaseGoldTurnsToCount integer DEFAULT 0;
-- Gain GAP from previous turns - Replacement for free GA on Great Artists, etc.
ALTER TABLE Units ADD BaseTurnsForGAPToCount integer DEFAULT 0;
-- Gain turns of WLTKD - Replacement for Influence from Trade Missions on Great Merchants, etc.
ALTER TABLE Units ADD BaseWLTKDTurns integer DEFAULT 0;
ALTER TABLE Units ADD NumFreeLux integer DEFAULT 0;

-- GP tabling - use this to define up to 5 GP types with indpendent meters (will not affect rates of other GPs)
-- GPs that you assign the same number to will affect each other, so make sure no other mods are using these!
ALTER TABLE Units ADD GPExtra integer DEFAULT 0;
-- Example: <GPExtra>1</GPExtra> in a unit's XML table will put all of its GPP into a unique meter and a faith buy track.

-- Unit gains this promotion in Friendly Lands.
ALTER TABLE Units ADD FriendlyLandsPromotion text REFERENCES UnitPromotions (Type);
ALTER TABLE Units ADD IsConvertUnit boolean DEFAULT 0;

-- relates to Great Artist and Great Writer scaling bonuses
ALTER TABLE Units ADD ScaleFromNumGWs integer DEFAULT 0;
ALTER TABLE Units ADD ScaleFromNumThemes integer DEFAULT 0;

-- How many culture bombs can this unit do, must have set CultureBombRadius to a number.
ALTER TABLE Units ADD NumberOfCultureBombs integer DEFAULT 0;

-- Cooldowns for Units/Buildings
ALTER TABLE Units ADD PurchaseCooldown integer DEFAULT 0;
-- Affects only the city the unit is purchased from (works like PurchaseCooldown for faith purchases)
ALTER TABLE Units ADD LocalFaithPurchaseCooldown integer DEFAULT 0;
-- Affects Faith purchases for all faith buys in all cities.
ALTER TABLE Units ADD GlobalFaithPurchaseCooldown integer DEFAULT 0;

ALTER TABLE Units ADD BaseLandAirDefense integer DEFAULT 0;

-- Adds Mounted trait to a unit that isn't in the unitcombat_mounted combat class
ALTER TABLE Units ADD IsMounted boolean DEFAULT 0;

ALTER TABLE Units ADD NoSupply boolean DEFAULT 0;

-- Unit Upgrades for free when reaching it's UnitClass Upgrade Tech
ALTER TABLE Units ADD FreeUpgrade boolean DEFAULT 0;

-- Unit gets a new Combat Strength in specific Era, must be set to 'true' or 1, if using Unit_EraCombatStrength, and Unit_EraCombatStrength must be filled in.
ALTER TABLE Units ADD UnitEraUpgrade boolean DEFAULT 0;

-- Special Units that have a different Special rating can be modified here to load on to ships (e.g. Great People).
ALTER TABLE Units ADD SpecialUnitCargoLoad text REFERENCES SpecialUnits (Type);

-- Can this unit only be trained during War?
ALTER TABLE Units ADD WarOnly boolean DEFAULT 0;

-- Start a WLTKD when this unit is born or gained. GP's only.
ALTER TABLE Units ADD WLTKDFromBirth boolean DEFAULT 0;

-- Start a GoldenAge when this unit is born or gained. GP's only.
ALTER TABLE Units ADD GoldenAgeFromBirth boolean DEFAULT 0;

-- Gain a culture boost equal to 4X the culture per turn when this unit is born. GP's only
ALTER TABLE Units ADD CultureBoost boolean DEFAULT 0;

-- Unit gets extra attacks and partial health is restored upon killing an enemy unit.
ALTER TABLE Units ADD ExtraAttackHealthOnKill boolean DEFAULT 0;

-- Unit gains 3x gold when it plunders a Trade Route.
ALTER TABLE Units ADD HighSeaRaider boolean DEFAULT 0;

ALTER TABLE Units ADD NumInfPerEra integer DEFAULT 0;
ALTER TABLE Units ADD RestingPointChange integer DEFAULT 0;
ALTER TABLE Units ADD ProductionCostAddedPerEra integer DEFAULT 0;

-- EVENTS_CAN_MOVE_INTO
ALTER TABLE Units ADD SendCanMoveIntoEvent boolean DEFAULT 0;

-- GLOBAL_CANNOT_EMBARK
ALTER TABLE Units ADD CannotEmbark boolean DEFAULT 0;

-- This unit can only be gifted by a city state as its unique unit (BALANCE_CORE_MINOR_CIV_GIFT)
ALTER TABLE Units ADD MinorCivGift boolean DEFAULT 0;

-- This unit cannot be a city state's unique unit or gifted by city states at all 
ALTER TABLE Units ADD NoMinorCivGift boolean DEFAULT 0;

-- This unit cannot be a city state's unique unit, but can still be gifted by city states if all other requirements are met (GLOBAL_EXCLUDE_FROM_GIFTS)
ALTER TABLE Units ADD NoMinorGifts boolean DEFAULT 0;

-- GLOBAL_MOVE_AFTER_UPGRADE
ALTER TABLE Units ADD MoveAfterUpgrade boolean DEFAULT 0;

-- GLOBAL_PROMOTION_CLASSES
ALTER TABLE Units ADD PromotionClass text REFERENCES UnitCombatInfos (Type);

-- GLOBAL_SEPARATE_GREAT_ADMIRAL
ALTER TABLE Units ADD CanRepairFleet boolean DEFAULT 0;
ALTER TABLE Units ADD CanChangePort boolean DEFAULT 0;

-- GLOBAL_STACKING_RULES
-- Additional units that can stack with this unit 
ALTER TABLE Units ADD NumberStackingUnits integer DEFAULT 0;
ALTER TABLE Units ADD StackCombat integer DEFAULT 0;

-- Amount of units of this class that the player can have per owned city. No limit if -1. Overrides the MaxPlayerInstances column.
ALTER TABLE UnitClasses ADD UnitInstancePerCity integer DEFAULT -1;

-- This name can only be used when the unit spawns in the specified era, or adjacent eras as a fallback
ALTER TABLE Unit_UniqueNames ADD EraType text REFERENCES Eras (Type);

-- Add useful properties to UnitCombatInfos for easy SQL reference
-- Please don't use this to determine properties of individual unit classes/units
ALTER TABLE UnitCombatInfos ADD IsMilitary boolean DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsRanged boolean DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsNaval boolean DEFAULT 0;
ALTER TABLE UnitCombatInfos ADD IsAerial boolean DEFAULT 0;
