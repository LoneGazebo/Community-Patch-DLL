-- Settler
UPDATE Language_en_US
SET Text = 'Founds new Cities to expand your Empire.[NEWLINE][NEWLINE][ICON_FOOD] Growth of the City is stopped while this Unit is being built. Reduces City''s [ICON_CITIZEN] Population by 1 when completed.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Cannot found Cities next to anyone''s border or if your Empire is very unhappy.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SETTLER';

-- Great Writer
UPDATE Language_en_US
SET Text = 'A Great Writer can create a [ICON_VP_GREATWRITING] Great Work of Writing (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Amphitheater, National Epic, Heroic Epic, or Royal Library). A Great Writer can also write a Political Treatise, which grants the player an amount of Culture that scales with the number of owned [ICON_GREAT_WORK] Great Works. Great Writers are expended when used either of these ways.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_WRITER_STRATEGY';

-- Great Artist
UPDATE Language_en_US
SET Text = 'A Great Artist can create a [ICON_VP_GREATART] Great Work of Art (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like the Palace, a Museum, or a Cathedral). A Great Artist can also generate a large sum of Golden Age Points, scaling with [ICON_GOLDEN_AGE] Golden Age Point output, [ICON_TOURISM] Tourism output, and the number of [COLOR_POSITIVE_TEXT]themed[ENDCOLOR] sets of [ICON_GREAT_WORK] Great Works. Great Artists are expended when used either of these ways.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_ARTIST_STRATEGY';

-- Great Musician
UPDATE Language_en_US
SET Text = 'A Great Musician can create a [ICON_VP_GREATMUSIC] Great Work of Music (generates both [ICON_CULTURE] Culture and [ICON_TOURISM] Tourism) that is placed in the nearest city that has an appropriate building with an empty slot (like an Opera House or Broadcast Tower).[NEWLINE][NEWLINE]A Great Musician can also travel to another civilization and perform a [COLOR_POSITIVE_TEXT]Concert Tour[ENDCOLOR], increasing your [ICON_TOURISM] Tourism with the target Civilization by 100%, and all other Civilizations by 50%, for 10 Turns plus 1 additional Turn for every owned [ICON_VP_GREATMUSIC] Great Work of Music. You also receive 1 [ICON_HAPPINESS_1] Happiness in every City. This action consumes the unit.[NEWLINE][NEWLINE]You cannot perform a Concert Tour if at war with the target Civilization, or if your [ICON_TOURISM] Cultural Influence over the Civilization is [COLOR_MAGENTA]Influential[ENDCOLOR] or greater.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_MUSICIAN_STRATEGY';

-- Great Scientist
UPDATE Language_en_US
SET Text = 'The Great Scientist can construct the special Academy improvement which, when worked, produces lots of [ICON_RESEARCH] Science. Further, a Great Scientist can give you a considerable boost towards your next tech, increased by [COLOR_POSITIVE_TEXT]10%[ENDCOLOR] per Academy created and owned. The Great Scientist is expended when used in any of these ways.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_SCIENTIST_STRATEGY';

-- Great Merchant
UPDATE Language_en_US
SET Text = 'The Great Merchant can construct the special Town improvement which, when worked, produces [ICON_GOLD] Gold and [ICON_FOOD] Food. The Great Merchant can also journey to a city-state and perform a "trade mission" which produces a large sum of [ICON_GOLD] Gold and starts a "We Love the King Day" in all owned cities, increased by 25% per Town created and owned. The Great Merchant is expended when used in either of these ways.[NEWLINE][NEWLINE]Towns receive +1 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if built on a Road that connects two owned Cities, and +2 [ICON_GOLD] Gold and [ICON_PRODUCTION] Production if a Railroad. Receive additional [ICON_GOLD] Gold and [ICON_PRODUCTION] Production (+1 for Roads, +2 for Railroads) if a Trade Route, either internal or international, passes over this Town.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_MERCHANT_STRATEGY';

-- Great Engineer
UPDATE Language_en_US
SET Text = 'Great Engineers can construct the special Manufactory improvement. When worked, it provides a lot of [ICON_PRODUCTION] Production for a city. The Great Engineer can also hurry the production of a unit, building or Wonder in a city, increased by 10% per Manufactory created and owned. The Great Engineer is expended when used in either of these ways.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_ENGINEER_STRATEGY';

-- Great General
UPDATE Language_en_US
SET Text = 'The Great General can construct the special [COLOR_POSITIVE_TEXT]Citadel[ENDCOLOR] improvement which gives a big defensive bonus to any Unit occupying it, damages Enemy Units that finish their turn in tiles adjacent to it, puts all adjacent tiles into your territory, and increases your Military Unit Supply Cap by 1. The Great General is consumed when it builds the [COLOR_POSITIVE_TEXT]Citadel[ENDCOLOR]. The Great General provides +15% [ICON_STRENGTH] Combat Strength to all player-owned Land Units within 2 tiles. The Great General is NOT consumed when he provides this bonus.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_GENERAL_STRATEGY';

