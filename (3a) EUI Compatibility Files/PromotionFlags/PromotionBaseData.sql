-- Icon atlases for new tech boost icons, will create a new one new patch
UPDATE IconTextureAtlases
SET Filename='UnitActionsGold.dds'
WHERE Atlas='UNIT_ACTION_GOLD_ATLAS' AND IconSize='64';
UPDATE IconTextureAtlases
SET Filename='UnitActionsGold360.dds'
WHERE Atlas='UNIT_ACTION_GOLD_ATLAS' AND IconSize='45';

-- add columns to UnitPromotions to handle grouping of ranked promotions
ALTER TABLE UnitPromotions	ADD RankList									text;
ALTER TABLE UnitPromotions	ADD RankNumber							integer default 0;
-- column to hold visibility preference for each promotion
ALTER TABLE UnitPromotions	ADD FlagPromoOrder						int default 0;
ALTER TABLE UnitPromotions	ADD SimpleHelpText						boolean default 0;
  
ALTER TABLE UnitPromotions	ADD ShowInUnitPanel						boolean default 1;
ALTER TABLE UnitPromotions	ADD IsVisibleAboveFlag					boolean default 1;

-- For promos where the title is the same as the Help text
UPDATE UnitPromotions SET SimpleHelpText = 1 WHERE Description = Help;

---------------------------------------
--Define rankings for promotion sets
---------------------------------------
--Auto rank promotion sets ending in an obvious index; UndeadDevel: SQL basics: underscore is a wildcard character xD
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = SUBSTR(Type, -1) WHERE Type LIKE '%/_1' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = SUBSTR(Type, -1) WHERE Type LIKE '%/_2' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = SUBSTR(Type, -1) WHERE Type LIKE '%/_3' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = SUBSTR(Type, -1) WHERE Type LIKE '%/_4' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = SUBSTR(Type, -1) WHERE Type LIKE '%/_5' ESCAPE '/';

UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = 1 WHERE Type LIKE '%/_I' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-3), RankNumber = 2 WHERE Type LIKE '%/_II' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-4), RankNumber = 3 WHERE Type LIKE '%/_III' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-3), RankNumber = 4 WHERE Type LIKE '%/_IV' ESCAPE '/';
UPDATE UnitPromotions SET RankList = SUBSTR(Type, 1, LENGTH(Type)-2), RankNumber = 5 WHERE Type LIKE '%/_V' ESCAPE '/';


--Manually rank non-indexed sets

/* bal: this aren't related
UPDATE UnitPromotions SET RankList = 'MOBILITY', RankNumber = 70 WHERE Type = 'PROMOTION_NAVAL_TRADITION'; -- +1 Movement, from Imperialism opener
--UPDATE UnitPromotions SET RankList = 'MOBILITY', RankNumber = 72 WHERE Type = 'PROMOTION_GREAT_LIGHTHOUSE'; -- UndeadDevel: VP replaced this with the promo on the following line
UPDATE UnitPromotions SET RankList = 'MOBILITY', RankNumber = 71 WHERE Type = 'PROMOTION_NAVAL_SENTRY_II'; -- don't add the regular naval sentry here as it will make scouting ships harder to identify if GL was built
UPDATE UnitPromotions SET RankList = 'MOBILITY', RankNumber = 72 WHERE Type = 'PROMOTION_MOBILITY'; -- UndeadDevel: make this have precedence as it's the only one in the group that can be chosen for individual units
*/
UPDATE UnitPromotions SET RankList = 'SCOUTING_SIGHT', RankNumber = 1 WHERE Type = 'PROMOTION_EXTRA_SIGHT_I'; -- +1 Sight (Zeppelin, Pioneer)
UPDATE UnitPromotions SET RankList = 'SCOUTING_SIGHT', RankNumber = 2 WHERE Type = 'PROMOTION_EXTRA_SIGHT_II'; -- +1 Sight
UPDATE UnitPromotions SET RankList = 'SCOUTING_SIGHT', RankNumber = 3 WHERE Type = 'PROMOTION_EXTRA_SIGHT_III';
UPDATE UnitPromotions SET RankList = 'SCOUTING_SIGHT', RankNumber = 4 WHERE Type = 'PROMOTION_EXTRA_SIGHT_IV';
UPDATE UnitPromotions SET RankList = 'SCOUTING_SIGHT', RankNumber = 5 WHERE Type = 'PROMOTION_IMPERIALISM_OPENER'; -- +1 Sight
-- UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 6 WHERE Type = 'PROMOTION_TREASURE_FLEET'; -- UndeadDevel: this now gives +1 Sight and an attack bonus and ability to heal outside Friendly Territory
-- UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 7 WHERE Type = 'PROMOTION_SENTRY'; -- UndeadDevel: +1 Sight, make this have -- precedence as it's the only one in the group that can be chosen for individual units
UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 8 WHERE Type = 'PROMOTION_SKIRMISHER_SENTRY';
UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 1 WHERE Type = 'PROMOTION_SCOUTING_1'; -- +1 Sight (Recon unit line)
																																					   
UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 2 WHERE Type = 'PROMOTION_SCOUTING_2'; -- +1 Sight (Recon unit line)
UPDATE UnitPromotions SET RankList = 'SCOUTING', RankNumber = 3 WHERE Type = 'PROMOTION_SCOUTING_3'; -- +1 Movement (Recon unit line)

  
UPDATE UnitPromotions SET RankList = 'HONOR', RankNumber = 1 WHERE Type = 'PROMOTION_HONOR_BONUS';
UPDATE UnitPromotions SET RankList = 'HONOR', RankNumber = 2 WHERE Type = 'PROMOTION_KILL_HEAL';
UPDATE UnitPromotions SET RankList = 'HONOR', RankNumber = 3 WHERE Type = 'PROMOTION_GOD_WAR';
UPDATE UnitPromotions SET RankList = 'HONOR', RankNumber = 4 WHERE Type = 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY'; -- MUC4VP uses it for the Eagle
UPDATE UnitPromotions SET RankList = 'HONOR', RankNumber = 5 WHERE Type = 'PROMOTION_HEAL_IF_DESTROY_ENEMY'; -- Janissary
  
UPDATE UnitPromotions SET RankList = 'BUFFALO', RankNumber = 1 WHERE Type = 'PROMOTION_IKLWA'; -- UndeadDevel: now in the correct order
UPDATE UnitPromotions SET RankList = 'BUFFALO', RankNumber = 2 WHERE Type = 'PROMOTION_BUFFALO_LOINS'; -- UndeadDevel: now in the correct order
UPDATE UnitPromotions SET RankList = 'BUFFALO', RankNumber = 3 WHERE Type = 'PROMOTION_BUFFALO_CHEST';
UPDATE UnitPromotions SET RankList = 'BUFFALO', RankNumber = 4 WHERE Type = 'PROMOTION_BUFFALO_HORNS';

UPDATE UnitPromotions SET RankList = 'PROMOTION_MEDIC', RankNumber = 1 WHERE Type = 'PROMOTION_MEDIC';
UPDATE UnitPromotions SET RankList = 'PROMOTION_MEDIC', RankNumber = 2 WHERE Type = 'PROMOTION_MEDIC_II';

UPDATE UnitPromotions SET RankList = 'PROMOTION_ANTI_AIR', RankNumber = 1 WHERE Type = 'PROMOTION_ANTI_AIR';
UPDATE UnitPromotions SET RankList = 'PROMOTION_ANTI_AIR', RankNumber = 2 WHERE Type = 'PROMOTION_ANTI_AIR_II';

