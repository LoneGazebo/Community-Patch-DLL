
-- Scaler for bonus happiness from luxuies. Sets the base value, and the base 'jump' from tier to tier.
	UPDATE Defines
	SET Value = '40'
	WHERE Name = 'BALANCE_HAPPINESS_LUXURY_BASE' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LUXURY_HAPPINESS' AND Value= 1 );

	-- Scaler defines how many cities count against the bonus pop - higher = you can expand more.
	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_POPULATION_DIVISOR', '1'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );	

	INSERT INTO Defines (
	Name, Value)
	SELECT 'BALANCE_HAPPINESS_LUXURY_SCALER', '3'
	WHERE EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_CITY_HAPPINESS' AND Value= 1 );	

	UPDATE Resources
	SET Happiness = '1'
	WHERE Happiness = '4' AND EXISTS (SELECT * FROM COMMUNITY WHERE Type='COMMUNITY_CORE_BALANCE_LUXURY_HAPPINESS' AND Value= 1 );