-- Great Admiral
UPDATE Language_en_US
SET Text = 'The Great Admiral can immediately cross oceans for free, making it a useful (if vulnerable) exploration vessel. Furthermore, while in owned territory, the Great Admiral can be sent on a [COLOR_POSITIVE_TEXT]Voyage of Discovery[ENDCOLOR] that expends the Unit and gives you two copies of a Luxury Resource not available on the current map.[NEWLINE][NEWLINE]The Great Admiral has the ability to instantly [COLOR_POSITIVE_TEXT]Repair[ENDCOLOR] every Naval and Embarked Unit in the same hex, as well as in adjacent hexes. The Great Admiral is consumed when performing this action. The Great Admiral also provides +15% [ICON_STRENGTH] Combat Strength to all player-owned Naval Units within 2 tiles. The Great Admiral is NOT consumed when he provides this bonus. When the Great Admiral is expended for a [COLOR_POSITIVE_TEXT]Voyage of Discovery[ENDCOLOR] or [COLOR_POSITIVE_TEXT]Repair[ENDCOLOR], it increases your Military Unit Supply Cap by 1.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_ADMIRAL_STRATEGY';

-- Inquisitor
UPDATE Language_en_US
SET Text = 'Used to remove other religions from cities. Inquisitors stationed in cities reduce foreign Missionary and Prophet spread power by 50%. May only be obtained by purchasing with [ICON_PEACE] Faith.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_INQUISITOR';

UPDATE Language_en_US
SET Text = 'Can be purchased with [ICON_PEACE] Faith in any city with a majority Religion that has been enhanced. They can remove ' || (SELECT Value FROM Defines WHERE Name = 'INQUISITION_EFFECTIVENESS') || '% of foreign religious pressure from your cities (expending the Inquisitor) or be placed inside a city to reduce foreign Missionary and Prophet spread power in that City by half.'
WHERE Tag = 'TXT_KEY_UNIT_INQUISITOR_STRATEGY';

-- Archaeologist
UPDATE Language_en_US
SET Text = 'Maximum [COLOR_POSITIVE_TEXT]3[ENDCOLOR] active Archaeologists per player at any one time. Archaeologists are a special subtype of Worker that are used to excavate Antiquity Sites to either create Landmark improvements or to extract [ICON_VP_ARTIFACT] Artifacts to fill in [ICON_GREAT_WORK] Great Work of Art slots in selected Buildings and Wonders. Archaeologists may work in territory owned by any player. They are consumed once they complete an Archaeological Dig at an Antiquity Site. Archaeologists may not be purchased with [ICON_GOLD] Gold and may only be built in a City with a [COLOR_POSITIVE_TEXT]{TXT_KEY_BUILDING_MUSEUM}[ENDCOLOR].'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHAEOLOGIST';

-- Guided Missile
UPDATE Language_en_US
SET Text = 'A cheap Unit that may be used once to damage Enemy Units or Garrisoned Units in Cities. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Requires 1 [ICON_RES_OIL] Oil.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_GUIDED_MISSILE';

UPDATE Language_en_US
SET Text = 'The Guided Missile is a one-shot unit which is destroyed when it attacks an enemy target. The Guided Missile may be based in a player-owned friendly city or aboard an attack/nuclear submarine, sensor combat ship or missile cruiser. They may move from base to base or attack an enemy unit. See the rules on Missiles for more information.'
WHERE Tag = 'TXT_KEY_UNIT_GUIDED_MISSILE_STRATEGY';

-- Atomic Bomb
UPDATE Language_en_US
SET Text = 'Unleash nuclear destruction upon your foes. Does great damage to Cities and damages any Unit caught in the blast radius. May be stationed on Aircraft Carriers.[NEWLINE][NEWLINE]Requires [COLOR_POSITIVE_TEXT]Manhattan Project[ENDCOLOR] and 1 [ICON_RES_URANIUM] Uranium.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ATOMIC_BOMB';

UPDATE Language_en_US
SET Text = 'An Atomic Bomb is an extremely powerful unit. The Atomic Bomb can be based in a player-owned city or aboard a carrier. It can move from base to base or attack a target within its range of 6 tiles. When it detonates, an Atomic Bomb will damage or possibly destroy units, and cities will be severely damaged, within its blast radius of 2 tiles. It is automatically destroyed when it attacks. See the rules on Nuclear Weapons for more details.'
WHERE Tag = 'TXT_KEY_UNIT_ATOMIC_BOMB_STRATEGY';

-- Nuclear Missile
UPDATE Language_en_US
SET Text = 'Unleash nuclear destruction upon your foes. Does great damage to Cities and damages any Unit caught in the blast radius.[NEWLINE][NEWLINE]Requires [COLOR_POSITIVE_TEXT]Manhattan Project[ENDCOLOR] and 1 [ICON_RES_URANIUM] Uranium.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_NUCLEAR_MISSILE';

UPDATE Language_en_US
SET Text = 'The Nuclear Missile is an upgraded, more powerful Atomic Bomb. The Nuclear Missile can be based in any city you own or aboard a Nuclear Submarine or Missile Cruiser. It can move from base to base or attack a target within its range of 24 tiles. When it detonates, a Nuclear Missile will damage (and possibly destroy) cities and destroy all units within its blast radius of 2 tiles. It is automatically destroyed when it attacks. See the rules on Nuclear Weapons for more details.'
WHERE Tag = 'TXT_KEY_UNIT_NUCLEAR_MISSILE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Bomber'
WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER';

UPDATE Language_en_US
SET Text = 'Early Air Unit that can bombard Enemy Units and Cities from the skies. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_WWI_BOMBER';

UPDATE Language_en_US
SET Text = 'The Bomber is an early air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range. Use it to attack enemy units and cities. When possible, send in triplanes or fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_WWI_BOMBER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Heavy Bomber'
WHERE Tag = 'TXT_KEY_UNIT_BOMBER';

UPDATE Language_en_US
SET Text = 'Air Unit that rains death from above onto Enemy Units and Cities. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BOMBER';

