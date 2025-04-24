---------------------------------------------------------------------------------------------------------------
-- Unit Panel promotions
---------------------------------------------------------------------------------------------------------------

-- Default
UPDATE UnitPromotions SET OrderPriority = 9999, ShowInUnitPanel = 1;

CREATE TEMP TABLE PromotionClass_Orders (
	PromotionClass TEXT,
	OrderNumber INTEGER
);

INSERT INTO PromotionClass_Orders
VALUES
	-- Plagues (should be always shown)
	('BOARDED', 1),
	-- Trunk promotions
	('SHOCK', 599),
	('DRILL', 589),
	('INTERCEPTOR', 104), -- reserve slot for Buffalo
	('BARRAGE', 599),
	('ACCURACY', 589),
	('TANK_HUNTER', 103),
	('HELI_MOBILITY', 104),
	('SIEGE', 589),
	('FIELD', 599),
	('TRAILBLAZER', 599),
	('SURVIVALISM', 589),
	('HULL', 599),
	('BOARDING_PARTY', 589),
	('TARGETING', 589),
	('BOMBARDMENT', 599),
	('TORPEDO', 599),
	('FLIGHT_DECK', 599),
	('ARMOR_PLATING', 589), -- also given to other units
	('ACE_PILOT', 599),
	('DOGFIGHTING', 589),
	('AIR_SUPREMACY', 579),
	('AIR_TARGETING', 589),
	('AIR_SIEGE', 599),
	('AIR_AMBUSH', 578),
	-- Unique unit free promotions
	('BUFFALO', 600),
	('BUSHIDO', 111),
	('MYSTIC_BLADE', 112), -- technically a unit can have both Bushido and Mystic Blade
	-- Building free promotions
	('DIPLO_BOOST', 201),
	-- Leaf promotions
	('COVER', 579),
	('AMBUSH', 578),
	('FORMATION', 577),
	('MEDIC', 576),
	('AIR_DEFENSE', 571),
	('MARCH', 598),
	('CHARGE', 304),
	('SENTRY', 561),
	('SPLASH', 581),
	('SCOUTING', 301),
	('NAVIGATOR', 578),
	('AIR_PENETRATION', 579),
	-- Unit free promotions (stays on upgrade)
	('GENERAL_MOVEMENT', 401),
	('SIEGE_ENGINE', 402),
	('WITHDRAW', 403),
	('GENERAL_RATE', 404),
	-- Policy promotions
	('AUTHORITY', 603),
	('IMPERIALISM', 608),
	-- Tech promotions
	('EMBARKATION', 701),
	('FALLOUT_RESISTANCE', 703),
	-- Unit free promotions (lost on upgrade)
	('FIELD_WORKS', 801),
	('COVERING_FIRE', 801),
	('PARADROP', 801),
	('DEPTH_CHARGES', 801),
	('AIR_RECON', 801),
	('SUBMARINE_BASE', 801),
	('DIPLO_BASE', 801),
	('CARGO', 802);

UPDATE UnitPromotions
SET OrderPriority = (SELECT OrderNumber FROM PromotionClass_Orders WHERE PromotionClass = RankList)
WHERE EXISTS (SELECT 1 FROM PromotionClass_Orders WHERE PromotionClass = RankList);

DROP TABLE PromotionClass_Orders;

CREATE TEMP TABLE UnitPromotion_Orders (
	PromotionType TEXT,
	OrderNumber INTEGER
);

