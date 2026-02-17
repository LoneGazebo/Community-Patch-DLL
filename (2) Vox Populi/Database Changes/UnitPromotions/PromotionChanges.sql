-- Unfortunately, the <Replace> command isn't smart enough to only replace specified values instead of whole rows,
-- so we need to redefine all existing and new promotions here.

-- If a promotion should grant blanket immunity to all Plagues added by VP, insert it into this helper table
-- If you also want compatibility with Plagues added by modmods, modify the associated trigger in (2) Vox Populi\Database Changes\Triggers.sql
CREATE TEMP TABLE PlagueImmunePromotions (
	PromotionType TEXT,
	DomainType TEXT,
	IncludeSpecial BOOLEAN DEFAULT 0 -- promotions not applied by UnitPromotions_Plagues, e.g., Prisoners of War; if set to 1, will be included regardless of domain
);

----------------------------------------------------------------------------------------------------------------------------
-- Land Melee promotion tree (mix of Melee/Gun/Mounted/Armor) drawn using ASCIIFlow
--
--                              ┌──► Medic I ─────► Medic II
--                              │
--                              ├───► Charge I ────► Charge II
--                              │
-- Shock I ───┬───► Shock II ─┬─┴───► Shock III ────┬──► Overrun
--            │               │                     │    March
--          ┌─┼───────────────┴──► Sentry           └─────────────────────────┐
--          │ │                    Cover I ─────► Cover II                    │
--          │ │                                                               ├───► Ambush I ───► Ambush II
--          │ ├─┬──► Air Defense I ───► Air Defense II ───► Air Defense III   │
--          │ │ │                                                             ├───► Mobility
--          │ │ │                  Amphibious                                 │
--          │ └─┼─────────────┬──► Woodsman         ┌─────────────────────────┘
--          │   │             │                     │    Blitz
-- Drill I ─┴───┴─► Drill II ─┴─┬───► Drill III ────┴──► Stalwart
--                              │
--                              ├───► Forlorn Hope
--                              │
--                              └───► Formation I ───► Formation II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET CombatPercent = 10, FlankAttackModifier = 5 WHERE RankList = 'SHOCK';

UPDATE UnitPromotions SET CombatPercent = 10, DamageReductionCityAssault = 15 WHERE RankList = 'DRILL';

UPDATE UnitPromotions SET VisibilityChange = 1 WHERE RankList = 'SENTRY';

UPDATE UnitPromotions SET RangedDefenseMod = 25 WHERE RankList = 'COVER';

UPDATE UnitPromotions SET Amphib = 1, River = 1 WHERE Type = 'PROMOTION_AMPHIBIOUS';
INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_AMPHIBIOUS', 'FEATURE_MARSH', 1);

-- Combat plot for melee, this unit's plot for ranged
UPDATE UnitPromotions SET RoughFromMod = 10 WHERE Type = 'PROMOTION_WOODSMAN';
DELETE FROM UnitPromotions_Features WHERE PromotionType = 'PROMOTION_WOODSMAN';
INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_WOODSMAN', 'FEATURE_FOREST', 1),
	('PROMOTION_WOODSMAN', 'FEATURE_JUNGLE', 1);

UPDATE UnitPromotions SET SameTileHealChange = 5, AdjacentTileHealChange = 5 WHERE RankList = 'MEDIC';
UPDATE UnitPromotions SET NeutralHealChange = 5, EnemyHealChange = 5 WHERE Type = 'PROMOTION_MEDIC_II';

UPDATE UnitPromotions SET AttackWoundedMod = 20, OpenAttack = 10 WHERE RankList = 'CHARGE';

-- Forlorn Hope
UPDATE UnitPromotions SET CityAttack = 100, CombatPercent = -10 WHERE Type = 'PROMOTION_SIEGE';

UPDATE UnitPromotions SET OpenDefense = 15 WHERE RankList = 'FORMATION';

UPDATE UnitPromotions SET FlankAttackModifier = 25, AOEDamageOnKill = 10 WHERE Type = 'PROMOTION_OVERRUN';

UPDATE UnitPromotions SET AlwaysHeal = 1 WHERE RankList = 'MARCH';

UPDATE UnitPromotions SET ExtraAttacks = 1, CanMoveAfterAttacking = 1 WHERE Type = 'PROMOTION_BLITZ';

UPDATE UnitPromotions SET DefenseMod = 35 WHERE Type = 'PROMOTION_STALWART';

UPDATE UnitPromotions SET RoughAttack = 15 WHERE RankList = 'AMBUSH';

UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_MOBILITY';

UPDATE UnitPromotions SET RangedDefenseMod = 10 WHERE RankList = 'AIR_DEFENSE';
UPDATE UnitPromotions SET LandAirDefenseBonus = 15 WHERE Type = 'PROMOTION_AIR_DEFENSE_1';
UPDATE UnitPromotions SET LandAirDefenseBonus = 20 WHERE Type = 'PROMOTION_AIR_DEFENSE_2';
UPDATE UnitPromotions SET LandAirDefenseBonus = 25 WHERE Type = 'PROMOTION_AIR_DEFENSE_3';

----------------------------------------------------------------------------------------------------------------------------
-- Land Ranged promotion tree (mix of Archer/Skirmisher) drawn using ASCIIFlow
--
--                                                               March
--             ┌────► Medic I ────► Medic II                     Parthian Tactics
--             │                                                 Range
-- Barrage I ──┴─┬──► Barrage II ─┬───┬──► Barrage III ────────► Firing Doctrine
--               │                │   │
--               │                │   └─────────────────────────────────────────┐
--               │                │                                             │
--             ┌─┼────────────────┴─► Cover I ─────► Cover II                   │
--             │ │                                                              ├───► Envelopment
--             │ ├──► Air Defense I ──► Air Defense II ──► Air Defense III      │
--             │ │                                                              │
--             │ │                   ┌──────────────────────────────────────────┘
--             │ │                   │
-- Accuracy I ─┴─┴──► Accuracy II ───┴───► Accuracy III ───────► Infiltrators
--                                                               Logistics
--                                                               Coup de Grace
--                                                               Sentry
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET RangedAttackModifier = 10, AttackAbove50HealthMod = 10 WHERE RankList = 'BARRAGE';

UPDATE UnitPromotions SET RangedAttackModifier = 10, AttackBelowEqual50HealthMod = 10 WHERE RankList = 'ACCURACY';

UPDATE UnitPromotions SET AttackAbove50HealthMod = 25 WHERE Type = 'PROMOTION_FIRING_DOCTRINE';

UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 15, AttackWoundedMod = 15, NoAdjacentUnitMod = 10 WHERE Type = 'PROMOTION_INFILTRATORS';

UPDATE UnitPromotions SET ExtraAttacks = 1, RangedAttackModifier = -30 WHERE Type = 'PROMOTION_LOGISTICS';

-- Archer only
UPDATE UnitPromotions SET RangeChange = 1, RangedAttackModifier = -20, MinimumRangeRequired = 2 WHERE Type = 'PROMOTION_RANGE';

-- Skirmisher only
UPDATE UnitPromotions
SET MountedOnly = 1
WHERE Type IN (
	'PROMOTION_PARTHIAN_TACTICS',
	'PROMOTION_COUP_DE_GRACE',
	'PROMOTION_SKIRMISHER_MARCH',
	'PROMOTION_SKIRMISHER_SENTRY',
	'PROMOTION_ENVELOPMENT'
);

UPDATE UnitPromotions SET MovesChange = 1, IgnoreZOC = 1 WHERE Type = 'PROMOTION_PARTHIAN_TACTICS';

UPDATE UnitPromotions SET AttackBelowEqual50HealthMod = 30 WHERE Type = 'PROMOTION_COUP_DE_GRACE';

UPDATE UnitPromotions SET RangedFlankAttack = 1, FlankAttackModifier = 10 WHERE Type = 'PROMOTION_ENVELOPMENT';

----------------------------------------------------------------------------------------------------------------------------
-- Extra promotion lines for Helicopter Gunship
--
-- Tank Hunter I ────► Tank Hunter II
--
-- Mobility I ───────► Mobility II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions_UnitCombatMods
SET Modifier = 50
WHERE PromotionType IN (SELECT Type FROM UnitPromotions WHERE RankList = 'TANK_HUNTER');

UPDATE UnitPromotions SET MovesChange = 1 WHERE RankList = 'HELI_MOBILITY';

----------------------------------------------------------------------------------------------------------------------------
-- Siege promotion tree drawn using ASCIIFlow
--
--                                       ┌──────► Volley
--                                       │                        Infiltrators
-- Siege I ─────────► Siege II ──────────┴──────► Siege III ────► Range
--
-- Cover I ─────────► Cover II
--                                                                Firing Doctrine
-- Field I ────┬────► Field II ──────────┬──────► Field III ────► Logistics
--             │                         │
--             └────► Coastal Artillery  └──────► Splash I ─────► Splash II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET RangedAttackModifier = 10, CityAttack = 15 WHERE RankList = 'SIEGE';

UPDATE UnitPromotions SET RangedAttackModifier = 10, OpenRangedAttackMod = 10 WHERE RankList = 'FIELD';

UPDATE UnitPromotions SET CityAttack = 50, AttackFortifiedMod = 50 WHERE Type = 'PROMOTION_VOLLEY';

