-- NOTE TO MODDERS. THE FAILURE OF THIS FILE TO LAUNCH CORRECTLY RESULTS IN DIVISION BY ZERO CRASHES. BE SMART. BE SAFE. HAVE A GOOD DAY.
-- LIST OF CRASHES I'VE EVER HAD:
	-- Division by zero
	-- Infinite recursion (fun!)
	-- Saving/loading corrupted saves (due to modified serialization)
-- Don't make my mistakes! Don't let this happen! ^^^

-------------------------------------------------------------------------------------------------------------------
-- Globals

-- Sharing Opinions
INSERT INTO	Defines	('Name', 'Value')	VALUES	('SHARE_OPINION_RAND', 100);								-- Random number to determine if AI will agree to share opinion to player.
INSERT INTO	Defines	('Name', 'Value')	VALUES	('SHARE_OPINION_FLAVOR_BASE', 2);							-- Minimum DiploBalance needed for AI to share opinion.
INSERT INTO	Defines	('Name', 'Value')	VALUES	('SHARE_OPINION_FLAVOR_MULTIPLIER', 10);					-- Multipied by (DiploBalance - FLAVOR_BASE) to determine initial percent AI will accept Share Opinion.
INSERT INTO	Defines	('Name', 'Value')	VALUES	('SHARE_OPINION_TURN_BUFFER', 20);							-- How many turns after we meet a civilization must we wait before they'll share their opinion?
-- Help Requests
INSERT INTO	Defines ('Name', 'Value')	VALUES	('HELP_REQUEST_TURN_LIMIT_MIN', 20);						-- Base turns before Help Request can be accepted again. Added by the Rand.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('HELP_REQUEST_TURN_LIMIT_RAND', 10);						-- Randomized number of turns before Help Request can be accepted again.
-- Technology Trading
INSERT INTO	Defines ('Name', 'Value')	VALUES	('TECH_COST_ERA_EXPONENT', 0.7);							-- Additional cost per era.
-- Vassalage
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_SCIENCE_PERCENT', 10);								-- How much of our master's (or multiple masters'?) science does our vassal get? note that with teams, the calc. is average of science * 10 / 100 --- NOT USED
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_HAPPINESS_PERCENT', 20);							-- Masters get a happiness boost per pop. of a vassal.
INSERT INTO	Defines	('Name', 'Value')	VALUES	('VASSALAGE_FREE_YIELD_FROM_VASSAL_PERCENT', 20);			-- Percent from culture per turn from Vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_SCORE_PERCENT', 50);								-- How much of our Vassal's Land/Population score do we get?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_CITY_POP_EXPONENT', 0.8);				-- How much does each population affect your vassal maintenance?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT', 10);			-- How much does the master pay for the vassal's units?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_RELIGIOUS_PRESSURE_MODIFIER', 33);					-- How much extra pressure does the Master's religion exert on the Vassal?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSAL_TOURISM_MODIFIER', 33);							-- How much extra pressure does the Master's religion exert on the Vassal?
-- DANGER! DO NOT SET TO ZERO!
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_PROTECT_VALUE_PER_OPINION_WEIGHT', 50);			-- YOU AREN'T COOL FOR DIVIDING BY ZERO. BE SMART.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_FAILED_PROTECT_VALUE_PER_OPINION_WEIGHT', 50);	-- ...YOU TRIED ANYWAY, DIDN'T YOU, YOU FOOL? WARNED YOU....
-- DANGER CLEARED! FEEL FREE TO SET THESE TO ZERO!
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_PROTECTED_CITY_DISTANCE', 6);				    -- ...okay, you can set this to zero. No really. No danger anymore! (How close to a vassal city must an enemy unit be for it to count for protect value.)
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_FAILED_PROTECT_CITY_DISTANCE', 0);				-- How close to a vassal city must a vassal unit be for it to count for failed protect value.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_PROTECTED_PER_TURN_DECAY', 25);					-- How much the protect score decays per turn. .25 a turn. CvDiplomacyAI::DoCounters()
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_FAILED_PROTECT_PER_TURN_DECAY', 25);			-- How much the failed protect score decays per turn. .25 a turn. CvDiplomacyAI::DoCounters()
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_LOST_CITIES_THRESHOLD', 75);				-- Percentage of cities that must be lost before Vassalage can be ended.
INSERT INTO	Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_POPULATION_THRESHOLD', 300);				-- Percentage of population over what we started with before Vassalage can be ended.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_MASTER_CITY_PERCENT_THRESHOLD', 60);		-- Percentage of cities that must be 
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_MASTER_POP_PERCENT_THRESHOLD', 60);		-- Percentage of cities that must be lost before Vassalage can be ended.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_CAPITULATE_BASE_THRESHOLD', 100);				-- How likely are we to capitulate?
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_TAX_PERCENT_MINIMUM', 0);				-- Minimum percent we can tax a vassal. Negative values may cause undefined behavior.
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_VASSAL_TAX_PERCENT_MAXIMUM', 25);				-- Maximum percent we can tax a vassal's income. Values greater than 100% will do nothing. Make sure this is a multiple of 5 please!

