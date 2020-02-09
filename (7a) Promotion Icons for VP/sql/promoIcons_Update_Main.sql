-- rev.03


-- Temporary Atlas

/*
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- White Triangle + 1 Yellow Bar
UPDATE UnitPromotions SET PortraitIndex = '0' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- White Triangle + 2 Yellow Bars
UPDATE UnitPromotions SET PortraitIndex = '1' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- White Triangle + 3 Yellow Bars
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- White Triangle + 3 Yellow Stars
UPDATE UnitPromotions SET PortraitIndex = '3' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- 2 White Triangles
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 3 White Bars
UPDATE UnitPromotions SET PortraitIndex = '5' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 1 Yellow Horns
UPDATE UnitPromotions SET PortraitIndex = '6' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle like a inclined star
UPDATE UnitPromotions SET PortraitIndex = '7' WHERE Type = '';

UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 2 White Stars
UPDATE UnitPromotions SET PortraitIndex = '8' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 3 White Stars
UPDATE UnitPromotions SET PortraitIndex = '9' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 4 White Stars
UPDATE UnitPromotions SET PortraitIndex = '10' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Yellow Triangle + 5 White Stars
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- 2 Yellow Triangles
UPDATE UnitPromotions SET PortraitIndex = '12' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- 2 Red Triangles
UPDATE UnitPromotions SET PortraitIndex = '13' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- 3 Red Triangles
UPDATE UnitPromotions SET PortraitIndex = '14' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'tempVPPI_Atlas' WHERE Type = ''; -- Red Triangle + Yellow Triangle
UPDATE UnitPromotions SET PortraitIndex = '15' WHERE Type = '';
*/


-----------------------------------------------------------------------------------------------------------------