UPDATE UnitPromotions SET SplashDamage = 5 WHERE RankList = 'SPLASH';

INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_COASTAL_ARTILLERY', 'UNITCOMBAT_NAVALMELEE', 50),
	('PROMOTION_COASTAL_ARTILLERY', 'UNITCOMBAT_NAVALRANGED', 50),
	('PROMOTION_COASTAL_ARTILLERY', 'UNITCOMBAT_CARRIER', 50);

----------------------------------------------------------------------------------------------------------------------------
-- Recon promotion tree drawn using ASCIIFlow
--
-- Survivalism I ────► Survivalism II ──┬───► Survivalism III ───────────────────────────────┐
--                                      │                                                    │
--                                      ├───► Medic I ──────► Medic II                       ├───► Frogman
--                                      │                                                    │
--                                      ├───► Scouting I ───► Scouting II                    ├───► Screening
--                                      │                                                    │
-- Trailblazer I ────► Trailblazer II ──┴───► Trailblazer III ───────────────────────────────┘
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions
SET
	NeutralHealChange = 5,
	EnemyHealChange = 5,
	DefenseMod = 25
WHERE Type IN ('PROMOTION_SURVIVALISM_1', 'PROMOTION_SURVIVALISM_2');

UPDATE UnitPromotions SET AlwaysHeal = 1, FreePillageMoves = 1 WHERE Type = 'PROMOTION_SURVIVALISM_3';

UPDATE UnitPromotions SET VisibilityChange = 1, EmbarkExtraVisibility = 1 WHERE Type = 'PROMOTION_TRAILBLAZER_1';
UPDATE UnitPromotions SET MovesChange = 1, ExtraNavalMovement = 1, River = 1 WHERE Type = 'PROMOTION_TRAILBLAZER_2';
UPDATE UnitPromotions
SET
	OutsideFriendlyLandsModifier = 20,
	AllowsEmbarkation = 1,
	CanCrossMountains = 1,
	EnemyRoute = 1,
	IgnoreZOC = 1
WHERE Type = 'PROMOTION_TRAILBLAZER_3';

INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_TRAILBLAZER_1', 'FEATURE_JUNGLE', 1),
	('PROMOTION_TRAILBLAZER_1', 'FEATURE_FOREST', 1),
	('PROMOTION_TRAILBLAZER_2', 'FEATURE_MARSH', 1);

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_TRAILBLAZER_1', 'TERRAIN_HILL', 1),
	('PROMOTION_TRAILBLAZER_2', 'TERRAIN_DESERT', 1),
	('PROMOTION_TRAILBLAZER_2', 'TERRAIN_SNOW', 1);

UPDATE UnitPromotions SET VisibilityChange = 1, EmbarkExtraVisibility = 1 WHERE Type = 'PROMOTION_SCOUTING_1';
UPDATE UnitPromotions SET MovesChange = 1, ExtraNavalMovement = 1 WHERE Type = 'PROMOTION_SCOUTING_2';

UPDATE UnitPromotions SET EmbarkFlatCost = 1, DisembarkFlatCost = 1, Amphib = 1, EmbarkDefenseModifier = 50 WHERE Type = 'PROMOTION_FROGMAN';

UPDATE UnitPromotions SET ExtraFlankPower = 1, FlankAttackModifier = 10 WHERE Type = 'PROMOTION_SCREENING';

----------------------------------------------------------------------------------------------------------------------------
-- Naval Melee promotion tree drawn using ASCIIFlow
--
--                    ┌───► Cover I ────► Cover II    Breacher
--                    │                               Naval Siege
--                    │                         ┌───► Commerce Raider           Vanguard
--                    │                         │                               Dauntless
-- Hull I ────────────┴─┬─► Hull II ────────────┴───► Hull III ───────────────► Blockade
--                      │
--                      ├─► Air Defense I ────► Air Defense II ────► Air Defense III
--                      │
--                      ├─► Can See Submarines
--                      │
-- Boarding Party I ──┬─┴─► Boarding Party II ──┬───► Boarding Party III ─────► Pincer
--                    │                         │                               Supply
--                    │                         └───► Medic I ────► Medic II    Blitz
--                    │                               Piracy
--                    │                               Encirclement
--                    │                               Minelayer
--                    │
--                    └───► Navigator I ─────► Navigator II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET CombatPercent = 10 WHERE RankList = 'HULL';
UPDATE UnitPromotions SET MaxHitPointsChange = 10 WHERE Type = 'PROMOTION_COASTAL_RAIDER_1';
UPDATE UnitPromotions SET MaxHitPointsChange = 15 WHERE Type IN ('PROMOTION_COASTAL_RAIDER_2', 'PROMOTION_COASTAL_RAIDER_3');

UPDATE UnitPromotions SET CombatPercent = 15 WHERE RankList = 'BOARDING_PARTY';

INSERT INTO UnitPromotions_Plagues
	(PromotionType, PlaguePromotionType, DomainType, ApplyOnAttack, ApplyOnDefense)
VALUES
	('PROMOTION_BOARDING_PARTY_1', 'PROMOTION_BOARDED_1', 'DOMAIN_SEA', 1, 1),
	('PROMOTION_BOARDING_PARTY_3', 'PROMOTION_BOARDED_2', 'DOMAIN_SEA', 1, 1);

INSERT INTO UnitPromotions_BlockedPromotions
	(PromotionType, BlockedPromotionType)
VALUES
	('PROMOTION_BOARDING_PARTY_2', 'PROMOTION_BOARDED_1'),
	('PROMOTION_BOARDING_PARTY_2', 'PROMOTION_BOARDED_2');

UPDATE UnitPromotions SET SeeInvisible = 'INVISIBLE_SUBMARINE' WHERE Type = 'PROMOTION_SEE_INVISIBLE_SUBMARINE';

UPDATE UnitPromotions SET MovesChange = 1, VisibilityChange = 1 WHERE RankList = 'NAVIGATOR';

UPDATE UnitPromotions SET AOEDamageOnKill = 15 WHERE Type = 'PROMOTION_BREACHER';
INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Defense)
SELECT
	'PROMOTION_BREACHER', Type, 10
FROM UnitCombatInfos
WHERE IsNaval = 1;

UPDATE UnitPromotions SET CityAttack = 75, HealOutsideFriendly = 1 WHERE Type = 'PROMOTION_NAVAL_SIEGE';

UPDATE UnitPromotions SET FreePillageMoves = 1, HealOnPillage = 1 WHERE Type = 'PROMOTION_COMMERCE_RAIDER';

UPDATE UnitPromotions SET CityAttackPlunderModifier = 100 WHERE Type = 'PROMOTION_PIRACY';
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_PIRACY', 'YIELD_GOLD', 100);

UPDATE UnitPromotions SET HPHealedIfDestroyEnemy = 10 WHERE Type = 'PROMOTION_ENCIRCLEMENT';
INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Attack)
SELECT
	'PROMOTION_ENCIRCLEMENT', Type, 10
FROM UnitCombatInfos
WHERE IsNaval = 1;

UPDATE UnitPromotions
SET
	AdjacentEnemySapMovement = (SELECT Value FROM Defines WHERE Name = 'MOVE_DENOMINATOR') * 2,
	RangedDefenseMod = -10
WHERE Type = 'PROMOTION_MINELAYER';

UPDATE UnitPromotions SET CityAttack = 125, DamageReductionCityAssault = 50 WHERE Type = 'PROMOTION_VANGUARD';

UPDATE UnitPromotions SET ChangeDamageValue = -5, FriendlyHealChange = 10, NeutralHealChange = 10, EnemyHealChange = 10 WHERE Type = 'PROMOTION_DAUNTLESS';

UPDATE UnitPromotions SET RangedDefenseMod = 25, ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_BLOCKADE';

UPDATE UnitPromotions SET FlankAttackModifier = 10, IgnoreZOC = 1 WHERE Type = 'PROMOTION_PINCER';

UPDATE UnitPromotions SET HealOutsideFriendly = 1, FriendlyHealChange = 5, NeutralHealChange = 5, EnemyHealChange = 5 WHERE Type = 'PROMOTION_SUPPLY';

----------------------------------------------------------------------------------------------------------------------------
-- Naval Ranged promotion tree drawn using ASCIIFlow
--                                                                Splash I
-- Targeting I ──┬───► Targeting II ───┬───► Targeting III ───┬─► Indomitable ─────────────────────────┐
--               │                     │                      │                                        │
--               │                     │                      └───────────────────────────┐            │
--               │                     │                                                  │            │
--               │                     └────────────────────────────────────┐             │            │
--               │                                                          │             │            │
--               ├─┬─► Air Defense I ──► Air Defense II ──► Air Defense III │             │            │
--               │ │                                                        ├──► Mobility ├───► Supply ├──► Logistics
--               └─┼───────────────────┬─► Sentry                           │             │            │
--                 │                   │                                    │             │            │
--                 │                   │ ┌──────────────────────────────────┘             │            │
--                 │                   │ │                                                │            │
--                 │                   │ │                    ┌───────────────────────────┘            │
--                 │                   │ │                    │                                        │
-- Bombardment I ──┴─► Bombardment II ─┴─┴─► Bombardment III ─┴─► Broadside ───────────────────────────┘
--                                                                Shrapnel Rounds
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions
SET AttackBelowEqual50HealthMod = 10
WHERE RankList = 'TARGETING';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
SELECT
	Type, 'DOMAIN_LAND', 10