UPDATE UnitPromotions SET RankList = 'PROMOTION_SPLASH', RankNumber = 1 WHERE Type = 'PROMOTION_SPLASH';
UPDATE UnitPromotions SET RankList = 'PROMOTION_SPLASH', RankNumber = 2 WHERE Type = 'PROMOTION_SPLASH_II';

UPDATE UnitPromotions SET RankList = 'PROMOTION_TERROR', RankNumber = 1 WHERE Type = 'PROMOTION_TERROR';
UPDATE UnitPromotions SET RankList = 'PROMOTION_TERROR', RankNumber = 2 WHERE Type = 'PROMOTION_TERROR_II';

UPDATE UnitPromotions SET RankList = 'PROMOTION_CHARGE', RankNumber = 1 WHERE Type = 'PROMOTION_CHARGE';
UPDATE UnitPromotions SET RankList = 'PROMOTION_CHARGE', RankNumber = 2 WHERE Type = 'PROMOTION_CHARGE_II';

-- group generic combat buffs
UPDATE UnitPromotions SET RankList = 'GENERIC_COMBAT_BUFF', RankNumber = 1 WHERE Type = 'PROMOTION_MORALE';
UPDATE UnitPromotions SET RankList = 'GENERIC_COMBAT_BUFF', RankNumber = 2 WHERE Type = 'PROMOTION_EE_DRYDOCK'; --EE

-- UndeadDevel: differentiate interception promos
UPDATE UnitPromotions SET RankList = 'INTERCEPTOR', RankNumber = 1 WHERE Type = 'PROMOTION_INTERCEPTION_1';
UPDATE UnitPromotions SET RankList = 'INTERCEPTOR', RankNumber = 2 WHERE Type = 'PROMOTION_INTERCEPTION_2';
UPDATE UnitPromotions SET RankList = 'INTERCEPTOR', RankNumber = 3 WHERE Type = 'PROMOTION_INTERCEPTION_3';

-- UndeadDevel: fold in all embarkation promos
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 1 WHERE Type = 'PROMOTION_EMBARKATION';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 2 WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 3 WHERE Type = 'PROMOTION_DEEPWATER_EMBARKATION';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 4 WHERE Type = 'PROMOTION_EMBARKED_SIGHT';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 5 WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 6 WHERE Type = 'PROMOTION_WAR_CANOES';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 7 WHERE Type = 'PROMOTION_OCEAN_CROSSING';
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 8 WHERE Type = 'PROMOTION_UNIT_DENMARK_VIKING_EXPLORATION_CAN_CROSS_OCEAN'; -- MUC4VP
UPDATE UnitPromotions SET RankList = 'EMBARKATION', RankNumber = 9 WHERE Type = 'PROMOTION_HOVER_UNIT';				-- not to be confused with PROMOTION_HOVERING_UNIT

-- UndeadDevel: fold in submarine promos
/* bal: these aren't related
UPDATE UnitPromotions SET RankList = 'SUBMARINES', RankNumber = 1 WHERE Type = 'PROMOTION_INVISIBLE_SUBMARINE';
UPDATE UnitPromotions SET RankList = 'SUBMARINES', RankNumber = 2 WHERE Type = 'PROMOTION_SEE_INVISIBLE_SUBMARINE';
*/
UPDATE UnitPromotions SET RankList = 'SUBMARINES', RankNumber = 1 WHERE Type = 'PROMOTION_ANTI_SUBMARINE_I';
UPDATE UnitPromotions SET RankList = 'SUBMARINES', RankNumber = 2 WHERE Type = 'PROMOTION_ANTI_SUBMARINE_II';

-- UndeadDevel: group some anti-City boni
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 1 WHERE Type = 'PROMOTION_STATUE_ZEUS';
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 2 WHERE Type = 'PROMOTION_FIRE_SUPPORT'; -- naval melee (+33%)
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 3 WHERE Type = 'PROMOTION_HEAVY_PAYLOAD'; -- cruise missiles (+50%)
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 4 WHERE Type = 'PROMOTION_SIEGE'; -- land melee unit City Assault (+50% and less damage from Cities)
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 5 WHERE Type = 'PROMOTION_CITY_SIEGE'; -- siege units (+100%)
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 6 WHERE Type = 'PROMOTION_CITY_SIEGE_II'; -- currently unused, it seems (+150%)
UPDATE UnitPromotions SET RankList = 'ANTI_CITY', RankNumber = 7 WHERE Type = 'PROMOTION_CITY_ASSAULT'; -- currently unused, it seems (+250%)

-- UndeadDevel: fold in hovering unit promos
UPDATE UnitPromotions SET RankList = 'HOVERING_UNITS', RankNumber = 1 WHERE Type = 'PROMOTION_HOVERING_UNIT';
UPDATE UnitPromotions SET RankList = 'HOVERING_UNITS', RankNumber = 2 WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST';
UPDATE UnitPromotions SET RankList = 'HOVERING_UNITS', RankNumber = 3 WHERE Type = 'PROMOTION_FLAT_MOVEMENT_COST';

-- UndeadDevel: group late game scout pillaging promos
UPDATE UnitPromotions SET RankList = 'SPECOPS_PILLAGE', RankNumber = 1 WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES';
UPDATE UnitPromotions SET RankList = 'SPECOPS_PILLAGE', RankNumber = 2 WHERE Type = 'PROMOTION_SCOUT_XP_PILLAGE';

-- UndeadDevel: group GG movement and aura promos
UPDATE UnitPromotions SET RankList = 'GREAT_GENERAL_MOVEMENT', RankNumber = 1 WHERE Type = 'PROMOTION_FASTER_GENERAL';		-- +1 Movement Points for GG from Imperialism
UPDATE UnitPromotions SET RankList = 'GREAT_GENERAL_MOVEMENT', RankNumber = 2 WHERE Type = 'PROMOTION_FAST_GENERAL';			-- +3 Movement Points for GG from Lightning Warfare

UPDATE UnitPromotions SET RankList = 'GREAT_GENERAL_AURA', RankNumber = 1 WHERE Type = 'PROMOTION_GREAT_GENERAL';					-- normal aura
UPDATE UnitPromotions SET RankList = 'GREAT_GENERAL_AURA', RankNumber = 2 WHERE Type = 'PROMOTION_BETTER_LEADERSHIP';			-- improved aura from Regimental System (Imperialism)

/*
-- UndeadDevel: group Himeji with Homeland Guardian to reduce amount of defensive promotions (sometimes it can happen that there are 5 Shields next to one another)/ balparmak-VP integration: not right now
UPDATE UnitPromotions SET RankList = 'HOMELAND_DEFENDER', RankNumber = 1 WHERE Type = 'PROMOTION_HIMEJI_CASTLE';
UPDATE UnitPromotions SET RankList = 'HOMELAND_DEFENDER', RankNumber = 2 WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN';
*/
-- UndeadDevel: group Cover and Field works promos
UPDATE UnitPromotions SET RankList = 'FIELD_WORKS', RankNumber = 1 WHERE Type = 'PROMOTION_FIELD_WORKS_0';
UPDATE UnitPromotions SET RankList = 'FIELD_WORKS', RankNumber = 2 WHERE Type = 'PROMOTION_FIELD_WORKS_1';
UPDATE UnitPromotions SET RankList = 'FIELD_WORKS', RankNumber = 3 WHERE Type = 'PROMOTION_FIELD_WORKS_2';
UPDATE UnitPromotions SET RankList = 'FIELD_WORKS', RankNumber = 4 WHERE Type = 'PROMOTION_FIELD_WORKS_3';