UPDATE Language_en_US
SET Text = 'The Heavy Bomber is an air unit. It is effective against ground targets, less so against naval targets, and it is quite vulnerable to enemy aircraft. The Heavy Bomber can be based on a player-owned city or aboard a carrier. It can move from base to base and perform missions within its range. Use Bombers to attack enemy units and cities. When possible, send in fighters first to "use up" enemy anti-aircraft defenses for that turn. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_BOMBER_STRATEGY';

-- Stealth Bomber
UPDATE Language_en_US
SET Text = 'A long-range Air Unit that rains death from above onto Enemy Units and Cities without detection. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_STEALTH_BOMBER';

UPDATE Language_en_US
SET Text = 'The Stealth Bomber is an upgraded bomber, possessing increased range, a greater Ranged Combat Strength, and an increased ability to avoid enemy anti-aircraft and fighters. The Stealth Bomber has the "air recon" ability, which means that everything within 6 tiles of its starting location is visible at the beginning of the turn. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_STEALTH_BOMBER_STRATEGY';

-- Triplane
UPDATE Language_en_US
SET Text = 'Early Air Unit designed to intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIPLANE';

UPDATE Language_en_US
SET Text = 'The Triplane is an early air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from city to city (or carrier) and can perform "missions" within its range. Use triplanes to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_TRIPLANE_STRATEGY';

-- Fighter
UPDATE Language_en_US
SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_FIGHTER';

UPDATE Language_en_US
SET Text = 'The Fighter is a moderately-powerful air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from city to city (or carrier) and can perform "missions" within its range. Use fighters to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. Fighters are especially effective against enemy helicopters. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_FIGHTER_STRATEGY';

-- Jet Fighter
UPDATE Language_en_US
SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_JET_FIGHTER';

UPDATE Language_en_US
SET Text = 'The Jet Fighter is a powerful air unit. It can be based in any city you own or aboard an aircraft carrier. It can move from base to base and can perform "missions" within its range. Use Jet Fighters to attack enemy aircraft and ground units, to scout enemy positions, and to defend against enemy air attacks. Jet Fighters are especially effective against enemy helicopters. The Jet Fighter has the "air recon" ability, which means that everything within 6 tiles of its starting location is visible at the beginning of the turn. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_JET_FIGHTER_STRATEGY';

-- Archer
UPDATE Language_en_US
SET Text = 'Ancient Era unit with a Ranged attack.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARCHER';

UPDATE Language_en_US
SET Text = 'The Archer is an early ranged unit. It is the first in the unit line to have 2 range, allowing it to initiate fights and keep away from melee attackers. Use archers to soften up targets before a melee strike.'
WHERE Tag = 'TXT_KEY_UNIT_ARCHER_STRATEGY';

-- Spearman
UPDATE Language_en_US
SET Text = 'The Spearman is the first melee unit available after the Warrior. It is more powerful than the Warrior, and gets a significant combat bonus against mounted units (Chariot Archer, Horsemen and so forth).'
WHERE Tag = 'TXT_KEY_UNIT_SPEARMAN_STRATEGY';

-- Tercio
UPDATE Language_en_US
SET Text = 'The Tercio is a powerful Renaissance-Era Melee Unit. Use these Units to protect your fragile Ranged Units, and to capture Cities.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_HELP';

UPDATE Language_en_US
SET Text = 'This Tercio represents the advent of early gunpowder (''Pike and Shot'') tactics during the Renaissance Era. Fights at double-strength against mounted units, making them an effective counter to Lancers, as well as any Knights still lingering on the battlefield.'
WHERE Tag = 'TXT_KEY_CIV5_SPAIN_TERCIO_STRATEGY';

UPDATE Language_en_US
SET Text = 'Fusilier'
WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN';

UPDATE Language_en_US
SET Text = 'The Fusilier is the gunpowder unit following the Tercio. It is significantly more powerful than the Tercio, giving the army with the advanced units a big advantage over civs which have not yet upgraded to the new unit. Furthermore, as the first gunpowder melee unit, it comes equipped with promotions designed to help it hold and push the front lines of a fight.'
WHERE Tag = 'TXT_KEY_UNIT_RIFLEMAN_STRATEGY';

UPDATE Language_en_US
SET Text = 'Rifleman'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY';

UPDATE Language_en_US
SET Text = 'The Rifleman is the basic Modern era combat unit. It is significantly stronger than its predecessor, the Fusilier.'
WHERE Tag = 'TXT_KEY_UNIT_GREAT_WAR_INFANTRY_STRATEGY';

-- Infantry
UPDATE Language_en_US
SET Text = 'Infantry is the basic Industrial era combat unit. It is significantly stronger than its predecessor, the Rifleman. Modern combat is increasingly complex, and on its own an Infantry unit is vulnerable to air, artillery and tank attack. When possible Infantry should be supported by artillery, tanks, and air (or anti-air) units.'
WHERE Tag = 'TXT_KEY_UNIT_INFANTRY_STRATEGY';

-- Pathfinder
UPDATE Language_en_US
SET Text = '{TXT_KEY_UNITCOMBAT_RECON} such as this one are usually the only units allowed to uncover the hidden secrets of [COLOR_POSITIVE_TEXT]{TXT_KEY_IMPROVEMENT_GOODY_HUT}[ENDCOLOR]. Be wary of using them to fend off barbarians, however, as they are meant for exploring!'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SHOSHONE_PATHFINDER';

UPDATE Language_en_US
SET Text = 'The Pathfinder is the game''s first reconnaissance unit. It fights poorly compared to a Warrior, but has better movement and vision.'
WHERE Tag = 'TXT_KEY_UNIT_SHOSHONE_PATHFINDER_STRATEGY';

