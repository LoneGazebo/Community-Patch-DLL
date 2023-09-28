-- Hovering Unit
UPDATE Language_en_US
SET Text = 'Unit may pass over Mountains, and may cross Water tiles without Embarking. Cannot attack in [COLOR_NEGATIVE_TEXT]Ocean tiles[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_HOVERING_UNIT_HELP';

-- Withdraw Before Melee
UPDATE Language_en_US
SET Text = 'Unit withdraws from the first Melee attack each turn if there are open tiles behind the Unit'
WHERE Tag = 'TXT_KEY_PROMOTION_WITHDRAW_BEFORE_MELEE_HELP';

-- Himeji Castle
UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength when fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_HIMEJI_CASTLE_HELP';

UPDATE Language_en_US
SET Text = 'Marksmanship'
WHERE Tag = 'TXT_KEY_PROMOTION_STRONGER_VS_DAMAGED';

UPDATE Language_en_US
SET Text = '+33% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Wounded Units[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_STRONGER_VS_DAMAGED_HELP';

-- Great General Combat Bonus
UPDATE Language_en_US
SET Text = '25% [ICON_STRENGTH] Combat Strength with Great General in same tile.'
WHERE Tag = 'TXT_KEY_PROMOTION_GENERAL_STACKING_HELP';

-- Statue Of Zeus
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Military Units:[ENDCOLOR] +25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].[NEWLINE][COLOR_POSITIVE_TEXT]Workers:[ENDCOLOR] Immune to [COLOR_NEGATIVE_TEXT]Prisoners of War[ENDCOLOR] Work Rate reduction.'
WHERE Tag = 'TXT_KEY_PROMOTION_STATUE_ZEUS_HELP';

-- Mobility, Mobility I, Mobility II
UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_MOBILITY_HELP';

-- Scouting
UPDATE Language_en_US
SET Text = '+1 [ICON_VP_VISION] Sight.'
WHERE Tag = 'TXT_KEY_PROMOTION_SCOUTING_1_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_VP_VISION] Sight.'
WHERE Tag = 'TXT_KEY_PROMOTION_SCOUTING_2_HELP';

UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_SCOUTING_3_HELP';

-- Sentry
UPDATE Language_en_US
SET Text = '+1 [ICON_VP_VISION] Sight.'
WHERE Tag = 'TXT_KEY_PROMOTION_SENTRY_HELP';

-- Enhanced Flank Attack
UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 10%.'
WHERE Tag = 'TXT_KEY_PROMOTION_FLANK_ATTACK_BONUS_HELP';

-- Combat Bonus in Forest/Jungle (33)
UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength when fighting in Forest/Jungle.'
WHERE Tag = 'TXT_KEY_PROMOTION_MOHAWK_HELP';

-- Combat Bonus in Forest/Jungle (33)
UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength when fighting in Forest/Jungle.'
WHERE Tag = 'TXT_KEY_PROMOTION_JAGUAR_HELP';

UPDATE Language_en_US
SET Text = 'Highlanders'
WHERE Tag = 'TXT_KEY_PROMOTION_SKI_INFANTRY';

UPDATE Language_en_US
SET Text = 'Double [ICON_MOVES] Movement in Snow, Tundra, and Hill Tiles.[NEWLINE]+25% [ICON_STRENGTH] Combat Strength in Snow, Tundra, and Hill Tiles without Forest or Jungle.'
WHERE Tag = 'TXT_KEY_PROMOTION_SKI_INFANTRY_HELP';

UPDATE Language_en_US
SET Text = 'Expeditionary Warfare'
WHERE Tag = 'TXT_KEY_PROMOTION_FOREIGN_LANDS';

UPDATE Language_en_US
SET Text = '+20% [ICON_STRENGTH] Combat Strength when fighting [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_FOREIGN_LANDS_HELP';

-- Homeland Guardian
UPDATE Language_en_US
SET Text = '+25% [ICON_STRENGTH] Combat Strength when fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_HOMELAND_GUARDIAN_HELP';

