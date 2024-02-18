-----------------------------------------------------------------------------------------------
-- Maritime: Food on capital tile and other city tiles x100 (but use only even multiples!)
-- Only has pre- and post-Renaissance values, and same value for allies throughout the game
-----------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 300 WHERE Name = 'FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE';
UPDATE Defines SET Value = 100 WHERE Name = 'FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_PRE_RENAISSANCE';

UPDATE Defines SET Value = 600 WHERE Name = 'FRIENDS_CAPITAL_FOOD_BONUS_AMOUNT_POST_RENAISSANCE';
UPDATE Defines SET Value = 100 WHERE Name = 'FRIENDS_OTHER_CITIES_FOOD_BONUS_AMOUNT_POST_RENAISSANCE';

UPDATE Defines SET Value = 200 WHERE Name = 'ALLIES_CAPITAL_FOOD_BONUS_AMOUNT';
UPDATE Defines SET Value = 200 WHERE Name = 'ALLIES_OTHER_CITIES_FOOD_BONUS_AMOUNT';

-----------------------------------------------------------------------------------------------
-- Merchantile: Gold on empire
-- Different yields per era up to Industrial
-----------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 1 WHERE Name = 'FRIENDS_GOLD_FLAT_BONUS_AMOUNT_ANCIENT';
UPDATE Defines SET Value = 2 WHERE Name = 'ALLIES_GOLD_FLAT_BONUS_AMOUNT_ANCIENT';

UPDATE Defines SET Value = 1 WHERE Name = 'FRIENDS_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL';
UPDATE Defines SET Value = 3 WHERE Name = 'ALLIES_GOLD_FLAT_BONUS_AMOUNT_CLASSICAL';

UPDATE Defines SET Value = 2 WHERE Name = 'FRIENDS_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL';
UPDATE Defines SET Value = 4 WHERE Name = 'ALLIES_GOLD_FLAT_BONUS_AMOUNT_MEDIEVAL';

UPDATE Defines SET Value = 3 WHERE Name = 'FRIENDS_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE';
UPDATE Defines SET Value = 5 WHERE Name = 'ALLIES_GOLD_FLAT_BONUS_AMOUNT_RENAISSANCE';

UPDATE Defines SET Value = 4 WHERE Name = 'FRIENDS_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL';
UPDATE Defines SET Value = 6 WHERE Name = 'ALLIES_GOLD_FLAT_BONUS_AMOUNT_INDUSTRIAL';

-----------------------------------------------------------------------------------------------
-- Militaristic: Science on empire
-- Different yields per era up to Industrial
-----------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 1 WHERE Name = 'FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT';
UPDATE Defines SET Value = 3 WHERE Name = 'ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_ANCIENT';

UPDATE Defines SET Value = 2 WHERE Name = 'FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL';
UPDATE Defines SET Value = 4 WHERE Name = 'ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_CLASSICAL';

UPDATE Defines SET Value = 4 WHERE Name = 'FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL';
UPDATE Defines SET Value = 6 WHERE Name = 'ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_MEDIEVAL';

UPDATE Defines SET Value = 6 WHERE Name = 'FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE';
UPDATE Defines SET Value = 8 WHERE Name = 'ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_RENAISSANCE';

UPDATE Defines SET Value = 10 WHERE Name = 'FRIENDS_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL';
UPDATE Defines SET Value = 10 WHERE Name = 'ALLIES_SCIENCE_FLAT_BONUS_AMOUNT_INDUSTRIAL';

-----------------------------------------------------------------------------------------------
-- Cultural: Culture on empire
-- Yields updated on Medieval and Industrial only (balancing is hard)
-----------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 1 WHERE Name = 'FRIENDS_CULTURE_BONUS_AMOUNT_ANCIENT';
UPDATE Defines SET Value = 2 WHERE Name = 'ALLIES_CULTURE_BONUS_AMOUNT_ANCIENT';

UPDATE Defines SET Value = 4 WHERE Name = 'FRIENDS_CULTURE_BONUS_AMOUNT_MEDIEVAL';
UPDATE Defines SET Value = 6 WHERE Name = 'ALLIES_CULTURE_BONUS_AMOUNT_MEDIEVAL';

UPDATE Defines SET Value = 10 WHERE Name = 'FRIENDS_CULTURE_BONUS_AMOUNT_INDUSTRIAL';
UPDATE Defines SET Value = 12 WHERE Name = 'ALLIES_CULTURE_BONUS_AMOUNT_INDUSTRIAL';

-----------------------------------------------------------------------------------------------
-- Religious: Faith on empire
-- Different yields per era up to Industrial
-----------------------------------------------------------------------------------------------
UPDATE Defines SET Value = 1 WHERE Name = 'FRIENDS_FAITH_FLAT_BONUS_AMOUNT_ANCIENT';
UPDATE Defines SET Value = 2 WHERE Name = 'ALLIES_FAITH_FLAT_BONUS_AMOUNT_ANCIENT';

UPDATE Defines SET Value = 2 WHERE Name = 'FRIENDS_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL';
UPDATE Defines SET Value = 3 WHERE Name = 'ALLIES_FAITH_FLAT_BONUS_AMOUNT_CLASSICAL';

UPDATE Defines SET Value = 7 WHERE Name = 'FRIENDS_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL';
UPDATE Defines SET Value = 7 WHERE Name = 'ALLIES_FAITH_FLAT_BONUS_AMOUNT_MEDIEVAL';

UPDATE Defines SET Value = 9 WHERE Name = 'FRIENDS_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE';
UPDATE Defines SET Value = 9 WHERE Name = 'ALLIES_FAITH_FLAT_BONUS_AMOUNT_RENAISSANCE';

UPDATE Defines SET Value = 12 WHERE Name = 'FRIENDS_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL';
UPDATE Defines SET Value = 12 WHERE Name = 'ALLIES_FAITH_FLAT_BONUS_AMOUNT_INDUSTRIAL';
