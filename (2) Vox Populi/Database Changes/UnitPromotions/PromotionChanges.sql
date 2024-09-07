-- Unfortunately, the <Replace> command isn't smart enough to only replace specified values instead of whole rows,
-- so we need to redefine all existing and new promotions here.

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

-- Overrun
UPDATE UnitPromotions SET FlankAttackModifier = 25, AOEDamageOnKill = 10 WHERE Type = 'PROMOTION_SHOCK_4';

UPDATE UnitPromotions SET AlwaysHeal = 1 WHERE RankList = 'MARCH';

UPDATE UnitPromotions SET ExtraAttacks = 1, CanMoveAfterAttacking = 1 WHERE Type = 'PROMOTION_BLITZ';

UPDATE UnitPromotions SET DefenseMod = 35 WHERE Type = 'PROMOTION_DRILL_4';

UPDATE UnitPromotions SET RoughAttack = 15 WHERE RankList = 'AMBUSH';

UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_MOBILITY';

UPDATE UnitPromotions SET RangedDefenseMod = 10 WHERE RankList = 'AIR_DEFENSE';
UPDATE UnitPromotions SET LandAirDefenseBonus = 15 WHERE Type = 'PROMOTION_ANTIAIR_LAND_1';
UPDATE UnitPromotions SET LandAirDefenseBonus = 20 WHERE Type = 'PROMOTION_ANTIAIR_LAND_2';
UPDATE UnitPromotions SET LandAirDefenseBonus = 25 WHERE Type = 'PROMOTION_ANTIAIR_LAND_3';

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

-- Firing Doctrine
UPDATE UnitPromotions SET AttackAbove50HealthMod = 25 WHERE Type = 'PROMOTION_BARRAGE_4';

-- Infiltrators
UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 15, AttackWoundedMod = 15, NoAdjacentUnitMod = 10 WHERE Type = 'PROMOTION_ACCURACY_4';

UPDATE UnitPromotions SET ExtraAttacks = 1, RangedAttackModifier = -30 WHERE Type = 'PROMOTION_LOGISTICS';

-- Archer only
UPDATE UnitPromotions SET RangeChange = 1, RangedAttackModifier = -20, MinimumRangeRequired = 2 WHERE Type = 'PROMOTION_RANGE';

-- Skirmisher only
UPDATE UnitPromotions
SET MountedOnly = 1
WHERE Type IN (
	'PROMOTION_SKIRMISHER_MOBILITY',
	'PROMOTION_SKIRMISHER_POWER',
	'PROMOTION_SKIRMISHER_MARCH',
	'PROMOTION_SKIRMISHER_SENTRY',
	'PROMOTION_ENVELOPMENT'
);

-- Parthian Tactics
UPDATE UnitPromotions SET MovesChange = 1, IgnoreZOC = 1 WHERE Type = 'PROMOTION_SKIRMISHER_MOBILITY';

-- Coup de Grace
UPDATE UnitPromotions SET AttackBelowEqual50HealthMod = 30 WHERE Type = 'PROMOTION_SKIRMISHER_POWER';

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
--                                       │                            Infiltrators
-- Siege I ─────────► Siege II ──────────┴──────► Siege III ────────► Range
--
-- Cover I ─────────► Cover II
--                                                                    Firing Doctrine
-- Field I ────┬────► Field II ──────────┬──────► Field III ────────► Logistics
--             │                         │
--             └────► Coastal Artillery  └──────► Splash Damage I ──► Splash Damage II
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

UPDATE UnitPromotions SET VisibilityChange = 1, EmbarkExtraVisibility = 1 WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_1';
UPDATE UnitPromotions SET MovesChange = 1, ExtraNavalMovement = 1, River = 1 WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_2';
UPDATE UnitPromotions
SET
	OutsideFriendlyLandsModifier = 20,
	AllowsEmbarkation = 1,
	CanCrossMountains = 1,
	EnemyRoute = 1,
	IgnoreZOC = 1
WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_3';

INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_WOODLAND_TRAILBLAZER_1', 'FEATURE_JUNGLE', 1),
	('PROMOTION_WOODLAND_TRAILBLAZER_1', 'FEATURE_FOREST', 1),
	('PROMOTION_WOODLAND_TRAILBLAZER_2', 'FEATURE_MARSH', 1);

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, IgnoreTerrainCostIn)
VALUES
	('PROMOTION_WOODLAND_TRAILBLAZER_1', 'TERRAIN_HILL', 1),
	('PROMOTION_WOODLAND_TRAILBLAZER_2', 'TERRAIN_DESERT', 1),
	('PROMOTION_WOODLAND_TRAILBLAZER_2', 'TERRAIN_SNOW', 1);

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
UPDATE UnitPromotions SET PlaguePromotion = 'PROMOTION_BOARDED_1', PlagueChance = 100 WHERE Type = 'PROMOTION_BOARDING_PARTY_1';
UPDATE UnitPromotions SET PlagueIDImmunity = 1 WHERE Type = 'PROMOTION_BOARDING_PARTY_2';
UPDATE UnitPromotions SET PlaguePromotion = 'PROMOTION_BOARDED_2', PlagueChance = 100 WHERE Type = 'PROMOTION_BOARDING_PARTY_3';

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

-- Commerce Raider
UPDATE UnitPromotions SET FreePillageMoves = 1, HealOnPillage = 1 WHERE Type = 'PROMOTION_PRESS_GANGS';

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

-- Vanguard
UPDATE UnitPromotions SET CityAttack = 125, DamageReductionCityAssault = 50 WHERE Type = 'PROMOTION_COASTAL_TERROR';

-- Dauntless
UPDATE UnitPromotions SET ChangeDamageValue = -5, FriendlyHealChange = 10, NeutralHealChange = 10, EnemyHealChange = 10 WHERE Type = 'PROMOTION_DAMAGE_REDUCTION';

-- Blockade
UPDATE UnitPromotions SET RangedDefenseMod = 25, ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_COASTAL_RAIDER_4';

-- Pincer
UPDATE UnitPromotions SET FlankAttackModifier = 10, IgnoreZOC = 1 WHERE Type = 'PROMOTION_BOARDING_PARTY_4';

UPDATE UnitPromotions SET HealOutsideFriendly = 1, FriendlyHealChange = 5, NeutralHealChange = 5, EnemyHealChange = 5 WHERE Type = 'PROMOTION_SUPPLY';

----------------------------------------------------------------------------------------------------------------------------
-- Naval Ranged promotion tree drawn using ASCIIFlow
--                                                                Splash Damage I
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

-- Indomitable
UPDATE UnitPromotions SET DefenseMod = 25, PlagueIDImmunity = 1 WHERE Type = 'PROMOTION_TARGETING_4';

-- Broadside
UPDATE UnitPromotions SET CityAttack = 40 WHERE Type = 'PROMOTION_BOMBARDMENT_4';

-- Shrapnel Rounds
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_BETTER_BOMBARDMENT', 'DOMAIN_LAND', 50);

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

-- Wolfpack
INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Attack)
VALUES
	('PROMOTION_TRUE_WOLFPACK', 'UNITCOMBAT_SUBMARINE', 30);

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

UPDATE UnitPromotions SET AllowsEmbarkation = 1, EmbarkedAllWater = 1, EmbarkExtraVisibility = 2 WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION';

UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';

UPDATE UnitPromotions SET CaptureDefeatedEnemy = 1, CapturedUnitsConscripted = 1 WHERE Type = 'PROMOTION_COERCION';

UPDATE UnitPromotions SET RiverDoubleMove = 1 WHERE Type = 'PROMOTION_WAR_CANOES';

UPDATE UnitPromotions SET ExtraAttacks = 1 WHERE Type = 'PROMOTION_MONGOL_TERROR';

UPDATE UnitPromotions SET AttackMod = 20 WHERE Type = 'PROMOTION_ATTACK_BONUS_SWEDEN';

