-- rev.05


-- Temporary Atlas
/*
-- 1st row
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 0  WHERE Type = ''; -- White Triangle + 1 Yellow Bar
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 1  WHERE Type = ''; -- White Triangle + 2 Yellow Bars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 2  WHERE Type = ''; -- White Triangle + 3 Yellow Bars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 3  WHERE Type = ''; -- White Triangle + 3 Yellow Stars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 4  WHERE Type = ''; -- 2 White Triangles
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 5  WHERE Type = ''; -- Yellow Triangle + 3 White Bars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 6  WHERE Type = ''; -- Yellow Triangle + 1 Yellow Horns
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 7  WHERE Type = ''; -- Yellow Triangle like a inclined star
-- 2nd row
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 8  WHERE Type = ''; -- Yellow Triangle + 2 White Stars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 9  WHERE Type = ''; -- Yellow Triangle + 3 White Stars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 10 WHERE Type = ''; -- Yellow Triangle + 4 White Stars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 11 WHERE Type = ''; -- Yellow Triangle + 5 White Stars
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 12 WHERE Type = ''; -- 2 Yellow Triangles
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 13 WHERE Type = ''; -- 2 Red Triangles
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 14 WHERE Type = ''; -- 3 Red Triangles
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas', PortraitIndex = 15 WHERE Type = ''; -- Red Triangle + Yellow Triangle
*/

-----------------------------------------------------------------------------------------------------------------

-- atlas_00
-- 1st row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 0  WHERE Type = 'PROMOTION_ANTI_FIGHTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 1  WHERE Type = 'PROMOTION_KNOCKOUT_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 2  WHERE Type = 'PROMOTION_KNOCKOUT_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 3  WHERE Type = 'PROMOTION_ANTI_SUBMARINE_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 4  WHERE Type = 'PROMOTION_ANTI_SUBMARINE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 5  WHERE Type = 'PROMOTION_ANTI_TANK';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 6  WHERE Type = 'PROMOTION_NAVAL_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 7  WHERE Type = 'PROMOTION_ANTI_HELICOPTER';
-- 2nd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 8  WHERE Type = 'PROMOTION_AMBUSH_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 9  WHERE Type = 'PROMOTION_AMBUSH_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 10 WHERE Type = 'PROMOTION_HELI_AMBUSH_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 11 WHERE Type = 'PROMOTION_HELI_AMBUSH_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 12 WHERE Type = 'PROMOTION_OPEN_TERRAIN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 13 WHERE Type = 'PROMOTION_ANTI_MOUNTED_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 14 WHERE Type = 'PROMOTION_ANTI_MOUNTED_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 15 WHERE Type = 'PROMOTION_MOUNTED_PENALTY';
-- 3rd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 16 WHERE Type = 'PROMOTION_STATUE_ZEUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 17 WHERE Type = 'PROMOTION_FIRE_SUPPORT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 18 WHERE Type = 'PROMOTION_HEAVY_PAYLOAD';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 19 WHERE Type IN ('PROMOTION_CITY_SIEGE', 'PROMOTION_CITY_SIEGE_II');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 20 WHERE Type = 'PROMOTION_CITY_ASSAULT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 21 WHERE Type = 'PROMOTION_DOUBLE_PLUNDER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 22 WHERE Type = 'PROMOTION_SMALL_CITY_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 23 WHERE Type = 'PROMOTION_CITY_PENALTY';
-- 4th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 24 WHERE Type = 'PROMOTION_BIG_CITY_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 25 WHERE Type = 'PROMOTION_NO_CAPTURE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 26 WHERE Type = 'PROMOTION_ONLY_ATTACKS_CITIES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 27 WHERE Type = 'PROMOTION_PRISONER_WAR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 28 WHERE Type = 'PROMOTION_BARBARIAN_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 29 WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 30 WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 31 WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_I';
-- 5th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 32 WHERE Type = 'PROMOTION_SHOCK_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 33 WHERE Type = 'PROMOTION_BARRAGE_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 34 WHERE Type = 'PROMOTION_SKIRMISHER_POWER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 35 WHERE Type = 'PROMOTION_BOMBARDMENT_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 36 WHERE Type = 'PROMOTION_BOARDING_PARTY_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 37 WHERE Type = 'PROMOTION_COASTAL_RAIDER_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 38 WHERE Type = 'PROMOTION_MORALE_EVENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 39 WHERE Type = 'PROMOTION_MORALE';
-- 6th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 40 WHERE Type = 'PROMOTION_GREAT_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 41 WHERE Type = 'PROMOTION_GREAT_ADMIRAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 42 WHERE Type = 'PROMOTION_GENERAL_STACKING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 43 WHERE Type = 'PROMOTION_BETTER_LEADERSHIP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 44 WHERE Type = 'PROMOTION_MOVEMENT_TO_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 45 WHERE Type = 'PROMOTION_SPAWN_GENERALS_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 46 WHERE Type = 'PROMOTION_SPAWN_GENERALS_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 47 WHERE Type = 'PROMOTION_TREASURE_FLEET';
-- 7th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 48 WHERE Type = 'PROMOTION_CHARGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 49 WHERE Type = 'PROMOTION_CHARGE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 50 WHERE Type = 'PROMOTION_STRONGER_VS_DAMAGED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 51 WHERE Type = 'PROMOTION_ATLATL_ATTACK';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 52 WHERE Type = 'PROMOTION_HONOR_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 53 WHERE Type = 'PROMOTION_ATTACK_BONUS_SWEDEN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 54 WHERE Type = 'PROMOTION_ADJACENT_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 55 WHERE Type = 'PROMOTION_ATTACK_BONUS';
-- 8th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 56 WHERE Type = 'PROMOTION_TERROR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 57 WHERE Type = 'PROMOTION_TERROR_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 58 WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 59 WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 60 WHERE Type = 'PROMOTION_SPLASH';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 61 WHERE Type = 'PROMOTION_SPLASH_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 62 WHERE Type = 'PROMOTION_AOE_STRIKE_ON_KILL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00', PortraitIndex = 63 WHERE Type = 'PROMOTION_AOE_STRIKE_FORTIFY';