-- Full Moon Striker
UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement.[NEWLINE]Gain 200 [ICON_RESEARCH] Science when pillaging tiles.'
WHERE Tag = 'TXT_KEY_PROMOTION_MOON_STRIKER_HELP';

-- Pride of the Ancestors
UPDATE Language_en_US
SET Text = '+20% [ICON_STRENGTH] Combat Strength when fighting in [COLOR_POSITIVE_TEXT]Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_HOMELAND_GUARDIAN_BOOGALOO_HELP';

-- Near Capital Bonus
UPDATE Language_en_US
SET Text = '+30% [ICON_STRENGTH] Combat Strength while fighting in the capital; bonus falls off as the Unit gets further away.'
WHERE Tag = 'TXT_KEY_PROMOTION_DEFEND_NEAR_CAPITAL_HELP';

-- Lightning Warfare
UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength when attacking.[NEWLINE]Ignores [COLOR_POSITIVE_TEXT]Zone of Control[ENDCOLOR].[NEWLINE]+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_LIGHTNING_WARFARE_HELP';

-- Patriotic War
UPDATE Language_en_US
SET Text = '+20% [ICON_STRENGTH] Combat Strength when adjacent to or in a [COLOR_POSITIVE_TEXT]City[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_NATIONALISM_HELP';

-- Penalty Attacking Cities (25)
UPDATE Language_en_US
SET Text = '-25% [ICON_STRENGTH] Combat Strength when attacking [COLOR_NEGATIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_SMALL_PROMOTION_CITY_PENALTY_HELP';

-- Penalty Attacking Cities (33)
UPDATE Language_en_US
SET Text = '-33% [ICON_STRENGTH] Combat Strength when attacking [COLOR_NEGATIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_CITY_PENALTY_HELP';

UPDATE Language_en_US
SET Text = 'All Tiles cost 1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_FLAT_MOVEMENT_COST';

-- Bonus vs Mounted (50)
UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_MOUNTED_I_HELP';

-- Enhanced Medic
UPDATE Language_en_US
SET Text = 'Friendly Units in this tile and in adjacent tiles [COLOR_POSITIVE_TEXT]Heal +10 HP[ENDCOLOR] per turn.[NEWLINE]Adjacent Enemy Units take 10 Damage Each Turn.'
WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_GENERAL_HELP';

-- Trade Mission Bonus
UPDATE Language_en_US
SET Text = '+100% more [ICON_GOLD] Gold from Trade Missions.'
WHERE Tag = 'TXT_KEY_PROMOTION_TRADE_MISSION_BONUS_HELP';

UPDATE Language_en_US
SET Text = 'Ace Pilot IV'
WHERE Tag = 'TXT_KEY_PROMOTION_SORTIE';

UPDATE Language_en_US
SET Text = '+34% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy Aircraft.[NEWLINE]+1 operational range.[NEWLINE]+1 [COLOR_POSITIVE_TEXT]Interception[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_SORTIE_HELP';

-- March
UPDATE Language_en_US
SET Text = 'Unit will [COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even if it performs an action.'
WHERE Tag = 'TXT_KEY_PROMOTION_MARCH_HELP';

-- Drill
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]Damage from Cities reduced by 15%.'
WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]Damage from Cities reduced by 15%.'
WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]Damage from Cities reduced by 15%.'
WHERE Tag = 'TXT_KEY_PROMOTION_DRILL_3_HELP';

-- Shock
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 5%.'
WHERE Tag = 'TXT_KEY_PROMOTION_SHOCK_3_HELP';

-- Accuracy
UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_ACCURACY_3_HELP';

-- Barrage
UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units above 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BARRAGE_3_HELP';

-- Woodsman
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE]Double [ICON_MOVES] Movement [COLOR_POSITIVE_TEXT]in Forest and Jungle[ENDCOLOR] Tiles.'
WHERE Tag = 'TXT_KEY_PROMOTION_WOODSMAN_HELP';

-- Formation
UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when defending in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_1_HELP';

UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Mounted Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when defending in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_FORMATION_2_HELP';