-- Paratrooper
UPDATE Language_en_US
SET Text = 'Strong infantry Unit capable of paradropping up to 9 tiles from friendly territory. Paratrooper can also move and pillage after paradropping, but cannot enter combat until the following turn. Has a combat bonus versus Siege units.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PARATROOPER';

UPDATE Language_en_US
SET Text = 'The Paratrooper is a late-game infantryman. It can parachute up to 9 tiles away (when starting in friendly territory). This allows the paratrooper to literally jump over enemy positions and destroy road networks, pillage vital resources and so forth, wrecking havoc behind his lines. The Paratrooper is at great risk when on such missions, so make sure the target is worth it!'
WHERE Tag = 'TXT_KEY_UNIT_PARATROOPER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Special Forces'
WHERE Tag = 'TXT_KEY_UNIT_MARINE';

UPDATE Language_en_US
SET Text = 'Information Era Unit especially useful for embarking and invading across the sea as well as taking out Gunpowder Units. Can also paradrop behind enemy lines.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MARINE';

UPDATE Language_en_US
SET Text = 'The Special Forces unit possesses promotions that enhance its Sight and attack strength when embarked at sea. It is also stronger versus Gunpowder Units, and can paradrop up to 9 tiles away from friendly territory.'
WHERE Tag = 'TXT_KEY_UNIT_MARINE_STRATEGY';

UPDATE Language_en_US
SET Text = 'Special forces and special operations forces are military units trained to perform unconventional missions. Special forces, as they would now be recognised, emerged in the early 20th century, with a significant growth in the field during the Second World War. Depending on the country, special forces may perform some of the following functions: airborne operations, counter-insurgency, "counter-terrorism", covert ops, direct action, hostage rescue, high-value targets/manhunting, intelligence operations, mobility operations, and unconventional warfare.'
WHERE Tag = 'TXT_KEY_CIV5_MARINE_TEXT';

-- Anti-Aircraft Gun
UPDATE Language_en_US
SET Text = 'These specialized artillery units will automatically attack any air unit bombing a target within 3 tiles. (They can only intercept one unit per turn.) They are quite weak in combat against other ground units and should be defended by stronger units when under threat of ground attack.'
WHERE Tag = 'TXT_KEY_UNIT_ANTI_AIRCRAFT_STRATEGY';

-- Mobile SAM
UPDATE Language_en_US
SET Text = 'Mobile SAM (surface-to-air) units provide an advancing army with anti-air defense. Mobile SAM units can intercept and shoot at enemy aircraft bombing targets within 4 hexes (but only one unit per turn). These units are fairly vulnerable to non-air attack and should be accompanied by infantry or armor.'
WHERE Tag = 'TXT_KEY_UNIT_MOBILE_SAM_STRATEGY';

-- Galley
UPDATE Language_en_US
SET Text = 'Galleys are the earliest naval unit. They are slow and weak, but can be used to establish an early naval presence. Use Galleys to protect your cities from early Barbarian incursions.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEY_STRATEGY';

UPDATE Language_en_US
SET Text = 'A galley is any type of ship that is mainly propelled by oars. Many galleys also used sails in favorable winds, but rowing with oars was relied on for maneuvering and for independence from wind power. The plan and size of galleys varied greatly from ancient times, but early vessels were often small enough to be picked up and carried onto shore when not in use, and were multipurpose vessels, used in both trade and warfare.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEY_PEDIA';

-- Frigate
UPDATE Language_en_US
SET Text = 'Powerful Renaissance Era Naval Unit used to wrest control of the seas.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal Tiles.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_FRIGATE';

UPDATE Language_en_US
SET Text = 'The Frigate is an upgrade over the Galleass. Its Range, Combat and Ranged Combat Strengths are much larger than the older naval unit. The Frigate can clear the seas of any Caravels, Triremes, and Barbarian units still afloat. It cannot, however, fire on non-Coastal Land Tiles.'
WHERE Tag = 'TXT_KEY_UNIT_FRIGATE_STRATEGY';

-- Galleass
UPDATE Language_en_US
SET Text = 'The Galleass is the second naval unit with a ranged attack available to the civilizations in the game. It is much stronger than earlier naval ships, and can enter the ocean. The Galleass is useful for clearing enemy ships out of shallow waters and supporting sieges.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEASS_STRATEGY';

UPDATE Language_en_US
SET Text = 'Corvette'
WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER';

UPDATE Language_en_US
SET Text = 'Naval Unit that specializes in melee combat and quick movement.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PRIVATEER';

UPDATE Language_en_US
SET Text = 'Naval Unit that attacks as a melee unit. It is significantly stronger than the Caravel.'
WHERE Tag = 'TXT_KEY_UNIT_PRIVATEER_STRATEGY';

UPDATE Language_en_US
SET Text = 'A corvette is a small warship. It is traditionally the smallest class of vessel considered to be a proper (or "rated") warship. The warship class above is that of frigate. The class below is historically sloop-of-war. The modern types of ship below a corvette are coastal patrol craft and fast attack craft. In modern terms, a corvette is typically between 500 tons and 2,000 tons although recent designs may approach 3,000 tons, which might instead be considered a small frigate.'
WHERE Tag = 'TXT_KEY_CIV5_PRIVATEER_TEXT';

-- Caravel
UPDATE Language_en_US
SET Text = 'Late-Medieval exploration Unit that can enter the Ocean. Fights as a naval melee unit.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CARAVEL';