-- atlas_01
-- 1st row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 0  WHERE Type = 'PROMOTION_ANTIAIR_LAND_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 1  WHERE Type = 'PROMOTION_ANTIAIR_LAND_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 2  WHERE Type = 'PROMOTION_ANTIAIR_LAND_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 3  WHERE Type = 'PROMOTION_FALLOUT_REDUCTION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 4  WHERE Type IN ('PROMOTION_FALLOUT_IMMUNITY', 'PROMOTION_NUCLEAR_SILO');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 5  WHERE Type = 'PROMOTION_SKI_INFANTRY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 6  WHERE Type = 'PROMOTION_DEFEND_NEAR_CAPITAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 7  WHERE Type = 'PROMOTION_HIMEJI_CASTLE';
-- 2nd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 8  WHERE Type = 'PROMOTION_RECON_BANDEIRANTES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 9  WHERE Type = 'PROMOTION_TARGETING_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 10 WHERE Type = 'PROMOTION_NAVAL_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 11 WHERE Type IN ('PROMOTION_OCEAN_IMPASSABLE', 'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 12 WHERE Type = 'PROMOTION_OCEAN_CROSSING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 13 WHERE Type = 'PROMOTION_HOVERING_UNIT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 14 WHERE Type = 'PROMOTION_DEEPWATER_EMBARKATION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 15 WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION';
-- 3rd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 16 WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 17 WHERE Type = 'PROMOTION_HOVER_UNIT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 18 WHERE Type = 'PROMOTION_EMBARKED_SIGHT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 19 WHERE Type = 'PROMOTION_WAR_CANOES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 20 WHERE Type IN ('PROMOTION_SIGHT_PENALTY', 'PROMOTION_SIGHT_PENALTY_NAVAL');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 21 WHERE Type = 'PROMOTION_EXTRA_SIGHT_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 22 WHERE Type = 'PROMOTION_SEE_INVISIBLE_SUBMARINE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 23 WHERE Type = 'PROMOTION_INVISIBLE_SUBMARINE';
-- 4th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 24 WHERE Type IN ('PROMOTION_NAVAL_SENTRY_II', 'PROMOTION_GREAT_LIGHTHOUSE');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 25 WHERE Type = 'PROMOTION_NAVAL_TRADITION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 26 WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 27 WHERE Type = 'PROMOTION_ARSENALE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 28 WHERE Type = 'PROMOTION_FLAT_MOVEMENT_COST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 29 WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 30 WHERE Type IN ('PROMOTION_FAST_ADMIRAL', 'PROMOTION_SACRED_STEPS');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 31 WHERE Type = 'PROMOTION_FAST_GENERAL';
-- 5th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 32 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_GUN';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 33 WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 34 WHERE Type = 'PROMOTION_STEAM_POWERED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 35 WHERE Type IN ('PROMOTION_CAN_MOVE_AFTER_ATTACKING', 'PROMOTION_CAN_MOVE_AFTER_ATTACKING_NAVAL');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 36 WHERE Type = 'PROMOTION_ONLY_DEFENSIVE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 37 WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 38 WHERE Type = 'PROMOTION_FOLIAGE_IMPASSABLE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 39 WHERE Type = 'PROMOTION_ROUGH_TERRAIN_ENDS_TURN';
-- 6th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 40 WHERE Type IN ('PROMOTION_JAGUAR', 'PROMOTION_MOHAWK');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 41 WHERE Type = 'PROMOTION_GUERILLA_FIGHTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 42 WHERE Type = 'PROMOTION_COERCION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 43 WHERE Type = 'PROMOTION_GAIN_EXPERIENCE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 44 WHERE Type = 'PROMOTION_MEDIC';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 45 WHERE Type = 'PROMOTION_FASTER_HEAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 46 WHERE Type = 'PROMOTION_MEDIC_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 47 WHERE Type = 'PROMOTION_EVERLASTING_YOUTH';
-- 7th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 48 WHERE Type = 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 49 WHERE Type = 'PROMOTION_HEAL_IF_DESTROY_ENEMY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 50 WHERE Type = 'PROMOTION_KILL_HEAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 52 WHERE Type = 'PROMOTION_PRIZE_RULES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 52 WHERE Type = 'PROMOTION_ENSLAVEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 53 WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 54 WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS_STRONG';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 55 WHERE Type = 'PROMOTION_RAIDER';
-- 8th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 56 WHERE Type = 'PROMOTION_INDIRECT_FIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 57 WHERE Type = 'PROMOTION_RANGED_SUPPORT_FIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 58 WHERE Type = 'PROMOTION_NAVAL_MISFIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 59 WHERE Type = 'PROMOTION_NAVAL_INACCURACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 60 WHERE Type = 'PROMOTION_SIEGE_INACCURACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 61 WHERE Type = 'PROMOTION_WEAK_RANGED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 62 WHERE Type = 'PROMOTION_BETTER_BOMBARDMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01', PortraitIndex = 63 WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES';