-- How much we view we are being treated
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_TREATMENT_THRESHOLD_DISAGREE', 10);
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_TREATMENT_THRESHOLD_MISTREATED', 30);
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_TREATMENT_THRESHOLD_UNHAPPY', 60);
INSERT INTO Defines ('Name', 'Value')	VALUES	('VASSALAGE_TREATMENT_THRESHOLD_ENSLAVED', 100);

INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_VOLUNTARY_VASSAL_MOD', 120);		-- Modifier to negative weights due to being voluntary vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_DEMANDED_WHILE_VASSAL', 20);				-- Opinion weight change each time you demand from a civ while their vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSAL_TAX_EXPONENT', 1.5);				-- Exponent to tax rate for determining negative opinion
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSAL_TAX_DIVISOR',  4);					-- Divisor (dont' set to zero!) for determining negative opinion for tax rate
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSAL_CURRENT_TAX_MODIFIER', 50);
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_WE_ARE_VOLUNTARY_VASSAL', -5);	-- Opinion Weight for AI voluntary vassal
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_WE_ARE_VASSAL', 0);				-- Opinion Weight for AI being your Vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_WE_ARE_MASTER', -20);			-- Opinion Weight for AI being your Master
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_THEY_PEACEFULLY_REVOKED', -20);	-- We peacefully revoked Vassalage
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_THEY_FORCIBLY_REVOKED', 50);		-- We forcefully revoked Vassalage
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_FAILED_PROTECT_MAX', 50);		-- Failed to protect a Vassal Maximum Opinion
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_PROTECT_MAX', -50);				-- Protected a Vassal Maximum Opinion
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_PEACEFULLY_REVOKED_NUM_TURNS_UNTIL_FORGOTTEN', 100);	-- How many turns until we forget you've peacefully revoked our vassalage?
INSERT INTO	Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_FORCIBLY_REVOKED_NUM_TURNS_UNTIL_FORGIVEN', 100);	-- How many turns until we forgive that you forcibly revoked our vassalage?
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_BROKEN_VASSAL_AGREEMENT_OPINION_WEIGHT', 40);
INSERT INTO Defines	('Name', 'Value')	VALUES	('OPINION_WEIGHT_VASSALAGE_TOO_MANY_VASSALS', 10);			-- Opinion weight change per vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('OPINION_WEIGHT_MASTER_LIBERATED_ME_FROM_VASSALAGE', -50);	-- Opinion weight for liberating a vassal
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_HOSTILE_WE_ARE_VASSAL', 1);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_GUARDED_WE_ARE_VASSAL', 1);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_DECEPTIVE_WE_ARE_VASSAL', 2);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_WAR_VASSAL_FORCEFULLY_REVOKED', 4);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_DECEPTIVE_VASSAL_FORCEFULLY_REVOKED', -10);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_FRIENDLY_VASSAL_FORCEFULLY_REVOKED', -10);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_WAR_VASSAL_PEACEFULLY_REVOKED', -4);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_DECEPTIVE_VASSAL_PEACEFULLY_REVOKED', 2);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_FRIENDLY_VASSAL_PEACEFULLY_REVOKED', 5);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_GUARDED_TOO_MANY_VASSALS', 10);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_WAR_TOO_MANY_VASSALS', 10);
INSERT INTO Defines ('Name', 'Value')	VALUES	('APPROACH_WAR_MY_VASSAL', 60);								-- How likely are we to go war with our vassal? (60 = 60%)

-- The following lines tell the AI how favorable a target is based on their vassal's strength/proximity
-- Additives
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_PATHETIC', 0);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_WEAK', 0);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_POOR', 5);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_AVERAGE', 20);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_STRONG', 35);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_POWERFUL', 50);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_IMMENSE', 100);

-- Multipliers
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_DISTANT', 100);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_FAR', 115);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_CLOSE', 150);
INSERT INTO Defines ('Name', 'Value')	VALUES	('TARGET_VASSAL_BACKUP_NEIGHBORS', 200);