UPDATE UnitPromotions SET RankList = 'COVER', RankNumber = 1 WHERE Type = 'PROMOTION_COVER_1';
UPDATE UnitPromotions SET RankList = 'COVER', RankNumber = 2 WHERE Type = 'PROMOTION_COVER_2';

-- Manual fixes for pseudo-4 proms
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_TARGETING_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_COASTAL_RAIDER_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_BOMBARDMENT_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_BOARDING_PARTY_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_BARRAGE_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_ACCURACY_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_SHOCK_4';
UPDATE UnitPromotions SET RankList = null , RankNumber = 0 WHERE Type = 'PROMOTION_DRILL_4';

-- UndeadDevel: Unique City States Clermont promotions
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 1 WHERE Type = 'PROMOTION_CLERMONT';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 2 WHERE Type = 'PROMOTION_CLERMONT1';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 3 WHERE Type = 'PROMOTION_CLERMONT2';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 4 WHERE Type = 'PROMOTION_CLERMONT3';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 5 WHERE Type = 'PROMOTION_CLERMONT4';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 6 WHERE Type = 'PROMOTION_CLERMONT5';
UPDATE UnitPromotions SET RankList = 'UCS_CLERMONT', RankNumber = 7 WHERE Type = 'PROMOTION_CLERMONT6';

-- UndeadDevel: need to adjust these, unfortunately
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 1 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_0'; -- MUC4VP
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 2 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_1';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 3 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_2';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 4 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_3';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 5 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_4';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 6 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_5';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 7 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_6';
UPDATE UnitPromotions SET RankList = 'BABYLON_LEGACY', RankNumber = 8 WHERE Type = 'PROMOTION_UNIT_BABYLON_LEGACY_7';

-- UndeadDevel: group MUC4VP "cause and effect" promos
UPDATE UnitPromotions SET RankList = 'ROME_PRAEFECTUS_CASTRORUM_CE', RankNumber = 1 WHERE Type = 'PROMOTION_UNIT_ROME_PRAEFECTUS_CASTRORUM_ON_LEGION';
UPDATE UnitPromotions SET RankList = 'ROME_PRAEFECTUS_CASTRORUM_CE', RankNumber = 2 WHERE Type = 'PROMOTION_UNIT_ROME_PRAEFECTUS_CASTRORUM_EFFECT';

UPDATE UnitPromotions SET RankList = 'ROME_LEGATUS_LEGIONIS_CE', RankNumber = 1 WHERE Type = 'PROMOTION_UNIT_ROME_LEGATUS_LEGIONIS_ON_BALLISTA';
UPDATE UnitPromotions SET RankList = 'ROME_LEGATUS_LEGIONIS_CE', RankNumber = 2 WHERE Type = 'PROMOTION_UNIT_ROME_LEGATUS_LEGIONIS_EFFECT';

UPDATE UnitPromotions SET RankList = 'SHOSHONE_BIG_HORSE_DANCE_CE', RankNumber = 1 WHERE Type = 'PROMOTION_UNIT_SHOSHONE_BIG_HORSE_DANCE';
UPDATE UnitPromotions SET RankList = 'SHOSHONE_BIG_HORSE_DANCE_CE', RankNumber = 2 WHERE Type = 'PROMOTION_UNIT_SHOSHONE_BIG_HORSE_DANCE_PERFORMED';

-- UndeadDevel: group these since they're not lost on upgrade and Scouts are too promotion-laden
UPDATE UnitPromotions SET RankList = 'SCOUT_GOODY_BONUS_U', RankNumber = 1 WHERE Type = 'PROMOTION_GOODY_HUT_PICKER';
UPDATE UnitPromotions SET RankList = 'SCOUT_GOODY_BONUS_U', RankNumber = 2 WHERE Type = 'PROMOTION_UNIT_MAYA_SCOUT_GOODY_BONUS_2';

-- UndeadDevel: group these two since they're both present on all siege units and the row is really long with lots of penalties
UPDATE UnitPromotions SET RankList = 'SIEGE_PENALTIES', RankNumber = 1 WHERE Type = 'PROMOTION_MUST_SET_UP';
UPDATE UnitPromotions SET RankList = 'SIEGE_PENALTIES', RankNumber = 2 WHERE Type = 'PROMOTION_NAVAL_MISFIRE';
UPDATE UnitPromotions SET RankList = 'SIEGE_PENALTIES', RankNumber = 3 WHERE Type = 'PROMOTION_SIEGE_INACCURACY';

---------------------------------------
--Set FlagPromoOrder to keep important promos visible and
-- consistently positioned, lower numbers take precedence
---------------------------------------
UPDATE UnitPromotions SET FlagPromoOrder = OrderPriority;
UPDATE UnitPromotions SET FlagPromoOrder = 90 where OrderPriority = 0;

-- most important promotions (mostly scouting, individual or temporary buffs, debuffs)
UPDATE UnitPromotions SET FlagPromoOrder = 10 WHERE Type IN (
	'PROMOTION_BOARDED_I',			-- temporary debuffs
	'PROMOTION_BOARDED_II',
	'PROMOTION_BOARDED_III',
	'PROMOTION_DAZED',
	'PROMOTION_UNIT_BYZANTIUM_GREEK_FIRE_1', -- MUC4VP
	'PROMOTION_UNIT_BYZANTIUM_GREEK_FIRE_2',
	'PROMOTION_UNIT_BYZANTIUM_GREEK_FIRE_3',
	'PROMOTION_UNIT_POLYNESIA_KAPU_KUIALUA_1',
	'PROMOTION_UNIT_ETHIOPIA_MAIM_1'
);

UPDATE UnitPromotions SET FlagPromoOrder = 12 WHERE Type IN (
	'PROMOTION_UNIT_CARTHAGE_ZOC', -- MUC4VP temporary buff
	'PROMOTION_EE_FIRST_RATE_DEFENSE' -- EE temporary buff
);

UPDATE UnitPromotions SET FlagPromoOrder = 13 WHERE Type IN (
	'PROMOTION_HEROES'				-- Capture Great People
);

UPDATE UnitPromotions SET FlagPromoOrder = 14 WHERE Type IN (
	'PROMOTION_ALHAMBRA',			-- currently lost on upgrade, so hard to remember which unit has it
	'PROMOTION_HELI_MOBILITY_1',
	'PROMOTION_HELI_MOBILITY_2'
);