UPDATE Language_en_US
SET Text = 'The Caravel is a significant upgrade to your naval power. A Melee unit, it is stronger and faster than the ageing Trireme, and it can enter Ocean Tiles. Use it to explore the world, or to defend your home cities.'
WHERE Tag = 'TXT_KEY_UNIT_CARAVEL_STRATEGY';

-- Ironclad
UPDATE Language_en_US
SET Text = 'A very powerful naval melee unit, the Ironclad dominates the oceans of the Industrial age.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_IRONCLAD';

-- Trireme
UPDATE Language_en_US
SET Text = 'Classical Era Naval Unit used to wrest control of the seas.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_TRIREME';

UPDATE Language_en_US
SET Text = 'The Trireme is the upgrade to the Galley. It is a melee attack unit, engaging naval units and coastal cities. The Trireme is good at clearing barbarian ships from your waters and for limited exploration (it cannot end its turn on Deep Ocean hexes outside of city borders unless you are Polynesia).'
WHERE Tag = 'TXT_KEY_UNIT_TRIREME_STRATEGY';

-- Carrier
UPDATE Language_en_US
SET Text = 'Initially carries 2 Aircraft; capacity may be boosted through promotions. Will intercept Enemy Aircraft which try to attack nearby Units.[NEWLINE][NEWLINE][COLOR_NEGATIVE_TEXT]Land attacks can only be performed on Coastal Tiles.[ENDCOLOR]'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CARRIER';

UPDATE Language_en_US
SET Text = 'The Carrier is a specialized vessel which carries fighter airplanes, bombers, and atomic bombs. The Carrier itself is unable to attack, but the aircraft it carries make it the most powerful offensive weapon afloat. Defensively, however, the Carrier is weak and should be escorted by destroyers and submarines. Carriers are, however, armed with anti-air weaporny, and will automatically attack any air unit bombing a target within 4 tiles. (They can only intercept one unit per turn.)'
WHERE Tag = 'TXT_KEY_UNIT_CARRIER_STRATEGY';

-- Battleship
UPDATE Language_en_US
SET Text = 'Very powerful Ranged Naval Unit. Starts with the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_INDIRECT_FIRE}[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BATTLESHIP';

UPDATE Language_en_US
SET Text = 'The Battleship is a powerful warship with a mighty Combat Strength. Its indirect fire ability allows it to bombard targets it cannot see (as long as other friendly units can see them). The Battleship is vulnerable to air and submarine attacks.'
WHERE Tag = 'TXT_KEY_UNIT_BATTLESHIP_STRATEGY';

-- Missile Cruiser
UPDATE Language_en_US
SET Text = 'The most powerful Naval Ranged unit with a solid shield. Starts with the [COLOR_POSITIVE_TEXT]{TXT_KEY_PROMOTION_INDIRECT_FIRE}[ENDCOLOR] Promotion and may carry up to 3 Missiles.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MISSILE_CRUISER';

UPDATE Language_en_US
SET Text = 'The Missile Cruiser is a modern warship. It''s fast and tough and carries a mean punch. Most importantly, the Missile Cruiser can carry Guided Missiles and Nuclear Missiles, allowing you to carry these deadly weapons right up to the enemy''s shore. Missile Cruisers combined with carriers, submarines, and destroyers make a fiendishly powerful armada.'
WHERE Tag = 'TXT_KEY_UNIT_MISSILE_CRUISER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Fleet Destroyer'
WHERE Tag = 'TXT_KEY_UNIT_DESTROYER';

UPDATE Language_en_US
SET Text = 'Fast late game ship making it highly effective at recon. Also used to hunt down and destroy enemy Submarines if promoted. Fights as a naval melee unit.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_DESTROYER';

UPDATE Language_en_US
SET Text = 'The Fleet Destroyer is a naval melee attack unit designed to hunt down submarines and to provide anti-aircraft support.'
WHERE Tag = 'TXT_KEY_UNIT_DESTROYER_STRATEGY';

UPDATE Language_en_US
SET Text = 'Skirmisher'
WHERE Tag = 'TXT_KEY_UNIT_MONGOL_KESHIK';

UPDATE Language_en_US
SET Text = 'Skirmishers possess a ranged attack, an increased movement rate, and increased flanking potential, allowing them to perform hit and run attacks or support the main force.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_HELP';

UPDATE Language_en_US
SET Text = 'Skirmishers are fast ranged units, deadly on open terrain. Unlike the Chariot before them, they can move through rough terrain without a movement penalty and can strengthen the attacks of flanking units. As a mounted unit, the Skirmisher is vulnerable to units equipped with spears.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_STRATEGY';

UPDATE Language_en_US
SET Text = 'In the 12th century, nomadic tribes came boiling out of Central Asia, conquering most of Asia, Eastern Europe and a large portion of the Middle East within a century. Their primary weapon was their incomparable mounted bowmen. The Mongols were one such nomadic tribe, and their children were almost literally "raised in the saddle." Riding on their small but hearty steppe ponies, the lightly-armed and armored Mongol Keshiks, a type of skirmisher, could cover an astonishing amount of territory in a day, far outpacing any infantry or even the heavier European cavalry.[NEWLINE][NEWLINE]In battle the Mongol Keshiks would shoot from horseback with deadly accuracy, disrupting and demoralizing the enemy formations. Once the enemy was suitably weakened, the Mongol heavy cavalry units would charge into the wavering foe to deliver the coup de grace. When facing armored European knights the Mongols would simply shoot their horses, then ignore or destroy the unhorsed men as they wished.'
WHERE Tag = 'TXT_KEY_CIV5_MONGOLIA_KESHIK_TEXT';