FROM UnitPromotions
WHERE RankList = 'TARGETING';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
SELECT
	Type, 'DOMAIN_SEA', 10
FROM UnitPromotions
WHERE RankList = 'TARGETING';

UPDATE UnitPromotions
SET
	RangedAttackModifier = 10,
	CityAttack = 30
WHERE RankList = 'BOMBARDMENT';

UPDATE UnitPromotions SET DefenseMod = 25 WHERE Type = 'PROMOTION_INDOMITABLE';
INSERT INTO UnitPromotions_BlockedPromotions
	(PromotionType, BlockedPromotionType)
VALUES
	('PROMOTION_INDOMITABLE', 'PROMOTION_BOARDED_1'),
	('PROMOTION_INDOMITABLE', 'PROMOTION_BOARDED_2');

UPDATE UnitPromotions SET CityAttack = 40 WHERE Type = 'PROMOTION_BROADSIDE';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_SHRAPNEL_ROUNDS', 'DOMAIN_LAND', 50);

----------------------------------------------------------------------------------------------------------------------------
-- Submarine promotion tree drawn using ASCIIFlow
--
--                              ┌───► Commerce Raider
--                              │
--                              ├───► Hunter Killer
--                              │
--                              ├───► Wolfpack      ┌───► Indomitable
--                              │                   │
-- Torpedo I ──┬──► Torpedo II ─┴───► Torpedo III ──┼───► Predator
--             │                                    │
--             ├──► Supply                          ├───► Infiltrators
--             │                                    │
--             └──► Navigator I ───► Navigator II   └───► Periscope Depth
----------------------------------------------------------------------------------------------------------------------------
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
SELECT
	Type, 'DOMAIN_LAND', 30
FROM UnitPromotions
WHERE RankList = 'TORPEDO';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
SELECT
	Type, 'DOMAIN_SEA', 30
FROM UnitPromotions
WHERE RankList = 'TORPEDO';

UPDATE UnitPromotions SET SeeInvisible = 'INVISIBLE_SUBMARINE' WHERE Type = 'PROMOTION_HUNTER_KILLER';
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_HUNTER_KILLER', 'UNITCOMBAT_SUBMARINE', 50);

INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Attack)
VALUES
	('PROMOTION_VP_WOLFPACK', 'UNITCOMBAT_SUBMARINE', 30);

UPDATE UnitPromotions SET HPHealedIfDestroyEnemy = 15, IgnoreZOC = 1 WHERE Type = 'PROMOTION_PREDATOR';

UPDATE UnitPromotions SET RangedDefenseMod = 40, ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_PERISCOPE_DEPTH';

----------------------------------------------------------------------------------------------------------------------------
-- Carrier promotion tree drawn using ASCIIFlow
--
-- Flight Deck I ──┬──► Flight Deck II ──┬─────► Flight Deck III ────┐
--                 │                     │                           │
--                 │                     └───────────┐               │
--                 │                                 │               │
--                 └──────────────────────┬─► Sentry ├──► Mobility   ├──► Supply
--                                        │          │               │
--                                        │ ┌────────┘               │
--                                        │ │                        │
-- Armor Plating I ───► Armor Plating II ─┴─┴──► Armor Plating III ──┘
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET CargoChange = 1 WHERE RankList = 'FLIGHT_DECK';

UPDATE UnitPromotions SET DefenseMod = 25 WHERE RankList = 'ARMOR_PLATING';

-- Extra Interceptor line for units with interception range
UPDATE UnitPromotions SET InterceptChanceChange = 15 WHERE RankList = 'INTERCEPTOR';

----------------------------------------------------------------------------------------------------------------------------
-- Fighter promotion tree drawn using ASCIIFlow
--                                                                Aerial Torpedo
-- Ace Pilot I ──────► Ace Pilot II ──────► Ace Pilot III ────┬─► Ace Pilot IV
--                                                            │
--                                                            ├─► Range (Air)
--                                                            │   Air Repair
--                                                            │
-- Dogfighting I ─┬──► Dogfighting II ────► Dogfighting III ──┴─► Bomb Rack
--                │
--                └──► Eagle Hunter
--
-- Air Supremacy I ──► Air Supremacy II
----------------------------------------------------------------------------------------------------------------------------

-- Ace Pilot
UPDATE UnitPromotions SET InterceptChanceChange = 33 WHERE Type = 'PROMOTION_INTERCEPTION_1';
UPDATE UnitPromotions SET InterceptChanceChange = 33, InterceptionCombatModifier = 33 WHERE Type = 'PROMOTION_INTERCEPTION_2';
UPDATE UnitPromotions SET InterceptChanceChange = 34, InterceptionCombatModifier = 33, RangeChange = 1 WHERE Type = 'PROMOTION_INTERCEPTION_3';
UPDATE UnitPromotions SET NumInterceptionChange = 1, InterceptionCombatModifier = 34, RangeChange = 1 WHERE Type = 'PROMOTION_SORTIE';

UPDATE UnitPromotions SET AirSweepCombatModifier = 33, GetGroundAttackDamage = 5 WHERE Type = 'PROMOTION_DOGFIGHTING_1';
UPDATE UnitPromotions SET AirSweepCombatModifier = 33, GetGroundAttackDamage = 10 WHERE Type = 'PROMOTION_DOGFIGHTING_2';
UPDATE UnitPromotions SET AirSweepCombatModifier = 34, GetGroundAttackDamage = 15 WHERE Type = 'PROMOTION_DOGFIGHTING_3';

DELETE FROM UnitPromotions_UnitCombatMods WHERE PromotionType IN (SELECT Type FROM UnitPromotions WHERE RankList = 'AIR_SUPREMACY');
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Modifier)
SELECT
	Type, 'DOMAIN_AIR', 100
FROM UnitPromotions
WHERE RankList = 'AIR_SUPREMACY';

-- Eagle Hunter
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_ANTI_FIGHTER', 'UNITCOMBAT_FIGHTER', 33);

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_AERIAL_TORPEDO', 'DOMAIN_SEA', 33);

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_BOMB_RACK', 'DOMAIN_LAND', 33);

UPDATE UnitPromotions SET RangeChange = 2 WHERE Type = 'PROMOTION_AIR_RANGE';

----------------------------------------------------------------------------------------------------------------------------
-- Bomber promotion tree drawn using ASCIIFlow
--
--                                            Evasion
--                                      ┌───► Air Ambush I ───► Air Ambush II
--                                      │
-- Siege I (Air) ─────► Siege II (Air) ─┴───► Siege III (Air) ────┬──► Range (Air)
--                                                                │
--                                                                ├──► Air Logistics
-- Air Targeting I ───► Air Targeting II ─┬─► Air Targeting III ──┘
--                                        │
--                                        └─► Air Penetration I ────► Air Penetration II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET CityAttack = 50 WHERE RankList = 'AIR_SIEGE';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_AIR_TARGETING_1', 'DOMAIN_LAND', 15),
	('PROMOTION_AIR_TARGETING_1', 'DOMAIN_SEA', 15),
	('PROMOTION_AIR_TARGETING_2', 'DOMAIN_LAND', 15),
	('PROMOTION_AIR_TARGETING_2', 'DOMAIN_SEA', 15),
	('PROMOTION_AIR_TARGETING_3', 'DOMAIN_LAND', 25),
	('PROMOTION_AIR_TARGETING_3', 'DOMAIN_SEA', 25);

UPDATE UnitPromotions SET InterceptionDefenseDamageModifier = -50 WHERE Type = 'PROMOTION_EVASION';

UPDATE UnitPromotions_UnitCombatMods
SET Modifier = 50
WHERE PromotionType IN (
	SELECT Type FROM UnitPromotions WHERE RankList = 'AIR_AMBUSH'
) AND UnitCombatType = 'UNITCOMBAT_ARMOR';

UPDATE UnitPromotions SET EvasionChange = 33 WHERE RankList = 'AIR_PENETRATION';

UPDATE UnitPromotions SET ExtraAttacks = 1 WHERE Type = 'PROMOTION_AIR_LOGISTICS';

----------------------------------------------------------------------------------------------------------------------------
-- Extra promotion tree for units trained in Ikanda, drawn using ASCIIFlow
--
--                                                               Mobility
--                                                               Blitz
--                                                               March
--                                      ┌──► Woodsman            Forlorn Hope
--                                      │                        Stalwart
-- Buffalo Chest ─────► Buffalo Horns ──┴──► Buffalo Loins ─┬──► Overrun
--                                                          │
--                                                          └──► Ambush I ────► Ambush II
----------------------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET CombatPercent = 10, MaxHitPointsChange = 25 WHERE Type = 'PROMOTION_BUFFALO_CHEST';
UPDATE UnitPromotions SET CombatPercent = 10, FlankAttackModifier = 25 WHERE Type = 'PROMOTION_BUFFALO_HORNS';
UPDATE UnitPromotions SET CombatPercent = 10, MovesChange = 1 WHERE Type = 'PROMOTION_BUFFALO_LOINS';