UPDATE UnitPromotions SET FlagPromoOrder = 15 WHERE Type IN (
	'PROMOTION_HELI_AMBUSH_1',
	'PROMOTION_HELI_AMBUSH_2',
	'PROMOTION_BUSHIDO',
	'PROMOTION_RIGHTEOUSNESS',		-- Bushido promos
	'PROMOTION_COURAGE',
	'PROMOTION_BENEVOLENCE',
	'PROMOTION_RESPECT',
	'PROMOTION_SINCERITY',
	'PROMOTION_BUSHIDO_HONOR',
	'PROMOTION_LOYALTY',
	'PROMOTION_SELF_CONTROL',
	'PROMOTION_MYSTIC_BLADE',
	'PROMOTION_INVULNERABIILITY',	-- Kris promos
	'PROMOTION_SNEAK_ATTACK',
	'PROMOTION_EVIL_SPIRITS',
	'PROMOTION_ENEMY_BLADE',
	'PROMOTION_HEROISM',
	'PROMOTION_AMBITION',
	'PROMOTION_RESTLESSNESS',
	'PROMOTION_RECRUITMENT',
	'PROMOTION_UNIT_BABYLON_LEGACY_0', -- MUC4VP
	'PROMOTION_UNIT_BABYLON_LEGACY_1',
	'PROMOTION_UNIT_BABYLON_LEGACY_2',
	'PROMOTION_UNIT_BABYLON_LEGACY_3',
	'PROMOTION_UNIT_BABYLON_LEGACY_4',
	'PROMOTION_UNIT_BABYLON_LEGACY_5',
	'PROMOTION_UNIT_BABYLON_LEGACY_6',
	'PROMOTION_UNIT_BABYLON_LEGACY_7',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_1', -- MUC4VP
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_2',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_3',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_4',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_5',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_6',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_7',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_8',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_9',
	'PROMOTION_UNIT_AZTEC_CUAUHOCELOTL_10',
	'PROMOTION_UNIT_MAYA_KATUN_AHAW_1', -- MUC4VP
	'PROMOTION_UNIT_MAYA_KATUN_AHAW_2',
	'PROMOTION_UNIT_MAYA_KATUN_AHAW_3',
	'PROMOTION_UNIT_MAYA_KATUN_AHAW_4'
);

UPDATE UnitPromotions SET FlagPromoOrder = 16 WHERE Type IN (
	'PROMOTION_CLERMONT', -- Unique City States
	'PROMOTION_CLERMONT1',
	'PROMOTION_CLERMONT2',
	'PROMOTION_CLERMONT3',
	'PROMOTION_CLERMONT4',
	'PROMOTION_CLERMONT5',
	'PROMOTION_CLERMONT6'
);

UPDATE UnitPromotions SET FlagPromoOrder = 17 WHERE Type IN (
	'PROMOTION_SCOUTING_1',
	'PROMOTION_SCOUTING_2',
	'PROMOTION_SCOUTING_3',
	'PROMOTION_EE_ADVENTURER',
	'PROMOTION_NAVAL_SENTRY_II',
	'PROMOTION_SKIRMISHER_SENTRY'
);

UPDATE UnitPromotions SET FlagPromoOrder = 18 WHERE Type IN (
	'PROMOTION_NAVAL_RECON_1',
	'PROMOTION_NAVAL_RECON_2',
	'PROMOTION_NAVAL_SENTRY',
	'PROMOTION_SENTRY'
);

UPDATE UnitPromotions SET FlagPromoOrder = 19 WHERE Type IN (
	'PROMOTION_SIEGE_I',				-- lets make sure they are displayed before their finishers
	'PROMOTION_SIEGE_II',
	'PROMOTION_SIEGE_III',
    'PROMOTION_WOLFPACK_1',
    'PROMOTION_WOLFPACK_2',
    'PROMOTION_WOLFPACK_3'
);

-- standard lines A
UPDATE UnitPromotions SET FlagPromoOrder = 20 WHERE Type IN (
	'PROMOTION_SHOCK_1',
	'PROMOTION_SHOCK_2',
	'PROMOTION_SHOCK_3',
	'PROMOTION_SHOCK_4',
	'PROMOTION_BARRAGE_1',
	'PROMOTION_BARRAGE_2',
	'PROMOTION_BARRAGE_3',
    'PROMOTION_BARRAGE_4',
	'PROMOTION_FIELD_I',
	'PROMOTION_FIELD_II',
	'PROMOTION_FIELD_III',
	'PROMOTION_WOODLAND_TRAILBLAZER_I',
	'PROMOTION_WOODLAND_TRAILBLAZER_II',
	'PROMOTION_WOODLAND_TRAILBLAZER_III',
	'PROMOTION_TRENCHES_1',
	'PROMOTION_TRENCHES_2',
	'PROMOTION_TRENCHES_3',
	'PROMOTION_TARGETING_1',
	'PROMOTION_TARGETING_2',
	'PROMOTION_TARGETING_3',
    'PROMOTION_BOARDING_PARTY_1',
    'PROMOTION_BOARDING_PARTY_2',
    'PROMOTION_BOARDING_PARTY_3',
    'PROMOTION_BOARDING_PARTY_4',
	'PROMOTION_DOGFIGHTING_1',
	'PROMOTION_DOGFIGHTING_2',
	'PROMOTION_DOGFIGHTING_3',
	'PROMOTION_AIR_SIEGE_1',
	'PROMOTION_AIR_SIEGE_2',
	'PROMOTION_AIR_SIEGE_3',
	'PROMOTION_FLIGHT_DECK_1',
	'PROMOTION_FLIGHT_DECK_2',
	'PROMOTION_FLIGHT_DECK_3'
);

-- standard lines B (except Armor Plating and Wolf Pack)
UPDATE UnitPromotions SET FlagPromoOrder = 25 WHERE Type IN (
	'PROMOTION_DRILL_1',
	'PROMOTION_DRILL_2',
	'PROMOTION_DRILL_3',
	'PROMOTION_DRILL_4',
	'PROMOTION_ACCURACY_1',
	'PROMOTION_ACCURACY_2',
	'PROMOTION_ACCURACY_3',
    'PROMOTION_ACCURACY_4',
	'PROMOTION_SURVIVALISM_1',
	'PROMOTION_SURVIVALISM_2',
	'PROMOTION_SURVIVALISM_3',
	'PROMOTION_GUERRILLA_1',
	'PROMOTION_GUERRILLA_2',
	'PROMOTION_GUERRILLA_3',
	'PROMOTION_BOMBARDMENT_1',
	'PROMOTION_BOMBARDMENT_2',
	'PROMOTION_BOMBARDMENT_3',
	'PROMOTION_BOMBARDMENT_4',
    'PROMOTION_COASTAL_RAIDER_1',
    'PROMOTION_COASTAL_RAIDER_2',
    'PROMOTION_COASTAL_RAIDER_3',
    'PROMOTION_COASTAL_RAIDER_4',
	'PROMOTION_INTERCEPTION_1',
	'PROMOTION_INTERCEPTION_2',
	'PROMOTION_INTERCEPTION_3',
	'PROMOTION_AIR_TARGETING_1',
	'PROMOTION_AIR_TARGETING_2',
	'PROMOTION_AIR_TARGETING_3'
);

UPDATE UnitPromotions SET FlagPromoOrder = 28 WHERE Type IN (
	'PROMOTION_MEDIC',
	'PROMOTION_MEDIC_II',
	'PROMOTION_TARGETING_4', -- used for subs and naval ranged alike, can be reached without Targeting (subs) and really doesn't fit the Targeting theme
	'PROMOTION_COASTAL_TERROR',
	'PROMOTION_DRILL_4',
	'PROMOTION_BARRAGE_4',
	'PROMOTION_BOMBARDMENT_4',
	'PROMOTION_COASTAL_RAIDER_4'
);

UPDATE UnitPromotions SET FlagPromoOrder = 29 WHERE Type IN (
	'PROMOTION_NAVAL_SIEGE',
	'PROMOTION_BLITZ',
	'PROMOTION_SECOND_ATTACK'
);

