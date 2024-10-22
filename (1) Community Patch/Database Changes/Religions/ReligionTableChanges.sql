-- This religion cannot be passively spread to foreign major civilization cities, occupied cities, or puppet cities
-- Kind of like a pantheon? See the Morindim mod
-- https://forums.civfanatics.com/threads/clans-of-the-morindim.525177/
-- Need the RELIGION_LOCAL_RELIGIONS option to function
ALTER TABLE Religions ADD LocalReligion integer DEFAULT 0;