--------------------------------------------
-- Trait free promotions
--------------------------------------------
UPDATE UnitPromotions SET FreePillageMoves = 1, AOEDamageOnPillage = 10, PartialHealOnPillage = 10 WHERE Type = 'PROMOTION_VIKING';
UPDATE UnitPromotions SET FreePillageMoves = 1, AOEDamageOnPillage = 15, HealOnPillage = 1 WHERE Type = 'PROMOTION_LONGBOAT';
INSERT INTO UnitPromotions_YieldFromPillage
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_VIKING', 'YIELD_GOLD', 30),
	('PROMOTION_LONGBOAT', 'YIELD_GOLD', 60);

UPDATE UnitPromotions
SET
	CapitalDefenseModifier = 0,
	CapitalDefenseFalloff = 1,
	CapitalDefenseLimit = 30,
	MovesChange = 1
WHERE Type = 'PROMOTION_SUN_NEVER_SETS';

UPDATE UnitPromotions SET AllowsEmbarkation = 1, EmbarkedAllWater = 1, EmbarkExtraVisibility = 2 WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION';

UPDATE UnitPromotions SET CaptureDefeatedEnemy = 1, CapturedUnitsConscripted = 1 WHERE Type = 'PROMOTION_COERCION';

UPDATE UnitPromotions SET RiverDoubleMove = 1 WHERE Type = 'PROMOTION_WAR_CANOES';

UPDATE UnitPromotions SET AttackMod = 20 WHERE Type = 'PROMOTION_ATTACK_BONUS_SWEDEN';

--------------------------------------------
-- Policy free promotions
--------------------------------------------
UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_FASTER_WORKER';

UPDATE UnitPromotions SET HPHealedIfDestroyEnemy = 15 WHERE Type = 'PROMOTION_ENDURANCE';

UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_CONSCRIPTION';

UPDATE UnitPromotions SET VisibilityChange = 1 WHERE Type = 'PROMOTION_IMPERIALISM';
UPDATE UnitPromotions SET MovesChange = 1 WHERE Type IN ('PROMOTION_FASTER_GENERAL', 'PROMOTION_NAVAL_TRADITION');

UPDATE UnitPromotions SET AuraRangeChange = 1, AuraEffectChange = 10 WHERE Type = 'PROMOTION_REGIMENTAL_TRADITIONS';

UPDATE UnitPromotions SET StrongerDamaged = 1 WHERE Type = 'PROMOTION_BANZAI';

UPDATE UnitPromotions SET NearbyCityCombatMod = 20, NearbyRange = 1 WHERE Type = 'PROMOTION_NATIONALISM';

UPDATE UnitPromotions SET MovesChange = 3 WHERE Type = 'PROMOTION_FAST_GENERAL';
UPDATE UnitPromotions SET AttackMod = 15, IgnoreZOC = 1 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_GUNPOWDER';
UPDATE UnitPromotions SET AttackMod = 15, MovesChange = 1 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_ARMORED';

UPDATE UnitPromotions SET FriendlyHealChange = 5, CityAttack = 25 WHERE Type = 'PROMOTION_MARE_NOSTRUM';

--------------------------------------------
-- Tech free promotions
--------------------------------------------
UPDATE UnitPromotions SET AllowsEmbarkation = 1 WHERE Type = 'PROMOTION_EMBARKATION';

UPDATE UnitPromotions SET CanMoveImpassable = 1 WHERE Type = 'PROMOTION_ENGINEERING_CORP';

UPDATE UnitPromotions SET ExtraFeatureDamage = 1 WHERE Type = 'PROMOTION_FALLOUT_RESISTANCE';

UPDATE UnitPromotions SET IgnoreFeatureDamage = 1 WHERE Type = 'PROMOTION_FALLOUT_IMMUNITY';
INSERT INTO UnitPromotions_BlockedPromotions
	(PromotionType, BlockedPromotionType)
VALUES
	('PROMOTION_FALLOUT_IMMUNITY', 'PROMOTION_FALLOUT_RESISTANCE');

--------------------------------------------
-- Natural Wonder free promotions
--------------------------------------------
UPDATE UnitPromotions SET MovesChange = 2 WHERE Type = 'PROMOTION_SACRED_STEPS';

UPDATE UnitPromotions SET FriendlyHealChange = 5, NeutralHealChange = 5, EnemyHealChange = 5 WHERE Type = 'PROMOTION_EVERLASTING_YOUTH';

--------------------------------------------
-- Building free promotions
--------------------------------------------
UPDATE UnitPromotions SET FriendlyLandsModifier = 15 WHERE Type = 'PROMOTION_HIMEJI_CASTLE';

UPDATE UnitPromotions SET CityAttack = 25 WHERE Type = 'PROMOTION_STATUE_ZEUS';
INSERT INTO UnitPromotions_BlockedPromotions
	(PromotionType, BlockedPromotionType)
VALUES
	('PROMOTION_STATUE_ZEUS', 'PROMOTION_PRISONERS_OF_WAR');

UPDATE UnitPromotions SET AttackMod = 15, ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_JINETE';

UPDATE UnitPromotions SET AttackMod = 10, VisibilityChange = 1, HealOutsideFriendly = 1 WHERE Type = 'PROMOTION_TREASURE_FLEET';

UPDATE UnitPromotions SET CombatPercent = 10, MovesChange = 1 WHERE Type = 'PROMOTION_VENETIAN_CRAFTSMANSHIP';

UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_MORALE';

UPDATE UnitPromotions SET AttackMod = 10 WHERE Type = 'PROMOTION_IKLWA';

UPDATE UnitPromotions SET DiplomaticMissionAccomplishment = 1 WHERE Type = 'PROMOTION_PROXENOS';

-- Eight Virtues of Bushido
UPDATE UnitPromotions SET HasPostCombatPromotions = 1 WHERE Type = 'PROMOTION_BUSHIDO';
INSERT INTO UnitPromotions_PostCombatRandomPromotion
	(PromotionType, NewPromotion)
VALUES
	('PROMOTION_BUSHIDO', 'PROMOTION_RIGHTEOUSNESS'),
	('PROMOTION_BUSHIDO', 'PROMOTION_COURAGE'),
	('PROMOTION_BUSHIDO', 'PROMOTION_BENEVOLENCE'),
	('PROMOTION_BUSHIDO', 'PROMOTION_RESPECT'),
	('PROMOTION_BUSHIDO', 'PROMOTION_SINCERITY'),
	('PROMOTION_BUSHIDO', 'PROMOTION_HONOR'),
	('PROMOTION_BUSHIDO', 'PROMOTION_LOYALTY'),
	('PROMOTION_BUSHIDO', 'PROMOTION_SELF_CONTROL');

UPDATE UnitPromotions SET DiploMissionInfluence = 10 WHERE RankList = 'DIPLO_BOOST';
UPDATE UnitPromotions SET DiploMissionInfluence = 15, RivalTerritory = 1 WHERE Type = 'PROMOTION_NOBILITY';
UPDATE UnitPromotions SET MovesChange = 1, ExtraNavalMovement = 1 WHERE Type IN ('PROMOTION_WIRE_SERVICE', 'PROMOTION_IMPERIAL_SEAL', 'PROMOTION_LITERACY');
UPDATE UnitPromotions SET IgnoreTerrainCost = 1 WHERE Type = 'PROMOTION_LITERACY';
UPDATE UnitPromotions SET RivalTerritory = 1 WHERE Type = 'PROMOTION_DIPLOMATIC_IMMUNITY';

UPDATE UnitPromotions SET MarriageMod = 2, MarriageModCap = 30 WHERE Type = 'PROMOTION_SCHUTZENKONIG';

UPDATE UnitPromotions SET GainsXPFromScouting = 1 WHERE Type = 'PROMOTION_AGE_OF_DISCOVERY';
INSERT INTO UnitPromotions_YieldFromScouting
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_AGE_OF_DISCOVERY', 'YIELD_GOLD', 1);

--------------------------------------------
-- Unit free promotions
--------------------------------------------

UPDATE UnitPromotions SET BarbarianCombatBonus = 25 WHERE Type = 'PROMOTION_BRUTE_FORCE';

UPDATE UnitPromotions SET MaxHitPointsChange = 10, RangedDefenseMod = 10 WHERE Type = 'PROMOTION_FIELD_WORKS_0';
UPDATE UnitPromotions SET MaxHitPointsChange = 15, RangedDefenseMod = 15 WHERE Type = 'PROMOTION_FIELD_WORKS_1';
UPDATE UnitPromotions SET MaxHitPointsChange = 20, RangedDefenseMod = 20 WHERE Type = 'PROMOTION_FIELD_WORKS_2';
UPDATE UnitPromotions SET MaxHitPointsChange = 25, RangedDefenseMod = 25 WHERE Type = 'PROMOTION_FIELD_WORKS_3';

UPDATE UnitPromotions SET OnlyDefensive = 1 WHERE Type = 'PROMOTION_ONLY_DEFENSIVE';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_NAVAL_TARGET_PENALTY', 'DOMAIN_SEA', -20);

