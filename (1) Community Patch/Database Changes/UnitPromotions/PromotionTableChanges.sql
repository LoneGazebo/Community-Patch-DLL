-- TODO: Documentation

ALTER TABLE UnitPromotions ADD IgnoreTerrainDamage boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD IgnoreFeatureDamage boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD ExtraTerrainDamage boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD ExtraFeatureDamage boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD MaxHitPointsChange integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD MaxHitPointsModifier integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD PartialHealOnPillage integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AOEDamageOnPillage integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD MinimumRangeRequired integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD MaximumRangeRequired integer DEFAULT 0;

-- Disallows units w/o 'mounted' flag at unit type level from taking a promotion.
ALTER TABLE UnitPromotions ADD MountedOnly boolean DEFAULT 0;

ALTER TABLE UnitPromotions ADD AttackFullyHealedMod integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD AttackAbove50HealthMod integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AttackBelowEqual50HealthMod integer DEFAULT 0;

-- ranged attacks benefit from other units flanking the target
ALTER TABLE UnitPromotions ADD RangedFlankAttack boolean DEFAULT 0;
-- counts as extra units when supporting a flank
ALTER TABLE UnitPromotions ADD ExtraFlankPower integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD SplashDamage integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD AOEDamageOnKill integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AoEWhileFortified integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AoEDamageOnMove integer DEFAULT 0; -- JJ: Do AoE damage when the unit moves

ALTER TABLE UnitPromotions ADD WorkRateMod integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD ReconChange integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD PromotionDuration integer DEFAULT 0;

-- Promotion restricted to tile. Lost if unit leaves it. Only used in conjuction with free promotions from features and terrains (see above)
ALTER TABLE UnitPromotions ADD IsLostOnMove boolean DEFAULT 0;

-- Put the name of the Promotion in this row in order to negate it with another promotion - currently only works with the terrain/feature related promotion functions above!
ALTER TABLE UnitPromotions ADD NegatesPromotion text REFERENCES UnitPromotions (Type);

-- Forced sets the damage the unit will receive from the combat REGARDLESS of how much damage was calculated to be dealt to it.
ALTER TABLE UnitPromotions ADD ForcedDamageValue integer DEFAULT 0;

-- Change reduces damage the unit will receive BASED on how much damage was calculated to be dealt to it. Negative numbers to reduce!
ALTER TABLE UnitPromotions ADD ChangeDamageValue integer DEFAULT 0;

-- A unit cannot be captured by another player (i.e. from prize ships, etc.). Does not apply to civilians.
ALTER TABLE UnitPromotions ADD CannotBeCaptured boolean DEFAULT 0;