-- Lancer
UPDATE Language_en_US
SET Text = 'Mounted Unit designed to be on the attack at all times. Good for hunting down enemy ranged units, and for skirmishing on the edges of the battlefield.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LANCER';

UPDATE Language_en_US
SET Text = 'The Lancer is the Renaissance horse unit that comes between the Knight and the first mechanized vehicle, the Landship. It is faster and more powerful than the Knight, able to sweep those once-mighty units from the map. The Lancer is a powerful offensive weapon.'
WHERE Tag = 'TXT_KEY_UNIT_LANCER_STRATEGY';

-- Cavalry
UPDATE Language_en_US
SET Text = 'Ranged Industrial-Era mounted unit, useful for flanking, harassment, and hit-and-run tactics.'
WHERE Tag = 'TXT_KEY_UNIT_CAVALRY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Light Tank'
WHERE Tag = 'TXT_KEY_UNIT_AT_GUN';

UPDATE Language_en_US
SET Text = 'Highly-mobile ranged unit specialized in hit-and-run tactics and skirmishing.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ANTI_TANK_GUN';

UPDATE Language_en_US
SET Text = 'The Light Tank is a specialized combat unit designed for hit-and-run tactics. Back them up with Riflemen, Tanks, and Artillery for a potent Modern Era fighting force.'
WHERE Tag = 'TXT_KEY_UNIT_AT_GUN_STRATEGY';

UPDATE Language_en_US
SET Text = 'A light tank is a tank variant initially designed for rapid movement, and now primarily employed in the reconnaissance role, or in support of expeditionary forces where main battle tanks cannot be made available. Early light tanks were generally armed and armored similar to an armored car, but used tracks in order to provide better cross-country mobility. The fast light tank was a major feature of the pre-WWII buildup, where it was expected they would be used to exploit breakthroughs in enemy lines created by slower, heavier tanks. Numerous small tank designs and "tankettes" were developed during this period and known under a variety of names, including the ''combat car''.'
WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_ANTITANKGUN_TEXT';

-- Helicopter Gunship
UPDATE Language_en_US
SET Text = 'Unit specialized in fighting Modern Armor and Tanks. It is capable of hovering over Mountains and Coast.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HELICOPTER_GUNSHIP';

-- Musketman
UPDATE Language_en_US
SET Text = 'First ranged gunpowder Unit of the game. Fairly cheap and powerful.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MUSKETMAN';

UPDATE Language_en_US
SET Text = 'The Musketman is the first ranged gunpowder unit in the game, and it replaces all of the older ranged foot-soldier types - Crossbowmen, Archers, and the like. Because it is a ranged unit, it can attack an enemy that is up to two hexes away.'
WHERE Tag = 'TXT_KEY_UNIT_MUSKETMAN_STRATEGY';

-- Gatling Gun
UPDATE Language_en_US
SET Text = 'Ranged infantry Unit of the mid-game that weakens nearby enemy units.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_GATLINGGUN';

UPDATE Language_en_US
SET Text = 'The Gatling Gun is a mid-game non-siege ranged unit that can lay down a terrifying hail of bullets. It is much more powerful than earlier ranged units like the Musketman, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. It weakens nearby enemy units, and gains bonus strength when defending. When attacking, the Gatling Gun deals less damage to Armored or fortified Units, as well as cities. Put Gatling Guns in your cities or on chokepoints for optimal defensive power.'
WHERE Tag = 'TXT_KEY_UNIT_GATLINGGUN_STRATEGY';

-- Machine Gun
UPDATE Language_en_US
SET Text = 'Late-game ranged Unit that weakens nearby enemy units.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MACHINE_GUN';

UPDATE Language_en_US
SET Text = 'The Machine Gun is the penultimate non-siege ranged unit, and can lay down a terrifying hail of suppressive fire. It is more powerful than earlier ranged units, but it is much weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Gatling Gun, it weakens nearby enemy units. When attacking, the Machine Gun deals less damage to Armored or fortified Units, as well as cities. It is vulnerable to melee attack. Put Machine Guns in your city for defense, or use them to control chokepoints.'
WHERE Tag = 'TXT_KEY_UNIT_MACHINE_GUN_STRATEGY';

-- Bazooka
UPDATE Language_en_US
SET Text = 'Information era ranged unit. Deals great damage to Armored Units.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BAZOOKA';

UPDATE Language_en_US
SET Text = 'The Bazooka is the last non-siege ranged unit, and is capable of truly terrfiying amounts of damage, especially to Armored units. Boasting a long range of 3, it is the most powerful ranged unit, but it is slower and defensively weaker than other military units of its era. As such, it should be used as a source of attrition. Like the Machine Gun, it weakens nearby enemy units. When attacking, the Bazooka deals less damage to fortified Units and cities, but deals additional damage to Armored units. This makes it a great defense unit.'
WHERE Tag = 'TXT_KEY_UNIT_BAZOOKA_STRATEGY';

-- Artillery
UPDATE Language_en_US
SET Text = 'Capable of firing 3 tiles away. Must set up prior to firing.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Artillery is a deadly siege weapon, more powerful than a cannon and with a longer range. Like the cannon it has limited visibility and must set up (1 mp) to attack, but its Ranged Combat strength is tremendous. Artillery also has the "indirect fire" ability, allowing it to shoot over obstructions at targets it cannot see (as long as other friendly units can see them). Like other siege weapons, Artillery is vulnerable to melee attack.'
WHERE Tag = 'TXT_KEY_UNIT_ARTILLERY_STRATEGY';