INSERT INTO UnitPromotion_Orders
VALUES
	('PROMOTION_ANTIAIR_LAND_1', 571),
	('PROMOTION_ANTIAIR_LAND_2', 571),
	('PROMOTION_ANTIAIR_LAND_3', 571),

	-- Plagues
	('PROMOTION_DAZED', 2),
	('PROMOTION_PRISONER_WAR', 3),
	-- Really important promotions
	('PROMOTION_JUGGERNAUT', 10),
	-- Unique unit free promotions (stays on upgrade)
	('PROMOTION_ADJACENT_BONUS', 113),
	('PROMOTION_IGNORE_TERRAIN_COST', 114),
	('PROMOTION_SPAWN_GENERALS_I', 115),
	('PROMOTION_SPAWN_GENERALS_II', 116),
	('PROMOTION_KNOCKOUT', 118),
	('PROMOTION_OPEN_TERRAIN', 119),
	('PROMOTION_MOVEMENT_TO_GENERAL', 120),
	('PROMOTION_GENERAL_STACKING', 121),
	('PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY', 122),
	('PROMOTION_HAKKAA_PAALLE', 123),
	('PROMOTION_SCOUT_CAVALRY', 124),
	('PROMOTION_FOREIGN_LANDS', 125),
	('PROMOTION_FASTER_HEAL', 126),
	('PROMOTION_STRONGER_VS_DAMAGED', 127),
	('PROMOTION_ESPRIT_DE_CORPS', 128),
	('PROMOTION_SKI_INFANTRY', 129),
	('PROMOTION_HAKA_WAR_DANCE', 131),
	('PROMOTION_MEDIC_GENERAL', 132),
	('PROMOTION_GAIN_EXPERIENCE', 133),
	('PROMOTION_TRADE_MISSION_BONUS', 134),
	('PROMOTION_HEAVY_CHARGE', 135),
	('PROMOTION_SAPPER', 136),
	('PROMOTION_GOLDEN_AGE_POINTS', 137),
	('PROMOTION_MANY_GOLDEN_AGE_POINTS', 138),
	('PROMOTION_HOMELAND_GUARDIAN', 139),
	('PROMOTION_DEFEND_NEAR_CAPITAL', 140),
	('PROMOTION_MOON_STRIKER', 141),
	('PROMOTION_LIGHTNING_WARFARE', 142),
	('PROMOTION_DOUBLE_PLUNDER', 143),
	('PROMOTION_FLANK_ATTACK_BONUS', 144),
	('PROMOTION_FLANK_ATTACK_BONUS_STRONG', 145),
	('PROMOTION_PRIZE_SHIPS', 146),
	('PROMOTION_ENSLAVEMENT', 147),
	('PROMOTION_RAIDER', 148),
	('PROMOTION_RECON_BANDEIRANTES', 149),
	('PROMOTION_ASSIZE_OF_ARMS', 150),
	('PROMOTION_FOCUS_FIRE', 151),
	('PROMOTION_AOE_STRIKE_ON_KILL', 152),
	('PROMOTION_AOE_STRIKE_FORTIFY', 153),
	('PROMOTION_DOPPELSOLDNER', 154),
	('PROMOTION_SLINGER', 155),
	('PROMOTION_REPEATER', 156),
	('PROMOTION_HEAVY_SHIP', 157),
	('PROMOTION_MOHAWK', 158),
	('PROMOTION_JAGUAR', 158),
	('PROMOTION_MYSTIC_BLADE', 159),
	-- Building free promotions
	('PROMOTION_MORALE', 201),
	('PROMOTION_BUSHIDO', 202),
	('PROMOTION_HIMEJI_CASTLE', 203),
	('PROMOTION_STATUE_ZEUS', 204),
	('PROMOTION_ALHAMBRA', 205),
	('PROMOTION_TREASURE_FLEET', 206),
	('PROMOTION_ARSENALE', 207),
	-- Leaf promotions, land and sea 	
	('PROMOTION_BLITZ', 587),	
	('PROMOTION_LOGISTICS', 591),
	('PROMOTION_MOBILITY', 582),	
	('PROMOTION_ACCURACY_4', 560), -- Infiltrators			
	-- Leaf promotions, land 
	('PROMOTION_SHOCK_4', 597), -- Overrun
	('PROMOTION_DRILL_4', 588),	-- Stalwart
	('PROMOTION_SIEGE', 569), -- Forlorn Hope
	('PROMOTION_AMPHIBIOUS', 567),
	('PROMOTION_WOODSMAN', 568),	
	('PROMOTION_RANGE', 598),
	('PROMOTION_BARRAGE_4', 590), -- Firing Doctrine
	('PROMOTION_VOLLEY', 569),
	('PROMOTION_COASTAL_ARTILLERY', 568),
	('PROMOTION_SKIRMISHER_MOBILITY', 597), -- Parthian Tactics
	('PROMOTION_SKIRMISHER_POWER', 585), -- Coup de Grace
	('PROMOTION_ENVELOPMENT', 569),	
	('PROMOTION_FROGMAN', 311),
	('PROMOTION_SCREENING', 312),	
	-- Leaf promotions, sea		
	('PROMOTION_DAMAGE_REDUCTION', 598), -- Dauntless
	('PROMOTION_COASTAL_TERROR', 597), -- Vanguard, Conquistador gets this
	('PROMOTION_COASTAL_RAIDER_4', 596), -- Blockade
	('PROMOTION_BOARDING_PARTY_4', 588), -- Pincer
	('PROMOTION_SUPPLY', 586),				
	('PROMOTION_ENCIRCLEMENT', 569),	
	('PROMOTION_NAVAL_SIEGE', 568),		
	('PROMOTION_BREACHER', 567),	
	('PROMOTION_PIRACY', 566),	
	('PROMOTION_SEE_INVISIBLE_SUBMARINE', 565),	
	('PROMOTION_PRESS_GANGS', 564), -- Commerce Raider
	('PROMOTION_MINELAYER', 563),	
	('PROMOTION_BOMBARDMENT_4', 598), -- Broadside	
	('PROMOTION_BETTER_BOMBARDMENT', 597), -- Shrapnel Rounds
	('PROMOTION_TARGETING_4', 589),	-- Indomitable
	('PROMOTION_PERISCOPE_DEPTH', 597),
	('PROMOTION_PREDATOR', 596),	
	('PROMOTION_HUNTER_KILLER', 569),
	('PROMOTION_TRUE_WOLFPACK', 568), -- Wolfpack
	-- Leaf promotions, air	
	('PROMOTION_SORTIE', 598),
	('PROMOTION_AERIAL_TORPEDO', 597),	
	('PROMOTION_AIR_LOGISTICS', 595),	
	('PROMOTION_AIR_RANGE', 587),
	('PROMOTION_BOMB_RACK', 586),
	('PROMOTION_ANTI_FIGHTER', 569), -- Eagle Hunter
	('PROMOTION_EVASION', 569),	
	-- Unit free promotions (stays on upgrade)
	('PROMOTION_CAN_MOVE_AFTER_ATTACKING', 411),
	('PROMOTION_RECON_EXPERIENCE', 413),
	('PROMOTION_OCEAN_CROSSING', 414),
	('PROMOTION_EMBARKED_SIGHT', 415),
	('PROMOTION_SCOUT_XP_PILLAGE', 416),
	('PROMOTION_ATTACK_BONUS', 417),
	('PROMOTION_FREE_PILLAGE_MOVES', 418),
	('PROMOTION_WITHDRAW_BEFORE_MELEE', 419),
	('PROMOTION_SILENT_HUNTER', 420),
	('PROMOTION_CAN_MOVE_IMPASSABLE', 421),
	('PROMOTION_RIVAL_TERRITORY', 422),
	-- Event/Natural Wonder promotions
	('PROMOTION_MORALE_EVENT', 496),
	('PROMOTION_ALTITUDE_TRAINING', 497),
	('PROMOTION_SACRED_STEPS', 498),
	('PROMOTION_EVERLASTING_YOUTH', 499),
	-- Barbarian promotions
	('PROMOTION_HILL_WALKER', 501),
	('PROMOTION_DESERT_WALKER', 502),
	('PROMOTION_WOODS_WALKER', 503),
	('PROMOTION_WHITE_WALKER', 504),
	('PROMOTION_MARSH_WALKER', 505),
	-- Policy promotions
	('PROMOTION_FASTER_WORKER', 602),
	('PROMOTION_NATIONALISM', 611),
	('PROMOTION_FAST_GENERAL', 612),
	('PROMOTION_LIGHTNING_WARFARE_GUN', 612),
	('PROMOTION_LIGHTNING_WARFARE_ARMOR', 612),
	('PROMOTION_MARE_NOSTRUM', 612),
	-- Tech promotions
	('PROMOTION_ICE_BREAKERS', 702),
	-- Trait promotions
	('PROMOTION_MONGOL_TERROR', 794),
	('PROMOTION_VIKING', 795),
	('PROMOTION_LONGBOAT', 795),
	('PROMOTION_COERCION', 796),
	('PROMOTION_ATTACK_BONUS_SWEDEN', 797),
	('PROMOTION_WAR_CANOES', 798),
	('PROMOTION_GOODY_HUT_PICKER', 799),
	-- Unit free promotions (lost on upgrade)
	('PROMOTION_CITY_ASSAULT', 803),
	('PROMOTION_RANGED_SUPPORT_FIRE', 804),
	('PROMOTION_FEARED_ELEPHANT', 805),
	('PROMOTION_BARBARIAN_BONUS', 806),
	('PROMOTION_SKIRMISHER_DOCTRINE', 807),
	('PROMOTION_HOVERING_UNIT', 808),
	('PROMOTION_FLAT_MOVEMENT_COST', 809),
	('PROMOTION_INDIRECT_FIRE', 810),
	('PROMOTION_COMMANDO', 811),
	('PROMOTION_INVISIBLE_SUBMARINE', 812),
	('PROMOTION_AIR_SWEEP', 813),
	('PROMOTION_AIR_MISFIRE', 814),
	('PROMOTION_ANTI_HELICOPTER', 815),
	('PROMOTION_STEALTH', 816),
	('PROMOTION_GREAT_GENERAL', 817),
	('PROMOTION_GREAT_ADMIRAL', 817),
	('PROMOTION_GREAT_DIPLOMAT', 817),
	('PROMOTION_SUPPLY_BOOST', 818),
	('PROMOTION_NUCLEAR_SILO', 819),
	('PROMOTION_MECH', 820),
	('PROMOTION_SELL_EXOTIC_GOODS', 821),
	('PROMOTION_CONSCRIPT', 822),
	('PROMOTION_ANTI_MOUNTED_I', 823),
	-- Penalties
	('PROMOTION_ONLY_ATTACKS_CITIES', 901),
	('PROMOTION_UNWELCOME_EVANGELIST', 901),
	('PROMOTION_CITY_PENALTY', 902),
	('PROMOTION_SMALL_CITY_PENALTY', 902),
	('PROMOTION_BIG_CITY_PENALTY', 902),
	('PROMOTION_OCEAN_HALF_MOVES', 903),
	('PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY', 903),
	('PROMOTION_MUST_SET_UP', 904),
	('PROMOTION_ROUGH_TERRAIN_HALF_TURN', 905),
	('PROMOTION_NAVAL_MISFIRE', 906),
	('PROMOTION_NAVAL_INACCURACY', 906),
	('PROMOTION_NO_DEFENSIVE_BONUSES', 998);