-- atlas_02
-- 1st row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 0  WHERE Type = 'PROMOTION_AIR_TARGETING_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 1  WHERE Type = 'PROMOTION_AIR_TARGETING_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 2  WHERE Type = 'PROMOTION_AIR_TARGETING_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 3  WHERE Type = 'PROMOTION_AIR_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 4  WHERE Type = 'PROMOTION_INTERCEPTION_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 5  WHERE Type = 'PROMOTION_INTERCEPTION_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 6  WHERE Type = 'PROMOTION_INTERCEPTION_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 7  WHERE Type = 'PROMOTION_INTERCEPTION_IV';
-- 2nd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 8  WHERE Type = 'PROMOTION_EVASION_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 9  WHERE Type = 'PROMOTION_EVASION_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 10 WHERE Type = 'PROMOTION_EVASION_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 11 WHERE Type = 'PROMOTION_ANTI_AIR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 12 WHERE Type = 'PROMOTION_ANTI_AIR_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 13 WHERE Type = 'PROMOTION_AIR_LOGISTICS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 14 WHERE Type = 'PROMOTION_AIR_SWEEP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 15 WHERE Type = 'PROMOTION_EVASION';
-- 3rd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 16 WHERE Type = 'PROMOTION_NATIONALISM';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 17 WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN_BOOGALOO';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 18 WHERE Type = 'PROMOTION_ACCURACY_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 19 WHERE Type = 'PROMOTION_FOREIGN_LANDS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 20 WHERE Type = 'PROMOTION_RIVAL_TERRITORY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 21 WHERE Type = 'PROMOTION_MUST_SET_UP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 22 WHERE Type = 'PROMOTION_SCOUT_XP_PILLAGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 23 WHERE Type = 'PROMOTION_SCOUT_XP_SPOTTING';
-- 4th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 24 WHERE Type = 'PROMOTION_NO_DEFENSIVE_BONUSES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 25 WHERE Type IN ('PROMOTION_CAN_MOVE_IMPASSABLE', 'PROMOTION_ICE_BREAKERS');
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 26 WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 27 WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 28 WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 29 WHERE Type = 'PROMOTION_PARADROP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 30 WHERE Type = 'PROMOTION_PRIZE_SHIPS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 31 WHERE Type = 'PROMOTION_HAKA_WAR_DANCE';
-- 5th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 32 WHERE Type = 'PROMOTION_EMISSARY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 33 WHERE Type = 'PROMOTION_ENVOY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 34 WHERE Type = 'PROMOTION_DIPLOMAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 35 WHERE Type = 'PROMOTION_AMBASSADOR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 36 WHERE Type = 'PROMOTION_GREAT_DIPLOMAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 37 WHERE Type = 'PROMOTION_NOBEL_LAUREATE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 38 WHERE Type = 'PROMOTION_PAX';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 39 WHERE Type = 'PROMOTION_SIGNET';
-- 6th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 40 WHERE Type = 'PROMOTION_LITERACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 41 WHERE Type = 'PROMOTION_EXPRESS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 42 WHERE Type = 'PROMOTION_IMMUNITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 43 WHERE Type = 'PROMOTION_UNWELCOME_EVANGELIST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 44 WHERE Type = 'PROMOTION_CARGO_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 45 WHERE Type = 'PROMOTION_CARGO_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 46 WHERE Type = 'PROMOTION_CARGO_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 47 WHERE Type = 'PROMOTION_CARGO_IV';
-- 7th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 48 WHERE Type = 'PROMOTION_VIKING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 49 WHERE Type = 'PROMOTION_LONGBOAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 50 WHERE Type = 'PROMOTION_SILENT_HUNTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 51 WHERE Type = 'PROMOTION_FEARED_ELEPHANT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 52 WHERE Type = 'PROMOTION_NAVAL_DEFENSE_BOOST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 53 WHERE Type = 'PROMOTION_SELL_EXOTIC_GOODS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 54 WHERE Type = 'PROMOTION_GREAT_MERCHANT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 55 WHERE Type = 'PROMOTION_TRADE_MISSION_BONUS';
-- 8th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 56 WHERE Type = 'PROMOTION_AIR_RECON';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 57 WHERE Type = 'PROMOTION_RECON_LONG_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 58 WHERE Type = 'PROMOTION_RECON_SHORT_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 59 WHERE Type = 'PROMOTION_AIR_MISFIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 60 WHERE Type = 'PROMOTION_NAVAL_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 61 WHERE Type = 'PROMOTION_HEAVY_SHIP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 62 WHERE Type = 'PROMOTION_SURVIVALISM_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02', PortraitIndex = 63 WHERE Type = 'PROMOTION_LOGISTICS';

