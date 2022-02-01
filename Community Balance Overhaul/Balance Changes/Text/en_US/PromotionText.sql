 -- added Evasion Bomber Aircraft Promotion
	UPDATE Language_en_US
	SET Text = 'Reduces damage taken from Interception by 50%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_HELP';
 
 -- added Hovering Unit
	UPDATE Language_en_US
	SET Text = 'Unit may pass over Mountains.'
	WHERE Tag = 'TXT_KEY_PROMOTION_HOVERING_UNIT_HELP';

 -- added Withdraw Before Melee
	UPDATE Language_en_US
	SET Text = '80% chance to withdraw from Melee combat. Chance of withdrawal decreased against fast Enemy Units or if there are limited open tiles behind the Unit.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WITHDRAW_BEFORE_MELEE_HELP';

 -- added Great Himeji Castle
	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength when fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_HIMEJI_CASTLE_HELP';

 -- added Great Lighthouse Promotion
	UPDATE Language_en_US
	SET Text = '+1 Sight.[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_GREAT_LIGHTHOUSE_HELP';

 -- added Damaged Enemy Bonus (33)
	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Wounded Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_STRONGER_VS_DAMAGED_HELP';

 -- added Great General Combat Bonus
	UPDATE Language_en_US
	SET Text = '25% [ICON_STRENGTH] Combat Strength with Great General in same tile.'
	WHERE Tag = 'TXT_KEY_PROMOTION_GENERAL_STACKING_HELP';

 -- added Statue Of Zeus Promotion
	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_STATUE_ZEUS_HELP';

 -- added 1 Extra Movement
	UPDATE Language_en_US
	SET Text = '+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_XTRA_MOVES_I_HELP';

 -- added Mobility
	UPDATE Language_en_US
	SET Text = '+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MOBILITY_HELP';

 -- added Scouting III
	UPDATE Language_en_US
	SET Text = '+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SCOUTING_3_HELP';

 -- added Enhanced Flank Attack
	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 10%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_FLANK_ATTACK_BONUS_HELP';

 -- added Combat Bonus in forest/jungle (33) Mohawk Warrior
	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when fighting in Forest/Jungle.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MOHAWK_HELP';