--------------------------------------------
-- Policy free promotions
--------------------------------------------
UPDATE UnitPromotions SET MovesChange = 1 WHERE Type = 'PROMOTION_FASTER_WORKER';

UPDATE UnitPromotions SET HPHealedIfDestroyEnemy = 15 WHERE Type = 'PROMOTION_KILL_HEAL';

UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_HONOR_BONUS';

UPDATE UnitPromotions SET VisibilityChange = 1 WHERE Type = 'PROMOTION_IMPERIALISM_OPENER';
UPDATE UnitPromotions SET MovesChange = 1 WHERE Type IN ('PROMOTION_FASTER_GENERAL', 'PROMOTION_NAVAL_TRADITION');

UPDATE UnitPromotions SET AuraRangeChange = 1, AuraEffectChange = 10 WHERE Type = 'PROMOTION_BETTER_LEADERSHIP';

UPDATE UnitPromotions SET StrongerDamaged = 1 WHERE Type = 'PROMOTION_NAVAL_DEFENSE_BOOST';

UPDATE UnitPromotions SET NearbyCityCombatMod = 20, NearbyRange = 1 WHERE Type = 'PROMOTION_NATIONALISM';

UPDATE UnitPromotions SET MovesChange = 3 WHERE Type = 'PROMOTION_FAST_GENERAL';
UPDATE UnitPromotions SET AttackMod = 15, IgnoreZOC = 1 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_GUN';
UPDATE UnitPromotions SET AttackMod = 15, MovesChange = 1 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_ARMOR';

UPDATE UnitPromotions SET FriendlyHealChange = 5, CityAttack = 25 WHERE Type = 'PROMOTION_MARE_NOSTRUM';

--------------------------------------------
-- Tech free promotions
--------------------------------------------
UPDATE UnitPromotions SET AllowsEmbarkation = 1 WHERE Type = 'PROMOTION_EMBARKATION';

-- Engineering Corp
UPDATE UnitPromotions SET CanMoveImpassable = 1 WHERE Type = 'PROMOTION_ICE_BREAKERS';

-- Fallout Resistance
UPDATE UnitPromotions SET ExtraFeatureDamage = 1 WHERE Type = 'PROMOTION_FALLOUT_REDUCTION';

UPDATE UnitPromotions SET IgnoreFeatureDamage = 1 WHERE Type = 'PROMOTION_FALLOUT_IMMUNITY';

--------------------------------------------
-- Natural Wonder free promotions
--------------------------------------------
UPDATE UnitPromotions SET HillsDoubleMove = 1 WHERE Type = 'PROMOTION_ALTITUDE_TRAINING';

UPDATE UnitPromotions SET MovesChange = 2 WHERE Type = 'PROMOTION_SACRED_STEPS';

UPDATE UnitPromotions SET FriendlyHealChange = 5, NeutralHealChange = 5, EnemyHealChange = 5 WHERE Type = 'PROMOTION_EVERLASTING_YOUTH';

--------------------------------------------
-- Building free promotions
--------------------------------------------
UPDATE UnitPromotions SET FriendlyLandsModifier = 15 WHERE Type = 'PROMOTION_HIMEJI_CASTLE';

UPDATE UnitPromotions SET CityAttack = 25, PlagueIDImmunity = 3 WHERE Type = 'PROMOTION_STATUE_ZEUS';

-- Jinete
UPDATE UnitPromotions SET AttackMod = 15, ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_ALHAMBRA';

UPDATE UnitPromotions SET AttackMod = 10, VisibilityChange = 1, HealOutsideFriendly = 1 WHERE Type = 'PROMOTION_TREASURE_FLEET';

-- Venetian Craftsmanship
UPDATE UnitPromotions SET CombatPercent = 10, MovesChange = 1 WHERE Type = 'PROMOTION_ARSENALE';

UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_MORALE';

UPDATE UnitPromotions SET AttackMod = 10 WHERE Type = 'PROMOTION_IKLWA';

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
	('PROMOTION_BUSHIDO', 'PROMOTION_BUSHIDO_HONOR'),
	('PROMOTION_BUSHIDO', 'PROMOTION_LOYALTY'),
	('PROMOTION_BUSHIDO', 'PROMOTION_SELF_CONTROL');