-- atlas_03
-- 1st row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 0  WHERE Type = 'PROMOTION_FIELD_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 1  WHERE Type = 'PROMOTION_FIELD_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 2  WHERE Type = 'PROMOTION_FIELD_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 3  WHERE Type = 'PROMOTION_SIEGE_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 4  WHERE Type = 'PROMOTION_SIEGE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 5  WHERE Type = 'PROMOTION_SIEGE_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 6  WHERE Type = 'PROMOTION_WOODS_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 7  WHERE Type = 'PROMOTION_DESERT_WARRIOR';
-- 2nd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 8  WHERE Type = 'PROMOTION_BUSHIDO';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 9  WHERE Type = 'PROMOTION_EVIL_SPIRITS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 10 WHERE Type = 'PROMOTION_ENEMY_BLADE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 11 WHERE Type = 'PROMOTION_HEROISM';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 12 WHERE Type = 'PROMOTION_HILL_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 13 WHERE Type = 'PROMOTION_MARSH_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 14 WHERE Type = 'PROMOTION_WHITE_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 15 WHERE Type = 'PROMOTION_DESERT_WALKER';
-- 3rd row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 16 WHERE Type = 'PROMOTION_BUSHIDO_HONOR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 17 WHERE Type = 'PROMOTION_LOYALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 18 WHERE Type = 'PROMOTION_SELF_CONTROL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 19 WHERE Type = 'PROMOTION_COURAGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 20 WHERE Type = 'PROMOTION_RESPECT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 21 WHERE Type = 'PROMOTION_RIGHTEOUSNESS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 22 WHERE Type = 'PROMOTION_SINCERITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 23 WHERE Type = 'PROMOTION_BENEVOLENCE';
-- 4th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 24 WHERE Type = 'PROMOTION_SUPPLY_BOOST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 25 WHERE Type = 'PROMOTION_CONSCRIPT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 26 WHERE Type = 'PROMOTION_BUFFALO_CHEST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 27 WHERE Type = 'PROMOTION_BUFFALO_HORNS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 28 WHERE Type = 'PROMOTION_BUFFALO_LOINS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 29 WHERE Type = 'PROMOTION_LIGHTNING_WARFARE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 30 WHERE Type = 'PROMOTION_SKIRMISHER_MOBILITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 31 WHERE Type = 'PROMOTION_DAMAGE_REDUCTION';
-- 5th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 32 WHERE Type = 'PROMOTION_COASTAL_RAIDER_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 33 WHERE Type = 'PROMOTION_COASTAL_RAIDER_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 34 WHERE Type = 'PROMOTION_COASTAL_RAIDER_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 35 WHERE Type = 'PROMOTION_BREACHER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 36 WHERE Type = 'PROMOTION_ENCIRCLEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 37 WHERE Type = 'PROMOTION_NAVAL_SIEGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 38 WHERE Type = 'PROMOTION_COASTAL_TERROR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 39 WHERE Type = 'PROMOTION_PIRACY';
-- 6th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 40 WHERE Type = 'PROMOTION_PRESS_GANGS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 41 WHERE Type = 'PROMOTION_MINELAYER';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 42 WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 43 WHERE Type = 'PROMOTION_BOARDED_I';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 44 WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 45 WHERE Type = 'PROMOTION_BOARDED_II';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 46 WHERE Type = '';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 47 WHERE Type = '';
-- 7th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 48 WHERE Type = 'PROMOTION_FIELD_WORKS_0';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 49 WHERE Type = 'PROMOTION_FIELD_WORKS_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 50 WHERE Type = 'PROMOTION_FIELD_WORKS_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 51 WHERE Type = 'PROMOTION_FIELD_WORKS_3';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 52 WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 53 WHERE Type = 'PROMOTION_NOBILITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 54 WHERE Type = 'PROMOTION_SLINGER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 55 WHERE Type = 'PROMOTION_DAZED';
-- 8th row
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 56 WHERE Type = 'PROMOTION_SIEGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 57 WHERE Type = 'PROMOTION_ALHAMBRA';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 58 WHERE Type = 'PROMOTION_REPEATER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 59 WHERE Type = 'PROMOTION_MECH';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 60 WHERE Type = 'PROMOTION_IKLWA';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 61 WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 62 WHERE Type = 'PROMOTION_KNOCKOUT_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03', PortraitIndex = 63 WHERE Type = 'PROMOTION_ESPRIT_DE_CORPS';