UPDATE UnitPromotions SET DefenseMod = 10, NearbyEnemyCombatMod = -15, NearbyEnemyCombatRange = 2 WHERE RankList = 'COVERING_FIRE';
UPDATE UnitPromotions SET AttackFortifiedMod = -50, CityAttack = -50 WHERE Type = 'PROMOTION_COVERING_FIRE_1';
UPDATE UnitPromotions SET AttackFortifiedMod = -25, CityAttack = -25 WHERE Type = 'PROMOTION_COVERING_FIRE_2';
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_COVERING_FIRE_1', 'UNITCOMBAT_ARMOR', -50);

UPDATE UnitPromotions SET NoDefensiveBonus = 1 WHERE Type = 'PROMOTION_NO_DEFENSIVE_BONUSES';

UPDATE UnitPromotions SET CanMoveAfterAttacking = 1 WHERE Type = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING';

UPDATE UnitPromotions SET CityAttack = -33 WHERE Type = 'PROMOTION_CITY_PENALTY';

UPDATE UnitPromotions SET AttackMod = 25, MaxHitPointsChange = 100, RangedSupportFire = 1 WHERE Type = 'PROMOTION_TITANIC';

INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, ExtraMove)
VALUES
	('PROMOTION_BEAM_AXLE', 'FEATURE_FOREST', 1),
	('PROMOTION_BEAM_AXLE', 'FEATURE_JUNGLE', 1),
	('PROMOTION_BEAM_AXLE', 'FEATURE_MARSH', 1);

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, ExtraMove)
VALUES
	('PROMOTION_BEAM_AXLE', 'TERRAIN_HILL', 1);

UPDATE UnitPromotions SET ExtraFlankPower = 1, RangedDefenseMod = 50 WHERE Type = 'PROMOTION_SKIRMISHER_DOCTRINE';

UPDATE UnitPromotions SET CanMoveAllTerrain = 1, CanMoveImpassable = 1, NoAttackInOcean = 1 WHERE Type = 'PROMOTION_HOVERING_UNIT';

UPDATE UnitPromotions SET FlatMovementCost = 1 WHERE Type = 'PROMOTION_FLAT_MOVEMENT_COST';

-- Half speed in enemy territory
UPDATE UnitPromotions SET MustSetUpToRangedAttack = 1 WHERE Type = 'PROMOTION_MUST_SET_UP';

-- Siege Engine
UPDATE UnitPromotions SET CityAttack = 100 WHERE Type = 'PROMOTION_CITY_SIEGE';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_SIEGE_INACCURACY', 'DOMAIN_LAND', -33);

UPDATE UnitPromotions SET RangeAttackIgnoreLOS = 1, RangedAttackModifier = -10 WHERE Type = 'PROMOTION_INDIRECT_FIRE';

UPDATE UnitPromotions SET GainsXPFromScouting = 1 WHERE Type = 'PROMOTION_RECONNAISSANCE';

UPDATE UnitPromotions SET CanCrossOceans = 1 WHERE Type = 'PROMOTION_OCEAN_EXPLORER';

-- Extra Sight While Embarked
UPDATE UnitPromotions SET EmbarkExtraVisibility = 1 WHERE Type = 'PROMOTION_EMBARKED_SIGHT';

-- Embarkation with Defense
UPDATE UnitPromotions SET EmbarkDefenseModifier = 100 WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION';

UPDATE UnitPromotions SET XPFromPillaging = 5 WHERE Type = 'PROMOTION_SCAVENGER';

UPDATE UnitPromotions SET ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_COMMANDO';

UPDATE UnitPromotions SET AttackMod = 25 WHERE Type = 'PROMOTION_ATTACK_BONUS';

UPDATE UnitPromotions SET FreePillageMoves = 1 WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES';

UPDATE UnitPromotions SET DropRange = 9 WHERE Type = 'PROMOTION_PARADROP';
UPDATE UnitPromotions SET DropRange = 40 WHERE Type = 'PROMOTION_EXTENDED_PARADROP';

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, HalfMove)
VALUES
	('PROMOTION_SHALLOW_DRAFT', 'TERRAIN_OCEAN', 1);

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_NAVAL_INACCURACY', 'DOMAIN_LAND', -25);

-- Depth Charges
INSERT INTO UnitPromotions_UnitClasses
	(PromotionType, UnitClassType, Attack, Defense)
VALUES
	('PROMOTION_ANTI_SUBMARINE_I', 'UNITCLASS_SUBMARINE', 33, 25),
	('PROMOTION_ANTI_SUBMARINE_I', 'UNITCLASS_ATTACK_SUBMARINE', 33, 25),
	('PROMOTION_ANTI_SUBMARINE_I', 'UNITCLASS_NUCLEAR_SUBMARINE', 33, 25),
	('PROMOTION_ANTI_SUBMARINE_II', 'UNITCLASS_SUBMARINE', 66, 50),
	('PROMOTION_ANTI_SUBMARINE_II', 'UNITCLASS_ATTACK_SUBMARINE', 66, 50),
	('PROMOTION_ANTI_SUBMARINE_II', 'UNITCLASS_NUCLEAR_SUBMARINE', 66, 50);

UPDATE UnitPromotions SET CargoChange = 1 WHERE Type = 'PROMOTION_CARGO_I';
UPDATE UnitPromotions SET CargoChange = 2 WHERE Type = 'PROMOTION_CARGO_II';
UPDATE UnitPromotions SET CargoChange = 3 WHERE Type = 'PROMOTION_CARGO_III';
UPDATE UnitPromotions SET CargoChange = 4 WHERE Type = 'PROMOTION_CARGO_IV';

UPDATE UnitPromotions SET AttackMod = 0 WHERE Type = 'PROMOTION_SILENT_HUNTER';
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_SILENT_HUNTER', 'DOMAIN_LAND', 50),
	('PROMOTION_SILENT_HUNTER', 'DOMAIN_SEA', 50);

-- Major City Attack Penalty
UPDATE UnitPromotions SET CityAttack = -75 WHERE Type = 'PROMOTION_BIG_CITY_PENALTY';

UPDATE UnitPromotions SET Invisible = 'INVISIBLE_SUBMARINE' WHERE Type = 'PROMOTION_INVISIBLE_SUBMARINE';

UPDATE UnitPromotions SET CanMoveImpassable = 1 WHERE Type = 'PROMOTION_CAN_MOVE_IMPASSABLE';

UPDATE UnitPromotions SET AirSweepCapable = 1 WHERE Type = 'PROMOTION_AIR_SWEEP';

UPDATE UnitPromotions SET CityAttack = -25 WHERE Type = 'PROMOTION_STRAFING_RUNS';
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_STRAFING_RUNS', 'DOMAIN_LAND', 15),
	('PROMOTION_STRAFING_RUNS', 'DOMAIN_SEA', 15);

UPDATE UnitPromotions SET Recon = 1 WHERE Type = 'PROMOTION_AIR_RECON';
UPDATE UnitPromotions SET ReconChange = 1 WHERE Type = 'PROMOTION_AIR_RECON_RANGE_1';
UPDATE UnitPromotions SET ReconChange = 2 WHERE Type = 'PROMOTION_AIR_RECON_RANGE_2';

INSERT INTO UnitPromotions_UnitClasses
	(PromotionType, UnitClassType, Modifier)
VALUES
	('PROMOTION_ANTI_HELICOPTER', 'UNITCLASS_HELICOPTER_GUNSHIP', 150);

UPDATE UnitPromotions SET MaxHitPointsChange = 50, InterceptionDefenseDamageModifier = -25, EvasionChange = 25 WHERE Type = 'PROMOTION_STEALTH';

-- Domestic Focus
UPDATE UnitPromotions_Terrains SET PassableTech = 'TECH_COMPASS' WHERE PromotionType = 'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY';

UPDATE UnitPromotions SET ExtraNavalMovement = 1 WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';

UPDATE UnitPromotions SET RivalTerritory = 1 WHERE Type = 'PROMOTION_RIVAL_TERRITORY';

UPDATE UnitPromotions SET RivalTerritory = 1, ReligiousStrengthLossRivalTerritory = 25 WHERE Type = 'PROMOTION_UNWELCOME_EVANGELIST';

UPDATE UnitPromotions SET DiploMissionInfluence = 40 WHERE Type = 'PROMOTION_EMISSARY';
UPDATE UnitPromotions SET DiploMissionInfluence = 50 WHERE Type = 'PROMOTION_ENVOY';
UPDATE UnitPromotions SET DiploMissionInfluence = 60 WHERE Type = 'PROMOTION_DIPLOMAT';
UPDATE UnitPromotions SET DiploMissionInfluence = 70 WHERE Type = 'PROMOTION_AMBASSADOR';

UPDATE UnitPromotions SET GreatGeneral = 1 WHERE Type = 'PROMOTION_GREAT_GENERAL';

UPDATE UnitPromotions SET GreatAdmiral = 1, NumRepairCharges = 1 WHERE Type = 'PROMOTION_GREAT_ADMIRAL';

UPDATE UnitPromotions SET DiploMissionInfluence = 100, RivalTerritory = 1, ExtraNavalMovement = 2 WHERE Type = 'PROMOTION_GREAT_DIPLOMAT';

UPDATE UnitPromotions SET MilitaryCapChange = 1 WHERE Type = 'PROMOTION_MILITARY_TRADITION';

UPDATE UnitPromotions SET NukeImmune = 1 WHERE Type = 'PROMOTION_SHIELDED_SILO';

--------------------------------------------
-- Unique unit free promotions
--------------------------------------------