-- atlas_00

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_FIGHTER'; --                           ->  Bonus Vs Fighter (33)
UPDATE UnitPromotions SET PortraitIndex = '0' WHERE Type = 'PROMOTION_ANTI_FIGHTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_KNOCKOUT_I'; --                             ->  Bonus Vs Gun (25)
UPDATE UnitPromotions SET PortraitIndex = '1' WHERE Type = 'PROMOTION_KNOCKOUT_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_KNOCKOUT_II'; --                            ->  Bonus Vs Siege (25)
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = 'PROMOTION_KNOCKOUT_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_SUBMARINE_I'; --                       ->  Bonus Vs Submarine (33)
UPDATE UnitPromotions SET PortraitIndex = '3' WHERE Type = 'PROMOTION_ANTI_SUBMARINE_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_SUBMARINE_II'; --                      ->  Bonus Vs Submarine (75)
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = 'PROMOTION_ANTI_SUBMARINE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_TANK'; --                              ->  Bonus Vs Tank (100)
UPDATE UnitPromotions SET PortraitIndex = '5' WHERE Type = 'PROMOTION_ANTI_TANK';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_NAVAL_BONUS'; --                            ->  Bonus Vs Naval (50)
UPDATE UnitPromotions SET PortraitIndex = '6' WHERE Type = 'PROMOTION_NAVAL_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_HELICOPTER'; --                        ->  Bonus Vs Helicoptere (150)
UPDATE UnitPromotions SET PortraitIndex = '7' WHERE Type = 'PROMOTION_ANTI_HELICOPTER';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_AMBUSH_1'; --                               ->  Ambush I
UPDATE UnitPromotions SET PortraitIndex = '8' WHERE Type = 'PROMOTION_AMBUSH_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_AMBUSH_2'; --                               ->  Ambush II
UPDATE UnitPromotions SET PortraitIndex = '9' WHERE Type = 'PROMOTION_AMBUSH_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_HELI_AMBUSH_1'; --                          ->  Bonus Vs Armored (25)
UPDATE UnitPromotions SET PortraitIndex = '10' WHERE Type = 'PROMOTION_HELI_AMBUSH_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_HELI_AMBUSH_2'; --                          ->  Bonus Vs Armored (25)
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = 'PROMOTION_HELI_AMBUSH_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_OPEN_TERRAIN'; --                           ->  Combat Bonus in Open Terrain
UPDATE UnitPromotions SET PortraitIndex = '12' WHERE Type = 'PROMOTION_OPEN_TERRAIN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_MOUNTED_I'; --                         ->  Bonus Vs Mounted (50)
UPDATE UnitPromotions SET PortraitIndex = '13' WHERE Type = 'PROMOTION_ANTI_MOUNTED_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ANTI_MOUNTED_II'; --                        ->  Bonus Vs Mounted (100)
UPDATE UnitPromotions SET PortraitIndex = '14' WHERE Type = 'PROMOTION_ANTI_MOUNTED_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MOUNTED_PENALTY'; --                        ->  Penalty Vs Mounted (33)
UPDATE UnitPromotions SET PortraitIndex = '15' WHERE Type = 'PROMOTION_MOUNTED_PENALTY';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_STATUE_ZEUS'; --                            ->  Statue of Zeus
UPDATE UnitPromotions SET PortraitIndex = '16' WHERE Type = 'PROMOTION_STATUE_ZEUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_FIRE_SUPPORT'; --                           ->  Bonus Vs City (33)
UPDATE UnitPromotions SET PortraitIndex = '17' WHERE Type = 'PROMOTION_FIRE_SUPPORT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_HEAVY_PAYLOAD'; --                          ->  Bonus Vs City (50)
UPDATE UnitPromotions SET PortraitIndex = '18' WHERE Type = 'PROMOTION_HEAVY_PAYLOAD';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CITY_SIEGE'; --                           | -> Bonus Vs City (100)
UPDATE UnitPromotions SET PortraitIndex = '19' WHERE Type = 'PROMOTION_CITY_SIEGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CITY_SIEGE_II'; --                        | -> Bonus Vs City (100)
UPDATE UnitPromotions SET PortraitIndex = '19' WHERE Type = 'PROMOTION_CITY_SIEGE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CITY_ASSAULT'; --                           ->  Bonus Vs city (150)
UPDATE UnitPromotions SET PortraitIndex = '20' WHERE Type = 'PROMOTION_CITY_ASSAULT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_DOUBLE_PLUNDER'; --                         ->  City Plunder
UPDATE UnitPromotions SET PortraitIndex = '21' WHERE Type = 'PROMOTION_DOUBLE_PLUNDER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SMALL_CITY_PENALTY'; --                     ->  Penalty Attacking City (25)
UPDATE UnitPromotions SET PortraitIndex = '22' WHERE Type = 'PROMOTION_SMALL_CITY_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CITY_PENALTY'; --                           ->  Penalty Attacking City (33)
UPDATE UnitPromotions SET PortraitIndex = '23' WHERE Type = 'PROMOTION_CITY_PENALTY';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BIG_CITY_PENALTY'; --                       ->  Major Penalty Attacking City 75
UPDATE UnitPromotions SET PortraitIndex = '24' WHERE Type = 'PROMOTION_BIG_CITY_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_NO_CAPTURE'; --                             ->  Unable to Capture City
UPDATE UnitPromotions SET PortraitIndex = '25' WHERE Type = 'PROMOTION_NO_CAPTURE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ONLY_ATTACKS_CITIES'; --                    ->  City Attack Only
UPDATE UnitPromotions SET PortraitIndex = '26' WHERE Type = 'PROMOTION_ONLY_ATTACKS_CITIES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_PRISONER_WAR'; --                           ->  Prisoner of War
UPDATE UnitPromotions SET PortraitIndex = '27' WHERE Type = 'PROMOTION_PRISONER_WAR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BARBARIAN_BONUS'; --                        ->  Brute Force
UPDATE UnitPromotions SET PortraitIndex = '28' WHERE Type = 'PROMOTION_BARBARIAN_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_III'; --                  ->  Penalty Vs Barbarians (-10)
UPDATE UnitPromotions SET PortraitIndex = '29' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_II'; --                   ->  Penalty Vs Barbarians (-20)
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_I'; --                    ->  Penalty Vs Barbarians (-30)
UPDATE UnitPromotions SET PortraitIndex = '31' WHERE Type = 'PROMOTION_BARBARIAN_PENALTY_I';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SHOCK_4'; --                                ->  Overrun
UPDATE UnitPromotions SET PortraitIndex = '32' WHERE Type = 'PROMOTION_SHOCK_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ACCURACY_4'; --                             ->  Firing Doctrine
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_ACCURACY_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SKIRMISHER_POWER'; --                       ->  Coup de Grace
UPDATE UnitPromotions SET PortraitIndex = '34' WHERE Type = 'PROMOTION_SKIRMISHER_POWER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BOMBARDMENT_4'; --                          ->  Broadside
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_BOMBARDMENT_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BOARDING_PARTY_4'; --                       ->  Pincer
UPDATE UnitPromotions SET PortraitIndex = '36' WHERE Type = 'PROMOTION_BOARDING_PARTY_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_COASTAL_RAIDER_4'; --                       ->  Glory Hounds
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_COASTAL_RAIDER_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MORALE_EVENT'; --                           ->  Morale Event
UPDATE UnitPromotions SET PortraitIndex = '38' WHERE Type = 'PROMOTION_MORALE_EVENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MORALE'; --                                 ->  Morale
UPDATE UnitPromotions SET PortraitIndex = '39' WHERE Type = 'PROMOTION_MORALE';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_GREAT_GENERAL'; --                          ->  Leadership
UPDATE UnitPromotions SET PortraitIndex = '40' WHERE Type = 'PROMOTION_GREAT_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_GREAT_ADMIRAL'; --                          ->  Naval Leadership
UPDATE UnitPromotions SET PortraitIndex = '41' WHERE Type = 'PROMOTION_GREAT_ADMIRAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_GENERAL_STACKING'; --                       ->  Great General Combat Bonus
UPDATE UnitPromotions SET PortraitIndex = '42' WHERE Type = 'PROMOTION_GENERAL_STACKING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_BETTER_LEADERSHIP'; --                      ->  Regimental Traditions
UPDATE UnitPromotions SET PortraitIndex = '43' WHERE Type = 'PROMOTION_BETTER_LEADERSHIP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MOVEMENT_TO_GENERAL'; --                    ->  Transfert Movement to General
UPDATE UnitPromotions SET PortraitIndex = '44' WHERE Type = 'PROMOTION_MOVEMENT_TO_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SPAWN_GENERALS_I'; --                       ->  Great Generals I
UPDATE UnitPromotions SET PortraitIndex = '45' WHERE Type = 'PROMOTION_SPAWN_GENERALS_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SPAWN_GENERALS_II'; --                      ->  Great Generals II
UPDATE UnitPromotions SET PortraitIndex = '46' WHERE Type = 'PROMOTION_SPAWN_GENERALS_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_TREASURE_FLEET'; --                         ->  Treasure Fleet
UPDATE UnitPromotions SET PortraitIndex = '47' WHERE Type = 'PROMOTION_TREASURE_FLEET';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CHARGE'; --                                 ->  Charge I
UPDATE UnitPromotions SET PortraitIndex = '48' WHERE Type = 'PROMOTION_CHARGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_CHARGE_II'; --                              ->  Charge II
UPDATE UnitPromotions SET PortraitIndex = '49' WHERE Type = 'PROMOTION_CHARGE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_STRONGER_VS_DAMAGED'; --                    ->  Damaged Enemy Bonus (33)
UPDATE UnitPromotions SET PortraitIndex = '50' WHERE Type = 'PROMOTION_STRONGER_VS_DAMAGED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ATLATL_ATTACK'; --                          ->  Atlatl Strike
UPDATE UnitPromotions SET PortraitIndex = '51' WHERE Type = 'PROMOTION_ATLATL_ATTACK';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_HONOR_BONUS'; --                            ->  Conscription
UPDATE UnitPromotions SET PortraitIndex = '52' WHERE Type = 'PROMOTION_HONOR_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ATTACK_BONUS_SWEDEN'; --                    ->  No Retreat
UPDATE UnitPromotions SET PortraitIndex = '53' WHERE Type = 'PROMOTION_ATTACK_BONUS_SWEDEN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ADJACENT_BONUS'; --                         ->  Discipline
UPDATE UnitPromotions SET PortraitIndex = '54' WHERE Type = 'PROMOTION_ADJACENT_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_ATTACK_BONUS'; --                           ->  Bonus When Attacking (25)
UPDATE UnitPromotions SET PortraitIndex = '55' WHERE Type = 'PROMOTION_ATTACK_BONUS';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_TERROR'; --                                 ->  Precision
UPDATE UnitPromotions SET PortraitIndex = '56' WHERE Type = 'PROMOTION_TERROR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_TERROR_II'; --                              ->  Precision II
UPDATE UnitPromotions SET PortraitIndex = '57' WHERE Type = 'PROMOTION_TERROR_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_I'; --                ->  Covering Fire I
UPDATE UnitPromotions SET PortraitIndex = '58' WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_II'; --               ->  Covering Fire II
UPDATE UnitPromotions SET PortraitIndex = '59' WHERE Type = 'PROMOTION_MODERN_RANGED_PENALTY_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SPLASH'; --                                 ->  Splash Damage
UPDATE UnitPromotions SET PortraitIndex = '60' WHERE Type = 'PROMOTION_SPLASH';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_SPLASH_II'; --                              ->  Splash Damage II
UPDATE UnitPromotions SET PortraitIndex = '61' WHERE Type = 'PROMOTION_SPLASH_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_AOE_STRIKE_ON_KILL'; --                     ->  Grenadiers
UPDATE UnitPromotions SET PortraitIndex = '62' WHERE Type = 'PROMOTION_AOE_STRIKE_ON_KILL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_00' WHERE Type = 'PROMOTION_AOE_STRIKE_FORTIFY'; --                     ->  Pilum
UPDATE UnitPromotions SET PortraitIndex = '63' WHERE Type = 'PROMOTION_AOE_STRIKE_FORTIFY';

