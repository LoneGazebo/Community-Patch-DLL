UPDATE Language_en_US
SET Text = 'The Trireme is the first naval unit available to civilizations in the game. It is a melee attack unit, engaging units at sea or cities (but it has no ranged attack). The Trireme is useful for clearing barbarian ships out of home waters and for limited exploration (it cannot end its turn on Deep Ocean hexes outside of the city borders unless it is from Polynesia).'
WHERE Tag = 'TXT_KEY_UNIT_TRIREME_STRATEGY';

UPDATE Language_en_US
SET Text = 'The Galleass is the first naval unit with a ranged attack generally available to the civilizations in the game. It is much stronger than earlier naval units but a bit slower. It cannot initiate melee combat. The Galleass is useful for clearing enemy ships out of shallow waters. It cannot end its turn on Deep Ocean hexes unless it is from Polynesia.'
WHERE Tag = 'TXT_KEY_UNIT_GALLEASS_STRATEGY';

UPDATE Language_en_US
SET Text = 'First siege Unit of the game. Does major damage to Units and Cities from a distance. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CATAPULT';

UPDATE Language_en_US
SET Text = 'The Catapult is a siege weapon, extremely useful when assaulting early enemy cities. It is slow and extremely vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. It moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_CATAPULT_STRATEGY';

UPDATE Language_en_US
SET Text = 'Siege Unit that does major damage from a distance. Moves at half-speed in enemy territory. Only the Romans may build it. This Unit has a higher [ICON_RANGE_STRENGTH] Ranged Combat Strength than the Catapult, which it replaces.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_BALLISTA';

UPDATE Language_en_US
SET Text = 'The Ballista is a Roman unique unit that is more powerful than the Catapult it replaces. It is an excellent siege weapon and extremely useful against cities, but it is quite vulnerable to attack. Be sure to protect the Ballista with other military units. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_ROMAN_BALLISTA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Powerful Medieval Siege Unit, good at reducing the defenses of enemy Cities. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_TREBUCHET';

UPDATE Language_en_US
SET Text = 'A Trebuchet is a medieval siege weapon, significantly more powerful than the earlier Catapult units. The Trebuchet moves at half-speed in enemy territory, but once it is in position it can do serious damage to a fortified enemy city. Defend your trebuchets with melee and ranged units to keep enemy units from getting too close.'
WHERE Tag = 'TXT_KEY_UNIT_TREBUCHET_STRATEGY';

UPDATE Language_en_US
SET Text = 'Anti-personnel rocket unit of the Medieval Era. Strong against land units, but weak against cities. Only Korea may build it. Replaces the Trebuchet. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_HELP';

UPDATE Language_en_US
SET Text = 'The Hwach''a is extremely effective against enemy land units. It is slow and vulnerable to enemy melee attack; it always should be supported by other units when it''s in the field. The Hwach''a moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_CIV5_KOREA_HWACHA_STRATEGY';

UPDATE Language_en_US
SET Text = 'Powerful Renaissance Era Siege Unit. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_CANNON';

UPDATE Language_en_US
SET Text = 'The Cannon is an important mid-game siege unit, devastating against civilizations which have not yet acquired them. Like other siege units, the Cannon moves at half-speed in enemy territory. The Cannon remains potent into the Industrial Era. It cannot see as far as other units, and should be protected with melee units.'
WHERE Tag = 'TXT_KEY_UNIT_CANNON_STRATEGY';

UPDATE Language_en_US
SET Text = 'The first Siege Unit in the game capable of firing 3 tiles away. Moves at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Artillery is a deadly siege weapon, more powerful than a Cannon and with a longer range. Like the Cannon, it has limited visibility and moves at half-speed in enemy territory, but its Ranged Combat strength is tremendous. Artillery also has the "indirect fire" ability, allowing it to shoot over obstructions at targets it cannot see (as long as other friendly units can see them). Like other siege weapons, Artillery is vulnerable to melee attack.'
WHERE Tag = 'TXT_KEY_UNIT_ARTILLERY_STRATEGY';

UPDATE Language_en_US
SET Text = 'Fast Siege Unit of the late-game. Unlike other Siege Units, it does not move at half-speed in enemy territory.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_ROCKET_ARTILLERY';

UPDATE Language_en_US
SET Text = 'Rocket Artillery is an extremely powerful siege weapon, far superior to the standard artillery. Rocket Artillery does not lose speed in enemy territory, making it much quicker than earlier guns. Although the Rocket Artillery''s sight range is limited, it can use indirect fire to attack targets it cannot see (as long as other friendly units can see them). Like all artillery, it is vulnerable to infantry or armor attack, and should be defended by appropriate units.'
WHERE Tag = 'TXT_KEY_UNIT_ROCKET_ARTILLERY_STRATEGY';

-- Giant Death Robot
UPDATE Language_en_US
SET Text = 'Doesn''t benefit from defensive terrain bonuses, but is otherwise stronger than any other military unit in the game.'
WHERE Tag = 'TXT_KEY_UNIT_HELP_MECH';

UPDATE Language_en_US
SET Text = 'Can be purchased with [ICON_PEACE] Faith in any city with a majority Religion that has been enhanced. They can remove other religions from your cities (expending the Inquisitor) or be placed inside or adjacent to a city to protect it from Missionaries and Prophets trying to spread other religions into that city.'
WHERE Tag = 'TXT_KEY_UNIT_INQUISITOR_STRATEGY';
