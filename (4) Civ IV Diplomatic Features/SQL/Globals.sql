-------------------------------------------------------------------------------------------------------------------
-- Globals

-- Sharing Opinions
INSERT INTO	Defines	('Name', 'Value')	VALUES	('SHARE_OPINION_TURN_BUFFER', 20);							-- How many turns after we meet a civilization must we wait before they'll share their opinion?
-- Help Requests
INSERT INTO	Defines ('Name', 'Value')	VALUES	('HELP_REQUEST_TURN_LIMIT_MIN', 20);						-- Base turns before Help Request can be accepted again. Added by the Rand.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('HELP_REQUEST_TURN_LIMIT_RAND', 10);						-- Randomized number of turns before Help Request can be accepted again.
-- Technology Trading
INSERT INTO	Defines ('Name', 'Value')	VALUES	('TECH_COST_ERA_EXPONENT', 0.7);							-- Additional cost per era.
-- Vassalage
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_HAPPINESS_PERCENT', 20);							-- Masters get a happiness boost per pop. of a vassal.
INSERT INTO	Defines	('Name', 'Value')	VALUES	('VASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT', 20);			-- Percent from culture per turn from Vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_SCORE_PERCENT', 50);								-- How much of our Vassal's Land/Population score do we get?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_CITY_POP_EXPONENT', 0.8);				-- How much does each population affect your vassal maintenance?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT', 10);			-- How much does the master pay for the vassal's units?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_TOURISM_MODIFIER', 33);							-- How much extra pressure does the Master's tourism exert on the Vassal?
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_PROTECTED_CITY_DISTANCE', 6);				    -- How close to a vassal city must an enemy unit be for it to count for protect value.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_FAILED_PROTECT_CITY_DISTANCE', 0);				-- How close to a vassal city must a vassal unit be for it to count for failed protect value.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_LOST_CITIES_THRESHOLD', 75);				-- Percentage of cities that must be lost before Vassalage can be ended.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_POPULATION_THRESHOLD', 300);				-- Percentage of population over what we started with before Vassalage can be ended.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD', 60);		-- Percentage of cities that must be 
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD', 60);		-- Percentage of cities that must be lost before Vassalage can be ended.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_CAPITULATE_BASE_THRESHOLD', 100);				-- How likely are we to capitulate?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM', 0);				-- Minimum percent we can tax a vassal. Negative values may cause undefined behavior.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM', 25);				-- Maximum percent we can tax a vassal's income. Values greater than 100% will do nothing. Make sure this is a multiple of 5 please!