UPDATE UnitPromotions SET FlagPromoOrder = 30 WHERE Type IN (
    'PROMOTION_DAMAGE_REDUCTION',
    'PROMOTION_PIRACY',
	'PROMOTION_MARCH',
	'PROMOTION_MARCH_RANGED',
	'PROMOTION_SKIRMISHER_MARCH',
	'PROMOTION_MARCH_IV',
	'PROMOTION_SPLASH',
	'PROMOTION_SPLASH_II',
    'PROMOTION_EVASION_I', -- Air Penetration, can't be picked by Stealth Bombers
    'PROMOTION_EVASION_II',
    'PROMOTION_EVASION_III', -- Stealth
	'PROMOTION_UNIT_BYZANTIUM_GREEK_FIRE_0', -- MUC4VP plague placers
	'PROMOTION_UNIT_POLYNESIA_KAPU_KUIALUA_0',
	'PROMOTION_UNIT_ETHIOPIA_MAIM_0'
);

UPDATE UnitPromotions SET FlagPromoOrder = 34 WHERE Type IN (
    'PROMOTION_MINELAYER',
    'PROMOTION_PRESS_GANGS',
    'PROMOTION_MOBILITY',
	'PROMOTION_SKIRMISHER_MOBILITY',
    'PROMOTION_EVASION'
);

UPDATE UnitPromotions SET FlagPromoOrder = 35 WHERE Type IN (
	'PROMOTION_AMPHIBIOUS',
	'PROMOTION_WOODSMAN',
	'PROMOTION_LOGISTICS',
	'PROMOTION_AIR_LOGISTICS',
	'PROMOTION_NAVAL_LOGISTICS',
    'PROMOTION_BREACHER',
    'PROMOTION_ENCIRCLEMENT'
);

UPDATE UnitPromotions SET FlagPromoOrder = 39 WHERE Type IN (
	'PROMOTION_SIEGE',
    'PROMOTION_INTERCEPTION_I',
    'PROMOTION_INTERCEPTION_II',
    'PROMOTION_INTERCEPTION_III',
    'PROMOTION_INTERCEPTION_IV'
);

UPDATE UnitPromotions SET FlagPromoOrder = 40 WHERE Type IN (
	'PROMOTION_BETTER_BOMBARDMENT',
	'PROMOTION_COVER_VANGUARD_1',
	'PROMOTION_COVER_VANGUARD_2',
	'PROMOTION_VOLLEY',
	'PROMOTION_DOUBLE_PLUNDER',
    'PROMOTION_ANTI_AIR',
    'PROMOTION_ANTI_AIR_II',
	'PROMOTION_TERROR',
	'PROMOTION_TERROR_II'
);

UPDATE UnitPromotions SET FlagPromoOrder = 42 WHERE Type IN (
	'PROMOTION_SKIRMISHER_DOCTRINE',
	'PROMOTION_FORMATION_1',
	'PROMOTION_FORMATION_2',
    'PROMOTION_ANTI_FIGHTER'
);

UPDATE UnitPromotions SET FlagPromoOrder = 43 WHERE Type IN (
	'PROMOTION_UNIT_ETHIOPIA_HOOKED_WEAPON', -- MUC4VP
	'PROMOTION_AIR_RANGE',
	'PROMOTION_RANGE'
);

UPDATE UnitPromotions SET FlagPromoOrder = 45 WHERE Type IN (
	'PROMOTION_CHARGE',
	'PROMOTION_CHARGE_II',
	'PROMOTION_AMBUSH_1',
	'PROMOTION_AMBUSH_2',
	'PROMOTION_SKIRMISH',
	'PROMOTION_SKIRMISHER_POWER',
	'PROMOTION_INDIRECT_FIRE',
	'PROMOTION_SORTIE',
	'PROMOTION_AIR_AMBUSH_1',
	'PROMOTION_AIR_AMBUSH_2'
);

UPDATE UnitPromotions SET FlagPromoOrder = 46 WHERE Type IN (
	'PROMOTION_GAIN_EXPERIENCE',
	'PROMOTION_EE_DRYDOCK',
	'PROMOTION_SAPPER',
	'PROMOTION_REPAIR',
	'PROMOTION_AIR_REPAIR',
	'PROMOTION_HELI_REPAIR'
);

-- want to keep this visible in the row so we know at a glance what unit was produced in the capital
UPDATE UnitPromotions SET FlagPromoOrder = 50 WHERE Type IN (
    'PROMOTION_MORALE',
    'PROMOTION_MORALE_EVENT'
);

-- most of these are unit-specific but kept on upgrade...want to know which unit "descends" from a special one
UPDATE UnitPromotions SET FlagPromoOrder = 55 WHERE Type IN (
	'PROMOTION_UNIT_AMERICA_HARDENED', -- MUC4VP (FreePromotion for Monitor)
	'PROMOTION_UNIT_ARABIA_DESERT_RAIDER', -- MUC4VP (FreePromotion for Hashemite Raider)
	'PROMOTION_UNIT_ASSYRIA_IRON_AND_HORSE', -- MUC4VP (FreePromotion for Iron Chariot)
	'PROMOTION_UNIT_NETHERLANDS_BURGEMEESTE', -- MUC4VP (FreePromotion for Goedendag)
	'PROMOTION_REPEATER',
	'PROMOTION_ASSIZE_OF_ARMS',
	'PROMOTION_RAIDER',
	'PROMOTION_FEARED_ELEPHANT',
	'PROMOTION_GOLDEN_AGE_POINTS',
	'PROMOTION_MOVEMENT_TO_GENERAL',
	'PROMOTION_HEAVY_SHIP',
	'PROMOTION_HEAVY_CHARGE',
	'PROMOTION_GENERAL_STACKING',
	'PROMOTION_FREE_PILLAGE_MOVES',
	'PROMOTION_HAKKAA_PAALLE',
	'PROMOTION_SCOUT_XP_PILLAGE'
);