-- Camel Archer, Comanche Rider, Nau: Withdraw Before Melee
UPDATE UnitPromotions SET ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE';

-- Hoplite: Unity
UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_ADJACENT_BONUS';
INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Modifier)
SELECT
	'PROMOTION_ADJACENT_BONUS', Type, 15
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsNaval = 0 AND IsAerial = 0;

-- Battering Ram, Great Bombard: Skeleton Key
UPDATE UnitPromotions SET CityAttack = 150 WHERE Type = 'PROMOTION_CITY_ASSAULT';

-- Battering Ram: City Attack Only
UPDATE UnitPromotions SET CityAttackOnly = 1 WHERE Type = 'PROMOTION_ONLY_ATTACKS_CITIES';

-- Cataphract: Minor City Attack Penalty
UPDATE UnitPromotions SET CityAttack = -25 WHERE Type = 'PROMOTION_SMALL_CITY_PENALTY';

-- Cataphract: Open Terrain Bonus (30)
UPDATE UnitPromotions SET OpenAttack = 30, OpenDefense = 30 WHERE Type = 'PROMOTION_OPEN_TERRAIN';

-- Impi: Ranged Support Fire
UPDATE UnitPromotions SET RangedSupportFire = 1 WHERE Type = 'PROMOTION_RANGED_SUPPORT_FIRE';

-- Impi: Fasimba
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_FASIMBA', 'UNITCOMBAT_GUN', 25);

-- Companion Cavalry: Great Generals I
UPDATE UnitPromotions SET GreatGeneralModifier = 50 WHERE Type = 'PROMOTION_SPAWN_GENERALS_I';

-- Hoplite, Samurai: Great Generals II
UPDATE UnitPromotions SET GreatGeneralModifier = 100 WHERE Type = 'PROMOTION_SPAWN_GENERALS_II';

-- Companion Cavalry: Transfer Movement to General
UPDATE UnitPromotions SET GreatGeneralReceivesMovement = 1 WHERE Type = 'PROMOTION_MOVEMENT_TO_GENERAL';

-- Companion Cavalry, Naresuan's Elephant: Great General Combat Bonus
UPDATE UnitPromotions SET GreatGeneralCombatModifier = 25 WHERE Type = 'PROMOTION_GENERAL_STACKING';

-- Jaguar: Tenacity
UPDATE UnitPromotions SET HPHealedIfDestroyEnemy = 25 WHERE Type = 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY';

-- Hakkapeliitta: Hakkaa Päälle!
UPDATE UnitPromotions SET AttackAbove50HealthMod = 30 WHERE Type = 'PROMOTION_HAKKAA_PAALLE';

-- Foreign Legion: Foreign Lands Bonus
UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 20 WHERE Type = 'PROMOTION_FOREIGN_LANDS';

-- Immortal: Faster Healing
UPDATE UnitPromotions SET FriendlyHealChange = 10, NeutralHealChange = 10, EnemyHealChange = 10 WHERE Type = 'PROMOTION_FASTER_HEAL';

-- Bowman, Cossack: Marksmanship
UPDATE UnitPromotions SET AttackWoundedMod = 33 WHERE Type = 'PROMOTION_STRONGER_VS_DAMAGED';

-- Cossack: Withering Fire
UPDATE UnitPromotions SET MoraleBreakChance = -1 WHERE Type = 'PROMOTION_WITHERING_FIRE';

-- Pictish Warrior, Norwegian Ski Infantry: Highlander
UPDATE UnitPromotions_Terrains SET Attack = 0, Defense = 0 WHERE PromotionType = 'PROMOTION_SKI_INFANTRY';
INSERT INTO UnitPromotions_TerrainModifiers
	(PromotionType, TerrainType, Attack, Defense)
VALUES
	('PROMOTION_SKI_INFANTRY', 'TERRAIN_SNOW', 25, 25),
	('PROMOTION_SKI_INFANTRY', 'TERRAIN_TUNDRA', 25, 25),
	('PROMOTION_SKI_INFANTRY', 'TERRAIN_HILL', 25, 25);

-- Elephants: Feared Elephant
UPDATE UnitPromotions SET NearbyEnemyCombatMod = -10, NearbyEnemyCombatRange = 1 WHERE Type = 'PROMOTION_FEARED_ELEPHANT';

-- Maori Warrior: Haka War Dance
UPDATE UnitPromotions
SET
	NearbyEnemyCombatMod = -20,
	NearbyEnemyCombatRange = 1,
	EmbarkFlatCost = 1,
	DisembarkFlatCost = 1
WHERE Type = 'PROMOTION_HAKA_WAR_DANCE';

-- Khan: Khaaaan!
UPDATE UnitPromotions SET NearbyEnemyDamage = 10 WHERE Type = 'PROMOTION_MEDIC_GENERAL';

-- Samurai, Sabum Kibitum, Treasure Ship: Quick Study
UPDATE UnitPromotions SET ExperiencePercent = 50 WHERE Type = 'PROMOTION_GAIN_EXPERIENCE';

-- Merchant of Venice: Trade Mission Bonus
UPDATE UnitPromotions SET TradeMissionGoldModifier = 100, ExtraNavalMovement = 2 WHERE Type = 'PROMOTION_TRADE_MISSION_BONUS';

-- Winged Hussar: Heavy Charge
UPDATE UnitPromotions SET HeavyCharge = 1 WHERE Type = 'PROMOTION_HEAVY_CHARGE';

-- Siege Tower: Sapper
UPDATE UnitPromotions SET Sapper = 1 WHERE Type = 'PROMOTION_SAPPER';

-- Minuteman: Golden Age from Victories
UPDATE UnitPromotions SET GoldenAgeValueFromKills = 100 WHERE Type = 'PROMOTION_GOLDEN_AGE_POINTS';

-- Pracinha: Pride of the Nation
UPDATE UnitPromotions SET GoldenAgeValueFromKills = 1000 WHERE Type = 'PROMOTION_PRIDE_OF_THE_NATION';

-- Minuteman, Berber Cavalry: Ignores Terrain Cost
UPDATE UnitPromotions SET IgnoreTerrainCost = 1 WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST';

-- Mehal Sefari, Berber Cavalry: Homeland Guardian
UPDATE UnitPromotions SET FriendlyLandsModifier = 25 WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN';

-- Mehal Sefari: Near Capital Bonus
UPDATE UnitPromotions SET CapitalDefenseModifier = 30, CapitalDefenseFalloff = -1, CapitalDefenseLimit = 0 WHERE Type = 'PROMOTION_DEFEND_NEAR_CAPITAL';

-- Comanche Rider: Full Moon Striker
UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_MOON_STRIKER';
INSERT INTO UnitPromotions_YieldFromPillage
	(PromotionType, YieldType, YieldNoScale)
VALUES
	('PROMOTION_MOON_STRIKER', 'YIELD_SCIENCE', 200);

-- Kris Swordsman: Mystic Blade
UPDATE UnitPromotions SET HasPostCombatPromotions = 1 WHERE Type = 'PROMOTION_MYSTIC_BLADE';
DELETE FROM UnitPromotions_PostCombatRandomPromotion WHERE NewPromotion IN ('PROMOTION_RECRUITMENT', 'PROMOTION_HEROISM');

-- Hussar, Musketeer: Lightning Warfare
UPDATE UnitPromotions SET AttackMod = 15, MovesChange = 1, IgnoreZOC = 1 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE';

-- Free Company: City Plunder
UPDATE UnitPromotions SET CityAttackPlunderModifier = 100 WHERE Type = 'PROMOTION_DOUBLE_PLUNDER';

-- Mercenary: Enhanced Flank Attack
UPDATE UnitPromotions SET FlankAttackModifier = 10 WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS';

-- Sipahi: Heavy Flanking
UPDATE UnitPromotions SET FlankAttackModifier = 25 WHERE Type = 'PROMOTION_HEAVY_FLANKING';

-- Sea Beggar: Prize Ships
UPDATE UnitPromotions SET CaptureDefeatedEnemy = 1 WHERE Type = 'PROMOTION_PRIZE_SHIPS';

-- War Chariot: Gift of the Pharaoh
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_GIFT_OF_THE_PHARAOH', 'YIELD_PRODUCTION', 150);

-- Mandekalu Cavalry: Raider
UPDATE UnitPromotions SET FlankAttackModifier = 20, CityAttackPlunderModifier = 50, EnemyRoute = 1 WHERE Type = 'PROMOTION_RAIDER';

-- Bandeirante: Flag Bearer
INSERT INTO UnitPromotions_YieldFromScouting
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_FLAG_BEARER', 'YIELD_GOLD', 2),
	('PROMOTION_FLAG_BEARER', 'YIELD_SCIENCE', 2),
	('PROMOTION_FLAG_BEARER', 'YIELD_CULTURE', 2);

-- Longbowman: Assize of Arms
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_ASSIZE_OF_ARMS', 'UNITCOMBAT_MOUNTED', 20),
	('PROMOTION_ASSIZE_OF_ARMS', 'UNITCOMBAT_ARMOR', 20);

-- Longbowman: Agincourt
UPDATE UnitPromotions SET RangeChange = 1, MinEffectiveHealth = 100 WHERE Type = 'PROMOTION_AGINCOURT';