-- B17
UPDATE Language_en_US
SET Text = 'Air Unit that rains death from above onto Enemy Units and Cities. This Unit has a chance of evading interception, and does additional damage to Cities compared to the Bomber. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Freedom[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Their Finest Hour[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_B17';

UPDATE Language_en_US
SET Text = 'The B17 Bomber is unique to the Freedom Ideology. Requires the Their Finest Hour Tenet to be unlocked. It is similar to the bomber, but it is more difficult for enemy anti-aircraft and fighters to target. The B17 also receives a bonus when attacking enemy cities. Unlike the bomber, its range is 10. See the rules on Aircraft for more details.'
WHERE Tag = 'TXT_KEY_UNIT_AMERICAN_B17_STRATEGY';

-- Pracinha
UPDATE Language_en_US
SET Text = 'The Pracinha can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BRAZILIAN_PRACINHA';

UPDATE Language_en_US
SET Text = 'This Atomic Era melee unit can use its Survivalism I promotion to help it survive on the front line. When the opportunity presents itself, it can defeat a weakened enemy to earn points toward starting another [ICON_GOLDEN_AGE] Golden Age.'
WHERE Tag = 'TXT_KEY_UNIT_BRAZILIAN_PRACINHA_STRATEGY';

-- Dromon
UPDATE Language_en_US
SET Text = 'The Dromon can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BYZANTINE_DROMON';

UPDATE Language_en_US
SET Text = 'The Dromon performs similarly to the Liburna, but is much more powerful against units. Starts with the [COLOR_POSITIVE_TEXT]Splash I[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Targeting I[ENDCOLOR] Promotions.'
WHERE Tag = 'TXT_KEY_UNIT_BYZANTINE_DROMON_STRATEGY';

-- Norwegian Ski Infantry
UPDATE Language_en_US
SET Text = 'The Norwegian Ski Infantry can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_HELP';

UPDATE Language_en_US
SET Text = 'The Norwegian Ski Infantry moves through Snow, Tundra, and Hills at double speed. It also has +25% [ICON_STRENGTH] Combat Strength in Snow, Tundra or Hill terrain if neither Forest nor Jungle are present.'
WHERE Tag = 'TXT_KEY_CIV5_DENMARK_SKI_INFANTRY_STRATEGY';

-- Longbowman
UPDATE Language_en_US
SET Text = 'The Longbowman can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_LONGBOWMAN';

UPDATE Language_en_US
SET Text = ' The Longbowman has a greater range than the Crossbowman, allowing it to attack enemies three hexes away, often destroying them before they can strike back. Like other ranged units, Longbowmen are vulnerable to melee attack, but have increased [ICON_STRENGTH] against Mounted and Armored Units.'
WHERE Tag = 'TXT_KEY_UNIT_ENGLISH_LONGBOWMAN_STRATEGY';

-- Foreign Legion
UPDATE Language_en_US
SET Text = 'Special Infantry Unit of the Industrial Era. This Unit has a combat bonus outside of friendly territory but is otherwise very similar to Fusiliers. Receives full XP from the City when purchased.[NEWLINE][NEWLINE]May only be purchased with [ICON_GOLD] Gold after completing the [COLOR_MAGENTA]Authority[ENDCOLOR] Policy Branch.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_FOREIGN_LEGION';

UPDATE Language_en_US
SET Text = 'The Foreign Legion is a special Industrial Era unit that may only be purchased with [ICON_GOLD] Gold after completing the Authority Policy Branch. It receives a significant combat bonus when operating outside of home territory, making it an excellent unit to use to gain control of foreign lands.'
WHERE Tag = 'TXT_KEY_UNIT_FRENCH_FOREIGNLEGION_STRATEGY';

UPDATE Language_en_US
SET Text = 'Established in 1831, the French Foreign Legion is a unique infantry unit in the French army. The legion was specifically created for foreign nationals who wished to fight for France. Famously courageous and totally indifferent to personal safety, the Foreign Legion have seen service in every French war since their inception. The unit has been highly romanticized - according to popular culture, it is a place where heartbroken men go to forget women and scoundrels go to escape justice. This may or may not be accurate, but whatever the case the Legion is a tough and very effective fighting force.[NEWLINE][NEWLINE]The practice of recruiting foreign nationals into its own corps has been emulated by other countries, such as the Dutch KNIL established in 1814, the Chinese Ever Victorious Army in 1860, the Spanish Foreign Legion in 1920, and the Israeli Mahal in 1948.'
WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_FOREIGNLEGION_TEXT';

UPDATE Language_en_US
SET Text = 'T-34'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_PANZER';

UPDATE Language_en_US
SET Text = 'Unique Land Unit that is incredibly effective on open ground.[NEWLINE][NEWLINE]Starts with the [COLOR_POSITIVE_TEXT]Armor Plating I[ENDCOLOR] and [COLOR_POSITIVE_TEXT]Mobility[ENDCOLOR] promotions.[NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Order[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Patriotic War[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_PANZER';

UPDATE Language_en_US
SET Text = 'The T-34 is unique to the Order Ideology. Requires the Patriotic War Tenet to be unlocked. The T-34 is stronger, faster, and has more defense than the tank. It can move after combat, allowing it to blow huge holes in enemy lines and then barrel through before the enemy can repair the gap.'
WHERE Tag = 'TXT_KEY_UNIT_GERMAN_PANZER_STRATEGY';