-- mostly unit-specific promos
UPDATE UnitPromotions SET FlagPromoOrder = 60 WHERE Type IN (
	'PROMOTION_SCOUT_XP_SPOTTING',
	'PROMOTION_SPAWN_GENERALS_I',
	'PROMOTION_SPAWN_GENERALS_II',
	'PROMOTION_ADJACENT_BONUS',
	'PROMOTION_AOE_STRIKE_ON_KILL',
	'PROMOTION_SKI_INFANTRY',
	'PROMOTION_JAGUAR',
	'PROMOTION_MOHAWK',
	'PROMOTION_RECON_BANDEIRANTES',
	'PROMOTION_OPEN_TERRAIN',
	'PROMOTION_GUERILLA_FIGHTER',
	'PROMOTION_FOREIGN_LANDS',
	'PROMOTION_FLANK_ATTACK_BONUS',
	'PROMOTION_ESPRIT_DE_CORPS',
	'PROMOTION_HAKA_WAR_DANCE',
	'PROMOTION_PRIZE_SHIPS',
	'PROMOTION_DESERT_WARRIOR',
	'PROMOTION_MOON_STRIKER',
	'PROMOTION_EE_FIRST_RATE',
	'PROMOTION_BUFFALO_CHEST',
	'PROMOTION_BUFFALO_LOINS',
	'PROMOTION_BUFFALO_HORNS',
	'PROMOTION_FASTER_HEAL',
	'PROMOTION_SELL_EXOTIC_GOODS',
	'PROMOTION_SCOUT_CAVALRY',
	'PROMOTION_UNIT_ASSYRIA_FURY_OF_NERGAL', -- MUC4VP from here on...most of these are not lost with upgrade so they should not get pushed to the very back
	'PROMOTION_UNIT_CELTS_CARNYX',
	'PROMOTION_UNIT_INDONESIA_FLYING_THE_AMA',
	'PROMOTION_UNIT_KOREA_DECK_SPIKES',
	'PROMOTION_UNIT_PERSIA_SAFAVI_AGITATOR',
	'PROMOTION_UNIT_ROME_LEGATUS_LEGIONIS_ON_BALLISTA',
	'PROMOTION_UNIT_ROME_LEGATUS_LEGIONIS_EFFECT',
	'PROMOTION_UNIT_ROME_PRAEFECTUS_CASTRORUM_ON_LEGION',
	'PROMOTION_UNIT_ROME_PRAEFECTUS_CASTRORUM_EFFECT',
	'PROMOTION_UNIT_SHOSHONE_BIG_HORSE_DANCE',
	'PROMOTION_UNIT_SHOSHONE_BIG_HORSE_DANCE_PERFORMED',
	'PROMOTION_UNIT_SIAM_COASTAL_CANNON',
	'PROMOTION_UNIT_SONGHAI_HORSE_FATHER',
	'PROMOTION_UNIT_OTTOMANS_SAHI_TOPU', -- special case: unit damages itself
	'PROMOTION_UNIT_SIAM_CROACHING_TIGER',
	'PROMOTION_UNIT_SPAIN_INVINCIBLE',
	'PROMOTION_UNIT_MAYA_KATUN_AHAW_0',
	'PROMOTION_UNIT_AZTEC_CAPTIVES_OF_WAR',
	'PROMOTION_UNIT_HUNS_HORSE_ARCHER',
	'PROMOTION_UNIT_AMERICA_MONITOR',
	'PROMOTION_UNIT_NETHERLANDS_GOEDENDAG',
	'PROMOTION_UNIT_INDIA_DHANURVIDYA',
	'PROMOTION_UNIT_ENGLAND_AGINCOURT',
	'PROMOTION_UNIT_EGYPT_MERCY',
	'PROMOTION_UNIT_HUNS_SACKER',
	'PROMOTION_UNIT_RUSSIA_GRAPESHOT',
	'PROMOTION_UNIT_DENMARK_VIKING_EXPLORATION',
	'PROMOTION_UNIT_POLAND_BOGURODZICA',
	'PROMOTION_UNIT_GREECE_PHILHELLENISM',
	'PROMOTION_UNIT_AUSTRIA_YORKSCHER_MARCHE',
	'PROMOTION_UNIT_GERMANY_FUROR_TEUTONICUS',
	'PROMOTION_UNIT_JAPAN_YAMATO',
	'PROMOTION_UNIT_INCA_CHASQUIWASI',
	'PROMOTION_UNIT_ARABIA_GARLAND_MINE',
	'PROMOTION_UNIT_MONGOLIA_MINGGHAN',
	'PROMOTION_UNIT_MOROCCO_RAZZIA',
	'PROMOTION_ATLATL_ATTACK',
	'PROMOTION_UNIT_CHINA_KOWTOW',
	'PROMOTION_UNIT_BRAZIL_EXPEDITIONARY_FORCE'
);

-- mostly trait- or policy- specific promos and keeping some unit-specific ones separate from the above group
UPDATE UnitPromotions SET FlagPromoOrder = 61 WHERE Type IN (
	'PROMOTION_ATTACK_BONUS_SWEDEN',
	'PROMOTION_STEAM_POWERED', -- MUC4VP (FreePromotion for Monitor)
	'PROMOTION_MONGOL_TERROR',
	'PROMOTION_VIKING',
	'PROMOTION_LONGBOAT',
	'PROMOTION_HOMELAND_GUARDIAN_BOOGALOO',
	'PROMOTION_ENSLAVEMENT',
	'PROMOTION_AOE_STRIKE_FORTIFY', -- Pilum (Rome)
	'PROMOTION_SLINGER',
	'PROMOTION_UNIT_POLAND_BARBICAN',
	'PROMOTION_UNIT_PORTUGAL_AGE_OF_DISCOVERY',
	'PROMOTION_UNIT_SONGHAI_HORSE_FATHER_EFFECT',
	'PROMOTION_UNIT_AZTEC_HUMAN_SACRIFICE',
	'PROMOTION_UNIT_INDIA_EPIC',
	'PROMOTION_UNIT_SPAIN_SANTA_MARIA',
	'PROMOTION_UNIT_HUNS_MIGRATION',
	'PROMOTION_UNIT_GERMANY_BARBARIAN_ALLIANCE',
	'PROMOTION_UNIT_BRAZIL_TOURISM',
	'PROMOTION_ANTI_GUNPOWDER', -- Zulu Impi is the only one getting this and with MUC4VP it's not lost with upgrade
	'PROMOTION_ARSENALE'
);

UPDATE UnitPromotions SET FlagPromoOrder = 62 WHERE Type IN (
	'PROMOTION_EE_FASIL_GHEBBI'
);

-- mostly defensive promotions
UPDATE UnitPromotions SET FlagPromoOrder = 68 WHERE Type IN (
	'PROMOTION_TREASURE_FLEET',
	'PROMOTION_KNOCKOUT_I', -- this is actually the upgrade promo, but leave grouped as is, as it will behave in a useful way
	'PROMOTION_KNOCKOUT_II',
	'PROMOTION_FIELD_WORKS_0',
	'PROMOTION_FIELD_WORKS_1',
	'PROMOTION_FIELD_WORKS_2',
	'PROMOTION_FIELD_WORKS_3',
	'PROMOTION_COVER_1',
	'PROMOTION_COVER_2',
	'PROMOTION_MECH'
);

-- special case: used by both Armored units (FreePromotions) and Carriers (line promotion)
UPDATE UnitPromotions SET FlagPromoOrder = 69 WHERE Type IN (
	'PROMOTION_ARMOR_PLATING_1',
	'PROMOTION_ARMOR_PLATING_2',
	'PROMOTION_ARMOR_PLATING_3',
	'PROMOTION_UNIT_MAYA_SCOUT_GOODY_BONUS_2', -- MUC4VP
	'PROMOTION_DEFEND_NEAR_CAPITAL'
);

UPDATE UnitPromotions SET FlagPromoOrder = 70 WHERE Type IN (
	'PROMOTION_ANTIAIR_LAND_I',
	'PROMOTION_ANTIAIR_LAND_II',
	'PROMOTION_ANTIAIR_LAND_III',
	'PROMOTION_DEFENSE_PENALTY',
	'PROMOTION_NAVAL_DEFENSE_BOOST',
	'PROMOTION_SUPPLY'
);

UPDATE UnitPromotions SET FlagPromoOrder = 71 WHERE Type IN (
	'PROMOTION_HOMELAND_GUARDIAN',
	'PROMOTION_HIMEJI_CASTLE'
);

UPDATE UnitPromotions SET FlagPromoOrder = 72 WHERE Type IN (
	'PROMOTION_ANTANANARIVO', -- Unique City States combat buffs
	'PROMOTION_BELGRADE',
	'PROMOTION_HANOI',
	'PROMOTION_MBANZA_KONGO',
	'PROMOTION_SIDON',
	'PROMOTION_YEREVAN',
	'PROMOTION_LACONIA',
	'PROMOTION_ODENSO',
	'PROMOTION_AMBRACIA',
	'PROMOTION_DODOMA',
	'PROMOTION_FAYA',
	'PROMOTION_SURREY'
);