-- atlas_01

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ANTIAIR_LAND_I'; --                         ->  Air Defense I
UPDATE UnitPromotions SET PortraitIndex = '0' WHERE Type = 'PROMOTION_ANTIAIR_LAND_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ANTIAIR_LAND_II'; --                        ->  Air Defense II
UPDATE UnitPromotions SET PortraitIndex = '1' WHERE Type = 'PROMOTION_ANTIAIR_LAND_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ANTIAIR_LAND_III'; --                       ->  Air Defence III
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = 'PROMOTION_ANTIAIR_LAND_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FALLOUT_REDUCTION'; --                      ->  Fallout Resistance
UPDATE UnitPromotions SET PortraitIndex = '3' WHERE Type = 'PROMOTION_FALLOUT_REDUCTION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FALLOUT_IMMUNITY'; --                    |  -> Fallout Immunity
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = 'PROMOTION_FALLOUT_IMMUNITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NUCLEAR_SILO'; --                        |  -> Shielded Silo
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = 'PROMOTION_NUCLEAR_SILO';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SKI_INFANTRY'; --                           ->  Bonuses in Snow, Toundra, Hills
UPDATE UnitPromotions SET PortraitIndex = '5' WHERE Type = 'PROMOTION_SKI_INFANTRY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_DEFEND_NEAR_CAPITAL'; --                    ->  Near Capital Bonus
UPDATE UnitPromotions SET PortraitIndex = '6' WHERE Type = 'PROMOTION_DEFEND_NEAR_CAPITAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_HIMEJI_CASTLE'; --                          ->  Himeji Castel
UPDATE UnitPromotions SET PortraitIndex = '7' WHERE Type = 'PROMOTION_HIMEJI_CASTLE';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_RECON_BANDEIRANTES'; --                     ->  Flag Bearers
UPDATE UnitPromotions SET PortraitIndex = '8' WHERE Type = 'PROMOTION_RECON_BANDEIRANTES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_TARGETING_4'; --                            ->  Indomitable
UPDATE UnitPromotions SET PortraitIndex = '9' WHERE Type = 'PROMOTION_TARGETING_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NAVAL_PENALTY'; --                          ->  Penalty Vs Naval (50)
UPDATE UnitPromotions SET PortraitIndex = '10' WHERE Type = 'PROMOTION_NAVAL_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE'; --                     | -> Cannot End Turn in Ocean Tiles
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY'; --     | -> Ocean Impassable Until Astronomy
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = 'PROMOTION_OCEAN_IMPASSABLE_UNTIL_ASTRONOMY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_OCEAN_CROSSING'; --                         ->  Ocean Explorer
UPDATE UnitPromotions SET PortraitIndex = '12' WHERE Type = 'PROMOTION_OCEAN_CROSSING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_HOVERING_UNIT'; --                          ->  Hovering Unit
UPDATE UnitPromotions SET PortraitIndex = '13' WHERE Type = 'PROMOTION_HOVERING_UNIT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_DEEPWATER_EMBARKATION'; --                  ->  Ocean Embarkation
UPDATE UnitPromotions SET PortraitIndex = '14' WHERE Type = 'PROMOTION_DEEPWATER_EMBARKATION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION'; --                   ->  Embarkation With Ocean Movement
UPDATE UnitPromotions SET PortraitIndex = '15' WHERE Type = 'PROMOTION_ALLWATER_EMBARKATION';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION'; --                  ->  Embarkation with Defense
UPDATE UnitPromotions SET PortraitIndex = '16' WHERE Type = 'PROMOTION_DEFENSIVE_EMBARKATION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_HOVER_UNIT'; --                             ->  Hovering Unit Embarked
UPDATE UnitPromotions SET PortraitIndex = '17' WHERE Type = 'PROMOTION_HOVER_UNIT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_EMBARKED_SIGHT'; --                         ->  Extra Sight While Embarking
UPDATE UnitPromotions SET PortraitIndex = '18' WHERE Type = 'PROMOTION_EMBARKED_SIGHT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_WAR_CANOES'; --                             ->  War Canoes
UPDATE UnitPromotions SET PortraitIndex = '19' WHERE Type = 'PROMOTION_WAR_CANOES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SIGHT_PENALTY'; --                 |        ->  Limited Visibility
UPDATE UnitPromotions SET PortraitIndex = '20' WHERE Type = 'PROMOTION_SIGHT_PENALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SIGHT_PENALTY_NAVAL'; --           |        ->  Limited Visibility
UPDATE UnitPromotions SET PortraitIndex = '20' WHERE Type = 'PROMOTION_SIGHT_PENALTY_NAVAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_EXTRA_SIGHT_II'; --                         ->  Extra Sight (2)
UPDATE UnitPromotions SET PortraitIndex = '21' WHERE Type = 'PROMOTION_EXTRA_SIGHT_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SEE_INVISIBLE_SUBMARINE'; --                ->  Can see Submarine
UPDATE UnitPromotions SET PortraitIndex = '22' WHERE Type = 'PROMOTION_SEE_INVISIBLE_SUBMARINE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_INVISIBLE_SUBMARINE'; --                    ->  Is Invisible (submarine)
UPDATE UnitPromotions SET PortraitIndex = '23' WHERE Type = 'PROMOTION_INVISIBLE_SUBMARINE';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NAVAL_SENTRY_II'; --                   |    -> Navigator II
UPDATE UnitPromotions SET PortraitIndex = '24' WHERE Type = 'PROMOTION_NAVAL_SENTRY_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_GREAT_LIGHTHOUSE'; --                  |    -> Great Lighouse
UPDATE UnitPromotions SET PortraitIndex = '24' WHERE Type = 'PROMOTION_GREAT_LIGHTHOUSE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NAVAL_TRADITION'; --                        ->  Exploration
UPDATE UnitPromotions SET PortraitIndex = '25' WHERE Type = 'PROMOTION_NAVAL_TRADITION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_OCEAN_MOVEMENT'; --                         ->  Naval Movement
UPDATE UnitPromotions SET PortraitIndex = '26' WHERE Type = 'PROMOTION_OCEAN_MOVEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ARSENALE'; --                               ->  Venitian Craftsmanship
UPDATE UnitPromotions SET PortraitIndex = '27' WHERE Type = 'PROMOTION_ARSENALE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FLAT_MOVEMENT_COST'; --                     ->  All Tiles Cost 1 Move
UPDATE UnitPromotions SET PortraitIndex = '28' WHERE Type = 'PROMOTION_FLAT_MOVEMENT_COST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST'; --                    ->  Ignores Terrain Cost
UPDATE UnitPromotions SET PortraitIndex = '29' WHERE Type = 'PROMOTION_IGNORE_TERRAIN_COST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FAST_ADMIRAL'; --                        |  -> Mobile Admiral (+2 movements)
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_FAST_ADMIRAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SACRED_STEPS'; --                        |  -> Sacred Steps (+2 movements)
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_SACRED_STEPS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FAST_GENERAL'; --                           ->  Mobile General (+3 movements)
UPDATE UnitPromotions SET PortraitIndex = '31' WHERE Type = 'PROMOTION_FAST_GENERAL';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_GUN'; --                  ->  Lightning Warfare (Gun)
UPDATE UnitPromotions SET PortraitIndex = '32' WHERE Type = 'PROMOTION_LIGHTNING_WARFARE_GUN';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_MONGOL_TERROR'; --                          ->  Steppe Nomads
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_MONGOL_TERROR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_STEAM_POWERED'; --                          ->  Double Movement in Coast
UPDATE UnitPromotions SET PortraitIndex = '34' WHERE Type = 'PROMOTION_STEAM_POWERED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING'; --              |->  Can Move After Attack
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING_NAVAL'; --        |->  Can Move After Attack
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_CAN_MOVE_AFTER_ATTACKING_NAVAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ONLY_DEFENSIVE'; --                         ->  May not melee attack
UPDATE UnitPromotions SET PortraitIndex = '36' WHERE Type = 'PROMOTION_ONLY_DEFENSIVE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE'; --                  ->  Withdraw Before Melee
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_WITHDRAW_BEFORE_MELEE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FOLIAGE_IMPASSABLE'; --                     ->  Can Not Enter in Forest or Jungle
UPDATE UnitPromotions SET PortraitIndex = '38' WHERE Type = 'PROMOTION_FOLIAGE_IMPASSABLE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ROUGH_TERRAIN_ENDS_TURN'; --                ->  Rough Terrain Penalty
UPDATE UnitPromotions SET PortraitIndex = '39' WHERE Type = 'PROMOTION_ROUGH_TERRAIN_ENDS_TURN';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_JAGUAR'; --                            |    -> Combat Bonus in Forest/Jungle (33)
UPDATE UnitPromotions SET PortraitIndex = '40' WHERE Type = 'PROMOTION_JAGUAR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_MOHAWK'; --                            |    -> Combat Bonus in Forest/Jungle (33)
UPDATE UnitPromotions SET PortraitIndex = '40' WHERE Type = 'PROMOTION_MOHAWK';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_GUERILLA_FIGHTER'; --                       ->  Guerilla Fighter
UPDATE UnitPromotions SET PortraitIndex = '41' WHERE Type = 'PROMOTION_GUERILLA_FIGHTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_COERCION'; --                               ->  Coercion
UPDATE UnitPromotions SET PortraitIndex = '42' WHERE Type = 'PROMOTION_COERCION';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_GAIN_EXPERIENCE'; --                        ->  Quick Study
UPDATE UnitPromotions SET PortraitIndex = '43' WHERE Type = 'PROMOTION_GAIN_EXPERIENCE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_MEDIC'; --                                  ->  Medic
UPDATE UnitPromotions SET PortraitIndex = '44' WHERE Type = 'PROMOTION_MEDIC';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FASTER_HEAL'; --                            ->  Faster Healing
UPDATE UnitPromotions SET PortraitIndex = '45' WHERE Type = 'PROMOTION_FASTER_HEAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_MEDIC_GENERAL'; --                          ->  Enhanced Medic
UPDATE UnitPromotions SET PortraitIndex = '46' WHERE Type = 'PROMOTION_MEDIC_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_EVERLASTING_YOUTH'; --                      ->  Everlasting Youth
UPDATE UnitPromotions SET PortraitIndex = '47' WHERE Type = 'PROMOTION_EVERLASTING_YOUTH';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY'; --          ->  Heal 25 if Kill a Unit
UPDATE UnitPromotions SET PortraitIndex = '48' WHERE Type = 'PROMOTION_PARTIAL_HEAL_IF_DESTROY_ENEMY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_HEAL_IF_DESTROY_ENEMY'; --                  ->  Heal 50 if Kill a Unit
UPDATE UnitPromotions SET PortraitIndex = '49' WHERE Type = 'PROMOTION_HEAL_IF_DESTROY_ENEMY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_KILL_HEAL'; --                              ->  Heal on Kill (15hp)
UPDATE UnitPromotions SET PortraitIndex = '50' WHERE Type = 'PROMOTION_KILL_HEAL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_PRIZE_RULES'; --                            ->  Prize Rules
UPDATE UnitPromotions SET PortraitIndex = '51' WHERE Type = 'PROMOTION_PRIZE_RULES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ENSLAVEMENT'; --                            ->  Gift of The Pharaoh
UPDATE UnitPromotions SET PortraitIndex = '52' WHERE Type = 'PROMOTION_ENSLAVEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS'; --                     ->  Enhanced Flank Attack
UPDATE UnitPromotions SET PortraitIndex = '53' WHERE Type = 'PROMOTION_FLANK_ATTACK_BONUS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_RAIDER'; --                                 ->  Raider
UPDATE UnitPromotions SET PortraitIndex = '54' WHERE Type = 'PROMOTION_RAIDER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_ESPRIT_DE_CORPS'; --                        ->  Withering Fire
UPDATE UnitPromotions SET PortraitIndex = '55' WHERE Type = 'PROMOTION_ESPRIT_DE_CORPS';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_INDIRECT_FIRE'; --                          ->  Indirect Fire
UPDATE UnitPromotions SET PortraitIndex = '56' WHERE Type = 'PROMOTION_INDIRECT_FIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_RANGED_SUPPORT_FIRE'; --                    ->  Range Support Fire
UPDATE UnitPromotions SET PortraitIndex = '57' WHERE Type = 'PROMOTION_RANGED_SUPPORT_FIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NAVAL_MISFIRE'; --                          ->  Naval Target Penalty
UPDATE UnitPromotions SET PortraitIndex = '58' WHERE Type = 'PROMOTION_NAVAL_MISFIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_NAVAL_INACCURACY'; --                       ->  Naval Inaccuracy
UPDATE UnitPromotions SET PortraitIndex = '59' WHERE Type = 'PROMOTION_NAVAL_INACCURACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_SIEGE_INACCURACY'; --                       ->  Siege Inaccuravy
UPDATE UnitPromotions SET PortraitIndex = '60' WHERE Type = 'PROMOTION_SIEGE_INACCURACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_WEAK_RANGED'; --                            ->  Weak Ranged Attack
UPDATE UnitPromotions SET PortraitIndex = '61' WHERE Type = 'PROMOTION_WEAK_RANGED';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_BETTER_BOMBARDMENT'; --                     ->  Shrapnel Rounds
UPDATE UnitPromotions SET PortraitIndex = '62' WHERE Type = 'PROMOTION_BETTER_BOMBARDMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_01' WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES'; --                     ->  No Movement Cost Pillage
UPDATE UnitPromotions SET PortraitIndex = '63' WHERE Type = 'PROMOTION_FREE_PILLAGE_MOVES';