UPDATE Language_en_US
SET Text = 'The T-34 was the main medium tank of the Soviet Red Army during World War II. One of the most influential tank designs in the world, the T-34 combined strong, sloped armor, a powerful gun, speed, and cross-country reliability, totally outmatching any German tank that was sent to fight against them. With its proven design combined with the Soviets'' overwhelming industrial capacity, the T-34 also became one of the most cost effective and most produced tanks in history, with over 80,000 built. Even today, numerous countries use T-34s in their armored divisions. Truly a tank that could stand the test of time!'
WHERE Tag = 'TXT_KEY_CIV5_INDUSTRIAL_PANZER_TEXT';

-- Companion Cavalry
UPDATE Language_en_US
SET Text = 'The Companion Cavalry can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_COMPANION_CAVALRY';

UPDATE Language_en_US
SET Text = 'Companion Cavalry are faster and more powerful than the Horseman unit, making them the most dangerous mounted units until the arrival of the Knight. They help generate Great Generals more rapidly than other units, and benefit greatly from being stacked with one. Use a stacked Great General''s increased movement speed to keep up with its Companion Cavalry retinue.'
WHERE Tag = 'TXT_KEY_UNIT_GREEK_COMPANIONCAVALRY_STRATEGY';

-- Battering Ram
UPDATE Language_en_US
SET Text = 'The Battering Ram can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_HUN_BATTERING_RAM';

UPDATE Language_en_US
SET Text = 'After defenders have been cleared away, use Battering Rams to quickly move to an enemy City and knock down its defenses with melee attacks. Battering Rams exist for a long time and become increasingly vulnerable to the units of newer eras, so keep that in mind when choosing new promotions for it.'
WHERE Tag = 'TXT_KEY_UNIT_HUN_BATTERING_RAM_STRATEGY';

-- Zero
UPDATE Language_en_US
SET Text = 'Air Unit designed to wrest control of the skies and intercept incoming Enemy Aircraft. This Unit has a bonus against other Fighters and does not require Oil. [COLOR_POSITIVE_TEXT]Does not use Military Supply.[ENDCOLOR][NEWLINE][NEWLINE]Only available to Civilizations following the [COLOR_MAGENTA]Autocracy[ENDCOLOR] Ideology. Requires the [COLOR_MAGENTA]Air Supremacy[ENDCOLOR] Tenet to be unlocked.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ZERO';

UPDATE Language_en_US
SET Text = 'The Zero is unique to the Autocracy Ideology. Requires the Air Supremacy Tenet to be unlocked. The Zero is a moderately-powerful air unit. It is like the standard fighter, except that it gets a significant combat bonus when battling other fighters and [COLOR_POSITIVE_TEXT]does not require the Oil resource[ENDCOLOR]. It can be based in any City you own or aboard an aircraft carrier. It can move from one City to another (or an aircraft carrier) and can perform "missions" within its range of 8 tiles. See the rules on Aircraft for more information.'
WHERE Tag = 'TXT_KEY_UNIT_JAPANESE_ZERO_STRATEGY';

-- Turtle Ship
UPDATE Language_en_US
SET Text = 'The Turtle Ship can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_HELP';

UPDATE Language_en_US
SET Text = 'The Turtle Ship has a more powerful attack than the Caravel, and is extremely difficult to destroy due to its Indomitable promotion. However, while it can end its movement in Ocean tiles, the Turtle Ship still travels at half speed through them.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_TURTLESHIP_STRATEGY';

-- Sipahi
UPDATE Language_en_US
SET Text = 'The Sipahi can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SIPAHI';

UPDATE Language_en_US
SET Text = 'The Sipahi is stronger than the Lancer and has massive flanking bonuses. Pin your opponent''s army down using your Gunpowder Units'' Zone Of Control while attacking with your Sipahi from the side. Defeat weakened units to trigger the Overrun bonus damage, then retreat to safety.'
WHERE Tag = 'TXT_KEY_UNIT_OTTOMAN_SIPAHI_STRATEGY';

-- Ballista
UPDATE Language_en_US
SET Text = 'The Ballista can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA';

UPDATE Language_en_US
SET Text = 'The Ballista is an excellent siege weapon. While still very useful against cities, its lack of penalties against land units and free Field I promotion gives it an extra punch against other units. The Ballista is still quite vulnerable to attack, so be sure to protect it with other military units. Moves at half speed in enemy territory, but has normal sight range.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY';

-- Hakkapeliitta
UPDATE Language_en_US
SET Text = 'The Hakkapeliitta can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_SWEDISH_HAKKAPELIITTA';

UPDATE Language_en_US
SET Text = 'The Hakkapeliitta is much faster and can see one hex farther than the Lancer. It pillages enemy improvements at no additional cost, so foray into enemy lands to knock out key improvements, using its improved sight and speed to minimize retaliatory attacks. Keep the unit healthy by pillaging and defeating weakened units, as it has lower [ICON_STRENGTH] Combat Strength than its contemporary Lancers.'
WHERE Tag = 'TXT_KEY_UNIT_SWEDISH_HAKKAPELIITTA_STRATEGY';

-- Great Galleass
UPDATE Language_en_US
SET Text = 'The Great Galleass can only be gifted by City-States.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_VENETIAN_GALLEASS';

UPDATE Language_en_US
SET Text = 'The Great Galleass performs similarly to the Galleass, but is more resilient in battle and has a stronger ranged attack. Starts with the [COLOR_POSITIVE_TEXT]Bombardment I[ENDCOLOR] Promotion.'
WHERE Tag = 'TXT_KEY_UNIT_VENETIAN_GALLEASS_STRATEGY';