-- atlas_04
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 0  WHERE Type = 'PROMOTION_ROUGH_TERRAIN_HALF_TURN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 1  WHERE Type = 'PROMOTION_OCEAN_HALF_MOVES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 2  WHERE Type = 'PROMOTION_ASSIZE_OF_ARMS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 3  WHERE Type = 'PROMOTION_HAKKAA_PAALLE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 4  WHERE Type = 'PROMOTION_FROGMAN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_04', PortraitIndex = 5  WHERE Type = 'PROMOTION_SCREENING';

-- extraPromo_atlas
UPDATE UnitPromotions SET IconAtlas = 'extraPromo_Atlas', PortraitIndex = 18 WHERE Type = 'PROMOTION_GOD_WAR';
UPDATE UnitPromotions SET IconAtlas = 'extraPromo_Atlas', PortraitIndex = 21 WHERE Type = 'PROMOTION_SKIRMISHER_DOCTRINE';
UPDATE UnitPromotions SET IconAtlas = 'extraPromo_Atlas', PortraitIndex = 22 WHERE Type = 'PROMOTION_SCOUT_CAVALRY';
UPDATE UnitPromotions SET IconAtlas = 'extraPromo_Atlas', PortraitIndex = 23 WHERE Type = 'PROMOTION_ENVELOPMENT';
UPDATE UnitPromotions SET IconAtlas = 'extraPromo_Atlas', PortraitIndex = 56 WHERE Type = 'PROMOTION_DOPPELSOLDNER';

