--
-- This LoadedFile table is simply an aid to see at a glance which sql/xml file has errors.
-- Since the entire file is aborted upon the first mistake, seeing which file doesn't get to the end may
-- speed error checking.
--

CREATE TABLE IF NOT EXISTS
	LoadedFile(
	Type text,
	Value);

INSERT INTO LoadedFile(Type, Value) VALUES ('Buildings__Start.sql'			, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Buildings__End.sql'			, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('BarbarianChanges.sql'			, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Beliefs.sql'					, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('DifficultyMod.sql'				, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('MinorCivChanges.sql'			, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Buildings.xml'					, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Womders.xml'					, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Buildings_SpecialistsandGP.xml', 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Tech_Data.xml'					, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Tech_Start.sql'				, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Tech_End.sql'					, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Tech_Unlocks.sql'				, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Units__Start.sql'				, 0);
INSERT INTO LoadedFile(Type, Value) VALUES ('Units__End.sql'				, 0);