UPDATE Language_en_US
SET Text = 'Charge I'
WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Open Terrain[ENDCOLOR].[NEWLINE]+20% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Wounded Units[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_CHARGE_HELP';

-- Ambush
UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE][NEWLINE]Available once you have researched [COLOR_CYAN]Combustion[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_1_HELP';

UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].[NEWLINE]+15% [ICON_STRENGTH] Combat Strength when attacking in [COLOR_POSITIVE_TEXT]Rough Terrain[ENDCOLOR].[NEWLINE][NEWLINE]Available once you have researched [COLOR_CYAN]Combustion[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AMBUSH_2_HELP';

UPDATE Language_en_US
SET Text = 'Open Terrain Bonus (30)'
WHERE Tag = 'TXT_KEY_PROMOTION_OPEN_TERRAIN';

-- Morale 
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.'
WHERE Tag = 'TXT_KEY_PROMOTION_MORALE_HELP';

UPDATE Language_en_US
SET Text = 'Anti-Tank Rounds'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_TANK';

-- Cover
UPDATE Language_en_US
SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending against all [COLOR_POSITIVE_TEXT]Ranged Attacks[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_COVER_1_HELP';

UPDATE Language_en_US
SET Text = '+25% [ICON_STRENGTH] Combat Strength when defending against all [COLOR_POSITIVE_TEXT]Ranged Attacks[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_COVER_2_HELP';

UPDATE Language_en_US
SET Text = 'Unity'
WHERE Tag = 'TXT_KEY_PROMOTION_DISCIPLINE';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15% [ICON_STRENGTH] Combat Strength per [COLOR_POSITIVE_TEXT]Adjacent Friendly Land Unit[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_DISCIPLINE_HELP';

-- Targeting
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].[NEWLINE]+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Units at or below 50% HP[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_TARGETING_3_HELP';

-- Bombardment
UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_1_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_2_HELP';

UPDATE Language_en_US
SET Text = '+10% [ICON_RANGE_STRENGTH] Ranged Combat Strength.[NEWLINE]+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_BOMBARDMENT_3_HELP';

-- Boarding Party
UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Inflicts the [COLOR_POSITIVE_TEXT]Boarded (I)[ENDCOLOR] status on Enemy Units during melee combat, reducing their [ICON_MOVES] Movement by 2 for 1 Turn.'
WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_1_HELP';

UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Immune to the [COLOR_POSITIVE_TEXT]Boarded[ENDCOLOR] effect from Naval Melee Units.'
WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_2_HELP';

UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength.[NEWLINE]Inflicts the [COLOR_POSITIVE_TEXT]Boarded (II)[ENDCOLOR] status on Enemy Units during melee combat, reducing their [ICON_MOVES] Movement by 4 for 1 Turn.'
WHERE Tag = 'TXT_KEY_PROMOTION_BOARDING_PARTY_3_HELP';

UPDATE Language_en_US
SET Text = 'Hull I'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_1';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+10 Hit Points.'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_1_HELP';

UPDATE Language_en_US
SET Text = 'Hull II'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_2';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15 Hit Points.'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_2_HELP';

UPDATE Language_en_US
SET Text = 'Hull III'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_3';

UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+15 Hit Points.'
WHERE Tag = 'TXT_KEY_PROMOTION_COASTAL_RAIDER_3_HELP';

-- Ranged Support Fire
UPDATE Language_en_US
SET Text = 'Before attacking [COLOR_POSITIVE_TEXT]Units[ENDCOLOR], performs a [COLOR_POSITIVE_TEXT]Ranged Attack[ENDCOLOR] at 50% [ICON_STRENGTH] Combat Strength. [NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Lost on Upgrade.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_PROMOTION_RANGED_SUPPORT_FIRE_HELP';

-- Wolfpack
UPDATE Language_en_US
SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 [ICON_VP_VISION] Sight.'
WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_1_HELP';

UPDATE Language_en_US
SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_2_HELP';

UPDATE Language_en_US
SET Text = '+30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]Unit withdraws from the first Melee attack each turn if possible.'
WHERE Tag = 'TXT_KEY_PROMOTION_WOLFPACK_3_HELP';

UPDATE Language_en_US
SET Text = 'Air Targeting I'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_1';

UPDATE Language_en_US
SET Text = 'Air Targeting II'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_2';

UPDATE Language_en_US
SET Text = '+15% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Naval[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Land Units[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_TARGETING_1_HELP';

UPDATE Language_en_US
SET Text = 'Depth Charges I'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_SUBMARINE_I';

UPDATE Language_en_US
SET Text = 'Depth Charges II'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_SUBMARINE_II';

-- Air Ambush I, Air Ambush II
UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength VS [COLOR_POSITIVE_TEXT]Armored Units[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_AMBUSH_1_HELP';

-- Siege (Air)
UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_1_HELP';

UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_2_HELP';

UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SIEGE_3_HELP';

UPDATE Language_en_US
SET Text = 'Forlorn Hope'
WHERE Tag = 'TXT_KEY_PROMOTION_SIEGE';

UPDATE Language_en_US
SET Text = '-10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+100% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_SIEGE_HELP';

UPDATE Language_en_US
SET Text = 'Siege Engine'
WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE';

UPDATE Language_en_US
SET Text = '+100% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_CITY_SIEGE_HELP';

UPDATE Language_en_US
SET Text = 'Skeleton Key'
WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT';

UPDATE Language_en_US
SET Text = '+150% [ICON_STRENGTH] Combat Strength when attacking [COLOR_POSITIVE_TEXT]Cities[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_CITY_ASSUALT_HELP';

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
SET Text = '+25% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]Heal +5 HP per turn [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_1_HELP';

UPDATE Language_en_US
SET Text = '+25% [ICON_STRENGTH] Combat Strength while defending.[NEWLINE]Heal +5 HP per turn [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_2_HELP';

UPDATE Language_en_US
SET Text = '[COLOR_POSITIVE_TEXT]Heal Every Turn[ENDCOLOR], even after performing an action.[NEWLINE]Pillaging costs no [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_SURVIVALISM_3_HELP';

-- Amphibious
UPDATE Language_en_US
SET Text = 'Eliminates combat penalties for attacking from the sea or over a river. Crossing a river costs only 1 [ICON_MOVES] Movement. Double [ICON_MOVES] Movement in Marsh tiles.'
WHERE Tag = 'TXT_KEY_PROMOTION_AMPHIBIOUS_HELP';

-- Medic
UPDATE Language_en_US
SET Text = 'This Unit and all Units in adjacent tiles [COLOR_POSITIVE_TEXT]Heal +5 HP[ENDCOLOR] per turn.'
WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_HELP';

-- Medic II
UPDATE Language_en_US
SET Text = 'This Unit and all Units in adjacent tiles [COLOR_POSITIVE_TEXT]Heal +5 HP[ENDCOLOR] per turn.[NEWLINE]This Unit [COLOR_POSITIVE_TEXT]Heals +5 HP[ENDCOLOR] per turn when [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_MEDIC_II_HELP';

UPDATE Language_en_US
SET Text = 'Tenacity'
WHERE Tag = 'TXT_KEY_PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY';

UPDATE Language_en_US
SET Text = 'Domestic Focus'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO';

UPDATE Language_en_US
SET Text = 'Cannot end turn on an Ocean Tile until you have researched [COLOR_POSITIVE_TEXT]Compass[ENDCOLOR].[NEWLINE]May move through visible Ocean Tiles if ending on a visible Coast Tile.'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE_ASTRO_HELP';

UPDATE Language_en_US
SET Text = 'Oceanic Perils'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_IMPASSABLE';

UPDATE Language_en_US
SET Text = 'Air Supremacy I'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR';

UPDATE Language_en_US
SET Text = 'Air Supremacy II'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_AIR_II';

-- Logistics
UPDATE Language_en_US
SET Text = '-30% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_LOGISTICS_HELP';

-- Restlessness
UPDATE Language_en_US
SET Text = 'May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR].[NEWLINE]+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_RESTLESSNESS_HELP';

-- Blitz
UPDATE Language_en_US
SET Text = 'May [COLOR_POSITIVE_TEXT]Attack Twice[ENDCOLOR], and can move after attacking.'
WHERE Tag = 'TXT_KEY_PROMOTION_BLITZ_HELP';

-- Indirect Fire
UPDATE Language_en_US
SET Text = '-10% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]Ranged attacks may be performed over obstacles (as long as other friendly Units can see the target).'
WHERE Tag = 'TXT_KEY_PROMOTION_INDIRECT_FIRE_HELP';

-- Range
UPDATE Language_en_US
SET Text = '-20% [ICON_RANGE_STRENGTH] Ranged Combat Strength when attacking.[NEWLINE]+1 Range.'
WHERE Tag = 'TXT_KEY_PROMOTION_RANGE_HELP';

-- Heavy Charge
UPDATE Language_en_US
SET Text = '+50% [ICON_STRENGTH] Combat Strength when attacking if the defender cannot retreat. The defender is forced to retreat (if possible) if it has lower [ICON_STRENGTH] Combat Strength than this Unit after all modifiers.'
WHERE Tag = 'TXT_KEY_PROMOTION_HEAVY_CHARGE_HELP';

-- Naval Movement
UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement for Naval Units.'
WHERE Tag = 'TXT_KEY_PROMOTION_OCEAN_MOVEMENT_HELP';

-- Supply
UPDATE Language_en_US
SET Text = 'May heal damage [COLOR_POSITIVE_TEXT]Outside of Friendly Territory[ENDCOLOR].[NEWLINE]Heal +5 HP per turn.'
WHERE Tag = 'TXT_KEY_PROMOTION_SUPPLY_HELP';

UPDATE Language_en_US
SET Text = 'Tank Hunter I'
WHERE Tag = 'TXT_KEY_PROMOTION_HELI_AMBUSH_1';

UPDATE Language_en_US
SET Text = 'Tank Hunter II'
WHERE Tag = 'TXT_KEY_PROMOTION_HELI_AMBUSH_2';

UPDATE Language_en_US
SET Text = 'Bonus vs Helicopters (150)'
WHERE Tag = 'TXT_KEY_PROMOTION_ANTI_HELICOPTER';

UPDATE Language_en_US
SET Text = 'Evasion'
WHERE Tag = 'TXT_KEY_PROMOTION_EVASION';

UPDATE Language_en_US
SET Text = '-50% Damage received while performing an [COLOR_POSITIVE_TEXT]Air Strike[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_HELP';

UPDATE Language_en_US
SET Text = 'Air Penetration I'
WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_I';

UPDATE Language_en_US
SET Text = 'Air Penetration II'
WHERE Tag = 'TXT_KEY_PROMOTION_EVASION_II';

-- Golden Age from Victories
UPDATE Language_en_US
SET Text = 'Gain 100% of the [ICON_STRENGTH] Combat Strength of defeated Enemy Units as [ICON_GOLDEN_AGE] Golden Age Points.'
WHERE Tag = 'TXT_KEY_PROMOTION_GOLDEN_AGE_POINTS_HELP';

-- Naval Leadership
UPDATE Language_en_US
SET Text = 'May use the [COLOR_POSITIVE_TEXT]Repair Fleet[ENDCOLOR] Ability once before being expended.[NEWLINE]Improves combat effectiveness of nearby Naval Units.'
WHERE Tag = 'TXT_KEY_PROMOTION_GREAT_ADMIRAL_HELP';

-- Exploration
UPDATE Language_en_US
SET Text = '+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_NAVAL_TRADITION_HELP';

-- Haka War Dance
UPDATE Language_en_US
SET Text = 'Enemy Units receive -15% [ICON_STRENGTH] Combat Strength when adjacent to any Unit with this promotion.'
WHERE Tag = 'TXT_KEY_PROMOTION_HAKA_WAR_DANCE_HELP';

-- Buffalo Chest
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+25 Hit Points.'
WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_CHEST_HELP';

-- Buffalo Horns
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE][COLOR_POSITIVE_TEXT]Flanking bonus[ENDCOLOR] increased by 25%.'
WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_HORNS_HELP';

-- Buffalo Loins
UPDATE Language_en_US
SET Text = '+10% [ICON_STRENGTH] Combat Strength.[NEWLINE]+1 [ICON_MOVES] Movement.'
WHERE Tag = 'TXT_KEY_PROMOTION_BUFFALO_LOINS_HELP';

UPDATE Language_en_US
SET Text = 'Faster Healing'
WHERE Tag = 'TXT_KEY_PROMOTION_FASTER_HEAL';

UPDATE Language_en_US
SET Text = 'Interceptor I'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_I';

UPDATE Language_en_US
SET Text = 'Interceptor II'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_II';

UPDATE Language_en_US
SET Text = 'Interceptor III'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_III';

UPDATE Language_en_US
SET Text = 'Interceptor IV'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_IV';

UPDATE Language_en_US
SET Text = 'Ace Pilot I'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_1';

UPDATE Language_en_US
SET Text = '+33% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_1_HELP';

UPDATE Language_en_US
SET Text = 'Ace Pilot II'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_2';

UPDATE Language_en_US
SET Text = '+33% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.[NEWLINE]+33% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy aircraft.'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_2_HELP';

UPDATE Language_en_US
SET Text = 'Ace Pilot III'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_3';

UPDATE Language_en_US
SET Text = '+34% Chance to [COLOR_POSITIVE_TEXT]Intercept[ENDCOLOR] Enemy Air Units.[NEWLINE]+33% [ICON_STRENGTH] Combat Strength when [COLOR_POSITIVE_TEXT]Intercepting[ENDCOLOR] Enemy aircraft. [NEWLINE]+1 operational range.'
WHERE Tag = 'TXT_KEY_PROMOTION_INTERCEPTION_3_HELP';

-- Air Sweep
UPDATE Language_en_US
SET Text = 'Can Air Sweep to clear intercepting Units near the target tile.'
WHERE Tag = 'TXT_KEY_PROMOTION_AIR_SWEEP_HELP';

-- Dogfighting
UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]5 Damage[ENDCOLOR] to all Aircraft Units on or adjacent to Air Sweep target.'
WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_1_HELP';

UPDATE Language_en_US
SET Text = '+33% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]10 Damage[ENDCOLOR] to all Aircraft Units on or adjacent to Air Sweep target.'
WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_2_HELP';

UPDATE Language_en_US
SET Text = '+34% [ICON_STRENGTH] Combat Strength when performing an [COLOR_POSITIVE_TEXT]Air Sweep[ENDCOLOR]. If this Air Sweep is not Intercepted, deal [COLOR_POSITIVE_TEXT]15 Damage[ENDCOLOR] to all Aircraft Units on or adjacent to Air Sweep target.'
WHERE Tag = 'TXT_KEY_PROMOTION_DOGFIGHTING_3_HELP';

-- Embarkation with Defense
UPDATE Language_en_US
SET Text = '+100% [ICON_STRENGTH] Combat Strength when defending while Embarked.'
WHERE Tag = 'TXT_KEY_PROMOTION_DEFENSIVE_EMBARKATION_HELP';

-- Sapper
UPDATE Language_en_US
SET Text = 'When next to an enemy City, other units within 2 tiles get +40% [ICON_STRENGTH] Combat Strength when attacking the City.[NEWLINE]If 2 tiles from an enemy City, other units within 2 tiles get +20% [ICON_STRENGTH] Combat Strength when attacking the City.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Bonuses do not stack.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_PROMOTION_SAPPER_HELP';

-- Exotic Cargo
UPDATE Language_en_US
SET Text = 'May Sell Exotic Goods when adjacent to City-State lands to gain [ICON_GOLD] Gold and XP based on distance from capital.[NEWLINE]Each Nau can perform this action twice.[NEWLINE][NEWLINE]If possible, when a Nau sells its Exotic Cargo, a Feitoria is also [COLOR_POSITIVE_TEXT]automatically created[ENDCOLOR] in its territory.'
WHERE Tag = 'TXT_KEY_PROMOTION_SELL_EXOTIC_GOODS_HELP';