-- Horse Archer: Focus Fire
UPDATE UnitPromotions SET MultiAttackBonus = 10 WHERE Type = 'PROMOTION_FOCUS_FIRE';

-- Carolean: Grenadier
UPDATE UnitPromotions SET AOEDamageOnKill = 15 WHERE Type = 'PROMOTION_GRENADIER';

-- Legion: Pilum
UPDATE UnitPromotions SET AoEWhileFortified = 10 WHERE Type = 'PROMOTION_PILUM';

-- Legion: Praefectus Castrorum
UPDATE UnitPromotions SET WorkRateMod = 60, RequiresLeadership = 1 WHERE Type = 'PROMOTION_PRAEFECTUS_CASTRORUM';

-- Ballista: Legatus Legionis
UPDATE UnitPromotions SET MovesChange = 1, RequiresLeadership = 1 WHERE Type = 'PROMOTION_LEGATUS_LEGIONIS';

-- Landsknecht: Doppelsöldner
UPDATE UnitPromotions SET AttackFullyHealedMod = 30 WHERE Type = 'PROMOTION_DOPPELSOLDNER';

-- Warak'aq: Concussive Hit
UPDATE UnitPromotions SET IgnoreZOC = 1 WHERE Type = 'PROMOTION_CONCUSSIVE_HIT';
INSERT INTO UnitPromotions_Plagues
	(PromotionType, PlaguePromotionType, DomainType, ApplyOnAttack)
VALUES
	('PROMOTION_CONCUSSIVE_HIT', 'PROMOTION_DAZED', 'DOMAIN_LAND', 1);

-- Chu-Ko-Nu: Siege Volley
UPDATE UnitPromotions SET NearbyCityCombatMod = 25, NearbyRange = 1, SplashDamage = 5 WHERE Type = 'PROMOTION_SIEGE_VOLLEY';

-- Quinquereme: Heavy Assault
UPDATE UnitPromotions SET AttackMod = 25, CityAttack = 25 WHERE Type = 'PROMOTION_HEAVY_ASSAULT';

-- Monitor: Hardened
UPDATE UnitPromotions SET ChangeDamageValue = -3 WHERE Type = 'PROMOTION_HARDENED';

-- Monitor: Harbor Defense
UPDATE UnitPromotions SET AdjacentCityDefenseMod = 5 WHERE Type = 'PROMOTION_HARBOR_DEFENSE';

-- Hashemite Raider: Garland Mine
UPDATE UnitPromotions SET FreePillageMoves = 1, AOEDamageOnPillage = 15 WHERE Type = 'PROMOTION_GARLAND_MINE';

-- Hashemite Raider: Desert Raider
UPDATE UnitPromotions_TerrainModifiers SET Attack = 25, Defense = 25 WHERE PromotionType = 'PROMOTION_DESERT_RAIDER';
INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, DoubleMove)
VALUES
	('PROMOTION_DESERT_RAIDER', 'TERRAIN_DESERT', 1);

-- Iron Chariot: Shock Cavalry
UPDATE UnitPromotions SET OpenAttack = 20, OpenDefense = 20 WHERE Type = 'PROMOTION_SHOCK_CAVALRY';

-- Grenzer: Frontiersman
UPDATE UnitPromotions SET BorderMod = 40 WHERE Type = 'PROMOTION_FRONTIERSMAN';

-- Otomi: Brute Strength
UPDATE UnitPromotions SET BarbarianCombatBonus = 40 WHERE Type = 'PROMOTION_BRUTE_STRENGTH';

-- Sabum Kibitum: Legacy
UPDATE UnitPromotions SET CombatModPerLevel = 5 WHERE Type = 'PROMOTION_LEGACY';

-- Amazonas: Riachuelo
UPDATE UnitPromotions SET GoldenAgeValueFromKills = 100 WHERE Type = 'PROMOTION_RIACHUELO';
INSERT INTO UnitPromotions_BlockedPromotions
	(PromotionType, BlockedPromotionType)
VALUES
	('PROMOTION_RIACHUELO', 'PROMOTION_BOARDED_1'),
	('PROMOTION_RIACHUELO', 'PROMOTION_BOARDED_2');

-- Dromon: Greek Fire
INSERT INTO UnitPromotions_Plagues
	(PromotionType, PlaguePromotionType, DomainType, ApplyOnAttack)
VALUES
	('PROMOTION_GREEK_FIRE', 'PROMOTION_ON_FIRE', 'DOMAIN_LAND', 1),
	('PROMOTION_GREEK_FIRE', 'PROMOTION_ON_FIRE', 'DOMAIN_SEA', 1);

-- Asamu: Elisha's Guile
UPDATE UnitPromotions
SET
	AllowsEmbarkation = 1,
	VisibilityChange = 1,
	EmbarkExtraVisibility = 1,
	ExtraWithdrawal = 100
WHERE Type = 'PROMOTION_ELISHAS_GUILE';

-- Atlas Elephant: At the Gates!
UPDATE UnitPromotions SET MaxHitPointsChange = 20, CanCrossMountains = 1 WHERE Type = 'PROMOTION_AT_THE_GATES';

-- Scythed Chariot: Rend
UPDATE UnitPromotions SET AoEDamageOnMove = 5 WHERE Type = 'PROMOTION_REND';

-- Scythed Chariot: Carnyx
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Attack)
VALUES
	('PROMOTION_CARNYX', 'UNITCOMBAT_MELEE', 10),
	('PROMOTION_CARNYX', 'UNITCOMBAT_ARCHER', 10);

-- Treasure Ship: Kowtow
UPDATE UnitPromotions SET InfluenceFromCombatXpTimes100 = 100 WHERE Type = 'PROMOTION_KOWTOW';

-- Mamluk: Vur Ha!
UPDATE UnitPromotions SET AttackAbove50HealthMod = 25, FreeAttackMoves = 1 WHERE Type = 'PROMOTION_VUR_HA';

-- Redcoat: Rule Britannia
UPDATE UnitPromotions SET EmbarkDefenseModifier = 100, EmbarkExtraVisibility = 2 WHERE Type = 'PROMOTION_RULE_BRITANNIA';
INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_RULE_BRITANNIA', 'TERRAIN_HILL', 1);

-- Chewa: Maim
INSERT INTO UnitPromotions_Plagues
	(PromotionType, PlaguePromotionType, DomainType, ApplyOnAttack)
VALUES
	('PROMOTION_MAIM', 'PROMOTION_MAIMED', 'DOMAIN_LAND', 1);

-- Chewa: Hooked Weapon
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_HOOKED_WEAPON', 'UNITCOMBAT_MOUNTED', 25);

-- Oromo Cavalry: Zemene Mesafint
UPDATE UnitPromotions SET VsUnhappyMod = 20 WHERE Type = 'PROMOTION_ZEMENE_MESAFINT';

-- Krupp Gun: Minenwerfer
UPDATE UnitPromotions SET AttackFortifiedMod = 50, PillageFortificationsOnKill = 1 WHERE Type = 'PROMOTION_MINENWERFER';

-- Krupp Gun: Trommelfeuer
UPDATE UnitPromotions SET AttackModPerSamePromotionAttack = 3, AttackModPerSamePromotionAttackCap = 30 WHERE Type = 'PROMOTION_TROMMELFEUER';

-- Klepht: Philhellenism
UPDATE UnitPromotions SET CombatModPerCSAlliance = 5 WHERE Type = 'PROMOTION_PHILHELLENISM';

-- Tarkhan: Bellum Alet
UPDATE UnitPromotions SET AoEHealOnPillage = 15 WHERE Type = 'PROMOTION_BELLUM_ALET';

-- Inti Maceman: Sentinel of the Sun
UPDATE UnitPromotions SET MaxHitPointsChange = 15 WHERE Type = 'PROMOTION_SENTINEL_OF_THE_SUN';
INSERT INTO UnitPromotions_TerrainModifiers
	(PromotionType, TerrainType, Attack, Defense)
VALUES
	('PROMOTION_SENTINEL_OF_THE_SUN', 'TERRAIN_HILL', 25, 25),
	('PROMOTION_SENTINEL_OF_THE_SUN', 'TERRAIN_MOUNTAIN', 25, 25);

-- Dhanurdhara: Epic
UPDATE UnitPromotions SET ExtraXPOnKill = 5 WHERE Type = 'PROMOTION_EPIC';

-- Dhanurdhara: Dhanurvidya
INSERT INTO UnitPromotions_YieldFromCombatExperienceTimes100
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_DHANURVIDYA', 'YIELD_CULTURE', 50),
	('PROMOTION_DHANURVIDYA', 'YIELD_FAITH', 50);

-- Djong: Cetbang
UPDATE UnitPromotions SET NearbyEnemyDamage = 10 WHERE Type = 'PROMOTION_CETBANG';

-- Yamato: Taikan Kyoho
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_TAIKAN_KYOHO', 'YIELD_GREAT_ADMIRAL_POINTS', 400);

-- Turtle Ship: Deck Spikes
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_DECK_SPIKES', 'UNITCOMBAT_NAVALMELEE', 25);

-- Holkan: Lost Codex
UPDATE UnitPromotions SET GoodyHutYieldBonus = 25 WHERE Type = 'PROMOTION_LOST_CODEX';
INSERT INTO UnitPromotions_YieldFromAncientRuins
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_LOST_CODEX', 'YIELD_SCIENCE', 5),
	('PROMOTION_LOST_CODEX', 'YIELD_CULTURE', 5);