-- added Combat Bonus in forest/jungle (33) Jaguar
	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when fighting in Forest/Jungle.'
	WHERE Tag = 'TXT_KEY_PROMOTION_JAGUAR_HELP';

	-- added Bonuses in Snow, Tundra, and Hills
	UPDATE Language_en_US
	SET Text = 'In Snow, Tundra, Hills: +25% [ICON_STRENGTH] Combat Strength (if Forest or Jungle is not present) and Double Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SKI_INFANTRY_HELP';

	-- added Expeditionary Warfare (Foreign Lands Bonus)
	UPDATE Language_en_US
	SET Text = 'Expeditionary Warfare'
	WHERE Tag = 'TXT_KEY_PROMOTION_FOREIGN_LANDS';;

	UPDATE Language_en_US
	SET Text = '+20% [ICON_STRENGTH] Combat Strength when fighting [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_FOREIGN_LANDS_HELP';

	-- added Homeland Guardian
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_HOMELAND_GUARDIAN_HELP';

	-- added Near Capital Bonus
	UPDATE Language_en_US
	SET Text = '+30% [ICON_STRENGTH] Combat Strength while fighting in the capital; bonus falls off as the Unit gets further away.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DEFEND_NEAR_CAPITAL_HELP';

	-- added Lightning Warfare
	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength when attacking.[NEWLINE]Ignores [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR].[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_LIGHTNING_WARFARE_HELP';

	-- added Nationalism
	UPDATE Language_en_US
	SET Text = '+20% [ICON_STRENGTH] Combat Strength when adjacent to or in a [COLOR_POSITIVE_TEXT]City[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_NATIONALISM_HELP';

	-- added Penalty Attacking Cities (25)
	UPDATE Language_en_US
	SET Text = '-25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_NEGATIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_SMALL_PROMOTION_CITY_PENALTY_HELP';

	-- added All tiles cost 1 Movement point
	UPDATE Language_en_US
	SET Text = 'All tiles cost 1 [ICON_MOVES] Movement point.'
	WHERE Tag = 'TXT_KEY_PROMOTION_FLAT_MOVEMENT_COST';

	-- added Penalty Attacking Cities	
	UPDATE Language_en_US
	SET Text = '-33% [ICON_STRENGTH] Combat Strength when attacking [COLOR_NEGATIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_PENALTY_HELP';

	-- added Bonus vs Mounted	
	UPDATE Language_en_US
	SET Text = '+50% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_MOUNTED_I_HELP';
	
-- Adjust Sortie
	
	UPDATE Language_en_US
	SET Text = '+34% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy Aircraft.[NEWLINE]+1 operational range.[NEWLINE]+1 [COLOR_POSITIVE_TEXT]Interception[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_SORTIE_HELP';
	
	UPDATE Language_en_US
	SET Text = 'Ace Pilot IV'
	WHERE Tag = 'TXT_KEY_PROMOTION_SORTIE';

-- March
	UPDATE Language_en_US
	SET Text = '-15% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]Unit will [COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even if it performs an action.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MARCH_HELP';

-- Replace Drill

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_1_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_2_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_3_HELP';

-- Replace Shock
	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_1_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_2_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_3_HELP';

-- Survivalism
	UPDATE Language_en_US
	SET Text = 'Unit will [COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even if it performs an action.[NEWLINE]Can pillage without expending movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_3_HELP';

-- Replace Accuracy

	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_2_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_3_HELP';

-- Replace Barrage with +5% Ranged Combat Strength and damage versus wounded units.

	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_2_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_3_HELP';

-- Woodsman, Ambush, Charge and Formation all get buffs in different terrains

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].[NEWLINE]+20% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Wounded Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength when defending in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE]Double movement rate [COLOR_POSITIVE_TEXT]Through Forest and Jungle[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOODSMAN_HELP';

	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when defending in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_1_HELP';

	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when defending in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_2_HELP';

	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE][NEWLINE]Available once you have researched [COLOR_CYAN]Combustion[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_1_HELP';

	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE][NEWLINE]Available once you have researched [COLOR_CYAN]Combustion[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_2_HELP';
	
-- Fix Morale tooltip
	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MORALE_HELP';

-- Adjust Anti-Tank
	UPDATE Language_en_US
	SET Text = 'Anti-Tank Rounds.'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_TANK';

	UPDATE Language_en_US
	SET Text = '+50% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_TANK';

	UPDATE Language_en_US
	SET Text = 'Anti-Tank Rounds'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_ANTI_TANK';
-- Charge now a two-part upgrade

	UPDATE Language_en_US
	SET Text = 'Charge I'
	WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE';

-- Cover now 25 and 25
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending against all [COLOR_POSITIVE_TEXT]Ranged Attacks[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_COVER_1_HELP';

	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending against all [COLOR_POSITIVE_TEXT]Ranged Attacks[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_COVER_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15% [ICON_STRENGTH] Combat Strength per [COLOR_POSITIVE_TEXT]Adjacent Friendly Unit[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_DISCIPLINE_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Naval and Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Naval and Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Naval and Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50 HP[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_3_HELP';

	-- Bombardment now specifically anti-City.
	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_3_HELP';

	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Inflicts the [COLOR_POSITIVE_TEXT]Boarded (I)[ENDCOLOR] status on enemies during melee combat, reducing their [ICON_MOVES] Movement by 2 for 1 Turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_1_HELP';

	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Immune to the [COLOR_POSITIVE_TEXT]Boarded[ENDCOLOR] effect from Naval Melee Units.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_2_HELP';

	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Inflicts the [COLOR_POSITIVE_TEXT]Boarded (II)[ENDCOLOR] status on enemies during melee combat, reducing their [ICON_MOVES] Movement by 4 for 1 Turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_3_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+10 Hit Points.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_1_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15 Hit Points.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_2_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15 Hit Points.'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_3_HELP';

	UPDATE Language_en_US
	SET Text = 'Dreadnought I'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_1';

	UPDATE Language_en_US
	SET Text = 'Dreadnought II'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_2';

	UPDATE Language_en_US
	SET Text = 'Dreadnought III'
	WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_3';

-- fixed Ranged Support Fire
	UPDATE Language_en_US
	SET Text = 'Before attacking [COLOR_POSITIVE_TEXT]Units[ENDCOLOR], performs a [COLOR_POSITIVE_TEXT]Ranged Attack[ENDCOLOR] at 50% [ICON_STRENGTH] Combat Strength. [COLOR_NEGATIVE_TEXT]Cannot Fortify.[ENDCOLOR][NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Lost on Upgrade.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_PROMOTION_RANGED_SUPPORT_FIRE_HELP';

-- fixed spear throw
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Gunpowder Units[ENDCOLOR].[NEWLINE]Before attacking [COLOR_POSITIVE_TEXT]Units[ENDCOLOR], performs a [COLOR_POSITIVE_TEXT]Ranged Attack[ENDCOLOR] at 50% [ICON_STRENGTH] Combat Strength. [COLOR_NEGATIVE_TEXT]Cannot Fortify.[ENDCOLOR][NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Lost on upgrade.[ENDCOLOR]'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_GUNPOWDER_HELP';

	
-- Wolfpack extremely strong

	UPDATE Language_en_US
	SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 Sight.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]40% chance to withdraw from Melee combat.'
	WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_3_HELP';

	UPDATE Language_en_US
	SET Text = 'Air Targeting I'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_1';

	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_1_HELP';

	UPDATE Language_en_US
	SET Text = 'Air Targeting II'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_2';


	UPDATE Language_en_US
	SET Text = '+15% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_2_HELP';

-- Reduce anti-sub power
	UPDATE Language_en_US
	SET Text = 'Bonus vs Submarines (33)'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_SUBMARINE_I';

	UPDATE Language_en_US
	SET Text = 'Bonus vs Submarines (33)'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_ANTI_SUBMARINE_I';

	UPDATE Language_en_US
	SET Text = 'Bonus vs Submarines (75)'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_SUBMARINE_II';

	UPDATE Language_en_US
	SET Text = 'Bonus vs Submarines (75)'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_ANTI_SUBMARINE_II';

-- Air Ambush - More Interceptions

	--INSERT INTO Language_en_US (Tag, Text) -- vanila uses the same TXT_KEY for both promos
	--SELECT 'TXT_KEY_PROMOTION_AIR_AMBUSH_2_HELP', Text FROM Language_en_US WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+50% [ICON_RANGE_STRENGTH] Strength VS [COLOR_POSITIVE_TEXT]Armored[ENDCOLOR] Units.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_1_HELP';

	--UPDATE Language_en_US
	--SET Text = '1 Extra [COLOR_POSITIVE_TEXT]Interception[ENDCOLOR] may be made per turn. +33% [ICON_RANGE_STRENGTH] Strength VS [COLOR_POSITIVE_TEXT]Armored[ENDCOLOR] Units.'
	--WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_2_HELP';
	INSERT INTO Language_en_US (Tag, Text) -- vanila uses March for this
	VALUES ('TXT_KEY_PROMOTION_AIR_AMBUSH_2_HELP', '1 Extra [COLOR_POSITIVE_TEXT]Interception[ENDCOLOR] may be made per turn. +25% [ICON_RANGE_STRENGTH] Strength VS [COLOR_POSITIVE_TEXT]Armored[ENDCOLOR] Units.');

	-- Bomber Siege
	UPDATE Language_en_US
	SET Text = '+50% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+50% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+50% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_3_HELP';

	-- City Assault
	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].[NEWLINE]Damage from Cities reduced by 33%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SIEGE_HELP';

	UPDATE Language_en_US
	SET Text = 'City Assault'
	WHERE Tag = 'TXT_KEY_PROMOTION_SIEGE';

	-- City Siege
	UPDATE Language_en_US
	SET Text = '+100% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR] (City Siege).'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_HELP';
	UPDATE Language_en_US
	SET Text = 'Bonus vs Cities (100)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE';
	UPDATE Language_en_US
	SET Text = 'Bonus vs Cities (100)'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_CITY_SIEGE';

	-- City Siege II
	UPDATE Language_en_US
	SET Text = '+100% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR] (City Siege II).'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_II_HELP';
	UPDATE Language_en_US
	SET Text = 'Bonus vs Cities (100)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_II';

	-- City Assault
	UPDATE Language_en_US
	SET Text = 'Bonus vs Cities (150)'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT';
	UPDATE Language_en_US
	SET Text = 'Bonus vs Cities (150)'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_CITY_ASSUALT';
	UPDATE Language_en_US
	SET Text = '+150% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR] (City Assault).'
	WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT_HELP';
 
	-- Siege
	UPDATE Language_en_US
	SET Text = 'City Assault (33)'
	WHERE Tag = 'TXT_KEY_PROMOTION_SIEGE';

	-- Armor Plating

	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending.'
	WHERE Tag = 'TXT_KEY_PROMOTION_ARMOR_PLATING_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending.'
	WHERE Tag = 'TXT_KEY_PROMOTION_ARMOR_PLATING_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending.'
	WHERE Tag = 'TXT_KEY_PROMOTION_ARMOR_PLATING_3_HELP';

	-- Volley
	UPDATE Language_en_US
	SET Text = '+50% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Fortified Units[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_VOLLEY_HELP';

	-- Survivalism
	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]+5 HP healed per turn [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_1_HELP';

	UPDATE Language_en_US
	SET Text = '+25% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]+5 HP healed per turn [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_2_HELP';

	-- Amphibious Change

	UPDATE Language_en_US
	SET Text = 'Eliminates combat penalties for attacking from the sea or over a river, and grants a movement bonus when moving across rivers.'
	WHERE Tag = 'TXT_KEY_PROMOTION_AMPHIBIOUS_HELP';

	-- Medic I/II
	UPDATE Language_en_US
	SET Text = '-10% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]This Unit and all Units in adjacent tiles [COLOR_POSITIVE_TEXT]Heal 5 Additional HP[ENDCOLOR] per turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_HELP';

	UPDATE Language_en_US
	SET Text = '-10% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]This Unit and all Units in adjacent tiles [COLOR_POSITIVE_TEXT]Heal 5 Additional HP[ENDCOLOR] per turn.[NEWLINE]This Unit [COLOR_POSITIVE_TEXT]Heals 5 Additional HP[ENDCOLOR] per turn when [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_II_HELP';

	UPDATE Language_en_US
	SET Text = 'Cannot end turn on an Ocean tile until you have researched [COLOR_POSITIVE_TEXT]Compass[ENDCOLOR].[NEWLINE]May move through Ocean tiles if ending on a Coastal tile.'
	WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';
	
	UPDATE Language_en_US
	SET Text = 'Compass Needed'
	WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO';
	
	-- Text change for Khan ability
	UPDATE Language_en_US
	SET Text = 'Units in this tile and in adjacent tiles [COLOR_POSITIVE_TEXT]Heal 10 Additional HP[ENDCOLOR] per turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_GENERAL_HELP';

	-- Fix Anti-Air Promotion Info
	UPDATE Language_en_US
	SET Text = 'Air Supremacy I'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR';

	UPDATE Language_en_US
	SET Text = 'Air Supremacy II'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR_II';

	-- Second Attack Explanations
	UPDATE Language_en_US
	SET Text = '-20% [ICON_STRENGTH] Combat Strength.[NEWLINE]May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_SECOND_ATTACK';

	UPDATE Language_en_US
	SET Text = '-30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].'
	WHERE Tag = 'TXT_KEY_PROMOTION_LOGISTICS_HELP';

	UPDATE Language_en_US
	SET Text = 'May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_RESTLESSNESS_HELP';

	-- Indirect Fire, Blitz, Range Combat Strength Reductions

	UPDATE Language_en_US
	SET Text = '-10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]Ranged attacks may be performed over obstacles (as long as other friendly Units can see the target).'
	WHERE Tag = 'TXT_KEY_PROMOTION_INDIRECT_FIRE_HELP';

	UPDATE Language_en_US
	SET Text = 'Unit may [COLOR_POSITIVE_TEXT]Attack Multiple Times[ENDCOLOR] per turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BLITZ_HELP';

	UPDATE Language_en_US
	SET Text = '-20% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 Range.'
	WHERE Tag = 'TXT_KEY_PROMOTION_RANGE_HELP';

	-- Changed English UA
	UPDATE Language_en_US
	SET Text = '1 Extra Movement for Naval Units.'
	WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_MOVEMENT_HELP';

	-- Bonus vs Naval
	UPDATE Language_en_US
	SET Text = 'Bonus vs Naval (50)'
	WHERE Tag = 'TXT_KEY_PROMOTION_NAVAL_BONUS';

	-- Supply
	UPDATE Language_en_US
	SET Text = 'May heal damage [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].[NEWLINE]+5 HP healed per turn.'
	WHERE Tag = 'TXT_KEY_PROMOTION_SUPPLY_HELP';
	
	-- Air Repair
	INSERT INTO Language_en_US (Tag, Text) -- vanila uses March for this
	VALUES ('TXT_KEY_PROMOTION_AIR_REPAIR_HELP', 'Unit will [COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even if it performs an action.');
	
	-- Air Logistics
	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_AIR_LOGISTICS', 'Air Logistics');
	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_AIR_LOGISTICS_HELP', 'May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].');
	
	UPDATE Language_en_US
	SET Text = 'Tank Hunter I'
	WHERE Tag = 'TXT_KEY_PROMOTION_HELI_AMBUSH_1';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_HELI_AMBUSH_1_HELP', '+25% [ICON_STRENGTH] Combat Strength VS Armored Units.');

	UPDATE Language_en_US
	SET Text = 'Tank Hunter II'
	WHERE Tag = 'TXT_KEY_PROMOTION_HELI_AMBUSH_2';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_HELI_AMBUSH_2_HELP', '+25% [ICON_STRENGTH] Combat Strength VS Armored Units.');

	UPDATE Language_en_US
	SET Text = 'Eagle Hunter'
	WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_FIGHTER';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_ANTI_FIGHTER_HELP', '33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Air Fighters[ENDCOLOR].');

	UPDATE Language_en_US
	SET Text = 'Evasion'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION';

	-- Evasion
	UPDATE Language_en_US
	SET Text = 'Air Penetration I'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_I';

	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]33%[ENDCOLOR] Chance to Evade Air Interception'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_I_HELP';

	UPDATE Language_en_US
	SET Text = 'Air Penetration I'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_EVASION_I';

	UPDATE Language_en_US
	SET Text = 'Air Penetration II'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_II';

	UPDATE Language_en_US
	SET Text = '[COLOR_POSITIVE_TEXT]33%[ENDCOLOR] Chance to Evade Air Interception'
	WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_II_HELP';

	UPDATE Language_en_US
	SET Text = 'Air Penetration II'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_EVASION_II';

	-- Repair
	UPDATE Language_en_US
	SET Text = 'Unit will [COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even if it performs an action.'
	WHERE Tag = 'TXT_KEY_PROMOTION_REPAIR_HELP';

	-- Embarkation
	UPDATE Language_en_US
	SET Text = 'Can embark onto [COLOR_POSITIVE_TEXT]Water[ENDCOLOR] tiles.[NEWLINE]Embarked Units can enter [COLOR_POSITIVE_TEXT]Ocean[ENDCOLOR] tiles and have +2 Sight.'
	WHERE Tag = 'TXT_KEY_PROMOTION_ALLWATER_EMBARKATION_HELP';
	
	-- Golden Age Points from Kills
	UPDATE Language_en_US
	SET Text = 'Earn [ICON_GOLDEN_AGE] Golden Age Points for each Enemy killed.'
	WHERE Tag = 'TXT_KEY_PROMOTION_GOLDEN_AGE_POINTS_HELP';
	
	-- Naval Leadership
	UPDATE Language_en_US
	SET Text = 'May use the [COLOR_POSITIVE_TEXT]Repair Fleet[ENDCOLOR] Ability once before being expended.[NEWLINE]Improves combat effectiveness of nearby naval Units.[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_GREAT_ADMIRAL_HELP';

	UPDATE Language_en_US
	SET Text = '+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_NAVAL_TRADITION_HELP';

	-- Haka
	UPDATE Language_en_US
	SET Text = 'Enemy Units receive -15% [ICON_STRENGTH] Combat Strength when adjacent to any Unit with this promotion.'
	WHERE Tag = 'TXT_KEY_PROMOTION_HAKA_WAR_DANCE_HELP';

	-- Adjusted Buffalo stuff

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 33%.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_HORNS_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+25 Hit Points.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_CHEST_HELP';

	UPDATE Language_en_US
	SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+1 [ICON_MOVES] Movement.'
	WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_LOINS_HELP';


	-- Correct "Heals at Double Rate" from Persia/Immortal
	UPDATE Language_en_US
	SET Text = 'Faster Healing'
	WHERE Tag = 'TXT_KEY_PROMOTION_FASTER_HEAL';

	UPDATE Language_en_US
	SET Text = 'Faster Healing'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_FASTER_HEAL';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_FASTER_HEAL_HELP', 
	'This Unit [COLOR_POSITIVE_TEXT]Heals 10 Additional HP[ENDCOLOR] per turn.');

	UPDATE UnitPromotions
	SET Help = 'TXT_KEY_PROMOTION_FASTER_HEAL_HELP'
	WHERE Type = 'PROMOTION_FASTER_HEAL';

	-- Interception
	UPDATE Language_en_US
	SET Text = 'Interceptor I'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_I';

	UPDATE Language_en_US
	SET Text = 'Interceptor I'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_INTERCEPTION_I';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_INTERCEPTION_I_HELP', 
	'+15% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.');

	UPDATE Language_en_US
	SET Text = 'Interceptor II'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_II';

	UPDATE Language_en_US
	SET Text = 'Interceptor II'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_INTERCEPTION_II';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_INTERCEPTION_II_HELP', 
	'+15% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.');

	UPDATE Language_en_US
	SET Text = 'Interceptor III'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_III';

	UPDATE Language_en_US
	SET Text = 'Interceptor III'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_INTERCEPTION_III';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_INTERCEPTION_III_HELP', 
	'+15% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.');

	UPDATE Language_en_US
	SET Text = 'Interceptor IV'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_IV';

	UPDATE Language_en_US
	SET Text = 'Interceptor IV'
	WHERE Tag = 'TXT_KEY_PEDIA_PROMOTION_INTERCEPTION_IV';

	INSERT INTO Language_en_US (Tag, Text)
	VALUES ('TXT_KEY_PROMOTION_INTERCEPTION_IV_HELP', 
	'+15% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.');


	UPDATE Language_en_US
	SET Text = 'Ace Pilot I'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_1';

	UPDATE Language_en_US
	SET Text = 'Ace Pilot II'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_2';

	UPDATE Language_en_US
	SET Text = 'Ace Pilot III'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_3';

	UPDATE Language_en_US
	SET Text = '+33% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+33% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.[NEWLINE]+33% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy aircraft.'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+34% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.[NEWLINE]+33% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy aircraft. [NEWLINE]+1 operational range.'
	WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_3_HELP';
	

	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]5 Damage[ENDCOLOR] to all Aircraft Units adjacent to Air Sweep target.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_1_HELP';
	
	UPDATE Language_en_US
	SET Text = '+33% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]10 Damage[ENDCOLOR] to all Aircraft Units adjacent to Air Sweep target.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_2_HELP';
	
	UPDATE Language_en_US
	SET Text = '+34% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]15 Damage[ENDCOLOR] to all Aircraft Units adjacent to Air Sweep target.'
	WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_3_HELP';