UPDATE UnitPromotions SET DiploMissionInfluence = 10 WHERE RankList = 'DIPLO_BOOST';
UPDATE UnitPromotions SET DiploMissionInfluence = 15, RivalTerritory = 1 WHERE Type = 'PROMOTION_NOBILITY';
UPDATE UnitPromotions SET MovesChange = 1, ExtraNavalMovement = 1 WHERE Type IN ('PROMOTION_EXPRESS', 'PROMOTION_PAX', 'PROMOTION_LITERACY');
UPDATE UnitPromotions SET IgnoreTerrainCost = 1 WHERE Type = 'PROMOTION_LITERACY';
UPDATE UnitPromotions SET RivalTerritory = 1 WHERE Type = 'PROMOTION_IMMUNITY';

--------------------------------------------
-- Unit free promotions
--------------------------------------------

-- Brute Force
UPDATE UnitPromotions SET BarbarianCombatBonus = 25 WHERE Type = 'PROMOTION_BARBARIAN_BONUS';

UPDATE UnitPromotions SET MaxHitPointsChange = 10, RangedDefenseMod = 10 WHERE Type = 'PROMOTION_FIELD_WORKS_0';
UPDATE UnitPromotions SET MaxHitPointsChange = 15, RangedDefenseMod = 15 WHERE Type = 'PROMOTION_FIELD_WORKS_1';
UPDATE UnitPromotions SET MaxHitPointsChange = 20, RangedDefenseMod = 20 WHERE Type = 'PROMOTION_FIELD_WORKS_2';
UPDATE UnitPromotions SET MaxHitPointsChange = 25, RangedDefenseMod = 25 WHERE Type = 'PROMOTION_FIELD_WORKS_3';

UPDATE UnitPromotions SET OnlyDefensive = 1 WHERE Type = 'PROMOTION_ONLY_DEFENSIVE';

INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_NAVAL_MISFIRE', 'DOMAIN_SEA', -20);

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

-- Titanic
UPDATE UnitPromotions SET AttackMod = 25, MaxHitPointsChange = 100, RangedSupportFire = 1 WHERE Type = 'PROMOTION_MECH';

-- Beam Axle
INSERT INTO UnitPromotions_Features
	(PromotionType, FeatureType, ExtraMove)
VALUES
	('PROMOTION_ROUGH_TERRAIN_HALF_TURN', 'FEATURE_FOREST', 1),
	('PROMOTION_ROUGH_TERRAIN_HALF_TURN', 'FEATURE_JUNGLE', 1),
	('PROMOTION_ROUGH_TERRAIN_HALF_TURN', 'FEATURE_MARSH', 1);

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, ExtraMove)
VALUES
	('PROMOTION_ROUGH_TERRAIN_HALF_TURN', 'TERRAIN_HILL', 1);

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

-- Reconnaissance
UPDATE UnitPromotions SET GainsXPFromScouting = 1 WHERE Type = 'PROMOTION_RECON_EXPERIENCE';

-- Ocean Explorer
UPDATE UnitPromotions SET CanCrossOceans = 1 WHERE Type = 'PROMOTION_OCEAN_CROSSING';

-- Extra Sight While Embarked
UPDATE UnitPromotions SET EmbarkExtraVisibility = 1 WHERE Type = 'PROMOTION_EMBARKED_SIGHT';

-- Embarkation with Defense
UPDATE UnitPromotions SET EmbarkDefenseModifier = 100 WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION';

-- Scavenger
UPDATE UnitPromotions SET GainsXPFromPillaging = 1 WHERE Type = 'PROMOTION_SCOUT_XP_PILLAGE';

UPDATE UnitPromotions SET ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_COMMANDO';

UPDATE UnitPromotions SET AttackMod = 25 WHERE Type = 'PROMOTION_ATTACK_BONUS';

UPDATE UnitPromotions SET FreePillageMoves = 1 WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES';