-- Units captured by a unit with this promotion gain the conscript promotion (don't count for military supply). Only used together with CaptureDefeatedEnemy
ALTER TABLE UnitPromotions ADD CapturedUnitsConscripted boolean DEFAULT 0;

-- A unit gains a combat bonus VS barbarians
ALTER TABLE UnitPromotions ADD BarbarianCombatBonus integer DEFAULT 0;

-- Ranged units have a % chance to force another unit to retreat, taking additional damage if they cannot do so.
-- Negative means no force retreat but target still takes additional damage (overrides any positive value already on the same unit)
ALTER TABLE UnitPromotions ADD MoraleBreakChance integer DEFAULT 0;

-- Promotion bonuses restricted to Barbarians.
ALTER TABLE UnitPromotions ADD BarbarianOnly boolean DEFAULT 0;

-- Promotion bonuses restricted to City States.
ALTER TABLE UnitPromotions ADD CityStateOnly boolean DEFAULT 0;

-- Promotion grants the same bonus as the Japan UA
ALTER TABLE UnitPromotions ADD StrongerDamaged boolean DEFAULT 0; -- No wounded penalty, instead gains a combat bonus the more damage the unit has
ALTER TABLE UnitPromotions ADD FightWellDamaged boolean DEFAULT 0; -- No wounded penalty, no additional bonus

-- Great General gives extra XP% during a golden age (Persia)
ALTER TABLE UnitPromotions ADD GeneralGoldenAgeExpPercent integer DEFAULT 0;

-- The Following Promotions Require IsNearbyPromotion and NearbyRange to be set. IsNearbyPromotion is an "m_unitsAreaEffectPromotion" Unit
ALTER TABLE UnitPromotions ADD IsNearbyPromotion boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyRange integer DEFAULT 0;
-- Set the Domain that Gets the Bonus
ALTER TABLE UnitPromotions ADD GiveDomain text REFERENCES Domains (Type);

-- Unit gives additional combat strength to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveCombatMod integer DEFAULT 0;

-- Unit Gives HP to additional units if they kill an enemy units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveHPHealedIfEnemyKilled integer DEFAULT 0;

-- Unit Gives additional XP in combat to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveExperiencePercent integer DEFAULT 0;

-- Unit Gives a bonus to outside friendly lands unis? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveOutsideFriendlyLandsModifier integer DEFAULT 0;

-- Unit Gives extra attacks to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveExtraAttacks integer DEFAULT 0;

-- Unit Gives extra defense to nearby units? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveDefenseMod integer DEFAULT 0;

-- Unit gives Invisibility to another Unit? Requires IsNearbyPromotion, NearbyRange, and GiveDomain Set on this Promotion.
ALTER TABLE UnitPromotions ADD GiveInvisibility boolean DEFAULT 0;

-- Unit only gives these effects at the start of the turn (works for GiveExperiencePercent, GiveCombatMod, GiveDefenseMod, GiveInvisibility, GiveOutsideFriendlyLandsModifier, GiveHPHealedIfEnemyKilled, GiveExtraAttacks)
ALTER TABLE UnitPromotions ADD GiveOnlyOnStartingTurn boolean DEFAULT 0;

-- Unit gains Combat modifier when near cities. Requires NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyCityCombatMod integer DEFAULT 0;

-- Unit gains Combat modifier when near friendly cities. Requires NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyFriendlyCityCombatMod integer DEFAULT 0;

-- Unit gains Combat modifier when near enemy cities. Requires NearbyRange Set on this Promotion.
ALTER TABLE UnitPromotions ADD NearbyEnemyCityCombatMod integer DEFAULT 0;

-- Unit Gives extra healing to nearby units? Must have set IsNearbyPromotion, NearbyRange, and GiveDomain for this promotion.
ALTER TABLE UnitPromotions ADD NearbyHealEnemyTerritory integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyHealNeutralTerritory integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyHealFriendlyTerritory integer DEFAULT 0;
-- End

-- Double Movement on Mountains
ALTER TABLE UnitPromotions ADD MountainsDoubleMove boolean DEFAULT 0;

-- Double Movement when next to Rivers
ALTER TABLE UnitPromotions ADD RiverDoubleMove boolean DEFAULT 0;

-- Embarking Costs One Movement Point
ALTER TABLE UnitPromotions ADD EmbarkFlatCost boolean DEFAULT 0;

-- Disembarking Costs One Movement Point
ALTER TABLE UnitPromotions ADD DisembarkFlatCost boolean DEFAULT 0;

-- Admirals can use their repair fleet action multiple times before expending.
ALTER TABLE UnitPromotions ADD NumRepairCharges integer DEFAULT 0;

-- Base damage dealth to adjacent air units after an unintercepted air sweep
ALTER TABLE UnitPromotions ADD GetGroundAttackDamage integer DEFAULT 0;

-- Allows for Unit to increase your supply cap when expended.
ALTER TABLE UnitPromotions ADD MilitaryCapChange integer DEFAULT 0;

-- Combat Modifier for determined range near a defined UnitClass
ALTER TABLE UnitPromotions ADD CombatBonusFromNearbyUnitClass text REFERENCES UnitClasses (Type);
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonusRange integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyUnitClassBonus integer DEFAULT 0;

-- Changes the intercept range against air units (NEW)
ALTER TABLE UnitPromotions ADD AirInterceptRangeChange integer DEFAULT 0;

-- Allows you to Convert a unit when it X plot is a different domain, e.g. A great general becomes a great admiral, etc.
ALTER TABLE UnitPromotions ADD ConvertDomainUnit text REFERENCES Units (Type);
ALTER TABLE UnitPromotions ADD ConvertDomain text REFERENCES Domains (Type);

-- City Gains Wonder Production Modifier while this Unit is stationed in this City
ALTER TABLE UnitPromotions ADD WonderProductionModifier integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD LandAirDefenseBonus integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD DiploMissionInfluence integer DEFAULT 0;

-- OBSOLETE: to be removed in VP5.0, replaced by UnitPromotions_Plagues and UnitPromotions_BlockedPromotions tables
ALTER TABLE UnitPromotions ADD PlagueChance integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD PlaguePromotion text REFERENCES UnitPromotions (Type);
ALTER TABLE UnitPromotions ADD PlagueIDImmunity integer DEFAULT -1;

ALTER TABLE UnitPromotions ADD PlagueID integer DEFAULT -1;
ALTER TABLE UnitPromotions ADD PlaguePriority integer DEFAULT 0;

-- Modify CS/RCS based on terrain roughness
ALTER TABLE UnitPromotions ADD OpenFromMod integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD RoughFromMod integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD NoSupply boolean DEFAULT 0;

-- Promotion grants a unit with XP if stacked with a Great General (or great admiral if a boat)
ALTER TABLE UnitPromotions ADD StackedGreatGeneralXP integer DEFAULT 0;

-- Promotions
ALTER TABLE UnitPromotions ADD GoodyHutYieldBonus integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD GainsXPFromScouting boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD GainsXPFromPillaging boolean DEFAULT 0; -- OBSOLETE: to be removed in VP5.0, replaced by XPFromPillaging
ALTER TABLE UnitPromotions ADD GainsXPFromSpotting boolean DEFAULT 0;

ALTER TABLE UnitPromotions ADD MultiAttackBonus integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD DamageReductionCityAssault integer DEFAULT 0;

-- Note: The below entries are used, e.g. mounting or dismounting a unit, say a Lancer gets below 50 HP "DamageThreshold", and can "dismount" and fortify as an Infantry type unit.

-- Unit will convert to another UnitType. Must define a "DamageThreshold" and the "ConvertDamageOrFullHPUnit" Type
ALTER TABLE UnitPromotions ADD IsConvertOnDamage boolean DEFAULT 0;

-- Unit will convert to the original UnitType when Max Hit Points are restored. Must define "ConvertDamageOrFullHPUnit" Type to the original Unit.
ALTER TABLE UnitPromotions ADD IsConvertOnFullHP boolean DEFAULT 0;

-- Unit will convert to another UnitType if "IsConvertOnDamage" and "DamageThreshold" are defined. If used to convert back to the original unit when full HP is restored, "IsConvertOnFullHp" must be defined.
ALTER TABLE UnitPromotions ADD ConvertDamageOrFullHPUnit text REFERENCES Units (Type);

-- Unit will convert to another UnitType. Must define a "IsConvertOnDamage" and the "ConvertDamageOrFullHPUnit" Type. Or Can be set with IsConvertEnemyUnitToBarbarian
ALTER TABLE UnitPromotions ADD DamageThreshold integer DEFAULT 0;

-- Can this unit convert an enemy unit into a barbarian? Must set DamageThreshold to a value you want enemy to convert.
ALTER TABLE UnitPromotions ADD IsConvertEnemyUnitToBarbarian boolean DEFAULT 0;

-- Promotion grants additional combat strength if on a pillaged improvement
ALTER TABLE UnitPromotions ADD PillageBonusStrength integer DEFAULT 0;

-- Promotion grants additional religious pressure when this unit is garrisoned in the city (if the player has a religion).
ALTER TABLE UnitPromotions ADD ReligiousPressureModifier integer DEFAULT 0;

-- When a unit with this promotion is adjacent to a city, city strength value is boosted by X %.
ALTER TABLE UnitPromotions ADD AdjacentCityDefenseMod integer DEFAULT 0;

-- Traveling Citadel.
ALTER TABLE UnitPromotions ADD NearbyEnemyDamage integer DEFAULT 0;

-- Reduce Enemy Movement at start of turn.
ALTER TABLE UnitPromotions ADD AdjacentEnemySapMovement integer DEFAULT 0;

-- When a unit (civilian or combat) with this promotion is stationed in a City, City gains X% modifier towards building military units.
ALTER TABLE UnitPromotions ADD MilitaryProductionModifier integer DEFAULT 0;

-- Unit gets a CS modifier if not adjacent to any Friendly Unit
ALTER TABLE UnitPromotions ADD NoAdjacentUnitMod integer DEFAULT 0;

-- Unit can't attack while in ocean tiles
ALTER TABLE UnitPromotions ADD NoAttackInOcean boolean DEFAULT 0;

ALTER TABLE UnitPromotions ADD AuraRangeChange integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AuraEffectChange integer DEFAULT 0;

-- Combat strength modifier when unit is on a border tile
ALTER TABLE UnitPromotions ADD BorderMod integer DEFAULT 0;

-- Combat strength modifier per active marriage with city state (not at war), capped at MarriageModCap
ALTER TABLE UnitPromotions ADD MarriageMod integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD MarriageModCap integer DEFAULT 0;

-- Combat strength modifier per level above 1
ALTER TABLE UnitPromotions ADD CombatModPerLevel integer DEFAULT 0;

-- Modifier for damage taken from all sources
ALTER TABLE UnitPromotions ADD DamageTakenMod integer DEFAULT 0;

-- Influence towards the nearest city state per XP gained from combat
ALTER TABLE UnitPromotions ADD InfluenceFromCombatXpTimes100 integer DEFAULT 0;

-- Unit doesn't lose movement when attacking, including from any terrain cost, river crossing, or ZoC
ALTER TABLE UnitPromotions ADD FreeAttackMoves boolean DEFAULT 0;

-- Used with CapitalDefenseModifier and CapitalDefenseFalloff
-- Combat strength modifier near player's capital, starting at CapitalDefenseModifier on the capital tile and decreasing by CapitalDefenseFalloff per tile away from the capital
-- Modifier is capped by CapitalDefenseLimit
ALTER TABLE UnitPromotions ADD CapitalDefenseLimit integer DEFAULT 0;

-- Combat strength modifier against units from unhappy empire
ALTER TABLE UnitPromotions ADD VsUnhappyMod integer DEFAULT 0;

-- Unit pillages the target tile's fortifications when attacking and defeating a unit
-- On pillage effects (e.g. heal, gold steal, instant yields) are not triggered by this
ALTER TABLE UnitPromotions ADD PillageFortificationsOnKill boolean DEFAULT 0;

-- Combat strength modifier when attacking per attack done by units with this promotion on the same turn, capped at AttackModPerSamePromotionAttackCap
ALTER TABLE UnitPromotions ADD AttackModPerSamePromotionAttack integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD AttackModPerSamePromotionAttackCap integer DEFAULT 0;

-- Combat strength modifier per city state ally, capped at BALANCE_MAX_CS_ALLY_STRENGTH allies
ALTER TABLE UnitPromotions ADD CombatModPerCSAlliance integer DEFAULT 0;

-- HP to all adjacent owned units when pillaging improvements that do not block healing
ALTER TABLE UnitPromotions ADD AoEHealOnPillage integer DEFAULT 0;

-- Extra XP when defeating a unit
ALTER TABLE UnitPromotions ADD ExtraXPOnKill integer DEFAULT 0;

-- XP when pillaging an improvement
ALTER TABLE UnitPromotions ADD XPFromPillaging integer DEFAULT 0;

-- Unit cannot have its HP increased by any means
ALTER TABLE UnitPromotions ADD CannotHeal boolean DEFAULT 0;

-- % Multiplier on fortify combat strength bonus (FORTIFY_MODIFIER_PER_TURN)
ALTER TABLE UnitPromotions ADD FortifyEffectiveness integer DEFAULT 100;

-- Extra damage on all units on target tile when unit attacks without having moved this turn
-- This damage is not affected by any damage modifying effects on the target(s), and sets off all on-kill triggers
ALTER TABLE UnitPromotions ADD TileDamageIfNotMoved integer DEFAULT 0;

-- When unit is expended, the current yields from the tile (after factoring in the improvement that might be built by the unit) is added to the player's capital
ALTER TABLE UnitPromotions ADD CopyYieldsFromExpendTile boolean DEFAULT 0;

-- When unit is expended, all owned units on the same tile gain XP
ALTER TABLE UnitPromotions ADD TileXPOnExpend integer DEFAULT 0;

-- This promotion is only effective if the unit starts its turn affected by leadership aura (great general or great admiral depending on domain)
ALTER TABLE UnitPromotions ADD RequiresLeadership boolean DEFAULT 0;

-- This promotion is only effective if the unit starts its turn at or above this percentage of health
ALTER TABLE UnitPromotions ADD MinEffectiveHealth integer DEFAULT 0;

-- PROMOTIONS_CROSS_ICE
ALTER TABLE UnitPromotions ADD CanCrossIce boolean DEFAULT 0;

-- PROMOTIONS_CROSS_MOUNTAINS
ALTER TABLE UnitPromotions ADD CanCrossMountains boolean DEFAULT 0;

-- PROMOTIONS_CROSS_OCEANS
ALTER TABLE UnitPromotions ADD CanCrossOceans boolean DEFAULT 0;

-- PROMOTIONS_DEEP_WATER_EMBARKATION
ALTER TABLE UnitPromotions ADD EmbarkedDeepWater boolean DEFAULT 0;

-- PROMOTIONS_IMPROVEMENT_BONUS
ALTER TABLE UnitPromotions ADD NearbyImprovementCombatBonus integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD NearbyImprovementBonusRange integer DEFAULT 0;
ALTER TABLE UnitPromotions ADD CombatBonusImprovement text REFERENCES Improvements (Type);

-- Add columns to UnitPromotions to handle grouping of ranked promotions
ALTER TABLE UnitPromotions ADD RankList text;
ALTER TABLE UnitPromotions ADD RankNumber integer DEFAULT 0;
-- to hold visibility preference for each promotion
ALTER TABLE UnitPromotions ADD FlagPromoOrder integer DEFAULT 0;

ALTER TABLE UnitPromotions ADD SimpleHelpText boolean DEFAULT 0;
ALTER TABLE UnitPromotions ADD ShowInUnitPanel boolean DEFAULT 1;
ALTER TABLE UnitPromotions ADD IsVisibleAboveFlag boolean DEFAULT 1;

-- Attack and defense modifiers against units of this domain
ALTER TABLE UnitPromotions_Domains ADD Attack integer DEFAULT 0;
ALTER TABLE UnitPromotions_Domains ADD Defense integer DEFAULT 0;

-- Heal rate is doubled for units with this promotion on tiles of the specified terrain/feature
ALTER TABLE UnitPromotions_Terrains ADD DoubleHeal boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features ADD DoubleHeal boolean DEFAULT 0;

-- Movement cost is doubled for units with this promotion when moving onto tiles of the specified terrain/feature
ALTER TABLE UnitPromotions_Terrains ADD HalfMove boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features ADD HalfMove boolean DEFAULT 0;

-- Movement cost is increased by 1 for units with this promotion when moving onto tiles of the specified terrain/feature
ALTER TABLE UnitPromotions_Terrains ADD ExtraMove boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features ADD ExtraMove boolean DEFAULT 0;

-- Note for below abilities: if a feature is present on a tile, the feature cost overrides the tile's base terrain cost, EXCEPT for Hills and Mountains, which add +1 movement cost

-- Units with this promotion ignore terrain cost when moving onto tiles containing the specified terrain/feature
-- Example: if set to true (1) for Forest, then when entering a Forest/Hill tile, both the Forest and the Hill cost are ignored
-- Whether the cost of rivers is ignored when doing this is determined by the IGNORE_SPECIFIC_TERRAIN_COSTS_INCLUDES_RIVERS define
ALTER TABLE UnitPromotions_Terrains ADD IgnoreTerrainCostIn boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features ADD IgnoreTerrainCostIn boolean DEFAULT 0;

-- Units with this promotion ignore the terrain cost ADDED BY tiles of the specified terrain/feature
-- Example: if set to true (1) for Forest, then when entering a Forest/Hill tile, ONLY the Forest cost is ignored, the Hill cost is still added
-- Likewise, the cost of crossing a river isn't ignored by this
ALTER TABLE UnitPromotions_Terrains ADD IgnoreTerrainCostFrom boolean DEFAULT 0;
ALTER TABLE UnitPromotions_Features ADD IgnoreTerrainCostFrom boolean DEFAULT 0;