-- atlas_02

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_TARGETING_1'; --                        ->  Air Targetting I
UPDATE UnitPromotions SET PortraitIndex = '0' WHERE Type = 'PROMOTION_AIR_TARGETING_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_TARGETING_2'; --                        ->  Air Targetting II
UPDATE UnitPromotions SET PortraitIndex = '1' WHERE Type = 'PROMOTION_AIR_TARGETING_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_TARGETING_3'; --                        ->  Air Targetting III
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = 'PROMOTION_AIR_TARGETING_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_RANGE'; --                              ->  Range (Air Units)
UPDATE UnitPromotions SET PortraitIndex = '3' WHERE Type = 'PROMOTION_AIR_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_INTERCEPTION_I'; --                         ->  Interception (20)
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = 'PROMOTION_INTERCEPTION_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_INTERCEPTION_II'; --                        ->  Interception (40)
UPDATE UnitPromotions SET PortraitIndex = '5' WHERE Type = 'PROMOTION_INTERCEPTION_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_INTERCEPTION_III'; --                       ->  Interception (50)
UPDATE UnitPromotions SET PortraitIndex = '6' WHERE Type = 'PROMOTION_INTERCEPTION_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_INTERCEPTION_IV'; --                        ->  Interception (100)
UPDATE UnitPromotions SET PortraitIndex = '7' WHERE Type = 'PROMOTION_INTERCEPTION_IV';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EVASION_I'; --                              ->  Evasion (50)
UPDATE UnitPromotions SET PortraitIndex = '8' WHERE Type = 'PROMOTION_EVASION_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EVASION_II'; --                             ->  Evasion (80)
UPDATE UnitPromotions SET PortraitIndex = '9' WHERE Type = 'PROMOTION_EVASION_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EVASION_III'; --                            ->  Stealth
UPDATE UnitPromotions SET PortraitIndex = '10' WHERE Type = 'PROMOTION_EVASION_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_ANTI_AIR'; --                               ->  Flak Targetting I
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = 'PROMOTION_ANTI_AIR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_ANTI_AIR_II'; --                            ->  Flak Targetting II
UPDATE UnitPromotions SET PortraitIndex = '12' WHERE Type = 'PROMOTION_ANTI_AIR_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_LOGISTICS'; --                          ->  Air Logistics
UPDATE UnitPromotions SET PortraitIndex = '13' WHERE Type = 'PROMOTION_AIR_LOGISTICS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_SWEEP'; --                              ->  Air Sweep
UPDATE UnitPromotions SET PortraitIndex = '14' WHERE Type = 'PROMOTION_AIR_SWEEP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EVASION'; --                                ->  Evasion (Reduced Damage)
UPDATE UnitPromotions SET PortraitIndex = '15' WHERE Type = 'PROMOTION_EVASION';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_NATIONALISM'; --                            ->  Patriotic War
UPDATE UnitPromotions SET PortraitIndex = '16' WHERE Type = 'PROMOTION_NATIONALISM';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN_BOOGALOO'; --             ->  Price of The Ancestors
UPDATE UnitPromotions SET PortraitIndex = '17' WHERE Type = 'PROMOTION_HOMELAND_GUARDIAN_BOOGALOO';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_BARRAGE_4'; --                              ->  Infiltrators
UPDATE UnitPromotions SET PortraitIndex = '18' WHERE Type = 'PROMOTION_BARRAGE_4';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_FOREIGN_LANDS'; --                          ->  Foreigh Land Bonus
UPDATE UnitPromotions SET PortraitIndex = '19' WHERE Type = 'PROMOTION_FOREIGN_LANDS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_RIVAL_TERRITORY'; --                        ->  Can Enter Rival Territory
UPDATE UnitPromotions SET PortraitIndex = '20' WHERE Type = 'PROMOTION_RIVAL_TERRITORY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_MUST_SET_UP'; --                            ->  Moves at Half-Speed in Enemy Territory
UPDATE UnitPromotions SET PortraitIndex = '21' WHERE Type = 'PROMOTION_MUST_SET_UP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SCOUT_XP_PILLAGE'; --                       ->  Scavenger
UPDATE UnitPromotions SET PortraitIndex = '22' WHERE Type = 'PROMOTION_SCOUT_XP_PILLAGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SCOUT_XP_SPOTTING'; --                      ->  Spotters
UPDATE UnitPromotions SET PortraitIndex = '23' WHERE Type = 'PROMOTION_SCOUT_XP_SPOTTING';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_NO_DEFENSIVE_BONUSES'; --                   ->  No Defensive Terrain Bonus
UPDATE UnitPromotions SET PortraitIndex = '24' WHERE Type = 'PROMOTION_NO_DEFENSIVE_BONUSES';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_CAN_MOVE_IMPASSABLE'; --                  | -> May Enter Ice Tiles
UPDATE UnitPromotions SET PortraitIndex = '25' WHERE Type = 'PROMOTION_CAN_MOVE_IMPASSABLE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_ICE_BREAKERS'; --                         | -> Ice Breakers
UPDATE UnitPromotions SET PortraitIndex = '25' WHERE Type = 'PROMOTION_ICE_BREAKERS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_I'; --                 ->  Trailblazer I
UPDATE UnitPromotions SET PortraitIndex = '26' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_II'; --                ->  Trailblazer II
UPDATE UnitPromotions SET PortraitIndex = '27' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_III'; --               ->  Trailblazer III
UPDATE UnitPromotions SET PortraitIndex = '28' WHERE Type = 'PROMOTION_WOODLAND_TRAILBLAZER_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_PARADROP'; --                               ->  May Paradrop
UPDATE UnitPromotions SET PortraitIndex = '29' WHERE Type = 'PROMOTION_PARADROP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_PRIZE_SHIPS'; --                            ->  Prize Ships
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_PRIZE_SHIPS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_HAKA_WAR_DANCE'; --                         ->  Haka War Dance
UPDATE UnitPromotions SET PortraitIndex = '31' WHERE Type = 'PROMOTION_HAKA_WAR_DANCE';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EMISSARY'; --                               ->  Initiate
UPDATE UnitPromotions SET PortraitIndex = '32' WHERE Type = 'PROMOTION_EMISSARY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_ENVOY'; --                                  ->  Adept
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_ENVOY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_DIPLOMAT'; --                               ->  Professional
UPDATE UnitPromotions SET PortraitIndex = '34' WHERE Type = 'PROMOTION_DIPLOMAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AMBASSADOR'; --                             ->  Plenipotentiary
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_AMBASSADOR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_GREAT_DIPLOMAT'; --                         ->  Master Diplomat
UPDATE UnitPromotions SET PortraitIndex = '36' WHERE Type = 'PROMOTION_GREAT_DIPLOMAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_NOBEL_LAUREATE'; --                         ->  Nobel Laureate
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_NOBEL_LAUREATE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_PAX'; --                                    ->  Imperial Seal
UPDATE UnitPromotions SET PortraitIndex = '38' WHERE Type = 'PROMOTION_PAX';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SIGNET'; --                                 ->  Royal Signet
UPDATE UnitPromotions SET PortraitIndex = '39' WHERE Type = 'PROMOTION_SIGNET';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_LITERACY'; --                               ->  Literacy
UPDATE UnitPromotions SET PortraitIndex = '40' WHERE Type = 'PROMOTION_LITERACY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_EXPRESS'; --                                ->  Wire Service
UPDATE UnitPromotions SET PortraitIndex = '41' WHERE Type = 'PROMOTION_EXPRESS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_IMMUNITY'; --                               -> Diplomatic Immunity
UPDATE UnitPromotions SET PortraitIndex = '42' WHERE Type = 'PROMOTION_IMMUNITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_UNWELCOME_EVANGELIST'; --                   ->  Unwelcome Evangelist
UPDATE UnitPromotions SET PortraitIndex = '43' WHERE Type = 'PROMOTION_UNWELCOME_EVANGELIST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_CARGO_I'; --                                ->  Can Carry 1 Cargo
UPDATE UnitPromotions SET PortraitIndex = '44' WHERE Type = 'PROMOTION_CARGO_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_CARGO_II'; --                               ->  Can Carry 2 Cargo
UPDATE UnitPromotions SET PortraitIndex = '45' WHERE Type = 'PROMOTION_CARGO_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_CARGO_III'; --                              ->  Can Carry 3 Cargo
UPDATE UnitPromotions SET PortraitIndex = '46' WHERE Type = 'PROMOTION_CARGO_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_CARGO_IV'; --                               ->  Can Carry 4 Cargo
UPDATE UnitPromotions SET PortraitIndex = '47' WHERE Type = 'PROMOTION_CARGO_IV';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_VIKING'; --                                 ->  Viking
UPDATE UnitPromotions SET PortraitIndex = '48' WHERE Type = 'PROMOTION_VIKING';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_LONGBOAT'; --                               ->  Longboat
UPDATE UnitPromotions SET PortraitIndex = '49' WHERE Type = 'PROMOTION_LONGBOAT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SILENT_HUNTER'; --                          ->  Bonus When Attacking 75 sub
UPDATE UnitPromotions SET PortraitIndex = '50' WHERE Type = 'PROMOTION_SILENT_HUNTER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_FEARED_ELEPHANT'; --                        ->  Feared Elephant
UPDATE UnitPromotions SET PortraitIndex = '51' WHERE Type = 'PROMOTION_FEARED_ELEPHANT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_NAVAL_DEFENSE_BOOST'; --                    ->  Ironside
UPDATE UnitPromotions SET PortraitIndex = '52' WHERE Type = 'PROMOTION_NAVAL_DEFENSE_BOOST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SELL_EXOTIC_GOODS'; --                      ->  Exotic Cargo
UPDATE UnitPromotions SET PortraitIndex = '53' WHERE Type = 'PROMOTION_SELL_EXOTIC_GOODS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_GREAT_MERCHANT'; --                         ->  Market Control
UPDATE UnitPromotions SET PortraitIndex = '54' WHERE Type = 'PROMOTION_GREAT_MERCHANT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_TRADE_MISSION_BONUS'; --                    ->  Trade Mission Bonus
UPDATE UnitPromotions SET PortraitIndex = '55' WHERE Type = 'PROMOTION_TRADE_MISSION_BONUS';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_RECON'; --                              ->  Air Recon
UPDATE UnitPromotions SET PortraitIndex = '56' WHERE Type = 'PROMOTION_AIR_RECON';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_RECON_LONG_RANGE'; --                       ->  Long Range Recon
UPDATE UnitPromotions SET PortraitIndex = '57' WHERE Type = 'PROMOTION_RECON_LONG_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_RECON_SHORT_RANGE'; --                      ->  Short Range Recon
UPDATE UnitPromotions SET PortraitIndex = '58' WHERE Type = 'PROMOTION_RECON_SHORT_RANGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_AIR_MISFIRE'; --                            ->  Air Skirmisher
UPDATE UnitPromotions SET PortraitIndex = '59' WHERE Type = 'PROMOTION_AIR_MISFIRE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_NAVAL_SENTRY'; --                           ->  Navigator I
UPDATE UnitPromotions SET PortraitIndex = '60' WHERE Type = 'PROMOTION_NAVAL_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_HEAVY_SHIP'; --                             ->  Heavy Assault
UPDATE UnitPromotions SET PortraitIndex = '61' WHERE Type = 'PROMOTION_HEAVY_SHIP';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_SURVIVALISM_3'; --                          ->  Survivalism III
UPDATE UnitPromotions SET PortraitIndex = '62' WHERE Type = 'PROMOTION_SURVIVALISM_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_02' WHERE Type = 'PROMOTION_LOGISTICS'; --                              ->  Logistics (Ranged Units)
UPDATE UnitPromotions SET PortraitIndex = '63' WHERE Type = 'PROMOTION_LOGISTICS';