UPDATE UnitPromotions SET DropRange = 9 WHERE Type = 'PROMOTION_PARADROP';
UPDATE UnitPromotions SET DropRange = 40 WHERE Type = 'PROMOTION_EXTENDED_PARADROP';

INSERT INTO UnitPromotions_Terrains
	(PromotionType, TerrainType, HalfMove)
VALUES
	('PROMOTION_OCEAN_HALF_MOVES', 'TERRAIN_OCEAN', 1);

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

-- Strafing Runs
UPDATE UnitPromotions SET CityAttack = -25 WHERE Type = 'PROMOTION_AIR_MISFIRE';
INSERT INTO UnitPromotions_Domains
	(PromotionType, DomainType, Attack)
VALUES
	('PROMOTION_AIR_MISFIRE', 'DOMAIN_LAND', 15),
	('PROMOTION_AIR_MISFIRE', 'DOMAIN_SEA', 15);

UPDATE UnitPromotions SET Recon = 1 WHERE Type = 'PROMOTION_AIR_RECON';
UPDATE UnitPromotions SET ReconChange = 1 WHERE Type = 'PROMOTION_RECON_RANGE_1';
UPDATE UnitPromotions SET ReconChange = 2 WHERE Type = 'PROMOTION_RECON_RANGE_2';

INSERT INTO UnitPromotions_UnitClasses
	(PromotionType, UnitClassType, Modifier)
VALUES
	('PROMOTION_ANTI_HELICOPTER', 'UNITCLASS_HELICOPTER_GUNSHIP', 150);

UPDATE UnitPromotions SET MaxHitPointsChange = 50, InterceptionDefenseDamageModifier = -25, EvasionChange = 25 WHERE Type = 'PROMOTION_STEALTH';

-- Domestic Focus
UPDATE UnitPromotions_Terrains SET PassableTech = 'TECH_COMPASS' WHERE PromotionType = 'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY';

UPDATE UnitPromotions SET RivalTerritory = 1 WHERE Type = 'PROMOTION_RIVAL_TERRITORY';

UPDATE UnitPromotions SET RivalTerritory = 1, ReligiousStrengthLossRivalTerritory = 25 WHERE Type = 'PROMOTION_UNWELCOME_EVANGELIST';

UPDATE UnitPromotions SET DiploMissionInfluence = 40 WHERE Type = 'PROMOTION_EMISSARY';
UPDATE UnitPromotions SET DiploMissionInfluence = 50 WHERE Type = 'PROMOTION_ENVOY';
UPDATE UnitPromotions SET DiploMissionInfluence = 60 WHERE Type = 'PROMOTION_DIPLOMAT';
UPDATE UnitPromotions SET DiploMissionInfluence = 70 WHERE Type = 'PROMOTION_AMBASSADOR';

UPDATE UnitPromotions SET GreatGeneral = 1 WHERE Type = 'PROMOTION_GREAT_GENERAL';

UPDATE UnitPromotions SET GreatAdmiral = 1, NumRepairCharges = 1 WHERE Type = 'PROMOTION_GREAT_ADMIRAL';

UPDATE UnitPromotions SET DiploMissionInfluence = 100, RivalTerritory = 1, ExtraNavalMovement = 2 WHERE Type = 'PROMOTION_GREAT_DIPLOMAT';

UPDATE UnitPromotions SET MilitaryCapChange = 1 WHERE Type = 'PROMOTION_SUPPLY_BOOST';

UPDATE UnitPromotions SET NukeImmune = 1 WHERE Type = 'PROMOTION_NUCLEAR_SILO';

--------------------------------------------
-- Unique unit free promotions
--------------------------------------------

-- Camel Archer, Comanche Riders, Nau: Withdraw Before Melee
UPDATE UnitPromotions SET ExtraWithdrawal = 100 WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE';

-- Hoplite: Unity
UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_ADJACENT_BONUS';
INSERT INTO UnitPromotions_CombatModPerAdjacentUnitCombat
	(PromotionType, UnitCombatType, Modifier)
SELECT
	'PROMOTION_ADJACENT_BONUS', Type, 15