-- can still be useful in some cases
UPDATE UnitPromotions SET FlagPromoOrder = 75 WHERE Type IN (
	'PROMOTION_IGNORE_TERRAIN_COST',
	'PROMOTION_EXTRA_MOVES_I',
	'PROMOTION_COERCION',
	'PROMOTION_CBOEE_SKIRMISH', --EE
	'PROMOTION_2HANDER' --EE
);

UPDATE UnitPromotions SET FlagPromoOrder = 76 WHERE Type IN (
	'PROMOTION_CAN_MOVE_AFTER_ATTACKING',
	'PROMOTION_CAN_MOVE_AFTER_ATTACKING_NAVAL',
	'PROMOTION_CONSCRIPT'
);

-- most of these are generic buffs that will be known to the player
UPDATE UnitPromotions SET FlagPromoOrder = 80 WHERE Type IN (
	'PROMOTION_WITHDRAW_BEFORE_MELEE',
	'PROMOTION_UNIT_PORTUGAL_CACADOR' -- MUC4VP
);

UPDATE UnitPromotions SET FlagPromoOrder = 81 WHERE Type IN (
	'PROMOTION_ALTITUDE_TRAINING',
	'PROMOTION_PRIZE_RULES'
);

UPDATE UnitPromotions SET FlagPromoOrder = 82 WHERE Type IN (
	'PROMOTION_EVERLASTING_YOUTH'
);

UPDATE UnitPromotions SET FlagPromoOrder = 83 WHERE Type IN (
	'PROMOTION_HONOR_BONUS',
	'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY',
	'PROMOTION_HEAL_IF_DESTROY_ENEMY',
	'PROMOTION_KILL_HEAL'
);

UPDATE UnitPromotions SET FlagPromoOrder = 84 WHERE Type IN (
	'PROMOTION_MODERN_RANGED_PENALTY_I',
	'PROMOTION_MODERN_RANGED_PENALTY_II',
	'PROMOTION_ROUGH_TERRAIN_HALF_TURN',
	'PROMOTION_HEAVY_PAYLOAD',
	'PROMOTION_ANTI_SUBMARINE_I',
	'PROMOTION_ANTI_SUBMARINE_II',
	'PROMOTION_LIGHTNING_WARFARE',
	'PROMOTION_LIGHTNING_WARFARE_GUN',
	'PROMOTION_LIGHTNING_WARFARE_ARMOR'
);

UPDATE UnitPromotions SET FlagPromoOrder = 85 WHERE Type IN (
	'PROMOTION_BARBARIAN_BONUS',
	'PROMOTION_STRONGER_VS_DAMAGED',
	'PROMOTION_ATTACK_BONUS',
	'PROMOTION_ANTI_TANK',
	'PROMOTION_SILENT_HUNTER',
	'PROMOTION_NAVAL_BONUS',
	'PROMOTION_ANTI_HELICOPTER',
	'PROMOTION_ANTI_MOUNTED_I',
	'PROMOTION_ANTI_MOUNTED_II'
);

UPDATE UnitPromotions SET FlagPromoOrder = 86 WHERE Type IN (
	'PROMOTION_CITY_SIEGE',
	'PROMOTION_CITY_SIEGE_II',
	'PROMOTION_CITY_ASSAULT',
	'PROMOTION_FIRE_SUPPORT'
);

UPDATE UnitPromotions SET FlagPromoOrder = 87 WHERE Type IN (
	'PROMOTION_STATUE_ZEUS',
	'PROMOTION_AIR_RECON',
	'PROMOTION_UNIT_DENMARK_VIKING_EXPLORATION_CAN_CROSS_OCEAN',
	'PROMOTION_OCEAN_CROSSING'
);
	
-- <<< this is where all promos that are not manually ordered here will be inserted

UPDATE UnitPromotions SET FlagPromoOrder = 91 WHERE Type IN (
	'PROMOTION_GOODY_HUT_PICKER',
	'PROMOTION_CAN_MOVE_IMPASSABLE', -- may enter ice tiles (subs)
	'PROMOTION_FLAT_MOVEMENT_COST'
);

UPDATE UnitPromotions SET FlagPromoOrder = 92 WHERE Type IN (
	'PROMOTION_EMBARKATION',
	'PROMOTION_WAR_CANOES',
	'PROMOTION_ALLWATER_EMBARKATION',
	'PROMOTION_HOVER_UNIT',				-- not to be confused with PROMOTION_HOVERING_UNIT
	'PROMOTION_EMBARKED_SIGHT',
	'PROMOTION_DEEPWATER_EMBARKATION',
	'PROMOTION_DEFENSIVE_EMBARKATION',
	'PROMOTION_HONOR_BONUS',
	'PROMOTION_KILL_HEAL'
);

-- slightly more useful to know about than the generic penalties
UPDATE UnitPromotions SET FlagPromoOrder = 94 WHERE Type IN (
	'PROMOTION_CARGO_I',
	'PROMOTION_CARGO_II',
	'PROMOTION_CARGO_III',
	'PROMOTION_CARGO_IV',
	'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY',
	'PROMOTION_OCEAN_IMPASSABLE',
	'PROMOTION_OCEAN_HALF_MOVES',
	'PROMOTION_NO_DEFENSIVE_BONUSES',
	'PROMOTION_ROUGH_TERRAIN_ENDS_TURN'
);

-- generic penalties
UPDATE UnitPromotions SET FlagPromoOrder = 95 WHERE Type IN (
	'PROMOTION_SIGHT_PENALTY',
	'PROMOTION_SIGHT_PENALTY_NAVAL',
	'PROMOTION_AIR_MISFIRE',
	'PROMOTION_NAVAL_PENALTY',
	'PROMOTION_BARBARIAN_PENALTY_I',
	'PROMOTION_BARBARIAN_PENALTY_II',
	'PROMOTION_BARBARIAN_PENALTY_III',
	'PROMOTION_BIG_CITY_PENALTY',
	'PROMOTION_CITY_PENALTY',
	'PROMOTION_SMALL_CITY_PENALTY'
);

UPDATE UnitPromotions SET FlagPromoOrder = 96 WHERE Type IN (
	'PROMOTION_MOUNTED_PENALTY',
	'PROMOTION_WEAK_RANGED',
	'PROMOTION_NAVAL_MISFIRE',
	'PROMOTION_NAVAL_INACCURACY'
);

-- player sees the action button / model, so completely useless
UPDATE UnitPromotions SET FlagPromoOrder = 97 WHERE Type IN (
	'PROMOTION_SIEGE_INACCURACY',
	'PROMOTION_HOVERING_UNIT',
	'PROMOTION_PARADROP',
	'PROMOTION_EXTENDED_PARADROP'
);

-- completely useless
UPDATE UnitPromotions SET FlagPromoOrder = 98 WHERE Type IN (
	'PROMOTION_ONLY_DEFENSIVE',
	'PROMOTION_MUST_SET_UP'
);

-- don't put any other promotions in here so this promotion can remain the last one; that way it can be made transparent and doesn't create "holes" in the line
UPDATE UnitPromotions SET FlagPromoOrder = 99 WHERE Type IN (
	'PROMOTION_CREATED'
);

-- don't show these on the unit panel
UPDATE UnitPromotions SET ShowInUnitPanel = 0 WHERE Type IN (
	'PROMOTION_ONLY_DEFENSIVE');