-- atlas_03

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_I'; --                                ->  Field I
UPDATE UnitPromotions SET PortraitIndex = '0' WHERE Type = 'PROMOTION_FIELD_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_II'; --                               ->  Field II
UPDATE UnitPromotions SET PortraitIndex = '1' WHERE Type = 'PROMOTION_FIELD_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_III'; --                              ->  Field III
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = 'PROMOTION_FIELD_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SIEGE_I'; --                                ->  Siege I
UPDATE UnitPromotions SET PortraitIndex = '3' WHERE Type = 'PROMOTION_SIEGE_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SIEGE_II'; --                               ->  Siege II
UPDATE UnitPromotions SET PortraitIndex = '4' WHERE Type = 'PROMOTION_SIEGE_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SIEGE_III'; --                              ->  Siege III
UPDATE UnitPromotions SET PortraitIndex = '5' WHERE Type = 'PROMOTION_SIEGE_III';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_WOODS_WALKER'; --                           ->  Woods Walkers
UPDATE UnitPromotions SET PortraitIndex = '6' WHERE Type = 'PROMOTION_WOODS_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_DESERT_WARRIOR'; --                         ->  Desert Warrior
UPDATE UnitPromotions SET PortraitIndex = '7' WHERE Type = 'PROMOTION_DESERT_WARRIOR';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BUSHIDO'; --                                ->  Eight Virtues of Bushido
UPDATE UnitPromotions SET PortraitIndex = '8' WHERE Type = 'PROMOTION_BUSHIDO';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_EVIL_SPIRITS'; --                           ->  Ancestral Protection
UPDATE UnitPromotions SET PortraitIndex = '9' WHERE Type = 'PROMOTION_EVIL_SPIRITS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_ENEMY_BLADE'; --                            ->  Enemy Defection
UPDATE UnitPromotions SET PortraitIndex = '10' WHERE Type = 'PROMOTION_ENEMY_BLADE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_HEROISM'; --                                ->  Heroism
UPDATE UnitPromotions SET PortraitIndex = '11' WHERE Type = 'PROMOTION_HEROISM';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_HILL_WALKER'; --                            ->  Hill Walkers
UPDATE UnitPromotions SET PortraitIndex = '12' WHERE Type = 'PROMOTION_HILL_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_MARSH_WALKER'; --                           ->  Marsh Walkers
UPDATE UnitPromotions SET PortraitIndex = '13' WHERE Type = 'PROMOTION_MARSH_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_WHITE_WALKER'; --                           ->  White Walkers
UPDATE UnitPromotions SET PortraitIndex = '14' WHERE Type = 'PROMOTION_WHITE_WALKER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_DESERT_WALKER'; --                          ->  Desert Walkers
UPDATE UnitPromotions SET PortraitIndex = '15' WHERE Type = 'PROMOTION_DESERT_WALKER';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BUSHIDO_HONOR'; --                          ->  Honor
UPDATE UnitPromotions SET PortraitIndex = '16' WHERE Type = 'PROMOTION_BUSHIDO_HONOR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_LOYALTY'; --                                ->  Loyalty
UPDATE UnitPromotions SET PortraitIndex = '17' WHERE Type = 'PROMOTION_LOYALTY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SELF_CONTROL'; --                           ->  Self Control
UPDATE UnitPromotions SET PortraitIndex = '18' WHERE Type = 'PROMOTION_SELF_CONTROL';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_COURAGE'; --                                ->  Courage
UPDATE UnitPromotions SET PortraitIndex = '19' WHERE Type = 'PROMOTION_COURAGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_RESPECT'; --                                ->  Respect
UPDATE UnitPromotions SET PortraitIndex = '20' WHERE Type = 'PROMOTION_RESPECT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_RIGHTEOUSNESS'; --                          ->  Righteousness
UPDATE UnitPromotions SET PortraitIndex = '21' WHERE Type = 'PROMOTION_RIGHTEOUSNESS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SINCERITY'; --                              ->  Sincerity
UPDATE UnitPromotions SET PortraitIndex = '22' WHERE Type = 'PROMOTION_SINCERITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BENEVOLENCE'; --                            ->  Benevolence
UPDATE UnitPromotions SET PortraitIndex = '23' WHERE Type = 'PROMOTION_BENEVOLENCE';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SUPPLY_BOOST'; --                           ->  Military Tradition
UPDATE UnitPromotions SET PortraitIndex = '24' WHERE Type = 'PROMOTION_SUPPLY_BOOST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_CONSCRIPT'; --                              ->  Conscript
UPDATE UnitPromotions SET PortraitIndex = '25' WHERE Type = 'PROMOTION_CONSCRIPT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BUFFALO_CHEST'; --                          ->  Buffalo Chest
UPDATE UnitPromotions SET PortraitIndex = '26' WHERE Type = 'PROMOTION_BUFFALO_CHEST';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BUFFALO_HORNS'; --                          ->  Buffalo Horns
UPDATE UnitPromotions SET PortraitIndex = '27' WHERE Type = 'PROMOTION_BUFFALO_HORNS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BUFFALO_LOINS'; --                          ->  Buffalo Lions
UPDATE UnitPromotions SET PortraitIndex = '28' WHERE Type = 'PROMOTION_BUFFALO_LOINS';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_LIGHTNING_WARFARE'; --                      ->  Lightning Warfare
UPDATE UnitPromotions SET PortraitIndex = '29' WHERE Type = 'PROMOTION_LIGHTNING_WARFARE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SKIRMISHER_MOBILITY'; --                    ->  Parthian Tactics
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_SKIRMISHER_MOBILITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_DAMAGE_REDUCTION'; --                       ->  Dauntless
UPDATE UnitPromotions SET PortraitIndex = '31' WHERE Type = 'PROMOTION_DAMAGE_REDUCTION';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_COASTAL_RAIDER_1'; --                       ->  Dreadnought I
UPDATE UnitPromotions SET PortraitIndex = '32' WHERE Type = 'PROMOTION_COASTAL_RAIDER_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_COASTAL_RAIDER_2'; --                       ->  Dreadnought II
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_COASTAL_RAIDER_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_COASTAL_RAIDER_3'; --                       ->  Dreadnought III
UPDATE UnitPromotions SET PortraitIndex = '34' WHERE Type = 'PROMOTION_COASTAL_RAIDER_3';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BREACHER'; --                               ->  Breacher
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_BREACHER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_ENCIRCLEMENT'; --                           ->  Encirclement
UPDATE UnitPromotions SET PortraitIndex = '36' WHERE Type = 'PROMOTION_ENCIRCLEMENT';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_NAVAL_SIEGE'; --                            ->  Naval Siege
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_NAVAL_SIEGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_COASTAL_TERROR'; --                         ->  vanguard
UPDATE UnitPromotions SET PortraitIndex = '38' WHERE Type = 'PROMOTION_COASTAL_TERROR';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_PIRACY'; --                                 ->  Piracy
UPDATE UnitPromotions SET PortraitIndex = '39' WHERE Type = 'PROMOTION_PIRACY';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_PRESS_GANGS'; --                            ->  Press Gangs
UPDATE UnitPromotions SET PortraitIndex = '40' WHERE Type = 'PROMOTION_PRESS_GANGS';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                 ->  UNUSED (-1 move)
--UPDATE UnitPromotions SET PortraitIndex = '41' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BOARDED_I'; --                              ->  Boarded (I)
UPDATE UnitPromotions SET PortraitIndex = '42' WHERE Type = 'PROMOTION_BOARDED_I';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BOARDED_II'; --                             ->  Boarded (II)
UPDATE UnitPromotions SET PortraitIndex = '43' WHERE Type = 'PROMOTION_BOARDED_II';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_MINELAYER'; --                              ->  Minelayer
UPDATE UnitPromotions SET PortraitIndex = '44' WHERE Type = 'PROMOTION_MINELAYER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_WORKS_1'; --                          ->  Field Works
UPDATE UnitPromotions SET PortraitIndex = '45' WHERE Type = 'PROMOTION_FIELD_WORKS_1';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_WORKS_2'; --                          ->  Entrenchments
UPDATE UnitPromotions SET PortraitIndex = '46' WHERE Type = 'PROMOTION_FIELD_WORKS_2';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_FIELD_WORKS_3'; --                          ->  DFPs
UPDATE UnitPromotions SET PortraitIndex = '47' WHERE Type = 'PROMOTION_FIELD_WORKS_3';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_ROUGH_TERRAIN_HALF_TURN'; --                 ->  Skirmisher Doctrine
UPDATE UnitPromotions SET PortraitIndex = '48' WHERE Type = 'PROMOTION_ROUGH_TERRAIN_HALF_TURN';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  UNUSED (Bonus vs Barbarian)
--UPDATE UnitPromotions SET PortraitIndex = '49' WHERE Type = '';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  UNUSED (Bonus vs Barbarian)
--UPDATE UnitPromotions SET PortraitIndex = '50' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_BOARDED_III'; --                             ->  Boarded (III)
UPDATE UnitPromotions SET PortraitIndex = '51' WHERE Type = 'PROMOTION_BOARDED_III';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  UNUSED (Ignore ZOC)
--UPDATE UnitPromotions SET PortraitIndex = '52' WHERE Type = '';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_NOBILITY'; --                                ->  Noble
UPDATE UnitPromotions SET PortraitIndex = '53' WHERE Type = 'PROMOTION_NOBILITY';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SLINGER'; --                                 ->  Concussive Hits
UPDATE UnitPromotions SET PortraitIndex = '54' WHERE Type = 'PROMOTION_SLINGER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_DAZED'; --                                   ->  Dazed
UPDATE UnitPromotions SET PortraitIndex = '55' WHERE Type = 'PROMOTION_DAZED';

UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_SIEGE'; --                                   ->  City Assault
UPDATE UnitPromotions SET PortraitIndex = '56' WHERE Type = 'PROMOTION_SIEGE';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_ALHAMBRA'; --                                ->  Jinete
UPDATE UnitPromotions SET PortraitIndex = '57' WHERE Type = 'PROMOTION_ALHAMBRA';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_REPEATER'; --                                ->  Siege Volleys
UPDATE UnitPromotions SET PortraitIndex = '58' WHERE Type = 'PROMOTION_REPEATER';
UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = 'PROMOTION_MECH'; --                                    ->  Titan
UPDATE UnitPromotions SET PortraitIndex = '59' WHERE Type = 'PROMOTION_MECH';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  
--UPDATE UnitPromotions SET PortraitIndex = '60' WHERE Type = '';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  
--UPDATE UnitPromotions SET PortraitIndex = '61' WHERE Type = '';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  
--UPDATE UnitPromotions SET PortraitIndex = '62' WHERE Type = '';
--UPDATE UnitPromotions SET IconAtlas = 'promoVP_atlas_03' WHERE Type = ''; --                                                  ->  
--UPDATE UnitPromotions SET PortraitIndex = '63' WHERE Type = '';



--
-- Firaxis Icons reassignment
--

-- Set "+1 Movement" Icon 

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_MOON_STRIKER'; --                           ->  Full Moon Striker
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_MOON_STRIKER';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_FASTER_GENERAL'; --                         ->  Faster General (+1 move)
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_FASTER_GENERAL';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_EXTRA_MOVES_I'; --                          ->  1 Extra Movement - (Commanche Riders)
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_EXTRA_MOVES_I';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_SCOUTING_3'; --                             ->  Scouting III
UPDATE UnitPromotions SET PortraitIndex = '33' WHERE Type = 'PROMOTION_SCOUTING_3';