FROM UnitCombatInfos
WHERE IsMilitary = 1 AND IsNaval = 0 AND IsAerial = 0;

-- Battering Ram: Skeleton Key
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
	('PROMOTION_KNOCKOUT', 'UNITCOMBAT_GUN', 25);

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
UPDATE UnitPromotions SET AttackWoundedMod = 20, HPHealedIfDestroyEnemy = 30 WHERE Type = 'PROMOTION_HAKKAA_PAALLE';

-- Hakkapeliitta: Scout Cavalry
UPDATE UnitPromotions SET CombatPercent = -20, FreePillageMoves = 1, VisibilityChange = 1 WHERE Type = 'PROMOTION_SCOUT_CAVALRY';

-- Foreign Legion: Foreign Lands Bonus
UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 20 WHERE Type = 'PROMOTION_FOREIGN_LANDS';

-- Immortal: Faster Healing
UPDATE UnitPromotions SET FriendlyHealChange = 10, NeutralHealChange = 10, EnemyHealChange = 10 WHERE Type = 'PROMOTION_FASTER_HEAL';

-- Bowman, Cossack: Marksmanship
UPDATE UnitPromotions SET AttackWoundedMod = 33 WHERE Type = 'PROMOTION_STRONGER_VS_DAMAGED';

-- Cossack: Withering Fire
UPDATE UnitPromotions SET MoraleBreakChance = -1 WHERE Type = 'PROMOTION_ESPRIT_DE_CORPS';

-- Pictish Warrior, Norwegian Ski Infantry: Highlanders
UPDATE UnitPromotions SET HillsDoubleMove = 1 WHERE Type = 'PROMOTION_SKI_INFANTRY';

-- Elephants: Feared Elephant
UPDATE UnitPromotions SET NearbyEnemyCombatMod = -10, NearbyEnemyCombatRange = 1 WHERE Type = 'PROMOTION_FEARED_ELEPHANT';

-- Maori Warrior: Haka War Dance
UPDATE UnitPromotions SET NearbyEnemyCombatMod = -15, NearbyEnemyCombatRange = 1 WHERE Type = 'PROMOTION_HAKA_WAR_DANCE';

-- Khan: Khaaaan!
UPDATE UnitPromotions SET NearbyEnemyDamage = 10 WHERE Type = 'PROMOTION_MEDIC_GENERAL';

-- Samurai: Quick Study
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
UPDATE UnitPromotions SET GoldenAgeValueFromKills = 1000 WHERE Type = 'PROMOTION_MANY_GOLDEN_AGE_POINTS';

-- Minuteman, Berber Cavalry: Ignores Terrain Cost
UPDATE UnitPromotions SET IgnoreTerrainCost = 1 WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST';

-- Mehal Sefari, Berber Cavalry: Homeland Guardian
UPDATE UnitPromotions SET FriendlyLandsModifier = 25 WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN';

-- Mehal Sefari: Near Capital Bonus
UPDATE UnitPromotions SET CapitalDefenseModifier = 30, CapitalDefenseFalloff = -3 WHERE Type = 'PROMOTION_DEFEND_NEAR_CAPITAL';

-- Comanche Riders: Full Moon Striker
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

-- Mercenaries: Enhanced Flank Attack
UPDATE UnitPromotions SET FlankAttackModifier = 10 WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS';

-- Sipahi: Superior Flank Attack
UPDATE UnitPromotions SET FlankAttackModifier = 25 WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS_STRONG';

-- Sea Beggar: Prize Ships
UPDATE UnitPromotions SET CaptureDefeatedEnemy = 1 WHERE Type = 'PROMOTION_PRIZE_SHIPS';

-- War Chariot: Gift of the Pharaoh
INSERT INTO UnitPromotions_YieldFromKills
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_ENSLAVEMENT', 'YIELD_PRODUCTION', 150);

-- Mandekalu Cavalry: Raider
UPDATE UnitPromotions SET FlankAttackModifier = 20, CityAttackPlunderModifier = 50, EnemyRoute = 1 WHERE Type = 'PROMOTION_RAIDER';

