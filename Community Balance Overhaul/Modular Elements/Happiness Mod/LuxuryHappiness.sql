-- resources give two happiness each
UPDATE Resources SET Happiness = '2' WHERE Happiness > '0' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LUXURY_HAPPINESS' AND Value= 1 );