-- Set "+1 Visibility Range" Icon

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_SENTRY'; --                                 ->  Sentry
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_SKIRMISHER_SENTRY'; --                      ->  Sentry (Ranged)
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_SKIRMISHER_SENTRY';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_EXTRA_SIGHT_I'; --                          ->  Extra Sight (1)
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_EXTRA_SIGHT_I';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_IMPERIALISM_OPENER'; --                     ->  Imperialism
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_IMPERIALISM_OPENER';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_SCOUTING_2'; --                             ->  Scouting II
UPDATE UnitPromotions SET PortraitIndex = '37' WHERE Type = 'PROMOTION_SCOUTING_2';

-- Set "+3 Visibility Range" Icon

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_EXTRA_SIGHT_III'; --                        ->  Extra Sight (3)
UPDATE UnitPromotions SET PortraitIndex = '38' WHERE Type = 'PROMOTION_EXTRA_SIGHT_III';

-- Set "+4 Visibility Range" Icon

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_EXTRA_SIGHT_IV'; --                         ->  Extra Sight (4)
UPDATE UnitPromotions SET PortraitIndex = '39' WHERE Type = 'PROMOTION_EXTRA_SIGHT_IV';

-- Set "Logistics" Icon

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_SECOND_ATTACK'; --                          ->  -20% Combat Strenght. May attack twice
UPDATE UnitPromotions SET PortraitIndex = '30' WHERE Type = 'PROMOTION_SECOND_ATTACK';

-- Set "Penalty on Defense" Icon

UPDATE UnitPromotions SET IconAtlas = 'KRIS_SWORDSMAN_PROMOTION_ATLAS' WHERE Type = 'PROMOTION_DEFENSE_PENALTY'; --         ->  Penalty on Defense (33)
UPDATE UnitPromotions SET PortraitIndex = '2' WHERE Type = 'PROMOTION_DEFENSE_PENALTY';

-- Set Firaxis leaf icon to Jaguar/Mohawk

UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_JAGUAR'; --                                  ->  Combat Bonus in Forest and Jungle (33)
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_JAGUAR';
UPDATE UnitPromotions SET IconAtlas = 'PROMOTION_ATLAS' WHERE Type = 'PROMOTION_MOHAWK'; --                                  ->  Combat Bonus in Forest and Jungle (33)
UPDATE UnitPromotions SET PortraitIndex = '35' WHERE Type = 'PROMOTION_MOHAWK';