--
-- Firaxis Icons reassignment
--

-- Set "Logistics" Icon
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 30 WHERE Type = 'PROMOTION_SECOND_ATTACK';

-- Set "+1 Movement" Icon
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 33 WHERE Type = 'PROMOTION_MOON_STRIKER';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 33 WHERE Type = 'PROMOTION_FASTER_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 33 WHERE Type = 'PROMOTION_EXTRA_MOVES_I';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 33 WHERE Type = 'PROMOTION_SCOUTING_3';

-- Set Firaxis leaf icon to Jaguar/Mohawk
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 35 WHERE Type = 'PROMOTION_JAGUAR';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 35 WHERE Type = 'PROMOTION_MOHAWK';

-- Set "+1 Visibility Range" Icon
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 37 WHERE Type = 'PROMOTION_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 37 WHERE Type = 'PROMOTION_SKIRMISHER_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 37 WHERE Type = 'PROMOTION_EXTRA_SIGHT_I';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 37 WHERE Type = 'PROMOTION_IMPERIALISM_OPENER';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 37 WHERE Type = 'PROMOTION_SCOUTING_2';

-- Set "+3 Visibility Range" Icon
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 38 WHERE Type = 'PROMOTION_EXTRA_SIGHT_III';

-- Set "+4 Visibility Range" Icon
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS', PortraitIndex = 39 WHERE Type = 'PROMOTION_EXTRA_SIGHT_IV';

-- Set "Penalty on Defense" Icon
UPDATE UnitPromotions SET IconAtlas = 'KRIS_SWORDSMAN_PROMOTION_ATLAS', PortraitIndex = 2 WHERE Type = 'PROMOTION_DEFENSE_PENALTY';