-- Black Tug: Mingghan
UPDATE UnitPromotions SET XPFromPillaging = 3, PillageBonusStrength = 20 WHERE Type = 'PROMOTION_MINGGHAN';

-- Corsair: Razzia
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_RAZZIA', 'YIELD_FOOD', 200),
	('PROMOTION_RAZZIA', 'YIELD_PRODUCTION', 200);
INSERT INTO UnitPromotions_YieldFromPillage
	(PromotionType, YieldType, YieldNoScale)
VALUES
	('PROMOTION_RAZZIA', 'YIELD_FOOD', 100),
	('PROMOTION_RAZZIA', 'YIELD_PRODUCTION', 100);
INSERT INTO UnitPromotions_YieldFromTRPlunder
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_RAZZIA', 'YIELD_FOOD', 200),
	('PROMOTION_RAZZIA', 'YIELD_PRODUCTION', 200);

-- Goedendag: Burgher
UPDATE UnitPromotions SET NearbyFriendlyCityCombatMod = 33, NearbyRange = 2 WHERE Type = 'PROMOTION_BURGHER';

-- Goedendag: Goedendag!
UPDATE UnitPromotions
SET
	IsNearbyPromotion = 1,
	GiveCombatMod = 10,
	GiveDomain = 'DOMAIN_LAND',
	NearbyRange = 1
WHERE Type = 'PROMOTION_GOEDENDAG';

-- Great Bombard: Unwieldy
UPDATE UnitPromotions SET CannotHeal = 1 WHERE Type = 'PROMOTION_UNWIELDY';
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_UNWIELDY', 'DOMAIN_LAND', -50);

-- Qizilbash: Safavi Agitator
INSERT INTO UnitPromotions_YieldFromPillage
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_SAFAVI_AGITATOR', 'YIELD_GOLD', 20),
	('PROMOTION_SAFAVI_AGITATOR', 'YIELD_FAITH', 10),
	('PROMOTION_SAFAVI_AGITATOR', 'YIELD_GOLDEN_AGE_POINTS', 30);

-- Cacador: Fighting Cock
UPDATE UnitPromotions SET StrongerDamaged = 1 WHERE Type = 'PROMOTION_FIGHTING_COCK';

-- Licorne: Grapeshot
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Attack)
VALUES
	('PROMOTION_GRAPESHOT', 'UNITCOMBAT_MELEE', 33),
	('PROMOTION_GRAPESHOT', 'UNITCOMBAT_GUN', 33);

-- Yellow Brow: Big Horse Dance
UPDATE UnitPromotions SET StrongerDamaged = 1, FortifiedModifier = 20 WHERE Type = 'PROMOTION_BIG_HORSE_DANCE';

-- Suea Mop: Crouching Tiger
UPDATE UnitPromotions SET TileDamageIfNotMoved = 15 WHERE Type = 'PROMOTION_CROUCHING_TIGER';

-- Sofa: Father of the Horse
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_FATHER_OF_THE_HORSE', 'UNITCOMBAT_MOUNTED', 15);

-- Armada: Santa Maria
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_SANTA_MARIA', 'YIELD_GREAT_ADMIRAL_POINTS', 100);

-- Armada: Invincible
UPDATE UnitPromotions SET CombatChange = 6, MinEffectiveHealth = 100 WHERE Type = 'PROMOTION_INVINCIBLE';

--------------------------------------------
-- Post-combat promotions
--------------------------------------------

-- Mystic Blade
UPDATE UnitPromotions SET FriendlyHealChange = 20, NeutralHealChange = 20, EnemyHealChange = 20, ChangeDamageValue = -2 WHERE Type = 'PROMOTION_INVULNERABIILITY';
UPDATE UnitPromotions SET FlankAttackModifier = 20, IgnoreTerrainCost = 1, IgnoreZOC = 1 WHERE Type = 'PROMOTION_SNEAK_ATTACK';
UPDATE UnitPromotions SET NearbyEnemyDamage = 10, EnemyDamage = -10, EnemyDamageChance = 100 WHERE Type = 'PROMOTION_ENEMY_BLADE';
UPDATE UnitPromotions SET AttackMod = 30, DefenseMod = 5 WHERE Type = 'PROMOTION_AMBITION';
UPDATE UnitPromotions SET AttackMod = 5, DefenseMod = 30 WHERE Type = 'PROMOTION_EVIL_SPIRITS';
UPDATE UnitPromotions SET MovesChange = 1, ExtraAttacks = 1 WHERE Type = 'PROMOTION_RESTLESSNESS';

-- Eight Virtues of Bushido
UPDATE UnitPromotions SET StrongerDamaged = 1, HPHealedIfDestroyEnemy = 10 WHERE RankList = 'BUSHIDO';
UPDATE UnitPromotions SET AttackAbove50HealthMod = 20 WHERE Type = 'PROMOTION_RESPECT';
UPDATE UnitPromotions SET AttackBelowEqual50HealthMod = 20 WHERE Type = 'PROMOTION_RIGHTEOUSNESS';
UPDATE UnitPromotions SET FriendlyLandsModifier = 15 WHERE Type = 'PROMOTION_LOYALTY';
UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 15 WHERE Type = 'PROMOTION_HONOR';
UPDATE UnitPromotions SET AttackMod = 15 WHERE Type = 'PROMOTION_COURAGE';
UPDATE UnitPromotions SET DefenseMod = 15 WHERE Type = 'PROMOTION_SELF_CONTROL';
UPDATE UnitPromotions SET AdjacentTileHealChange = 5 WHERE Type = 'PROMOTION_BENEVOLENCE';
UPDATE UnitPromotions SET ExperiencePercent = 50 WHERE Type = 'PROMOTION_SINCERITY';

--------------------------------------------
-- Event free promotions
--------------------------------------------
UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_FERVOR';

--------------------------------------------
-- Quest promotions
--------------------------------------------
UPDATE UnitPromotions SET MaxHitPointsChange = 25 WHERE Type = 'PROMOTION_JUGGERNAUT';

--------------------------------------------
-- Barbarian promotions
--------------------------------------------
UPDATE UnitPromotions
SET BarbarianOnly = 1
WHERE Type IN (
	'PROMOTION_MARSH_WALKER',
	'PROMOTION_WHITE_WALKER',
	'PROMOTION_WOODS_WALKER',
	'PROMOTION_DESERT_WALKER',
	'PROMOTION_HILL_WALKER'
);

INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_MARSH_WALKER', 'FEATURE_MARSH', 1),
	('PROMOTION_WOODS_WALKER', 'FEATURE_FOREST', 1),
	('PROMOTION_WOODS_WALKER', 'FEATURE_JUNGLE', 1);

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_HILL_WALKER', 'TERRAIN_HILL', 1),
	('PROMOTION_WHITE_WALKER', 'TERRAIN_SNOW', 1),
	('PROMOTION_DESERT_WALKER', 'TERRAIN_DESERT', 1);

--------------------------------------------
-- Plagues
--------------------------------------------
UPDATE UnitPromotions SET PlagueID = 1, PlaguePriority = 0, PromotionDuration = 1, MovesChange = -2 WHERE Type = 'PROMOTION_BOARDED_1';
UPDATE UnitPromotions SET PlagueID = 1, PlaguePriority = 1, PromotionDuration = 1, MovesChange = -4 WHERE Type = 'PROMOTION_BOARDED_2';

UPDATE UnitPromotions SET PlagueID = 2, PromotionDuration = 5, CombatPercent = -15 WHERE Type = 'PROMOTION_DAZED';

UPDATE UnitPromotions SET PlagueID = 3, PromotionDuration = 50, WorkRateMod = -50 WHERE Type = 'PROMOTION_PRISONERS_OF_WAR';

UPDATE UnitPromotions SET PlagueID = 4, PromotionDuration = 1, DamageTakenMod = 20 WHERE Type = 'PROMOTION_ON_FIRE';

UPDATE UnitPromotions SET PlagueID = 5, PromotionDuration = 1, MovesChange = -1 WHERE Type = 'PROMOTION_MAIMED';

--------------------------------------------
-- Promotions that provide blanket immunity to all Plagues
--------------------------------------------

INSERT INTO UnitPromotions_BlockedPromotions
    (PromotionType, BlockedPromotionType)
SELECT
    a.PromotionType,
    b.PlaguePromotionType
FROM PlagueImmunePromotions a, UnitPromotions_Plagues b
WHERE (a.DomainType = b.DomainType OR a.DomainType IS NULL OR b.DomainType IS NULL)

UNION ALL

-- Special plagues = any promotion with a PlagueID != -1 that is not already in UnitPromotions_Plagues
SELECT
    a.PromotionType,
    b.Type
FROM PlagueImmunePromotions a, UnitPromotions b
WHERE a.IncludeSpecial = 1
AND b.PlagueID <> -1
AND NOT EXISTS (SELECT 1 FROM UnitPromotions_Plagues WHERE PlaguePromotionType = b.Type)

-- Avoid duplicates
EXCEPT
SELECT
    PromotionType,
    BlockedPromotionType
FROM UnitPromotions_BlockedPromotions;

DROP TABLE PlagueImmunePromotions;