UPDATE UnitPromotions
SET OrderPriority = (SELECT OrderNumber FROM UnitPromotion_Orders WHERE PromotionType = Type)
WHERE EXISTS (SELECT 1 FROM UnitPromotion_Orders WHERE PromotionType = Type);

DROP TABLE UnitPromotion_Orders;

-- don't show these on the unit panel
UPDATE UnitPromotions SET ShowInUnitPanel = 0 WHERE Type IN (
	'PROMOTION_ONLY_DEFENSIVE'
);

---------------------------------------------------------------------------------------------------------------
-- Unit Flag promotions
-- Promotions with the same RankList will be grouped together
---------------------------------------------------------------------------------------------------------------
UPDATE UnitPromotions SET FlagPromoOrder = OrderPriority;

-- There shouldn't be any here, but just in case, put these promotions at the end.
UPDATE UnitPromotions SET FlagPromoOrder = 999 WHERE OrderPriority = 0;

---------------------------------------------------------------------------------------------------------------
-- Promotions which should not be displayed above the Unit Flag
-- Trait- and unit-specific free promotions are already automatically excluded with the respective settings
---------------------------------------------------------------------------------------------------------------

-- Default
UPDATE UnitPromotions SET IsVisibleAboveFlag = 1;

UPDATE UnitPromotions
SET IsVisibleAboveFlag = 0
WHERE Type IN (
	'PROMOTION_EMBARKATION',
	'PROMOTION_FASTER_GENERAL',
	'PROMOTION_FAST_GENERAL',
	'PROMOTION_SIGNET',
	'PROMOTION_EXPRESS',
	'PROMOTION_LITERACY',
	'PROMOTION_IMMUNITY',
	'PROMOTION_PAX',
	'PROMOTION_NOBILITY',
	'PROMOTION_FALLOUT_REDUCTION',
	'PROMOTION_FALLOUT_IMMUNITY',
	'PROMOTION_ICE_BREAKERS',
	'PROMOTION_INSTA_HEAL'
);

---------------------------------------------------------------------------------------------------------------
-- Misc.
---------------------------------------------------------------------------------------------------------------

-- For promos where the title is the same as the Help text
UPDATE UnitPromotions SET SimpleHelpText = 1 WHERE Description = Help;