---------------------------------------
--Promotions which should not be displayed above the Unit Flag
---------------------------------------
UPDATE UnitPromotions SET IsVisibleAboveFlag = 1;
UPDATE UnitPromotions SET IsVisibleAboveFlag = 0 WHERE
	Type LIKE '%PENALTY%'
	OR Type LIKE '%NOUPGRADE%'
	OR Type LIKE '%CARGO%'
	OR Type LIKE '%EMBARKATION%'
--	OR Type LIKE '%INTERCEPTION%'             		  -- UndeadDevel: since VP changes how these are used they are actually meaningful and should be displayed
--	OR Type LIKE '%EVASION%'
	OR Type IN (
	'PROMOTION_FASTER_GENERAL'						, --VP
	'PROMOTION_FAST_GENERAL'							, --VP
	'PROMOTION_NAVAL_MISFIRE'						, --VP
	'PROMOTION_SIEGE_INACCURACY'					, --VP
	'PROMOTION_NAVAL_INACCURACY'					, --VP
	'PROMOTION_AIR_MISFIRE'								, --VP
	'PROMOTION_OCEAN_CROSSING'					, --VP
	'PROMOTION_UNIT_DENMARK_VIKING_EXPLORATION_CAN_CROSS_OCEAN', -- MUC4VP
	'PROMOTION_RECON_EXPERIENCE'				, --VP
	'PROMOTION_HOVER_UNIT'								, --VP
	'PROMOTION_BARBARIAN_PENALTY_I'				, --VP
	'PROMOTION_BARBARIAN_PENALTY_II'			, --VP
	'PROMOTION_BARBARIAN_PENALTY_III'			, --VP
	'PROMOTION_EVASION_III'								, --VP
	'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY',
	'PROMOTION_WEAK_RANGED'							,
--	'PROMOTION_PRIZE_RULES'							,
--	'PROMOTION_EE_FIRST_RATE'							, --Enlightenment Era, flagship
--	'PROMOTION_EE_FIRST_RATE_DEFENSE'		, --Enlightenment Era, proximity to flagship
--	'PROMOTION_EE_FASIL_GHEBBI'						, --Enlightenment Era, proximity to wonder
	'PROMOTION_CREATED'									, -- CIV-Linked Great Generals
	'PROMOTION_EMISSARY'									, --CSD
	'PROMOTION_ENVOY'										, --CSD
	'PROMOTION_DIPLOMAT'									, --CSD
	'PROMOTION_AMBASSADOR'							, --CSD
	'PROMOTION_GREAT_MERCHANT'					, --CSD
	'PROMOTION_GREAT_DIPLOMAT'						, --CSD
--	'PROMOTION_NOBEL_LAUREATE'						, --sweden
	'PROMOTION_SIGNET'										,
	'PROMOTION_EXPRESS'									,
	'PROMOTION_LITERACY'									,
	'PROMOTION_IMMUNITY'									,
	'PROMOTION_PAX'						    				,
	'PROMOTION_NOBILITY'									,
--	'PROMOTION_CAN_MOVE_AFTER_ATTACKING'	, -- UndeadDevel: useful to distinguish late game scouts that have upgraded from Zeppelin and kept this promo
	'PROMOTION_SELL_EXOTIC_GOODS'				, --Portugal	
	'PROMOTION_WAR_CANOES'							, --songhai
	'PROMOTION_VIKING'										, --denmark
	'PROMOTION_LONGBOAT'								, --denmark
--	'PROMOTION_DOUBLE_PLUNDER'					, --Landsknechts; UndeadDevel: keep this visible to be able to easily see Landsknecht upgrade units
	'PROMOTION_MOVEMENT_TO_GENERAL'			, 
--	'PROMOTION_GENERAL_STACKING'   				,
	'PROMOTION_SILENT_HUNTER'    					,
--	'PROMOTION_ANTI_FIGHTER'    						, -- UndeadDevel: it's now choosable for fighters in VP
	'PROMOTION_FLAT_MOVEMENT_COST'			, 
	'PROMOTION_PARADROP'								, 
	'PROMOTION_ENSLAVEMENT'							, --Ramses
	'PROMOTION_GODDY_HUT_PICKER'				, --Shosho
	'PROMOTION_GOLDEN_AGE_POINTS'				, --Braz
	'PROMOTION_GAIN_EXPERIENCE'					, --oda,jap
	'PROMOTION_SPAWN_GENERALS_II'				,
	'PROMOTION_SPAWN_GENERALS_I'				,
	'PROMOTION_AIR_SWEEP'								,
	'PROMOTION_AIR_RECON'								,
	'PROMOTION_CAN_MOVE_IMPASSABLE'			,
	'PROMOTION_INVISIBLE_SUBMARINE'				,
	'PROMOTION_HOVERING_UNIT'						,
	'PROMOTION_WITHDRAW_BEFORE_MELEE'		,
	'PROMOTION_SEE_INVISIBLE_SUBMARINE'		,
	'PROMOTION_ANTI_SUBMARINE_II'					,	
	'PROMOTION_EXTENDED_PARADROP'				,
	'PROMOTION_GREAT_ADMIRAL'						,	
	'PROMOTION_RIVAL_TERRITORY'						,		
	'PROMOTION_UNWELCOME_EVANGELIST'		,		
	'PROMOTION_FALLOUT_REDUCTION'				,		
	'PROMOTION_FALLOUT_IMMUNITY'					,		
	'PROMOTION_ICE_BREAKERS'							, -- UndeadDevel: VP promo for Workers after a certain Tech
	'PROMOTION_EMBARKED_SIGHT'						,		
--	'PROMOTION_ANTI_TANK'								, -- is FreePromotion of several units
	'PROMOTION_ROUGH_TERRAIN_ENDS_TURN'	, --penalty
	'PROMOTION_ONLY_DEFENSIVE'						, --penalty
	'PROMOTION_NO_DEFENSIVE_BONUSES'		, --penalty
	'PROMOTION_MUST_SET_UP'							, --penalty
	'PROMOTION_CITY_SIEGE'								, --demolish
	'PROMOTION_CITY_ASSAULT'							, --demolish
	'PROMOTION_NEW_UNIT'								, --
	'PROMOTION_DEFENSE_1'								, --
	'PROMOTION_DEFENSE_2'								, --
	'PROMOTION_GREAT_GENERAL'						, --
	'PROMOTION_FREE_UPGRADES'						, --citystates
--	'PROMOTION_ANTI_CAVALRY'							, --Ottomans
--	'PROMOTION_MEDIC_GENERAL'						, --Mongolia
--	'PROMOTION_SENTRY'										, --America
	'PROMOTION_FREE_PILLAGE_MOVES'				, --Denmark
	'PROMOTION_OCEAN_MOVEMENT'					, --England
	'PROMOTION_OCEAN_IMPASSABLE'					, --Korea
	'PROMOTION_MUSTERED'								,
	'PROMOTION_FAST_ADMIRAL'							,
	'PROMOTION_INSTA_HEAL'                  			,
--    'PROMOTION_HONOR_BONUS'                 		, --from Authority...every unit will have it, so not a distinguishing feature and player will know about his opponent's social policy
--    'PROMOTION_KILL_HEAL'                   				, --from Authority...every melee unit will have it so not a distinguishing feature and very obvious and need to prevent clutter from this very common promo (same as Honor)
	'PROMOTION_STATUE_ZEUS'					  		  -- UndeadDevel: not super relevant, looks similar to more important promos like City Assault and thus confusing and everyone will know about the WWs of his opponent
	);