-- Bandeirantes: Flag Bearers
INSERT INTO UnitPromotions_YieldFromScouting
	(PromotionType, YieldType, Yield)
VALUES
	('PROMOTION_RECON_BANDEIRANTES', 'YIELD_GOLD', 3),
	('PROMOTION_RECON_BANDEIRANTES', 'YIELD_SCIENCE', 3),
	('PROMOTION_RECON_BANDEIRANTES', 'YIELD_CULTURE', 3);

-- Longbowman: Assize of Arms
INSERT INTO UnitPromotions_UnitCombatMods
	(PromotionType, UnitCombatType, Modifier)
VALUES
	('PROMOTION_ASSIZE_OF_ARMS', 'UNITCOMBAT_MOUNTED', 20),
	('PROMOTION_ASSIZE_OF_ARMS', 'UNITCOMBAT_ARMOR', 20);

-- Horse Archer: Focus Fire
UPDATE UnitPromotions SET MultiAttackBonus = 10 WHERE Type = 'PROMOTION_FOCUS_FIRE';

-- Carolean: Grenadiers
UPDATE UnitPromotions SET AOEDamageOnKill = 15 WHERE Type = 'PROMOTION_AOE_STRIKE_ON_KILL';

-- Legion: Pilum
UPDATE UnitPromotions SET AoEWhileFortified = 10 WHERE Type = 'PROMOTION_AOE_STRIKE_FORTIFY';

-- Landsknecht: Doppelsöldner
UPDATE UnitPromotions SET AttackFullyHealedMod = 30 WHERE Type = 'PROMOTION_DOPPELSOLDNER';

-- Waraq'Ak: Concussive Hits
UPDATE UnitPromotions SET PlaguePromotion = 'PROMOTION_DAZED', PlagueChance = 100, IgnoreZOC = 1 WHERE Type = 'PROMOTION_SLINGER';

-- Chu-Ko-Nu: Siege Volleys
UPDATE UnitPromotions SET NearbyCityCombatMod = 25, NearbyRange = 1, SplashDamage = 5 WHERE Type = 'PROMOTION_REPEATER';

-- Quinquereme: Heavy Assault
UPDATE UnitPromotions SET AttackMod = 25, CityAttack = 25 WHERE Type = 'PROMOTION_HEAVY_SHIP';

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
UPDATE UnitPromotions SET OutsideFriendlyLandsModifier = 15 WHERE Type = 'PROMOTION_BUSHIDO_HONOR';
UPDATE UnitPromotions SET AttackMod = 15 WHERE Type = 'PROMOTION_COURAGE';
UPDATE UnitPromotions SET DefenseMod = 15 WHERE Type = 'PROMOTION_SELF_CONTROL';
UPDATE UnitPromotions SET AdjacentTileHealChange = 5 WHERE Type = 'PROMOTION_BENEVOLENCE';
UPDATE UnitPromotions SET ExperiencePercent = 50 WHERE Type = 'PROMOTION_SINCERITY';

--------------------------------------------
-- Plagues
--------------------------------------------
UPDATE UnitPromotions SET PlagueID = 1, PlaguePriority = 0, PromotionDuration = 1, MovesChange = -2 WHERE Type = 'PROMOTION_BOARDED_1';
UPDATE UnitPromotions SET PlagueID = 1, PlaguePriority = 1, PromotionDuration = 1, MovesChange = -4 WHERE Type = 'PROMOTION_BOARDED_2';

UPDATE UnitPromotions SET PlagueID = 2, PromotionDuration = 5, CombatPercent = -15 WHERE Type = 'PROMOTION_DAZED';

UPDATE UnitPromotions SET PlagueID = 3, PromotionDuration = 50, WorkRateMod = -50 WHERE Type = 'PROMOTION_PRISONER_WAR';

--------------------------------------------
-- Event free promotions
--------------------------------------------
UPDATE UnitPromotions SET CombatPercent = 10 WHERE Type = 'PROMOTION_MORALE_EVENT';

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
